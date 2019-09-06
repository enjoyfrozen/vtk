/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkEnSightMasterServerReader
 * @brief   reader for compound EnSight files
*/

#ifndef vtkEnSightMasterServerReader_h
#define vtkEnSightMasterServerReader_h

#include "vtkIOEnSightModule.h" // For export macro
#include "vtkGenericEnSightReader.h"

class vtkCollection;

class VTKIOENSIGHT_EXPORT vtkEnSightMasterServerReader : public vtkGenericEnSightReader
{
public:
  vtkTypeMacro(vtkEnSightMasterServerReader, vtkGenericEnSightReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkEnSightMasterServerReader* New();

  /**
   * Determine which file should be read for piece
   */
  int DetermineFileName(int piece);

  //@{
  /**
   * Get the file name that will be read.
   */
  vtkGetStringMacro(PieceCaseFileName);
  //@}

  //@{
  /**
   * Set or get the current piece.
   */
  vtkSetMacro(CurrentPiece, int);
  vtkGetMacro(CurrentPiece, int);
  //@}

  int CanReadFile(const char *fname) override;

protected:
  vtkEnSightMasterServerReader();
  ~vtkEnSightMasterServerReader() override;

  int RequestData(vtkInformation *, vtkInformationVector **,
                          vtkInformationVector *) override;
  int RequestInformation(vtkInformation *, vtkInformationVector **,
                                 vtkInformationVector *) override;

  vtkSetStringMacro(PieceCaseFileName);
  char* PieceCaseFileName;
  int MaxNumberOfPieces;
  int CurrentPiece;

private:
  vtkEnSightMasterServerReader(const vtkEnSightMasterServerReader&) = delete;
  void operator=(const vtkEnSightMasterServerReader&) = delete;
};

#endif
