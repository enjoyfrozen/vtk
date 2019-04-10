/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDIYUtilities.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDIYUtilities.h"

#include "vtkBoundingBox.h"
#include "vtkLogger.h"
#include "vtkMPI.h"
#include "vtkMPICommunicator.h"
#include "vtkMultiProcessController.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkUnstructuredGrid.h"
#include "vtkXMLDataObjectWriter.h"
#include "vtkXMLUnstructuredGridReader.h"

#include <cassert>

//----------------------------------------------------------------------------
vtkDIYUtilities::vtkDIYUtilities() {}

//----------------------------------------------------------------------------
vtkDIYUtilities::~vtkDIYUtilities() {}

//----------------------------------------------------------------------------
diy::mpi::communicator vtkDIYUtilities::GetCommunicator(vtkMultiProcessController* controller)
{
  vtkMPICommunicator* vtkcomm =
    vtkMPICommunicator::SafeDownCast(controller ? controller->GetCommunicator() : nullptr);
  return vtkcomm ? diy::mpi::communicator(*vtkcomm->GetMPIComm()->GetHandle())
                 : diy::mpi::communicator(MPI_COMM_NULL);
}

//----------------------------------------------------------------------------
void vtkDIYUtilities::AllReduce(diy::mpi::communicator& comm, vtkBoundingBox& bbox)
{
  if (comm.size() > 1)
  {
    std::vector<double> local_minpoint(3), local_maxpoint(3);
    bbox.GetMinPoint(&local_minpoint[0]);
    bbox.GetMaxPoint(&local_maxpoint[0]);

    std::vector<double> global_minpoint(3), global_maxpoint(3);
    diy::mpi::all_reduce(comm, local_minpoint, global_minpoint, diy::mpi::minimum<float>());
    diy::mpi::all_reduce(comm, local_maxpoint, global_maxpoint, diy::mpi::maximum<float>());

    bbox.SetMinPoint(&global_minpoint[0]);
    bbox.SetMaxPoint(&global_maxpoint[0]);
  }
}

//----------------------------------------------------------------------------
void vtkDIYUtilities::Save(diy::BinaryBuffer& bb, vtkDataSet* p)
{
  if (p)
  {
    diy::save(bb, p->GetDataObjectType());
    auto writer = vtkXMLDataObjectWriter::NewWriter(p->GetDataObjectType());
    if (writer)
    {
      writer->WriteToOutputStringOn();
      writer->SetCompressorTypeToLZ4();
      writer->SetEncodeAppendedData(false);
      writer->SetInputDataObject(p);
      writer->Write();
      diy::save(bb, writer->GetOutputString());
      writer->Delete();
    }
    else
    {
      vtkLogF(
        ERROR, "Cannot serialize `%s` yet. Aborting for debugging purposes.", p->GetClassName());
      abort();
    }
  }
  else
  {
    diy::save(bb, static_cast<int>(-1)); // can't be VTK_VOID since VTK_VOID == VTK_POLY_DATA.
  }
}

//----------------------------------------------------------------------------
void vtkDIYUtilities::Load(diy::BinaryBuffer& bb, vtkDataSet*& p)
{
  p = nullptr;
  int type;
  diy::load(bb, type);
  if (type == -1)
  {
    p = nullptr;
  }
  else
  {
    std::string data;
    diy::load(bb, data);

    assert(type == VTK_UNSTRUCTURED_GRID);
    vtkNew<vtkXMLUnstructuredGridReader> reader;
    reader->ReadFromInputStringOn();
    reader->SetInputString(data);
    reader->Update();

    p = vtkUnstructuredGrid::New();
    p->ShallowCopy(reader->GetOutputDataObject(0));
  }
}

//----------------------------------------------------------------------------
diy::ContinuousBounds vtkDIYUtilities::Convert(const vtkBoundingBox& bbox)
{
  if (bbox.IsValid())
  {
    diy::ContinuousBounds bds;
    bds.min[0] = static_cast<float>(bbox.GetMinPoint()[0]);
    bds.min[1] = static_cast<float>(bbox.GetMinPoint()[1]);
    bds.min[2] = static_cast<float>(bbox.GetMinPoint()[2]);
    bds.max[0] = static_cast<float>(bbox.GetMaxPoint()[0]);
    bds.max[1] = static_cast<float>(bbox.GetMaxPoint()[1]);
    bds.max[2] = static_cast<float>(bbox.GetMaxPoint()[2]);
    return bds;
  }
  return diy::ContinuousBounds();
}

//----------------------------------------------------------------------------
vtkBoundingBox vtkDIYUtilities::Convert(const diy::ContinuousBounds& bds)
{
  double bounds[6];
  bounds[0] = bds.min[0];
  bounds[1] = bds.max[0];
  bounds[2] = bds.min[1];
  bounds[3] = bds.max[1];
  bounds[4] = bds.min[2];
  bounds[5] = bds.max[2];
  vtkBoundingBox bbox;
  bbox.SetBounds(bounds);
  return bbox;
}

//----------------------------------------------------------------------------
void vtkDIYUtilities::Broadcast(
  diy::mpi::communicator& comm, std::vector<vtkBoundingBox>& boxes, int source)
{
  // FIXME: can't this be made more elegant?
  std::vector<double> raw_bounds;
  if (comm.rank() == source)
  {
    raw_bounds.resize(6 * boxes.size());
    for (size_t cc = 0; cc < boxes.size(); ++cc)
    {
      boxes[cc].GetBounds(&raw_bounds[6 * cc]);
    }
  }
  diy::mpi::broadcast(comm, raw_bounds, source);
  if (comm.rank() != source)
  {
    boxes.resize(raw_bounds.size() / 6);
    for (size_t cc = 0; cc < boxes.size(); ++cc)
    {
      boxes[cc].SetBounds(&raw_bounds[6 * cc]);
    }
  }
}

//----------------------------------------------------------------------------
void vtkDIYUtilities::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
