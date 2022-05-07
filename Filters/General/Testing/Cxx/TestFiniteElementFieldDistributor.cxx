#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkFiniteElementFieldDistributor.h"
#include "vtkInformation.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkTypeFloat64Array.h"
#include "vtkUnstructuredGrid.h"

#include <cstdlib>

vtkPartitionedDataSetCollection* TestFiniteElementFieldDistributor_Make2dQuads()
{
  auto output = vtkPartitionedDataSetCollection::New();
  vtkNew<vtkUnstructuredGrid> mesh, edges;
  vtkNew<vtkPoints> points;
  vtkNew<vtkCellArray> quads, lines;
  vtkNew<vtkTypeFloat64Array> pressureArr, velocityArr;
  vtkIdType edgeIds[7][2] = { { 0, 1 }, { 1, 2 }, { 2, 3 }, { 3, 0 }, { 1, 4 }, { 4, 5 },
    { 5, 2 } };
  double velocity[7][2] = { { 3, 4 }, { -4, 5 }, { -4, 3 }, { -3, 2 }, { 4, -4 }, { 1, 1 },
    { -5, -5 } };
  vtkIdType faces[2][4] = { { 0, 1, 2, 3 }, { 1, 4, 5, 2 } };
  double pressure[2][4] = { { -4, 3, 5, -3 }, { -4, 4, 5, 2 } };

  points->InsertNextPoint(-1, -1, 0);
  points->InsertNextPoint(1, -1, 0);
  points->InsertNextPoint(1, 1, 0);
  points->InsertNextPoint(-1, 1, 0);
  points->InsertNextPoint(2, -1, 0);
  points->InsertNextPoint(2, 1, 0);

  mesh->SetPoints(points);
  edges->SetPoints(points);

  // prepare the faces.
  quads->InsertNextCell(4, faces[0]);
  quads->InsertNextCell(4, faces[1]);
  // prepare the edges.
  for (int e = 0; e < 7; ++e)
  {
    lines->InsertNextCell(2, edgeIds[e]);
  }
  mesh->SetCells(VTK_QUAD, quads);
  edges->SetCells(VTK_LINE, lines);

  // velocity on every edge.
  velocityArr->SetName("Velocity"); // note the magic text prefix '__vtk_edge__'
  velocityArr->SetComponentName(0, "X");
  velocityArr->SetComponentName(1, "Y");
  velocityArr->SetNumberOfComponents(2);
  velocityArr->SetNumberOfTuples(7);
  for (int i = 0; i < 7; ++i)
  {
    velocityArr->SetTypedTuple(i, velocity[i]);
  }
  edges->GetCellData()->AddArray(velocityArr);

  // pressure in each quad.
  pressureArr->SetName("Pressure");
  pressureArr->SetNumberOfComponents(4); // no. components == no. of cell points.
  pressureArr->SetNumberOfTuples(2);
  for (int i = 0; i < 2; ++i)
  {
    pressureArr->SetTypedTuple(i, pressure[i]);
  }
  mesh->GetCellData()->AddArray(pressureArr);

  vtkNew<vtkPartitionedDataSet> elementsPds, edgesPds;
  int pdsIdx = 0;
  {
    int partIdx = 0;
    elementsPds->SetPartition(pdsIdx, mesh);
    output->SetPartitionedDataSet(pdsIdx, elementsPds);
    // pressure is defined as a HGRAD DG cell-centered field on quad vertices (0D)
    //    output->GetMetaData(pdsIdx)->Set(vtkFiniteElementFieldDistributor::FEM_BLOCK_TYPE(),
    //      int(vtkFiniteElementFieldDistributor::FEMBlockType::ELEMENTS));
    //    output->GetMetaData(pdsIdx)->Set(
    //      vtkFiniteElementFieldDistributor::HGRAD_REF_ELEMENT(), int(VTKCellType::VTK_QUAD));
    //    output->GetMetaData(pdsIdx)->Set(
    //      vtkFiniteElementFieldDistributor::HGRAD_DG_FIELDS(), "Pressure", 0);
    //    output->GetMetaData(pdsIdx)->Set(vtkFiniteElementFieldDistributor::HGRAD_DG_BASIS_ORDER(),
    //    1);
    edgesPds->SetPartition(partIdx, edges);
  }
  ++pdsIdx;
  {
    output->SetPartitionedDataSet(pdsIdx, edgesPds);
    // velocity is defined as a CG HCURL field on edges of the quads (1D)
    //    output->GetMetaData(pdsIdx)->Set(vtkFiniteElementFieldDistributor::FEM_BLOCK_TYPE(),
    //      int(vtkFiniteElementFieldDistributor::FEMBlockType::EDGES));
    //    output->GetMetaData(pdsIdx)->Set(
    //      vtkFiniteElementFieldDistributor::HCURL_REF_ELEMENT(), int(VTKCellType::VTK_QUAD));
    //    output->GetMetaData(pdsIdx)->Set(
    //      vtkFiniteElementFieldDistributor::HCURL_CG_FIELDS(), "Velocity", 0);
    //    output->GetMetaData(pdsIdx)->Set(vtkFiniteElementFieldDistributor::HCURL_CG_BASIS_ORDER(),
    //    1);
  }
  return output;
}

//----------------------------------------------------------------------------
int TestFiniteElementFieldDistributor(int, char*[])
{
  int retVal = EXIT_SUCCESS;
  vtkNew<vtkFiniteElementFieldDistributor> filter;

  auto input = vtk::TakeSmartPointer(TestFiniteElementFieldDistributor_Make2dQuads());
  filter->SetInputDataObject(input);
  filter->Update();
  auto out = filter->GetOutput();
  out->Print(std::cout);
  return retVal;
}
