/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSMPTransform
 * @brief   Transform that uses the SMP framework
 *
 * Just like its parent, vtkTransform, vtkSMPTransform calculates and
 * manages transforms. Its main difference is that it performs various
 * transform operations over a set of points in parallel using the SMP
 * framework.
 * @sa
 * vtkTransform
*/

#ifndef vtkSMPTransform_h
#define vtkSMPTransform_h

#include "vtkFiltersSMPModule.h" // For export macro
#include "vtkTransform.h"

#if !defined(VTK_LEGACY_REMOVE)
class VTKFILTERSSMP_EXPORT vtkSMPTransform : public vtkTransform
{
 public:
  static vtkSMPTransform *New();
  vtkTypeMacro(vtkSMPTransform, vtkTransform);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Apply the transformation to a series of points, and append the
   * results to outPts.
   */
  void TransformPoints(vtkPoints *inPts, vtkPoints *outPts) override;

  /**
   * Apply the transformation to a series of normals, and append the
   * results to outNms.
   */
  void TransformNormals(vtkDataArray *inNms, vtkDataArray *outNms) override;

  /**
   * Apply the transformation to a series of vectors, and append the
   * results to outVrs.
   */
  void TransformVectors(vtkDataArray *inVrs, vtkDataArray *outVrs) override;

  /**
   * Apply the transformation to a combination of points, normals
   * and vectors.
   */
  void TransformPointsNormalsVectors(vtkPoints *inPts,
                                     vtkPoints *outPts,
                                     vtkDataArray *inNms,
                                     vtkDataArray *outNms,
                                     vtkDataArray *inVrs,
                                     vtkDataArray *outVrs,
                                     int nOptionalVectors = 0,
                                     vtkDataArray** inVrsArr = nullptr,
                                     vtkDataArray** outVrsArr = nullptr) override;

protected:
  vtkSMPTransform ();
  ~vtkSMPTransform () override {}

private:
  vtkSMPTransform (const vtkSMPTransform&) = delete;
  void operator=(const vtkSMPTransform&) = delete;
};

#endif //VTK_LEGACY_REMOVE
#endif
