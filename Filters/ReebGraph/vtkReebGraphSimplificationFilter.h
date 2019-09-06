/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkReebGraphSimplificationFilter
 * @brief   simplify an input Reeb graph.
 *
 * The filter takes an input vtkReebGraph object and outputs a
 * vtkReebGraph object.
*/

#ifndef vtkReebGraphSimplificationFilter_h
#define vtkReebGraphSimplificationFilter_h

#include "vtkFiltersReebGraphModule.h" // For export macro
#include "vtkDirectedGraphAlgorithm.h"

class vtkReebGraph;
class vtkReebGraphSimplificationMetric;

class VTKFILTERSREEBGRAPH_EXPORT vtkReebGraphSimplificationFilter :
  public vtkDirectedGraphAlgorithm
{
public:
  static vtkReebGraphSimplificationFilter* New();
  vtkTypeMacro(vtkReebGraphSimplificationFilter, vtkDirectedGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the persistence threshold for simplification (from 0 to 1).
   * Default value: 0 (no simplification).
   */
  vtkSetMacro(SimplificationThreshold, double);
  vtkGetMacro(SimplificationThreshold, double);
  //@}

  /**
   * Set the persistence metric evaluation code
   * Default value: nullptr (standard topological persistence).
   */
  void SetSimplificationMetric(vtkReebGraphSimplificationMetric *metric);

  vtkReebGraph* GetOutput();

protected:
  vtkReebGraphSimplificationFilter();
  ~vtkReebGraphSimplificationFilter();

  double SimplificationThreshold;

  vtkReebGraphSimplificationMetric *SimplificationMetric;

  int FillInputPortInformation(int portNumber, vtkInformation *) override;
  int FillOutputPortInformation(int, vtkInformation *) override;

  int RequestData(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector) override;

private:
  vtkReebGraphSimplificationFilter(const vtkReebGraphSimplificationFilter&) = delete;
  void operator=(const vtkReebGraphSimplificationFilter&) = delete;
};

#endif
