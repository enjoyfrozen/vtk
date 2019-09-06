/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkImageRenderManager.h"

#include "vtkFloatArray.h"
#include "vtkMultiProcessController.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTimerLog.h"
#include "vtkUnsignedCharArray.h"

vtkStandardNewMacro(vtkImageRenderManager);

//----------------------------------------------------------------------------
vtkImageRenderManager::vtkImageRenderManager()
{
}

//----------------------------------------------------------------------------
vtkImageRenderManager::~vtkImageRenderManager()
{
}

//----------------------------------------------------------------------------
void vtkImageRenderManager::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkImageRenderManager::PreRenderProcessing()
{
  // Turn swap buffers off before the render so the end render method has a
  // chance to add to the back buffer.
  if (this->UseBackBuffer)
  {
    this->RenderWindow->SwapBuffersOff();
  }
}

//----------------------------------------------------------------------------
void vtkImageRenderManager::PostRenderProcessing()
{
  if (!this->UseCompositing || this->CheckForAbortComposite())
  {
    return;
  }

  // Swap buffers here
  if (this->UseBackBuffer)
  {
    this->RenderWindow->SwapBuffersOn();
  }
  this->RenderWindow->Frame();
}
