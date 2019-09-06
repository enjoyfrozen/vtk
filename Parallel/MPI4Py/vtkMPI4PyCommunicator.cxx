/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/

#include "vtkMPI4PyCommunicator.h"

#include "vtkObjectFactory.h"
#include "vtkMPICommunicator.h"
#include "vtkMPI.h"

#include <mpi4py/mpi4py.h>

vtkStandardNewMacro(vtkMPI4PyCommunicator);

//----------------------------------------------------------------------------
vtkMPI4PyCommunicator::vtkMPI4PyCommunicator()
{
}

//----------------------------------------------------------------------------
PyObject* vtkMPI4PyCommunicator::ConvertToPython(vtkMPICommunicator* comm)
{
  // Import mpi4py if it does not exist.
  if (!PyMPIComm_New)
  {
    if (import_mpi4py() < 0)
    {
      Py_RETURN_NONE;
    }
  }

  if (!comm || !comm->GetMPIComm())
  {
    Py_RETURN_NONE;
  }

  return PyMPIComm_New(*comm->GetMPIComm()->GetHandle());
}

//----------------------------------------------------------------------------
vtkMPICommunicator* vtkMPI4PyCommunicator::ConvertToVTK(PyObject* comm)
{
  // Import mpi4py if it does not exist.
  if (!PyMPIComm_Get)
  {
    if (import_mpi4py() < 0)
    {
      return nullptr;
    }
  }

  if (!comm || !PyObject_TypeCheck(comm, &PyMPIComm_Type))
  {
    return nullptr;
  }

  MPI_Comm *mpiComm = PyMPIComm_Get(comm);
  vtkMPICommunicator* vtkComm = vtkMPICommunicator::New();
  vtkMPICommunicatorOpaqueComm opaqueComm(mpiComm);
  if (!vtkComm->InitializeExternal(&opaqueComm))
  {
    vtkComm->Delete();
    return nullptr;
  }

  return vtkComm;
}

//----------------------------------------------------------------------------
void vtkMPI4PyCommunicator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
