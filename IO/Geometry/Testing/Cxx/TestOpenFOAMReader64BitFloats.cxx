/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkOpenFOAMReader.h"

#include "vtkCellData.h"
#include "vtkCompositeDataGeometryFilter.h"
#include "vtkCompositePolyDataMapper.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkUnstructuredGrid.h"

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

int TestOpenFOAMReader64BitFloats(int argc, char* argv[])
{
  // Read file name.
  char* filename =
    vtkTestUtilities::ExpandDataFileName(argc, argv,
                                         "Data/OpenFOAM/64BitFloats/test.foam");

  // Read the file
  vtkNew<vtkOpenFOAMReader> reader;
  reader->SetFileName(filename);
  delete [] filename;
  reader->Use64BitFloatsOn();

  // Visualize
  vtkNew<vtkCompositeDataGeometryFilter> polyFilter;
  polyFilter->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkCompositePolyDataMapper> mapper;
  mapper->SetInputConnection(polyFilter->GetOutputPort());
  mapper->SetScalarRange(1, 2);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);

  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);

  renderer->AddActor(actor);
  renderer->SetBackground(.2, .4, .6);

  renderWindow->Render();

  int retVal = vtkRegressionTestImage( renderWindow );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    renderWindowInteractor->Start();
  }

  return EXIT_SUCCESS;
}
