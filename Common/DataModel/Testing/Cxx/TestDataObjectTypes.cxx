/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkDataObjectTypes.h"

class TestDataObjectTypesTester: public vtkDataObjectTypes
{
public:
  static int Test() { return vtkDataObjectTypes::Validate(); };
};

int TestDataObjectTypes(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  return TestDataObjectTypesTester::Test();
}
