/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtk_json_forward.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef vtk_json_forward_h
#define vtk_json_forward_h

// VTK_MODULE_USE_EXTERNAL_VTK_json is defined in this header,
// so include it.
#include <vtk_json_external.h>

#if VTK_MODULE_USE_EXTERNAL_VTK_json
// using external JSON, no mangling needed.

#else

// JSON may get already defined if `vtk_json.h` was included,
// hence this check to avoid redefinition.
#if !defined(nlohmann)
// mangle the namespace.
#define nlohmann vtk_nlohmann
#endif // !defined(nlohmann)

#endif // VTK_MODULE_USE_EXTERNAL_VTK_json

#endif
