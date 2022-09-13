/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractSelection.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkExtractSelection
 * @brief   extract a subset from a vtkDataSet.
 *
 * vtkExtractSelection extracts some subset of cells and points from
 * its input dataobject. The dataobject is given on its first input port.
 * The subset is described by the contents of the vtkSelection on its
 * second input port.  Depending on the contents of the vtkSelection
 * this will create various vtkSelectors to identify the
 * selected elements.
 *
 * This filter supports vtkCompositeDataSet (output is vtkMultiBlockDataSet),
 * vtkTable and vtkDataSet (output is vtkUnstructuredGrid).
 * Other types of input are not processed and the corresponding output is a
 * default constructed object of the input type.
 *
 * @sa
 * vtkSelection vtkSelector vtkSelectionNode
 */

#ifndef vtkExtractSelection_h
#define vtkExtractSelection_h

#include "vtkDataObjectAlgorithm.h"
#include "vtkFiltersExtractionModule.h" // For export macro

#include "vtkSelectionNode.h" // for vtkSelectionNode::SelectionContent
#include "vtkSmartPointer.h"  // for smart pointer

VTK_ABI_NAMESPACE_BEGIN
class vtkSignedCharArray;
class vtkSelection;
class vtkSelectionNode;
class vtkSelector;
class vtkUnstructuredGrid;
class vtkTable;

class VTKFILTERSEXTRACTION_EXPORT vtkExtractSelection : public vtkDataObjectAlgorithm
{
public:
  static vtkExtractSelection* New();
  vtkTypeMacro(vtkExtractSelection, vtkDataObjectAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Convenience method to specify the selection connection (2nd input
   * port)
   */
  void SetSelectionConnection(vtkAlgorithmOutput* algOutput)
  {
    this->SetInputConnection(1, algOutput);
  }

  ///@{
  /**
   * This flag tells the extraction filter not to extract a subset of the data, but instead to
   * produce a vtkInsidedness array and add it to the input dataset.
   *
   * Turn this feature on for higher performance filtering so this filter only builds the array, but
   * doesn't have to filter and copy data arrays when you know a downstream filter will have to
   * iterate over your whole data anyways.
   *
   * Default value is false.
   */
  vtkSetMacro(PreserveTopology, bool);
  vtkGetMacro(PreserveTopology, bool);
  vtkBooleanMacro(PreserveTopology, bool);
  ///@}

  //@{
  /**
   * The name for the insidedness array from a previous filter which should be combined with the
   * result of this filter.
   *
   * This is useful when stacking multiple filters in sequence, as filters generating these arrays
   * often pass data with shallow copies for performance reasons. Then each filter needs to create
   * it's own insidedness array with the result up to that point. If only a later filter is
   * modified, that later filter then still has the previous result, and doesn't accidentally
   * recycle its own previous results into a later calculation.
   *
   * Default value is nullptr(0).
   */
  vtkSetStringMacro(TopologyFilterInputArrayName);
  vtkGetStringMacro(TopologyFilterInputArrayName);
  //@}

  //@{
  /**
   * The name for the resulting insidedness array defining which points or cells should be filtered
   * from the dataset.
   *
   * Default value is "vtkInsidedness".
   */
  vtkSetStringMacro(TopologyFilterOutputArrayName);
  vtkGetStringMacro(TopologyFilterOutputArrayName);
  //@}

protected:
  vtkExtractSelection();
  ~vtkExtractSelection() override;

  /**
   * Sets up empty output dataset
   */
  int RequestDataObject(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  /**
   * Sets up empty output dataset
   */
  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  // Gets the attribute association of the selection.  Currently we support ROW, POINT, and CELL.
  // If the selection types are mismatched the boolean parameter will be set to false, otherwise
  // it will be true after the function returns.
  vtkDataObject::AttributeTypes GetAttributeTypeOfSelection(vtkSelection* sel, bool& sane);

  /**
   * Creates a new vtkSelector for the given content type.
   * May return null if not supported.
   */
  virtual vtkSmartPointer<vtkSelector> NewSelectionOperator(
    vtkSelectionNode::SelectionContent type);

  /**
   * Given a non-composite input data object (either a block of a larger composite
   * or the whole input), along with the element type being extracted and the
   * computed insidedness array this method either copies the input and adds the
   * insidedness array (if PreserveTopology is on) or returns a new data object
   * containing only the elements to be extracted.
   */
  vtkSmartPointer<vtkDataObject> ExtractElements(vtkDataObject* block,
    vtkDataObject::AttributeTypes elementType, vtkSignedCharArray* insidednessArray);

  int FillInputPortInformation(int port, vtkInformation* info) override;

  /**
   * Given a vtkDataSet and an array of which cells to extract, this populates
   * the given vtkUnstructuredGrid with the selected cells.
   */
  void ExtractSelectedCells(
    vtkDataSet* input, vtkUnstructuredGrid* output, vtkSignedCharArray* cellInside);
  /**
   * Given a vtkDataSet and an array of which points to extract, the populates
   * the given vtkUnstructuredGrid with the selected points and a cell of type vertex
   * for each point.
   */
  void ExtractSelectedPoints(
    vtkDataSet* input, vtkUnstructuredGrid* output, vtkSignedCharArray* pointInside);
  /**
   * Given an input vtkTable and an array of which rows to extract, this populates
   * the output table with the selected rows.
   */
  void ExtractSelectedRows(vtkTable* input, vtkTable* output, vtkSignedCharArray* rowsInside);

  bool PreserveTopology = false;
  char* TopologyFilterInputArrayName = nullptr;
  char* TopologyFilterOutputArrayName = nullptr;

private:
  vtkExtractSelection(const vtkExtractSelection&) = delete;
  void operator=(const vtkExtractSelection&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif
