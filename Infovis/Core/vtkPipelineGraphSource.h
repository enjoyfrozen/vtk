/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPipelineGraphSource
 * @brief   a graph constructed from a VTK pipeline
 *
 *
*/

#ifndef vtkPipelineGraphSource_h
#define vtkPipelineGraphSource_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkDirectedGraphAlgorithm.h"
#include "vtkStdString.h"

class vtkCollection;

class VTKINFOVISCORE_EXPORT vtkPipelineGraphSource : public vtkDirectedGraphAlgorithm
{
public:
  static vtkPipelineGraphSource* New();
  vtkTypeMacro(vtkPipelineGraphSource,vtkDirectedGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  void AddSink(vtkObject* object);
  void RemoveSink(vtkObject* object);

  /**
   * Generates a GraphViz DOT file that describes the VTK pipeline
   * terminating at the given sink.
   */
  static void PipelineToDot(vtkAlgorithm* sink, ostream& output, const vtkStdString& graph_name = "");
  /**
   * Generates a GraphViz DOT file that describes the VTK pipeline
   * terminating at the given sinks.
   */
  static void PipelineToDot(vtkCollection* sinks, ostream& output, const vtkStdString& graph_name = "");

protected:
  vtkPipelineGraphSource();
  ~vtkPipelineGraphSource() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  vtkCollection* Sinks;

private:
  vtkPipelineGraphSource(const vtkPipelineGraphSource&) = delete;
  void operator=(const vtkPipelineGraphSource&) = delete;

};

#endif

// VTK-HeaderTest-Exclude: vtkPipelineGraphSource.h
