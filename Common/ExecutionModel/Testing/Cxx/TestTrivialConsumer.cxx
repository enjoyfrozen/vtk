/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkNew.h"
#include "vtkSphereSource.h"
#include "vtkTrivialConsumer.h"

int TestTrivialConsumer(int, char*[])
{
  vtkNew<vtkSphereSource> spheres;
  vtkNew<vtkTrivialConsumer> consumer;

  consumer->SetInputConnection(spheres->GetOutputPort());
  consumer->Update();

  return 0;
}
