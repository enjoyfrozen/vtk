/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCellDataToPointData.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkCellDataToPointData
 * @brief   map cell data to point data
 *
 * vtkCellDataToPointData is a filter that transforms cell data (i.e., data
 * specified per cell) into point data (i.e., data specified at cell
 * points). The method of transformation is based on averaging the data
 * values of all cells using each point. For large datasets with
 * several cell data arrays, the filter optionally supports selective
 * processing to speed up processing. Optionally, the input cell data can
 * be passed through to the output as well.
 *
 * Options exist to control which cells are used to perform the averaging
 * operation. Since unstructured grids and polydata can contain cells of
 * different dimensions, in some cases it is desirable to perform cell
 * averaging using cells of a specified dimension. The available options to
 * control this functionality are All (default), Patch and DataSetMax. Patch
 * uses only the highest dimension cells attached to a point. DataSetMax uses
 * the highest cell dimension in the entire data set.
 *
 * @warning
 * This filter is an abstract filter, that is, the output is an abstract type
 * (i.e., vtkDataSet). Use the convenience methods (e.g.,
 * GetPolyDataOutput(), GetStructuredPointsOutput(), etc.) to get the type
 * of output you want.
 *
 * @warning
 * For maximum performance, use the ContributingCellOption=All. Other options
 * significantly, negatively impact performance (on the order of >10x).
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential execution type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.
 *
 * @sa
 * vtkPointData vtkCellData vtkPointDataToCellData
 */

#ifndef vtkCellDataToPointData_h
#define vtkCellDataToPointData_h

#include "vtkDataSetAlgorithm.h"
#include "vtkFiltersCoreModule.h" // For export macro

VTK_ABI_NAMESPACE_BEGIN
class vtkDataSet;

class VTKFILTERSCORE_EXPORT vtkCellDataToPointData : public vtkDataSetAlgorithm
{
public:
  ///@{
  /**
   * Standard methods for instantiation, type information, and printing.
   */
  static vtkCellDataToPointData* New();
  vtkTypeMacro(vtkCellDataToPointData, vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /// Options to specify what cells contribute to the cell-averaging calculation
  enum ContributingCellEnum
  {
    All = 0,   //!< All cells
    Patch = 1, //!< Highest dimension cells in the patch of cells contributing to the calculation
    DataSetMax = 2 //!< Highest dimension cells in the data set
  };

  /// Options to specify what weight cells to contribute to the cell-averaging calculation
  enum WeightCellEnum
  {
    Standard = 0,  //!< Weight of each cell is specific to each point (1/numberCellsByThisPoint)
    Quad = 4,      //!< Weight of each cell is 1/4 to each point (2D Eulerian origin grid)
    Hexahedron = 8 //!< Weight of each cell is 1/8 to each point (3D Eulerian origin grid)
  };

  /// Options to specify what boundary condition that disables on specific points WeightCellOption
  enum BoundaryConditionPointEnum
  {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 2,
    AXIS_NONE = 3
  };

  ///@{
  /**
   * Control whether the input cell data is to be passed to the output. If
   * on, then the input cell data is passed through to the output; otherwise,
   * only generated point data is placed into the output. The default is false.
   */
  vtkSetMacro(PassCellData, bool);
  vtkGetMacro(PassCellData, bool);
  vtkBooleanMacro(PassCellData, bool);
  ///@}

  ///@{
  /**
   * Option to specify what cells to include in the cell-averaging computation.
   * Options are all cells (All, Patch and DataSetMax). The default is All.
   * This option is only applicable for an input representing a
   * vtkUnstructuredGrid and vtkPolyData type mesh
   */
  vtkSetClampMacro(ContributingCellOption, int, 0, 2);
  vtkGetMacro(ContributingCellOption, int);
  ///@}

  ///@{
  /**
   * Option to specify what weights cells to contribute in the cell-averaging computation.
   * Options are Standard (specific to each cell), Quad (unstructured grid based on
   * 2D Eulerian submesh), Hexahedron (unstrucuterd grid based on 3D Eulerian submesh).
   * The default is Standard.
   * Using this option disables ContributingCellOption option.
   */
  vtkSetClampMacro(WeightCellOption, int, 0, 8);
  vtkGetMacro(WeightCellOption, int);
  ///@}

  ///@{
  /**
   * Option to specify what boundary condition that disables on specific points
   * WeightCellOption (None, AXIS_X, AXIS_Y, AXIS_Z).
   * The default is None.
   */
  vtkSetClampMacro(BoundaryConditionPoint, int, 0, 3);
  vtkGetMacro(BoundaryConditionPoint, int);
  ///@}

  ///@{
  /**
   * Option to set axis alignment value to be used as boundary condition point
   * by BoundaryConditionPointOption. Default is 0.
   */
  vtkSetMacro(AxisAlignment, double);
  vtkGetMacro(AxisAlignment, double);
  ///@}

  ///@{
  /**
   * Option to set absolute error epsilon on axis alignment value. Default is 1e-7.
   */
  vtkSetMacro(AbsoluteErrorEpsilonOnAxisAlignment, double);
  vtkGetMacro(AbsoluteErrorEpsilonOnAxisAlignment, double);
  ///@}

  ///@{
  /**
   * Activate selective processing of arrays. If false, only arrays selected
   * by the user will be considered by this filter. The default is true.
   */
  vtkSetMacro(ProcessAllArrays, bool);
  vtkGetMacro(ProcessAllArrays, bool);
  vtkBooleanMacro(ProcessAllArrays, bool);
  ///@}

  ///@{
  /**
   * To get piece invariance, this filter has to request an
   * extra ghost level.  By default piece invariance is on.
   */
  vtkSetMacro(PieceInvariant, bool);
  vtkGetMacro(PieceInvariant, bool);
  vtkBooleanMacro(PieceInvariant, bool);
  ///@}

  /**
   * Adds an array to be processed. This only has an effect if the
   * ProcessAllArrays option is turned off. If a name is already present,
   * nothing happens.
   */
  virtual void AddCellDataArray(const char* name);

  /**
   * Removes an array to be processed. This only has an effect if the
   * ProcessAllArrays option is turned off. If the specified name is not
   * present, nothing happens.
   */
  virtual void RemoveCellDataArray(const char* name);

  /**
   * Removes all arrays to be processed from the list. This only has an effect
   * if the ProcessAllArrays option is turned off.
   */
  virtual void ClearCellDataArrays();

protected:
  vtkCellDataToPointData();
  ~vtkCellDataToPointData() override;

  virtual vtkIdType GetNumberOfCellArraysToProcess();
  virtual void GetCellArraysToProcess(const char* names[]);

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  ///@{
  /**
   * Special algorithm for unstructured grids and polydata to make sure
   * that we properly take into account ContributingCellOption.
   */
  int RequestDataForUnstructuredData(
    vtkInformation*, vtkInformationVector**, vtkInformationVector*);
  ///@}

  int InterpolatePointData(vtkDataSet* input, vtkDataSet* output);

  ///@{
  /**
   * Option to pass cell data arrays through to the output. Default is false/off.
   */
  bool PassCellData;
  ///@}

  ///@{
  /**
   * Option to specify what cells to include in the computation.
   * Options are all cells (All, Patch and DataSet). The default is All.
   * This option is only applicable for an input representing a
   * vtkUnstructuredGrid and vtkPolyData type mesh
   */
  int ContributingCellOption;
  ///@}

  ///@{
  /**
   * Option to specify what weights cells to contribute in the cell-averaging computation.
   * Options are Standard (specific to each cell), Quad (unstructured grid based on
   * 2D Eulerian submesh), Hexahedron (unstrucuterd grid based on 3D Eulerian submesh).
   * The default is Standard.
   * Using this option disables ContributingCellOption option.
   */
  int WeightCellOption;
  ///@}

  ///@{
  /**
   * Option to specify what boundary condition that disables on specific points
   * WeightCellOption (None, AXIS_X, AXIS_Y, AXIS_Z).
   * The default is None.
   */
  int BoundaryConditionPoint;
  ///@}

  ///@{
  /**
   * Option to set axis alignment value to be used as boundary condition point
   * by BoundaryConditionPointOption. Default is 0.
   */
  double AxisAlignment;
  ///@}

  ///@{
  /**
   * Option to set absolute error epsilon on axis alignment value. Default is 1e-7.
   */
  double AbsoluteErrorEpsilonOnAxisAlignment;
  ///@}

  /**
   * Option to activate selective processing of arrays. The default is true.
   */
  bool ProcessAllArrays;

  bool PieceInvariant;

  class Internals;
  Internals* Implementation;

private:
  vtkCellDataToPointData(const vtkCellDataToPointData&) = delete;
  void operator=(const vtkCellDataToPointData&) = delete;
};

VTK_ABI_NAMESPACE_END
#endif
