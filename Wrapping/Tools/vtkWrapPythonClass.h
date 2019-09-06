/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkWrapPythonClass_h
#define vtkWrapPythonClass_h

#include "vtkParse.h"
#include "vtkParseData.h"
#include "vtkParseHierarchy.h"

/* Wrap one class, returns zero if not wrappable */
int vtkWrapPython_WrapOneClass(
  FILE *fp, const char *module, const char *classname,
  ClassInfo *data, FileInfo *file_info, HierarchyInfo *hinfo,
  int is_vtkobject);

/* get the true superclass */
const char *vtkWrapPython_GetSuperClass(
  ClassInfo *data, HierarchyInfo *hinfo);

/* check whether the superclass of the specified class is wrapped,
   the module for the superclass is returned and is_external is
   set if the module is different from ours */
const char *vtkWrapPython_HasWrappedSuperClass(
  HierarchyInfo *hinfo, const char *classname, int *is_external);

/* generate the class docstring and write it to "fp" */
void vtkWrapPython_ClassDoc(
  FILE *fp, FileInfo *file_info, ClassInfo *data, HierarchyInfo *hinfo,
  int is_vtkobject);

#endif /* vtkWrapPythonClass_h */
/* VTK-HeaderTest-Exclude: vtkWrapPythonClass.h */
