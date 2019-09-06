/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkArrayReader
 * @brief    Reads sparse and dense vtkArray data written by vtkArrayWriter.
 *
 *
 * Reads sparse and dense vtkArray data written with vtkArrayWriter.
 *
 * Outputs:
 *   Output port 0: vtkArrayData containing a dense or sparse array.
 *
 * @sa
 * vtkArrayWriter
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National Laboratories.
*/

#ifndef vtkArrayReader_h
#define vtkArrayReader_h

#include "vtkIOCoreModule.h" // For export macro
#include "vtkArrayDataAlgorithm.h"

class VTKIOCORE_EXPORT vtkArrayReader :
  public vtkArrayDataAlgorithm
{
public:
  static vtkArrayReader* New();
  vtkTypeMacro(vtkArrayReader, vtkArrayDataAlgorithm);
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
  static vtkArray* Read(istream& stream);

  /**
   * Read an arbitrary array from a string.
   */
  static vtkArray* Read(const vtkStdString& str);

protected:
  vtkArrayReader();
  ~vtkArrayReader() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  char* FileName;
  vtkStdString InputString;
  bool ReadFromInputString;

private:
  vtkArrayReader(const vtkArrayReader&) = delete;
  void operator=(const vtkArrayReader&) = delete;
};

#endif

