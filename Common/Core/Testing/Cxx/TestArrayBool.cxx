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

int TestArrayBool(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  try
  {
    // Confirm that we can work with dense arrays of bool values
    vtkSmartPointer<vtkDenseArray<char> > dense = vtkSmartPointer<vtkDenseArray<char> >::New();
    vtkDenseArray<char>& dense_ref = *dense;
    dense->Resize(2, 2);
    dense->Fill(0);

    test_expression(dense->GetValue(1, 1) == 0);
    dense->SetValue(1, 1, 1);
    test_expression(dense->GetValue(1, 1) == 1);

    test_expression(dense->GetValue(0, 1) == 0);
    test_expression(dense_ref[vtkArrayCoordinates(0, 1)] == 0);
    dense_ref[vtkArrayCoordinates(0, 1)] = 1;
    test_expression(dense_ref[vtkArrayCoordinates(0, 1)] == 1);
    test_expression(dense->GetValue(0, 1) == 1);

    // Confirm that we can work with sparse arrays of bool values
    vtkSmartPointer<vtkSparseArray<char> > sparse = vtkSmartPointer<vtkSparseArray<char> >::New();
    sparse->Resize(2, 2);

    test_expression(sparse->GetValue(1, 1) == 0);
    sparse->SetValue(1, 1, 1);
    test_expression(sparse->GetValue(1, 1) == 1);

    return 0;
  }
  catch(std::exception& e)
  {
    cerr << e.what() << endl;
    return 1;
  }
}
