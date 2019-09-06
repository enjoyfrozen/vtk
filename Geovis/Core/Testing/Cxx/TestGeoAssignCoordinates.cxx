/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/


#include "vtkDataSetAttributes.h"
#include "vtkDoubleArray.h"
#include "vtkGeoAssignCoordinates.h"
#include "vtkGraphMapper.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

int TestGeoAssignCoordinates(int argc, char *argv[])
{
  VTK_CREATE(vtkMutableDirectedGraph, g);
  VTK_CREATE(vtkDoubleArray, latitude);
  latitude->SetName("latitude");
  VTK_CREATE(vtkDoubleArray, longitude);
  longitude->SetName("longitude");
  for (vtkIdType i = -90; i <= 90; i += 10)
  {
    for (vtkIdType j = -180; j < 180; j += 20)
    {
      g->AddVertex();
      latitude->InsertNextValue(i);
      longitude->InsertNextValue(j);
    }
  }
  g->GetVertexData()->AddArray(latitude);
  g->GetVertexData()->AddArray(longitude);

  VTK_CREATE(vtkGeoAssignCoordinates, assign);
  assign->SetInputData(g);
  assign->SetLatitudeArrayName("latitude");
  assign->SetLongitudeArrayName("longitude");
  assign->SetGlobeRadius(1.0);
  assign->Update();

  VTK_CREATE(vtkGraphMapper, mapper);
  mapper->SetInputConnection(assign->GetOutputPort());
  VTK_CREATE(vtkActor, actor);
  actor->SetMapper(mapper);
  VTK_CREATE(vtkRenderer, ren);
  ren->AddActor(actor);
  VTK_CREATE(vtkRenderWindowInteractor, iren);
  VTK_CREATE(vtkRenderWindow, win);
  win->AddRenderer(ren);
  win->SetInteractor(iren);
  ren->ResetCamera();

  int retVal = vtkRegressionTestImage(win);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Initialize();
    iren->Start();

    retVal = vtkRegressionTester::PASSED;
  }

  return !retVal;
}
