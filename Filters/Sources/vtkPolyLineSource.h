/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPolyLineSource
 * @brief   create a poly line from a list of input points
 *
 * vtkPolyLineSource is a source object that creates a poly line from
 * user-specified points. The output is a vtkPolyLine.
*/

#ifndef vtkPolyLineSource_h
#define vtkPolyLineSource_h

#include "vtkFiltersSourcesModule.h" // For export macro
#include "vtkPolyPointSource.h"

class vtkPoints;

class VTKFILTERSSOURCES_EXPORT vtkPolyLineSource : public vtkPolyPointSource
{
public:
  static vtkPolyLineSource* New();
  vtkTypeMacro(vtkPolyLineSource, vtkPolyPointSource);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set whether to close the poly line by connecting the last and first points.
   */
  vtkSetMacro(Closed, vtkTypeBool);
  vtkGetMacro(Closed, vtkTypeBool);
  vtkBooleanMacro(Closed, vtkTypeBool);
  //@}

protected:
  vtkPolyLineSource();
  ~vtkPolyLineSource() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector *) override;

  vtkTypeBool Closed;

private:
  vtkPolyLineSource(const vtkPolyLineSource&) = delete;
  void operator=(const vtkPolyLineSource&) = delete;
};

#endif
