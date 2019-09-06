/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkOverlayPass.h"
#include "vtkObjectFactory.h"
#include <cassert>

vtkStandardNewMacro(vtkOverlayPass);

// ----------------------------------------------------------------------------
vtkOverlayPass::vtkOverlayPass() = default;

// ----------------------------------------------------------------------------
vtkOverlayPass::~vtkOverlayPass() = default;

// ----------------------------------------------------------------------------
void vtkOverlayPass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkOverlayPass::Render(const vtkRenderState *s)
{
  assert("pre: s_exists" && s!=nullptr);

  this->NumberOfRenderedProps=0;
  this->RenderFilteredOverlay(s);
}
