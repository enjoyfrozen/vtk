/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkISIReader
 * @brief   reader for ISI files
 *
 *
 * ISI is a tagged format for expressing bibliographic citations.  Data is
 * structured as a collection of records with each record composed of
 * one-to-many fields.  See
 *
 * http://isibasic.com/help/helpprn.html#dialog_export_format
 *
 * for details.  vtkISIReader will convert an ISI file into a vtkTable, with
 * the set of table columns determined dynamically from the contents of the
 * file.
*/

#ifndef vtkISIReader_h
#define vtkISIReader_h

#include "vtkIOInfovisModule.h" // For export macro
#include "vtkTableAlgorithm.h"

class vtkTable;

class VTKIOINFOVIS_EXPORT vtkISIReader : public vtkTableAlgorithm
{
public:
  static vtkISIReader* New();
  vtkTypeMacro(vtkISIReader,vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set/get the file to load
   */
  vtkGetStringMacro(FileName);
  vtkSetStringMacro(FileName);
  //@}

  //@{
  /**
   * Set/get the delimiter to be used for concatenating field data (default: ";")
   */
  vtkGetStringMacro(Delimiter);
  vtkSetStringMacro(Delimiter);
  //@}

  //@{
  /**
   * Set/get the maximum number of records to read from the file (zero = unlimited)
   */
  vtkGetMacro(MaxRecords,int);
  vtkSetMacro(MaxRecords,int);
  //@}

 protected:
  vtkISIReader();
  ~vtkISIReader() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  char* FileName;
  char* Delimiter;
  int MaxRecords;

private:
  vtkISIReader(const vtkISIReader&) = delete;
  void operator=(const vtkISIReader&) = delete;
};

#endif
