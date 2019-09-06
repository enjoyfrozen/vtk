/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkXYZMolReader2
 * @brief   read Molecular Data files
 *
 * vtkXYZMolReader2 is a source object that reads Molecule files
 * The reader will detect multiple timesteps in an XYZ molecule file.
 *
 * @par Thanks:
 * Dr. Jean M. Favre who developed and contributed this class
*/

#ifndef vtkXYZMolReader2_h
#define vtkXYZMolReader2_h

#include "vtkDomainsChemistryModule.h" // For export macro
#include "vtkMoleculeAlgorithm.h"

#include <vector>   // for std::vector
#include <istream>  // for std::istream

class vtkMolecule;

class VTKDOMAINSCHEMISTRY_EXPORT vtkXYZMolReader2 : public vtkMoleculeAlgorithm
{
public:
  static vtkXYZMolReader2 *New();
  vtkTypeMacro(vtkXYZMolReader2,vtkMoleculeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get/Set the output (vtkMolecule) that the reader will fill
   */
  vtkMolecule *GetOutput();
  void SetOutput(vtkMolecule *) override;
  //@}

  //@{
  /**
   * Get/Set the name of the XYZ Molecule file
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

protected:
  vtkXYZMolReader2();
  ~vtkXYZMolReader2() override;

  int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *) override;
  int RequestInformation(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *) override;

  char *FileName;
  std::vector<istream::pos_type> file_positions; // to store beginning of each tstep
  std::vector<double> TimeSteps;

  int NumberOfTimeSteps;
  int NumberOfAtoms;

private:
  vtkXYZMolReader2(const vtkXYZMolReader2&) = delete;
  void operator=(const vtkXYZMolReader2&) = delete;
};

#endif
