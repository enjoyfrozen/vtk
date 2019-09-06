/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOpenVRFollower
 * @brief   OpenVR Follower
 *
 * vtkOpenVRFollower a follower that aligns with PhysicalViewUp
*/

#ifndef vtkOpenVRFollower_h
#define vtkOpenVRFollower_h

#include "vtkRenderingOpenVRModule.h" // For export macro
#include "vtkFollower.h"

class VTKRENDERINGOPENVR_EXPORT vtkOpenVRFollower : public vtkFollower
{
public:
  static vtkOpenVRFollower *New();
  vtkTypeMacro(vtkOpenVRFollower, vtkFollower);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual void Render(vtkRenderer *ren) override;

  /**
   * Generate the matrix based on ivars. This method overloads its superclasses
   * ComputeMatrix() method due to the special vtkFollower matrix operations.
   */
  void ComputeMatrix() override;

protected:
  vtkOpenVRFollower();
  ~vtkOpenVRFollower();

  double LastViewUp[3];

private:
  vtkOpenVRFollower(const vtkOpenVRFollower&) = delete;
  void operator=(const vtkOpenVRFollower&) = delete;
};

#endif
