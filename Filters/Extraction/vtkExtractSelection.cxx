/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractSelection.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkExtractSelection.h"

#include "vtkBlockSelector.h"
#include "vtkCell.h"
#include "vtkCellData.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataObject.h"
#include "vtkDataObjectTree.h"
#include "vtkDataSet.h"
#include "vtkExtractCells.h"
#include "vtkFrustumSelector.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLocationSelector.h"
#include "vtkLogger.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkSMPTools.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSelector.h"
#include "vtkSignedCharArray.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTable.h"
#include "vtkUniformGridAMRDataIterator.h"
#include "vtkUnstructuredGrid.h"
#include "vtkValueSelector.h"

#include <cassert>
#include <map>
#include <numeric>
#include <vector>

static constexpr unsigned char MASKED_CELL_VALUE = vtkDataSetAttributes::HIDDENCELL |
  vtkDataSetAttributes::DUPLICATECELL | vtkDataSetAttributes::REFINEDCELL;

static constexpr unsigned char MASKED_POINT_VALUE = vtkDataSetAttributes::HIDDENPOINT;

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkExtractSelection);
//------------------------------------------------------------------------------
vtkExtractSelection::vtkExtractSelection()
{
  this->SetNumberOfInputPorts(2);
}

//------------------------------------------------------------------------------
vtkExtractSelection::~vtkExtractSelection() = default;

//------------------------------------------------------------------------------
int vtkExtractSelection::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
  }
  else
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkSelection");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  }
  return 1;
}

//------------------------------------------------------------------------------
int vtkExtractSelection::RequestDataObject(
  vtkInformation*, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  auto inputDO = vtkDataObject::GetData(inputVector[0], 0);
  if (!inputDO)
  {
    return 0;
  }

  const int inputType = inputDO->GetDataObjectType();
  int outputType = -1;

  if (this->PreserveTopology)
  {
    // when PreserveTopology is ON, we preserve input data type.
    outputType = inputType;
  }
  else if (vtkDataObjectTree::SafeDownCast(inputDO))
  {
    // For DataObjectTree, preserve the type.
    outputType = inputType;
  }
  else if (vtkCompositeDataSet::SafeDownCast(inputDO))
  {
    // For other composite datasets, we create a vtkMultiBlockDataSet as output;
    outputType = VTK_MULTIBLOCK_DATA_SET;
  }
  else if (vtkDataSet::SafeDownCast(inputDO))
  {
    // vtkDataSet becomes a vtkUnstructuredGrid.
    outputType = VTK_UNSTRUCTURED_GRID;
  }
  else
  {
    // preserve input type for the rest e.g. vtkTable, vtkGraph etc.
    outputType = inputType;
  }

  auto outInfo = outputVector->GetInformationObject(0);
  if (outputType != -1 &&
    vtkDataObjectAlgorithm::SetOutputDataObject(outputType, outInfo, /*exact=*/true))
  {
    return 1;
  }

  vtkErrorMacro("Not sure what type of output to create!");
  return 0;
}

//------------------------------------------------------------------------------
vtkDataObject::AttributeTypes vtkExtractSelection::GetAttributeTypeOfSelection(
  vtkSelection* sel, bool& sane)
{
  sane = true;
  int fieldType = -1;
  for (unsigned int n = 0; n < sel->GetNumberOfNodes(); n++)
  {
    vtkSelectionNode* node = sel->GetNode(n);

    int nodeFieldType = node->GetFieldType();

    if (nodeFieldType == vtkSelectionNode::POINT &&
      node->GetProperties()->Has(vtkSelectionNode::CONTAINING_CELLS()) &&
      node->GetProperties()->Get(vtkSelectionNode::CONTAINING_CELLS()))
    {
      // we're really selecting cells, not points.
      nodeFieldType = vtkSelectionNode::CELL;
    }

    if (n != 0 && fieldType != nodeFieldType)
    {
      sane = false;
      vtkErrorMacro("Selection contains mismatched attribute types!");
      return vtkDataObject::NUMBER_OF_ATTRIBUTE_TYPES;
    }

    fieldType = nodeFieldType;
  }

  return fieldType == -1 ? vtkDataObject::NUMBER_OF_ATTRIBUTE_TYPES
                         : static_cast<vtkDataObject::AttributeTypes>(
                             vtkSelectionNode::ConvertSelectionFieldToAttributeType(fieldType));
}

namespace
{
//----------------------------------------------------------------------------
void InvertSelection(vtkSignedCharArray* array)
{
  const vtkIdType n = array->GetNumberOfTuples();
  vtkSMPTools::For(0, n, [&array](vtkIdType start, vtkIdType end) {
    for (vtkIdType i = start; i < end; ++i)
    {
      array->SetValue(i, static_cast<signed char>(array->GetValue(i) * -1 + 1));
    }
  });
}

//----------------------------------------------------------------------------
// Remove all selection nodes that their propId = vtkSelectionNode::PROCESS_ID()
// is not the same as the processId = vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER().
void vtkTrimSelection(vtkSelection* input, int processId)
{
  if (input)
  {
    unsigned int numNodes = input->GetNumberOfNodes();
    for (unsigned int cc = 0; cc < numNodes; cc++)
    {
      vtkSelectionNode* node = input->GetNode(cc);
      int propId = (node->GetProperties()->Has(vtkSelectionNode::PROCESS_ID()))
        ? node->GetProperties()->Get(vtkSelectionNode::PROCESS_ID())
        : -1;
      if (propId != -1 && processId != -1 && propId != processId)
      {
        input->RemoveNode(node);
      }
    }
  }
}
}

//------------------------------------------------------------------------------
int vtkExtractSelection::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  outInfo->Set(CAN_HANDLE_PIECE_REQUEST(), 1);
  return 1;
}

//------------------------------------------------------------------------------
int vtkExtractSelection::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkDataObject* input = vtkDataObject::GetData(inputVector[0], 0);
  vtkSelection* selection = vtkSelection::GetData(inputVector[1], 0);
  vtkDataObject* output = vtkDataObject::GetData(outputVector, 0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // If no input, error
  if (!input)
  {
    vtkErrorMacro(<< "No input specified");
    return 0;
  }

  // If no selection, quietly select nothing
  if (!selection)
  {
    return 1;
  }

  // preserve only nodes that their processId matches the current processId
  if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER()))
  {
    int processId = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_PIECE_NUMBER());
    vtkTrimSelection(selection, processId);
  }

  // check for empty selection
  if (selection->GetNumberOfNodes() == 0)
  {
    return 1;
  }

  // check for select FieldType consistency right here and return failure
  // if they are not consistent.
  bool sane;
  const auto assoc = this->GetAttributeTypeOfSelection(selection, sane);
  if (!sane || assoc == vtkDataObject::NUMBER_OF_ATTRIBUTE_TYPES)
  {
    vtkErrorMacro("Selection has selection nodes with inconsistent field types.");
    return 0;
  }

  // Create operators for each of vtkSelectionNode instances and initialize them.
  std::map<std::string, vtkSmartPointer<vtkSelector>> selectors;
  for (unsigned int cc = 0, max = selection->GetNumberOfNodes(); cc < max; ++cc)
  {
    auto node = selection->GetNode(cc);
    auto name = selection->GetNodeNameAtIndex(cc);

    if (auto anOperator = this->NewSelectionOperator(
          static_cast<vtkSelectionNode::SelectionContent>(node->GetContentType())))
    {
      anOperator->SetInsidednessArrayName(name);
      anOperator->Initialize(node);
      selectors[name] = anOperator;
    }
    else
    {
      vtkWarningMacro("Unhandled selection node with content type : " << node->GetContentType());
    }
  }

  // Check if vtkSelector::ExpandToConnectedElements will be used.
  // This is useful because we can omit shallow copy of the input data.
  bool expandToConnectedElements = false;
  for (unsigned int cc = 0, max = selection->GetNumberOfNodes(); cc < max; ++cc)
  {
    auto node = selection->GetNode(cc);
    int association = vtkSelectionNode::ConvertSelectionFieldToAttributeType(node->GetFieldType());
    const int layers = node->GetProperties()->Get(vtkSelectionNode::CONNECTED_LAYERS());

    if (layers >= 1 && (association == vtkDataObject::POINT || association == vtkDataObject::CELL))
    {
      expandToConnectedElements = true;
      break;
    }
  }

  if (auto inputCD = vtkCompositeDataSet::SafeDownCast(input))
  {
    auto outputCD = vtkCompositeDataSet::SafeDownCast(output);
    assert(outputCD != nullptr);
    outputCD->CopyStructure(inputCD);

    vtkSmartPointer<vtkCompositeDataIterator> inIter;
    inIter.TakeReference(inputCD->NewIterator());

    // Initialize the output composite dataset to have blocks with the same type
    // as the input.
    for (inIter->InitTraversal(); !inIter->IsDoneWithTraversal(); inIter->GoToNextItem())
    {
      if (this->CheckAbort())
      {
        break;
      }
      auto blockInput = inIter->GetCurrentDataObject();
      if (blockInput)
      {
        vtkSmartPointer<vtkDataObject> clone;
        if (expandToConnectedElements || this->PreserveTopology)
        {
          clone.TakeReference(blockInput->NewInstance());
          clone->ShallowCopy(blockInput);
        }
        else
        {
          if (assoc != vtkDataObject::ROW)
          {
            clone.TakeReference(vtkUnstructuredGrid::New());
          }
          else
          {
            clone.TakeReference(vtkTable::New());
          }
        }
        outputCD->SetDataSet(inIter, clone);
      }
    }

    // Evaluate the operators.
    vtkLogStartScope(TRACE, "execute selectors");
    for (const auto& nodeSelector : selectors)
    {
      if (this->CheckAbort())
      {
        break;
      }
      nodeSelector.second->Execute(inputCD, outputCD);
    }
    vtkLogEndScope("execute selectors");

    vtkLogStartScope(TRACE, "evaluate expression and extract output");
    // Now iterate again over the composite dataset and evaluate the expression to
    // combine all the insidedness arrays and then extract the elements.
    vtkSmartPointer<vtkCompositeDataIterator> outIter;
    outIter.TakeReference(outputCD->NewIterator());
    bool globalEvaluationResult = true;
    // input iterator is needed because if inputCD is subclass of vtkUniformGridAMR,
    // GetDataSet requires the iterator to be vtkUniformGridAMRDataIterator
    vtkTypeBool isUniformGridAMR = outputCD->IsA("vtkUniformGridAMR");
    if (isUniformGridAMR)
    {
      inIter->GoToFirstItem();
    }
    for (outIter->GoToFirstItem(); !outIter->IsDoneWithTraversal(); outIter->GoToNextItem())
    {
      if (this->CheckAbort())
      {
        break;
      }
      auto outputBlock = outIter->GetCurrentDataObject();
      if (outputBlock)
      {
        // Evaluate the expression.
        auto evaluationResult = this->EvaluateSelection(outputBlock, assoc, selection, selectors);
        if (evaluationResult != EvaluationResult::INVALID)
        {
          // Extract the elements.
          auto iter = isUniformGridAMR ? inIter : outIter;
          auto extractResult =
            this->ExtractElements(inputCD->GetDataSet(iter), assoc, evaluationResult, outputBlock);
          outputCD->SetDataSet(outIter, extractResult);
        }
        else
        {
          globalEvaluationResult = false;
          break;
        }
      }
      if (isUniformGridAMR)
      {
        inIter->GoToNextItem();
      }
    }
    vtkLogEndScope("evaluate expression and extract output");
    // check for evaluate result errors
    if (!globalEvaluationResult)
    {
      // If the expression evaluation failed, then we need to set all the blocks to null.
      for (outIter->GoToFirstItem(); !outIter->IsDoneWithTraversal(); outIter->GoToNextItem())
      {
        outputCD->SetDataSet(outIter, nullptr);
      }
      return 0;
    }
  }
  else
  {
    assert(output != nullptr);

    vtkSmartPointer<vtkDataObject> clone;
    if (expandToConnectedElements || this->PreserveTopology)
    {
      clone.TakeReference(input->NewInstance());
      clone->ShallowCopy(input);
    }
    else
    {
      if (assoc != vtkDataObject::ROW)
      {
        clone.TakeReference(vtkUnstructuredGrid::New());
      }
      else
      {
        clone.TakeReference(vtkTable::New());
      }
    }

    // Evaluate the operators.
    vtkLogStartScope(TRACE, "execute selectors");
    for (const auto& nodeSelector : selectors)
    {
      if (this->CheckAbort())
      {
        break;
      }
      nodeSelector.second->Execute(input, clone);
    }
    vtkLogEndScope("execute selectors");

    vtkLogStartScope(TRACE, "evaluate expression");
    auto evaluateResult = this->EvaluateSelection(clone, assoc, selection, selectors);
    vtkLogEndScope("evaluate expression");
    // check for evaluate result errors
    if (evaluateResult == EvaluationResult::INVALID)
    {
      output->Initialize();
      return 0;
    }

    vtkLogStartScope(TRACE, "extract output");
    if (auto extractResult = this->ExtractElements(input, assoc, evaluateResult, clone))
    {
      output->ShallowCopy(extractResult);
    }
    vtkLogEndScope("extract output");
  }

  return 1;
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkSelector> vtkExtractSelection::NewSelectionOperator(
  vtkSelectionNode::SelectionContent contentType)
{
  switch (contentType)
  {
    case vtkSelectionNode::GLOBALIDS:
    case vtkSelectionNode::PEDIGREEIDS:
    case vtkSelectionNode::VALUES:
    case vtkSelectionNode::INDICES:
    case vtkSelectionNode::THRESHOLDS:
      return vtkSmartPointer<vtkValueSelector>::New();

    case vtkSelectionNode::FRUSTUM:
      return vtkSmartPointer<vtkFrustumSelector>::New();

    case vtkSelectionNode::LOCATIONS:
      return vtkSmartPointer<vtkLocationSelector>::New();

    case vtkSelectionNode::BLOCKS:
    case vtkSelectionNode::BLOCK_SELECTORS:
      return vtkSmartPointer<vtkBlockSelector>::New();

    case vtkSelectionNode::USER:
    case vtkSelectionNode::QUERY:
    default:
      return nullptr;
  }
}

//------------------------------------------------------------------------------
vtkExtractSelection::EvaluationResult vtkExtractSelection::EvaluateSelection(
  vtkDataObject* dataObject, vtkDataObject::AttributeTypes association, vtkSelection* selection,
  std::map<std::string, vtkSmartPointer<vtkSelector>>& selectors)
{
  auto fieldData = dataObject->GetAttributes(association);
  if (!fieldData)
  {
    return EvaluationResult::NONE;
  }

  // Iterate over operators and set up a map from selection node name to insidedness
  // array.
  std::map<std::string, vtkSignedCharArray*> arrayMap;
  for (const auto& nodeSelector : selectors)
  {
    auto name = nodeSelector.first;
    auto insidednessArray = vtkSignedCharArray::SafeDownCast(fieldData->GetArray(name.c_str()));
    auto node = selection->GetNode(name);
    if (insidednessArray != nullptr && node->GetProperties()->Has(vtkSelectionNode::INVERSE()) &&
      node->GetProperties()->Get(vtkSelectionNode::INVERSE()))
    {
      ::InvertSelection(insidednessArray);
    }
    arrayMap[name] = insidednessArray;
  }

  // Evaluate the map of insidedness arrays
  std::array<signed char, 2> range;
  auto blockInsidedness = selection->Evaluate(arrayMap, range);
  if (blockInsidedness)
  {
    blockInsidedness->SetName("__vtkInsidedness__");
    if (range[0] == 0 && range[1] == 0)
    {
      return EvaluationResult::NONE;
    }
    else if (range[0] == 1 && range[1] == 1)
    {
      fieldData->AddArray(blockInsidedness);
      return EvaluationResult::ALL;
    }
    else
    {
      fieldData->AddArray(blockInsidedness);
      return EvaluationResult::MIXED;
    }
  }
  else
  {
    return EvaluationResult::INVALID;
  }
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkDataObject> vtkExtractSelection::ExtractElements(vtkDataObject* inputBlock,
  vtkDataObject::AttributeTypes type, EvaluationResult evaluationResult, vtkDataObject* outputBlock)
{
  // Check if no work needs to be done
  vtkUnsignedCharArray* inputGhostArray = inputBlock->GetAttributes(type)->GetGhostArray();
  bool extractAll = evaluationResult == EvaluationResult::ALL;
  bool extractNone = evaluationResult == EvaluationResult::NONE;
  if (extractNone && inputGhostArray == nullptr)
  {
    // Assume nothing was selected and return.
    return nullptr;
  }

  // Check if there are no cells listed to filter
  auto fd = outputBlock->GetAttributes(type);
  vtkSmartPointer<vtkSignedCharArray> insidednessArray =
    fd ? vtkSignedCharArray::SafeDownCast(fd->GetArray("__vtkInsidedness__")) : nullptr;
  if ((!inputGhostArray || inputGhostArray->GetNumberOfTuples() <= 0) &&
    (!insidednessArray || insidednessArray->GetNumberOfTuples() <= 0))
  {
    // No ghost or insidedness arrays
    return nullptr;
  }

  // If a ghost array should be generated, create the ghost array from the selection criteria
  // instead of filtering data directly
  if (this->GenerateGhostArray &&
    (type == vtkDataObject::AttributeTypes::CELL || type == vtkDataObject::AttributeTypes::POINT))
  {
    vtkSmartPointer<vtkUnsignedCharArray> ghosts;
    if (type == vtkDataObject::AttributeTypes::CELL)
    {
      ghosts = ExtractElementGhosts(MASKED_CELL_VALUE,
        vtkDataSetAttributes::CellGhostTypes::HIDDENCELL, inputGhostArray, insidednessArray);
    }
    else
    {
      ghosts = ExtractElementGhosts(MASKED_POINT_VALUE,
        vtkDataSetAttributes::PointGhostTypes::HIDDENPOINT, inputGhostArray, insidednessArray);
    }

    outputBlock->ShallowCopy(inputBlock);
    if (ghosts != nullptr)
    {
      outputBlock->GetAttributesAsFieldData(type)->AddArray(ghosts);
    }
    if (insidednessArray != nullptr)
    {
      insidednessArray->SetName("vtkInsidedness");
      outputBlock->GetAttributesAsFieldData(type)->AddArray(insidednessArray);
    }
    return outputBlock;
  }

  // If a ghost array exists from a previous filter, setup filtering so those ghost cells can be
  // removed from the output as if they were directly filtered
  if (inputGhostArray)
  {
    if (insidednessArray == nullptr)
    {
      insidednessArray = vtkSmartPointer<vtkSignedCharArray>::New();
      insidednessArray->SetNumberOfValues(inputGhostArray->GetNumberOfValues());
      insidednessArray->FillComponent(0, 1);
    }

    if (type == vtkDataObject::AttributeTypes::CELL)
    {
      ExtractElementHideGhostsInInsidedness(MASKED_CELL_VALUE, inputGhostArray, insidednessArray);
    }
    else
    {
      ExtractElementHideGhostsInInsidedness(MASKED_POINT_VALUE, inputGhostArray, insidednessArray);
    }
  }

  // If data is to be filtered...
  vtkSmartPointer<vtkDataObject> result;
  if (this->PreserveTopology)
  {
    if (inputGhostArray != nullptr)
    {
      fd->AddArray(inputGhostArray);
    }
    if (insidednessArray != nullptr)
    {
      insidednessArray->SetName("vtkInsidedness");
      outputBlock->GetAttributesAsFieldData(type)->AddArray(insidednessArray);
    }
    result = outputBlock;
    return result;
  }
  else if (type == vtkDataObject::POINT)
  {
    vtkDataSet* input = vtkDataSet::SafeDownCast(inputBlock);
    if (!input)
    {
      return nullptr;
    }
    // if output is already a vtkUnstructuredGrid, we can use it directly
    vtkSmartPointer<vtkUnstructuredGrid> output;
    if (outputBlock->GetDataObjectType() == VTK_UNSTRUCTURED_GRID)
    {
      outputBlock->Initialize();
      output = static_cast<vtkUnstructuredGrid*>(outputBlock);
    }
    else
    {
      output = vtkSmartPointer<vtkUnstructuredGrid>::New();
    }
    this->ExtractSelectedPoints(input, output, insidednessArray, extractAll);
    result = output;
  }
  else if (type == vtkDataObject::CELL)
  {
    vtkDataSet* input = vtkDataSet::SafeDownCast(inputBlock);
    if (!input)
    {
      return nullptr;
    }
    // if output is already a vtkUnstructuredGrid, we can use it directly
    vtkSmartPointer<vtkUnstructuredGrid> output;
    if (outputBlock->GetDataObjectType() == VTK_UNSTRUCTURED_GRID)
    {
      outputBlock->Initialize();
      output = static_cast<vtkUnstructuredGrid*>(outputBlock);
    }
    else
    {
      output = vtkSmartPointer<vtkUnstructuredGrid>::New();
    }
    this->ExtractSelectedCells(input, output, insidednessArray, extractAll);
    result = output;
  }
  else if (type == vtkDataObject::ROW)
  {
    vtkTable* input = vtkTable::SafeDownCast(inputBlock);
    if (!input)
    {
      return nullptr;
    }
    // if output is already a vtkTable, we can use it directly
    vtkSmartPointer<vtkTable> output;
    if (outputBlock->GetDataObjectType() == VTK_TABLE)
    {
      outputBlock->Initialize();
      output = static_cast<vtkTable*>(outputBlock);
    }
    else
    {
      output = vtkSmartPointer<vtkTable>::New();
    }
    this->ExtractSelectedRows(input, output, insidednessArray, extractAll);
    result = output;
  }
  else
  {
    outputBlock->Initialize();
    result = outputBlock;
  }
  return result && result->GetNumberOfElements(type) > 0 ? result : nullptr;
}

//------------------------------------------------------------------------------
template <typename T, typename T2>
vtkSmartPointer<vtkUnsignedCharArray> vtkExtractSelection::ExtractElementGhosts(T hiddenMask,
  T2 hiddenValue, vtkUnsignedCharArray* inputGhostArray,
  vtkSmartPointer<vtkSignedCharArray> insidednessArray)
{
  vtkIdType size = 0;
  if (inputGhostArray != nullptr)
  {
    size = std::max(size, inputGhostArray->GetNumberOfTuples());
  }
  if (insidednessArray != nullptr)
  {
    size = std::max(size, insidednessArray->GetNumberOfTuples());
  }
  if (size <= 0)
  {
    return nullptr;
  }

  vtkNew<vtkUnsignedCharArray> ghosts;
  ghosts->SetNumberOfTuples(size);
  ghosts->FillComponent(0, 0);
  ghosts->SetName(vtkDataSetAttributes::GhostArrayName());

  if (inputGhostArray && inputGhostArray->GetSize() == size)
  {
    for (vtkIdType i = 0; i < size; i++)
    {
      if ((inputGhostArray->GetValue(i) & hiddenMask) ||
        (insidednessArray != nullptr && insidednessArray->GetValue(i) == 0))
      {
        ghosts->SetValue(i, hiddenValue);
      }
    }
  }
  else
  {
    if (inputGhostArray)
    {
      vtkErrorMacro("A ghost array exists, but is the wrong size and will be ignored");
    }

    if (insidednessArray != nullptr)
    {
      for (vtkIdType i = 0; i < size; i++)
      {
        if (insidednessArray->GetValue(i) == 0)
        {
          ghosts->SetValue(i, hiddenValue);
        }
      }
    }
  }

  return ghosts;
}

//------------------------------------------------------------------------------
template <typename T>
void vtkExtractSelection::ExtractElementHideGhostsInInsidedness(T hiddenMask,
  vtkUnsignedCharArray* inputGhostArray, vtkSmartPointer<vtkSignedCharArray> insidednessArray)
{
  vtkIdType numValues =
    std::min(inputGhostArray->GetNumberOfValues(), insidednessArray->GetNumberOfValues());
  for (vtkIdType i = 0; i < numValues; i++)
  {
    if (inputGhostArray->GetValue(i) & hiddenMask)
    {
      insidednessArray->SetValue(i, 0);
    }
  }
}

//------------------------------------------------------------------------------
void vtkExtractSelection::ExtractSelectedCells(
  vtkDataSet* input, vtkUnstructuredGrid* output, vtkSignedCharArray* cellInside, bool extractAll)
{
  vtkLogScopeF(TRACE, "ExtractSelectedCells");
  const vtkIdType numPts = input->GetNumberOfPoints();
  const vtkIdType numCells = input->GetNumberOfCells();

  // The "input" is a shallow copy of the input to this filter and hence we can
  // modify it. We add original cell ids and point ids arrays.
  vtkNew<vtkIdTypeArray> originalPointIds;
  originalPointIds->SetNumberOfComponents(1);
  originalPointIds->SetName("vtkOriginalPointIds");
  originalPointIds->SetNumberOfTuples(numPts);
  std::iota(originalPointIds->GetPointer(0), originalPointIds->GetPointer(0) + numPts, 0);
  input->GetPointData()->AddArray(originalPointIds);

  vtkNew<vtkIdTypeArray> originalCellIds;
  originalCellIds->SetNumberOfComponents(1);
  originalCellIds->SetName("vtkOriginalCellIds");
  originalCellIds->SetNumberOfTuples(numCells);
  std::iota(originalCellIds->GetPointer(0), originalCellIds->GetPointer(0) + numCells, 0);
  input->GetCellData()->AddArray(originalCellIds);

  vtkNew<vtkExtractCells> extractor;
  if (extractAll)
  {
    // all elements are selected, pass all data.
    // we still use the extractor since it does the data conversion, if needed
    extractor->SetExtractAllCells(true);
  }
  else
  {
    // convert insideness array to cell ids to extract.
    std::vector<vtkIdType> ids;
    ids.reserve(numCells);
    for (vtkIdType cc = 0; cc < numCells; ++cc)
    {
      if (cellInside->GetValue(cc) != 0)
      {
        ids.push_back(cc);
      }
    }
    extractor->SetAssumeSortedAndUniqueIds(true);
    extractor->SetCellIds(&ids.front(), static_cast<vtkIdType>(ids.size()));
  }

  extractor->SetInputDataObject(input);
  extractor->Update();
  output->ShallowCopy(extractor->GetOutput());
}

//------------------------------------------------------------------------------
void vtkExtractSelection::ExtractSelectedPoints(
  vtkDataSet* input, vtkUnstructuredGrid* output, vtkSignedCharArray* pointInside, bool extractAll)
{
  vtkIdType numPts = input->GetNumberOfPoints();

  vtkPointData* pd = input->GetPointData();
  vtkPointData* outputPD = output->GetPointData();

  // To copy points in a type agnostic way later
  auto pointSet = vtkPointSet::SafeDownCast(input);

  outputPD->SetCopyGlobalIds(1);
  outputPD->CopyFieldOff("vtkOriginalPointIds");
  outputPD->CopyAllocate(pd);

  vtkNew<vtkIdTypeArray> originalPointIds;
  originalPointIds->SetNumberOfComponents(1);
  originalPointIds->SetName("vtkOriginalPointIds");
  outputPD->AddArray(originalPointIds);

  vtkNew<vtkPoints> newPts;
  if (!extractAll)
  {
    if (pointSet)
    {
      newPts->SetDataType(pointSet->GetPoints()->GetDataType());
    }
    newPts->Allocate(numPts / 4, numPts);

    double x[3];
    for (vtkIdType ptId = 0; ptId < numPts; ++ptId)
    {
      signed char isInside;
      assert(ptId < pointInside->GetNumberOfValues());
      pointInside->GetTypedTuple(ptId, &isInside);
      if (isInside)
      {
        // copy point
        vtkIdType newPointId = -1;
        if (pointSet)
        {
          newPointId = newPts->GetNumberOfPoints();
          newPts->InsertPoints(newPointId, 1, ptId, pointSet->GetPoints());
        }
        else
        {
          input->GetPoint(ptId, x);
          newPointId = newPts->InsertNextPoint(x);
        }
        assert(newPointId >= 0);
        // copy point data
        outputPD->CopyData(pd, ptId, newPointId);
        // set original point id
        originalPointIds->InsertNextValue(ptId);
      }
    }
  }
  else
  {
    // copy points
    if (pointSet)
    {
      newPts->ShallowCopy(pointSet->GetPoints());
    }
    else
    {
      newPts->SetNumberOfPoints(numPts);
      vtkSMPTools::For(0, numPts, [&](vtkIdType beginPtId, vtkIdType endPtId) {
        double x[3];
        for (vtkIdType ptId = beginPtId; ptId < endPtId; ++ptId)
        {
          input->GetPoint(ptId, x);
          newPts->SetPoint(ptId, x);
        }
      });
    }
    // copy point data
    outputPD->PassData(pd);
    // set original point ids
    originalPointIds->SetNumberOfTuples(numPts);
    vtkSMPTools::For(0, numPts, [&](vtkIdType beginPtId, vtkIdType endPtId) {
      for (vtkIdType ptId = beginPtId; ptId < endPtId; ++ptId)
      {
        originalPointIds->SetValue(ptId, ptId);
      }
    });
  }
  output->SetPoints(newPts);

  // produce a new vtk_vertex cell for each accepted point
  vtkIdType newNumPts = output->GetNumberOfPoints();
  // create connectivity array
  vtkNew<vtkIdTypeArray> connectivity;
  connectivity->SetNumberOfValues(newNumPts);
  vtkSMPTools::For(0, newNumPts, [&](vtkIdType beginPtId, vtkIdType endPtId) {
    for (vtkIdType ptId = beginPtId; ptId < endPtId; ++ptId)
    {
      connectivity->SetValue(ptId, ptId);
    }
  });
  // create offsets array
  vtkNew<vtkIdTypeArray> offsets;
  offsets->SetNumberOfValues(newNumPts + 1);
  vtkSMPTools::For(0, newNumPts + 1, [&](vtkIdType begin, vtkIdType end) {
    for (vtkIdType i = begin; i < end; i++)
    {
      offsets->SetValue(i, i);
    }
  });
  // create cell array
  vtkNew<vtkCellArray> cells;
  cells->SetData(offsets, connectivity);
  // create cell types
  vtkNew<vtkUnsignedCharArray> cellTypes;
  cellTypes->SetNumberOfValues(newNumPts);
  static constexpr unsigned char cellType = VTK_VERTEX;
  vtkSMPTools::Fill(cellTypes->GetPointer(0), cellTypes->GetPointer(newNumPts), cellType);
  // set cells
  output->SetCells(cellTypes, cells);

  // Copy field data
  output->GetFieldData()->ShallowCopy(input->GetFieldData());
}

//------------------------------------------------------------------------------
void vtkExtractSelection::ExtractSelectedRows(
  vtkTable* input, vtkTable* output, vtkSignedCharArray* rowsInside, bool extractAll)
{
  const vtkIdType numRows = input->GetNumberOfRows();
  vtkNew<vtkIdTypeArray> originalRowIds;
  originalRowIds->SetName("vtkOriginalRowIds");

  output->GetRowData()->CopyFieldOff("vtkOriginalRowIds");
  output->GetRowData()->CopyStructure(input->GetRowData());

  if (!extractAll)
  {
    for (vtkIdType rowId = 0; rowId < numRows; ++rowId)
    {
      signed char isInside;
      rowsInside->GetTypedTuple(rowId, &isInside);
      if (isInside)
      {
        output->InsertNextRow(input->GetRow(rowId));
        originalRowIds->InsertNextValue(rowId);
      }
    }
  }
  else
  {
    output->ShallowCopy(input);
    originalRowIds->SetNumberOfTuples(numRows);
    vtkSMPTools::For(0, numRows, [&](vtkIdType beginRowId, vtkIdType endRowId) {
      for (vtkIdType rowId = beginRowId; rowId < endRowId; ++rowId)
      {
        originalRowIds->SetValue(rowId, rowId);
      }
    });
  }
  output->AddColumn(originalRowIds);
}

//------------------------------------------------------------------------------
void vtkExtractSelection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "GenerateGhostArray: " << this->GenerateGhostArray << endl;
  os << indent << "PreserveTopology: " << this->PreserveTopology << endl;
}
VTK_ABI_NAMESPACE_END
