/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkExtractPiece
 *
 * vtkExtractPiece returns the appropriate piece of each
 * sub-dataset in the vtkCompositeDataSet.
 * This filter can handle sub-datasets of type vtkImageData, vtkPolyData,
 * vtkRectilinearGrid, vtkStructuredGrid, and vtkUnstructuredGrid; it does
 * not handle sub-grids of type vtkCompositeDataSet.
*/

#ifndef vtkExtractPiece_h
#define vtkExtractPiece_h

#include "vtkFiltersParallelImagingModule.h" // For export macro
#include "vtkCompositeDataSetAlgorithm.h"

class vtkImageData;
class vtkPolyData;
class vtkRectilinearGrid;
class vtkStructuredGrid;
class vtkUnstructuredGrid;
class vtkCompositeDataIterator;

class VTKFILTERSPARALLELIMAGING_EXPORT vtkExtractPiece : public vtkCompositeDataSetAlgorithm
{
public:
  static vtkExtractPiece* New();
  vtkTypeMacro(vtkExtractPiece, vtkCompositeDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkExtractPiece() {}
  ~vtkExtractPiece() override {}

  int RequestDataObject(vtkInformation* request,
                                vtkInformationVector** inputVector,
                                vtkInformationVector* outputVector) override;

  int RequestUpdateExtent(vtkInformation*,
                                  vtkInformationVector**,
                                  vtkInformationVector*) override;
  int RequestData(vtkInformation*,
                          vtkInformationVector**,
                          vtkInformationVector*) override;

  void ExtractImageData(vtkImageData *imageData,
                        vtkCompositeDataSet *output,
                        int piece, int numberOfPieces, int ghostLevel,
                        vtkCompositeDataIterator* iter);
  void ExtractPolyData(vtkPolyData *polyData,
                       vtkCompositeDataSet *output,
                       int piece, int numberOfPieces, int ghostLevel,
                       vtkCompositeDataIterator* iter);
  void ExtractRectilinearGrid(vtkRectilinearGrid *rGrid,
                              vtkCompositeDataSet *output,
                              int piece, int numberOfPieces, int ghostLevel,
                              vtkCompositeDataIterator* iter);
  void ExtractStructuredGrid(vtkStructuredGrid *sGrid,
                             vtkCompositeDataSet *output,
                             int piece, int numberOfPieces, int ghostLevel,
                             vtkCompositeDataIterator* iter);
  void ExtractUnstructuredGrid(vtkUnstructuredGrid *uGrid,
                               vtkCompositeDataSet *output,
                               int piece, int numberOfPieces, int ghostLevel,
                               vtkCompositeDataIterator* iter);
private:
  vtkExtractPiece(const vtkExtractPiece&) = delete;
  void operator=(const vtkExtractPiece&) = delete;
};

#endif
