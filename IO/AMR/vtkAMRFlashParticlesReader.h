/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkAMRFlashParticlesReader
 *
 *
 *  A concrete instance of vtkAMRBaseParticlesReader that implements
 *  functionality for reading flash particle datasets.
*/

#ifndef vtkAMRFlashParticlesReader_h
#define vtkAMRFlashParticlesReader_h

#include "vtkIOAMRModule.h" // For export macro
#include "vtkAMRBaseParticlesReader.h"

class vtkIndent;
class vtkPolyData;
class vtkPointData;
class vtkIdList;
class vtkFlashReaderInternal;

class VTKIOAMR_EXPORT vtkAMRFlashParticlesReader :
  public vtkAMRBaseParticlesReader
{
public:
  static vtkAMRFlashParticlesReader* New();
  vtkTypeMacro( vtkAMRFlashParticlesReader, vtkAMRBaseParticlesReader );
  void PrintSelf(ostream &os, vtkIndent indent ) override;

  /**
   * See vtkAMRBaseParticlesReader::GetTotalNumberOfParticles.
   */
  int GetTotalNumberOfParticles() override;

protected:
  vtkAMRFlashParticlesReader();
  ~vtkAMRFlashParticlesReader() override;

  /**
   * See vtkAMRBaseParticlesReader::ReadMetaData
   */
  void ReadMetaData() override;

  /**
   * See vtkAMRBaseParticlesReader::SetupParticlesDataSelections
   */
  void SetupParticleDataSelections() override;

  /**
   * See vtkAMRBaseParticlesReader::ReadParticles
   */
  vtkPolyData* ReadParticles( const int blkidx ) override;

  /**
   * Reads the particlles of the given block from the given file.
   */
  vtkPolyData* GetParticles( const char* file, const int blkidx );

  vtkFlashReaderInternal *Internal;

private:
  vtkAMRFlashParticlesReader( const vtkAMRFlashParticlesReader& ) = delete;
  void operator=(const vtkAMRFlashParticlesReader& ) = delete;
};

#endif /* vtkAMRFlashParticlesReader_h */
