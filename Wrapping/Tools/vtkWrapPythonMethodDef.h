/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkWrapPythonMethodDef_h
#define vtkWrapPythonMethodDef_h

#include "vtkParse.h"
#include "vtkParseData.h"
#include "vtkParseHierarchy.h"

/* check whether a method is wrappable */
int vtkWrapPython_MethodCheck(
  ClassInfo *data, FunctionInfo *currentFunction, HierarchyInfo *hinfo);

/* print out all methods and the method table */
void vtkWrapPython_GenerateMethods(
  FILE *fp, const char *classname, ClassInfo *data,
  FileInfo *finfo, HierarchyInfo *hinfo,
  int is_vtkobject, int do_constructors);

#endif /* vtkWrapPythonMethodDef_h */
/* VTK-HeaderTest-Exclude: vtkWrapPythonMethodDef.h */
