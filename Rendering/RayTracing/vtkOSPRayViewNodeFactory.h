/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayViewNodeFactory
 * @brief   matches vtk rendering classes to
 * specific ospray ViewNode classes
 *
 * Ensures that vtkOSPRayPass makes ospray specific translator instances
 * for every VTK rendering pipeline class instance it encounters.
*/

#ifndef vtkOSPRayViewNodeFactory_h
#define vtkOSPRayViewNodeFactory_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkViewNodeFactory.h"

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayViewNodeFactory :
  public vtkViewNodeFactory
{
public:
  static vtkOSPRayViewNodeFactory* New();
  vtkTypeMacro(vtkOSPRayViewNodeFactory, vtkViewNodeFactory);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkOSPRayViewNodeFactory();
  ~vtkOSPRayViewNodeFactory();

private:
  vtkOSPRayViewNodeFactory(const vtkOSPRayViewNodeFactory&) = delete;
  void operator=(const vtkOSPRayViewNodeFactory&) = delete;
};

#endif
