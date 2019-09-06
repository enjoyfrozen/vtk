/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkFrameBufferObjectBase
 * @brief   abstract interface to OpenGL FBOs
 *
 * API for classes that encapsulate an OpenGL Frame Buffer Object.
*/

#ifndef vtkFrameBufferObjectBase_h
#define vtkFrameBufferObjectBase_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkObject.h"

class vtkRenderer;
class vtkProp;
class vtkInformation;

class VTKRENDERINGCORE_EXPORT vtkFrameBufferObjectBase : public vtkObject
{
 public:
  vtkTypeMacro(vtkFrameBufferObjectBase, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Dimensions in pixels of the framebuffer.
   */
  virtual int *GetLastSize() = 0;
  virtual void GetLastSize (int &_arg1, int &_arg2) = 0;
  virtual void GetLastSize (int _arg[2]) = 0;
  //@}

protected:
  vtkFrameBufferObjectBase(); // no default constructor.
  ~vtkFrameBufferObjectBase() override;

private:
  vtkFrameBufferObjectBase(const vtkFrameBufferObjectBase &) = delete;
  void operator=(const vtkFrameBufferObjectBase &) = delete;
};

#endif
