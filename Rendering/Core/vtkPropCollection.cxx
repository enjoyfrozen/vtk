/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPropCollection.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPropCollection);

int vtkPropCollection::GetNumberOfPaths()
{
  int numPaths=0;
  vtkProp *aProp;

  vtkCollectionSimpleIterator pit;
  for ( this->InitTraversal(pit); (aProp=this->GetNextProp(pit)); )
  {
    numPaths += aProp->GetNumberOfPaths();
  }
  return numPaths;
}
