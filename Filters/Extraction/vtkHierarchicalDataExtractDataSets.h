/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHierarchicalDataExtractDataSets
 * @brief   extract a number of datasets
 *
 * Legacy class. Use vtkExtractDataSets instead.
 *
 * @sa
 * vtkExtractDataSets
*/

#ifndef vtkHierarchicalDataExtractDataSets_h
#define vtkHierarchicalDataExtractDataSets_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkExtractDataSets.h"

struct vtkHierarchicalDataExtractDataSetsInternals;

class VTKFILTERSEXTRACTION_EXPORT vtkHierarchicalDataExtractDataSets :
  public vtkExtractDataSets
{
public:
  vtkTypeMacro(vtkHierarchicalDataExtractDataSets,vtkExtractDataSets);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkHierarchicalDataExtractDataSets *New();

protected:
  vtkHierarchicalDataExtractDataSets();
  ~vtkHierarchicalDataExtractDataSets() override;

private:
  vtkHierarchicalDataExtractDataSets(const vtkHierarchicalDataExtractDataSets&) = delete;
  void operator=(const vtkHierarchicalDataExtractDataSets&) = delete;
};

#endif


