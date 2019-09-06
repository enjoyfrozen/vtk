/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/


/**
 * @class   vtkOpenGLProjectedTetrahedraMapper
 * @brief   OpenGL implementation of PT
 *
 * @bug
 * This mapper relies highly on the implementation of the OpenGL pipeline.
 * A typical hardware driver has lots of options and some settings can
 * cause this mapper to produce artifacts.
 *
*/

#ifndef vtkOpenGLProjectedTetrahedraMapper_h
#define vtkOpenGLProjectedTetrahedraMapper_h

#include "vtkRenderingVolumeOpenGL2Module.h" // For export macro
#include "vtkProjectedTetrahedraMapper.h"

#include "vtkOpenGLHelper.h" // used for ivars

class vtkVisibilitySort;
class vtkUnsignedCharArray;
class vtkFloatArray;
class vtkRenderWindow;
class vtkOpenGLFramebufferObject;
class vtkOpenGLRenderWindow;
class vtkOpenGLVertexBufferObject;

class VTKRENDERINGVOLUMEOPENGL2_EXPORT vtkOpenGLProjectedTetrahedraMapper
  : public vtkProjectedTetrahedraMapper
{
public:
  vtkTypeMacro(vtkOpenGLProjectedTetrahedraMapper,
                       vtkProjectedTetrahedraMapper);
  static vtkOpenGLProjectedTetrahedraMapper *New();
  void PrintSelf(ostream &os, vtkIndent indent) override;

  void ReleaseGraphicsResources(vtkWindow *window) override;

  void Render(vtkRenderer *renderer, vtkVolume *volume) override;

  //@{
  /**
   * Set/get whether to use floating-point rendering buffers rather
   * than the default.
   */
  vtkSetMacro(UseFloatingPointFrameBuffer,bool);
  vtkGetMacro(UseFloatingPointFrameBuffer,bool);
  vtkBooleanMacro(UseFloatingPointFrameBuffer,bool);
  //@}

  /**
   * Return true if the rendering context provides
   * the nececessary functionality to use this class.
   */
  bool IsSupported(vtkRenderWindow *context) override;

protected:
  vtkOpenGLProjectedTetrahedraMapper();
  ~vtkOpenGLProjectedTetrahedraMapper() override;

  void Initialize(vtkRenderer *ren);
  bool Initialized;
  int  CurrentFBOWidth, CurrentFBOHeight;
  bool AllocateFOResources(vtkRenderer *ren);
  bool CanDoFloatingPointFrameBuffer;
  bool FloatingPointFrameBufferResourcesAllocated;
  bool UseFloatingPointFrameBuffer;
  bool HasHardwareSupport;

  vtkUnsignedCharArray *Colors;
  int UsingCellColors;

  vtkFloatArray *TransformedPoints;

  float MaxCellSize;
  vtkTimeStamp InputAnalyzedTime;
  vtkTimeStamp ColorsMappedTime;

  // The VBO and its layout.
  vtkOpenGLVertexBufferObject *VBO;

  // Structures for the various cell types we render.
  vtkOpenGLHelper Tris;

  int GaveError;

  vtkVolumeProperty *LastProperty;

  vtkOpenGLFramebufferObject *Framebuffer;

  float *SqrtTable;
  float SqrtTableBias;

  virtual void ProjectTetrahedra(vtkRenderer *renderer, vtkVolume *volume,
    vtkOpenGLRenderWindow* renWin);

  float GetCorrectedDepth(float x, float y, float z1, float z2,
                          const float inverse_projection_mat[16],
                          int use_linear_depth_correction,
                          float linear_depth_correction);

  /**
   * Update progress ensuring that OpenGL state is saved and restored before
   * invoking progress.
   */
  void GLSafeUpdateProgress(double value, vtkOpenGLRenderWindow* context);

private:
  vtkOpenGLProjectedTetrahedraMapper(const vtkOpenGLProjectedTetrahedraMapper &) = delete;
  void operator=(const vtkOpenGLProjectedTetrahedraMapper &) = delete;

  class vtkInternals;
  vtkInternals *Internals;
};

#endif
