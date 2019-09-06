/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#ifndef UNSTRUCTUREDGHOSTZONESCOMMON_H_
#define UNSTRUCTUREDGHOSTZONESCOMMON_H_

// VTK includes
#include "vtkMathUtilities.h"
#include "vtkMPIUtilities.h"

// C/C++ includes
#include <cmath>
#include <iomanip>
#include <sstream>

//------------------------------------------------------------------------------
//    G L O B A L   D A T A
//------------------------------------------------------------------------------
struct global
{
  static double Origin[3];
  static double Spacing[3];
  static int Dims[3];

  static int Rank;
  static int NRanks;

  static vtkUnstructuredGrid* Grid;
};

int CheckGrid(vtkUnstructuredGrid* ghostGrid, const int iteration);

//------------------------------------------------------------------------------
void UpdateGrid(const int iteration);

//------------------------------------------------------------------------------
void SetXYZCellField();

//------------------------------------------------------------------------------
void SetXYZNodeField();

//------------------------------------------------------------------------------
void WriteDataSet(
      vtkUnstructuredGrid* grid, const std::string& file);

//------------------------------------------------------------------------------
void GetPoint(
      const int i, const int j, const int k,double pnt[3]);

void GenerateDataSet();

#endif /* UNSTRUCTUREDGHOSTZONESCOMMON_H_ */
