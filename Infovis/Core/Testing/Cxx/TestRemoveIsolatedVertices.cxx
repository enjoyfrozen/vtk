/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#include <vtkSmartPointer.h>
 #include <vtkMutableUndirectedGraph.h>
#include <vtkRemoveIsolatedVertices.h>

int TestRemoveIsolatedVertices(int, char *[])
{
  vtkSmartPointer<vtkMutableUndirectedGraph> g =
      vtkSmartPointer<vtkMutableUndirectedGraph>::New();

  // Create 3 vertices
  vtkIdType v1 = g->AddVertex();
  vtkIdType v2 = g->AddVertex();
  g->AddVertex();

  g->AddEdge ( v1, v2 );

  vtkSmartPointer<vtkRemoveIsolatedVertices> filter =
    vtkSmartPointer<vtkRemoveIsolatedVertices>::New();
  filter->SetInputData(g);
  filter->Update();

  if(filter->GetOutput()->GetNumberOfVertices() != 2)
  {
    std::cerr << "There are " << filter->GetOutput()->GetNumberOfVertices()
              << " vertices but there should be 2." << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
