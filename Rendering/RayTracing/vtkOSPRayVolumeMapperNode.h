/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayVolumeMapperNode
 * @brief   links vtkVolumeMapper  to OSPRay
 *
 * Translates vtkVolumeMapper state into OSPRay rendering calls
*/

#ifndef vtkOSPRayVolumeMapperNode_h
#define vtkOSPRayVolumeMapperNode_h

#include "vtkOSPRayCache.h" // For common cache infrastructure
#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkVolumeMapperNode.h"

#include "RTWrapper/RTWrapper.h" // for handle types

class vtkAbstractArray;
class vtkDataSet;
class vtkVolume;

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayVolumeMapperNode :
  public vtkVolumeMapperNode
{
public:
  static vtkOSPRayVolumeMapperNode* New();
  vtkTypeMacro(vtkOSPRayVolumeMapperNode, vtkVolumeMapperNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Make ospray calls to render me.
   */
  virtual void Render(bool prepass) override;

  /**
   * TODO: fix me
   * should be controlled by VTK SampleDistance, otherwise
   * should use macros and modify self.
   */
  void SetSamplingRate(double rate) { this->SamplingRate = rate; }
  double GetSamplingRate() { return this->SamplingRate; }

protected:
  vtkOSPRayVolumeMapperNode();
  ~vtkOSPRayVolumeMapperNode();

  /**
   * updates internal OSPRay transfer function for volume
   */
  void UpdateTransferFunction(RTW::Backend *be, vtkVolume* vol, double *dataRange=nullptr);

  //TODO: SetAndGetters?
  int NumColors;
  double SamplingRate;
  double SamplingStep;  //base sampling step of each voxel
  bool UseSharedBuffers;
  bool Shade;  //volume shading set through volProperty
  OSPData SharedData;

  vtkTimeStamp BuildTime;
  vtkTimeStamp PropertyTime;

  OSPGeometry OSPRayIsosurface;
  OSPVolume OSPRayVolume;
  OSPTransferFunction TransferFunction;
  std::vector<float> TFVals;
  std::vector<float> TFOVals;

  vtkOSPRayCache<vtkOSPRayCacheItemObject> *Cache;

private:
  vtkOSPRayVolumeMapperNode(const vtkOSPRayVolumeMapperNode&) = delete;
  void operator=(const vtkOSPRayVolumeMapperNode&) = delete;
};
#endif
