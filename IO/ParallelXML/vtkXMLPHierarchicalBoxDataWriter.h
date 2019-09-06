/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXMLPHierarchicalBoxDataWriter
 * @brief   parallel writer for
 * vtkHierarchicalBoxDataSet for backwards compatibility.
 *
 * vtkXMLPHierarchicalBoxDataWriter is an empty subclass of
 * vtkXMLPUniformGridAMRWriter for backwards compatibility.
*/

#ifndef vtkXMLPHierarchicalBoxDataWriter_h
#define vtkXMLPHierarchicalBoxDataWriter_h

#include "vtkXMLPUniformGridAMRWriter.h"
#include "vtkIOParallelXMLModule.h" // For export macro

class VTKIOPARALLELXML_EXPORT vtkXMLPHierarchicalBoxDataWriter :
  public vtkXMLPUniformGridAMRWriter
{
public:
  static vtkXMLPHierarchicalBoxDataWriter* New();
  vtkTypeMacro(vtkXMLPHierarchicalBoxDataWriter, vtkXMLPUniformGridAMRWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkXMLPHierarchicalBoxDataWriter();
  ~vtkXMLPHierarchicalBoxDataWriter() override;

private:
  vtkXMLPHierarchicalBoxDataWriter(const vtkXMLPHierarchicalBoxDataWriter&) = delete;
  void operator=(const vtkXMLPHierarchicalBoxDataWriter&) = delete;
};

#endif
