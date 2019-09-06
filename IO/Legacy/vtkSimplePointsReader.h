/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSimplePointsReader
 * @brief   Read a list of points from a file.
 *
 * vtkSimplePointsReader is a source object that reads a list of
 * points from a file.  Each point is specified by three
 * floating-point values in ASCII format.  There is one point per line
 * of the file.  A vertex cell is created for each point in the
 * output.  This reader is meant as an example of how to write a
 * reader in VTK.
*/

#ifndef vtkSimplePointsReader_h
#define vtkSimplePointsReader_h

#include "vtkIOLegacyModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKIOLEGACY_EXPORT vtkSimplePointsReader : public vtkPolyDataAlgorithm
{
public:
  static vtkSimplePointsReader* New();
  vtkTypeMacro(vtkSimplePointsReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set/Get the name of the file from which to read points.
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

protected:
  vtkSimplePointsReader();
  ~vtkSimplePointsReader() override;

  char* FileName;

  int RequestData(vtkInformation*,
                  vtkInformationVector**,
                  vtkInformationVector*) override;
private:
  vtkSimplePointsReader(const vtkSimplePointsReader&) = delete;
  void operator=(const vtkSimplePointsReader&) = delete;
};

#endif
