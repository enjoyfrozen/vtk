/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef PyVTKExtras_h
#define PyVTKExtras_h

#include "vtkWrappingPythonCoreModule.h" // For export macro
#include "vtkPython.h"

//--------------------------------------------------------------------
// This will add extras to the provided dict.  It is called during the
// initialization of the vtkCommonCore python module.
extern "C"
{
VTKWRAPPINGPYTHONCORE_EXPORT void PyVTKAddFile_PyVTKExtras(PyObject *dict);
}

#endif
