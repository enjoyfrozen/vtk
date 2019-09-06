/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOpenQubeMoleculeSource
 * @brief   Read a OpenQube readable file and output
 * a vtkMolecule object
 *
 *
*/

#ifndef vtkOpenQubeMoleculeSource_h
#define vtkOpenQubeMoleculeSource_h

#include "vtkDomainsChemistryModule.h" // For export macro
#include "vtkDataReader.h"

class vtkMolecule;

namespace OpenQube
{
  class Molecule;
  class BasisSet;
}

class VTKDOMAINSCHEMISTRY_EXPORT vtkOpenQubeMoleculeSource : public vtkDataReader
{
public:
  static vtkOpenQubeMoleculeSource *New();
  vtkTypeMacro(vtkOpenQubeMoleculeSource,vtkDataReader);
  void PrintSelf(ostream& os, vtkIndent indent);

  //@{
  /**
   * Get/Set the output (vtkMolecule) that the reader will fill
   */
  vtkMolecule *GetOutput();
  void SetOutput(vtkMolecule *);
  //@}

  //@{
  /**
   * Get/Set the name of the OpenQube readable file.
   * @note: If both a source OpenQube BasisSet object and a FileName
   * have been set with SetBasisSet and SetFileName, the object takes
   * precedence over the file and the file will not be read.
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  //@{
  /**
   * Get/Set the OpenQube BasisSet object to read from.
   * @note: If both a source OpenQube BasisSet object and a FileName
   * have been set with SetBasisSet and SetFileName, the object takes
   * precedence over the file and the file will not be read.
   */
  virtual void SetBasisSet(OpenQube::BasisSet *b);
  vtkGetMacro(BasisSet, OpenQube::BasisSet*);
  //@}

  //@{
  /**
   * Get/Set whether or not to take ownership of the BasisSet object. Defaults
   * to false when SetBasisSet is used and true when the basis is read from a
   * file set by SetFileName. Destroying this class or setting a new BasisSet
   * or FileName will delete the BasisSet if true.
   */
  vtkSetMacro(CleanUpBasisSet, bool);
  vtkGetMacro(CleanUpBasisSet, bool);
  vtkBooleanMacro(CleanUpBasisSet, bool);
  //@}

protected:
  vtkOpenQubeMoleculeSource();
  ~vtkOpenQubeMoleculeSource();

  int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *);
  int FillOutputPortInformation(int, vtkInformation*);

  char *FileName;
  OpenQube::BasisSet *BasisSet;
  bool CleanUpBasisSet;

  /**
   * Copy the OpenQube::Molecule object @a oqmol into the provided
   * vtkMolecule object @a mol.
   */
  void CopyOQMoleculeToVtkMolecule(const OpenQube::Molecule *oqmol,
                                   vtkMolecule *mol);

private:
  vtkOpenQubeMoleculeSource(const vtkOpenQubeMoleculeSource&) = delete;
  void operator=(const vtkOpenQubeMoleculeSource&) = delete;
};

#endif
