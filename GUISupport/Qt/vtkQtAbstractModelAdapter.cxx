/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkObject.h" // For vtkGenericWarningMacro
#include "vtkQtAbstractModelAdapter.h"

int vtkQtAbstractModelAdapter::ModelColumnToFieldDataColumn(int col) const
{
  int result = -1;
  switch (this->ViewType)
  {
    case FULL_VIEW:
      result = col;
      break;
    case DATA_VIEW:
      result = this->DataStartColumn + col;
      break;
    default:
      vtkGenericWarningMacro("vtkQtAbstractModelAdapter: Bad view type.");
      break;
  };
  return result;
}
