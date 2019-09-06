/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkTextureIO
 *
 * A small collection of I/O routines that write vtkTextureObject
 * to disk for debugging.
*/

#ifndef vtkTextureIO_h
#define vtkTextureIO_h

#include "vtkRenderingLICOpenGL2Module.h" // for export
#include "vtkPixelExtent.h" // for pixel extent

// included vtkSystemIncludes in vtkPixelExtent
#include <cstddef> // for NULL
#include <string> // for string
#include <deque> // for deque

class vtkTextureObject;

class VTKRENDERINGLICOPENGL2_EXPORT vtkTextureIO
{
public:
  /**
   * Write to disk as image data with subset(optional) at dataset origin(optional)
   */
  static void Write(
          const char *filename,
          vtkTextureObject *texture,
          const unsigned int *subset=nullptr,
          const double *origin=nullptr);

  /**
   * Write to disk as image data with subset(optional) at dataset origin(optional)
   */
  static void Write(
          std::string filename,
          vtkTextureObject *texture,
          const unsigned int *subset=nullptr,
          const double *origin=nullptr)
  {
      Write(filename.c_str(), texture, subset, origin);
  }

  /**
   * Write to disk as image data with subset(optional) at dataset origin(optional)
   */
  static void Write(
          std::string filename,
          vtkTextureObject *texture,
          const vtkPixelExtent &subset,
          const double *origin=nullptr)
  {
      Write(filename.c_str(), texture, subset.GetDataU(), origin);
  }

  /**
   * Write list of subsets to disk as multiblock image data at dataset origin(optional).
   */
  static void Write(
          const char *filename,
          vtkTextureObject *texture,
          const std::deque<vtkPixelExtent> &exts,
          const double *origin=nullptr);

  //@{
  /**
   * Write list of subsets to disk as multiblock image data at dataset origin(optional).
   */
  static void Write(
          std::string filename,
          vtkTextureObject *texture,
          const std::deque<vtkPixelExtent> &exts,
          const double *origin=nullptr)
  {
      Write(filename.c_str(),texture,exts,origin);
  }
};
  //@}

#endif
// VTK-HeaderTest-Exclude: vtkTextureIO.h
