/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXMLHierarchicalBoxDataWriter
 * @brief   writer for vtkHierarchicalBoxDataSet
 * for backwards compatibility.
 *
 * vtkXMLHierarchicalBoxDataWriter is an empty subclass of
 * vtkXMLUniformGridAMRWriter for writing vtkUniformGridAMR datasets in
 * VTK-XML format.
*/

#ifndef vtkXMLHierarchicalBoxDataWriter_h
#define vtkXMLHierarchicalBoxDataWriter_h

#include "vtkXMLUniformGridAMRWriter.h"

class VTKIOXML_EXPORT vtkXMLHierarchicalBoxDataWriter : public vtkXMLUniformGridAMRWriter
{
public:
  static vtkXMLHierarchicalBoxDataWriter* New();
  vtkTypeMacro(vtkXMLHierarchicalBoxDataWriter, vtkXMLUniformGridAMRWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Get the default file extension for files written by this writer.
   */
  const char* GetDefaultFileExtension() override
    { return "vth"; }

protected:
  vtkXMLHierarchicalBoxDataWriter();
  ~vtkXMLHierarchicalBoxDataWriter() override;

private:
  vtkXMLHierarchicalBoxDataWriter(const vtkXMLHierarchicalBoxDataWriter&) = delete;
  void operator=(const vtkXMLHierarchicalBoxDataWriter&) = delete;

};

#endif
