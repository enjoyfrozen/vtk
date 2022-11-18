/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestIndexedImplicitBackend.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkIndexedImplicitBackend.h"

#include "vtkDataArrayRange.h"
#include "vtkIdList.h"
#include "vtkIntArray.h"

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <random>

int TestIndexedImplicitBackend(int, char*[])
{
  vtkNew<vtkIntArray> baseArray;
  baseArray->SetNumberOfComponents(1);
  baseArray->SetNumberOfTuples(100);
  auto range = vtk::DataArrayValueRange<1>(baseArray);
  std::iota(range.begin(), range.end(), 0);

  vtkNew<vtkIdList> handles;
  handles->SetNumberOfIds(100);
  {
    std::vector<vtkIdType> buffer(100);
    std::iota(buffer.begin(), buffer.end(), 0);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(buffer.begin(), buffer.end(), g);
    for (vtkIdType idx = 0; idx < 100; idx++)
    {
      handles->SetId(idx, buffer[idx]);
    }
  }

  vtkIndexedImplicitBackend<int> backend(handles, baseArray);
  for (int idx = 0; idx < 100; idx++)
  {
    if (backend(idx) != static_cast<int>(handles->GetId(idx)))
    {
      std::cout << "Indexed backend evaluation failed with: " << backend(idx)
                << " != " << handles->GetId(idx) << std::endl;
    }
  }

  return EXIT_SUCCESS;
}
