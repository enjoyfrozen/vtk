/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkOpenGLVolumeOpacityTable_h
#define vtkOpenGLVolumeOpacityTable_h
#ifndef __VTK_WRAP__

#include "vtkOpenGLVolumeLookupTable.h"

#include "vtkVolumeMapper.h"

// Forward declarations
class vtkOpenGLRenderWindow;

//----------------------------------------------------------------------------
class vtkOpenGLVolumeOpacityTable : public vtkOpenGLVolumeLookupTable
{
public:
  vtkTypeMacro(vtkOpenGLVolumeOpacityTable, vtkOpenGLVolumeLookupTable);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkOpenGLVolumeOpacityTable* New();

protected:
  vtkOpenGLVolumeOpacityTable() = default;

  /**
   * Update the internal texture object using the opacity transfer function
   */
  void InternalUpdate(vtkObject* func,
                      int blendMode,
                      double sampleDistance,
                      double unitDistance,
                      int filterValue) override;

  /**
   * Test whether the internal function needs to be updated.
   */
  bool NeedsUpdate(vtkObject* func,
                   double scalarRange[2],
                   int blendMode,
                   double sampleDistance) override;

  int LastBlendMode = vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND;
  double LastSampleDistance = 1.0;

private:
  vtkOpenGLVolumeOpacityTable(const vtkOpenGLVolumeOpacityTable&) = delete;
  vtkOpenGLVolumeOpacityTable& operator=(const vtkOpenGLVolumeOpacityTable&) =
    delete;
};

#endif // __VTK_WRAP__
#endif // vtkOpenGLVolumeOpacityTable_h
// VTK-HeaderTest-Exclude: vtkOpenGLVolumeOpacityTable.h
