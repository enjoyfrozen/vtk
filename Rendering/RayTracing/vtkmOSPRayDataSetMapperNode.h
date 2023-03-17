/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkmOSPRayDataSetMapperNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkmOSPRayDataSetMapperNode
 * @brief
 *
 */

#ifndef vtkmOSPRayDataSetMapperNode_h
#define vtkmOSPRayDataSetMapperNode_h

#include "vtkMapperNode.h"
#include "vtkOSPRayCache.h"               // For common cache infrastructure
#include "vtkRenderingRayTracingModule.h" // For export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations
class vtkOSPRayActorNode;
class vtkmDataSet;

class VTKRENDERINGRAYTRACING_EXPORT vtkmOSPRayDataSetMapperNode : public vtkMapperNode
{
public:
  /**
   * Instantiate the class.
   */
  static vtkmOSPRayDataSetMapperNode* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkmOSPRayDataSetMapperNode, vtkMapperNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Make ospray calls to render me.
   */
  void Render(bool prepass) override;

  /**
   * Invalidates cached rendering data.
   */
  void Invalidate(bool prepass) override;

protected:
  vtkmOSPRayDataSetMapperNode() = default;
  ~vtkmOSPRayDataSetMapperNode() = default;

  // Helper members
  std::vector<OSPGeometricModel> GeometricModels;
  std::vector<OSPInstance> Instances;
  ///@{
  /**
   * @brief add/remove precomputed ospray geometries to renderer model.
   */
  void RenderGeometricModels();
  void ClearGeometricModels();
  ///@}
  void ORenderDataSet(void* renderer, vtkOSPRayActorNode* aNode, vtkmDataSet* ds,
    double* ambientColor, double* diffuseColor, double opacity, std::string material);

private:
  vtkmOSPRayDataSetMapperNode(const vtkmOSPRayDataSetMapperNode&) = delete;
  void operator=(const vtkmOSPRayDataSetMapperNode) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkmOSPRayDataSetMapperNode_h
