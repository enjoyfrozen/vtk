/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkEdgeLayout
 * @brief   layout graph edges
 *
 *
 * This class is a shell for many edge layout strategies which may be set
 * using the SetLayoutStrategy() function.  The layout strategies do the
 * actual work.
*/

#ifndef vtkEdgeLayout_h
#define vtkEdgeLayout_h

#include "vtkInfovisLayoutModule.h" // For export macro
#include "vtkGraphAlgorithm.h"

class vtkEdgeLayoutStrategy;
class vtkEventForwarderCommand;

class VTKINFOVISLAYOUT_EXPORT vtkEdgeLayout : public vtkGraphAlgorithm
{
public:
  static vtkEdgeLayout *New();
  vtkTypeMacro(vtkEdgeLayout, vtkGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * The layout strategy to use during graph layout.
   */
  void SetLayoutStrategy(vtkEdgeLayoutStrategy *strategy);
  vtkGetObjectMacro(LayoutStrategy, vtkEdgeLayoutStrategy);
  //@}

  /**
   * Get the modification time of the layout algorithm.
   */
  vtkMTimeType GetMTime() override;

protected:
  vtkEdgeLayout();
  ~vtkEdgeLayout() override;

  vtkEdgeLayoutStrategy* LayoutStrategy;

  //@{
  /**
   * This intercepts events from the strategy object and re-emits them
   * as if they came from the layout engine itself.
   */
  vtkEventForwarderCommand *EventForwarder;
  unsigned long ObserverTag;
  //@}

  int RequestData(
    vtkInformation *,
    vtkInformationVector **,
    vtkInformationVector *) override;

private:

  vtkGraph *InternalGraph;

  vtkEdgeLayout(const vtkEdgeLayout&) = delete;
  void operator=(const vtkEdgeLayout&) = delete;
};

#endif
