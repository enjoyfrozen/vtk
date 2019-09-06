/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkRendererCollection.h"
#include "vtkRenderWindow.h"
#include "vtkObjectFactory.h"

#include <cstdlib>

vtkStandardNewMacro(vtkRendererCollection);

// Forward the Render() method to each renderer in the list.
void vtkRendererCollection::Render()
{
  vtkRenderer      *ren, *firstRen;
  vtkRenderWindow  *renWin;
  int               numLayers, i;

  vtkCollectionSimpleIterator rsit;
  this->InitTraversal(rsit);
  firstRen = this->GetNextRenderer(rsit);
  if (firstRen == nullptr)
  {
    // We cannot determine the number of layers because there are no
    // renderers.  No problem, just return.
    return;
  }
  renWin = firstRen->GetRenderWindow();
  numLayers = renWin->GetNumberOfLayers();

  // Only have the renderers render from back to front.  This is necessary
  // because transparent renderers clear the z-buffer before each render and
  // then overlay their image.
  for (i = 0; i < numLayers; i++)
  {
    for (this->InitTraversal(rsit); (ren = this->GetNextRenderer(rsit)); )
    {
      if (ren->GetLayer() == i)
      {
        ren->Render();
      }
    }
  }

  // Let the user know if they have put a renderer at an unused layer.
  for (this->InitTraversal(rsit); (ren = this->GetNextRenderer(rsit)); )
  {
    if (ren->GetLayer() < 0 || ren->GetLayer() >= numLayers)
    {
      vtkErrorMacro(<< "Invalid layer for renderer: not rendered.");
    }
  }
}

vtkRenderer *vtkRendererCollection::GetFirstRenderer()
{
  if ( this->Top == nullptr )
  {
    return nullptr;
  }
  else
  {
    return static_cast<vtkRenderer *>(this->Top->Item);
  }
}

//----------------------------------------------------------------------------
void vtkRendererCollection::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
