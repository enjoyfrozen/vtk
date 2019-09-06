/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkIdListCollection.h"
#include "vtkNew.h"
#include "vtkPoints.h"
#include "vtkPolygonBuilder.h"
#include "vtkSmartPointer.h"

int TestPolygonBuilder3(int, char* [])
{

  vtkPolygonBuilder builder;
  builder.InsertTriangle(nullptr);
  vtkNew<vtkIdListCollection> polys;
  builder.GetPolygons(polys);
  if (polys->GetNumberOfItems() != 0)
  {
    return EXIT_FAILURE;
  }


  return EXIT_SUCCESS;
}
