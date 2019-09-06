/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkQtInitialization
 * @brief   Initializes a Qt application.
 *
 *
 * Utility class that initializes Qt by creating an instance of
 * QApplication in its ctor, if one doesn't already exist.
 * This is mainly of use in ParaView with filters that use Qt in
 * their implementation - create an instance of vtkQtInitialization
 * prior to instantiating any filters that require Qt.
*/

#ifndef vtkQtInitialization_h
#define vtkQtInitialization_h

#include "vtkRenderingQtModule.h" // For export macro
#include "vtkObject.h"

class QApplication;

class VTKRENDERINGQT_EXPORT vtkQtInitialization : public vtkObject
{
public:
  static vtkQtInitialization* New();
  vtkTypeMacro(vtkQtInitialization, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkQtInitialization();
  ~vtkQtInitialization() override;

private:
  vtkQtInitialization(const vtkQtInitialization &) = delete;
  void operator=(const vtkQtInitialization &) = delete;

  QApplication *Application;

};

#endif // vtkQtInitialization_h

