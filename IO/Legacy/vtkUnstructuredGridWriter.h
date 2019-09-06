/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkUnstructuredGridWriter
 * @brief   write vtk unstructured grid data file
 *
 * vtkUnstructuredGridWriter is a source object that writes ASCII or binary
 * unstructured grid data files in vtk format. See text for format details.
 * @warning
 * Binary files written on one system may not be readable on other systems.
*/

#ifndef vtkUnstructuredGridWriter_h
#define vtkUnstructuredGridWriter_h

#include "vtkIOLegacyModule.h" // For export macro
#include "vtkDataWriter.h"
class vtkUnstructuredGrid;

class VTKIOLEGACY_EXPORT vtkUnstructuredGridWriter : public vtkDataWriter
{
public:
  static vtkUnstructuredGridWriter *New();
  vtkTypeMacro(vtkUnstructuredGridWriter,vtkDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get the input to this writer.
   */
  vtkUnstructuredGrid* GetInput();
  vtkUnstructuredGrid* GetInput(int port);
  //@}

protected:
  vtkUnstructuredGridWriter() {}
  ~vtkUnstructuredGridWriter() override {}

  void WriteData() override;

  int WriteCellsAndFaces(ostream *fp, vtkUnstructuredGrid *grid,
                         const char *label);

  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkUnstructuredGridWriter(const vtkUnstructuredGridWriter&) = delete;
  void operator=(const vtkUnstructuredGridWriter&) = delete;
};

#endif
