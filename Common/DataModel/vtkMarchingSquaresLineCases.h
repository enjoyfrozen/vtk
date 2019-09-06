/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#ifndef vtkMarchingSquaresLineCases_h
#define vtkMarchingSquaresLineCases_h
//
// Marching squares cases for generating isolines.
//
#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkSystemIncludes.h"

typedef int EDGE_LIST;
struct VTKCOMMONDATAMODEL_EXPORT vtkMarchingSquaresLineCases
{
  EDGE_LIST edges[5];
  static vtkMarchingSquaresLineCases* GetCases();
};

#endif
// VTK-HeaderTest-Exclude: vtkMarchingSquaresLineCases.h
