/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkComputeQuartiles
 * @brief   Extract quartiles and extremum values
 * of all columns of a table or all fields of a dataset.
 *
 *
 * vtkComputeQuartiles accepts any vtkDataObject as input and produces a
 * vtkTable data as output.
 * This filter can be used to generate a table to create box plots
 * using a vtkPlotBox instance.
 * The filter internally uses vtkOrderStatistics to compute quartiles.
 *
 * @sa
 * vtkTableAlgorithm vtkOrderStatistics vtkPlotBox vtkChartBox
 *
 * @par Thanks:
 * This class was written by Kitware SAS and supported by EDF - www.edf.fr
*/

#ifndef vtkComputeQuartiles_h
#define vtkComputeQuartiles_h

#include "vtkFiltersStatisticsModule.h" // For export macro
#include "vtkTableAlgorithm.h"

class vtkDataSet;
class vtkDoubleArray;
class vtkFieldData;
class vtkTable;


class VTKFILTERSSTATISTICS_EXPORT vtkComputeQuartiles : public vtkTableAlgorithm
{
public:
  static vtkComputeQuartiles* New();
  vtkTypeMacro(vtkComputeQuartiles, vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkComputeQuartiles();
  ~vtkComputeQuartiles() override;

  int FillInputPortInformation (int port, vtkInformation *info) override;

  int RequestData(vtkInformation *request,
                  vtkInformationVector **inputVector,
                  vtkInformationVector *outputVector) override;

  void ComputeTable(vtkDataObject*, vtkTable*, vtkIdType);

  int FieldAssociation;

private:
  void operator=(const vtkComputeQuartiles&) = delete;
  vtkComputeQuartiles(const vtkComputeQuartiles&) = delete;

  int GetInputFieldAssociation();
  vtkFieldData* GetInputFieldData(vtkDataObject* input);
};

#endif
