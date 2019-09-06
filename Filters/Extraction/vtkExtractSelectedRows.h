/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkExtractSelectedRows
 * @brief   return selected rows of a table
 *
 *
 * The first input is a vtkTable to extract rows from.
 * The second input is a vtkSelection containing the selected indices.
 * The third input is a vtkAnnotationLayers containing selected indices.
 * The field type of the input selection is ignored when converted to row
 * indices.
*/

#ifndef vtkExtractSelectedRows_h
#define vtkExtractSelectedRows_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkTableAlgorithm.h"


class VTKFILTERSEXTRACTION_EXPORT vtkExtractSelectedRows : public vtkTableAlgorithm
{
public:
  static vtkExtractSelectedRows* New();
  vtkTypeMacro(vtkExtractSelectedRows,vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * A convenience method for setting the second input (i.e. the selection).
   */
  void SetSelectionConnection(vtkAlgorithmOutput* in);

  /**
   * A convenience method for setting the third input (i.e. the annotation layers).
   */
  void SetAnnotationLayersConnection(vtkAlgorithmOutput* in);

  /**
   * Specify the first vtkGraph input and the second vtkSelection input.
   */
  int FillInputPortInformation(int port, vtkInformation* info) override;

  //@{
  /**
   * When set, a column named vtkOriginalRowIds will be added to the output.
   * False by default.
   */
  vtkSetMacro(AddOriginalRowIdsArray, bool);
  vtkGetMacro(AddOriginalRowIdsArray, bool);
  vtkBooleanMacro(AddOriginalRowIdsArray, bool);
  //@}

protected:
  vtkExtractSelectedRows();
  ~vtkExtractSelectedRows() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  bool AddOriginalRowIdsArray;
private:
  vtkExtractSelectedRows(const vtkExtractSelectedRows&) = delete;
  void operator=(const vtkExtractSelectedRows&) = delete;
};

#endif

