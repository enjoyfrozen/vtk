/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkBlueObeliskData.h"
#include "vtkChemistryConfigure.h"

#include <fstream>

int GenerateBlueObeliskHeader(int, char *[])
{
  std::ifstream xml(VTK_BODR_DATA_PATH_BUILD "/elements.xml");
  if (!xml)
  {
    std::cerr << "Error opening file " VTK_BODR_DATA_PATH_BUILD
                 "/elements.xml.\n";
    return EXIT_FAILURE;
  }

  std::cout << "// VTK/Domains/Chemistry/Testing/Cxx/GenerateBlueObeliskHeader.cxx\n";
  if (!vtkBlueObeliskData::GenerateHeaderFromXML(xml, std::cout))
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
