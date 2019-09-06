/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
/**
 * @class   vtkMapper2D
 * @brief   abstract class specifies interface for objects which render 2D actors
 *
 * vtkMapper2D is an abstract class which defines the interface for objects
 * which render two dimensional actors (vtkActor2D).
 *
 * @sa
 * vtkActor2D
*/

#ifndef vtkMapper2D_h
#define vtkMapper2D_h

#include "vtkRenderingCoreModule.h" // For export macro
#include "vtkAbstractMapper.h"

class vtkViewport;
class vtkActor2D;

class VTKRENDERINGCORE_EXPORT vtkMapper2D : public vtkAbstractMapper
{
public:
  vtkTypeMacro(vtkMapper2D, vtkAbstractMapper);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual void RenderOverlay(vtkViewport*, vtkActor2D*) {}
  virtual void RenderOpaqueGeometry(vtkViewport*, vtkActor2D*) {}
  virtual void RenderTranslucentPolygonalGeometry(vtkViewport*, vtkActor2D*) {}
  virtual vtkTypeBool HasTranslucentPolygonalGeometry() { return 0; }

protected:
  vtkMapper2D() {}
  ~vtkMapper2D() override {}

private:
  vtkMapper2D(const vtkMapper2D&) = delete;
  void operator=(const vtkMapper2D&) = delete;
};

#endif
