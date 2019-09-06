/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include <vtkArrayData.h>
#include <vtkArrayPrint.h>
#include <vtkExtractArray.h>
#include <vtkSmartPointer.h>
#include <vtkSparseArray.h>

#include <iostream>
#include <stdexcept>

#define test_expression(expression) \
{ \
  if(!(expression)) \
    throw std::runtime_error("Expression failed: " #expression); \
}

int ArrayExtractFactoredArray(int vtkNotUsed(argc), char *vtkNotUsed(argv)[])
{
  try
  {
    vtkSmartPointer<vtkSparseArray<double> > a = vtkSmartPointer<vtkSparseArray<double> >::New();
    vtkSmartPointer<vtkSparseArray<double> > b = vtkSmartPointer<vtkSparseArray<double> >::New();

    vtkSmartPointer<vtkArrayData> factored = vtkSmartPointer<vtkArrayData>::New();
    factored->AddArray(a);
    factored->AddArray(b);

    vtkSmartPointer<vtkExtractArray> extract = vtkSmartPointer<vtkExtractArray>::New();
    extract->SetInputData(factored);

    extract->SetIndex(0);
    extract->Update();
    test_expression(extract->GetOutput()->GetArray(static_cast<vtkIdType>(0)) == a.GetPointer());

    extract->SetIndex(1);
    extract->Update();
    test_expression(extract->GetOutput()->GetArray(static_cast<vtkIdType>(0)) == b.GetPointer());

    return EXIT_SUCCESS;
  }
  catch(std::exception& e)
  {
    std::cout << e.what() << std::endl;
    return EXIT_FAILURE;
  }
}

