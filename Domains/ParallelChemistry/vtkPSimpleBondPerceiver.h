/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPSimpleBondPerceiver
 * @brief   Create a simple guess of a molecule's topology
 *
 *
 * vtkPSimpleBondPerceiver is the parallel version of vtkSimpleBondPerceiver.
 * It computes ghost atoms, ghost bonds and then it calls algorithm from the
 * serial version.
 *
 * @par Thanks:
 * This class has been written by Kitware SAS from an initial work made by
 * Aymeric Pelle from Universite de Technologie de Compiegne, France,
 * and Laurent Colombet and Thierry Carrard from Commissariat a l'Energie
 * Atomique (CEA/DIF).
 */

#ifndef vtkPSimpleBondPerceiver_h
#define vtkPSimpleBondPerceiver_h

#include "vtkDomainsParallelChemistryModule.h" // For export macro
#include "vtkSimpleBondPerceiver.h"

class VTKDOMAINSPARALLELCHEMISTRY_EXPORT vtkPSimpleBondPerceiver : public vtkSimpleBondPerceiver
{
public:
  static vtkPSimpleBondPerceiver* New();
  vtkTypeMacro(vtkPSimpleBondPerceiver, vtkSimpleBondPerceiver);

protected:
  vtkPSimpleBondPerceiver() = default;
  ~vtkPSimpleBondPerceiver() = default;

  /**
   * Create ghosts level in molecule.
   * Return true if ghosts are correctly initialized.
   */
  bool CreateGhosts(vtkMolecule* molecule);

  /**
   * Compute the bonds. Reimplements Superclass to create ghost before.
   */
  void ComputeBonds(vtkMolecule* molecule) override;

private:
  vtkPSimpleBondPerceiver(const vtkPSimpleBondPerceiver&) = delete; // Not implemented.
  void operator=(const vtkPSimpleBondPerceiver&) = delete;          // Not implemented.
};
#endif
