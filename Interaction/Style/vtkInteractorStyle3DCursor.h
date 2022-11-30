#ifndef vtkInteractorStyle3DCursor_h
#define vtkInteractorStyle3DCursor_h

#include "vtkInteractionStyleModule.h" // for export macro
#include "vtkInteractorStyleSwitchBase.h" // for inheritance
#include "vtkNew.h" // for vtkNew

VTK_ABI_NAMESPACE_BEGIN

class vtkActor;
class vtkCallbackCommand;

// Pickers
class vtkHardwarePicker;
class vtkWorldPointPicker;

class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyle3DCursor : public vtkInteractorStyleSwitchBase
{
public:
  static vtkInteractorStyle3DCursor* New();
  vtkTypeMacro(vtkInteractorStyle3DCursor, vtkInteractorStyleSwitchBase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Overriden to add a new callback as passive observer
   */
  void SetInteractor(vtkRenderWindowInteractor* interactor) override;

  void PlaceCursor();

protected:
  vtkInteractorStyle3DCursor();
  ~vtkInteractorStyle3DCursor() override = default;

  void SetCurrentRenderer(vtkRenderer*) override;

  vtkNew<vtkActor> Cursor;
  vtkNew<vtkCallbackCommand> Place3DCursor;
  vtkNew<vtkHardwarePicker> Picker;

private:
  vtkInteractorStyle3DCursor(const vtkInteractorStyle3DCursor&) = delete;
  void operator=(const vtkInteractorStyle3DCursor&) = delete;
};
VTK_ABI_NAMESPACE_END

#endif // vtkInteractorStyle3DCursor_h
