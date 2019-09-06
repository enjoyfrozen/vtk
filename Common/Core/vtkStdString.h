/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkStdString
 * @brief   Wrapper around std::string to keep symbols short.
 *
 * vtkStdString derives from std::string to provide shorter symbol
 * names than basic_string<...> in namespace std given by the standard
 * STL string.
*/

#ifndef vtkStdString_h
#define vtkStdString_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkSystemIncludes.h" // For VTKCOMMONCORE_EXPORT.
#include <string>       // For the superclass.

class vtkStdString;
VTKCOMMONCORE_EXPORT ostream& operator<<(ostream&, const vtkStdString&);

class vtkStdString : public std::string
{
public:
  typedef std::string StdString;
  typedef StdString::value_type             value_type;
  typedef StdString::pointer                pointer;
  typedef StdString::reference              reference;
  typedef StdString::const_reference        const_reference;
  typedef StdString::size_type              size_type;
  typedef StdString::difference_type        difference_type;
  typedef StdString::iterator               iterator;
  typedef StdString::const_iterator         const_iterator;
  typedef StdString::reverse_iterator       reverse_iterator;
  typedef StdString::const_reverse_iterator const_reverse_iterator;

  vtkStdString(): StdString() {}
  vtkStdString(const value_type* s): StdString(s) {}
  vtkStdString(const value_type* s, size_type n): StdString(s, n) {}
  vtkStdString(const StdString& s, size_type pos=0, size_type n=npos):
    StdString(s, pos, n) {}

  operator const char *() { return this->c_str(); }
};

#endif
// VTK-HeaderTest-Exclude: vtkStdString.h
