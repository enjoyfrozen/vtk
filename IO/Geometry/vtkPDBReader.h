/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPDBReader
 * @brief   read Molecular Data files
 *
 * vtkPDBReader is a source object that reads Molecule files
 * The FileName must be specified
 *
 * @par Thanks:
 * Dr. Jean M. Favre who developed and contributed this class
*/

#ifndef vtkPDBReader_h
#define vtkPDBReader_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkMoleculeReaderBase.h"


class VTKIOGEOMETRY_EXPORT vtkPDBReader : public vtkMoleculeReaderBase
{
public:
  vtkTypeMacro(vtkPDBReader,vtkMoleculeReaderBase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkPDBReader *New();

protected:
  vtkPDBReader();
  ~vtkPDBReader() override;

  void ReadSpecificMolecule(FILE* fp) override;

private:
  vtkPDBReader(const vtkPDBReader&) = delete;
  void operator=(const vtkPDBReader&) = delete;
};

#endif
