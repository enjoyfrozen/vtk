/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestWeakPtr.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkNew.h"
#include "vtkObject.h"
#include "vtkWeakPtr.h"

#include <iostream>

#include <cstdlib>

#define ERROR(...) std::cerr << "ERROR: " __VA_ARGS__ << std::endl

int TestWeakPtr(int, char*[])
{
  // Test default construction.
  {
    vtkWeakPtr<vtkObject> weak;
    if (weak)
    {
      ERROR("default construction is truthy for `if (weak)`");
    }
    if (!weak)
    {
      // Expected.
    }
    else
    {
      ERROR("default construction is false-y for `if (!weak)`");
    }
  }

  // Test pointer construction and assignment.
  {
    vtkNew<vtkObject> obj;

    vtkWeakPtr<vtkObject> weak(obj);
    if (!weak)
    {
      ERROR("ptr construction is truthy for `if (!weak)`");
    }
    if (weak)
    {
      // Expected.
    }
    else
    {
      ERROR("ptr construction is false-y for `if (weak)`");
    }
  }

  // Test copy construction and assignment.
  {
    vtkNew<vtkObject> obj;

    vtkWeakPtr<vtkObject> weak1(obj);
    vtkWeakPtr<vtkObject> weak(weak1);
    if (!weak)
    {
      ERROR("ptr copy construction is truthy for `if (!weak)`");
    }
    if (weak)
    {
      // Expected.
    }
    else
    {
      ERROR("ptr copy construction is false-y for `if (weak)`");
    }

    vtkWeakPtr<vtkObject> weak3;
    weak = weak3;
    if (weak)
    {
      ERROR("default copy assignment is truthy for `if (weak)`");
    }
    if (!weak)
    {
      // Expected.
    }
    else
    {
      ERROR("default copy assignment is false-y for `if (!weak)`");
    }
  }

  // Test move construction and assignment.
  {
    vtkNew<vtkObject> obj;

    vtkWeakPtr<vtkObject> weak1(obj);
    vtkWeakPtr<vtkObject> weak(std::move(weak1));
    if (!weak)
    {
      ERROR("ptr move construction is truthy for `if (!weak)`");
    }
    if (weak)
    {
      // Expected.
    }
    else
    {
      ERROR("ptr move construction is false-y for `if (weak)`");
    }

    // Use the moved-from `weak1` to move-assign.
    weak = std::move(weak1);
    if (weak)
    {
      ERROR("default move assignment is truthy for `if (weak)`");
    }
    if (!weak)
    {
      // Expected.
    }
    else
    {
      ERROR("default move assignment is false-y for `if (!weak)`");
    }
  }

  // Test comparisons.
  {
    vtkNew<vtkObject> obj1;
    vtkNew<vtkObject> obj2;

    vtkWeakPtr<vtkObject> weak0a;
    vtkWeakPtr<vtkObject> weak0b;
    vtkWeakPtr<vtkObject> weak1a(obj1);
    vtkWeakPtr<vtkObject> weak1b(obj1);
    vtkWeakPtr<vtkObject> weak2(obj2);

    if (weak0a.owner_before(weak0a))
    {
      ERROR("default constructed is truthy for `weak0a 'before' weak0a`");
    }
    if (weak0a.owner_before(weak0b) || weak0b.owner_before(weak0a))
    {
      ERROR("default constructed is truthy for `weak0a 'before' weak0b` (or vice versa)");
    }

    if (weak1a.owner_before(weak1a))
    {
      ERROR("ptr constructed is truthy for `weak1a 'before' weak1a`");
    }

    if (weak1a.owner_before(weak1b) || weak1b.owner_before(weak1a))
    {
      ERROR("ptr constructed is false-y for `weak1a == weak1b`");
    }

    if (!weak1a.owner_before(weak2) && !weak2.owner_before(weak1a))
    {
      ERROR("ptr constructed is truthy for `weak1a == weak2`");
    }
  }

  // Test `Lock`
  {
    vtkWeakPtr<vtkObject> weak;

    if (auto* ptr = weak.Lock())
    {
      ERROR("default constructed gave a non-`nullptr` for `Lock`");
    }

    {
      vtkNew<vtkObject> obj;
      weak = obj;

      if (auto* ptr = weak.Lock())
      {
        if (ptr != obj)
        {
          ERROR("ptr assignment gave the wrong value for `Lock`");
        }

        // We got a new reference from `Lock`, so release it here.
        ptr->UnRegister(nullptr);
      }
      else
      {
        ERROR("ptr assignment gave a `nullptr` for `Lock`");
      }
    }

    if (weak)
    {
      ERROR("ptr assignment to a deleted object is truthy for `if (weak)`");
    }
  }

  return EXIT_SUCCESS;
}
