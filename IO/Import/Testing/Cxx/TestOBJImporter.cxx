/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkNew.h"
#include "vtkOBJImporter.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"

#include "vtksys/SystemTools.hxx"

#include <sstream>
#include "vtkMapper.h"

int TestOBJImporter( int argc, char * argv [] )
{
  // note that the executable name is stripped out already
  // so argc argv will not have it

  // Files for testing demonstrate updated functionality for OBJ import:
  //       polydata + textures + actor properties all get loaded.
  if(argc < 2)
  {
    std::cout <<"expected TestName File1.obj [File2.obj.mtl]  [texture1]  ... "<<std::endl;
    return EXIT_FAILURE;
  }

  std::string filenameOBJ(argv[1]);

  std::string filenameMTL,texfile1;

  if(argc >= 3)
  {
    filenameMTL = argv[2];
  }

  if(argc >= 4)
  {
    texfile1 = argv[3];
  }
  std::string texture_path1 = vtksys::SystemTools::GetFilenamePath(texfile1);

  vtkNew<vtkOBJImporter> importer;
  importer->SetFileName(filenameOBJ.data());
  importer->SetFileNameMTL(filenameMTL.data());
  importer->SetTexturePath(texture_path1.data());

  vtkNew<vtkRenderer> ren;
  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;

  renWin->AddRenderer(ren);
  iren->SetRenderWindow(renWin);
  importer->SetRenderWindow(renWin);
  importer->Update();

  ren->ResetCamera();

  if( 1 > ren->GetActors()->GetNumberOfItems() )
  {
    std::cout << "failed to get an actor created?!" << std::endl;
    return EXIT_FAILURE;
  }

  ren->GetActiveCamera()->SetPosition(10,10,-10);
  ren->ResetCamera();
  renWin->SetSize(800,600);
  iren->Start();

  return (EXIT_SUCCESS);
}
