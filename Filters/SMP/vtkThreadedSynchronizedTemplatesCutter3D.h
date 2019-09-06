/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkThreadedSynchronizedTemplatesCutter3D
 * @brief   generate cut surface from structured points
 *
 *
 * vtkThreadedSynchronizedTemplatesCutter3D is an implementation of the
 * synchronized template algorithm.
 *
 * @sa
 * vtkContourFilter vtkSynchronizedTemplates3D vtkThreadedSynchronizedTemplates3D vtkSynchronizedTemplatesCutter3D
*/

#ifndef vtkThreadedSynchronizedTemplatesCutter3D_h
#define vtkThreadedSynchronizedTemplatesCutter3D_h

#include "vtkFiltersSMPModule.h" // For export macro
#include "vtkThreadedSynchronizedTemplates3D.h"

class vtkImplicitFunction;

#if !defined(VTK_LEGACY_REMOVE)
class VTKFILTERSSMP_EXPORT vtkThreadedSynchronizedTemplatesCutter3D : public vtkThreadedSynchronizedTemplates3D
{
public:
  static vtkThreadedSynchronizedTemplatesCutter3D *New();

  vtkTypeMacro(vtkThreadedSynchronizedTemplatesCutter3D,vtkThreadedSynchronizedTemplates3D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Needed by templated functions.
   */
  void ThreadedExecute(vtkImageData *data, vtkInformation *outInfo, int);

  //@{
  /**
   * Specify the implicit function to perform the cutting.
   */
  virtual void SetCutFunction(vtkImplicitFunction*);
  vtkGetObjectMacro(CutFunction,vtkImplicitFunction);
  //@}

  //@{
  /**
   * Set/get the desired precision for the output types. See the documentation
   * for the vtkAlgorithm::DesiredOutputPrecision enum for an explanation of
   * the available precision settings.
   */
  vtkSetClampMacro(OutputPointsPrecision, int, SINGLE_PRECISION, DEFAULT_PRECISION);
  vtkGetMacro(OutputPointsPrecision, int);
  //@}

  /**
   * Override GetMTime because we delegate to vtkContourValues and refer to
   * vtkImplicitFunction.
   */
  vtkMTimeType GetMTime() override;

protected:
  vtkThreadedSynchronizedTemplatesCutter3D();
  ~vtkThreadedSynchronizedTemplatesCutter3D() override;

  vtkImplicitFunction *CutFunction;
  int OutputPointsPrecision;

  int RequestData(vtkInformation *,
                          vtkInformationVector **,
                          vtkInformationVector *) override;

  int FillOutputPortInformation(int port, vtkInformation* info) override;
private:
  vtkThreadedSynchronizedTemplatesCutter3D(const vtkThreadedSynchronizedTemplatesCutter3D&) = delete;
  void operator=(const vtkThreadedSynchronizedTemplatesCutter3D&) = delete;
};

#endif //VTK_LEGACY_REMOVE
#endif
