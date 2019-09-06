/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkLightsPass.h"
#include "vtkObjectFactory.h"
#include <cassert>
#include "vtkRenderState.h"
#include "vtkRenderer.h"

vtkStandardNewMacro(vtkLightsPass);

// ----------------------------------------------------------------------------
vtkLightsPass::vtkLightsPass() = default;

// ----------------------------------------------------------------------------
vtkLightsPass::~vtkLightsPass() = default;

// ----------------------------------------------------------------------------
void vtkLightsPass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkLightsPass::Render(const vtkRenderState *s)
{
  assert("pre: s_exists" && s!=nullptr);

  this->NumberOfRenderedProps=0;

  this->ClearLights(s->GetRenderer());
  this->UpdateLightGeometry(s->GetRenderer());
  this->UpdateLights(s->GetRenderer());
}
