/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkDummyController.h"

#include "ExerciseMultiProcessController.h"

#include "vtkSmartPointer.h"
#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

int DummyController(int argc, char* argv[])
{
  VTK_CREATE(vtkDummyController, controller);

  controller->Initialize(&argc, &argv, 1);

  int retval = ExerciseMultiProcessController(controller);

  controller->Finalize();

  return retval;
}
