/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkFixedPointVolumeRayCastMIPHelper
 * @brief   A helper that generates MIP images for the volume ray cast mapper
 *
 * This is one of the helper classes for the vtkFixedPointVolumeRayCastMapper.
 * It will generate maximum intensity images.
 * This class should not be used directly, it is a helper class for
 * the mapper and has no user-level API.
 *
 * @sa
 * vtkFixedPointVolumeRayCastMapper
*/

#ifndef vtkFixedPointVolumeRayCastMIPHelper_h
#define vtkFixedPointVolumeRayCastMIPHelper_h

#include "vtkRenderingVolumeModule.h" // For export macro
#include "vtkFixedPointVolumeRayCastHelper.h"

class vtkFixedPointVolumeRayCastMapper;
class vtkVolume;

class VTKRENDERINGVOLUME_EXPORT vtkFixedPointVolumeRayCastMIPHelper : public vtkFixedPointVolumeRayCastHelper
{
public:
  static vtkFixedPointVolumeRayCastMIPHelper *New();
  vtkTypeMacro(vtkFixedPointVolumeRayCastMIPHelper,vtkFixedPointVolumeRayCastHelper);
  void PrintSelf( ostream& os, vtkIndent indent ) override;

  void  GenerateImage( int threadID,
                               int threadCount,
                               vtkVolume *vol,
                               vtkFixedPointVolumeRayCastMapper *mapper) override;

protected:
  vtkFixedPointVolumeRayCastMIPHelper();
  ~vtkFixedPointVolumeRayCastMIPHelper() override;

private:
  vtkFixedPointVolumeRayCastMIPHelper(const vtkFixedPointVolumeRayCastMIPHelper&) = delete;
  void operator=(const vtkFixedPointVolumeRayCastMIPHelper&) = delete;
};

#endif



