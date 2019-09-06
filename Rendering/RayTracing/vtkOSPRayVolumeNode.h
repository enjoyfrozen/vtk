/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayVolumeNode
 * @brief   links vtkVolume and vtkMapper to OSPRay
 *
 * Translates vtkVolume/Mapper state into OSPRay rendering calls
*/

#ifndef vtkOSPRayVolumeNode_h
#define vtkOSPRayVolumeNode_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkVolumeNode.h"

class vtkVolume;
class vtkCompositeDataDisplayAttributes;
class vtkDataArray;
class vtkInformationIntegerKey;
class vtkInformationObjectBaseKey;
class vtkInformationStringKey;
class vtkPiecewiseFunction;
class vtkPolyData;

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayVolumeNode :
  public vtkVolumeNode
{
public:
  static vtkOSPRayVolumeNode* New();
  vtkTypeMacro(vtkOSPRayVolumeNode, vtkVolumeNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Overridden to take into account my renderables time, including
   * mapper and data into mapper inclusive of composite input
   */
  virtual vtkMTimeType GetMTime() override;

protected:
  vtkOSPRayVolumeNode();
  ~vtkOSPRayVolumeNode();

private:
  vtkOSPRayVolumeNode(const vtkOSPRayVolumeNode&) = delete;
  void operator=(const vtkOSPRayVolumeNode&) = delete;
};
#endif
