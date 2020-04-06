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
#include "vtkCharArray.h"
#include "vtkDoubleArray.h"
#include "vtkArrayDispatch.h"
#include "vtkDataArrayRange.h"
#include "vtkSMPTools.h"
#include "vtkSMPThreadLocal.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkStaticPointLocator.h"
#include "vtkMinimalStandardRandomSequence.h"

vtkStandardNewMacro(vtkPointSmoothingFilter);

vtkCxxSetObjectMacro(vtkPointSmoothingFilter, FrameFieldArray, vtkDataArray);
vtkCxxSetObjectMacro(vtkPointSmoothingFilter, Locator, vtkAbstractPointLocator);
vtkCxxSetObjectMacro(vtkPointSmoothingFilter, Plane, vtkPlane);

//----------------------------------------------------------------------------
namespace
{
  //--------------------------------------------------------------------------
  // Machinery for extracting eigenfunctions. Needed if smoothing mode is set
  // to Tensors.
  template <typename DataT>
  struct ExtractEigenfunctions
  {
    DataT* InTensors;
    double *OutTensors; //9-component tensors with eigenfunctions extracted

    ExtractEigenfunctions(DataT* tIn, double *tOut) : InTensors(tIn), OutTensors(tOut)
    {}

    void Extract(double *tensor, double *eTensor)
    {
      double *m[3], w[3], *v[3];
      double m0[3], m1[3], m2[3];
      double v0[3], v1[3], v2[3];
      double xv[3], yv[3], zv[3];

      // set up working matrices
      m[0] = m0;
      m[1] = m1;
      m[2] = m2;
      v[0] = v0;
      v[1] = v1;
      v[2] = v2;

      // We are interested in the symmetrical part of the tensor only, since
      // eigenvalues are real if and only if the matrice of reals is symmetrical
      for (auto j=0; j < 3; j++)
      {
        for (auto i=0; i < 3; i++)
        {
          m[i][j] = 0.5 * (tensor[i + 3 * j] + tensor[j + 3 * i]);
        }
      }

      vtkMath::Jacobi(m, w, v);

      // copy non-normalized eigenvectors
      eTensor[0] = w[0] * v[0][0];
      eTensor[1] = w[0] * v[1][0];
      eTensor[2] = w[0] * v[2][0];
      eTensor[3] = w[1] * v[0][1];
      eTensor[4] = w[1] * v[1][1];
      eTensor[5] = w[1] * v[2][1];
      eTensor[6] = w[2] * v[0][2];
      eTensor[7] = w[2] * v[1][2];
      eTensor[8] = w[2] * v[2][2];
    }
  };

  template <typename DataT>
  struct Extract6Eigenfunctions : public ExtractEigenfunctions<DataT>
  {
    Extract6Eigenfunctions(DataT* tIn, double *tOut) :
      ExtractEigenfunctions<DataT>(tIn,tOut)
    {}

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      const auto tuples = vtk::DataArrayTupleRange<6>(this->InTensors, ptId, endPtId);
      double *t = this->OutTensors + 6*ptId;
      double tensor[9];

      for (const auto tuple : tuples)
      {
        for (auto i=0; i<6; ++i)
        {
          tensor[i] = tuple[i];
        }
        vtkMath::TensorFromSymmetricTensor(tensor);
        this->Extract(tensor,t);
        ++ptId; //move to the next point
        t += 6; //move to next output tensor
      }
    }
  };//Extract6Eigenfunctions - 6 components

  template <typename DataT>
  struct Extract9Eigenfunctions : public ExtractEigenfunctions<DataT>
  {
    Extract9Eigenfunctions(DataT* tIn, double *tOut) :
      ExtractEigenfunctions<DataT>(tIn,tOut)
    {}

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      const auto tuples = vtk::DataArrayTupleRange<9>(this->InTensors, ptId, endPtId);
      double *t = this->OutTensors + 9*ptId;
      double tensor[9];

      for (const auto tuple : tuples)
      {
        for (auto i=0; i<9; ++i)
        {
          tensor[i] = tuple[i];
        }
        this->Extract(tensor,t);
        ++ptId; //move to the next point
        t += 9; //move to next output tensor
      }
    }
  };//Extract9Eigenfunctions - 9 components

  // Hooks into dispatcher vtkArrayDispatch by providing a callable generic
  struct EigenWorker
  {
    vtkDoubleArray *Eigens;

    EigenWorker()
    {
      this->Eigens = vtkDoubleArray::New();
    }

    template <typename DataT>
    void operator()(DataT* tensor, vtkIdType numPts)
    {
      this->Eigens->SetNumberOfComponents(9);
      this->Eigens->SetNumberOfTuples(numPts);
      if ( tensor->GetNumberOfComponents() == 9 )
      {
        Extract9Eigenfunctions<DataT> extract9(tensor,this->Eigens->GetPointer(0));
        vtkSMPTools::For(0, numPts, extract9);
      }
      else
      {
        Extract6Eigenfunctions<DataT> extract6(tensor,this->Eigens->GetPointer(0));
        vtkSMPTools::For(0, numPts, extract6);
      }
    }
  };

  // Centralize the dispatch to avoid duplication
  vtkDataArray* ComputeEigenvalues(vtkDataArray *tensors, vtkIdType numPts)
  {
    using vtkArrayDispatch::Reals;
    using EigenDispatch = vtkArrayDispatch::DispatchByValueType<Reals>;
    EigenWorker eigenWorker;
    if (!EigenDispatch::Execute(tensors, eigenWorker, numPts))
    { // Fallback to slowpath for other point types
      eigenWorker(tensors, numPts);
    }
    return eigenWorker.Eigens;
  }

  //--------------------------------------------------------------------------
  // These classes compute the forced displacement of a point within a
  // neighborhood of points. Besides geometric proximity, attribute
  // data (e.g., scalars, tensors) may also affect the displacement.
  struct DisplacePoint
  {
    vtkDataArray *Data; //data attribute of interest
    double PackingRadius; //radius of average sphere
    double RelaxationFactor; //controls effect of smoothing
    double PackingFactor;
    double AttractionFactor;
    vtkNew<vtkMinimalStandardRandomSequence> RandomSeq;

    DisplacePoint(vtkDataArray *data, double radius, double rf,
                  double pf, double af) :
      Data(data), PackingRadius(radius), RelaxationFactor(rf),
      PackingFactor(pf), AttractionFactor(af)
    {
      this->RandomSeq->Initialize(1177);
    }

    // Generate a displacement for the given point from the
    // surrounding neighborhood.
    virtual void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                            const vtkIdType *neis, const double *neiPts,
                            double disp[3]) = 0;

    // Compute an inter-point force depending on normalized radius. The force
    // is linearly repulsive near the point 0<=r<=1; has a slight (cubic)
    // attractive force in the region (1<r<=(1+af); and produces no force
    // further away.
    inline double ParticleForce(double r, double af)
    {
      double af1 = 1.0 + af;
      if ( r <= 1.0 ) //repulsive, negative force
      {
        return (r - 1.0);
      }
      else if ( r > af1 ) //far away do nothing
      {
        return 0.0;
      }
      else //attractive, positive force
      {
        return ((r-1.0)*(af1-r)*(af1-r)/(af*af));
      }
    }

  };
  // Nearby points apply forces (not modified by distance nor attribute data)
  // This is a form of Laplacian smoothing. Attributes do not affect the
  // displacement. This has a tendency to collapse points to the center of
  // their local neighborhood.
  struct GeometricDisplacement : public DisplacePoint
  {
    GeometricDisplacement(vtkDataArray *data, double radius, double rf,
                          double pf, double af) :
      DisplacePoint(data,radius,rf,pf,af) {}

    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      int count=0;
      double ave[3]={0.0,0.0,0.0};
      double len, fVec[3];
      vtkIdType neiId;
      double R=this->PackingFactor*this->PackingRadius;
      for (auto i=0; i<numNeis; ++i)
      {
        neiId = neis[i];
        // Make sure to have a valid connection within sphere of influence
        if ( neiId >= 0 )
        {
          fVec[0] = neiPts[3*i] - x[0];
          fVec[1] = neiPts[3*i+1] - x[1];
          fVec[2] = neiPts[3*i+2] - x[2];
          if ( (len=vtkMath::Normalize(fVec)) <= R )
          {
            ++count;
            ave[0] += neiPts[3*i];
            ave[1] += neiPts[3*i+1];
            ave[2] += neiPts[3*i+2];
          }
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
    UniformDisplacement(vtkDataArray *data, double radius, double rf,
                          double pf, double af) :
      DisplacePoint(data,radius,rf,pf,af) {}

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
            {//points coincident, bump them apart
            fVec[0] = this->RandomSeq->GetValue(); this->RandomSeq->Next();
          }
          force = this->ParticleForce(len/(this->PackingFactor*this->PackingRadius),
                                      this->AttractionFactor);
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

    ScalarDisplacement(vtkDataArray *data, double radius, double rf,
                       double pf, double af, double range[2]) :
      DisplacePoint(data,radius,rf,pf,af)
    {
      this->Range[0] = range[0];
      this->Range[1] = range[1];
      this->ScalarAverage = (this->Range[0] + this->Range[1]) / 2.0;
    }

    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      double len, force, s, s0, s1, sf;
      double fVec[3];
      vtkIdType neiId;
      disp[0] = disp[1] = disp[2] = 0.0;
      this->Data->GetTuple(p0,&s0);
      for (auto i=0; i<numNeis; ++i)
      {
        neiId = neis[i];
        if ( neiId >= 0 ) //valid connection to another point
        {
          this->Data->GetTuple(neiId,&s1);
          //         s = 0.5*(s0+s1); //average
          s = (s1 > s0 ? s1 : s0);
          sf = (s - this->Range[0]) / (this->ScalarAverage - this->Range[0]);
          fVec[0] = neiPts[3*i] - x[0];
          fVec[1] = neiPts[3*i+1] - x[1];
          fVec[2] = neiPts[3*i+2] - x[2];
          if ( (len=vtkMath::Normalize(fVec)) == 0.0 )
            {//points coincident, bump them apart
            fVec[0] = this->RandomSeq->GetValue(); this->RandomSeq->Next();
          }
          force = this->ParticleForce(len/(this->PackingFactor*this->PackingRadius),
                                      this->AttractionFactor);
          disp[0] += (sf * force * this->RelaxationFactor * fVec[0]);
          disp[1] += (sf * force * this->RelaxationFactor * fVec[1]);
          disp[2] += (sf * force * this->RelaxationFactor * fVec[2]);
        }
      }
    }
  };
  // Forces on nearby points are moderated by distance and tensor values.
  struct TensorDisplacement : public DisplacePoint
  {
    TensorDisplacement(vtkDataArray *data, double radius, double rf,
                       double pf, double af) :
      DisplacePoint(data,radius,rf,pf,af) {}

    // Tensor represented by columnar eigenvectors. Project normalized
    // vector vec against the three eigenvectors and return length.
    double ComputeTensorLength(double vec[3], double tensor[9])
    {
      double dot = vtkMath::Dot(vec,tensor);
      double len = dot * dot;

      dot = vtkMath::Dot(vec,tensor+3);
      len += (dot * dot);

      dot = vtkMath::Dot(vec,tensor+6);
      len += (dot * dot);

      return sqrt(len);
    }

    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      double fVec[3], len;
      double tl0, tl1, tl, force, sf, t0[9], t1[9];
      vtkIdType neiId;
      disp[0] = disp[1] = disp[2] = 0.0;
      this->Data->GetTuple(p0,t0);
      for (auto i=0; i<numNeis; ++i)
      {
        neiId = neis[i];
        if ( neiId >= 0 ) //valid connection to another point
        {
          fVec[0] = neiPts[3*i] - x[0];
          fVec[1] = neiPts[3*i+1] - x[1];
          fVec[2] = neiPts[3*i+2] - x[2];
          if ( (len=vtkMath::Normalize(fVec)) == 0.0 )
            {//points coincident, bump them apart
            fVec[0] = this->RandomSeq->GetValue(); this->RandomSeq->Next();
          }
          this->Data->GetTuple(neiId,t1);
          tl0 = this->ComputeTensorLength(fVec,t0);
          tl1 = this->ComputeTensorLength(fVec,t1);
          tl = (tl1 > tl0 ? tl1 : tl0);
          sf = tl / this->PackingRadius;
          force = this->ParticleForce(len/(this->PackingFactor*this->PackingRadius),
                                      this->AttractionFactor);
          disp[0] += (sf * force * this->RelaxationFactor * fVec[0]);
          disp[1] += (sf * force * this->RelaxationFactor * fVec[1]);
          disp[2] += (sf * force * this->RelaxationFactor * fVec[2]);
        }
      }
    }
  };
  // Forces on nearby points are moderated by distance and tensor eigenvalues.
  struct FrameFieldDisplacement : public DisplacePoint
  {
    FrameFieldDisplacement(vtkDataArray *data, double radius, double rf,
                           double pf, double af) :
      DisplacePoint(data,radius,rf,pf,af) {}

    void operator()(vtkIdType p0, double x[3], vtkIdType numNeis,
                    const vtkIdType *neis, const double *neiPts,
                    double disp[3]) override
    {
      disp[0] = 0.0;
      disp[1] = 0.0;
      disp[2] = 0.0;
    }
  };

  //--------------------------------------------------------------------------
  // For each point, build the connectivity array to nearby points. The number
  // of neighbors is given by the specified neighborhood size.
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

  //--------------------------------------------------------------------------
  // Constrain point movement depending on classification. The point can move
  // freely, on a plane, or is fixed.
  struct PointConstraints
  {
    enum
    {
      UNCONSTRAINED=0,
      PLANE=1,
      CORNER=2
    };
    vtkNew<vtkCharArray> ClassificationArray;
    vtkNew<vtkDoubleArray> NormalsArray;
    char *Classification;
    double *Normals;
    double FixedAngle;
    double BoundaryAngle;
    PointConstraints(vtkIdType numPts, double fa, double ba) :
      FixedAngle(fa), BoundaryAngle(ba)
    {
      this->ClassificationArray->SetName("Constraint Scalars");
      this->ClassificationArray->SetNumberOfComponents(1);
      this->ClassificationArray->SetNumberOfTuples(numPts);
      this->Classification = this->ClassificationArray->GetPointer(0);
      this->NormalsArray->SetNumberOfComponents(3);
      this->NormalsArray->SetNumberOfTuples(numPts);
      this->Normals = this->NormalsArray->GetPointer(0);
    }
    vtkDataArray *GetClassificationArray() {return this->ClassificationArray.Get();}
    vtkDataArray *GetNormalsArray() {return this->NormalsArray.Get();}
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



  //--------------------------------------------------------------------------
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
    vtkPlane *Plane;
    double PlaneOrigin[3];
    double PlaneNormal[3];
    vtkSMPThreadLocal<double*> LocalNeiPoints;

    SmoothPoints(PointsT1 *inPts, PointsT2 *outPts, int neiSize, double relaxF,
                 const vtkIdType *conn, DisplacePoint *f, PointConstraints *c,
                 vtkPlane *plane) :
      InPoints(inPts), OutPoints(outPts), NeiSize(neiSize), RelaxationFactor(relaxF),
      Conn(conn), Displace(f), Constraints(c), Plane(plane)
    {
      if ( this->Plane )
      {
        this->Plane->GetOrigin(this->PlaneOrigin);
        this->Plane->GetNormal(this->PlaneNormal);
        vtkMath::Normalize(this->PlaneNormal);
      }
    }

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
            { // constrain to a point constraint plane
              double *normal = this->Constraints->Normals + 3*ptId;
              vtkPlane::ProjectVector(disp,x,normal,disp);
            }
          }
        }

        // Move the point
        x[0] += disp[0];
        x[1] += disp[1];
        x[2] += disp[2];

        // If point motion is constrained to a plane, project onto the plane
        if ( this->Plane )
        {
          vtkPlane::ProjectPoint(x,this->PlaneOrigin,this->PlaneNormal,x);
        }

        // Update the output points buffer
        outPts[ptId][0] = x[0];
        outPts[ptId][1] = x[1]; + disp[1];
        outPts[ptId][2] = x[2];

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
                    DisplacePoint *f, PointConstraints *c, vtkPlane *plane)
    {
      SmoothPoints<PointsT1,PointsT2> smooth(inPts, outPts, neiSize, relaxF,
                                             conn, f, c, plane);
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
  this->GenerateConstraintScalars = false;
  this->GenerateConstraintNormals = false;

  this->ComputePackingRadius = true;
  this->PackingRadius = 1.0;
  this->PackingFactor = 1.0;
  this->AttractionFactor = 0.5;

  this->MotionConstraint = UNCONSTRAINED_MOTION;
  this->Plane = nullptr;
}

//----------------------------------------------------------------------------
vtkPointSmoothingFilter::~vtkPointSmoothingFilter()
{
  this->SetFrameFieldArray(nullptr);
  this->SetLocator(nullptr);
  this->SetPlane(nullptr);
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
  // points. Also classify the points as to on boundary or on edge etc. This
  // calculation is only done if not manually overridden.
  using vtkArrayDispatch::Reals;
  double radius=this->PackingRadius;
  PointConstraints *constraints=nullptr;
  if ( this->EnableConstraints || this->ComputePackingRadius )
  {
    if ( this->EnableConstraints )
    {
      constraints = new PointConstraints(numPts,this->FixedAngle,this->BoundaryAngle);
    }
    using MeshDispatch = vtkArrayDispatch::DispatchByValueType<Reals>;
    MeshWorker meshWorker;
    if (!MeshDispatch::Execute(pts, meshWorker, numPts, neiSize, conn, constraints))
    { // Fallback to slowpath for other point types
      meshWorker(pts, numPts, neiSize, conn, constraints);
    }
    double minConnLen = meshWorker.MinLength; //the min and max "edge" lengths
    double maxConnLen = meshWorker.MaxLength;
    this->PackingRadius = radius = meshWorker.AverageLength/2.0;
  }

  // Establish the type of inter-point forces/displacements
  vtkSmartPointer<vtkDataArray> computedFrameField;
  DisplacePoint *disp;
  if ( smoothingMode == UNIFORM_SMOOTHING )
  {
    disp = new UniformDisplacement(nullptr,radius,this->RelaxationFactor,
                                   this->PackingFactor,this->AttractionFactor);
  }
  else if ( smoothingMode == SCALAR_SMOOTHING )
  {
    double range[2];
    inScalars->GetRange(range);
    disp = new ScalarDisplacement(inScalars,radius,this->RelaxationFactor,
                                  this->PackingFactor,this->AttractionFactor,range);
  }
  else if ( smoothingMode == TENSOR_SMOOTHING )
  {
    computedFrameField = ComputeEigenvalues(inTensors, numPts);
    disp = new FrameFieldDisplacement(computedFrameField.Get(),radius,this->RelaxationFactor,
                                      this->PackingFactor,this->AttractionFactor);
  }
  else if ( smoothingMode == FRAME_FIELD_SMOOTHING )
  {
    disp = new FrameFieldDisplacement(frameField,radius,this->RelaxationFactor,
                                      this->PackingFactor,this->AttractionFactor);
  }
  else //GEOMETRIC_SMOOTHING
  {
    disp = new GeometricDisplacement(nullptr,radius,this->RelaxationFactor,
                                     this->PackingFactor,this->AttractionFactor);
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
  vtkPlane *plane = ( this->MotionConstraint == PLANE_MOTION && this->Plane ?
                      this->Plane : nullptr );

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
                                 numPts, neiSize, relaxF, conn, disp, constraints,
                                 plane))
    { // Fallback to slowpath for other point types
      sworker(inBuf->GetData(), outBuf->GetData(), numPts, neiSize, relaxF,
              conn, disp, constraints, plane);
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

  // If constraint scalars are requested, produce them
  if ( constraints && this->GenerateConstraintScalars )
  {
    outPD->AddArray(constraints->GetClassificationArray());
  }

  // If constraint vectors are requested, produce them
  if ( constraints && this->GenerateConstraintNormals )
  {
    outPD->AddArray(constraints->GetNormalsArray());
  }

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
  os << indent << "Generate Constraint Scalars: "
     << (this->GenerateConstraintScalars ? "On\n" : "Off\n");
  os << indent << "Generate Constraint Normals: "
     << (this->GenerateConstraintNormals ? "On\n" : "Off\n");

  os << indent << "Compute Packing Radius: "
     << (this->ComputePackingRadius ? "On\n" : "Off\n");
  os << indent << "Packing Radius: " << this->PackingRadius << "\n";
  os << indent << "Packing Factor: " << this->PackingFactor << "\n";
  os << indent << "Attraction Factor: " << this->AttractionFactor << "\n";

  os << indent << "Motion Constraint: " << this->MotionConstraint << "\n";
  os << indent << "Plane: " << this->Plane << "\n";
}
