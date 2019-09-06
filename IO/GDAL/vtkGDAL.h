/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkGDAL
 * @brief   Shared data for GDAL classes
 *
*/

#ifndef vtkGDAL_h
#define vtkGDAL_h

#include "vtkObject.h"
#include <vtkIOGDALModule.h> // For export macro

class vtkInformationStringKey;
class vtkInformationIntegerVectorKey;

class VTKIOGDAL_EXPORT vtkGDAL : public vtkObject
{
 public:
  vtkTypeMacro(vtkGDAL,vtkObject);
  // Key used to put GDAL map projection string in the output information
  // by readers.
  static vtkInformationStringKey* MAP_PROJECTION();
  static vtkInformationIntegerVectorKey* FLIP_AXIS();
 protected:

 private:
  vtkGDAL();  // Static class
  ~vtkGDAL() override;
  vtkGDAL(const vtkGDAL&) = delete;
  void operator=(const vtkGDAL&) = delete;
};

#endif // vtkGDAL_h
// VTK-HeaderTest-Exclude: vtkGDAL.h
