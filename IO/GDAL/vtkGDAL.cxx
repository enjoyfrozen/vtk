/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkGDAL.h"
#include "vtkInformationKey.h"
#include "vtkInformationIntegerVectorKey.h"
#include "vtkInformationStringKey.h"

vtkInformationKeyMacro(vtkGDAL, MAP_PROJECTION, String);
vtkInformationKeyRestrictedMacro(vtkGDAL, FLIP_AXIS, IntegerVector, 3);
