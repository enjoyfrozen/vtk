/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkLinearSubdivisionFilter
 * @brief   generate a subdivision surface using the Linear Scheme
 *
 * vtkLinearSubdivisionFilter is a filter that generates output by
 * subdividing its input polydata. Each subdivision iteration create 4
 * new triangles for each triangle in the polydata.
 *
 * @par Thanks:
 * This work was supported by PHS Research Grant No. 1 P41 RR13218-01
 * from the National Center for Research Resources.
 *
 * @sa
 * vtkInterpolatingSubdivisionFilter vtkButterflySubdivisionFilter
*/

#ifndef vtkLinearSubdivisionFilter_h
#define vtkLinearSubdivisionFilter_h

#include "vtkFiltersModelingModule.h" // For export macro
#include "vtkInterpolatingSubdivisionFilter.h"

class vtkIntArray;
class vtkPointData;
class vtkPoints;
class vtkPolyData;

class VTKFILTERSMODELING_EXPORT vtkLinearSubdivisionFilter : public vtkInterpolatingSubdivisionFilter
{
public:
  //@{
  /**
   * Construct object with NumberOfSubdivisions set to 1.
   */
  static vtkLinearSubdivisionFilter *New();
  vtkTypeMacro(vtkLinearSubdivisionFilter,vtkInterpolatingSubdivisionFilter);
  //@}

protected:
  vtkLinearSubdivisionFilter () {}
  ~vtkLinearSubdivisionFilter () override {}

  int GenerateSubdivisionPoints (vtkPolyData *inputDS,
                                 vtkIntArray *edgeData,
                                 vtkPoints *outputPts,
                                 vtkPointData *outputPD) override;

private:
  vtkLinearSubdivisionFilter(const vtkLinearSubdivisionFilter&) = delete;
  void operator=(const vtkLinearSubdivisionFilter&) = delete;
};

#endif


// VTK-HeaderTest-Exclude: vtkLinearSubdivisionFilter.h
