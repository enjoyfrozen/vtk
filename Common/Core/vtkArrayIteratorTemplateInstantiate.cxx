/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#define vtkArrayIteratorTemplateInstantiate_cxx

#include "vtkArrayIteratorTemplate.txx"

vtkInstantiateTemplateMacro(
  template class VTKCOMMONCORE_EXPORT vtkArrayIteratorTemplate)
template class VTKCOMMONCORE_EXPORT vtkArrayIteratorTemplate<vtkStdString>;
template class VTKCOMMONCORE_EXPORT vtkArrayIteratorTemplate<vtkUnicodeString>;
template class VTKCOMMONCORE_EXPORT vtkArrayIteratorTemplate<vtkVariant>;
