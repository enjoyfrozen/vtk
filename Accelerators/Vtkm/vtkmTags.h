/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#ifndef vtkmTags_h
#define vtkmTags_h

#include "vtkCellArray.h"
#include "vtkPoints.h"
#include "vtkAOSDataArrayTemplate.h"
#include "vtkSOADataArrayTemplate.h"

#include "vtkmConfig.h" //required for general vtkm setup

namespace tovtkm {

struct VTKM_ALWAYS_EXPORT vtkSOAArrayContainerTag
{
};

//this tag is used to construct points coordinates
struct VTKM_ALWAYS_EXPORT vtkCellArrayContainerTag
{
};

template<typename T> struct VTKM_ALWAYS_EXPORT ArrayContainerTagType;

template<typename T>
struct VTKM_ALWAYS_EXPORT ArrayContainerTagType< vtkSOADataArrayTemplate< T > >
{
  typedef vtkSOAArrayContainerTag TagType;
};


}

#endif
// VTK-HeaderTest-Exclude: vtkmTags.h
