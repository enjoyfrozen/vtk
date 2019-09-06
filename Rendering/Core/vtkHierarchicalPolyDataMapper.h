/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHierarchicalPolyDataMapper
 * @brief   a class that renders hierarchical polygonal data
 *
 * Legacy class. Use vtkCompositePolyDataMapper instead.
 *
 * @sa
 * vtkPolyDataMapper
*/

#ifndef vtkHierarchicalPolyDataMapper_h
#define vtkHierarchicalPolyDataMapper_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkCompositePolyDataMapper.h"

class VTKRENDERINGCORE_EXPORT vtkHierarchicalPolyDataMapper : public vtkCompositePolyDataMapper
{

public:
  static vtkHierarchicalPolyDataMapper *New();
  vtkTypeMacro(vtkHierarchicalPolyDataMapper, vtkCompositePolyDataMapper);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkHierarchicalPolyDataMapper();
  ~vtkHierarchicalPolyDataMapper() override;

private:
  vtkHierarchicalPolyDataMapper(const vtkHierarchicalPolyDataMapper&) = delete;
  void operator=(const vtkHierarchicalPolyDataMapper&) = delete;
};

#endif
