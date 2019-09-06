/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkAbstractPolyDataReader
 * @brief   Superclass for algorithms that read
 * models from a file.
 *
 * This class allows to use a single base class to manage AbstractPolyData
 * reader classes in a uniform manner without needing to know the actual
 * type of the reader.
 * i.e. makes it possible to create maps to associate filename extension
 * and vtkAbstractPolyDataReader object.
 *
 * @sa
 * vtkOBJReader vtkPLYReader vtkSTLReader
*/

#ifndef vtkAbstractPolyDataReader_h
#define vtkAbstractPolyDataReader_h

#include "vtkIOCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKIOCORE_EXPORT vtkAbstractPolyDataReader : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkAbstractPolyDataReader, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify file name of AbstractPolyData file (obj / ply / stl).
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

protected:
  vtkAbstractPolyDataReader();
  ~vtkAbstractPolyDataReader() override;

  char *FileName;
private:
  vtkAbstractPolyDataReader(const vtkAbstractPolyDataReader&) = delete;
  void operator=(const vtkAbstractPolyDataReader&) = delete;
};

#endif
