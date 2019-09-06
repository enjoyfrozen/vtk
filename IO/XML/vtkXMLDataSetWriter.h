/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXMLDataSetWriter
 * @brief   Write any type of VTK XML file.
 *
 * vtkXMLDataSetWriter is a wrapper around the VTK XML file format
 * writers.  Given an input vtkDataSet, the correct writer is
 * automatically selected based on the type of input.
 *
 * @sa
 * vtkXMLImageDataWriter vtkXMLStructuredGridWriter
 * vtkXMLRectilinearGridWriter vtkXMLPolyDataWriter
 * vtkXMLUnstructuredGridWriter
*/

#ifndef vtkXMLDataSetWriter_h
#define vtkXMLDataSetWriter_h

#include "vtkIOXMLModule.h" // For export macro
#include "vtkXMLDataObjectWriter.h"

class vtkCallbackCommand;

class VTKIOXML_EXPORT vtkXMLDataSetWriter : public vtkXMLDataObjectWriter
{
public:
  vtkTypeMacro(vtkXMLDataSetWriter,vtkXMLDataObjectWriter);
  static vtkXMLDataSetWriter* New();

protected:
  vtkXMLDataSetWriter();
  ~vtkXMLDataSetWriter() override;

  // see algorithm for more info
  int FillInputPortInformation(int port, vtkInformation* info) override;

private:
  vtkXMLDataSetWriter(const vtkXMLDataSetWriter&) = delete;
  void operator=(const vtkXMLDataSetWriter&) = delete;
};

#endif
