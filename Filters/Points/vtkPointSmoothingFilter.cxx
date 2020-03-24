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
#include "vtkArrayDispatch.h"
#include "vtkDataArrayRange.h"
#include "vtkSMPTools.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkStaticPointLocator.h"

vtkStandardNewMacro(vtkPointSmoothingFilter);

vtkCxxSetObjectMacro(vtkPointSmoothingFilter, FrameFieldArray, vtkDataArray);
vtkCxxSetObjectMacro(vtkPointSmoothingFilter, Locator, vtkAbstractPointLocator);

//----------------------------------------------------------------------------
namespace
{

  // Compute force depending on normalized radius. The force is linearly
  // repulsive near the point; has a slight (cubic) attractive force in the
  // region (1< r<=1.5); and produces no force further away.
  inline double ForceFunction(double r)
  {
    if ( r <= 1.0 ) //repulsive
    {
      return (r - 1.0);
    }
    else if ( r > 1.5 ) //far away do nothing
    {
      return 0.0;
    }
    else //attractive
    {
      return ((r-1.0)*(1.5-r)*(1.5-r)/0.25);
    }
  }


  // These classes compute the forced displacement between two points. The
  // classes vary on attribute data which affects the type of forces beween
  // particles.
  struct DisplacePoint
  {
    vtkDataArray *Data;
    double Radius; //radius of average sphere

    DisplacePoint(vtkDataArray *data, double ave) :
      Data(data), Radius(ave) {}
    virtual void operator()(vtkIdType p0, vtkIdType p1, double x[3],
                            double y[3], double disp[3]) = 0;
  };
  // Nearby points apply forces (not modified by distance nor attribute data)
  struct LaplacianDisplacement : public DisplacePoint
  {
    LaplacianDisplacement(vtkDataArray *data, double ave) :
      DisplacePoint(data,ave) {}
    void operator()(vtkIdType p0, vtkIdType p1, double x[3],
                    double y[3], double disp[3]) override
    {
      disp[0] = y[0] - x[0];
      disp[1] = y[1] - x[1];
      disp[2] = y[2] - x[2];
    }
  };
  // Forces on nearby points are moderated by distance apart
  struct UniformDisplacement : public DisplacePoint
  {
    UniformDisplacement(vtkDataArray *data, double ave) :
      DisplacePoint(data,ave) {}
    void operator()(vtkIdType p0, vtkIdType p1, double x[3],
                    double y[3], double disp[3]) override
    {
      double len = sqrt ( vtkMath::Distance2BetweenPoints(x,y) );
      double f = ForceFunction((len/2.0)*this->Radius);
      disp[0] = f * (y[0] - x[0]);
      disp[1] = f * (y[1] - x[1]);
      disp[2] = f * (y[2] - x[2]);
    }
  };
  // Forces on nearby points are moderated by distance and scalar values
  struct ScalarDisplacement : public DisplacePoint
  {
    double Range[2];
    double ScalarAverage;

    ScalarDisplacement(vtkDataArray *data, double ave, double range[2]) :
      DisplacePoint(data,ave)
    {
      this->Range[0] = range[0];
      this->Range[1] = range[1];
      this->ScalarAverage = (this->Range[0] + this->Range[1]) / 2.0;
    }
    void operator()(vtkIdType p0, vtkIdType p1, double x[3],
                    double y[3], double disp[3]) override
    {
      double len = sqrt ( vtkMath::Distance2BetweenPoints(x,y) );
      // The length is modified by the point spheres scaled by scalars. The point
      // spheres are assumed to be of radius Radius.
      double s0, s1, sf0, sf1;
      this->Data->GetTuple(p0,&s0);
      this->Data->GetTuple(p1,&s1);
      sf0 = (s0 - this->Range[0]) / (this->ScalarAverage - this->Range[0]);
      sf1 = (s1 - this->Range[0]) / (this->ScalarAverage - this->Range[0]);

      // Now compute the displacement
      double f = ForceFunction((len/2.0)*this->Radius);
      disp[0] = f * (y[0] - x[0]);
      disp[1] = f * (y[1] - x[1]);
      disp[2] = f * (y[2] - x[2]);
    }
  };
  // Forces on nearby points are moderated by distance and tensor values
  struct TensorDisplacement : public DisplacePoint
  {
    TensorDisplacement(vtkDataArray *data, double ave) :
      DisplacePoint(data,ave) {}
    void operator()(vtkIdType p0, vtkIdType p1, double x[3],
                    double y[3], double disp[3]) override
    {
      disp[0] = y[0] - x[0];
      disp[1] = y[1] - x[1];
      disp[2] = y[2] - x[2];
    }
  };
  // Forces on nearby points are moderated by distance and tensor eigenvalues
  struct FrameFieldDisplacement : public DisplacePoint
  {
    FrameFieldDisplacement(vtkDataArray *data, double ave) :
      DisplacePoint(data,ave) {}
    void operator()(vtkIdType p0, vtkIdType p1, double x[3],
                    double y[3], double disp[3]) override
    {
      disp[0] = y[0] - x[0];
      disp[1] = y[1] - x[1];
      disp[2] = y[2] - x[2];
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
      vtkIdType numNeis, *cptr = this->Conn + (ptId * this->NeiSize);
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
            *cptr++ = *nptr;
            ++numInserted;
          }
          ++nptr;
        }
        // In rare cases not all neighbors may be found, mark with a (-1)
        for ( ; numInserted < this->NeiSize; ++numInserted )
        {
          *cptr++ = (-1);
        }
        ++ptId; //move to the next point
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

  // Smoothing operation based on double buffering (simplifies threading). In
  // general the types of points (input and output buffers) can be different.
  template <typename PointsT>
  struct CharacterizeMesh
  {
    PointsT *Points;
    int NeiSize;
    const vtkIdType *Conn;
    double MinLength;
    double MaxLength;
    double AverageLength;
    vtkSMPThreadLocal<double> LocalMin;
    vtkSMPThreadLocal<double> LocalMax;
    vtkSMPThreadLocal<vtkIdType> LocalNEdges;
    vtkSMPThreadLocal<double> LocalAve;

    CharacterizeMesh(PointsT *inPts, int neiSize, const vtkIdType *conn) :
      Points(inPts), NeiSize(neiSize), Conn(conn),
      MinLength(0.0), MaxLength(0.0), AverageLength(0.0)
    {}

    void Initialize()
    {
      this->LocalMin.Local() = VTK_DOUBLE_MAX;
      this->LocalMax.Local() = VTK_DOUBLE_MIN;
      this->LocalNEdges.Local() = 0;
      this->LocalAve.Local() = 0.0;
    }

    // Determine the minimum and maximum edge lengths
    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      const vtkIdType *cptr = this->Conn + (this->NeiSize * ptId);
      const auto inPts = vtk::DataArrayTupleRange<3>(this->Points);
      double &min = this->LocalMin.Local();
      double &max = this->LocalMax.Local();
      vtkIdType &nEdges = this->LocalNEdges.Local();
      double &ave = this->LocalAve.Local();
      double x[3], y[3], len;
      vtkIdType neiId;

      for ( ; ptId < endPtId; ++ptId )
      {
        x[0] = inPts[ptId][0];
        x[1] = inPts[ptId][1];
        x[2] = inPts[ptId][2];

        for (auto i=0; i<this->NeiSize; i++)
        {
          neiId = *cptr++;
          // Process valid connections, and to reduce work only edges where
          // the neighbor id > pt id.
          if ( neiId >= 0 && neiId > ptId)
          {
            y[0] = inPts[neiId][0];
            y[1] = inPts[neiId][1];
            y[2] = inPts[neiId][2];

            len = sqrt( vtkMath::Distance2BetweenPoints(x,y) );
            min = std::min(len, min);
            max = std::max(len, max);
            ++nEdges;
            ave += len;
          }
        }
      }//for all points in this batch
    }//operator()

    // Composite the data
    void Reduce()
    {
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
    void operator()(PointsT* inPts, vtkIdType numPts, int neiSize, vtkIdType *conn)
    {
      CharacterizeMesh<PointsT> characterize(inPts, neiSize, conn);
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

    SmoothPoints(PointsT1 *inPts, PointsT2 *outPts, int neiSize, double relaxF,
                 const vtkIdType *conn, DisplacePoint *f) :
      InPoints(inPts), OutPoints(outPts), NeiSize(neiSize), RelaxationFactor(relaxF),
      Conn(conn), Displace(f)
    {}

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      const vtkIdType *cptr = this->Conn + (this->NeiSize * ptId);
      const auto inPts = vtk::DataArrayTupleRange<3>(this->InPoints);
      auto outPts = vtk::DataArrayTupleRange<3>(this->OutPoints);
      double x[3], y[3], sumDisp[3], disp[3], relaxF=this->RelaxationFactor;
      int npts;

      for ( ; ptId < endPtId; ++ptId )
      {
        npts = 0;
        sumDisp[0] = sumDisp[1] = sumDisp[2] = 0.0;
        x[0] = inPts[ptId][0];
        x[1] = inPts[ptId][1];
        x[2] = inPts[ptId][2];
        for (auto i=0; i<this->NeiSize; ++i, ++cptr)
        {
          // average the disps
          if ( *cptr >= 0 ) //valid connection to another point
          {
            ++npts;
            y[0] = inPts[*cptr][0];
            y[1] = inPts[*cptr][1];
            y[2] = inPts[*cptr][2];
            (*this->Displace)(ptId,*cptr,x,y,disp);
            sumDisp[0] += disp[0];
            sumDisp[1] += disp[1];
            sumDisp[2] += disp[2];
          }
        }
        if ( npts <= 0 ) // no contributions just copy point to output
        {
          outPts[ptId][0] = x[0];
          outPts[ptId][1] = x[1];
          outPts[ptId][2] = x[2];
        }
        else
        {
          sumDisp[0] /= static_cast<double>(npts);
          sumDisp[1] /= static_cast<double>(npts);
          sumDisp[2] /= static_cast<double>(npts);
          outPts[ptId][0] = inPts[ptId][0] + (relaxF * sumDisp[0]);
          outPts[ptId][1] = inPts[ptId][1] + (relaxF * sumDisp[1]);
          outPts[ptId][2] = inPts[ptId][2] + (relaxF * sumDisp[2]);
        }
      }//for all points in this batch
    }//operator()

  };//SmoothPoints

  // Hooks into dispatcher vtkArrayDispatch by providing a callable generic
  struct SmoothWorker
  {
    template <typename PointsT1, typename PointsT2>
    void operator()(PointsT1* inPts, PointsT2* outPts, vtkIdType numPts,
                    int neiSize, double relaxF, vtkIdType *conn, DisplacePoint *f)
    {
      SmoothPoints<PointsT1,PointsT2> smooth(inPts, outPts, neiSize, relaxF, conn, f);
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
  // points.
  using vtkArrayDispatch::Reals;
  using MeshDispatch = vtkArrayDispatch::DispatchByValueType<Reals>;
  MeshWorker meshWorker;
  if (!MeshDispatch::Execute(pts, meshWorker, numPts, neiSize, conn))
  { // Fallback to slowpath for other point types
    meshWorker(pts, numPts, neiSize, conn);
  }
  double minConnLen = meshWorker.MinLength; //the min and max "edge" lengths
  double maxConnLen = meshWorker.MaxLength;
  double radius = meshWorker.AverageLength/2.0;

  // Establish the type of inter-point forces/displacements
  DisplacePoint *disp;
  if ( smoothingMode == UNIFORM_SMOOTHING )
  {
    disp = new UniformDisplacement(nullptr,radius);
  }
  else if ( smoothingMode == SCALAR_SMOOTHING )
  {
    double range[2];
    inScalars->GetRange(range);
    disp = new ScalarDisplacement(inScalars,radius,range);
  }
  else if ( smoothingMode == TENSOR_SMOOTHING )
  {
    disp = new TensorDisplacement(inTensors,radius);
  }
  else if ( smoothingMode == FRAME_FIELD_SMOOTHING )
  {
    disp = new FrameFieldDisplacement(frameField,radius);
  }
  else //LAPLACIAN_SMOOTHING
  {
    disp = new LaplacianDisplacement(nullptr,radius);
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
                                 numPts, neiSize, relaxF, conn, disp))
    { // Fallback to slowpath for other point types
      sworker(inBuf->GetData(), outBuf->GetData(), numPts, neiSize, relaxF, conn, disp);
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
}
