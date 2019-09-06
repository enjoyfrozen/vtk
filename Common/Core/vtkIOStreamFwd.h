/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkIOStreamFwd
 * @brief   Forward-declare C++ iostreams as used by VTK.
 *
 * This header forward-declares the proper streams.
*/

#ifndef vtkIOStreamFwd_h
#define vtkIOStreamFwd_h

#include "vtkConfigure.h"

#ifdef _MSC_VER
#pragma warning (push, 3)
#endif

// Forward-declare ansi streams.
#include <iosfwd>
using std::ios;
using std::streambuf;
using std::istream;
using std::ostream;
using std::iostream;
using std::filebuf;
using std::ifstream;
using std::ofstream;
using std::fstream;

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // vtkIOStreamFwd_h
// VTK-HeaderTest-Exclude: vtkIOStreamFwd.h
