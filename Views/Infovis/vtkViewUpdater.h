/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkViewUpdater
 * @brief   Updates views automatically
 *
 *
 * vtkViewUpdater registers with annotation change events for a set of
 * annotation links, and updates all views when an annotation link fires an
 * annotation changed event. This is often needed when multiple views share
 * a selection with vtkAnnotationLink.
*/

#ifndef vtkViewUpdater_h
#define vtkViewUpdater_h

#include "vtkViewsInfovisModule.h" // For export macro
#include "vtkObject.h"

class vtkAnnotationLink;
class vtkView;

class VTKVIEWSINFOVIS_EXPORT vtkViewUpdater : public vtkObject
{
public:
  static vtkViewUpdater *New();
  vtkTypeMacro(vtkViewUpdater, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void AddView(vtkView* view);
  void RemoveView(vtkView* view);

  void AddAnnotationLink(vtkAnnotationLink* link);

protected:
  vtkViewUpdater();
  ~vtkViewUpdater() override;

private:
  vtkViewUpdater(const vtkViewUpdater&) = delete;
  void operator=(const vtkViewUpdater&) = delete;

  class vtkViewUpdaterInternals;
  vtkViewUpdaterInternals* Internals;

};

#endif
