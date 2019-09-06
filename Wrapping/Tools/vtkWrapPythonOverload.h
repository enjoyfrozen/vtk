/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkWrapPythonOverload_h
#define vtkWrapPythonOverload_h

#include "vtkParse.h"
#include "vtkParseData.h"
#include "vtkParseHierarchy.h"

/* output the method table for all overloads of a particular method */
void vtkWrapPython_OverloadMethodDef(
  FILE *fp, const char *classname, ClassInfo *data, int *overloadMap,
  FunctionInfo **wrappedFunctions, int numberOfWrappedFunctions, int fnum,
  int numberOfOccurrences, int all_legacy);

/* a master method to choose which overload to call */
void vtkWrapPython_OverloadMasterMethod(
  FILE *fp, const char *classname, int *overloadMap, int maxArgs,
  FunctionInfo **wrappedFunctions, int numberOfWrappedFunctions, int fnum,
  int is_vtkobject, int all_legacy);

/* generate an int array that maps arg counts to overloads */
int *vtkWrapPython_ArgCountToOverloadMap(
  FunctionInfo **wrappedFunctions, int numberOfWrappedFunctions,
  int fnum, int is_vtkobject, int *nmax, int *overlap);

#endif /* vtkWrapPythonOverload_h */
/* VTK-HeaderTest-Exclude: vtkWrapPythonOverload.h */
