/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkUnstructuredGridReader
 * @brief   read vtk unstructured grid data file
 *
 * vtkUnstructuredGridReader is a source object that reads ASCII or binary
 * unstructured grid data files in vtk format. (see text for format details).
 * The output of this reader is a single vtkUnstructuredGrid data object.
 * The superclass of this class, vtkDataReader, provides many methods for
 * controlling the reading of the data file, see vtkDataReader for more
 * information.
 * @warning
 * Binary files written on one system may not be readable on other systems.
 * @sa
 * vtkUnstructuredGrid vtkDataReader
*/

#ifndef vtkUnstructuredGridReader_h
#define vtkUnstructuredGridReader_h

#include "vtkIOLegacyModule.h" // For export macro
#include "vtkDataReader.h"

class vtkUnstructuredGrid;

class VTKIOLEGACY_EXPORT vtkUnstructuredGridReader : public vtkDataReader
{
public:
  static vtkUnstructuredGridReader *New();
  vtkTypeMacro(vtkUnstructuredGridReader,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get the output of this reader.
   */
  vtkUnstructuredGrid *GetOutput();
  vtkUnstructuredGrid *GetOutput(int idx);
  void SetOutput(vtkUnstructuredGrid *output);
  //@}

  /**
   * Actual reading happens here
   */
  int ReadMeshSimple(const std::string& fname,
                     vtkDataObject* output) override;

protected:
  vtkUnstructuredGridReader();
  ~vtkUnstructuredGridReader() override;

  int FillOutputPortInformation(int, vtkInformation*) override;
private:
  vtkUnstructuredGridReader(const vtkUnstructuredGridReader&) = delete;
  void operator=(const vtkUnstructuredGridReader&) = delete;
};

#endif
