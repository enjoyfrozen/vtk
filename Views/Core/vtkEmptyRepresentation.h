/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkEmptyRepresentation
 *
 *
*/

#ifndef vtkEmptyRepresentation_h
#define vtkEmptyRepresentation_h

#include "vtkViewsCoreModule.h" // For export macro
#include "vtkDataRepresentation.h"
#include "vtkSmartPointer.h" // For SP ivars

class vtkConvertSelectionDomain;

class VTKVIEWSCORE_EXPORT vtkEmptyRepresentation : public vtkDataRepresentation
{
public:
  static vtkEmptyRepresentation* New();
  vtkTypeMacro(vtkEmptyRepresentation, vtkDataRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Since this representation has no inputs, override superclass
   * implementation with one that ignores "port" and "conn" and still allows it
   * to have an annotation output.
   */
  vtkAlgorithmOutput* GetInternalAnnotationOutputPort() override
    { return this->GetInternalAnnotationOutputPort(0); }
  vtkAlgorithmOutput* GetInternalAnnotationOutputPort(int port) override
    { return this->GetInternalAnnotationOutputPort(port, 0); }
  vtkAlgorithmOutput* GetInternalAnnotationOutputPort(int port, int conn) override;

protected:
  vtkEmptyRepresentation();
  ~vtkEmptyRepresentation() override;

private:
  vtkEmptyRepresentation(const vtkEmptyRepresentation&) = delete;
  void operator=(const vtkEmptyRepresentation&) = delete;

  vtkSmartPointer<vtkConvertSelectionDomain> ConvertDomains;

};

#endif

