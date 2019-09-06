/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkWrapPythonNamespace_h
#define vtkWrapPythonNamespace_h

#include "vtkParse.h"
#include "vtkParseData.h"
#include "vtkParseHierarchy.h"

/* Wrap one class, returns zero if not wrappable */
int vtkWrapPython_WrapNamespace(
  FILE *fp, const char *module, NamespaceInfo *data);

#endif /* vtkWrapPythonNamespace_h */
/* VTK-HeaderTest-Exclude: vtkWrapPythonNamespace.h */
