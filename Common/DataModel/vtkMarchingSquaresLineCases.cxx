/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/

#include "vtkMarchingSquaresLineCases.h"

// Note: the following code is placed here to deal with cross-library
// symbol export and import on Microsoft compilers.
static vtkMarchingSquaresLineCases VTK_MARCHING_SQUARES_LINECASES[] = {
  {{-1, -1, -1, -1, -1}},
  {{0, 3, -1, -1, -1}},
  {{1, 0, -1, -1, -1}},
  {{1, 3, -1, -1, -1}},
  {{2, 1, -1, -1, -1}},
  {{0, 3, 2, 1, -1}},
  {{2, 0, -1, -1, -1}},
  {{2, 3, -1, -1, -1}},
  {{3, 2, -1, -1, -1}},
  {{0, 2, -1, -1, -1}},
  {{1, 0, 3, 2, -1}},
  {{1, 2, -1, -1, -1}},
  {{3, 1, -1, -1, -1}},
  {{0, 1, -1, -1, -1}},
  {{3, 0, -1, -1, -1}},
  {{-1, -1, -1, -1, -1}}
};

vtkMarchingSquaresLineCases* vtkMarchingSquaresLineCases::GetCases()
{
  return VTK_MARCHING_SQUARES_LINECASES;
}
