/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHierarchicalDataExtractLevel
 * @brief   extract levels between min and max
 *
 * Legacy class. Use vtkExtractLevel instead.
*/

#ifndef vtkHierarchicalDataExtractLevel_h
#define vtkHierarchicalDataExtractLevel_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkExtractLevel.h"

class VTKFILTERSEXTRACTION_EXPORT vtkHierarchicalDataExtractLevel : public vtkExtractLevel
{
public:
  vtkTypeMacro(vtkHierarchicalDataExtractLevel,vtkExtractLevel);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkHierarchicalDataExtractLevel *New();

protected:
  vtkHierarchicalDataExtractLevel();
  ~vtkHierarchicalDataExtractLevel() override;

private:
  vtkHierarchicalDataExtractLevel(const vtkHierarchicalDataExtractLevel&) = delete;
  void operator=(const vtkHierarchicalDataExtractLevel&) = delete;
};

#endif


