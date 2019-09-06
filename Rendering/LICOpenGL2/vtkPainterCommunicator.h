/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class vtkPainterCommunicator
 * @brief A communicator that can safely be used inside a painter.
 *
 * A simple container holding a handle to an MPI communicator.
 * This API is sufficient to allow for control flow with/without
 * MPI. The parallel parts of the code should use the derived
 * class vtkPPainterCommunicator.
*/

#ifndef vtkPainterCommunicator_h
#define vtkPainterCommunicator_h

#include "vtkRenderingLICOpenGL2Module.h" // for export macro

class VTKRENDERINGLICOPENGL2_EXPORT vtkPainterCommunicator
{
public:
  vtkPainterCommunicator(){}
  virtual ~vtkPainterCommunicator(){}

  /**
   * Copy and assignment operators. Both use Copy internally
   * and do take ownership.
   */
  vtkPainterCommunicator(const vtkPainterCommunicator &other)
    { this->Copy(&other, false); }

  vtkPainterCommunicator &operator=(const vtkPainterCommunicator &other)
    { this->Copy(&other, false); return *this; }

  /**
   * Copy the communicator, the flag indicates if ownership
   * should be assumed. The owner is responsible for free'ing
   * the communicator.
   */
  virtual void Copy(const vtkPainterCommunicator *, bool){}

  /**
   * Duplicate the communicator.
   */
  virtual void Duplicate(const vtkPainterCommunicator *){}

  /**
   * Querry MPI about the communicator.
   */
  virtual int GetRank(){ return 0; }
  virtual int GetSize(){ return 1; }
  virtual bool GetIsNull(){ return false; }

  /**
   * Querry MPI about the world communicator.
   */
  virtual int GetWorldRank(){ return 0; }
  virtual int GetWorldSize(){ return 1; }

  /**
   * Querry MPI about its state.
   */
  virtual bool GetMPIInitialized(){ return false; }
  virtual bool GetMPIFinalized(){ return true; }
};

#endif
// VTK-HeaderTest-Exclude: vtkPainterCommunicator.h
