/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkScaledTextActor
 * @brief   create text that will scale as needed
 *
 * vtkScaledTextActor is deprecated. New code should use vtkTextActor with
 * the Scaled = true option.
 *
 * @sa
 * vtkTextActor vtkActor2D vtkTextMapper
*/

#ifndef vtkScaledTextActor_h
#define vtkScaledTextActor_h

#include "vtkRenderingFreeTypeModule.h" // For export macro
#include "vtkTextActor.h"

class VTKRENDERINGFREETYPE_EXPORT vtkScaledTextActor : public vtkTextActor
{
public:
  vtkTypeMacro(vtkScaledTextActor,vtkTextActor);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Instantiate object with a rectangle in normaled view coordinates
   * of (0.2,0.85, 0.8, 0.95).
   */
  static vtkScaledTextActor *New();

protected:
   vtkScaledTextActor();
private:
  vtkScaledTextActor(const vtkScaledTextActor&) = delete;
  void operator=(const vtkScaledTextActor&) = delete;
};


#endif

