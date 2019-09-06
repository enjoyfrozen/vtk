/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include <cstdlib>

#include "vtkNew.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkPolyData.h"
#include "vtkTestUtilities.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLUnstructuredGridReader.h"

int TestDataSetSurfaceFilterQuadraticTetsGhostCells(int argc, char* argv[])
{
  char *cfname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/quadratic_tets_with_ghost_cells_0.vtu");

  vtkNew<vtkXMLUnstructuredGridReader> reader;
  reader->SetFileName(cfname);
  delete[] cfname;

  vtkNew<vtkDataSetSurfaceFilter> surfaceFilter;
  surfaceFilter->SetInputConnection(reader->GetOutputPort());
  surfaceFilter->Update();

  vtkPolyData* surface = surfaceFilter->GetOutput();
  int numCells = surface->GetNumberOfCells();
  if (numCells != 672)
  {
    std::cerr << "Expected 672 cells, got: " << numCells << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
