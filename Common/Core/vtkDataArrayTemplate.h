/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkDataArrayTemplate
 *
 * vtkDataArrayTemplate is deprecated, use vtkAOSDataArrayTemplate instead.
*/

#ifndef vtkDataArrayTemplate_h
#define vtkDataArrayTemplate_h

#include "vtkAOSDataArrayTemplate.h"

#ifndef VTK_LEGACY_REMOVE

template <typename ValueType>
class vtkDataArrayTemplate : public vtkAOSDataArrayTemplate<ValueType>
{
public:
  vtkTemplateTypeMacro(vtkDataArrayTemplate<ValueType>,
                       vtkAOSDataArrayTemplate<ValueType>)

  static vtkDataArrayTemplate<ValueType>* New()
  {
    VTK_STANDARD_NEW_BODY(vtkDataArrayTemplate<ValueType>);
  }

protected:
  vtkDataArrayTemplate() {}
  ~vtkDataArrayTemplate() override {}

private:
  vtkDataArrayTemplate(const vtkDataArrayTemplate&) = delete;
  void operator=(const vtkDataArrayTemplate&) = delete;
};

#endif // VTK_LEGACY_REMOVE

#endif // vtkDataArrayTemplate_h

// VTK-HeaderTest-Exclude: vtkDataArrayTemplate.h
