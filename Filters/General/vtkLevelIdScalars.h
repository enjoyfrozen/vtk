/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkLevelIdScalars
 *
 *
 *  Empty class for backwards compatibility.
*/

#ifndef vtkLevelIdScalars_h
#define vtkLevelIdScalars_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkOverlappingAMRLevelIdScalars.h"

class VTKFILTERSGENERAL_EXPORT vtkLevelIdScalars :
  public vtkOverlappingAMRLevelIdScalars
{
  public:
    static vtkLevelIdScalars* New();
    vtkTypeMacro(vtkLevelIdScalars,vtkOverlappingAMRLevelIdScalars);
    void PrintSelf(ostream& os, vtkIndent indent) override;

  protected:
    vtkLevelIdScalars();
    ~vtkLevelIdScalars() override;

  private:
    vtkLevelIdScalars(const vtkLevelIdScalars&) = delete;
    void operator=(const vtkLevelIdScalars&) = delete;
};

#endif /* VTKLEVELIDSCALARS_H_ */
