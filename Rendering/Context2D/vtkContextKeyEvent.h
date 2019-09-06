/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkContextKeyEvent
 * @brief   data structure to represent key events.
 *
 *
 * Provides a convenient data structure to represent key events in the
 * vtkContextScene. Passed to vtkAbstractContextItem objects.
*/

#ifndef vtkContextKeyEvent_h
#define vtkContextKeyEvent_h

#include "vtkRenderingContext2DModule.h" // For export macro
#include "vtkWeakPointer.h" // For vtkWeakPointer
#include "vtkVector.h" // For vtkVector2i

class vtkRenderWindowInteractor;

class VTKRENDERINGCONTEXT2D_EXPORT vtkContextKeyEvent
{
public:
  vtkContextKeyEvent();

  /**
   * Set the interactor for the key event.
   */
  void SetInteractor(vtkRenderWindowInteractor *interactor);

  /**
   * Get the interactor for the key event. This can be null, and is provided
   * only for convenience.
   */
  vtkRenderWindowInteractor* GetInteractor() const;

  /**
   * Set the position of the mouse when the key was pressed.
   */
  void SetPosition(const vtkVector2i& position) { this->Position = position; }

  /**
   * Get the position of the mouse when the key was pressed.
   */
  vtkVector2i GetPosition() const { return this->Position; }

  char GetKeyCode() const;

protected:
  vtkWeakPointer<vtkRenderWindowInteractor> Interactor;
  vtkVector2i Position;
};

#endif // vtkContextKeyEvent_h
// VTK-HeaderTest-Exclude: vtkContextKeyEvent.h
