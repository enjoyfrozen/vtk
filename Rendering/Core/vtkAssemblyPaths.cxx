/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAssemblyPaths.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkAssemblyPaths);

vtkMTimeType vtkAssemblyPaths::GetMTime()
{
  vtkMTimeType mtime = this->vtkCollection::GetMTime();

  vtkAssemblyPath *path;
  for (this->InitTraversal(); (path = this->GetNextItem());)
  {
    vtkMTimeType pathMTime = path->GetMTime();
    if (pathMTime > mtime)
    {
      mtime = pathMTime;
    }
  }
  return mtime;
}
