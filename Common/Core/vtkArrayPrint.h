/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkArrayPrint
 * @brief   Print arrays in different formats
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National
 * Laboratories.
*/

#ifndef vtkArrayPrint_h
#define vtkArrayPrint_h

#include "vtkTypedArray.h"

/// @relates vtkArrayPrint
/// Serializes the contents of an array to a stream as a series of
/// coordinates.  For 2D arrays of double values, the output is compatible
/// with the MatrixMarket "Coordinate Text File" format.
template<typename T>
void vtkPrintCoordinateFormat(ostream& stream, vtkTypedArray<T>* array);

/// @relates vtkArrayPrint
/// Serializes the contents of a matrix to a stream in human-readable form.
template<typename T>
void vtkPrintMatrixFormat(ostream& stream, vtkTypedArray<T>* matrix);

/// @relates vtkArrayPrint
/// Serializes the contents of a vector to a stream in human-readable form.
template<typename T>
void vtkPrintVectorFormat(ostream& stream, vtkTypedArray<T>* vector);

#include "vtkArrayPrint.txx"

#endif
// VTK-HeaderTest-Exclude: vtkArrayPrint.h
