/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class vtkOSPRayAMRVolumeMapperNode
 * @brief links vtkVolumeMapper  to OSPRay
 *
 * Translates vtkAMRVolumeMapper state into OSPRay rendering calls
 * Directly samples the vtkAMR data structure without resampling
 * Data is expected to be overlapping, only floats and doubles are now
 * supported.
*/

#ifndef vtkOSPRayAMRVolumeMapperNode_h
#define vtkOSPRayAMRVolumeMapperNode_h

#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkOSPRayVolumeMapperNode.h"

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayAMRVolumeMapperNode :
  public vtkOSPRayVolumeMapperNode
{
public:
  static vtkOSPRayAMRVolumeMapperNode* New();
  vtkTypeMacro(vtkOSPRayAMRVolumeMapperNode, vtkOSPRayVolumeMapperNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
  * Traverse graph in ospray's preferred order and render
  */
  virtual void Render(bool prepass) override;
protected:

  vtkOSPRayAMRVolumeMapperNode();
  ~vtkOSPRayAMRVolumeMapperNode() = default;

private:
  vtkOSPRayAMRVolumeMapperNode(const vtkOSPRayAMRVolumeMapperNode&) = delete;
  void operator=(const vtkOSPRayAMRVolumeMapperNode&) = delete;

  float OldSamplingRate;
};
#endif
