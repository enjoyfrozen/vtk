/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTestHTGGenerator.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyrgight notice for more information.

=========================================================================*/

/**
 * @class vtkTestHTGGenerator
 * @brief Helper class for generating a curated set of HyperTree Grids (HTGs) for testing purposes
 *
 * Provides a set of public methods for generating some commonly used HTG setups.
 */

#include "vtkHyperTreeGridAlgorithm.h"
#include "vtkTestingDataModelModule.h" //for export macro

#ifndef vtkTestHTGGenerator_h
#define vtkTestHTGGenerator_h

class vtkHyperTreeGrid;
class vtkHyperTreeGridNonOrientedCursor;
class vtkDoubleArray;

class VTKTESTINGDATAMODEL_EXPORT vtkTestHTGGenerator : public vtkHyperTreeGridAlgorithm
{
public:
  /**
   * Standard object factory setup
   */
  vtkTypeMacro(vtkTestHTGGenerator, vtkHyperTreeGridAlgorithm);
  static vtkTestHTGGenerator* New();

  /**
   * Helper methods for generating HTGs
   */
  void generateUnbalanced(vtkHyperTreeGrid* HTG, int dim, int factor, int depth,
    const std::vector<double>& extent, const std::vector<int>& subdivisions);

  void generateBalanced(vtkHyperTreeGrid* HTG, int dim, int factor, int depth,
    const std::vector<double>& extent, const std::vector<int>& subdivisions);

  enum HTGType
  {
    UNBALANCED_3DEPTH_2BRANCH_2X3,
    BALANCED_3DEPTH_2BRANCH_2X3,
    UNBALANCED_2DEPTH_3BRANCH_3X3,
    BALANCED_4DEPTH_3BRANCH_2X2,
    UNBALANCED_3DEPTH_2BRANCH_3X2X3,
    BALANCED_2DEPTH_3BRANCH_3X3X2,
    CUSTOM
  };

  /**
   * Get/Set HyperTreeGrid mode
   */
  vtkGetEnumMacro(HTGMode, HTGType);
  vtkSetEnumMacro(HTGMode, HTGType);

  /**
   * Get/Set for custom mode
   */
  vtkGetMacro(CustomBalanced, bool);
  vtkSetMacro(CustomBalanced, bool);

  vtkGetMacro(CustomDim, int);
  vtkSetMacro(CustomDim, int);

  vtkGetMacro(CustomFactor, int);
  vtkSetMacro(CustomFactor, int);

  vtkGetMacro(CustomDepth, int);
  vtkSetMacro(CustomDepth, int);

  void SetCustomExtent(int extentSize, double* extent);
  double* GetCustomExtent() { return CustomExtent.data(); };

  void SetCustomSubdivisions(int subSize, int* subdivisions);
  int* GetCustomSubdivisions() { return CustomSubdivisions.data(); };

  /**
   * Specializations
   */
  void generateUnbalanced3DepthQuadTree2x3(vtkHyperTreeGrid* HTG);

  void generateBalanced3DepthQuadTree2x3(vtkHyperTreeGrid* HTG);

  void generateUnbalanced2Depth3BranchTree3x3(vtkHyperTreeGrid* HTG);

  void generateBalanced4Depth3BranchTree2x2(vtkHyperTreeGrid* HTG);

  void generateUnbalanced3DepthOctTree3x2x3(vtkHyperTreeGrid* HTG);

  void generateBalanced2Depth3BranchTree3x3x2(vtkHyperTreeGrid* HTG);

  int generateCustom(vtkHyperTreeGrid* HTG);

protected:
  /**
   * Contructor setup
   */
  vtkTestHTGGenerator();
  virtual ~vtkTestHTGGenerator() = default;
  vtkTestHTGGenerator(const vtkTestHTGGenerator&) = delete;

  void operator=(const vtkTestHTGGenerator&) = delete;

  int FillOutputPortInformation(int, vtkInformation*) override;

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  int ProcessTrees(vtkHyperTreeGrid*, vtkDataObject*) override;

  void preprocess(vtkHyperTreeGrid* HTG, int dim, int factor, const std::vector<double>& extent,
    const std::vector<int>& subdivisions);

  void recurseBalanced(
    vtkHyperTreeGridNonOrientedCursor* cursor, vtkDoubleArray* levels, const int maxDepth);

  HTGType HTGMode;

  bool CustomBalanced;
  int CustomDim;
  int CustomFactor;
  int CustomDepth;
  std::vector<double> CustomExtent;
  std::vector<int> CustomSubdivisions;

}; // vtkTestHTGGenerator

#endif // vtkTestHTGGenerator_h
