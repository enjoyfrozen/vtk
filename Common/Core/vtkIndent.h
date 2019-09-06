/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkIndent
 * @brief   a simple class to control print indentation
 *
 * vtkIndent is used to control indentation during the chaining print
 * process. This way nested objects can correctly indent themselves.
*/

#ifndef vtkIndent_h
#define vtkIndent_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkSystemIncludes.h"

class vtkIndent;
VTKCOMMONCORE_EXPORT ostream& operator<<(ostream& os, const vtkIndent& o);

class VTKCOMMONCORE_EXPORT vtkIndent
{
public:
  void Delete() {delete this;};
  explicit vtkIndent(int ind=0) {this->Indent=ind;};
  static vtkIndent *New();

  /**
   * Determine the next indentation level. Keep indenting by two until the
   * max of forty.
   */
  vtkIndent GetNextIndent();

  /**
   * Print out the indentation. Basically output a bunch of spaces.
   */
  friend VTKCOMMONCORE_EXPORT ostream& operator<<(ostream& os, const vtkIndent& o);

protected:
  int Indent;

};

#endif
// VTK-HeaderTest-Exclude: vtkIndent.h
