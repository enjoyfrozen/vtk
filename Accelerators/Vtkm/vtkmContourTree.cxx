#include "vtkmContourTree.h"

#include "vtkCompositeDataSet.h"
#include "vtkDataSetAttributes.h"
#include "vtkPolyData.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLogger.h"
#include "vtkMultiProcessController.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"

#if VTK_MODULE_ENABLE_VTK_ParallelMPI
# include "vtkMPI.h"
# include "vtkMPIController.h"
# include "vtkMPICommunicator.h"
# undef DIY_NO_MPI
#else
# define DIY_NO_MPI
#endif

#include "vtkmlib/ArrayConverters.h"
#include "vtkmlib/DataSetConverters.h"

#include "vtkmContour.h"
#include "vtkmFilterPolicy.h"

#include <vtkm/filter/ContourTreeUniformAugmented.h>
#include <vtkm/worklet/contourtree_augmented/ProcessContourTree.h>

#include <algorithm>


//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkmContourTree)

//-----------------------------------------------------------------------------
vtkCxxSetObjectMacro(vtkmContourTree, Controller, vtkMultiProcessController);

//-----------------------------------------------------------------------------
vtkmContourTree::vtkmContourTree()
  : Controller(nullptr)
  , NumberOfIsoValues(3)
  , IsoValuesSelectMethod(0)
  , IsoValuesType(1)
  , ComputeNormals(true)
  , ComputeGradients(false)
  , ComputeScalars(true)
{
  this->SetController(vtkMultiProcessController::GetGlobalController());

  this->SetNumberOfInputPorts(1);
  this->SetNumberOfOutputPorts(1);

  // by default process active point scalars
  this->SetInputArrayToProcess(0,0,0,vtkDataObject::FIELD_ASSOCIATION_POINTS,
                               vtkDataSetAttributes::SCALARS);
}

//-----------------------------------------------------------------------------
vtkmContourTree::~vtkmContourTree()
{
  this->SetController(nullptr);
}

//-----------------------------------------------------------------------------
void vtkmContourTree::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  if (this->Controller)
  {
    this->Controller->PrintSelf(os, indent);
  }
}

//-----------------------------------------------------------------------------
int vtkmContourTree::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  // info->Append(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkCompositeDataSet");
  return 1;
}

//=============================================================================
namespace
{

namespace contour_tree = vtkm::worklet::contourtree_augmented;

//----------------------------------------------------------------------------
inline vtkmdiy::mpi::communicator GetDiyCommunicator(vtkMultiProcessController* controller)
{
#if VTK_MODULE_ENABLE_VTK_ParallelMPI
  vtkMPICommunicator *vtkcomm = vtkMPICommunicator::SafeDownCast(
    controller->GetCommunicator());
  return vtkmdiy::mpi::communicator(*vtkcomm->GetMPIComm()->GetHandle());
#else
  return vtkmdiy::mpi::communicator();
#endif
}

//----------------------------------------------------------------------------
inline void ComputeGlobalExtents(vtkmdiy::mpi::communicator &comm,
                                 const int lextents[6], int gextents[6])
{
  vtkm::Vec<int, 3> localExtentsMin, localExtentsMax;
  for (int i = 0; i < 3; ++i)
  {
    localExtentsMin[i] = lextents[2*i];
    localExtentsMax[i] = lextents[2*i + 1];
  }

  vtkm::Vec<int, 3> globalExtentsMin, globalExtentsMax;
  vtkmdiy::mpi::all_reduce(comm, localExtentsMin, globalExtentsMin,
                           vtkmdiy::mpi::minimum<int>());
  vtkmdiy::mpi::all_reduce(comm, localExtentsMax, globalExtentsMax,
                           vtkmdiy::mpi::maximum<int>());

  for (int i = 0; i < 3; ++i)
  {
    gextents[2*i] = globalExtentsMin[i];
    gextents[2*i + 1] = globalExtentsMax[i];
  }
}

//----------------------------------------------------------------------------
struct FilterResults
{
  contour_tree::ContourTree ContourTree;
  contour_tree::IdArrayType SortedOrder;
  vtkm::Id NumberOfIterations;
};

//----------------------------------------------------------------------------
template <typename ValueType, typename StorageTag>
contour_tree::process_contourtree_inc::Branch<ValueType>* ComputeBranchDecomposition(
  const vtkm::cont::ArrayHandle<ValueType, StorageTag>& fieldArray,
  const FilterResults& ctOut)
{
  contour_tree::IdArrayType superarcIntrinsicWeight;
  contour_tree::IdArrayType superarcDependentWeight;
  contour_tree::IdArrayType supernodeTransferWeight;
  contour_tree::IdArrayType hyperarcDependentWeight;

  contour_tree::ProcessContourTree::ComputeVolumeWeights(
    ctOut.ContourTree,
    ctOut.NumberOfIterations,
    superarcIntrinsicWeight,  // (output)
    superarcDependentWeight,  // (output)
    supernodeTransferWeight,  // (output)
    hyperarcDependentWeight); // (output)

  // compute the branch decomposition by volume
  contour_tree::IdArrayType whichBranch;
  contour_tree::IdArrayType branchMinimum;
  contour_tree::IdArrayType branchMaximum;
  contour_tree::IdArrayType branchSaddle;
  contour_tree::IdArrayType branchParent;

  contour_tree::ProcessContourTree::ComputeVolumeBranchDecomposition(
    ctOut.ContourTree,
    superarcDependentWeight,
    superarcIntrinsicWeight,
    whichBranch,               // (output)
    branchMinimum,             // (output)
    branchMaximum,             // (output)
    branchSaddle,              // (output)
    branchParent);             // (output)

  // create explicit representation of the branch decompostion from the array representation
  auto* branchRoot = contour_tree::ProcessContourTree::ComputeBranchDecomposition(
    ctOut.ContourTree.superparents,
    ctOut.ContourTree.supernodes,
    whichBranch,
    branchMinimum,
    branchMaximum,
    branchSaddle,
    branchParent,
    ctOut.SortedOrder,
    fieldArray,
    true);

  return branchRoot;
}

//----------------------------------------------------------------------------
struct IsoValuesGenerator
{
  template <typename ValueType, typename StorageTag>
  void operator()(const vtkm::cont::ArrayHandle<ValueType, StorageTag>& fieldArray,
                  const FilterResults& ctOut,
                  int numLevels,
                  int contourSelectMethod,
                  int contourType,
                  std::vector<double>& isovals) const
  {
    const auto eps = static_cast<ValueType>(1e-3f);

    auto* branchRoot = ComputeBranchDecomposition(fieldArray, ctOut);
    branchRoot->simplifyToSize(numLevels + 1, true);

    std::vector<ValueType> result;
    switch(contourSelectMethod)
    {
      default:
      case 0:
        branchRoot->getRelevantValues(contourType, eps, result);
        break;
      case 1:
      {
        contour_tree::process_contourtree_inc::PiecewiseLinearFunction<ValueType> plf;
        branchRoot->accumulateIntervals(contourType, eps, plf);
        result = plf.nLargest(numLevels);
        break;
      }
    }

    delete branchRoot;

    for (const auto& v : result)
    {
      isovals.push_back(static_cast<double>(v));
    }
  }
};

} // anonymous namespace

//-----------------------------------------------------------------------------
int vtkmContourTree::RequestData(vtkInformation* vtkNotUsed(request),
                                 vtkInformationVector** inputVector,
                                 vtkInformationVector* outputVector)
{
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  vtkImageData *input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // grab the input array to process to determine the field we want to average
  int association = this->GetInputArrayAssociation(0, inputVector);
  auto fieldArray = this->GetInputArrayToProcess(0, inputVector);
  if (association != vtkDataObject::FIELD_ASSOCIATION_POINTS ||
      fieldArray == nullptr ||
      fieldArray->GetName() == nullptr || fieldArray->GetName()[0] == '\0')
  {
    vtkErrorMacro(<< "Invalid field: Requires a point field with a valid name.");
    return 0;
  }

  const char* fieldName = fieldArray->GetName();

  // Figure out the spatial decomposition
  vtkmdiy::mpi::communicator comm = GetDiyCommunicator(this->Controller);
  vtkm::cont::EnvironmentTracker::SetCommunicator(comm);

  int lExtents[6], gExtents[6];
  input->GetExtent(lExtents);
  ComputeGlobalExtents(comm, lExtents, gExtents);

  vtkm::Id3 globalSize(gExtents[1] - gExtents[0] + 1,
                       gExtents[3] - gExtents[2] + 1,
                       gExtents[5] - gExtents[4] + 1);

  vtkm::Id3 localSize(lExtents[1] - lExtents[0] + 1,
                      lExtents[3] - lExtents[2] + 1,
                      lExtents[5] - lExtents[4] + 1);

  vtkm::Id3 localExtentOrigin(lExtents[0], lExtents[2], lExtents[4]);

  std::vector<vtkm::Id3> starts;
  vtkmdiy::mpi::all_gather(comm, localExtentOrigin, starts);

  vtkm::Id3 blocksPerDim;
  vtkm::Id3 localBlockIndex;
  std::vector<int> ijks(starts.size());
  for (int i = 0; i < 3; ++i)
  {
    ijks.clear();
    for (const auto& s : starts)
    {
      ijks.push_back(s[i]);
    }
    std::sort(ijks.begin(), ijks.end());
    auto newend = std::unique(ijks.begin(), ijks.end());
    blocksPerDim[i] = std::distance(ijks.begin(), newend);
    localBlockIndex[i] =
      std::distance(ijks.begin(), std::find(ijks.begin(), newend, localExtentOrigin[i]));
  }

  vtkLog(INFO, "blocksPerDim: " << blocksPerDim << "\n" <<
               "globalSize: " << globalSize << "\n" <<
               "localBlockIndex: " << localBlockIndex << "\n" <<
               "localExtentOrigin: " << localExtentOrigin << "\n" <<
               "localSize: " << localSize << "\n");

  try
  {
    // convert the input dataset
    vtkm::cont::PartitionedDataSet vtkmInput;
    auto block = tovtkm::Convert(input);
    auto field = tovtkm::Convert(fieldArray, association);
    block.AddField(field);
    vtkmInput.AppendPartition(block);

    // apply the filter
    vtkm::filter::ContourTreePPP2 filter;
    filter.SetActiveField(fieldName);
    filter.SetSpatialDecomposition(
      blocksPerDim,
      globalSize,
      vtkm::cont::make_ArrayHandle(&localBlockIndex, 1),
      vtkm::cont::make_ArrayHandle(&localExtentOrigin, 1),
      vtkm::cont::make_ArrayHandle(&localSize, 1));

    auto resultDs = filter.Execute(vtkmInput);

    // generate the iso-values, rank 0 has the final, complete contour tree
    if (comm.rank() == 0)
    {
      FilterResults fr;
      fr.ContourTree = filter.GetContourTree();
      fr.SortedOrder = filter.GetSortOrder();
      fr.NumberOfIterations = filter.GetNumIterations();

      using FieldTypes =
        typename vtkm::filter::FilterTraits<vtkm::filter::ContourTreePPP2>::InputFieldTypeList;

      this->IsoValues.clear();
      auto dataField = resultDs.GetPartition(0).GetField(0);
      vtkm::cont::CastAndCall(dataField.GetData().ResetTypes<FieldTypes>(),
                              IsoValuesGenerator{},
                              fr,
                              this->NumberOfIsoValues,
                              this->IsoValuesSelectMethod,
                              this->IsoValuesType,
                              this->IsoValues);

      if (vtkLogger::IsEnabled() && !this->IsoValues.empty())
      {
        auto mesg = std::to_string(this->IsoValues[0]);
        for (std::size_t i = 1; i < this->IsoValues.size(); ++i)
        {
          mesg += ", " + std::to_string(this->IsoValues[i]);
        }
        vtkLog(INFO, "isoValues: " << mesg);
      }

      // select only unique values
      std::sort(this->IsoValues.begin(), this->IsoValues.end());
      auto newend = std::unique(this->IsoValues.begin(), this->IsoValues.end());
      this->IsoValues.resize(std::distance(this->IsoValues.begin(), newend));
    }
  }
  catch (const vtkm::cont::Error& e)
  {
    vtkErrorMacro(<< "VTK-m error: " << e.GetMessage());
    return 0;
  }

  // broadcast isoValues to all the nodes
  vtkmdiy::mpi::broadcast(comm, this->IsoValues, 0);

  // Run the iso-surface filter
  vtkContourFilter* contourFilter =
    ((globalSize[0] > 1) && (globalSize[1] > 1) && (globalSize[2] > 1)) ?
    static_cast<vtkContourFilter*>(vtkmContour::New()) : // currently only support 3D
    vtkContourFilter::New();

  contourFilter->SetComputeScalars(this->ComputeScalars);
  contourFilter->SetComputeGradients(this->ComputeGradients);
  contourFilter->SetComputeNormals(this->ComputeNormals);
  for (size_t i = 0; i < this->IsoValues.size(); ++i)
  {
    contourFilter->SetValue(static_cast<int>(i), this->IsoValues[i]);
  }
  contourFilter->SetInputData(input);
  contourFilter->SetInputArrayToProcess(0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, fieldName);
  contourFilter->Update();

  output->ShallowCopy(contourFilter->GetOutput());
  contourFilter->FastDelete();

  return 1;
}

//----------------------------------------------------------------------------
namespace mangled_diy_namespace {

namespace mpi {
namespace detail {

template <class T, vtkm::IdComponent NumComps>
struct mpi_datatype<vtkm::Vec<T, NumComps>>
{
  using VecT = vtkm::Vec<T, NumComps>;

  static MPI_Datatype datatype() { return get_mpi_datatype<T>(); }
  static const void* address(const VecT& x) { return &x[0]; }
  static void* address(VecT& x) { return &x[0]; }
  static int count(const VecT&) { return NumComps; }
};

}
} // namespace mpi::detail
} // mangled_diy_namespace
