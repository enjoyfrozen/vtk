/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkReferenceCount
 * @brief   Obsolete / empty subclass of object.
 *
 * vtkReferenceCount functionality has now been moved into vtkObject
 * @sa
 * vtkObject
*/

#ifndef vtkReferenceCount_h
#define vtkReferenceCount_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkObject.h"

class VTKCOMMONCORE_EXPORT vtkReferenceCount : public vtkObject
{
public:
  static vtkReferenceCount *New();

  vtkTypeMacro(vtkReferenceCount,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkReferenceCount();
  ~vtkReferenceCount() override;

private:
  vtkReferenceCount(const vtkReferenceCount&) = delete;
  void operator=(const vtkReferenceCount&) = delete;
};


#endif

