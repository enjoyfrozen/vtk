/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHyperTreeGridToDualGrid
 * @brief   Convert hyper tree grid to the dual unstructured grid.
 *
 * This filter is the new home for what was the dataset API within the
 * vtkHyperTreeGrid class.
*/

#ifndef vtkHyperTreeGridToDualGrid_h
#define vtkHyperTreeGridToDualGrid_h

#include "vtkFiltersHyperTreeModule.h" // For export macro
#include "vtkHyperTreeGridAlgorithm.h"

#include <map> // std::map

class vtkHyperTreeGrid;
class vtkHyperTreeGridNonOrientedMooreSuperCursor;
class vtkIdTypeArray;
class vtkUnstructuredGrid;
class vtkPoints;

class VTKFILTERSHYPERTREE_EXPORT vtkHyperTreeGridToDualGrid : public vtkHyperTreeGridAlgorithm
{
public:
  static vtkHyperTreeGridToDualGrid* New();
  vtkTypeMacro( vtkHyperTreeGridToDualGrid, vtkHyperTreeGridAlgorithm );
  void PrintSelf( ostream&, vtkIndent ) override;

protected:
  vtkHyperTreeGridToDualGrid();
  ~vtkHyperTreeGridToDualGrid() override;

  /**
   * For this algorithm the output is a vtkUnstructuredGrid instance
   */
  int FillOutputPortInformation( int, vtkInformation* ) override;

  /**
   * Main routine to convert the grid of tree into an unstructured grid
   */
  int ProcessTrees( vtkHyperTreeGrid*, vtkDataObject* ) override;

private:
  vtkHyperTreeGridToDualGrid(const vtkHyperTreeGridToDualGrid&) = delete;
  void operator=(const vtkHyperTreeGridToDualGrid&) = delete;

  void TraverseDualRecursively( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor, vtkHyperTreeGrid *in );
  void GenerateDualCornerFromLeaf1D( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor, vtkHyperTreeGrid *in);
  void GenerateDualCornerFromLeaf2D( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor, vtkHyperTreeGrid *in );
  void GenerateDualCornerFromLeaf3D( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor, vtkHyperTreeGrid *in );

  void TraverseDualRecursively( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor,
                                vtkBitArray* mask,
                                vtkHyperTreeGrid *in);
  void ShiftDualCornerFromMaskedLeaf2D( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor,
                                        vtkBitArray* mask,
                                        vtkHyperTreeGrid *in);
  void ShiftDualCornerFromMaskedLeaf3D( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor,
                                        vtkBitArray* mask,
                                        vtkHyperTreeGrid *in);
  void GenerateDualCornerFromLeaf2D( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor,
                                     vtkBitArray* mask,
                                     vtkHyperTreeGrid *in);
  void GenerateDualCornerFromLeaf3D( vtkHyperTreeGridNonOrientedMooreSuperCursor* cursor,
                                     vtkBitArray* mask,
                                     vtkHyperTreeGrid *in);

  vtkPoints *Points;
  vtkIdTypeArray *Connectivity;
  std::map<vtkIdType, bool> PointShifted;
  std::map<vtkIdType, double> PointShifts[3];
  std::map<vtkIdType, double> ReductionFactors;
};

#endif /* vtkHyperTreeGridToDualGrid_h */
