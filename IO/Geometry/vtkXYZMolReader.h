/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXYZMolReader
 * @brief   read Molecular Data files
 *
 * vtkXYZMolReader is a source object that reads Molecule files
 * The FileName must be specified
 *
 * @par Thanks:
 * Dr. Jean M. Favre who developed and contributed this class
*/

#ifndef vtkXYZMolReader_h
#define vtkXYZMolReader_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkMoleculeReaderBase.h"


class VTKIOGEOMETRY_EXPORT vtkXYZMolReader : public vtkMoleculeReaderBase
{
public:
  vtkTypeMacro(vtkXYZMolReader,vtkMoleculeReaderBase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkXYZMolReader *New();

  /**
   * Test whether the file with the given name can be read by this
   * reader.
   */
  virtual int CanReadFile(const char* name);

  //@{
  /**
   * Set the current time step. It should be greater than 0 and smaller than
   * MaxTimeStep.
   */
  vtkSetMacro(TimeStep, int);
  vtkGetMacro(TimeStep, int);
  //@}

  //@{
  /**
   * Get the maximum time step.
   */
  vtkGetMacro(MaxTimeStep, int);
  //@}

protected:
  vtkXYZMolReader();
  ~vtkXYZMolReader() override;

  void ReadSpecificMolecule(FILE* fp) override;

  /**
   * Get next line that is not a comment. It returns the beginning of data on
   * line (skips empty spaces)
   */
  char* GetNextLine(FILE* fp, char* line, int maxlen);

  int GetLine1(const char* line, int *cnt);
  int GetLine2(const char* line, char *name);
  int GetAtom(const char* line, char* atom, float *x);

  void InsertAtom(const char* atom, float *pos);

  vtkSetMacro(MaxTimeStep, int);

  int TimeStep;
  int MaxTimeStep;

private:
  vtkXYZMolReader(const vtkXYZMolReader&) = delete;
  void operator=(const vtkXYZMolReader&) = delete;
};

#endif
