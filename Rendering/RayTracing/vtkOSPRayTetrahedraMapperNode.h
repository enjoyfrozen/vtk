/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOSPRayTetrahedraMapperNode
 * @brief   Unstructured grid volume renderer.
 *
 * vtkOSPRayTetrahedraMapperNode implements a volume rendering
 * that directly samples the AMR structure using OSPRay.
 *
*/

#ifndef vtkOSPRayTetrahedraMapperNode_h
#define vtkOSPRayTetrahedraMapperNode_h

#include "vtkOSPRayCache.h" // For common cache infrastructure
#include "vtkRenderingRayTracingModule.h" // For export macro
#include "vtkVolumeMapperNode.h"

#include "RTWrapper/RTWrapper.h" // for handle types

class VTKRENDERINGRAYTRACING_EXPORT vtkOSPRayTetrahedraMapperNode : public vtkVolumeMapperNode

{
public:
  vtkTypeMacro(vtkOSPRayTetrahedraMapperNode,
                       vtkVolumeMapperNode);
  static vtkOSPRayTetrahedraMapperNode *New();
  void PrintSelf(ostream &os, vtkIndent indent) override;

  /**
   * Make ospray calls to render me.
   */
  virtual void Render(bool prepass) override;

protected:
  vtkOSPRayTetrahedraMapperNode();
  ~vtkOSPRayTetrahedraMapperNode() override;

  int NumColors;
  double SamplingRate;

  vtkTimeStamp BuildTime;
  vtkTimeStamp PropertyTime;

  OSPVolume OSPRayVolume;
  OSPTransferFunction TransferFunction;
  std::vector<float> TFVals;
  std::vector<float> TFOVals;

  std::vector<int> Cells;
  std::vector<osp::vec3f> Vertices;
  std::vector<float> Field;

  vtkOSPRayCache<vtkOSPRayCacheItemObject> *Cache;
private:
  vtkOSPRayTetrahedraMapperNode(const vtkOSPRayTetrahedraMapperNode&) = delete;
  void operator=(const vtkOSPRayTetrahedraMapperNode &) = delete;
};

#endif
