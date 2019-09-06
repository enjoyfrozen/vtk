/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOBJReader
 * @brief   read Wavefront .obj files
 *
 * vtkOBJReader is a source object that reads Wavefront .obj
 * files. The output of this source object is polygonal data.
 * @sa
 * vtkOBJImporter
*/

#ifndef vtkOBJReader_h
#define vtkOBJReader_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkAbstractPolyDataReader.h"

class VTKIOGEOMETRY_EXPORT vtkOBJReader : public vtkAbstractPolyDataReader
{
public:
  static vtkOBJReader *New();
  vtkTypeMacro(vtkOBJReader,vtkAbstractPolyDataReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
  * Get first comment in the file.
  * Comment may be multiple lines. # and leading spaces are removed.
  */
  vtkGetStringMacro(Comment);
  //@}

protected:
  vtkOBJReader();
  ~vtkOBJReader() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

  /**
  * Set comment string. Internal use only.
  */
  vtkSetStringMacro(Comment);

  char* Comment;

private:
  vtkOBJReader(const vtkOBJReader&) = delete;
  void operator=(const vtkOBJReader&) = delete;
};

#endif
