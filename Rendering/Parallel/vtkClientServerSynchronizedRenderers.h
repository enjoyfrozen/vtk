/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkClientServerSynchronizedRenderers
 *
 * vtkClientServerSynchronizedRenderers is a vtkSynchronizedRenderers subclass
 * designed to be used in 2 processes, client-server mode.
*/

#ifndef vtkClientServerSynchronizedRenderers_h
#define vtkClientServerSynchronizedRenderers_h

#include "vtkRenderingParallelModule.h" // For export macro
#include "vtkSynchronizedRenderers.h"

class VTKRENDERINGPARALLEL_EXPORT vtkClientServerSynchronizedRenderers :
  public vtkSynchronizedRenderers
{
public:
  static vtkClientServerSynchronizedRenderers* New();
  vtkTypeMacro(vtkClientServerSynchronizedRenderers, vtkSynchronizedRenderers);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkClientServerSynchronizedRenderers();
  ~vtkClientServerSynchronizedRenderers() override;

  void MasterEndRender() override;
  void SlaveEndRender() override;

private:
  vtkClientServerSynchronizedRenderers(const vtkClientServerSynchronizedRenderers&) = delete;
  void operator=(const vtkClientServerSynchronizedRenderers&) = delete;

};

#endif
