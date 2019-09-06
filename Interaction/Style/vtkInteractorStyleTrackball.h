/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkInteractorStyleTrackball
 * @brief   provides trackball motion control
 *
 *
 * vtkInteractorStyleTrackball is an implementation of vtkInteractorStyle
 * that defines the trackball style. It is now deprecated and as such a
 * subclass of vtkInteractorStyleSwitch
 *
 * @sa
 * vtkInteractorStyleSwitch vtkInteractorStyleTrackballActor vtkInteractorStyleJoystickCamera
*/

#ifndef vtkInteractorStyleTrackball_h
#define vtkInteractorStyleTrackball_h

#include "vtkInteractionStyleModule.h" // For export macro
#include "vtkInteractorStyleSwitch.h"

class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyleTrackball : public vtkInteractorStyleSwitch
{
public:
  static vtkInteractorStyleTrackball *New();
  vtkTypeMacro(vtkInteractorStyleTrackball,vtkInteractorStyleSwitch);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkInteractorStyleTrackball();
  ~vtkInteractorStyleTrackball() override;

private:
  vtkInteractorStyleTrackball(const vtkInteractorStyleTrackball&) = delete;
  void operator=(const vtkInteractorStyleTrackball&) = delete;
};

#endif
