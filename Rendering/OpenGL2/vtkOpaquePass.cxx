/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkOpaquePass.h"
#include "vtkObjectFactory.h"
#include <cassert>

vtkStandardNewMacro(vtkOpaquePass);

// ----------------------------------------------------------------------------
vtkOpaquePass::vtkOpaquePass() = default;

// ----------------------------------------------------------------------------
vtkOpaquePass::~vtkOpaquePass() = default;

// ----------------------------------------------------------------------------
void vtkOpaquePass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkOpaquePass::Render(const vtkRenderState *s)
{
  assert("pre: s_exists" && s!=nullptr);

  this->NumberOfRenderedProps=0;
  this->RenderFilteredOpaqueGeometry(s);
}
