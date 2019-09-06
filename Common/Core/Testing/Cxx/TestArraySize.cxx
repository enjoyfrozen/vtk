/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include <vtkDenseArray.h>
#include <vtkSmartPointer.h>
#include <vtkSparseArray.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

#define test_expression(expression) \
{ \
  if(!(expression)) \
  { \
    std::ostringstream buffer; \
    buffer << "Expression failed at line " << __LINE__ << ": " << #expression; \
    throw std::runtime_error(buffer.str()); \
  } \
}

int TestArraySize(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  try
  {
    // Test to see that array sizes don't overflow ...
    vtkSmartPointer<vtkSparseArray<double> > array = vtkSmartPointer<vtkSparseArray<double> >::New();
    array->Resize(1200000, 18000);
    test_expression(array->GetSize() == 21600000000ULL);

    return 0;
  }
  catch(std::exception& e)
  {
    cerr << e.what() << endl;
    return 1;
  }
}
