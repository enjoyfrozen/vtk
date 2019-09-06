/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkExtractPointCloudPiece
 * @brief   Return a piece of a point cloud
 *
 * This filter takes the output of a vtkHierarchicalBinningFilter and allows
 * the pipeline to stream it. Pieces are determined from an offset integral
 * array associated with the field data of the input.
*/

#ifndef vtkExtractPointCloudPiece_h
#define vtkExtractPointCloudPiece_h

#include "vtkFiltersPointsModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkIdList;
class vtkIntArray;

class VTKFILTERSPOINTS_EXPORT vtkExtractPointCloudPiece : public vtkPolyDataAlgorithm
{
public:
  //@{
  /**
   * Standard methods for instantiation, printing, and type information.
   */
  static vtkExtractPointCloudPiece *New();
  vtkTypeMacro(vtkExtractPointCloudPiece, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  //@{
  /**
   * Turn on or off modulo sampling of the points. By default this is on and the
   * points in a given piece will be reordered in an attempt to reduce spatial
   * coherency.
   */
  vtkSetMacro(ModuloOrdering,bool);
  vtkGetMacro(ModuloOrdering,bool);
  vtkBooleanMacro(ModuloOrdering,bool);
  //@}

protected:
  vtkExtractPointCloudPiece();
  ~vtkExtractPointCloudPiece() override {}

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  int RequestUpdateExtent(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
  bool ModuloOrdering;

private:
  vtkExtractPointCloudPiece(const vtkExtractPointCloudPiece&) = delete;
  void operator=(const vtkExtractPointCloudPiece&) = delete;
};

#endif
