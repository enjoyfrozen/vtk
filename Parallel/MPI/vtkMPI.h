/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/

#ifndef vtkMPI_h
#define vtkMPI_h
#ifndef __VTK_WRAP__

#ifndef USE_STDARG
 #define USE_STDARG
#include "vtkParallelMPIModule.h" // For export macro
 #include "mpi.h"
 #undef USE_STDARG
#else
 #include "mpi.h"
#endif

#include "vtkSystemIncludes.h"

class VTKPARALLELMPI_EXPORT vtkMPICommunicatorOpaqueComm
{
public:
  vtkMPICommunicatorOpaqueComm(MPI_Comm* handle = 0);

  MPI_Comm* GetHandle();

  friend class vtkMPICommunicator;
  friend class vtkMPIController;

protected:
  MPI_Comm* Handle;
};

class VTKPARALLELMPI_EXPORT vtkMPICommunicatorReceiveDataInfo
{
public:
  vtkMPICommunicatorReceiveDataInfo()
  {
    this->Handle=0;
  }
  MPI_Datatype DataType;
  MPI_Status Status;
  MPI_Comm* Handle;
};

class VTKPARALLELMPI_EXPORT vtkMPIOpaqueFileHandle
{
public:
  vtkMPIOpaqueFileHandle() : Handle(MPI_FILE_NULL) { }
  MPI_File Handle;
};

//-----------------------------------------------------------------------------
class vtkMPICommunicatorOpaqueRequest
{
public:
  MPI_Request Handle;
};


#endif
#endif // vtkMPI_h
// VTK-HeaderTest-Exclude: vtkMPI.h
