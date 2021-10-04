/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtk_json.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef vtk_json_h
#define vtk_json_h

// VTK_MODULE_USE_EXTERNAL_VTK_json is defined in this header,
// so include it.
#include <vtk_json_external.h>

#if VTK_MODULE_USE_EXTERNAL_VTK_json
# include <nlohmann/json.hpp>
#else

#if defined(vtk_json_forward_h) && defined(vtk_nlohmann)
// vtk_json_forward.h defines VTK_JSON to help mangle forward declarations.
// However that can conflict with definitions in CLI.hpp, so we undef it here,
// if the header was already included.
#undef vtk_nlohmann
#endif

# include <vtkjson/nlohmann/json.hpp>
#endif // VTK_MODULE_USE_EXTERNAL_VTK_json

#endif
