/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCompositeRenderManager
 * @brief   An object to control sort-last parallel rendering.
 *
 *
 * vtkCompositeRenderManager is a subclass of vtkParallelRenderManager that
 * uses compositing to do parallel rendering.  This class has
 * replaced vtkCompositeManager.
 *
*/

#ifndef vtkCompositeRenderManager_h
#define vtkCompositeRenderManager_h

#include "vtkRenderingParallelModule.h" // For export macro
#include "vtkParallelRenderManager.h"

class vtkCompositer;
class vtkFloatArray;

class VTKRENDERINGPARALLEL_EXPORT vtkCompositeRenderManager : public vtkParallelRenderManager
{
public:
  vtkTypeMacro(vtkCompositeRenderManager, vtkParallelRenderManager);
  static vtkCompositeRenderManager *New();
  void PrintSelf(ostream &os, vtkIndent indent) override;

  //@{
  /**
   * Set/Get the composite algorithm.
   */
  void SetCompositer(vtkCompositer *c);
  vtkGetObjectMacro(Compositer, vtkCompositer);
  //@}

protected:
  vtkCompositeRenderManager();
  ~vtkCompositeRenderManager() override;

  vtkCompositer *Compositer;

  void PreRenderProcessing() override;
  void PostRenderProcessing() override;

  vtkFloatArray *DepthData;
  vtkUnsignedCharArray *TmpPixelData;
  vtkFloatArray *TmpDepthData;

  int SavedMultiSamplesSetting;

private:
  vtkCompositeRenderManager(const vtkCompositeRenderManager &) = delete;
  void operator=(const vtkCompositeRenderManager &) = delete;
};

#endif //vtkCompositeRenderManager_h
