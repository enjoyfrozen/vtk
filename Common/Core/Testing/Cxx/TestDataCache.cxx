/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestDataCache.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkCommand.h"
#include "vtkDataCache.h"
#include "vtkIntArray.h"
#include "vtkLogger.h"
#include "vtkObject.h"
#include "vtkSmartPointer.h"

int TestDataCache(int argc, char* argv[])
{
  auto obj = vtk::TakeSmartPointer(vtkObject::New());

  // A quick check for keys to ensure it works as expected.
  DataCacheNS::KeyT k(std::string("alpha"), 10, obj);
  vtkLogF(INFO, "hash = %ld", k.GetHash());

  DataCacheNS::KeyT k2(std::string("alpha"), 10, obj);
  vtkLogIfF(ERROR, k != k2, "keys must be identical!");

  obj->Modified();
  DataCacheNS::KeyT k3(std::string("alpha"), 10, obj);
  vtkLogIfF(ERROR, k == k3, "keys must not identical!");

  auto cache = vtkDataCache::GetInstance();
  vtkLogIfF(ERROR, cache->GetCachedData<vtkIntArray>(10, obj) != nullptr, "null expected");

  auto context = vtk::TakeSmartPointer(vtkObject::New());

  auto data = vtk::TakeSmartPointer(vtkIntArray::New());
  cache->AddToCache(context, data, 10, obj);

  vtkLogIfF(ERROR, cache->GetCachedData<vtkIntArray>(10, obj) != data, "invalid cache");

  // now cache should become obsolete.
  obj->Modified();
  vtkLogIfF(ERROR, cache->GetNumberOfItems() != 0, "cache was not cleared");

  // now let's modify context.
  cache->AddToCache(context, data, 10, obj);
  vtkLogIfF(ERROR, cache->GetCachedData<vtkIntArray>(10, obj) != data, "invalid cache");
  context->Modified();
  vtkLogIfF(ERROR, cache->GetNumberOfItems() != 0, "cache was not cleared");

  return EXIT_SUCCESS;
}
