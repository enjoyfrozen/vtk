/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkOpenGLVolumeRGBTable_h
#define vtkOpenGLVolumeRGBTable_h
#ifndef __VTK_WRAP__

#include "vtkOpenGLVolumeLookupTable.h"

// Forward declarations
class vtkOpenGLRenderWindow;

//----------------------------------------------------------------------------
class vtkOpenGLVolumeRGBTable : public vtkOpenGLVolumeLookupTable
{
public:
  vtkTypeMacro(vtkOpenGLVolumeRGBTable, vtkOpenGLVolumeLookupTable);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkOpenGLVolumeRGBTable* New();

protected:
  vtkOpenGLVolumeRGBTable();

  /**
   * Update the internal texture object using the color transfer function
   */
  void InternalUpdate(vtkObject* func,
                      int blendMode,
                      double sampleDistance,
                      double unitDistance,
                      int filterValue) override;

private:
  vtkOpenGLVolumeRGBTable(const vtkOpenGLVolumeRGBTable&) = delete;
  vtkOpenGLVolumeRGBTable& operator=(const vtkOpenGLVolumeRGBTable&) = delete;
};

#endif // __VTK_WRAP__
#endif // vtkOpenGLVolumeRGBTable_h
// VTK-HeaderTest-Exclude: vtkOpenGLVolumeRGBTable.h
