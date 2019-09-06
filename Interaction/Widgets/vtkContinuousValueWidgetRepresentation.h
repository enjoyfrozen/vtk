/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/


/**
 * @class   vtkContinuousValueWidgetRepresentation
 * @brief   provide the representation for a continuous value
 *
 * This class is used mainly as a superclass for continuous value widgets
 *
*/

#ifndef vtkContinuousValueWidgetRepresentation_h
#define vtkContinuousValueWidgetRepresentation_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkWidgetRepresentation.h"

class VTKINTERACTIONWIDGETS_EXPORT vtkContinuousValueWidgetRepresentation :
  public vtkWidgetRepresentation
{
public:
  //@{
  /**
   * Standard methods for the class.
   */
  vtkTypeMacro(vtkContinuousValueWidgetRepresentation,
                       vtkWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  //@{
  /**
   * Methods to interface with the vtkSliderWidget. The PlaceWidget() method
   * assumes that the parameter bounds[6] specifies the location in display
   * space where the widget should be placed.
   */
  void PlaceWidget(double bounds[6]) override;
  void BuildRepresentation() override {}
  void StartWidgetInteraction(double eventPos[2]) override = 0;
  void WidgetInteraction(double eventPos[2]) override = 0;
//  virtual void Highlight(int);
  //@}

  // Enums are used to describe what is selected
  enum _InteractionState
  {
    Outside=0,
    Inside,
    Adjusting
  };

  // Set/Get the value
  virtual void SetValue(double value);
  virtual double GetValue() {return this->Value;};

protected:
  vtkContinuousValueWidgetRepresentation();
  ~vtkContinuousValueWidgetRepresentation() override;

  double Value;

private:
  vtkContinuousValueWidgetRepresentation
  (const vtkContinuousValueWidgetRepresentation&) = delete;
  void operator=(const vtkContinuousValueWidgetRepresentation&) = delete;
};

#endif
