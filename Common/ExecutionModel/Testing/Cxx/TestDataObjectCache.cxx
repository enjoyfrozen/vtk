#include <array>

#include "vtkCellArray.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkDataObjectCache.h"
#include "vtkExecutive.h"
#include "vtkInformation.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkPolyData.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkSmartPointer.h"
#include "vtkTestUtilities.h"
#include "vtkWeakPointer.h"

#define VTK_SUCCESS 0
#define VTK_FAILURE 1

// -------------------------------------------------------------------------------------------------
struct ObjectTimeStamp
{
  ObjectTimeStamp() noexcept = default;
  explicit ObjectTimeStamp(vtkDataObject* obj_)
    : obj(obj_)
    , mtime(obj_ ? obj_->GetMTime() : vtkMTimeType{})
  {
  }
  vtkWeakPointer<vtkDataObject> obj;
  vtkMTimeType mtime = {};
};

bool operator==(const ObjectTimeStamp& lhs, const ObjectTimeStamp& rhs)
{
  return lhs.obj == rhs.obj && lhs.mtime == rhs.mtime;
}

bool operator!=(const ObjectTimeStamp& lhs, const ObjectTimeStamp& rhs)
{
  return !operator==(lhs, rhs);
}
std::ostream& operator<<(std::ostream& stream, ObjectTimeStamp objmtime)
{
  if (objmtime.obj)
    stream << objmtime.obj << "@" << objmtime.mtime;
  else if (objmtime.mtime)
    stream << "<deleted>@" << objmtime.mtime;
  else
    stream << "<nullptr>";
  return stream;
}

// Test the timestamp comparison functions
int TestObjectTimeStamp()
{
  int errors = 0;
  auto obj1 = vtkSmartPointer<vtkPolyData>::New();
  auto obj2 = vtkSmartPointer<vtkPolyData>::New();
  ObjectTimeStamp ts1(obj1);
  ObjectTimeStamp ts2(obj2);
  ObjectTimeStamp ts3 = ts1;
  ts3.mtime++;
  errors += (ts1 != ObjectTimeStamp(obj1));
  errors += (ts1 == ObjectTimeStamp(obj2));
  errors += (ts1 == ts3);
  errors += (ObjectTimeStamp{}.obj != nullptr);
  errors += (ObjectTimeStamp{}.mtime != 0);
  return errors;
}

// -------------------------------------------------------------------------------------------------
vtkSmartPointer<vtkMultiBlockDataSet> CreateMultiBlockDataSet(int numberOfBlocks)
{
  auto mbd = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mbd->SetNumberOfBlocks(numberOfBlocks);
  for (int i = 0; i < numberOfBlocks; ++i)
  {
    mbd->SetBlock(i, vtkSmartPointer<vtkPolyData>::New());
  }
  return mbd;
}

// Get the non-composite blocks from the dataobject and create timestamps
std::vector<ObjectTimeStamp> GetBlockTimeStamps(vtkDataObject* obj)
{
  std::vector<ObjectTimeStamp> result;
  auto mbd = vtkMultiBlockDataSet::SafeDownCast(obj);
  if (mbd)
  {
    for (int i = 0; i < mbd->GetNumberOfBlocks(); ++i)
    {
      result.emplace_back(mbd->GetBlock(i));
    }
  }
  else
    result.emplace_back(obj);
  return result;
}

// -------------------------------------------------------------------------------------------------
// This struct has helper methods for testing the vtkDataObjectCache
struct DataObjectCacheTestHelper
{
  vtkDataObjectCache* subject = nullptr;
  std::string message = "DataObjectCacheTestHelper";
  int errors = 0;

  void expectContains(ObjectTimeStamp expected)
  {
    if (!subject->Contains(expected.obj))
    {
      std::cerr << message << " unexpectedly does not contain " << expected << "\n";
      ++errors;
    }
  }

  void expectFindEquals(vtkDataObject* obj, ObjectTimeStamp expected)
  {
    auto actual = ObjectTimeStamp{ subject->FindObject(obj) };
    if (actual != expected)
    {
      std::cerr << message << " FindObject for " << obj << " unexpectedly found " << actual
                << " expected " << expected << "\n";
      ++errors;
    }
  }
};

int TestDataObjectCache()
{
  DataObjectCacheTestHelper test;

  auto cache = vtkSmartPointer<vtkDataObjectCache>::New();
  if (!cache)
  {
    std::cerr << "vtkDataObjectCache::New failed";
    ++test.errors;
  }
  test.subject = cache;

  auto mbd1 = CreateMultiBlockDataSet(2);
  auto mbd2 = CreateMultiBlockDataSet(2);
  auto blocks1 = GetBlockTimeStamps(mbd1);
  auto blocks2 = GetBlockTimeStamps(mbd2);

  cache->Update(mbd1);
  test.message = "After Update";
  test.expectContains(blocks1[0]);
  test.expectContains(blocks1[1]);

  test.message = "After Finalize";
  cache->Finalize(mbd1, mbd2);
  test.expectFindEquals(blocks1[0].obj, blocks2[0]);
  test.expectFindEquals(blocks1[0].obj, blocks2[0]);

  blocks1[1].obj->Modified();

  cache->Update(mbd1);
  test.message = "After Modified+Update";
  test.expectContains(blocks1[0]);
  test.expectContains(blocks1[1]);
  test.expectFindEquals(blocks1[0].obj, blocks2[0]);
  test.expectFindEquals(blocks1[1].obj, ObjectTimeStamp{});

  if (test.errors)
  {
    std::cerr << "TestDataObjectCache: " << test.errors << " errors\n";
  }
  return test.errors;
}

// ----------------------------------------------------------------------------
// This algorithm has 2 output ports, and only counts the RequestData calls
class vtkTestAlgorithm : public vtkPolyDataAlgorithm
{
public:
  static vtkTestAlgorithm* New();
  vtkTestAlgorithm(const vtkTestAlgorithm&) = delete;
  void operator=(const vtkTestAlgorithm&) = delete;

  vtkTypeMacro(vtkTestAlgorithm, vtkPolyDataAlgorithm);

public:
  int count = 0;

protected:
  vtkTestAlgorithm()
  {
    this->SetNumberOfInputPorts(2);
    this->SetNumberOfOutputPorts(2);
  }

  int FillInputPortInformation(int port, vtkInformation* info) override
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
    if (port == 1)
      info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
    return 1;
  }

  int RequestData(vtkInformation* vtkNotUsed(request), vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override
  {
    ++count;
    return 1;
  }
};

vtkStandardNewMacro(vtkTestAlgorithm);

// -------------------------------------------------------------------------------------------------
// This struct has helper methods for testing the execution of the vtkTestAlgorithm and
// the output data structures.
struct AlgorithmTestHelper
{
  vtkTestAlgorithm* subject = nullptr;
  std::string message = "AlgorithmTestHelper";
  int errors = 0;

  void expectCount(int expected)
  {
    if (subject->count != expected)
    {
      std::cerr << message << " expectAndResetCount fails: expected " << expected << " actual "
                << subject->count << "\n";
      ++errors;
    }
  }

  void expectAndResetCount(int expected)
  {
    expectCount(expected);
    subject->count = 0;
  }

  void expectOutputEqual(int port, ObjectTimeStamp expected)
  {
    auto actual = ObjectTimeStamp(subject->GetOutput(port));
    if (expected != actual)
    {
      std::cerr << message << " expectOutputEqual fails for output on port " << port
                << ": expected " << expected << " actual " << actual << "\n";
      ++errors;
    }
  }

  void expectOutputNotEqual(int port, ObjectTimeStamp expected)
  {
    auto actual = ObjectTimeStamp(subject->GetOutput(port));
    if (expected == actual)
    {
      std::cerr << message << " expectOutputNotEqual fails for output on port " << port
                << ": expected " << expected << " actual " << actual << "\n";
      ++errors;
    }
  }

  void expectBlockEqual(int port, int blockNo, ObjectTimeStamp expected)
  {
    auto mbd = vtkMultiBlockDataSet::SafeDownCast(subject->GetOutputDataObject(port));
    ObjectTimeStamp actual = mbd ? ObjectTimeStamp{ mbd->GetBlock(blockNo) } : ObjectTimeStamp{};
    if (actual != expected)
    {
      std::cerr << message << " expectBlockEqual fails for block #" << blockNo << " on port "
                << port << ": expected " << expected << " actual " << actual << "\n";
      ++errors;
    }
  }

  void expectBlockNotEqual(int port, int blockNo, ObjectTimeStamp expected)
  {
    auto mbd = vtkMultiBlockDataSet::SafeDownCast(subject->GetOutputDataObject(port));
    ObjectTimeStamp actual = mbd ? ObjectTimeStamp{ mbd->GetBlock(blockNo) } : ObjectTimeStamp{};
    if (actual == expected)
    {
      std::cerr << message << " expectBlockNotEqual fails for block #" << blockNo << " on port "
                << port << ": expected " << expected << " actual " << actual << "\n";
      ++errors;
    }
  }
};

int TestPolyDataInput()
{
  int errors = 0;
  auto poly = vtkSmartPointer<vtkPolyData>::New();

  auto filter = vtkSmartPointer<vtkTestAlgorithm>::New();
  filter->SetInputDataObject(0, poly);

  AlgorithmTestHelper test;
  test.subject = filter;

  // Update to create initial outputs
  filter->Update();
  const ObjectTimeStamp initial_out0{ filter->GetOutput(0) };
  const ObjectTimeStamp initial_out1{ filter->GetOutput(1) };
  filter->Update();
  test.message = "input polydata not modified";
  test.expectAndResetCount(1);
  test.expectOutputEqual(0, initial_out0);
  test.expectOutputEqual(1, initial_out1);

  // second update with input modified
  poly->Modified();
  filter->Update();
  test.message = "input polydata modified";
  test.expectAndResetCount(1);
  test.expectOutputNotEqual(0, initial_out0);
  test.expectOutputNotEqual(1, initial_out1);

  if (test.errors)
  {
    std::cerr << "TestPolyDataInput: " << test.errors << " errors\n";
  }
  return errors;
}

int TestMultiBlockDataSetInput()
{
  auto mbd = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  mbd->SetNumberOfBlocks(2);
  mbd->SetBlock(0, vtkSmartPointer<vtkPolyData>::New());
  mbd->SetBlock(1, vtkSmartPointer<vtkPolyData>::New());

  auto filter = vtkSmartPointer<vtkTestAlgorithm>::New();
  filter->GetExecutive()->DebugOn();
  AlgorithmTestHelper test;
  test.subject = filter;

  filter->SetInputDataObject(0, mbd);

  // second update with all inputs unmodified
  test.message = "initial";
  filter->Update();
  test.expectAndResetCount(2);

  const auto initial_out0 = GetBlockTimeStamps(filter->GetOutputDataObject(0));
  const auto initial_out1 = GetBlockTimeStamps(filter->GetOutputDataObject(1));

  test.message = "Update unmodified";
  filter->Update();
  test.expectAndResetCount(0);
  test.expectBlockEqual(0, 0, initial_out0[0]);
  test.expectBlockEqual(0, 1, initial_out0[1]);
  test.expectBlockEqual(1, 0, initial_out1[0]);
  test.expectBlockEqual(1, 1, initial_out1[1]);

  test.message = "Update block[1] modified";
  mbd->GetBlock(1)->Modified();
  mbd->Modified();
  filter->Update();
  test.expectAndResetCount(1);
  test.expectBlockEqual(0, 0, initial_out0[0]);
  test.expectBlockNotEqual(0, 1, initial_out0[1]);
  test.expectBlockEqual(1, 0, initial_out1[0]);
  test.expectBlockNotEqual(1, 1, initial_out1[1]);

  auto out0 = GetBlockTimeStamps(filter->GetOutputDataObject(0));
  auto out1 = GetBlockTimeStamps(filter->GetOutputDataObject(1));

  test.message = "add block";
  mbd->SetNumberOfBlocks(3);
  mbd->SetBlock(2, vtkSmartPointer<vtkPolyData>::New());
  filter->Update();
  test.expectAndResetCount(1);
  for (int port = 0; port < 2; ++port)
  {
    auto& out = port == 0 ? out0 : out1;
    for (int block = 0; block < 2; ++block)
      test.expectBlockEqual(port, block, out[block]);
  }

  out0 = GetBlockTimeStamps(filter->GetOutputDataObject(0));
  out1 = GetBlockTimeStamps(filter->GetOutputDataObject(1));

  if (out0.size() != 3 || out1.size() != 3)
  {
    std::cerr << "Expected 3 blocks after adding a block but found" << out0.size() << "\n";
    ++test.errors;
  }

  test.message = "reset the first block";
  mbd->SetBlock(0, nullptr);
  filter->Update();
  test.expectAndResetCount(0); // no changed block to process
  for (int port = 0; port < 2; ++port)
  {
    auto& out = port == 0 ? out0 : out1;
    test.expectBlockEqual(port, 0, ObjectTimeStamp{}); // block 0 was reset
    test.expectBlockEqual(port, 1, out[1]);
    test.expectBlockEqual(port, 2, out[2]);
  }

  test.message = "last block duplicated";
  mbd->SetBlock(0, mbd->GetBlock(1)); // moved to lower index
  mbd->SetBlock(1, mbd->GetBlock(2)); // copied to lower index
  filter->Update();
  test.expectAndResetCount(0); // no changed block to process
  for (int port = 0; port < 2; ++port)
  {
    auto& out = port == 0 ? out0 : out1;
    test.expectBlockEqual(port, 0, out[1]);
    test.expectBlockEqual(port, 1, out[2]);
    test.expectBlockEqual(port, 2, out[2]);
  }
  filter->Modified();
  filter->Update();
  test.message = "Filter modified";
  test.expectAndResetCount(3); // filter itself is changed so all blocks are updated

  test.message = "Second input connected";
  auto input2 = vtkSmartPointer<vtkPolyData>::New();
  filter->SetInputDataObject(1, input2);
  filter->Update();
  test.expectAndResetCount(3); // non-composite second input changed so all blocks are updated

  test.message = "new last block";
  mbd->SetBlock(2, vtkSmartPointer<vtkPolyData>::New());
  filter->Update();
  test.expectAndResetCount(1); // no changed block to process
  out0 = GetBlockTimeStamps(filter->GetOutputDataObject(0));
  out1 = GetBlockTimeStamps(filter->GetOutputDataObject(1));

  test.message = "Second input updated";
  input2->Modified();
  filter->Update();
  test.expectAndResetCount(3); // non-composite second input changed so all blocks are updated

  test.message = "Input release data flag set";
  filter->GetInputInformation()->Set(vtkStreamingDemandDrivenPipeline::RELEASE_DATA(), 1);
  mbd->Modified();
  filter->Update();
  test.expectAndResetCount(3); // three blocks => three calls
  if (mbd->GetNumberOfBlocks() > 0)
  {
    std::cerr << "Expected vtkMultiBlockDataSet ReleaseData to reset the number of blocks";
    test.errors += 1;
  }

  if (test.errors)
  {
    std::cerr << "TestMultiBlockDataSetInput: " << test.errors << " errors\n";
  }
  return test.errors;
}

int TestDataObjectCache(int argc, char* argv[])
{
  //  vtkCompositeDataPipeline::SetGlobalDataCachingEnabled(false);

  int errors = 0;
  errors += TestObjectTimeStamp();
  errors += TestDataObjectCache();
  errors += TestPolyDataInput();
  errors += TestMultiBlockDataSetInput();
  return errors == 0 ? VTK_SUCCESS : VTK_FAILURE;
}
