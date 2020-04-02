/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointSmoothingFilter.cxx

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See LICENSE file for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPointSmoothingFilter.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkLogger.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkDataArray.h"
#include "vtkIdList.h"
#include "vtkPlane.h"
#include "vtkArrayDispatch.h"
#include "vtkDataArrayRange.h"
#include "vtkSMPTools.h"
#include "vtkSMPThreadLocal.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkStaticPointLocator.h"

vtkStandardNewMacro(vtkPointSmoothingFilter);

vtkCxxSetObjectMacro(vtkPointSmoothingFilter, FrameFieldArray, vtkDataArray);
vtkCxxSetObjectMacro(vtkPointSmoothingFilter, Locator, vtkAbstractPointLocator);

//----------------------------------------------------------------------------
namespace
{
  // Compute an inter-point force depending on normalized radius. The force
  // is linearly repulsive near the point; has a slight (cubic) attractive
  // force in the region (1<r<=gamma); and produces no force further away.
  inline double ForceFunction(double r, double gamma=1.0)
  {
    double gp1 = 1.0 + gamma;
    if ( r <= 1.0 ) //repulsive
    {
      return -(r - 1.0);
    }
    else if ( r > gp1 ) //far away do nothing
    {
      return 0.0;
    }
    else //attractive
    {
      return (-(r-1.0)*(gp1-r)*(gp1-r)/(gamma*gamma));
    }
  }

  // These classes compute the forced displacement of a point within a
  // neighborhood of points. Besides geometric proximity, attribute
  // data (e.g., scalars, tensors) may also affect the displacement.
  struct DisplacePoint
  {
    vtkDataArray *Data; //data attribute of interest
    double Radius; //radius of average sphere
    double RelaxationFactor; //controls effect of smoothing

    DisplacePoint(vtkDataArray *data, double radius, double rf) :
      Data(data), Radius(radius), RelaxationFactor(rf) {}

    // Return 1 if this edge pair(p0,p1) exerts a force on the point p0.
    // Otherwise 0. p0 is assumed to be the center point.
    virtual void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                            const vtkIdType *neis, const double *neiPts,
                            double disp[3]) = 0;
  };
  // Nearby points apply forces (not modified by distance nor attribute data)
  // This is a form of Laplacian smoothing. Attributes do not affect the
  // displacement. This has a tendency to collapse points to the center of
  // their local neighborhood.
  struct GeometricDisplacement : public DisplacePoint
  {
    GeometricDisplacement(vtkDataArray *data, double radius, double rf) :
      DisplacePoint(data,radius,rf) {}
    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      double count=0;
      double ave[3]={0.0,0.0,0.0};
      vtkIdType neiId;
      for (auto i=0; i<numNeis; ++i)
      {
        neiId = neis[i];
        if ( neiId >= 0 ) //valid connection to another point
        {
          ++count;
          ave[0] += neiPts[3*i];
          ave[1] += neiPts[3*i+1];
          ave[2] += neiPts[3*i+2];
        }
      }
      if ( count <= 0 )
      {
        disp[0] = 0.0;
        disp[1] = 0.0;
        disp[2] = 0.0;
      }
      else
      {
        // Displace towards the average of surrounding points
        ave[0] /= static_cast<double>(count);
        ave[1] /= static_cast<double>(count);
        ave[2] /= static_cast<double>(count);
        disp[0] = this->RelaxationFactor * (ave[0] - x[0]);
        disp[1] = this->RelaxationFactor * (ave[1] - x[1]);
        disp[2] = this->RelaxationFactor * (ave[2] - x[2]);
      }
    }
  };
  // Forces from nearby points are moderated by their distance. Attributes
  // do not affect the displacement.
  struct UniformDisplacement : public DisplacePoint
  {
    UniformDisplacement(vtkDataArray *data, double radius, double rf) :
      DisplacePoint(data,radius,rf) {}
    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      double len, force;
      double fVec[3];
      vtkIdType neiId;
      disp[0] = disp[1] = disp[2] = 0.0;
      for (auto i=0; i<numNeis; ++i)
      {
        neiId = neis[i];
        if ( neiId >= 0 ) //valid connection to another point
        {
          fVec[0] = neiPts[3*i] - x[0];
          fVec[1] = neiPts[3*i+1] - x[1];
          fVec[2] = neiPts[3*i+2] - x[2];
          if ( (len=vtkMath::Normalize(fVec)) == 0.0 )
          {
            fVec[0] = 1.0; //arbitrary bump
          }
          force = ForceFunction(len/this->Radius,0.5);
          disp[0] += force * this->RelaxationFactor * fVec[0];
          disp[1] += force * this->RelaxationFactor * fVec[1];
          disp[2] += force * this->RelaxationFactor * fVec[2];
        }
      }
    }
  };
  // Forces on nearby points are moderated by distance and scalar values.
  struct ScalarDisplacement : public DisplacePoint
  {
    double Range[2];
    double ScalarAverage;

    ScalarDisplacement(vtkDataArray *data, double radius, double rf, double range[2]) :
      DisplacePoint(data,radius,rf)
    {
      this->Range[0] = range[0];
      this->Range[1] = range[1];
      this->ScalarAverage = (this->Range[0] + this->Range[1]) / 2.0;
    }
    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      disp[0] = 0.0;
      disp[1] = 0.0;
      disp[2] = 0.0;
    }
  };
  // Forces on nearby points are moderated by distance and tensor values.
  struct TensorDisplacement : public DisplacePoint
  {
    TensorDisplacement(vtkDataArray *data, double radius, double rf) :
      DisplacePoint(data,radius,rf) {}
    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      disp[0] = 0.0;
      disp[1] = 0.0;
      disp[2] = 0.0;
    }
  };
  // Forces on nearby points are moderated by distance and tensor eigenvalues.
  struct FrameFieldDisplacement : public DisplacePoint
  {
    FrameFieldDisplacement(vtkDataArray *data, double radius, double rf) :
      DisplacePoint(data,radius,rf) {}
    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      disp[0] = 0.0;
      disp[1] = 0.0;
      disp[2] = 0.0;
    }
  };

  template <typename PointsT>
  struct BuildConnectivity
  {
    PointsT* Points;
    int NeiSize;
    vtkAbstractPointLocator *Locator;
    vtkIdType *Conn;
    vtkSMPThreadLocalObject<vtkIdList> LocalNeighbors;

    BuildConnectivity(PointsT* pts, int neiSize,
                     vtkAbstractPointLocator *loc, vtkIdType *conn) :
      Points(pts), NeiSize(neiSize), Locator(loc), Conn(conn)
    {}

    void Initialize()
    {
      this->LocalNeighbors.Local()->Allocate(this->NeiSize+1);
    }

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      const auto tuples = vtk::DataArrayTupleRange<3>(this->Points, ptId, endPtId);
      vtkIdList *neis = this->LocalNeighbors.Local();
      double x[3];
      vtkIdType i, numInserted, *nptr;
      vtkIdType numNeis, *neighbors = this->Conn + (ptId * this->NeiSize);
      for (const auto tuple : tuples)
      {
        x[0] = static_cast<double>(tuple[0]);
        x[1] = static_cast<double>(tuple[1]);
        x[2] = static_cast<double>(tuple[2]);

        // Exclude ourselves from list of neighbors and be paranoid about it (that
        // is don't insert too many points)
        this->Locator->FindClosestNPoints(this->NeiSize+1, x, neis);
        numNeis = neis->GetNumberOfIds();
        nptr = neis->GetPointer(0);
        for (numInserted=0, i=0; i < numNeis && numInserted < this->NeiSize; ++i)
        {
          if ( *nptr != ptId )
          {
            neighbors[numInserted++] = *nptr;
          }
          ++nptr;
        }
        // In rare cases not all neighbors may be found, mark with a (-1)
        for ( ; numInserted < this->NeiSize; ++numInserted )
        {
          neighbors[numInserted] = (-1);
        }
        ++ptId; //move to the next point
        neighbors += this->NeiSize;
      }
    }

    // An Initialize() method requires a Reduce() method
    void Reduce()
    {
    }

  };//BuildConnectivity

  // Hooks into dispatcher vtkArrayDispatch by providing a callable generic
  struct ConnectivityWorker
  {
    template <typename PointsT>
    void operator()(PointsT* pts, vtkIdType numPts, int neiSize,
                    vtkAbstractPointLocator *loc, vtkIdType *conn)
    {
      BuildConnectivity<PointsT> buildConn(pts, neiSize, loc, conn);
      vtkSMPTools::For(0, numPts, buildConn);
    }
  };

  // Centralize the dispatch to avoid duplication
  void UpdateConnectivity(vtkDataArray *pts, vtkIdType numPts, int neiSize,
                          vtkAbstractPointLocator *loc, vtkIdType *conn)
  {
    using vtkArrayDispatch::Reals;
    using ConnDispatch = vtkArrayDispatch::DispatchByValueType<Reals>;
    ConnectivityWorker connWorker;
    if (!ConnDispatch::Execute(pts, connWorker, numPts, neiSize, loc, conn))
    { // Fallback to slowpath for other point types
      connWorker(pts, numPts, neiSize, loc, conn);
    }
  }

  // Constrain point movement depending on classification. The point can move
  // freely, on the plane, or is fixed.
  struct PointConstraints
  {
    enum
    {
      UNCONSTRAINED=0,
      PLANE=1,
      CORNER=2
    };
    char *Classification;
    double *Normals;
    double FixedAngle;
    double BoundaryAngle;
    PointConstraints(vtkIdType numPts, double fa, double ba) :
      FixedAngle(fa), BoundaryAngle(ba)
    {
      this->Classification = new char[numPts];
      this->Normals = new double[numPts*3];
    }
    ~PointConstraints()
    {
      delete [] this->Classification;
      delete [] this->Normals;
    }
  };

  // Characterize the mesh, including classifying points as to whether they
  // are on boundaries or are fixed.
  template <typename PointsT>
  struct CharacterizeMesh
  {
    PointsT *Points;
    int NeiSize;
    const vtkIdType *Conn;
    PointConstraints *Constraints;
    double MinLength;
    double MaxLength;
    double AverageLength;

    double CornerAngle;
    double BoundaryAngle;

    vtkSMPThreadLocal<double*> LocalNeiPoints;
    vtkSMPThreadLocal<double*> LocalNeiNormals;
    vtkSMPThreadLocal<double> LocalMin;
    vtkSMPThreadLocal<double> LocalMax;
    vtkSMPThreadLocal<vtkIdType> LocalNEdges;
    vtkSMPThreadLocal<double> LocalAve;

    CharacterizeMesh(PointsT *inPts, int neiSize, const vtkIdType *conn, PointConstraints *c) :
      Points(inPts), NeiSize(neiSize), Conn(conn), Constraints(c),
      MinLength(0.0), MaxLength(0.0), AverageLength(0.0)
    {
      if ( this->Constraints )
      {
        this->CornerAngle = cos ( vtkMath::RadiansFromDegrees(this->Constraints->FixedAngle) );
        this->BoundaryAngle = cos ( vtkMath::RadiansFromDegrees(this->Constraints->BoundaryAngle) );
      }
    }

    void ClassifyPoint(vtkIdType ptId, double x[3], vtkIdType neiSize,
                       const vtkIdType *neis, const double *neiPts,
                       double *normals)
    {
      // Compute an average normal
      double *normal;
      double *aveN = this->Constraints->Normals + 3*ptId;
      aveN[0] = aveN[1] = aveN[2] = 0.0;
      char *classification = this->Constraints->Classification + ptId;
      for (auto i=0; i<neiSize; ++i)
      {
        if ( neis[i] >= 0 ) //check for valid neighbor
        {
          normal = normals + 3*i;
          normal[0] = neiPts[3*i] - x[0];
          normal[1] = neiPts[3*i+1] - x[1];
          normal[2] = neiPts[3*i+2] - x[2];
          vtkMath::Normalize(normal);
          aveN[0] += normal[0];
          aveN[1] += normal[1];
          aveN[2] += normal[2];
        }
      }
      double mag = vtkMath::Normalize(aveN);
      if ( mag == 0.0 )
      {
        *classification = PointConstraints::UNCONSTRAINED;
        return;
      }

      // Now determine angles away from average normal. This provides
      // a classification.
      double dot, minDot=1.0;
      for (auto i=0; i<neiSize; ++i)
      {
        if ( neis[i] >= 0 )
        {
          normal = normals + 3*i;
          dot = vtkMath::Dot(normal,aveN);
          minDot = (dot < minDot ? dot : minDot);
        }
      }
      if ( minDot >= this->CornerAngle )
      {
        *classification = PointConstraints::CORNER;
      }
      else if ( minDot >= this->BoundaryAngle )
      {
        *classification = PointConstraints::PLANE;
      }
      else
      {
        *classification = PointConstraints::UNCONSTRAINED;
      }
    }

    void Initialize()
    {
      this->LocalNeiPoints.Local() = new double [this->NeiSize*3];
      this->LocalNeiNormals.Local() = new double [this->NeiSize*3];
      this->LocalMin.Local() = VTK_DOUBLE_MAX;
      this->LocalMax.Local() = VTK_DOUBLE_MIN;
      this->LocalNEdges.Local() = 0;
      this->LocalAve.Local() = 0.0;
    }

    // Determine the minimum and maximum edge lengths
    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      const vtkIdType *neis = this->Conn + (this->NeiSize * ptId);
      const auto inPts = vtk::DataArrayTupleRange<3>(this->Points);
      double &min = this->LocalMin.Local();
      double &max = this->LocalMax.Local();
      vtkIdType &nEdges = this->LocalNEdges.Local();
      double &ave = this->LocalAve.Local();
      double *neiPts = this->LocalNeiPoints.Local();
      double *neiNormals = this->LocalNeiNormals.Local();
      double x[3], y[3], len;
      vtkIdType neiId;

      for ( ; ptId < endPtId; ++ptId, neis+=this->NeiSize )
      {
        x[0] = inPts[ptId][0];
        x[1] = inPts[ptId][1];
        x[2] = inPts[ptId][2];
        // Gather the coordinates of the points surrounding the point to smooth
        for (auto i=0; i<this->NeiSize; ++i)
        {
          neiId = neis[i];
          if ( neiId >= 0 ) //valid connection to another point
          {
            neiPts[3*i] = y[0] = inPts[neiId][0];
            neiPts[3*i+1] = y[1] = inPts[neiId][1];
            neiPts[3*i+2] = y[2] = inPts[neiId][2];

            // Process valid connections, and to reduce work only edges where
            // the neighbor id > pt id.
            if ( neiId >= 0 && neiId > ptId)
            {
              len = sqrt( vtkMath::Distance2BetweenPoints(x,y) );
              min = std::min(len, min);
              max = std::max(len, max);
              ++nEdges;
              ave += len;
            }
          }
        }
        // Classify point if requested
        if ( this->Constraints )
        {
          this->ClassifyPoint(ptId,x,this->NeiSize,neis,neiPts,neiNormals);
        }
      }//for all points in this batch
    }//operator()

    // Composite the data
    void Reduce()
    {
      // Don't need scratch storage anymore
      delete [] this->LocalNeiPoints.Local();
      delete [] this->LocalNeiNormals.Local();

      // Min / max edge lengths
      double min = VTK_DOUBLE_MAX;
      double max = VTK_DOUBLE_MIN;
      for (auto iter = this->LocalMin.begin(); iter != this->LocalMin.end(); ++iter)
      {
        min = std::min(*iter, min);
      }
      for (auto iter = this->LocalMax.begin(); iter != this->LocalMax.end(); ++iter)
      {
        max = std::max(*iter, max);
      }
      this->MinLength = min;
      this->MaxLength = max;

      // Average length
      vtkIdType numEdges = 0;
      double ave = 0.0;
      for (auto iter = this->LocalNEdges.begin(); iter != this->LocalNEdges.end(); ++iter)
      {
        numEdges += *iter;
      }
      for (auto iter = this->LocalAve.begin(); iter != this->LocalAve.end(); ++iter)
      {
        ave += *iter;
      }
      this->AverageLength = ave / static_cast<double>( numEdges );
    }
  };//CharacterizeMesh

  // Hooks into dispatcher vtkArrayDispatch by providing a callable generic
  struct MeshWorker
  {
    double MinLength;
    double MaxLength;
    double AverageLength;
    MeshWorker() : MinLength(0.0), MaxLength(0.0) {}

    template <typename PointsT>
    void operator()(PointsT* inPts, vtkIdType numPts, int neiSize, vtkIdType *conn,
                    PointConstraints *constraints)
    {
      CharacterizeMesh<PointsT> characterize(inPts, neiSize, conn, constraints);
      vtkSMPTools::For(0, numPts, characterize);
      this->MinLength = characterize.MinLength;
      this->MaxLength = characterize.MaxLength;
      this->AverageLength = characterize.AverageLength;
    }
  };//MeshWorker



  // Smoothing operation based on double buffering (simplifies threading). In
  // general the types of points (input and output buffers) can be different.
  template <typename PointsT1, typename PointsT2>
  struct SmoothPoints
  {
    PointsT1 *InPoints;
    PointsT2 *OutPoints;
    int NeiSize;
    double RelaxationFactor;
    const vtkIdType *Conn;
    DisplacePoint *Displace;
    PointConstraints *Constraints;
    vtkSMPThreadLocal<double*> LocalNeiPoints;

    SmoothPoints(PointsT1 *inPts, PointsT2 *outPts, int neiSize, double relaxF,
                 const vtkIdType *conn, DisplacePoint *f, PointConstraints *c) :
      InPoints(inPts), OutPoints(outPts), NeiSize(neiSize), RelaxationFactor(relaxF),
      Conn(conn), Displace(f), Constraints(c)
    {}

    void Initialize()
    {
      this->LocalNeiPoints.Local() = new double [this->NeiSize*3];
    }

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      const vtkIdType *neis = this->Conn + (this->NeiSize * ptId);
      const auto inPts = vtk::DataArrayTupleRange<3>(this->InPoints);
      auto outPts = vtk::DataArrayTupleRange<3>(this->OutPoints);
      double *neiPts = this->LocalNeiPoints.Local();
      vtkIdType neiId;
      double x[3], y[3], disp[3];

      for ( ; ptId < endPtId; ++ptId, neis+=this->NeiSize )
      {
        // Gather the coordinates of the points surrounding the point to smooth
        for (auto i=0; i<this->NeiSize; ++i)
        {
          neiId = neis[i];
          if ( neiId >= 0 ) //valid connection to another point
          {
            neiPts[3*i] = inPts[neiId][0];
            neiPts[3*i+1] = inPts[neiId][1];
            neiPts[3*i+2] = inPts[neiId][2];
          }
        }

        // Now compute a displacement for the current point
        x[0] = inPts[ptId][0];
        x[1] = inPts[ptId][1];
        x[2] = inPts[ptId][2];
        (*this->Displace)(ptId,x,this->NeiSize,neis,neiPts,disp);

        // It may be necessary to constrain the points's motion
        if ( this->Constraints )
        {
          // Check the classification of the point. May have to constrain it's motion.
          if ( this->Constraints->Classification[ptId] == PointConstraints::CORNER )
          { //do nothing, point will never move
            disp[0] = disp[1] = disp[2] = 0.0;
          }
          else
          {
            if ( this->Constraints->Classification[ptId] == PointConstraints::PLANE )
            { // constrain to plane
              double *normal = this->Constraints->Normals + 3*ptId;
              vtkPlane::ProjectVector(disp,x,normal,disp);
            }
          }
        }

        // Move the point
        outPts[ptId][0] = x[0] + disp[0];
        outPts[ptId][1] = x[1] + disp[1];
        outPts[ptId][2] = x[2] + disp[2];
      }//for all points in this batch
    }//operator()

    void Reduce()
    {
      delete [] this->LocalNeiPoints.Local();
    }

  };//SmoothPoints

  // Hooks into dispatcher vtkArrayDispatch by providing a callable generic
  struct SmoothWorker
  {
    template <typename PointsT1, typename PointsT2>
    void operator()(PointsT1* inPts, PointsT2* outPts, vtkIdType numPts,
                    int neiSize, double relaxF, vtkIdType *conn,
                    DisplacePoint *f, PointConstraints *c)
    {
      SmoothPoints<PointsT1,PointsT2> smooth(inPts, outPts, neiSize, relaxF,
                                             conn, f, c);
      vtkSMPTools::For(0, numPts, smooth);
    }
  };//SmoothWorker

} // anonymous namespace



//================= Begin class proper =======================================
//----------------------------------------------------------------------------
vtkPointSmoothingFilter::vtkPointSmoothingFilter()
{
  this->NeighborhoodSize = 8; // works well for 2D
  this->SmoothingMode = DEFAULT_SMOOTHING;
  this->Convergence = 0.0; // runs to number of specified iterations
  this->NumberOfIterations = 20;
  this->NumberOfSubIterations = 4;
  this->RelaxationFactor = 0.1;
  this->FrameFieldArray = nullptr;

  this->Locator = vtkStaticPointLocator::New();

  this->EnableConstraints = false;
  this->FixedAngle = 60.0;
  this->BoundaryAngle = 110.0;
}

//----------------------------------------------------------------------------
vtkPointSmoothingFilter::~vtkPointSmoothingFilter()
{
  this->SetFrameFieldArray(nullptr);
  this->SetLocator(nullptr);
}

//----------------------------------------------------------------------------
int vtkPointSmoothingFilter::
RequestData(vtkInformation* vtkNotUsed(request),
            vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output datasets
  vtkSmartPointer<vtkPointSet> input = vtkPointSet::GetData(inputVector[0]);
  vtkPointSet* output = vtkPointSet::GetData(outputVector);

  // Copy the input to the output as a starting point. We'll replace
  // the points and update point data later on.
  output->CopyStructure(input);
  output->GetCellData()->PassData(input->GetCellData());
  if ( this->NumberOfIterations <= 0 ) // Trivial case: 0 iterations
  {
    output->GetPointData()->PassData(input->GetPointData());
    return 1;
  }

  // Check the input
  vtkIdType numPts=input->GetNumberOfPoints();
  if ( numPts < 1 )
  {
    vtkLog(ERROR, "Points required");
    return 0;
  }
  if (!this->Locator)
  {
    vtkLog(ERROR, "Point locator required\n");
    return 0;
  }

  // Determine the smoothing mode
  vtkPointData *inPD=input->GetPointData(), *outPD=output->GetPointData();
  vtkDataArray *inScalars = inPD->GetScalars();
  vtkDataArray *inTensors = inPD->GetTensors();
  vtkDataArray *frameField = this->FrameFieldArray;
  int smoothingMode=UNIFORM_SMOOTHING;
  if ( this->SmoothingMode == DEFAULT_SMOOTHING )
  {
    smoothingMode = (frameField != nullptr ? FRAME_FIELD_SMOOTHING :
                     (inTensors != nullptr ? TENSOR_SMOOTHING :
                      (inScalars != nullptr ? SCALAR_SMOOTHING : UNIFORM_SMOOTHING)));
  }
  else if ( this->SmoothingMode == GEOMETRIC_SMOOTHING )
  {
    smoothingMode = GEOMETRIC_SMOOTHING;
  }
  else if ( this->SmoothingMode == SCALAR_SMOOTHING && inScalars != nullptr )
  {
    smoothingMode = SCALAR_SMOOTHING;
  }
  else if ( this->SmoothingMode == TENSOR_SMOOTHING && inTensors != nullptr )
  {
    smoothingMode = TENSOR_SMOOTHING;
  }
  else if ( this->SmoothingMode == FRAME_FIELD_SMOOTHING && frameField != nullptr )
  {
    smoothingMode = FRAME_FIELD_SMOOTHING;
  }
  vtkDebugMacro(<< "Smoothing glyphs: mode is: " << smoothingMode);

  // We'll build a locator for two purposes: 1) to build a point connectivity
  // list (connections to close points); and 2) interpolate data from neighbor
  // points.
  vtkDataArray *pts = input->GetPoints()->GetData();
  this->Locator->SetDataSet(input);
  this->Locator->BuildLocator();

  // The point neighborhood must be initially defined. Later on we'll update
  // it periodically.
  vtkIdType neiSize = (numPts < this->NeighborhoodSize ? numPts : this->NeighborhoodSize);
  vtkIdType *conn = new vtkIdType[numPts * neiSize];
  UpdateConnectivity(pts, numPts, neiSize, this->Locator, conn);

  // In order to perform smoothing properly we need to characterize the point
  // spacing and/or scalar, tensor, and or frame field data values. Later on
  // this enables the appropriate computation of the smoothing forces on the
  // points. Also classify the points as to on boundary or on edge etc.
  PointConstraints *constraints=nullptr;
  if ( this->EnableConstraints )
  {
    constraints = new PointConstraints(numPts,this->FixedAngle,this->BoundaryAngle);
  }
  using vtkArrayDispatch::Reals;
  using MeshDispatch = vtkArrayDispatch::DispatchByValueType<Reals>;
  MeshWorker meshWorker;
  if (!MeshDispatch::Execute(pts, meshWorker, numPts, neiSize, conn, constraints))
  { // Fallback to slowpath for other point types
    meshWorker(pts, numPts, neiSize, conn, constraints);
  }
  double minConnLen = meshWorker.MinLength; //the min and max "edge" lengths
  double maxConnLen = meshWorker.MaxLength;
  double radius = meshWorker.AverageLength/2.0;

  // Establish the type of inter-point forces/displacements
  DisplacePoint *disp;
  if ( smoothingMode == UNIFORM_SMOOTHING )
  {
    disp = new UniformDisplacement(nullptr,radius,this->RelaxationFactor);
  }
  else if ( smoothingMode == SCALAR_SMOOTHING )
  {
    double range[2];
    inScalars->GetRange(range);
    disp = new ScalarDisplacement(inScalars,radius,this->RelaxationFactor,range);
  }
  else if ( smoothingMode == TENSOR_SMOOTHING )
  {
    disp = new TensorDisplacement(inTensors,radius,this->RelaxationFactor);
  }
  else if ( smoothingMode == FRAME_FIELD_SMOOTHING )
  {
    disp = new FrameFieldDisplacement(frameField,radius,this->RelaxationFactor);
  }
  else //GEOMETRIC_SMOOTHING
  {
    disp = new GeometricDisplacement(nullptr,radius,this->RelaxationFactor);
  }

  // Prepare for smoothing. We double buffer the points. The output points
  // type is the same as the input points type.
  vtkPoints *pts0 = vtkPoints::New();
  pts0->SetDataType(pts->GetDataType());
  pts0->SetNumberOfPoints(numPts);
  pts0->DeepCopy(input->GetPoints());
  vtkPoints *pts1 = vtkPoints::New();
  pts1->SetDataType(pts->GetDataType());
  pts1->SetNumberOfPoints(numPts);
  vtkPoints *swapBuf, *inBuf=pts0, *outBuf=pts1;
  int numSubIters = (this->NumberOfSubIterations < this->NumberOfIterations ?
                     this->NumberOfSubIterations : this->NumberOfIterations);
  double relaxF = this->RelaxationFactor;

  // We need to incrementally compute a local neighborhood. This will be
  // performed every sub-iterations. This requires another point locator to
  // periodically rebuild the neighborhood connectivity. The initial point locator
  // is not modified we we can interpolate from the original points.
  vtkPolyData *tmpPolyData = vtkPolyData::New();
  tmpPolyData->SetPoints(inBuf);
  vtkAbstractPointLocator *tmpLocator = this->Locator->NewInstance();
  tmpLocator->SetDataSet(tmpPolyData);

  // Begin looping. We dispatch to various workers depending on points type.
  using SmoothDispatch = vtkArrayDispatch::Dispatch2ByValueType<Reals,Reals>;
  SmoothWorker sworker;
  bool converged = false;
  for ( int iterNum=0; iterNum < this->NumberOfIterations && !converged; ++iterNum )
  {
    // Perform a smoothing iteration using the current connectivity.
    if (!SmoothDispatch::Execute(inBuf->GetData(), outBuf->GetData(), sworker,
                                 numPts, neiSize, relaxF, conn, disp, constraints))
    { // Fallback to slowpath for other point types
      sworker(inBuf->GetData(), outBuf->GetData(), numPts, neiSize, relaxF,
              conn, disp, constraints);
    }

    // Build connectivity every sub-iterations.
    if ( ! (iterNum % numSubIters) )
    {
      // Build the point connectivity list as necessary. This is threaded and optimized over
      // Real types.
      tmpLocator->BuildLocator();
      UpdateConnectivity(pts, numPts, neiSize, tmpLocator, conn);
    }

    swapBuf = inBuf;
    inBuf = outBuf;
    outBuf = swapBuf;
    tmpLocator->Modified(); //ensure a rebuild the next time we build connectivity
  }//over all iterations

  // Set the output points
  output->SetPoints(outBuf);

  // Clean up
  delete constraints;
  delete [] conn;
  pts0->Delete();
  pts1->Delete();
  tmpPolyData->Delete();
  tmpLocator->Delete();

  // Copy point data
  outPD->PassData(inPD);

  return 1;
}

//----------------------------------------------------------------------------
int vtkPointSmoothingFilter::
FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPointSet");
  return 1;
}

//----------------------------------------------------------------------------
void vtkPointSmoothingFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Smoothing Mode: " << this->SmoothingMode << endl;
  os << indent << "Neighborhood Size: " << this->NeighborhoodSize << endl;
  os << indent << "Number of Iterations: " << this->NumberOfIterations << endl;
  os << indent << "Number of Sub-iterations: " << this->NumberOfSubIterations << endl;
  os << indent << "Relaxation Factor: " << this->RelaxationFactor << endl;
  os << indent << "Convergence: " << this->Convergence << endl;
  os << indent << "Frame Field Array: " << this->FrameFieldArray << "\n";
  os << indent << "Locator: " << this->Locator << "\n";

  os << indent << "Enable Constraints: " << (this->EnableConstraints ? "On\n" : "Off\n");
  os << indent << "Fixed Angle: " << this->FixedAngle << "\n";
  os << indent << "Boundary Angle: " << this->BoundaryAngle << "\n";

}
