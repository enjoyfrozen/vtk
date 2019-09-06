/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * This file contains utilities doing name mangling
 */

#ifndef vtkParseMangle_h
#define vtkParseMangle_h

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Generate a mangled name for a type, use gcc ia64 ABI.
 * The result is placed in new_name, which must be large enough
 * to accept the result.
 */
size_t vtkParse_MangledTypeName(const char *name, char *new_name);

/**
 * Generate a mangled name for a literal.  Only handles decimal
 * integer literals.  It guesses type from suffix "u", "ul",
 * "ull", "l", "ll" so only certain types are supported.
 */
size_t vtkParse_MangledLiteral(const char *name, char *new_name);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
/* VTK-HeaderTest-Exclude: vtkParseMangle.h */
