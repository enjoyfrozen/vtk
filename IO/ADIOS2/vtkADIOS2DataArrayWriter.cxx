// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkADIOS2DataArrayWriter.h"
#include "vtkArrayDispatch.h"
#include "vtkDataArray.h"
#include "vtkDataArrayAccessor.h"
#include "vtkLogger.h"
#include "vtkObjectFactory.h"

#include <adios2.h> // adios2

#if VTK_MODULE_ENABLE_VTK_ParallelMPI
#include "vtkMPIController.h"
#endif

#include <numeric> //std::accumulate

VTK_ABI_NAMESPACE_BEGIN

class vtkADIOS2DataArrayWriter::vtkInternals
{
public:
  std::unique_ptr<adios2::ADIOS> Adios;
  adios2::IO AdiosIO;
  adios2::Engine BpWriter;

  const std::map<vtkADIOS2DataArrayWriter::ADIOS2WriterType, std::string> WRITER_TYPES = {
    { ADIOS2WriterType::BP3, "BP3" },
    { ADIOS2WriterType::BP4, "BP4" },
    { ADIOS2WriterType::BP5, "BP5" },
  };

  vtkInternals()
  {
#if VTK_MODULE_ENABLE_VTK_ParallelMPI
    vtkMultiProcessController* controller = vtkMultiProcessController::GetGlobalController();
    if (!controller)
    {
      vtkLogF(ERROR, "Global MPI controller is not defined!");
      return;
    }
    vtkMPICommunicator* communicator =
      vtkMPICommunicator::SafeDownCast(controller->GetCommunicator());
    MPI_Comm mpiCommunicator = *(communicator->GetMPIComm()->GetHandle());

    this->Adios.reset(new adios2::ADIOS(mpiCommunicator));
#else
    this->Adios.reset(new adios2::ADIOS());
#endif
  }
};

// Array Dispatcher for copying values from any type of vtkDataArray to
// the BP file by creating a adios2 variable of the proper type and copying the values.
struct CopyArrayWorker
{

  adios2::IO& AdiosIO;
  adios2::Engine& BpWriter;

  CopyArrayWorker(adios2::IO& adiosIO, adios2::Engine& bpWriter)
    : AdiosIO(adiosIO)
    , BpWriter(bpWriter){};

  // Fast path
  template <typename ValueType>
  void operator()(vtkAOSDataArrayTemplate<ValueType>* array, const std::string& name,
    const adios2::Dims& shape, const adios2::Dims& start, const adios2::Dims& count,
    const bool constantDims)
  {

    adios2::Variable<ValueType> bpArray = this->AdiosIO.InquireVariable<ValueType>(name);
    if (bpArray)
    {
      return;
    }

    bpArray = this->AdiosIO.DefineVariable<ValueType>(name, shape, start, count, constantDims);
    try
    {
      this->BpWriter.Put(bpArray, array->Begin(), adios2::Mode::Sync);
    }
    catch (const std::exception& ex)
    {
      vtkLogF(ERROR, "Exception writting array %s to BP File\n%s", name.c_str(), ex.what());
    }
  }

  // For SOA layouts just go element by element
  template <typename Array>
  void operator()(Array* array, const std::string& name, const adios2::Dims& shape,
    const adios2::Dims& start, const adios2::Dims& count, bool constantDims)
  {
    vtkDataArrayAccessor<Array> accessor(array);
    using ValueType = typename vtkDataArrayAccessor<Array>::APIType;
    adios2::Variable<ValueType> bpArray = this->AdiosIO.InquireVariable<ValueType>(name);
    if (bpArray)
    {
      return;
    }

    std::vector<ValueType> rawData;

    const vtkIdType numItems = array->GetNumberOfTuples();
    const vtkIdType numberOfComponents = array->GetNumberOfComponents();
    rawData.reserve(numItems * numberOfComponents);
    for (vtkIdType i = 0; i < numItems; ++i)
    {
      for (vtkIdType j = 0; j < numberOfComponents; ++j)
      {
        rawData.push_back(accessor.Get(i, j));
      }
    }

    bpArray = this->AdiosIO.DefineVariable<ValueType>(name, shape, start, count, constantDims);
    try
    {
      this->BpWriter.Put(bpArray, rawData.data(), adios2::Mode::Sync);
    }
    catch (const std::exception& ex)
    {
      vtkLogF(ERROR, "Exception writting array %s to BP File\n%s", name.c_str(), ex.what());
    }
  }
};

vtkStandardNewMacro(vtkADIOS2DataArrayWriter);
//----------------------------------------------------------------------------
vtkADIOS2DataArrayWriter::vtkADIOS2DataArrayWriter()
{
  this->Internals = new vtkInternals();
}

//----------------------------------------------------------------------------
void vtkADIOS2DataArrayWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkADIOS2DataArrayWriter::~vtkADIOS2DataArrayWriter()
{
  this->Close();

  delete this->Internals;
  this->Internals = nullptr;
  this->SetFileName(nullptr);
}

//----------------------------------------------------------------------------
bool vtkADIOS2DataArrayWriter::Open()
{
  auto& internals = *(this->Internals);
  internals.AdiosIO = internals.Adios->DeclareIO("vtkADIOS2ArrayWriter");
  internals.AdiosIO.SetEngine(internals.WRITER_TYPES.at(this->GetWriterType()));
  try
  {
    internals.BpWriter = internals.AdiosIO.Open(this->FileName, adios2::Mode::Write);
  }
  catch (std::exception& ex)
  {
    vtkLogF(ERROR, "Exception opening ADIOS file\n%s", ex.what());
    return false;
  }

  if (!internals.BpWriter)
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
void vtkADIOS2DataArrayWriter::Close()
{
  auto& internals = *(this->Internals);
  if (internals.BpWriter)
  {
    try
    {
      internals.BpWriter.Close();
    }
    catch (std::exception& ex)
    {
      vtkLogF(ERROR, "Exception raised when closing ADIOS file\n%s", ex.what());
    }
  }
}

//----------------------------------------------------------------------------
void vtkADIOS2DataArrayWriter::Abort()
{
  this->Close();
  vtksys::SystemTools::RemoveADirectory(this->GetFileName());
}

//----------------------------------------------------------------------------
void vtkADIOS2DataArrayWriter::AddArray(vtkDataArray* array, const char* arrayname)
{
  auto& internals = *(this->Internals);
  if (!internals.BpWriter)
  {
    return;
  }
  // TODO MPI
#if VTK_MODULE_ENABLE_VTK_ParallelMPI
  vtkMultiProcessController* controller = vtkMultiProcessController::GetGlobalController();
  if (!controller)
  {
    vtkLogF(ERROR, "Global MPI controller is not defined!");
    return;
  }
  size_t rank = controller->GetLocalProcessId();
  size_t worldSize = controller->GetNumberOfProcesses();
  size_t localSize = array->GetNumberOfTuples() * array->GetNumberOfComponents();

  // need to use AllGather since vtkMultiProcessController does not support MPI_Scan equivalent yet.
  std::vector<size_t> gatheredResult(worldSize);
  controller->AllGather(&localSize, gatheredResult.data(), 1);
  size_t startIndex =
    std::accumulate(gatheredResult.begin(), std::next(gatheredResult.begin(), rank), 0);
  size_t size = std::accumulate(gatheredResult.begin(), gatheredResult.end(), 0);
#else
  size_t startIndex = 0;
  size_t localSize = array->GetNumberOfTuples() * array->GetNumberOfComponents();
  size_t size = 1 * localSize;
#endif
  adios2::Dims shape = { size };
  adios2::Dims start = { startIndex };
  adios2::Dims count = { localSize };

  CopyArrayWorker worker(internals.AdiosIO, internals.BpWriter);

  using Dispatcher = vtkArrayDispatch::DispatchByValueType<vtkArrayDispatch::AllTypes>;

  if (!Dispatcher::Execute(array, worker, arrayname, shape, start, count, adios2::ConstantDims))
  {
    worker(array, arrayname, shape, start, count, adios2::ConstantDims);
  }
}

//----------------------------------------------------------------------------
void vtkADIOS2DataArrayWriter::AddAttribute(
  const std::string& attribute, const std::string& attributeName)
{
  auto& internals = *(this->Internals);
  if (internals.BpWriter)
  {
    internals.AdiosIO.DefineAttribute(attributeName, attribute);
  }
}

//----------------------------------------------------------------------------
bool vtkADIOS2DataArrayWriter::ArrayExists(const char* arrayname)
{
  auto& internals = *(this->Internals);
  if (internals.AdiosIO.InquireVariable(arrayname))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
bool vtkADIOS2DataArrayWriter::AttributeExists(const char* arrayname)
{
  auto& internals = *(this->Internals);
  if (internals.AdiosIO.InquireAttribute<std::string>(arrayname))
  {
    return true;
  }
  return false;
}

VTK_ABI_NAMESPACE_END
