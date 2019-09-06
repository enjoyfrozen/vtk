/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkNew.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkXMLUnstructuredGridReader.h"
#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

int TestExtractSurfaceNonLinearSubdivision(int argc, char* argv[])
{
  // Basic visualisation.
  vtkNew<vtkRenderer> ren;
  ren->SetBackground(0,0,0);

  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  renWin->SetSize(300,300);


  vtkNew<vtkXMLUnstructuredGridReader> reader;
  char* filename = vtkTestUtilities::ExpandDataFileName(
    argc, argv, "Data/quadraticTetra01.vtu");
  reader->SetFileName(filename);
  delete [] filename;
  filename = nullptr;

  vtkNew<vtkDataSetSurfaceFilter> extract_surface;
  extract_surface->SetInputConnection(reader->GetOutputPort());
  extract_surface->SetNonlinearSubdivisionLevel(4);

  vtkNew<vtkPolyDataMapper> mapper;
  mapper->SetInputConnection(extract_surface->GetOutputPort());
  mapper->ScalarVisibilityOn();
  mapper->SelectColorArray("scalars");
  mapper->SetScalarModeToUsePointFieldData();

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  ren->AddActor(actor);
  ren->ResetCamera();

  int retVal = vtkRegressionTestImage(renWin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }
  return !retVal;
}
