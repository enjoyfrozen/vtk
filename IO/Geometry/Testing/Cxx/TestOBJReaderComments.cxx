/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// .NAME Verifies that vtkOBJReader does something sensible w/rt materials.
// .SECTION Description
//

#include "vtkOBJReader.h"

#include "vtkStringArray.h"
#include "vtkTestUtilities.h"

int TestOBJReaderComments( int argc, char *argv[] )
{
  // Create the reader.
  char* fname = vtkTestUtilities::ExpandDataFileName(
    argc, argv, "Data/Viewpoint/cow.obj");
  vtkSmartPointer<vtkOBJReader> reader =
    vtkSmartPointer<vtkOBJReader>::New();
  reader->SetFileName(fname);
  reader->Update();
  delete [] fname;

  const char* comment = reader->GetComment();
  if(!comment)
  {
    std::cerr << "Could not read comments" << std::endl;
    return EXIT_FAILURE;
  }
  std::string commentStr(comment);
  if(commentStr.empty())
  {
    std::cerr << "Expected non-empty comment." << std::endl;
    return EXIT_FAILURE;
  }

  if (commentStr.find("Cow (moo)") == std::string::npos
    || commentStr.find("Viewpoint Animation Engineering") == std::string::npos)
  {
    std::cerr << "Did not find expected comment. Comment:" << std::endl;
    std::cerr << comment << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
