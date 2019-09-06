/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTableReader
 * @brief   read vtkTable data file
 *
 * vtkTableReader is a source object that reads ASCII or binary
 * vtkTable data files in vtk format. (see text for format details).
 * The output of this reader is a single vtkTable data object.
 * The superclass of this class, vtkDataReader, provides many methods for
 * controlling the reading of the data file, see vtkDataReader for more
 * information.
 * @warning
 * Binary files written on one system may not be readable on other systems.
 * @sa
 * vtkTable vtkDataReader vtkTableWriter
*/

#ifndef vtkTableReader_h
#define vtkTableReader_h

#include "vtkIOLegacyModule.h" // For export macro
#include "vtkDataReader.h"

class vtkTable;

class VTKIOLEGACY_EXPORT vtkTableReader : public vtkDataReader
{
public:
  static vtkTableReader *New();
  vtkTypeMacro(vtkTableReader,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get the output of this reader.
   */
  vtkTable *GetOutput();
  vtkTable *GetOutput(int idx);
  void SetOutput(vtkTable *output);
  //@}

  /**
   * Actual reading happens here
   */
  int ReadMeshSimple(const std::string& fname,
                     vtkDataObject* output) override;

protected:
  vtkTableReader();
  ~vtkTableReader() override;

  int FillOutputPortInformation(int, vtkInformation*) override;
private:
  vtkTableReader(const vtkTableReader&) = delete;
  void operator=(const vtkTableReader&) = delete;
};

#endif
