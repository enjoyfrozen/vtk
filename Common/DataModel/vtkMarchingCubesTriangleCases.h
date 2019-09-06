/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTKm-Copyright.txt                                                      */
/*===========================================================================*/
#ifndef vtkMarchingCubesTriangleCases_h
#define vtkMarchingCubesTriangleCases_h
//
// marching cubes case table for generating isosurfaces
//
#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkSystemIncludes.h"

typedef int EDGE_LIST;
struct VTKCOMMONDATAMODEL_EXPORT vtkMarchingCubesTriangleCases
{
  EDGE_LIST edges[16];
  static vtkMarchingCubesTriangleCases* GetCases();
};

#endif
// VTK-HeaderTest-Exclude: vtkMarchingCubesTriangleCases.h
