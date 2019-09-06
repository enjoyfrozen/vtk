/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkImageToPoints
 * @brief   Extract all image voxels as points.
 *
 * This filter takes an input image and an optional stencil, and creates
 * a vtkPolyData that contains the points and the point attributes but no
 * cells.  If a stencil is provided, only the points inside the stencil
 * are included.
 * @par Thanks:
 * Thanks to David Gobbi, Calgary Image Processing and Analysis Centre,
 * University of Calgary, for providing this class.
*/

#ifndef vtkImageToPoints_h
#define vtkImageToPoints_h

#include "vtkImagingHybridModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkImageStencilData;

class VTKIMAGINGHYBRID_EXPORT vtkImageToPoints :
  public vtkPolyDataAlgorithm
{
public:
  static vtkImageToPoints *New();
  vtkTypeMacro(vtkImageToPoints,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Only extract the points that lie within the stencil.
   */
  void SetStencilConnection(vtkAlgorithmOutput *port);
  vtkAlgorithmOutput *GetStencilConnection();
  void SetStencilData(vtkImageStencilData *stencil);
  //@}

  //@{
  /**
   * Set the desired precision for the output points.
   * See vtkAlgorithm::DesiredOutputPrecision for the available choices.
   * The default is double precision.
   */
  vtkSetMacro(OutputPointsPrecision, int);
  vtkGetMacro(OutputPointsPrecision, int);
  //@}

protected:
  vtkImageToPoints();
  ~vtkImageToPoints() override;

  int RequestInformation(vtkInformation *request,
                                 vtkInformationVector **inInfo,
                                 vtkInformationVector *outInfo) override;

  int RequestUpdateExtent(vtkInformation *request,
                                 vtkInformationVector **inInfo,
                                 vtkInformationVector *outInfo) override;

  int RequestData(vtkInformation *request,
                          vtkInformationVector **inInfo,
                          vtkInformationVector *outInfo) override;

  int FillInputPortInformation(int port, vtkInformation *info) override;
  int FillOutputPortInformation(int port, vtkInformation *info) override;

  int OutputPointsPrecision;

private:
  vtkImageToPoints(const vtkImageToPoints&) = delete;
  void operator=(const vtkImageToPoints&) = delete;
};

#endif
