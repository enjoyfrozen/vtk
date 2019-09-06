/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayVolumeInterface
 * @brief   Removes link dependence
 * on optional ospray module.
 *
 * Class allows SmartVolume to use OSPRay for rendering when ospray
 * is enabled. When disabled, this class does nothing but return a warning.
*/

#ifndef vtkOSPRayVolumeInterface_h
#define vtkOSPRayVolumeInterface_h

#include "vtkRenderingVolumeModule.h" // For export macro
#include "vtkVolumeMapper.h"

class vtkRenderer;
class vtkVolume;

class VTKRENDERINGVOLUME_EXPORT vtkOSPRayVolumeInterface
: public vtkVolumeMapper
{
public:
  static vtkOSPRayVolumeInterface *New();
  vtkTypeMacro(vtkOSPRayVolumeInterface,vtkVolumeMapper);
  void PrintSelf( ostream& os, vtkIndent indent ) override;

  /**
   * Overridden to warn about lack of OSPRay if not overridden.
   */
  void Render(vtkRenderer *, vtkVolume *) override;

protected:
  vtkOSPRayVolumeInterface();
  ~vtkOSPRayVolumeInterface() override;

private:
  vtkOSPRayVolumeInterface
    (const vtkOSPRayVolumeInterface&) = delete;
  void operator=(const vtkOSPRayVolumeInterface&) = delete;
};

#endif
