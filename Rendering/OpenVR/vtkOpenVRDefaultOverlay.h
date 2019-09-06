/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOpenVRDefaultOverlay
 * @brief   OpenVR overlay
 *
 * vtkOpenVRDefaultOverlay support for VR overlays
*/

#ifndef vtkOpenVRDefaultOverlay_h
#define vtkOpenVRDefaultOverlay_h

#include "vtkRenderingOpenVRModule.h" // For export macro
#include "vtkOpenVROverlay.h"

class VTKRENDERINGOPENVR_EXPORT vtkOpenVRDefaultOverlay : public vtkOpenVROverlay
{
public:
  static vtkOpenVRDefaultOverlay *New();
  vtkTypeMacro(vtkOpenVRDefaultOverlay, vtkOpenVROverlay);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Render the overlay, we set some opf the spots based on current settings
   */
  void Render() override;

protected:
  vtkOpenVRDefaultOverlay();
  ~vtkOpenVRDefaultOverlay();

  void SetupSpots() override;

private:
  vtkOpenVRDefaultOverlay(const vtkOpenVRDefaultOverlay&) = delete;
  void operator=(const vtkOpenVRDefaultOverlay&) = delete;
};

#endif
