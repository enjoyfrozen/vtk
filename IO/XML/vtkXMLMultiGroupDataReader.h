/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXMLMultiGroupDataReader
 * @brief   Reader for multi-block datasets
 *
 * vtkXMLMultiGroupDataReader is a legacy reader that reads multi group files
 * into multiblock datasets.
*/

#ifndef vtkXMLMultiGroupDataReader_h
#define vtkXMLMultiGroupDataReader_h

#include "vtkIOXMLModule.h" // For export macro
#include "vtkXMLMultiBlockDataReader.h"

class VTKIOXML_EXPORT vtkXMLMultiGroupDataReader : public vtkXMLMultiBlockDataReader
{
public:
  static vtkXMLMultiGroupDataReader* New();
  vtkTypeMacro(vtkXMLMultiGroupDataReader,vtkXMLMultiBlockDataReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkXMLMultiGroupDataReader();
  ~vtkXMLMultiGroupDataReader() override;

  // Get the name of the data set being read.
  const char* GetDataSetName() override
  {
    return "vtkMultiGroupDataSet";
  }

private:
  vtkXMLMultiGroupDataReader(const vtkXMLMultiGroupDataReader&) = delete;
  void operator=(const vtkXMLMultiGroupDataReader&) = delete;
};

#endif
