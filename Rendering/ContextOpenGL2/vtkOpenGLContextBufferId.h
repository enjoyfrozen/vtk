/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkOpenGLContextBufferId
 * @brief   2D array of ids stored in VRAM.
 *
 *
 * An 2D array where each element is the id of an entity drawn at the given
 * pixel.
*/

#ifndef vtkOpenGLContextBufferId_h
#define vtkOpenGLContextBufferId_h

#include "vtkRenderingContextOpenGL2Module.h" // For export macro
#include "vtkAbstractContextBufferId.h"

class vtkTextureObject;
class vtkOpenGLRenderWindow;

class VTKRENDERINGCONTEXTOPENGL2_EXPORT vtkOpenGLContextBufferId : public vtkAbstractContextBufferId
{
public:
  vtkTypeMacro(vtkOpenGLContextBufferId, vtkAbstractContextBufferId);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  /**
   * Creates a 2D Painter object.
   */
  static vtkOpenGLContextBufferId *New();

  /**
   * Release any graphics resources that are being consumed by this object.
   */
  void ReleaseGraphicsResources() override;

  //@{
  /**
   * Set/Get the OpenGL context owning the texture object resource.
   */
  void SetContext(vtkRenderWindow *context) override;
  vtkRenderWindow *GetContext() override;
  //@}

  /**
   * Returns if the context supports the required extensions.
   * \pre context_is_set: this->GetContext()!=0
   */
  bool IsSupported() override;

  /**
   * Allocate the memory for at least Width*Height elements.
   * \pre positive_width: GetWidth()>0
   * \pre positive_height: GetHeight()>0
   * \pre context_is_set: this->GetContext()!=0
   */
  void Allocate() override;

  /**
   * Tell if the buffer has been allocated.
   */
  bool IsAllocated() const override;

  /**
   * Copy the contents of the current read buffer to the internal texture
   * starting at lower left corner of the framebuffer (srcXmin,srcYmin).
   * \pre is_allocated: this->IsAllocated()
   */
  void SetValues(int srcXmin,
                         int srcYmin) override;

  /**
   * Return item under abscissa x and ordinate y.
   * Abscissa go from left to right.
   * Ordinate go from bottom to top.
   * The return value is -1 if there is no item.
   * \pre is_allocated: IsAllocated()
   * \post valid_result: result>=-1
   */
  vtkIdType GetPickedItem(int x, int y) override;

protected:
  vtkOpenGLContextBufferId();
  ~vtkOpenGLContextBufferId() override;

  vtkOpenGLRenderWindow *Context;
  vtkTextureObject *Texture;

private:
  vtkOpenGLContextBufferId(const vtkOpenGLContextBufferId &) = delete;
  void operator=(const vtkOpenGLContextBufferId &) = delete;
};

#endif // #ifndef vtkOpenGLContextBufferId_h
