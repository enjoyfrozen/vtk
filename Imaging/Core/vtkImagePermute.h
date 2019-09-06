/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImagePermute
 * @brief    Permutes axes of input.
 *
 * vtkImagePermute reorders the axes of the input. Filtered axes specify
 * the input axes which become X, Y, Z.  The input has to have the
 * same scalar type of the output. The filter does copy the
 * data when it executes. This filter is actually a very thin wrapper
 * around vtkImageReslice.
*/

#ifndef vtkImagePermute_h
#define vtkImagePermute_h


#include "vtkImagingCoreModule.h" // For export macro
#include "vtkImageReslice.h"

class VTKIMAGINGCORE_EXPORT vtkImagePermute : public vtkImageReslice
{
public:
  static vtkImagePermute *New();
  vtkTypeMacro(vtkImagePermute,vtkImageReslice);

  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * The filtered axes are the input axes that get relabeled to X,Y,Z.
   */
  void SetFilteredAxes(int x, int y, int z);
  void SetFilteredAxes(const int xyz[3]) {
    this->SetFilteredAxes(xyz[0], xyz[1], xyz[2]); };
  vtkGetVector3Macro(FilteredAxes, int);
  //@}

protected:
  vtkImagePermute();
  ~vtkImagePermute() override {}

  int FilteredAxes[3];

private:
  vtkImagePermute(const vtkImagePermute&) = delete;
  void operator=(const vtkImagePermute&) = delete;
};

#endif



