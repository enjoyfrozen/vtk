/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkFacetReader
 * @brief   reads a dataset in Facet format
 *
 * vtkFacetReader creates a poly data dataset. It reads ASCII files
 * stored in Facet format
 *
 * The facet format looks like this:
 * FACET FILE ...
 * nparts
 * Part 1 name
 * 0
 * npoints 0 0
 * p1x p1y p1z
 * p2x p2y p2z
 * ...
 * 1
 * Part 1 name
 * ncells npointspercell
 * p1c1 p2c1 p3c1 ... pnc1 materialnum partnum
 * p1c2 p2c2 p3c2 ... pnc2 materialnum partnum
 * ...
*/

#ifndef vtkFacetReader_h
#define vtkFacetReader_h

#include "vtkFiltersHybridModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSHYBRID_EXPORT vtkFacetReader : public vtkPolyDataAlgorithm
{
public:
  static vtkFacetReader *New();
  vtkTypeMacro(vtkFacetReader,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify file name of Facet datafile to read
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  static int CanReadFile(const char *filename);

protected:
  vtkFacetReader();
  ~vtkFacetReader() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  char *FileName;

private:
  vtkFacetReader(const vtkFacetReader&) = delete;
  void operator=(const vtkFacetReader&) = delete;
};

#endif

