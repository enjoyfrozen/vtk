/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkMPI4PyCommunicator
 * @brief   Class for bridging MPI4Py with vtkMPICommunicator.
 *
 *
 * This class can be used to convert between VTK and MPI4Py communicators.
 *
 * @sa
 * vtkMPICommunicator
*/

#ifndef vtkMPI4PyCommunicator_h
#define vtkMPI4PyCommunicator_h
#if !defined(__VTK_WRAP__) || defined(__VTK_WRAP_PYTHON__)

#include "vtkPython.h" // For PyObject*; must be first

#include "vtkParallelMPI4PyModule.h" // For export macro
#include "vtkObject.h"

class vtkMPICommunicator;

class VTKPARALLELMPI4PY_EXPORT vtkMPI4PyCommunicator : public vtkObject
{
public:

  vtkTypeMacro(vtkMPI4PyCommunicator, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkMPI4PyCommunicator* New();
  vtkMPI4PyCommunicator();

  /**
   * Convert a VTK communicator into an mpi4py communicator.
   */
  static PyObject* ConvertToPython(vtkMPICommunicator* comm);

  /**
   * Convert an mpi4py communicator into a VTK communicator.
   */
  static vtkMPICommunicator* ConvertToVTK(PyObject* comm);

private:
  vtkMPI4PyCommunicator(const vtkMPI4PyCommunicator&) = delete;
  void operator=(const vtkMPI4PyCommunicator&) = delete;
};

#endif
#endif
