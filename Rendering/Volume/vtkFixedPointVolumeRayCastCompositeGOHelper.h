/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkFixedPointVolumeRayCastCompositeGOHelper
 * @brief   A helper that generates composite images for the volume ray cast mapper
 *
 * This is one of the helper classes for the vtkFixedPointVolumeRayCastMapper.
 * It will generate composite images using an alpha blending operation.
 * This class should not be used directly, it is a helper class for
 * the mapper and has no user-level API.
 *
 * @sa
 * vtkFixedPointVolumeRayCastMapper
*/

#ifndef vtkFixedPointVolumeRayCastCompositeGOHelper_h
#define vtkFixedPointVolumeRayCastCompositeGOHelper_h

#include "vtkRenderingVolumeModule.h" // For export macro
#include "vtkFixedPointVolumeRayCastHelper.h"

class vtkFixedPointVolumeRayCastMapper;
class vtkVolume;

class VTKRENDERINGVOLUME_EXPORT vtkFixedPointVolumeRayCastCompositeGOHelper : public vtkFixedPointVolumeRayCastHelper
{
public:
  static vtkFixedPointVolumeRayCastCompositeGOHelper *New();
  vtkTypeMacro(vtkFixedPointVolumeRayCastCompositeGOHelper,vtkFixedPointVolumeRayCastHelper);
  void PrintSelf( ostream& os, vtkIndent indent ) override;

  void  GenerateImage( int threadID,
                               int threadCount,
                               vtkVolume *vol,
                               vtkFixedPointVolumeRayCastMapper *mapper) override;

protected:
  vtkFixedPointVolumeRayCastCompositeGOHelper();
  ~vtkFixedPointVolumeRayCastCompositeGOHelper() override;

private:
  vtkFixedPointVolumeRayCastCompositeGOHelper(const vtkFixedPointVolumeRayCastCompositeGOHelper&) = delete;
  void operator=(const vtkFixedPointVolumeRayCastCompositeGOHelper&) = delete;
};

#endif


