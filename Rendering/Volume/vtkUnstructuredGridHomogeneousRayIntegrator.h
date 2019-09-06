/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/


/**
 * @class   vtkUnstructuredGridHomogeneousRayIntegrator
 * @brief   performs piecewise constant ray integration.
 *
 *
 *
 * vtkUnstructuredGridHomogeneousRayIntegrator performs homogeneous ray
 * integration.  This is a good method to use when volume rendering scalars
 * that are defined on cells.
 *
*/

#ifndef vtkUnstructuredGridHomogeneousRayIntegrator_h
#define vtkUnstructuredGridHomogeneousRayIntegrator_h

#include "vtkRenderingVolumeModule.h" // For export macro
#include "vtkUnstructuredGridVolumeRayIntegrator.h"

class vtkVolumeProperty;

class VTKRENDERINGVOLUME_EXPORT vtkUnstructuredGridHomogeneousRayIntegrator : public vtkUnstructuredGridVolumeRayIntegrator
{
public:
  vtkTypeMacro(vtkUnstructuredGridHomogeneousRayIntegrator,
                       vtkUnstructuredGridVolumeRayIntegrator);
  static vtkUnstructuredGridHomogeneousRayIntegrator *New();
  void PrintSelf(ostream &os, vtkIndent indent) override;

  void Initialize(vtkVolume *volume, vtkDataArray *scalars) override;

  void Integrate(vtkDoubleArray *intersectionLengths,
                         vtkDataArray *nearIntersections,
                         vtkDataArray *farIntersections,
                         float color[4]) override;

  //@{
  /**
   * For quick lookup, the transfer function is sampled into a table.
   * This parameter sets how big of a table to use.  By default, 1024
   * entries are used.
   */
  vtkSetMacro(TransferFunctionTableSize, int);
  vtkGetMacro(TransferFunctionTableSize, int);
  //@}

protected:
  vtkUnstructuredGridHomogeneousRayIntegrator();
  ~vtkUnstructuredGridHomogeneousRayIntegrator() override;

  vtkVolume *Volume;
  vtkVolumeProperty *Property;

  int      NumComponents;
  float  **ColorTable;
  float  **AttenuationTable;
  double  *TableShift;
  double  *TableScale;
  vtkTimeStamp TablesBuilt;

  int UseAverageColor;
  int TransferFunctionTableSize;

  virtual void GetTransferFunctionTables(vtkDataArray *scalars);

private:
  vtkUnstructuredGridHomogeneousRayIntegrator(const vtkUnstructuredGridHomogeneousRayIntegrator&) = delete;
  void operator=(const vtkUnstructuredGridHomogeneousRayIntegrator&) = delete;
};

#endif //vtkUnstructuredGridHomogeneousRayIntegrator_h
