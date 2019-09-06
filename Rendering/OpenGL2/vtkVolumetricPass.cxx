/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkVolumetricPass.h"
#include "vtkObjectFactory.h"
#include <cassert>

vtkStandardNewMacro(vtkVolumetricPass);

// ----------------------------------------------------------------------------
vtkVolumetricPass::vtkVolumetricPass() = default;

// ----------------------------------------------------------------------------
vtkVolumetricPass::~vtkVolumetricPass() = default;

// ----------------------------------------------------------------------------
void vtkVolumetricPass::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

// ----------------------------------------------------------------------------
// Description:
// Perform rendering according to a render state \p s.
// \pre s_exists: s!=0
void vtkVolumetricPass::Render(const vtkRenderState *s)
{
  assert("pre: s_exists" && s!=nullptr);

  this->NumberOfRenderedProps=0;
  this->RenderFilteredVolumetricGeometry(s);
}
