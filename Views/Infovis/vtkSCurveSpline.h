/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkSCurveSpline
 * @brief   computes an interpolating spline using a
 * a SCurve basis.
 *
 *
 * vtkSCurveSpline is a concrete implementation of vtkSpline using a
 * SCurve basis.
 *
 * @sa
 * vtkSpline vtkKochanekSpline
*/

#ifndef vtkSCurveSpline_h
#define vtkSCurveSpline_h

#include "vtkViewsInfovisModule.h" // For export macro
#include "vtkSpline.h"

class VTKVIEWSINFOVIS_EXPORT vtkSCurveSpline : public vtkSpline
{
public:
  static vtkSCurveSpline *New();

  vtkTypeMacro(vtkSCurveSpline,vtkSpline);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Compute SCurve Splines for each dependent variable
   */
  void Compute () override;

  /**
   * Evaluate a 1D SCurve spline.
   */
  double Evaluate (double t) override;

  /**
   * Deep copy of SCurve spline data.
   */
  void DeepCopy(vtkSpline *s) override;

  vtkSetMacro(NodeWeight,double);
  vtkGetMacro(NodeWeight,double);
protected:
  vtkSCurveSpline();
  ~vtkSCurveSpline() override {}

  double NodeWeight;

private:
  vtkSCurveSpline(const vtkSCurveSpline&) = delete;
  void operator=(const vtkSCurveSpline&) = delete;
};

#endif
