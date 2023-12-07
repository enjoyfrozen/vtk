// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

/**
 * @class  vtkADIOS2ADIOS2DataArrayWriter
 * @brief  Write vtkDataArrays to ADIOS2 BP file.
 */

#ifndef vtkADIOS2DataArrayWriter_h
#define vtkADIOS2DataArrayWriter_h

#include "vtkIOADIOS2Module.h" // For export macro
#include "vtkObject.h"

VTK_ABI_NAMESPACE_BEGIN
class vtkDataArray;

class VTKIOADIOS2_EXPORT vtkADIOS2DataArrayWriter : public vtkObject
{
public:
  static vtkADIOS2DataArrayWriter* New();
  vtkTypeMacro(vtkADIOS2DataArrayWriter, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Get / set the filename where data will be stored (when used as a filter).
   */
  vtkSetFilePathMacro(FileName);
  vtkGetFilePathMacro(FileName);
  ///@}

  /**
   * @brief Different type of internal writers for the ADIOS2 file.  For
   * differences among the different ADIOS2 engines see
   * https://adios2.readthedocs.io/en/latest/engines/engines.html
   */
  enum class ADIOS2WriterType
  {
    BP3,
    BP4,
    BP5,
  };

  /**
   * Get/Set the engine type of the internal ADIOS2 writer.
   *
   * @param WriterType
   * @param ADIOS2WriterType
   */
  vtkSetEnumMacro(WriterType, ADIOS2WriterType);
  vtkGetEnumMacro(WriterType, ADIOS2WriterType);

  /**
   * Initialize the internal adios file using the given FileName and WriterType
   */
  bool Open();

  /**
   * Flush to the file in disk and close the writer. After the writer is
   * closed no more data can be written. Until \ref Open is called again.
   */
  void Close();

  /**
   * Same as Close except deletes the file, if created. This is useful to
   * interrupt the exporting on failure.
   */
  void Abort();

  // If file is not Opened this is a noop.
  void AddArray(vtkDataArray* array, const char* arrayname = nullptr);

  // Add @param attribute  under name @param attributeName as a string attribute in the BP file.
  // If file is not Opened this is a noop.
  void AddAttribute(const std::string& attribute, const std::string& attributeName);

  // Check whether an array of name @param arrayname has been already added into the file.
  bool ArrayExists(const char* arrayname);

  // Check whether an attribute of name @param attributeName and of type \p std::string has
  // been already added into the file.
  bool AttributeExists(const char* attributeName);

protected:
  vtkADIOS2DataArrayWriter();
  ~vtkADIOS2DataArrayWriter() override;

private:
  vtkADIOS2DataArrayWriter(const vtkADIOS2DataArrayWriter&) = delete;
  void operator=(const vtkADIOS2DataArrayWriter&) = delete;

  char* FileName = nullptr;
  ADIOS2WriterType WriterType = ADIOS2WriterType::BP3;

  class vtkInternals;
  vtkInternals* Internals;
};

VTK_ABI_NAMESPACE_END
#endif
