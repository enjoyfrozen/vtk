/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayVolumeMapper
 * @brief   Standalone OSPRayVolumeMapper.
 *
 * This is a standalone interface for ospray volume rendering to be used
 * within otherwise OpenGL rendering contexts such as within the
 * SmartVolumeMapper.
*/

#ifndef vtkOSPRayVolumeMapper_h
#define vtkOSPRayVolumeMapper_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkOSPRayVolumeInterface.h"

class vtkOSPRayPass;
class vtkRenderer;
class vtkWindow;

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayVolumeMapper
  : public vtkOSPRayVolumeInterface
{
public:
  static vtkOSPRayVolumeMapper *New();
  vtkTypeMacro(vtkOSPRayVolumeMapper,vtkOSPRayVolumeInterface);
  void PrintSelf( ostream& os, vtkIndent indent ) override;

  /**
   * Release any graphics resources that are being consumed by this mapper.
   * The parameter window could be used to determine which graphic
   * resources to release.
   */
  virtual void ReleaseGraphicsResources(vtkWindow *) override;

  // Initialize internal constructs
  virtual void Init();

  /**
   * Render the volume onto the screen.
   * Overridden to use OSPRay to do the work.
   */
  virtual void Render(vtkRenderer *, vtkVolume *) override;

protected:
  vtkOSPRayVolumeMapper();
  ~vtkOSPRayVolumeMapper();

  vtkOSPRayPass *InternalOSPRayPass;
  vtkRenderer *InternalRenderer;
  bool Initialized;

private:
  vtkOSPRayVolumeMapper(const vtkOSPRayVolumeMapper&) = delete;
  void operator=(const vtkOSPRayVolumeMapper&) = delete;
};

#endif
