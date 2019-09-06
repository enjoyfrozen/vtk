/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkObjectFactory.h"
#include "vtkImageItem.h"
#include "vtkContextView.h"
#include "vtkContextScene.h"
#include "vtkPNGReader.h"
#include "vtkImageData.h"
#include "vtkNew.h"

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"

//----------------------------------------------------------------------------
int TestContextImage(int argc, char * argv [])
{
  char* logo = vtkTestUtilities::ExpandDataFileName(argc, argv,
                                                    "Data/vtk.png");

  // Set up a 2D context view, context test object and add it to the scene
  vtkNew<vtkContextView> view;
  view->GetRenderWindow()->SetSize(320, 181);
  vtkNew<vtkImageItem> item;
  view->GetScene()->AddItem(item);

  vtkNew<vtkPNGReader> reader;
  reader->SetFileName(logo);
  reader->Update();
  item->SetImage(vtkImageData::SafeDownCast(reader->GetOutput()));
  item->SetPosition(25, 30);

  view->GetRenderWindow()->SetMultiSamples(0);
  view->GetInteractor()->Initialize();
  view->GetInteractor()->Start();

  delete []logo;
  return EXIT_SUCCESS;
}
