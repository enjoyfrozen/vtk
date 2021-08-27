/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataCache.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDataCache.h"

#include "vtkNew.h"
#include "vtkObjectFactory.h"

#include <mutex>
#include <unordered_map>

//==========================================================================
namespace DataCacheNS
{

// this has to be defined here since it needs vtkDataCache.
void ObserverT::Cleanup()
{
  this->DataCache->Remove(this->DataKey, this->ObserverKey);
}

ObserverT::~ObserverT()
{
  for (auto& pair : this->ObserverIds)
  {
    pair.first->RemoveObserver(pair.second);
  }
  this->ObserverIds.clear();
}

void ObserverT::Monitor(vtkObject* object)
{
  if (this->ObserverIds.find(object) == this->ObserverIds.end())
  {
    auto event1 = object->AddObserver(vtkCommand::ModifiedEvent, this, &ObserverT::Cleanup);
    auto event2 = object->AddObserver(vtkCommand::DeleteEvent, this, &ObserverT::Cleanup);
    this->ObserverIds.emplace(object, event1);
    this->ObserverIds.emplace(object, event2);
  }
}

} // namespace DataCacheNS

class vtkDataCache::vtkInternals
{
public:
  mutable std::mutex CacheMutex;
  std::unordered_map<DataCacheNS::KeyT, vtkSmartPointer<vtkObjectBase>> Cache;

  mutable std::mutex ObserversMutex;
  std::unordered_map<DataCacheNS::KeyT, std::shared_ptr<DataCacheNS::ObserverT>> Observers;
};

vtkStandardNewMacro(vtkDataCache);
//----------------------------------------------------------------------------
vtkDataCache::vtkDataCache()
  : Internals(new vtkDataCache::vtkInternals())
{
}

//----------------------------------------------------------------------------
vtkDataCache::~vtkDataCache() = default;

//----------------------------------------------------------------------------
vtkDataCache* vtkDataCache::GetInstance()
{
  // FIXME:
  static auto cache = vtk::TakeSmartPointer(vtkDataCache::New());
  return cache;
}

//----------------------------------------------------------------------------
void vtkDataCache::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkObjectBase> vtkDataCache::GetCachedDataAsObject(
  const DataCacheNS::KeyT& key) const
{
  const auto& internals = (*this->Internals);
  try
  {
    const std::lock_guard<std::mutex> lock(internals.CacheMutex);
    return internals.Cache.at(key);
  }
  catch (std::out_of_range&)
  {
    return nullptr;
  }
}

//----------------------------------------------------------------------------
void vtkDataCache::AddToCacheInternal(vtkObjectBase* context, vtkObjectBase* data,
  const DataCacheNS::KeyT& dataKey, const DataCacheNS::KeyT& observerKey,
  std::shared_ptr<DataCacheNS::ObserverT>& observer)

{
  auto& internals = (*this->Internals);
  {
    const std::lock_guard<std::mutex> lock(internals.CacheMutex);
    internals.Cache[dataKey] = data;
  }
  {
    const std::lock_guard<std::mutex> lock(internals.ObserversMutex);
    internals.Observers[observerKey] = observer;
  }
}

//----------------------------------------------------------------------------
void vtkDataCache::Remove(const DataCacheNS::KeyT& dataKey, const DataCacheNS::KeyT& observerKey)
{
  auto& internals = (*this->Internals);

  {
    const std::lock_guard<std::mutex> lock(internals.ObserversMutex);
    internals.Observers.erase(observerKey);
  }

  {
    const std::lock_guard<std::mutex> lock(internals.CacheMutex);
    internals.Cache.erase(dataKey);
  }
}

//----------------------------------------------------------------------------
size_t vtkDataCache::GetNumberOfItems() const
{
  auto& internals = (*this->Internals);
  return internals.Cache.size();
}
