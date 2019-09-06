/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXMLPUnstructuredDataWriter
 * @brief   Superclass for PVTK XML unstructured data writers.
 *
 * vtkXMLPUnstructuredDataWriter provides PVTK XML writing
 * functionality that is common among all the parallel unstructured
 * data formats.
*/

#ifndef vtkXMLPUnstructuredDataWriter_h
#define vtkXMLPUnstructuredDataWriter_h

#include "vtkIOParallelXMLModule.h" // For export macro
#include "vtkXMLPDataWriter.h"

class vtkPointSet;
class vtkXMLUnstructuredDataWriter;

class VTKIOPARALLELXML_EXPORT vtkXMLPUnstructuredDataWriter : public vtkXMLPDataWriter
{
public:
  vtkTypeMacro(vtkXMLPUnstructuredDataWriter,vtkXMLPDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkXMLPUnstructuredDataWriter();
  ~vtkXMLPUnstructuredDataWriter() override;

  vtkPointSet* GetInputAsPointSet();
  virtual vtkXMLUnstructuredDataWriter* CreateUnstructuredPieceWriter()=0;
  vtkXMLWriter* CreatePieceWriter(int index) override;
  void WritePData(vtkIndent indent) override;
private:
  vtkXMLPUnstructuredDataWriter(const vtkXMLPUnstructuredDataWriter&) = delete;
  void operator=(const vtkXMLPUnstructuredDataWriter&) = delete;
};

#endif
