/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include <vtkDenseArray.h>
#include <vtkSmartPointer.h>

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

int TestArrayAPIConvenience(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  try
  {
    vtkSmartPointer<vtkDenseArray<double> > a = vtkSmartPointer<vtkDenseArray<double> >::New();
    vtkSmartPointer<vtkDenseArray<double> > b = vtkSmartPointer<vtkDenseArray<double> >::New();

    a->Resize(5);
    b->Resize(vtkArrayExtents(5));
    test_expression(a->GetExtents() == b->GetExtents());

    a->SetValue(2, 3);
    b->SetValue(vtkArrayCoordinates(2), 3);
    test_expression(a->GetValue(2) == b->GetValue(vtkArrayCoordinates(2)));

    a->Resize(5, 6);
    b->Resize(vtkArrayExtents(5, 6));
    test_expression(a->GetExtents() == b->GetExtents());

    a->SetValue(2, 3, 4);
    b->SetValue(vtkArrayCoordinates(2, 3), 4);
    test_expression(a->GetValue(2, 3) == b->GetValue(vtkArrayCoordinates(2, 3)));

    a->Resize(5, 6, 7);
    b->Resize(vtkArrayExtents(5, 6, 7));
    test_expression(a->GetExtents() == b->GetExtents());

    a->SetValue(2, 3, 4, 5);
    b->SetValue(vtkArrayCoordinates(2, 3, 4), 5);
    test_expression(a->GetValue(2, 3, 4) == b->GetValue(vtkArrayCoordinates(2, 3, 4)));

    return 0;
  }
  catch(std::exception& e)
  {
    cerr << e.what() << endl;
    return 1;
  }
}
