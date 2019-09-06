/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkDataSetGradient
 * @brief   computes scalar field gradient
 *
 *
 * vtkDataSetGradient Computes per cell gradient of point scalar field
 * or per point gradient of cell scalar field.
 *
 * @par Thanks:
 * This file is part of the generalized Youngs material interface reconstruction algorithm contributed by
 * CEA/DIF - Commissariat a l'Energie Atomique, Centre DAM Ile-De-France <br>
 * BP12, F-91297 Arpajon, France. <br>
 * Implementation by Thierry Carrard (CEA)
*/

#ifndef vtkDataSetGradient_h
#define vtkDataSetGradient_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkDataSetAlgorithm.h"


class VTKFILTERSGENERAL_EXPORT vtkDataSetGradient : public vtkDataSetAlgorithm
{
 public:
  static vtkDataSetGradient* New();
  vtkTypeMacro(vtkDataSetGradient,vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set/Get the name of computed vector array.
   */
  vtkSetStringMacro(ResultArrayName);
  vtkGetStringMacro(ResultArrayName);
  //@}

 protected:
  vtkDataSetGradient ();
  ~vtkDataSetGradient() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char* ResultArrayName;

 private:
  vtkDataSetGradient(const vtkDataSetGradient&) = delete;
  void operator=(const vtkDataSetGradient&) = delete;
};

#endif /* VTK_DATA_SET_GRADIENT_H */

