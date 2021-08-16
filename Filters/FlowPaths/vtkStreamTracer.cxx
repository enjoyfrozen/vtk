/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkStreamTracer.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkStreamTracer.h"

#include "vtkAMRInterpolatedVelocityField.h"
#include "vtkAbstractInterpolatedVelocityField.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkCellLocatorInterpolatedVelocityField.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkDoubleArray.h"
#include "vtkExecutive.h"
#include "vtkGenericCell.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkIntArray.h"
#include "vtkInterpolatedVelocityField.h"
#include "vtkMath.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkOverlappingAMR.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkPolyData.h"
#include "vtkPolyLine.h"
#include "vtkRungeKutta2.h"
#include "vtkRungeKutta4.h"
#include "vtkRungeKutta45.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkSMPTools.h"
#include "vtkSmartPointer.h"
#include "vtkStaticCellLocator.h"

#include <vector>

vtkObjectFactoryNewMacro(vtkStreamTracer);
vtkCxxSetObjectMacro(vtkStreamTracer, Integrator, vtkInitialValueProblemSolver);
vtkCxxSetObjectMacro(vtkStreamTracer, InterpolatorPrototype, vtkAbstractInterpolatedVelocityField);

const double vtkStreamTracer::EPSILON = 1.0E-12;

namespace
{
// special function to interpolate the point data from the input to the output
// if fast == true, then it just calls the usual InterpolatePoint function,
// otherwise,
// it makes sure the array exists in the input before trying to copy it to the
// output. if it doesn't exist in the input but is in the output then we
// remove it from the output instead of having bad values there.
// this is meant for multiblock data sets where the grids may not have the
// same point data arrays or have them in different orders.
void InterpolatePoint(vtkDataSetAttributes* outPointData, vtkDataSetAttributes* inPointData,
  vtkIdType toId, vtkIdList* ids, double* weights, bool fast)
{
  if (fast)
  {
    outPointData->InterpolatePoint(inPointData, toId, ids, weights);
  }
  else
  {
    for (int i = outPointData->GetNumberOfArrays() - 1; i >= 0; i--)
    {
      vtkAbstractArray* toArray = outPointData->GetAbstractArray(i);
      if (vtkAbstractArray* fromArray = inPointData->GetAbstractArray(toArray->GetName()))
      {
        toArray->InterpolateTuple(toId, ids, fromArray, weights);
      }
      else
      {
        outPointData->RemoveArray(toArray->GetName());
      }
    }
  }
}

}

//------------------------------------------------------------------------------
vtkStreamTracer::vtkStreamTracer()
{
  this->Integrator = vtkRungeKutta2::New();
  this->IntegrationDirection = FORWARD;
  for (int i = 0; i < 3; i++)
  {
    this->StartPosition[i] = 0.0;
  }

  this->MaximumPropagation = 1.0;
  this->IntegrationStepUnit = CELL_LENGTH_UNIT;
  this->InitialIntegrationStep = 0.5;
  this->MinimumIntegrationStep = 1.0E-2;
  this->MaximumIntegrationStep = 1.0;

  this->MaximumError = 1.0e-6;
  this->MaximumNumberOfSteps = 2000;
  this->TerminalSpeed = EPSILON;

  this->ComputeVorticity = true;
  this->RotationScale = 1.0;

  this->LastUsedStepSize = 0.0;

  this->GenerateNormalsInIntegrate = true;

  this->InterpolatorPrototype = nullptr;

  this->SetNumberOfInputPorts(2);

  // by default process active point vectors
  this->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::VECTORS);

  this->HasMatchingPointAttributes = true;

  this->SurfaceStreamlines = false;
}

//------------------------------------------------------------------------------
vtkStreamTracer::~vtkStreamTracer()
{
  this->SetIntegrator(nullptr);
  this->SetInterpolatorPrototype(nullptr);
}

//------------------------------------------------------------------------------
void vtkStreamTracer::SetSourceConnection(vtkAlgorithmOutput* algOutput)
{
  this->SetInputConnection(1, algOutput);
}

//------------------------------------------------------------------------------
void vtkStreamTracer::SetSourceData(vtkDataSet* source)
{
  this->SetInputData(1, source);
}

//------------------------------------------------------------------------------
vtkDataSet* vtkStreamTracer::GetSource()
{
  if (this->GetNumberOfInputConnections(1) < 1)
  {
    return nullptr;
  }
  return vtkDataSet::SafeDownCast(this->GetExecutive()->GetInputData(1, 0));
}

//------------------------------------------------------------------------------
int vtkStreamTracer::GetIntegratorType()
{
  if (!this->Integrator)
  {
    return NONE;
  }
  if (!strcmp(this->Integrator->GetClassName(), "vtkRungeKutta2"))
  {
    return RUNGE_KUTTA2;
  }
  if (!strcmp(this->Integrator->GetClassName(), "vtkRungeKutta4"))
  {
    return RUNGE_KUTTA4;
  }
  if (!strcmp(this->Integrator->GetClassName(), "vtkRungeKutta45"))
  {
    return RUNGE_KUTTA45;
  }
  return UNKNOWN;
}

//------------------------------------------------------------------------------
void vtkStreamTracer::SetInterpolatorTypeToDataSetPointLocator()
{
  this->SetInterpolatorType(static_cast<int>(INTERPOLATOR_WITH_DATASET_POINT_LOCATOR));
}

//------------------------------------------------------------------------------
void vtkStreamTracer::SetInterpolatorTypeToCellLocator()
{
  this->SetInterpolatorType(static_cast<int>(INTERPOLATOR_WITH_CELL_LOCATOR));
}

//------------------------------------------------------------------------------
void vtkStreamTracer::SetInterpolatorType(int interpType)
{
  if (interpType == INTERPOLATOR_WITH_CELL_LOCATOR)
  {
    // create an interpolator equipped with a cell locator
    vtkNew<vtkCellLocatorInterpolatedVelocityField> cellLoc;

    // specify the type of the cell locator attached to the interpolator
    constexpr double tolerance = 1e-6;
    vtkNew<vtkStaticCellLocator> cellLocType;
    cellLocType->SetTolerance(tolerance);
    cellLocType->UseDiagonalLengthToleranceOn();
    cellLoc->SetCellLocatorPrototype(cellLocType);

    this->SetInterpolatorPrototype(cellLoc);
  }
  else
  {
    // create an interpolator equipped with a point locator (by default)
    vtkSmartPointer<vtkInterpolatedVelocityField> pntLoc =
      vtkSmartPointer<vtkInterpolatedVelocityField>::New();
    this->SetInterpolatorPrototype(pntLoc);
  }
}

//------------------------------------------------------------------------------
void vtkStreamTracer::SetIntegratorType(int type)
{
  vtkInitialValueProblemSolver* ivp = nullptr;
  switch (type)
  {
    case RUNGE_KUTTA2:
      ivp = vtkRungeKutta2::New();
      break;
    case RUNGE_KUTTA4:
      ivp = vtkRungeKutta4::New();
      break;
    case RUNGE_KUTTA45:
      ivp = vtkRungeKutta45::New();
      break;
    default:
      vtkWarningMacro("Unrecognized integrator type. Keeping old one.");
      break;
  }
  if (ivp)
  {
    this->SetIntegrator(ivp);
    ivp->Delete();
  }
}

//------------------------------------------------------------------------------
void vtkStreamTracer::SetIntegrationStepUnit(int unit)
{
  if (unit != LENGTH_UNIT && unit != CELL_LENGTH_UNIT)
  {
    unit = CELL_LENGTH_UNIT;
  }

  if (unit == this->IntegrationStepUnit)
  {
    return;
  }

  this->IntegrationStepUnit = unit;
  this->Modified();
}

//------------------------------------------------------------------------------
double vtkIntervalInformation::
ConvertToLength(double interval, int unit, double cellLength)
{
  double retVal = 0.0;
  if (unit == vtkStreamTracer::LENGTH_UNIT)
  {
    retVal = interval;
  }
  else if (unit == vtkStreamTracer::CELL_LENGTH_UNIT)
  {
    retVal = interval * cellLength;
  }
  return retVal;
}

//------------------------------------------------------------------------------
double vtkIntervalInformation::
ConvertToLength(vtkIntervalInformation& interval, double cellLength)
{
  return ConvertToLength(interval.Interval, interval.Unit, cellLength);
}

//------------------------------------------------------------------------------
void vtkStreamTracer::ConvertIntervals(
  double& step, double& minStep, double& maxStep, int direction, double cellLength)
{
  minStep = maxStep = step = direction *
    vtkIntervalInformation::ConvertToLength(this->InitialIntegrationStep, this->IntegrationStepUnit, cellLength);

  if (this->MinimumIntegrationStep > 0.0)
  {
    minStep =
      vtkIntervalInformation::ConvertToLength(this->MinimumIntegrationStep, this->IntegrationStepUnit, cellLength);
  }

  if (this->MaximumIntegrationStep > 0.0)
  {
    maxStep =
      vtkIntervalInformation::ConvertToLength(this->MaximumIntegrationStep, this->IntegrationStepUnit, cellLength);
  }
}

//------------------------------------------------------------------------------
void vtkStreamTracer::CalculateVorticity(
  vtkGenericCell* cell, double pcoords[3], vtkDoubleArray* cellVectors, double vorticity[3])
{
  double* cellVel;
  double derivs[9];

  cellVel = cellVectors->GetPointer(0);
  cell->Derivatives(0, pcoords, cellVel, 3, derivs);
  vorticity[0] = derivs[7] - derivs[5];
  vorticity[1] = derivs[2] - derivs[6];
  vorticity[2] = derivs[3] - derivs[1];
}

//------------------------------------------------------------------------------
void vtkStreamTracer::InitializeSeeds(vtkDataArray*& seeds, vtkIdList*& seedIds,
  vtkIntArray*& integrationDirections, vtkDataSet* source)
{
  seedIds = vtkIdList::New();
  integrationDirections = vtkIntArray::New();
  seeds = nullptr;

  if (source)
  {
    vtkIdType numSeeds = source->GetNumberOfPoints();
    if (numSeeds > 0)
    {
      // For now, one thread will do all

      if (this->IntegrationDirection == BOTH)
      {
        seedIds->SetNumberOfIds(2 * numSeeds);
        for (vtkIdType i = 0; i < numSeeds; ++i)
        {
          seedIds->SetId(i, i);
          seedIds->SetId(numSeeds + i, i);
        }
      }
      else
      {
        seedIds->SetNumberOfIds(numSeeds);
        for (vtkIdType i = 0; i < numSeeds; ++i)
        {
          seedIds->SetId(i, i);
        }
      }
      // Check if the source is a PointSet
      vtkPointSet* seedPts = vtkPointSet::SafeDownCast(source);
      if (seedPts)
      {
        // If it is, use it's points as source
        vtkDataArray* orgSeeds = seedPts->GetPoints()->GetData();
        seeds = orgSeeds->NewInstance();
        seeds->DeepCopy(orgSeeds);
      }
      else
      {
        // Else, create a seed source
        seeds = vtkDoubleArray::New();
        seeds->SetNumberOfComponents(3);
        seeds->SetNumberOfTuples(numSeeds);
        for (vtkIdType i = 0; i < numSeeds; ++i)
        {
          seeds->SetTuple(i, source->GetPoint(i));
        }
      }
    }
  } // if a source is available

  else // use the start position
  {
    seeds = vtkDoubleArray::New();
    seeds->SetNumberOfComponents(3);
    seeds->InsertNextTuple(this->StartPosition);
    seedIds->InsertNextId(0);
    if (this->IntegrationDirection == BOTH)
    {
      seedIds->InsertNextId(0);
    }
  }

  if (seeds)
  {
    vtkIdType i;
    vtkIdType numSeeds = seeds->GetNumberOfTuples();
    if (this->IntegrationDirection == BOTH)
    {
      for (i = 0; i < numSeeds; i++)
      {
        integrationDirections->InsertNextValue(FORWARD);
      }
      for (i = 0; i < numSeeds; i++)
      {
        integrationDirections->InsertNextValue(BACKWARD);
      }
    }
    else
    {
      for (i = 0; i < numSeeds; i++)
      {
        integrationDirections->InsertNextValue(this->IntegrationDirection);
      }
    }
  }//if seeds are available
}

//------------------------------------------------------------------------------
int vtkStreamTracer::SetupOutput(vtkInformation* inInfo, vtkInformation* outInfo)
{
  int piece = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
  int numPieces = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_NUMBER_OF_PIECES());

  vtkDataObject* input = inInfo->Get(vtkDataObject::DATA_OBJECT());
  vtkDataObject* output = outInfo->Get(vtkDataObject::DATA_OBJECT());

  // Pass through field data
  output->GetFieldData()->PassData(input->GetFieldData());

  vtkCompositeDataSet* hdInput = vtkCompositeDataSet::SafeDownCast(input);
  vtkDataSet* dsInput = vtkDataSet::SafeDownCast(input);
  if (hdInput)
  {
    this->InputData = hdInput;
    hdInput->Register(this);
    return 1;
  }
  else if (dsInput)
  {
    vtkNew<vtkMultiBlockDataSet> mb;
    mb->SetNumberOfBlocks(numPieces);
    mb->SetBlock(piece, dsInput);
    this->InputData = mb;
    mb->Register(this);
    return 1;
  }
  else
  {
    vtkErrorMacro(
      "This filter cannot handle input of type: " << (input ? input->GetClassName() : "(none)"));
    return 0;
  }
}

//------------------------------------------------------------------------------
int vtkStreamTracer::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  if (!this->SetupOutput(inInfo, outInfo))
  {
    return 0;
  }

  vtkInformation* sourceInfo = inputVector[1]->GetInformationObject(0);
  vtkDataSet* source = nullptr;
  if (sourceInfo)
  {
    source = vtkDataSet::SafeDownCast(sourceInfo->Get(vtkDataObject::DATA_OBJECT()));
  }
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDataArray* seeds = nullptr;
  vtkIdList* seedIds = nullptr;
  vtkIntArray* integrationDirections = nullptr;
  this->InitializeSeeds(seeds, seedIds, integrationDirections, source);

  if (seeds)
  {
    vtkAbstractInterpolatedVelocityField* func = nullptr;
    int maxCellSize = 0;
    if (this->CheckInputs(func, &maxCellSize) != VTK_OK)
    {
      vtkDebugMacro("No appropriate inputs have been found. Can not execute.");
      if (func)
      {
        func->Delete();
      }
      seeds->Delete();
      integrationDirections->Delete();
      seedIds->Delete();
      this->InputData->UnRegister(this);
      return 1;
    }

    if (vtkOverlappingAMR::SafeDownCast(this->InputData))
    {
      vtkOverlappingAMR* amr = vtkOverlappingAMR::SafeDownCast(this->InputData);
      amr->GenerateParentChildInformation();
    }

    vtkCompositeDataIterator* iter = this->InputData->NewIterator();
    vtkSmartPointer<vtkCompositeDataIterator> iterP(iter);
    iter->Delete();

    iterP->GoToFirstItem();
    vtkDataSet* input0 = nullptr;
    if (!iterP->IsDoneWithTraversal() && !input0)
    {
      input0 = vtkDataSet::SafeDownCast(iterP->GetCurrentDataObject());
      iterP->GoToNextItem();
    }
    int vecType(0);
    vtkDataArray* vectors = this->GetInputArrayToProcess(0, input0, vecType);
    if (vectors)
    {
      const char* vecName = vectors->GetName();
      double propagation = 0;
      vtkIdType numSteps = 0;
      double integrationTime = 0;
      this->Integrate(input0->GetPointData(), output, seeds, seedIds, integrationDirections,
                      func, maxCellSize, vecType, vecName, propagation, numSteps, integrationTime,
                      this->CustomTerminationCallback, this->CustomTerminationClientData,
                      this->CustomReasonForTermination);
    }//if vectors are available
    func->Delete();
    seeds->Delete();
  } //if seeds are provided

  integrationDirections->Delete();
  seedIds->Delete();

  this->InputData->UnRegister(this);
  return 1;
}

//------------------------------------------------------------------------------
int vtkStreamTracer::CheckInputs(vtkAbstractInterpolatedVelocityField*& func, int* maxCellSize)
{
  if (!this->InputData)
  {
    return VTK_ERROR;
  }

  vtkOverlappingAMR* amrData = vtkOverlappingAMR::SafeDownCast(this->InputData);

  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(this->InputData->NewIterator());

  vtkDataSet* input0 = nullptr;
  iter->GoToFirstItem();
  while (!iter->IsDoneWithTraversal() && input0 == nullptr)
  {
    input0 = vtkDataSet::SafeDownCast(iter->GetCurrentDataObject());
    iter->GoToNextItem();
  }
  if (!input0)
  {
    return VTK_ERROR;
  }

  int vecType(0);
  vtkDataArray* vectors = this->GetInputArrayToProcess(0, input0, vecType);
  if (!vectors)
  {
    return VTK_ERROR;
  }

  // Set the function set to be integrated
  if (!this->InterpolatorPrototype)
  {
    if (amrData)
    {
      func = vtkAMRInterpolatedVelocityField::New();
    }
    else
    {
      func = vtkInterpolatedVelocityField::New();
    }
    // turn on the following segment, in place of the above line, if an
    // interpolator equipped with a cell locator is dedired as the default
    //
    // func = vtkCellLocatorInterpolatedVelocityField::New();
    // vtkSmartPointer< vtkStaticCellLocator > locator =
    // vtkSmartPointer< vtkStaticCellLocator >::New();
    // vtkCellLocatorInterpolatedVelocityField::SafeDownCast( func )
    //   ->SetCellLocatorPrototype( locator );
  }
  else
  {
    if (amrData &&
      vtkAMRInterpolatedVelocityField::SafeDownCast(this->InterpolatorPrototype) == nullptr)
    {
      this->InterpolatorPrototype = vtkAMRInterpolatedVelocityField::New();
    }
    func = this->InterpolatorPrototype->NewInstance();
    func->CopyParameters(this->InterpolatorPrototype);
  }

  if (vtkAMRInterpolatedVelocityField::SafeDownCast(func))
  {
    assert(amrData);
    vtkAMRInterpolatedVelocityField::SafeDownCast(func)->SetAMRData(amrData);
    if (maxCellSize)
    {
      *maxCellSize = 8;
    }
  }
  else if (vtkCompositeInterpolatedVelocityField::SafeDownCast(func))
  {
    iter->GoToFirstItem();
    while (!iter->IsDoneWithTraversal())
    {
      vtkDataSet* inp = vtkDataSet::SafeDownCast(iter->GetCurrentDataObject());
      if (inp)
      {
        int cellSize = inp->GetMaxCellSize();
        if (cellSize > *maxCellSize)
        {
          *maxCellSize = cellSize;
        }
        vtkCompositeInterpolatedVelocityField::SafeDownCast(func)->AddDataSet(inp);
      }
      iter->GoToNextItem();
    }
  }
  else
  {
    assert(false);
  }

  const char* vecName = vectors->GetName();
  func->SelectVectors(vecType, vecName);

  // Check if the data attributes match, warn if not
  vtkPointData* pd0 = input0->GetPointData();
  int numPdArrays = pd0->GetNumberOfArrays();
  this->HasMatchingPointAttributes = true;
  for (iter->GoToFirstItem(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
  {
    vtkDataSet* data = vtkDataSet::SafeDownCast(iter->GetCurrentDataObject());
    vtkPointData* pd = data->GetPointData();
    if (pd->GetNumberOfArrays() != numPdArrays)
    {
      this->HasMatchingPointAttributes = false;
    }
    for (int i = 0; i < numPdArrays; i++)
    {
      if (!pd->GetArray(pd0->GetArrayName(i)) || !pd0->GetArray(pd->GetArrayName(i)))
      {
        this->HasMatchingPointAttributes = false;
      }
    }
  }
  return VTK_OK;
}

// Support threaded integration of streamlines. Each streamline integration
// executes in a different thread (i.e., there is no benefit to threading
// if only a single streamline is integrated). Basically the way the threading
// works is that each thread processes a portion of the streamline seeds, each
// accumulating their own "output" via thread local storage. Then these thread
// outputs are combined to produce the final output.
namespace
{ // anonymous

// Each threaded tracer maintains its own output. To simplify things,
// the thread local output is contained in one struct.
struct vtkLocalThreadOutput
{
  // These are initialized in the Initialize() method.
  vtkSmartPointer<vtkInitialValueProblemSolver> LocalIntegrator;
  vtkSmartPointer<vtkInterpolatedVelocityField> Func;

  // These helper objects can be (mostly) initialized in this
  // struct's default constructor.
  std::vector<double> Weights;
  vtkSmartPointer<vtkGenericCell> Cell;
  vtkSmartPointer<vtkPoints> OutputPoints;
  vtkSmartPointer<vtkDoubleArray> Time;
  vtkSmartPointer<vtkDoubleArray> VelocityVectors;
  vtkSmartPointer<vtkDoubleArray> CellVectors;
  vtkSmartPointer<vtkDoubleArray> Vorticity;
  vtkSmartPointer<vtkDoubleArray> Rotation;
  vtkSmartPointer<vtkDoubleArray> AngularVelocity;
  vtkSmartPointer<vtkPolyData> Output;
  vtkPointData *OutputPD; //convenience to get at Output's point data
  double LastUsedStepSize;

  // Construct the data local to each thread. This constructor
  // handles hard-wired initialization. In the thread Initialize()
  // method, additional initialization is performed which depends
  // on user-specified parameters.
  vtkLocalThreadOutput()
  {
    this->Cell.TakeReference(vtkGenericCell::New());
    this->OutputPoints.TakeReference(vtkPoints::New());

    this->Time.TakeReference(vtkDoubleArray::New());
    this->Time->SetName("IntegrationTime");

    this->VelocityVectors.TakeReference(vtkDoubleArray::New());

    this->CellVectors.TakeReference(vtkDoubleArray::New());
    this->CellVectors->SetNumberOfComponents(3);
    this->CellVectors->Allocate(3 * VTK_CELL_SIZE);

    this->Vorticity.TakeReference(vtkDoubleArray::New());
    this->Vorticity->SetNumberOfComponents(3);
    this->Vorticity->SetName("Vorticity");

    this->Rotation.TakeReference(vtkDoubleArray::New());
    this->Rotation->SetName("Rotation");

    this->AngularVelocity.TakeReference(vtkDoubleArray::New());
    this->AngularVelocity->SetName("AngularVelocity");

    this->Output.TakeReference(vtkPolyData::New());
    this->OutputPD = this->Output->GetPointData();

    this->LastUsedStepSize = 0.0;
  }

};

// In order to ensure that the threaded output is the same as serial output,
// we organize output based on the seed number. Each seed will (likely)
// produce a stream tracer - these stream tracers are eventually composited
// (i.e., Reduced()) by seed number. That way, no matter what order the seeds
// are processed the output will be the same (i.e., results invariant).
//
// Also, this struct contains a more compact representation of streamlines
// (and associated cell data) which is expanded (in Reduce()) to produce the
// final filter output. A prefix sum is performed after all of the
// streamlines are generated, this generates offsets and such which are used
// to control where the output points and lines are written.
struct TracerOffset
{
  // For each seed (and hence streamline), these are where the data
  // originated from (i.e., which thread), a pointer to the local thread
  // data, the consecutive sequence of points that compose the streamline,
  // and the reason for termination.
  vtkLocalThreadOutput *ThreadOutput;
  vtkIdType ThreadPtId;
  vtkIdType NumPts;
  int RetVal;

  // These keeps track of where output is written to (in global filter output).
  // Generated via a prefix sum/scan in Reduce().
  vtkIdType PtOffset;
  vtkIdType CellId;

  TracerOffset() : ThreadOutput(nullptr), ThreadPtId(-1), NumPts(0),
                   RetVal(vtkStreamTracer::NOT_INITIALIZED),
                   PtOffset(0), CellId(-1) {}
};

using TracerOffsets = std::vector<TracerOffset>;


// The following class performs the threaded streamline integration.  The
// data members below control the propagation of streamlines based on the
// state of the vtkStreamTracer. Because threads may execute in a different
// order between runs, and we'd like the output to stay the same across runs,
// we order the output based on seed number (in Offsets).
struct TracerIntegrator
{
  // Integrator data members
  vtkStreamTracer *StreamTracer;
  double MaximumError;
  vtkIdType MaximumNumberOfSteps;
  double MaximumPropagation;
  bool ComputeVorticity;
  double RotationScale;
  double TerminalSpeed;
  bool SurfaceStreamlines;

  vtkPointData *Input0Data;
  vtkDataArray *SeedSource;
  vtkIdList *SeedIds;
  vtkIntArray *IntegrationDirections;
  TracerOffsets &Offsets;
  vtkAbstractInterpolatedVelocityField *FuncPrototype;
  vtkSmartPointer<vtkInitialValueProblemSolver> Integrator;
  int MaxCellSize;
  double InPropagation;
  vtkIdType InNumSteps;
  double InIntegrationTime;
  int VecType;
  const char* VecName;
  bool GenerateNormalsInIntegrate;
  vtkPolyData *Output;
  std::vector<CustomTerminationCallbackType> CustomTerminationCallback;
  std::vector<void*> CustomTerminationClientData;
  std::vector<int> CustomReasonForTermination;
  bool HasMatchingPointAttributes;

  // The following data are collected on a per-thread basis. Each thread generates
  // one or more streamlines.
  vtkSMPThreadLocal<vtkLocalThreadOutput> LocalThreadOutput;

  TracerIntegrator(vtkStreamTracer *streamTracer, vtkPointData *input0Data, vtkDataArray *seedSource,
                   vtkIdList *seedIds, vtkIntArray *intDirs, TracerOffsets &offsets,
                   vtkAbstractInterpolatedVelocityField *func, vtkInitialValueProblemSolver *integrator,
                   int maxCellSize, double inPropagation, vtkIdType inNumSteps, double inIntegrationTime,
                   int vecType, const char *vecName, bool genNormals, vtkPolyData *output,
                   std::vector<CustomTerminationCallbackType> &customTerminationCallback,
                   std::vector<void*> &customTerminationClientData,
                   std::vector<int> &customReasonForTermination,
                   bool hasMatching) :
    StreamTracer(streamTracer),
    Input0Data(input0Data), SeedSource(seedSource), SeedIds(seedIds), IntegrationDirections(intDirs), Offsets(offsets),
    FuncPrototype(func), Integrator(integrator), MaxCellSize(maxCellSize), InPropagation(inPropagation),
    InNumSteps(inNumSteps), InIntegrationTime(inIntegrationTime), VecType(vecType), VecName(vecName),
    GenerateNormalsInIntegrate(genNormals), Output(output),
    CustomTerminationCallback(customTerminationCallback),
    CustomTerminationClientData(customTerminationClientData),
    CustomReasonForTermination(customReasonForTermination),
    HasMatchingPointAttributes(hasMatching)
  {
    this->MaximumError = this->StreamTracer->GetMaximumError();
    this->MaximumNumberOfSteps = this->StreamTracer->GetMaximumNumberOfSteps();
    this->MaximumPropagation = this->StreamTracer->GetMaximumPropagation();
    this->ComputeVorticity = this->StreamTracer->GetComputeVorticity();
    this->RotationScale = this->StreamTracer->GetRotationScale();
    this->TerminalSpeed = this->StreamTracer->GetTerminalSpeed();
    this->SurfaceStreamlines = this->StreamTracer->GetSurfaceStreamlines();

    // Some initialization is required to construct supporting locators and
    // perform other non-thread-safe startup tasks. The side effect of the
    // methods below are to cause this initialization to occur.
    double point1[3], velocity[3];
    seedSource->GetTuple(seedIds->GetId(0), point1);
    func->FunctionValues(point1, velocity);
  }

  void Initialize()
  {
    // Some data members of the local output require per-thread initialization.
    vtkLocalThreadOutput &localOutput = this->LocalThreadOutput.Local();

    localOutput.LocalIntegrator.TakeReference(this->Integrator->NewInstance());

    localOutput.Func.TakeReference(vtkInterpolatedVelocityField::New());
    localOutput.Func->CopyParameters(this->FuncPrototype);

    if (this->VecType != vtkDataObject::POINT)
    {
      localOutput.VelocityVectors = vtkSmartPointer<vtkDoubleArray>::New();
      localOutput.VelocityVectors->SetName(this->VecName);
      localOutput.VelocityVectors->SetNumberOfComponents(3);
    }

    localOutput.Output->GetPointData()->
      InterpolateAllocate(this->Input0Data, this->MaximumNumberOfSteps);
  }

  void operator()(vtkIdType seedNum, vtkIdType endSeedNum)
  {
    // Symbolic shortcuts to thread local data
    vtkLocalThreadOutput &localOutput = this->LocalThreadOutput.Local();
    std::vector<double> &weights = localOutput.Weights;
    vtkGenericCell *cell = localOutput.Cell;
    vtkInitialValueProblemSolver *integrator = localOutput.LocalIntegrator;
    vtkInterpolatedVelocityField *func = localOutput.Func;
    vtkPoints *outputPoints = localOutput.OutputPoints;
    vtkDoubleArray *time = localOutput.Time;
    vtkDoubleArray *velocityVectors = localOutput.VelocityVectors;
    vtkDoubleArray *cellVectors = localOutput.CellVectors;
    vtkDoubleArray *vorticity = localOutput.Vorticity;
    vtkDoubleArray *rotation = localOutput.Rotation;
    vtkDoubleArray *angularVel = localOutput.AngularVelocity;
    vtkPolyData *output = localOutput.Output;
    double &lastUsedStepSize = localOutput.LastUsedStepSize;

    // Initialize in preparation for stream tracer production
    int maxCellSize = this->MaxCellSize;
    vtkDataArray *seedSource = this->SeedSource;
    vtkIdList *seedIds = this->SeedIds;
    vtkIntArray *integrationDirections = this->IntegrationDirections;
    double propagation = this->InPropagation;
    vtkIdType numSteps = this->InNumSteps;
    double integrationTime = this->InIntegrationTime;
    int vecType = this->VecType;
    const char *vecName = this->VecName;
    double lastPoint[3];

    // Useful pointers
    vtkDataSetAttributes* outputPD = output->GetPointData();
    vtkPointData* inputPD;
    vtkDataSet* input;
    vtkDataArray* inVectors;

    int direction = 1;
    double *weightsPtr = nullptr;
    if (maxCellSize > 0)
    {
      weights.reserve(maxCellSize);
      weightsPtr = weights.data();
    }

    // Associate the interpolation function with the integrator
    integrator->SetFunctionSet(func);

    // Check Surface option
    vtkInterpolatedVelocityField* surfaceFunc = nullptr;
    if (this->SurfaceStreamlines == true)
    {
      surfaceFunc = vtkInterpolatedVelocityField::SafeDownCast(func);
      if (surfaceFunc)
      {
        surfaceFunc->SetForceSurfaceTangentVector(true);
        surfaceFunc->SetSurfaceDataset(true);
      }
    }

    // We will interpolate all point attributes of the input on each point of
    // the output (unless they are turned off). Note that we are using only
    // the first input, if there are more than one, the attributes have to match.
    double velocity[3];

    for ( ; seedNum < endSeedNum; ++seedNum )
    {
      switch (integrationDirections->GetValue(seedNum))
      {
        case vtkStreamTracer::FORWARD:
          direction = 1;
          break;
        case vtkStreamTracer::BACKWARD:
          direction = -1;
          break;
      }

      // temporary variables used in the integration
      double point1[3], point2[3], pcoords[3], vort[3], omega;
      vtkIdType index, numPts = 0;

      // Clear the last cell to avoid starting a search from
      // the last point in the streamline
      func->ClearLastCellId();

      // Initial point
      seedSource->GetTuple(seedIds->GetId(seedNum), point1);
      memcpy(point2, point1, 3 * sizeof(double));
      if (!func->FunctionValues(point1, velocity))
      {
        continue;
      }

      if (propagation >= this->MaximumPropagation || numSteps > this->MaximumNumberOfSteps)
      {
        continue;
      }

      numPts++;
      vtkIdType nextPoint = outputPoints->InsertNextPoint(point1);
      double lastInsertedPoint[3];
      outputPoints->GetPoint(nextPoint, lastInsertedPoint);
      time->InsertNextValue(integrationTime);

      // We will always pass an arc-length step size to the integrator.
      // If the user specifies a step size in cell length unit, we will
      // have to convert it to arc length.
      vtkIntervalInformation stepSize; // either positive or negative
      stepSize.Unit = vtkStreamTracer::LENGTH_UNIT;
      stepSize.Interval = 0;
      vtkIntervalInformation aStep; // always positive
      aStep.Unit = vtkStreamTracer::LENGTH_UNIT;
      double step, minStep = 0, maxStep = 0;
      double stepTaken;
      double speed;
      double cellLength;
      int retVal=vtkStreamTracer::OUT_OF_LENGTH, tmp;

      // Make sure we use the dataset found by the vtkAbstractInterpolatedVelocityField
      input = func->GetLastDataSet();
      inputPD = input->GetPointData();
      inVectors = input->GetAttributesAsFieldData(vecType)->GetArray(vecName);
      // Convert intervals to arc-length unit
      input->GetCell(func->GetLastCellId(), cell);
      cellLength = sqrt(static_cast<double>(cell->GetLength2()));
      speed = vtkMath::Norm(velocity);
      // Never call conversion methods if speed == 0
      if (speed != 0.0)
      {
        this->StreamTracer->
          ConvertIntervals(stepSize.Interval, minStep, maxStep, direction, cellLength);
      }

      // Interpolate all point attributes on first point
      func->GetLastWeights(weightsPtr);
      InterpolatePoint(outputPD, inputPD, nextPoint, cell->PointIds, weightsPtr,
                       this->HasMatchingPointAttributes);
      // handle both point and cell velocity attributes.
      vtkDataArray* outputVelocityVectors = outputPD->GetArray(vecName);
      if (vecType != vtkDataObject::POINT)
      {
        velocityVectors->InsertNextTuple(velocity);
        outputVelocityVectors = velocityVectors;
      }

      // Compute vorticity if required.
      // This can be used later for streamribbon generation.
      if (this->ComputeVorticity)
      {
        if (vecType == vtkDataObject::POINT)
        {
          inVectors->GetTuples(cell->PointIds, cellVectors);
          func->GetLastLocalCoordinates(pcoords);
          this->StreamTracer->CalculateVorticity(cell, pcoords, cellVectors, vort);
        }
        else
        {
          vort[0] = 0;
          vort[1] = 0;
          vort[2] = 0;
        }
        vorticity->InsertNextTuple(vort);
        // rotation
        // local rotation = vorticity . unit tangent ( i.e. velocity/speed )
        if (speed != 0.0)
        {
          omega = vtkMath::Dot(vort, velocity);
          omega /= speed;
          omega *= this->RotationScale;
        }
        else
        {
          omega = 0.0;
        }
        angularVel->InsertNextValue(omega);
        rotation->InsertNextValue(0.0);
      }

      double error = 0;

      // Integrate until the maximum propagation length is reached,
      // maximum number of steps is reached or until a boundary is encountered.
      // Begin Integration
      while (propagation < this->MaximumPropagation)
      {

        if (numSteps > this->MaximumNumberOfSteps)
        {
          retVal = vtkStreamTracer::OUT_OF_STEPS;
          break;
        }

        bool endIntegration = false;
        for (std::size_t i = 0; i < this->CustomTerminationCallback.size(); ++i)
        {
          if (this->CustomTerminationCallback[i](
                this->CustomTerminationClientData[i], outputPoints, outputVelocityVectors, direction))
          {
            retVal = this->CustomReasonForTermination[i];
            endIntegration = true;
            break;
          }
        }
        if (endIntegration)
        {
          break;
        }

        // Never call conversion methods if speed == 0
        if ((speed == 0) || (speed <= this->TerminalSpeed))
        {
          retVal = vtkStreamTracer::STAGNATION;
          break;
        }

        // If, with the next step, propagation will be larger than
        // max, reduce it so that it is (approximately) equal to max.
        aStep.Interval = fabs(stepSize.Interval);

        if ((propagation + aStep.Interval) > this->MaximumPropagation)
        {
          aStep.Interval = this->MaximumPropagation - propagation;
          if (stepSize.Interval >= 0)
          {
            stepSize.Interval = vtkIntervalInformation::ConvertToLength(aStep, cellLength);
          }
          else
          {
            stepSize.Interval = vtkIntervalInformation::ConvertToLength(aStep, cellLength) * (-1.0);
          }
          maxStep = stepSize.Interval;
        }
        lastUsedStepSize = stepSize.Interval;

        // Calculate the next step using the integrator provided
        // Break if the next point is out of bounds.
        func->SetNormalizeVector(true);
        tmp = integrator->ComputeNextStep(point1, point2, 0, stepSize.Interval, stepTaken, minStep,
          maxStep, this->MaximumError, error);
        func->SetNormalizeVector(false);
        if (tmp != 0)
        {
          retVal = tmp;
          memcpy(lastPoint, point2, 3 * sizeof(double));
          break;
        }

        // This is the next starting point
        if (this->SurfaceStreamlines && surfaceFunc != nullptr)
        {
          if (surfaceFunc->SnapPointOnCell(point2, point1) != 1)
          {
            retVal = vtkStreamTracer::OUT_OF_DOMAIN;
            memcpy(lastPoint, point2, 3 * sizeof(double));
            break;
          }
        }
        else
        {
          for (int i = 0; i < 3; i++)
          {
            point1[i] = point2[i];
          }
        }

        // Interpolate the velocity at the next point
        if (!func->FunctionValues(point2, velocity))
        {
          retVal = vtkStreamTracer::OUT_OF_DOMAIN;
          memcpy(lastPoint, point2, 3 * sizeof(double));
          break;
        }

        // It is not enough to use the starting point for stagnation calculation
        // Use average speed to check if it is below stagnation threshold
        double speed2 = vtkMath::Norm(velocity);
        if ((speed + speed2) / 2 <= this->TerminalSpeed)
        {
          retVal = vtkStreamTracer::STAGNATION;
          break;
        }

        integrationTime += stepTaken / speed;
        // Calculate propagation (using the same units as MaximumPropagation
        propagation += fabs(stepSize.Interval);

        // Make sure we use the dataset found by the vtkAbstractInterpolatedVelocityField
        input = func->GetLastDataSet();
        inputPD = input->GetPointData();
        inVectors = input->GetAttributesAsFieldData(vecType)->GetArray(vecName);

        // Calculate cell length and speed to be used in unit conversions
        input->GetCell(func->GetLastCellId(), cell);
        cellLength = sqrt(static_cast<double>(cell->GetLength2()));
        speed = speed2;

        // Check if conversion to float will produce a point in same place
        float convertedPoint[3];
        for (int i = 0; i < 3; i++)
        {
          convertedPoint[i] = point1[i];
        }
        if (lastInsertedPoint[0] != convertedPoint[0] || lastInsertedPoint[1] != convertedPoint[1] ||
          lastInsertedPoint[2] != convertedPoint[2])
        {
          // Point is valid. Insert it.
          numPts++;
          nextPoint = outputPoints->InsertNextPoint(point1);
          outputPoints->GetPoint(nextPoint, lastInsertedPoint);
          time->InsertNextValue(integrationTime);

          // Interpolate all point attributes on current point
          func->GetLastWeights(weightsPtr);
          InterpolatePoint(outputPD, inputPD, nextPoint, cell->PointIds, weightsPtr,
                           this->HasMatchingPointAttributes);

          if (vecType != vtkDataObject::POINT)
          {
            velocityVectors->InsertNextTuple(velocity);
          }
          // Compute vorticity if required
          // This can be used later for streamribbon generation.
          if (this->ComputeVorticity)
          {
            if (vecType == vtkDataObject::POINT)
            {
              inVectors->GetTuples(cell->PointIds, cellVectors);
              func->GetLastLocalCoordinates(pcoords);
              this->StreamTracer->CalculateVorticity(cell, pcoords, cellVectors, vort);
            }
            else
            {
              vort[0] = 0;
              vort[1] = 0;
              vort[2] = 0;
            }
            vorticity->InsertNextTuple(vort);
            // rotation
            // angular velocity = vorticity . unit tangent ( i.e. velocity/speed )
            // rotation = sum ( angular velocity * stepSize )
            omega = vtkMath::Dot(vort, velocity);
            omega /= speed;
            omega *= this->RotationScale;
            index = angularVel->InsertNextValue(omega);
            rotation->InsertNextValue(rotation->GetValue(index - 1) +
              (angularVel->GetValue(index - 1) + omega) / 2 *
                (integrationTime - time->GetValue(index - 1)));
          }
        }

        // Never call conversion methods if speed == 0
        if ((speed == 0) || (speed <= this->TerminalSpeed))
        {
          retVal = vtkStreamTracer::STAGNATION;
          break;
        }

        // Convert all intervals to arc length
        this->StreamTracer->ConvertIntervals(step, minStep, maxStep, direction, cellLength);

        // If the solver is adaptive and the next step size (stepSize.Interval)
        // that the solver wants to use is smaller than minStep or larger
        // than maxStep, re-adjust it. This has to be done every step
        // because minStep and maxStep can change depending on the cell
        // size (unless it is specified in arc-length unit)
        if (integrator->IsAdaptive())
        {
          if (fabs(stepSize.Interval) < fabs(minStep))
          {
            stepSize.Interval = fabs(minStep) * stepSize.Interval / fabs(stepSize.Interval);
          }
          else if (fabs(stepSize.Interval) > fabs(maxStep))
          {
            stepSize.Interval = fabs(maxStep) * stepSize.Interval / fabs(stepSize.Interval);
          }
        }
        else
        {
          stepSize.Interval = step;
        }
      }

      // If a valid polyline, insert it. We also keep track of the number of
      // points generated and other related information for the purposes of
      // generating offsets and in general managing the threading output.
      if (numPts > 1)
      {
        TracerOffset& offset = this->Offsets[seedNum];
        offset.ThreadOutput = &localOutput;
        offset.ThreadPtId = outputPoints->GetNumberOfPoints() - numPts;
        offset.NumPts = numPts;
        offset.RetVal = retVal;
      }

      // Initialize these to 0 before starting the next line.
      // The values passed in the function call are only used
      // for the first line.
      propagation = 0;
      numSteps = 0;
      integrationTime = 0;

    }//for all seeds in this batch
  }

  // Perform the final compositing operation to assemble the
  // filter output.
  struct CompositeOverSeeds
  {
    TracerOffsets &Offsets;
    vtkPoints *OutPoints;
    vtkIdType *CAOffsets;
    vtkIdType *CAConn;
    vtkPointData *OutPD;
    int *SeedIds;
    int *RetVals;

    CompositeOverSeeds(TracerOffsets &offsets, vtkPoints *outPoints, vtkIdType *caOffsetsPtr,
                       vtkIdType *caConnPtr, vtkPointData *outPD, int *seedIdsPtr,int *retValsPtr) :
      Offsets(offsets), OutPoints(outPoints), CAOffsets(caOffsetsPtr), CAConn(caConnPtr),
      OutPD(outPD), SeedIds(seedIdsPtr), RetVals(retValsPtr)
    {}

    void operator()(vtkIdType seedId, vtkIdType endSeedId)
    {
      double x[3];

      for ( ; seedId < endSeedId; ++seedId )
      {
        TracerOffset& offset = this->Offsets[seedId];
        if ( offset.NumPts > 0 )
        {
          vtkIdType cellId = offset.CellId;

          // Copy the points and cells from the threads output to
          // the filter output. Also copy the point data.
          vtkPoints *threadPts = offset.ThreadOutput->OutputPoints;
          vtkIdType outPtId = offset.PtOffset;
          this->CAOffsets[cellId] = offset.PtOffset;
          vtkPointData *threadPD = offset.ThreadOutput->OutputPD;

          for ( auto i=0; i < offset.NumPts; ++i )
          {
            vtkIdType threadId = offset.ThreadPtId + i;
            vtkIdType outId = outPtId + i;
            threadPts->GetPoint(threadId,x);
            this->OutPoints->SetPoint(outId,x);
            this->CAConn[outId] = outId;
            this->OutPD->CopyData(threadPD,threadId,outId);
          }

          // Copy the cell data
          this->SeedIds[cellId] = seedId;
          this->RetVals[cellId] = offset.RetVal;
        } // if a streamline generated for this seed
      } // for all seeds
    } // operator()
  };

  // Assemble the thread output.
  void AssembleOutput(vtkLocalThreadOutput& threadOutput)
  {
    vtkPointData *outputPD = threadOutput.OutputPD;
    outputPD->AddArray(threadOutput.Time);

    if ( this->ComputeVorticity )
    {
      outputPD->AddArray(threadOutput.Vorticity);
      outputPD->AddArray(threadOutput.Rotation);
      outputPD->AddArray(threadOutput.AngularVelocity);
    }

  }

  // Combine the outputs of the threads into the filter output. This is
  // effectively an append operation.
  void Reduce()
  {
    // Perform a prefix sum to generate offsets. These will be used to
    // allocate the single filter output, and copy thread data to the filter
    // output.
    vtkIdType ptOffset=0, numCells=0, npts;
    for ( auto& offIter : this->Offsets )
    {
      if ( (npts = offIter.NumPts) > 0 )
      {
        offIter.PtOffset = ptOffset;
        ptOffset += npts;
        offIter.CellId = numCells++;
      }
    }
    // The number of filter output points
    vtkIdType numPts = ptOffset;

    // Now finalize the output in each thread. Meaning assigning point data
    // to the thread output. (We deferred doing this previously so as not to
    // interfere with the point data interpolation of filter input
    // attributes.)  In the magical process of copying data from the threads
    // to the final filter output, all point data must be properly set up in
    // the thread output so that vtkPointData::CopyData() works properly.
    auto ldEnd = this->LocalThreadOutput.end();
    for (auto ldItr = this->LocalThreadOutput.begin(); ldItr != ldEnd; ++ldItr)
    {
      this->AssembleOutput(*ldItr);
    }

    // In the following, allocate the output points, cell array, and the
    // point and cell attribute data.

    // Geometry: points
    vtkNew<vtkPoints> outPoints;
    outPoints->SetNumberOfPoints(numPts);

    // Topology: allocate objects that are assembled into a cell array.
    vtkNew<vtkIdTypeArray> caOffsets;
    vtkIdType* caOffsetsPtr = caOffsets->WritePointer(0, numCells+1);
    vtkNew<vtkIdTypeArray> caConn;
    vtkIdType* caConnPtr = caConn->WritePointer(0, numPts);
    caOffsetsPtr[numCells] = numPts;

    // Interpolated point data: need to copy from thread local to the filter
    // output. Streamer point data: use the first thread local data to
    // configure the arrays because all threads have the same data
    // sttributes.
    vtkPointData *threadPD = this->LocalThreadOutput.Local().OutputPD;
    vtkPointData *outputPD = this->Output->GetPointData();
    outputPD->CopyAllocate(threadPD,numPts);

    // Allocate streamer cell data: seed ids and streamer termination return
    // values.
    vtkNew<vtkIntArray> seedIds;
    int* seedIdsPtr = seedIds->WritePointer(0,numCells);
    seedIds->SetName("SeedIds");

    vtkNew<vtkIntArray> retVals;
    retVals->SetName("ReasonForTermination");
    int* retValsPtr = retVals->WritePointer(0,numCells);

    this->Output->GetCellData()->AddArray(retVals);
    this->Output->GetCellData()->AddArray(seedIds);

    // Now thread over the seeds, producing the final points, polylines,
    // and attribute data, as well as copying over interpolated point data.
    CompositeOverSeeds comp(this->Offsets, outPoints,caOffsetsPtr,caConnPtr,
                            outputPD,seedIdsPtr,retValsPtr);
    vtkSMPTools::For(0, this->Offsets.size(), comp);

    // Finally, assemble the objects to create the filter output.
    this->Output->SetPoints(outPoints);
    vtkNew<vtkCellArray> streamers;
    streamers->SetData(caOffsets,caConn);
    this->Output->SetLines(streamers);

    // If requested, generate normals
    if ( this->GenerateNormalsInIntegrate )
    {
      this->StreamTracer->GenerateNormals(this->Output, nullptr, this->VecName);
    }

  } // Reduce()

}; // TracerIntegrator

} //anonymous

//------------------------------------------------------------------------------
// This method sets up the integration for one or more threads. Care has to be
// taken to ensure that duplicate locators are not created (memory issues), and
// that thread-safe operations are used.
void vtkStreamTracer::
Integrate(vtkPointData* input0Data, vtkPolyData* output,
          vtkDataArray* seedSource, vtkIdList* seedIds, vtkIntArray* intDirs,
          vtkAbstractInterpolatedVelocityField* func, int maxCellSize, int vecType,
          const char* vecName, double& inPropagation, vtkIdType& inNumSteps, double& inIntegrationTime,
          std::vector<CustomTerminationCallbackType> &customTerminationCallback,
          std::vector<void*> &customTerminationClientData,
          std::vector<int> &customReasonForTermination)
{
  vtkInitialValueProblemSolver* integrator = this->GetIntegrator();
  if ( integrator == nullptr)
  {
    vtkErrorMacro("No integrator is specified.");
    return;
  }

  // Setup the offsets for compositing data.
  vtkIdType numSeeds = seedIds->GetNumberOfIds();
  TracerOffsets offsets(numSeeds);

  // We will interpolate all point attributes of the input on each point of
  // the output (unless they are turned off). Note that we are using only
  // the first input, if there are more than one, the attributes have to match.
  //
  // Note: We have to use a specific value (safe to employ the maximum number
  //       of steps) as the size of the initial memory allocation here. The
  //       use of the default argument might incur a crash problem (due to
  //       "insufficient memory") in the parallel mode. This is the case when
  //       a streamline intensely shuttles between two processes in an exactly
  //       interleaving fashion --- only one point is produced on each process
  //       (and actually two points, after point duplication, are saved to a
  //       vtkPolyData in vtkDistributedStreamTracer::NoBlockProcessTask) and
  //       as a consequence a large number of such small vtkPolyData objects
  //       are needed to represent a streamline, consuming up the memory before
  //       the intermediate memory is timely released.
  output->GetPointData()->InterpolateAllocate(input0Data, this->MaximumNumberOfSteps);


  // Generate streamlines.
  TracerIntegrator ti(this, input0Data, seedSource, seedIds, intDirs, offsets,
                      func, integrator, maxCellSize, inPropagation, inNumSteps,
                      inIntegrationTime, vecType, vecName, this->GenerateNormalsInIntegrate,
                      output, customTerminationCallback, customTerminationClientData,
                      customReasonForTermination, this->HasMatchingPointAttributes);

  // Streamline threading only kicks in when the number of seeds exceeds a
  // threshold value.  This is because there is a cost to spinning up
  // threads, and then compositing the results. So for small numbers of
  // seeds, just use a serial approach. Otherwise thread the streamlines.
  const int VTK_ST_THREADING_THRESHOLD = 8;
  if ( numSeeds < VTK_ST_THREADING_THRESHOLD )
  {// Serial
    vtkSMPTools::LocalScope(vtkSMPTools::Config{"Sequential"}, [&]()
    {vtkSMPTools::For(0,numSeeds, ti);} );
  }
  else
  {
    vtkSMPTools::For(0,numSeeds, ti);
  }
}

//------------------------------------------------------------------------------
void vtkStreamTracer::
GenerateNormals(vtkPolyData* output, double* firstNormal, const char* vecName)
{
  vtkDataSetAttributes* outputPD = output->GetPointData();
  vtkPoints* outputPoints = output->GetPoints();
  vtkIdType numPts = outputPoints->GetNumberOfPoints();
  if (numPts <= 1 || !this->ComputeVorticity )
  {
    return;
  }

  // Setup the computation
  vtkCellArray* outputLines = output->GetLines();
  vtkDataArray* rotation = outputPD->GetArray("Rotation");

  vtkNew<vtkDoubleArray> normals;
  normals->SetNumberOfComponents(3);
  normals->SetNumberOfTuples(numPts);
  normals->SetName("Normals");

  // Make sure the normals are initialized in case
  // GenerateSlidingNormals() fails and returns before
  // creating all normals
  vtkSMPTools::For(0,numPts, [&](vtkIdType ptId, vtkIdType endPtId)
  {
    for ( ; ptId < endPtId; ++ptId)
    {
      normals->SetTuple3(ptId, 1, 0, 0);
    }
  });

  // Generate the orientation normals. This will be threaded since none of the
  // lines "reuse" points from another line.
  vtkNew<vtkPolyLine> lineNormalGenerator;
  lineNormalGenerator->GenerateSlidingNormals(outputPoints, outputLines,
                                              normals, firstNormal, true);

  // Now generate the final streamer normals
  vtkDataArray* newVectors = outputPD->GetVectors(vecName);
  if (newVectors == nullptr || newVectors->GetNumberOfTuples() != numPts)
  { // This should never happen.
    vtkErrorMacro("Bad velocity array.");
    return;
  }

  // Thread the final normal generation
  vtkSMPTools::For(0,numPts, [&](vtkIdType ptId, vtkIdType endPtId)
  {
    double normal[3], local1[3], local2[3], theta, costheta, sintheta, length;
    double velocity[3];
    for ( ; ptId < endPtId; ++ptId)
    {
      normals->GetTuple(ptId, normal);
      newVectors->GetTuple(ptId, velocity);
      // obtain two unit orthogonal vectors on the plane perpendicular to
      // the streamline
      for (auto j = 0; j < 3; j++)
      {
        local1[j] = normal[j];
      }
      length = vtkMath::Normalize(local1);
      vtkMath::Cross(local1, velocity, local2);
      vtkMath::Normalize(local2);
      // Rotate the normal with theta
      rotation->GetTuple(ptId, &theta);
      costheta = cos(theta);
      sintheta = sin(theta);
      for (auto j = 0; j < 3; j++)
      {
        normal[j] = length * (costheta * local1[j] + sintheta * local2[j]);
      }
      normals->SetTuple(ptId, normal);
    }
  });//lambda

  // Associate normals with the output
  outputPD->AddArray(normals);
  outputPD->SetActiveAttribute("Normals", vtkDataSetAttributes::VECTORS);
}

//------------------------------------------------------------------------------
// This is used by sub-classes in certain situations. It
// does a lot less (for example, does not compute attributes)
// than Integrate.
double vtkStreamTracer::SimpleIntegrate(
  double seed[3], double lastPoint[3], double stepSize, vtkAbstractInterpolatedVelocityField* func)
{
  vtkIdType numSteps = 0;
  vtkIdType maxSteps = 20;
  double error = 0;
  double stepTaken = 0;
  double point1[3], point2[3];
  double velocity[3];
  double speed;
  int stepResult;

  (void)seed; // Seed is not used

  memcpy(point1, lastPoint, 3 * sizeof(double));

  // Create a new integrator, the type is the same as Integrator
  vtkInitialValueProblemSolver* integrator = this->GetIntegrator()->NewInstance();
  integrator->SetFunctionSet(func);

  while (true)
  {

    if (numSteps++ > maxSteps)
    {
      break;
    }

    // Calculate the next step using the integrator provided
    // Break if the next point is out of bounds.
    func->SetNormalizeVector(true);
    double tmpStepTaken = 0;
    stepResult =
      integrator->ComputeNextStep(point1, point2, 0, stepSize, tmpStepTaken, 0, 0, 0, error);
    stepTaken += tmpStepTaken;
    func->SetNormalizeVector(false);
    if (stepResult != 0)
    {
      memcpy(lastPoint, point2, 3 * sizeof(double));
      break;
    }

    // This is the next starting point
    for (int i = 0; i < 3; i++)
    {
      point1[i] = point2[i];
    }

    // Interpolate the velocity at the next point
    if (!func->FunctionValues(point2, velocity))
    {
      memcpy(lastPoint, point2, 3 * sizeof(double));
      break;
    }

    speed = vtkMath::Norm(velocity);

    // Never call conversion methods if speed == 0
    if ((speed == 0) || (speed <= this->TerminalSpeed))
    {
      break;
    }

    memcpy(point1, point2, 3 * sizeof(double));
    // End Integration
  }

  integrator->Delete();
  return stepTaken;
}

//------------------------------------------------------------------------------
int vtkStreamTracer::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
  }
  else if (port == 1)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  }
  return 1;
}

//------------------------------------------------------------------------------
void vtkStreamTracer::AddCustomTerminationCallback(
  CustomTerminationCallbackType callback, void* clientdata, int reasonForTermination)
{
  this->CustomTerminationCallback.push_back(callback);
  this->CustomTerminationClientData.push_back(clientdata);
  this->CustomReasonForTermination.push_back(reasonForTermination);
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkStreamTracer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Start position: " << this->StartPosition[0] << " " << this->StartPosition[1]
     << " " << this->StartPosition[2] << endl;
  os << indent << "Terminal speed: " << this->TerminalSpeed << endl;

  os << indent << "Maximum propagation: " << this->MaximumPropagation << " unit: length." << endl;

  os << indent << "Integration step unit: "
     << ((this->IntegrationStepUnit == LENGTH_UNIT) ? "length." : "cell length.") << endl;

  os << indent << "Initial integration step: " << this->InitialIntegrationStep << endl;

  os << indent << "Minimum integration step: " << this->MinimumIntegrationStep << endl;

  os << indent << "Maximum integration step: " << this->MaximumIntegrationStep << endl;

  os << indent << "Integration direction: ";
  switch (this->IntegrationDirection)
  {
    case FORWARD:
      os << "forward.";
      break;
    case BACKWARD:
      os << "backward.";
      break;
    case BOTH:
      os << "both directions.";
      break;
  }
  os << endl;

  os << indent << "Integrator: " << this->Integrator << endl;
  os << indent << "Maximum error: " << this->MaximumError << endl;
  os << indent << "Maximum number of steps: " << this->MaximumNumberOfSteps << endl;
  os << indent << "Vorticity computation: " << (this->ComputeVorticity ? " On" : " Off") << endl;
  os << indent << "Rotation scale: " << this->RotationScale << endl;
}

//------------------------------------------------------------------------------
vtkExecutive* vtkStreamTracer::CreateDefaultExecutive()
{
  return vtkCompositeDataPipeline::New();
}
