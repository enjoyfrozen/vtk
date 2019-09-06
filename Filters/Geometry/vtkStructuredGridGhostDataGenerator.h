/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkStructuredGridGhostDataGenerator
 *
 *
 *  A concrete implementation of vtkDataSetGhostGenerator for generating ghost
 *  data on partitioned structured grids on a singled process. For a distributed
 *  data-set see vtkPStructuredGridGhostDataGenerator.
 *
 * @warning
 * <ol>
 *   <li>
 *    The input multi-block dataset must:
 *    <ul>
 *      <li> Have the whole-extent set </li>
 *      <li> Each block must be an instance of vtkStructuredGrid </li>
 *      <li> Each block must have its corresponding global extent set in the
 *           meta-data using the PIECE_EXTENT() key </li>
 *      <li> All blocks must have the same fields loaded </li>
 *    </ul>
 *   </li>
 *   <li>
 *    The code currently does not handle the following cases:
 *    <ul>
 *      <li>Ghost cells along Periodic boundaries</li>
 *      <li>Growing ghost layers beyond the extents of the neighboring grid</li>
 *    </ul>
 *   </li>
 * </ol>
 *
 * @sa
 * vtkDataSetGhostGenerator, vtkPStructuredGridGhostDataGenerator
*/

#ifndef vtkStructuredGridGhostDataGenerator_h
#define vtkStructuredGridGhostDataGenerator_h

#include "vtkFiltersGeometryModule.h" // For export macro
#include "vtkDataSetGhostGenerator.h"

// Forward declarations
class vtkMultiBlockDataSet;
class vtkIndent;
class vtkStructuredGridConnectivity;

class VTKFILTERSGEOMETRY_EXPORT vtkStructuredGridGhostDataGenerator :
  public vtkDataSetGhostGenerator
{
public:
  static vtkStructuredGridGhostDataGenerator* New();
  vtkTypeMacro(vtkStructuredGridGhostDataGenerator,vtkDataSetGhostGenerator);
  void PrintSelf(ostream &os, vtkIndent indent) override;

protected:
  vtkStructuredGridGhostDataGenerator();
  ~vtkStructuredGridGhostDataGenerator() override;

  /**
   * Registers the grid associated with this instance of multi-block.
   */
  void RegisterGrids(vtkMultiBlockDataSet *in);

  /**
   * Creates the output.
   */
  void CreateGhostedDataSet(
      vtkMultiBlockDataSet *in,
      vtkMultiBlockDataSet *out );

  /**
   * Generates ghost layers.
   */
  void GenerateGhostLayers(
      vtkMultiBlockDataSet *in, vtkMultiBlockDataSet *out) override;

  vtkStructuredGridConnectivity *GridConnectivity;
private:
  vtkStructuredGridGhostDataGenerator(const vtkStructuredGridGhostDataGenerator&) = delete;
  void operator=(const vtkStructuredGridGhostDataGenerator&) = delete;
};

#endif /* vtkStructuredGridGhostDataGenerator_h */
