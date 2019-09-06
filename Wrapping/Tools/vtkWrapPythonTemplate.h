/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkWrapPythonTemplate_h
#define vtkWrapPythonTemplate_h

#include "vtkParse.h"
#include "vtkParseData.h"
#include "vtkParseHierarchy.h"

/* if name has template args, convert to pythonic dict format */
size_t vtkWrapPython_PyTemplateName(const char *name, char *pname);

/* wrap a templated class */
int vtkWrapPython_WrapTemplatedClass(
  FILE *fp, ClassInfo *data, FileInfo *file_info, HierarchyInfo *hinfo);

#endif /* vtkWrapPythonTemplate_h */
/* VTK-HeaderTest-Exclude: vtkWrapPythonTemplate.h */
