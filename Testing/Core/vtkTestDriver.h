/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// This header is included by all the C++ test drivers in VTK.
#ifndef vtkTestDriver_h
#define vtkTestDriver_h

#include <exception> // for std::exception
#include "vtkFloatingPointExceptions.h"

#include <clocale> // C setlocale()
#include <locale> // C++ locale

#include <vtksys/SystemInformation.hxx> // for stacktrace

#include <vtkLogger.h> // for logging

#include "vtkWindowsTestUtilities.h" // for windows stack trace

#endif
// VTK-HeaderTest-Exclude: vtkTestDriver.h
