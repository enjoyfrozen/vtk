/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSMPTools.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkSMPTools.h"

//------------------------------------------------------------------------------
void vtkSMPTools::Initialize(int numThreads)
{
  // Can only set number threads in vtkm if using tbb or openmp
  std::cout << "Unable to initialize " << numThreads << " threads as requested\n";
}

//------------------------------------------------------------------------------
int vtkSMPTools::GetEstimatedNumberOfThreads()
{
  std::cout << "Unable to get estimated number of threads as requested\n";
  return (1);
}
