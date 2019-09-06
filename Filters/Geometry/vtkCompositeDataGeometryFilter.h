/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCompositeDataGeometryFilter
 * @brief   extract geometry from multi-group data
 *
 * vtkCompositeDataGeometryFilter applies vtkGeometryFilter to all
 * leaves in vtkCompositeDataSet. Place this filter at the end of a
 * pipeline before a polydata consumer such as a polydata mapper to extract
 * geometry from all blocks and append them to one polydata object.
*/

#ifndef vtkCompositeDataGeometryFilter_h
#define vtkCompositeDataGeometryFilter_h

#include "vtkFiltersGeometryModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkPolyData;

class VTKFILTERSGEOMETRY_EXPORT vtkCompositeDataGeometryFilter : public vtkPolyDataAlgorithm
{
public:
  static vtkCompositeDataGeometryFilter *New();
  vtkTypeMacro(vtkCompositeDataGeometryFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * see vtkAlgorithm for details
   */
  int ProcessRequest(vtkInformation* request,
                     vtkInformationVector** inputVector,
                     vtkInformationVector* outputVector) override;

protected:
  vtkCompositeDataGeometryFilter();
  ~vtkCompositeDataGeometryFilter() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  // Create a default executive.
  vtkExecutive* CreateDefaultExecutive() override;

  virtual int RequestCompositeData(vtkInformation*,
                                   vtkInformationVector**,
                                   vtkInformationVector*);

private:
  vtkCompositeDataGeometryFilter(const vtkCompositeDataGeometryFilter&) = delete;
  void operator=(const vtkCompositeDataGeometryFilter&) = delete;
};

#endif


