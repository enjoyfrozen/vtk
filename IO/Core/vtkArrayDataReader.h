/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkArrayDataReader
 * @brief    Reads vtkArrayData written by vtkArrayDataWriter.
 *
 *
 * Reads vtkArrayData data written with vtkArrayDataWriter.
 *
 * Outputs:
 *   Output port 0: vtkArrayData containing a collection of vtkArrays.
 *
 * @sa
 * vtkArrayDataWriter
*/

#ifndef vtkArrayDataReader_h
#define vtkArrayDataReader_h

#include "vtkIOCoreModule.h" // For export macro
#include "vtkArrayDataAlgorithm.h"

class VTKIOCORE_EXPORT vtkArrayDataReader :
  public vtkArrayDataAlgorithm
{
public:
  static vtkArrayDataReader* New();
  vtkTypeMacro(vtkArrayDataReader, vtkArrayDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the filesystem location from which data will be read.
   */
  vtkGetStringMacro(FileName);
  vtkSetStringMacro(FileName);
  //@}

  //@{
  /**
   * The input string to parse. If you set the input string, you must also set
   * the ReadFromInputString flag to parse the string instead of a file.
   */
  virtual void SetInputString(const vtkStdString& string);
  virtual vtkStdString GetInputString();
  //@}

  //@{
  /**
   * Whether to read from an input string as opposed to a file, which is the default.
   */
  vtkSetMacro(ReadFromInputString, bool);
  vtkGetMacro(ReadFromInputString, bool);
  vtkBooleanMacro(ReadFromInputString, bool);
  //@}

  /**
   * Read an arbitrary array from a stream.  Note: you MUST always
   * open streams in binary mode to prevent problems reading files
   * on Windows.
   */
  static vtkArrayData* Read(istream& stream);

  /**
   * Read an arbitrary array from a string.
   */
  static vtkArrayData* Read(const vtkStdString& str);

protected:
  vtkArrayDataReader();
  ~vtkArrayDataReader() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  char* FileName;
  vtkStdString InputString;
  bool ReadFromInputString;

private:
  vtkArrayDataReader(const vtkArrayDataReader&) = delete;
  void operator=(const vtkArrayDataReader&) = delete;
};

#endif
