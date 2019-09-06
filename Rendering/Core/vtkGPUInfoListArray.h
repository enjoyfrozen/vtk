/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkGPUInfoListArray
 * @brief   Internal class vtkGPUInfoList.
 *
 * vtkGPUInfoListArray is just a PIMPL mechanism for vtkGPUInfoList.
*/

#ifndef vtkGPUInfoListArray_h
#define vtkGPUInfoListArray_h

#include "vtkGPUInfo.h"
#include <vector> // STL Header

class vtkGPUInfoListArray
{
public:
  std::vector<vtkGPUInfo *> v;
};

#endif
// VTK-HeaderTest-Exclude: vtkGPUInfoListArray.h
