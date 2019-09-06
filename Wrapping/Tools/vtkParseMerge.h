/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 This file contains utility functions for merging together the
 methods for a class with those inherited from all superclasses.
*/

#ifndef vtkParseMerge_h
#define vtkParseMerge_h

#include "vtkParseData.h"
#include "vtkParseHierarchy.h"

/**
 * This struct is meant to supplement ClassInfo, it gives information
 * about which class (or classes) each method was inherited from
 */
typedef struct _MergeInfo
{
  int   NumberOfClasses;    /* number of classes in genealogy */
  const char **ClassNames;         /* class name */
  int   NumberOfFunctions;  /* must match FunctionInfo */
  int  *NumberOfOverrides; /* n classes that define this function */
  int **OverrideClasses;  /* class for the override */
} MergeInfo;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Merge all inherited methods into the ClassInfo.
 * This will find and parse the header files for all the superclasses,
 * and recursively add all inherited superclass methods into one ClassInfo.
 * The returned MergeInfo object provides information about which class
 * each inherited method was inherited from.
 */
MergeInfo *vtkParseMerge_MergeSuperClasses(
  FileInfo *finfo, NamespaceInfo *data, ClassInfo *classInfo);

/**
 * Create an initialized MergeInfo object.
 */
MergeInfo *vtkParseMerge_CreateMergeInfo(ClassInfo *classInfo);

/**
 * Free the MergeInfo object.
 */
void vtkParseMerge_FreeMergeInfo(MergeInfo *info);

/**
 * Recursive suproutine to inherit methods from "classname".
 * The class named by "classname" should be a superclass of the ClassInfo.
 * If the MergeInfo is not provided (if it is NULL), then the only methods
 * that are brought into the class are ones that are explicitly named
 * by using declarations.  The hintfile can also be NULL, if there is
 * no hints file.
 */
void vtkParseMerge_MergeHelper(
  FileInfo *finfo, const NamespaceInfo *data, const HierarchyInfo *hinfo,
  const char *classname, int nhintfiles, char **hintfiles, MergeInfo *info,
  ClassInfo *merge);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/* VTK-HeaderTest-Exclude: vtkParseMerge.h */
