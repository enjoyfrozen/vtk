/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkDummyGPUInfoList
 * @brief   Do thing during Probe()
 *
 * vtkDummyGPUInfoList implements Probe() by just setting the count of
 * GPUs to be zero. Useful when an OS specific implementation is not available.
 * @sa
 * vtkGPUInfo vtkGPUInfoList
*/

#ifndef vtkDummyGPUInfoList_h
#define vtkDummyGPUInfoList_h

#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkGPUInfoList.h"

class VTKRENDERINGOPENGL2_EXPORT vtkDummyGPUInfoList : public vtkGPUInfoList
{
public:
  static vtkDummyGPUInfoList* New();
  vtkTypeMacro(vtkDummyGPUInfoList, vtkGPUInfoList);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Build the list of vtkInfoGPU if not done yet.
   * \post probed: IsProbed()
   */
  void Probe() override;

protected:
  //@{
  /**
   * Default constructor.
   */
  vtkDummyGPUInfoList();
  ~vtkDummyGPUInfoList() override;
  //@}

private:
  vtkDummyGPUInfoList(const vtkDummyGPUInfoList&) = delete;
  void operator=(const vtkDummyGPUInfoList&) = delete;
};

#endif
