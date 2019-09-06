/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkLinearContourLineInterpolator
 * @brief   Interpolates supplied nodes with line segments
 *
 * The line interpolator interpolates supplied nodes (see InterpolateLine)
 * with line segments. The fineness of the curve may be controlled using
 * SetMaximumCurveError and SetMaximumNumberOfLineSegments.
 *
 * @sa
 * vtkContourLineInterpolator
*/

#ifndef vtkLinearContourLineInterpolator_h
#define vtkLinearContourLineInterpolator_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkContourLineInterpolator.h"

class VTKINTERACTIONWIDGETS_EXPORT vtkLinearContourLineInterpolator
                          : public vtkContourLineInterpolator
{
public:

  /**
   * Instantiate this class.
   */
  static vtkLinearContourLineInterpolator *New();

  //@{
  /**
   * Standard methods for instances of this class.
   */
  vtkTypeMacro(vtkLinearContourLineInterpolator,vtkContourLineInterpolator);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  int InterpolateLine( vtkRenderer *ren,
                               vtkContourRepresentation *rep,
                               int idx1, int idx2 ) override;

protected:
  vtkLinearContourLineInterpolator();
  ~vtkLinearContourLineInterpolator() override;

private:
  vtkLinearContourLineInterpolator(const vtkLinearContourLineInterpolator&) = delete;
  void operator=(const vtkLinearContourLineInterpolator&) = delete;
};

#endif
