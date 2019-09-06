/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkExtractEdges
 * @brief   extract cell edges from any type of data
 *
 * vtkExtractEdges is a filter to extract edges from a dataset. Edges
 * are extracted as lines or polylines.
 *
 * @sa
 * vtkFeatureEdges
*/

#ifndef vtkExtractEdges_h
#define vtkExtractEdges_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkIncrementalPointLocator;

class VTKFILTERSEXTRACTION_EXPORT vtkExtractEdges : public vtkPolyDataAlgorithm
{
public:
  static vtkExtractEdges *New();
  vtkTypeMacro(vtkExtractEdges,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set / get a spatial locator for merging points. By
   * default an instance of vtkMergePoints is used.
   */
  void SetLocator(vtkIncrementalPointLocator *locator);
  vtkGetObjectMacro(Locator,vtkIncrementalPointLocator);
  //@}

  /**
   * Create default locator. Used to create one when none is specified.
   */
  void CreateDefaultLocator();

  /**
   * Return MTime also considering the locator.
   */
  vtkMTimeType GetMTime() override;

protected:
  vtkExtractEdges();
  ~vtkExtractEdges() override;

  // Usual data generation method
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  int FillInputPortInformation(int port, vtkInformation *info) override;

  vtkIncrementalPointLocator *Locator;
private:
  vtkExtractEdges(const vtkExtractEdges&) = delete;
  void operator=(const vtkExtractEdges&) = delete;
};

#endif


