/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkmOSPRayDataSetMapperNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkmOSPRayDataSetMapperNode.h"

#include "vtkActor.h"
#include "vtkOSPRayActorNode.h"
#include "vtkOSPRayRendererNode.h"
#include "vtkObjectFactory.h"
#include "vtkProperty.h"
#include "vtkmDataSet.h"
#include "vtkmDataSetMapper.h"
#include <vtkm/cont/DataSet.h>

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkmOSPRayDataSetMapperNode);

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::Invalidate(bool prepass)
{
  if (prepass)
  {
    this->RenderTime = 0;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::Render(bool prepass)
{
  if (prepass)
  {
    // Do the rendering
    vtkWarningMacro(<< "Rendering a dataset");
    // we use a lot of params from our parent
    vtkOSPRayActorNode* aNode = vtkOSPRayActorNode::SafeDownCast(this->Parent);
    vtkActor* act = vtkActor::SafeDownCast(aNode->GetRenderable());

    if (act->GetVisibility() == false)
    {
      return;
    }
    vtkOSPRayRendererNode* orn =
      static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));

    // if there are no changes, just reuse last result
    vtkMTimeType inTime = aNode->GetMTime();
    if (this->RenderTime >= inTime)
    {
      this->RenderGeometricModels();
      return;
    }
    this->RenderTime = inTime;
    this->ClearGeometricModels();

    vtkmDataSet* ds = nullptr;
    vtkmDataSetMapper* mapper = vtkmDataSetMapper::SafeDownCast(act->GetMapper());
    if (mapper && mapper->GetNumberOfInputPorts() > 0)
    {
      ds = vtkmDataSet::SafeDownCast(mapper->GetInput());
    }
    if (ds)
    {
      vtkProperty* property = act->GetProperty();
      double ambient[3];
      double diffuse[3];
      property->GetAmbientColor(ambient);
      property->GetDiffuseColor(diffuse);
      this->ORenderDataSet(
        orn->GetORenderer(), aNode, ds, ambient, diffuse, property->GetOpacity(), "");
    }
    this->RenderGeometricModels();
  }
}

//------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::RenderGeometricModels()
{
  vtkOSPRayRendererNode* orn =
    static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));

  for (auto instance : this->Instances)
  {
    orn->Instances.emplace_back(instance);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::ClearGeometricModels()
{
  vtkOSPRayRendererNode* orn =
    static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));

  RTW::Backend* backend = orn->GetBackend();

  for (auto instance : this->Instances)
  {
    ospRelease(instance);
  }
  this->Instances.clear();
}

//------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::ORenderDataSet(void* vtkNotUsed(renderer),
  vtkOSPRayActorNode* vtkNotUsed(aNode), vtkmDataSet* ds, double* vtkNotUsed(ambientColor),
  double* vtkNotUsed(diffuseColor), double vtkNotUsed(opacity), std::string vtkNotUsed(material))
{
  vtkOSPRayRendererNode* orn =
    static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));
  RTW::Backend* backend = orn->GetBackend();
  if (backend == nullptr)
    return;

  // OSPRenderer oRenderer = static_cast<OSPRenderer>(renderer);
  // vtkActor* act = vtkActor::SafeDownCast(aNode->GetRenderable());
  // vtkProperty* property = act->GetProperty();

  // TODO: Perform transformations (actor and texture)

  // make geometry
  auto vtkmds = ds->GetVtkmDataSet();
  vtkWarningMacro(<< "Number Of Points in vtkmDataSet: "
                  << vtkmds.GetCoordinateSystem().GetNumberOfPoints());
}

VTK_ABI_NAMESPACE_END
