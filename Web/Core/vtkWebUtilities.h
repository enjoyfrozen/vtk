/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkWebUtilities
 * @brief   collection of utility functions for ParaView Web.
 *
 * vtkWebUtilities consolidates miscellaneous utility functions useful for
 * Python scripts designed for ParaView Web.
*/

#ifndef vtkWebUtilities_h
#define vtkWebUtilities_h

#include "vtkObject.h"
#include "vtkWebCoreModule.h" // needed for exports
#include <string>

class vtkDataSet;

class VTKWEBCORE_EXPORT vtkWebUtilities : public vtkObject
{
public:
  static vtkWebUtilities* New();
  vtkTypeMacro(vtkWebUtilities, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static std::string WriteAttributesToJavaScript(int field_type, vtkDataSet*);
  static std::string WriteAttributeHeadersToJavaScript(
    int field_type, vtkDataSet*);

  //@{
  /**
   * This method is similar to the ProcessRMIs() method on the GlobalController
   * except that it is Python friendly in the sense that it will release the
   * Python GIS lock, so when run in a thread, this will truly work in the
   * background without locking the main one.
   */
  static void ProcessRMIs();
  static void ProcessRMIs(int reportError, int dont_loop=0);
  //@}

protected:
  vtkWebUtilities();
  ~vtkWebUtilities() override;

private:
  vtkWebUtilities(const vtkWebUtilities&) = delete;
  void operator=(const vtkWebUtilities&) = delete;

};

#endif
// VTK-HeaderTest-Exclude: vtkWebUtilities.h
