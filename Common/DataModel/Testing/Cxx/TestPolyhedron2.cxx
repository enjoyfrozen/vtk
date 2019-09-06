/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkUnstructuredGrid.h"
#include "vtkPolyhedron.h"
#include "vtkPlane.h"

#include "vtkTestUtilities.h"
#include "vtkNew.h"
#include "vtkCutter.h"
#include "vtkPlane.h"
#include "vtkXMLUnstructuredGridReader.h"
#include "vtkXMLUnstructuredGridWriter.h"
#include "vtkXMLPolyDataWriter.h"

// Test of contour/clip of vtkPolyhedron. uses input from https://gitlab.kitware.com/vtk/vtk/issues/14485
int TestPolyhedron2( int argc, char* argv[] )
{
  if (argc < 3) return 1; // test not run with data on the command line

  vtkObject::GlobalWarningDisplayOff();

  const char* filename = argv[2];
  vtkNew<vtkXMLUnstructuredGridReader> reader;
  reader->SetFileName(filename);
  reader->Update();

  vtkUnstructuredGrid* pGrid = reader->GetOutput();

  vtkNew<vtkCutter> cutter;
  vtkNew<vtkPlane> p;
  p->SetOrigin(pGrid->GetCenter());
  p->SetNormal(1,0,0);

  cutter->SetCutFunction(p);
  cutter->SetGenerateTriangles(0);

  cutter->SetInputConnection(0, reader->GetOutputPort());
  cutter->Update();

  vtkPolyData* output = vtkPolyData::SafeDownCast(cutter->GetOutputDataObject(0));
  if (output->GetNumberOfCells() != 2)
  {
    std::cerr << "Expected 2 polygons but found " << output->GetNumberOfCells() << " polygons in sliced polyhedron." << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
