/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXMLPartitionedDataSetWriter
 * @brief   writer for vtkPartitionedDataSet.
 *
 * vtkXMLPartitionedDataSetWriter is a vtkXMLCompositeDataWriter subclass to handle
 * vtkPartitionedDataSet.
*/

#ifndef vtkXMLPartitionedDataSetWriter_h
#define vtkXMLPartitionedDataSetWriter_h

#include "vtkIOXMLModule.h" // For export macro
#include "vtkXMLCompositeDataWriter.h"

class VTKIOXML_EXPORT vtkXMLPartitionedDataSetWriter : public vtkXMLCompositeDataWriter
{
public:
  static vtkXMLPartitionedDataSetWriter* New();
  vtkTypeMacro(vtkXMLPartitionedDataSetWriter, vtkXMLCompositeDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Get the default file extension for files written by this writer.
   */
  const char* GetDefaultFileExtension() override
    { return "vtpd"; }

protected:
  vtkXMLPartitionedDataSetWriter();
  ~vtkXMLPartitionedDataSetWriter() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  // Internal method called recursively to create the xml tree for the children
  // of compositeData.
  int WriteComposite(vtkCompositeDataSet* compositeData,
    vtkXMLDataElement* parent, int &writerIdx) override;

private:
  vtkXMLPartitionedDataSetWriter(const vtkXMLPartitionedDataSetWriter&) = delete;
  void operator=(const vtkXMLPartitionedDataSetWriter&) = delete;

};

#endif
