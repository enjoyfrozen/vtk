/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataObjectCache.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDataObjectCache.h"

#include <algorithm>

#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataObject.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkWeakPointer.h"

#include <vector>

vtkStandardNewMacro(vtkDataObjectCache);

using DataObjectPair = std::pair<vtkWeakPointer<vtkDataObject>, vtkSmartPointer<vtkDataObject>>;
using DataObjectPairVec = std::vector<DataObjectPair>;

//------------------------------------------------------------------------------
class vtkDataObjectCacheInternals
{
public:
  DataObjectPairVec Cache;
};

//------------------------------------------------------------------------------
vtkDataObjectCache::vtkDataObjectCache()
{
  this->Internal = new vtkDataObjectCacheInternals;
}

//------------------------------------------------------------------------------
vtkDataObjectCache::~vtkDataObjectCache()
{
  delete this->Internal;
}

//------------------------------------------------------------------------------
void vtkDataObjectCache::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Cache size: " << this->GetCacheSize();
  os << indent << "Number of out-of-date items: " << this->GetNumberOfInvalidItems();
}

//------------------------------------------------------------------------------
bool vtkDataObjectCache::Release()
{
  const bool cacheWasEmpty = this->Internal->Cache.empty();
  this->Internal->Cache = {};
  if (!cacheWasEmpty)
  {
    this->Modified();
  }
  return !cacheWasEmpty;
}

//------------------------------------------------------------------------------
bool vtkDataObjectCache::Clear()
{
  const bool cacheWasEmpty = this->Internal->Cache.empty();
  this->Internal->Cache.clear();
  if (!cacheWasEmpty)
  {
    this->Modified();
  }
  return !cacheWasEmpty;
}

//------------------------------------------------------------------------------
bool vtkDataObjectCache::Update(vtkCompositeDataSet* inComposite)
{
  // count the number of leaves
  size_t count = 0;
  if (inComposite)
  {
    auto iter = vtkSmartPointer<vtkCompositeDataIterator>::Take(inComposite->NewIterator());
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
      if (!iter->GetCurrentDataObject()->IsA("vtkCompositeDataSet"))
      {
        ++count;
      }
    }
  }
  if (count == 0)
  {
    return Clear();
  }

  // create new storage
  DataObjectPairVec pairVec;
  size_t last = 0;
  auto iter = vtkSmartPointer<vtkCompositeDataIterator>::Take(inComposite->NewIterator());
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
  {
    last = std::max<size_t>(last, iter->GetCurrentFlatIndex());
  }
  pairVec.resize(last + 1);
  bool changed = pairVec.size() != this->Internal->Cache.size();

  // Store the input blocks and the existing output blocks that are still
  // up-to-date
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
  {
    const auto inObject = iter->GetCurrentDataObject();
    const auto index = iter->GetCurrentFlatIndex();
    auto outObject = this->FindObject(inObject, index);
    if (outObject && outObject->GetMTime() < inObject->GetMTime())
    {
      outObject = nullptr;
    }
    pairVec[index] = { inObject, outObject };
    if (!changed && this->Internal->Cache.at(index) != pairVec[index])
    {
      changed = true;
    }
  }

  // replace the cache
  std::swap(pairVec, this->Internal->Cache);

  if (changed)
  {
    this->Modified();
  }
  return changed;
}

//------------------------------------------------------------------------------
bool vtkDataObjectCache::Finalize(
  vtkCompositeDataSet* inComposite, vtkCompositeDataSet* outComposite)
{
  bool changed = false;
  auto& cache = this->Internal->Cache;
  const bool cacheWasEmpty = cache.empty();
  if (cacheWasEmpty && inComposite)
  {
    // count the number of leaves
    size_t count = 0;
    auto iter = vtkSmartPointer<vtkCompositeDataIterator>::Take(inComposite->NewIterator());
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
      ++count;
    }
    if (count < 1)
    {
      return changed;
    }
    cache.resize(count + 1);
    changed = true;
  }

  auto iter = vtkSmartPointer<vtkCompositeDataIterator>::Take(inComposite->NewIterator());
  for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
  {
    const auto inObject = iter->GetCurrentDataObject();
    const auto index = iter->GetCurrentFlatIndex();
    const auto outObject = outComposite ? outComposite->GetDataSet(index) : nullptr;
    if (cacheWasEmpty)
    {
      cache[index] = { inObject, outObject };
    }
    else if (cache[index].second != outObject)
    {
      cache[index].second = outObject;
      changed = true;
    }
  }

  if (changed)
  {
    this->Modified();
  }
  return changed;
}

//------------------------------------------------------------------------------
bool vtkDataObjectCache::Contains(vtkDataObject* inObj, vtkIdType hint) const
{
  const auto& cache = this->Internal->Cache;
  if (!inObj || cache.empty())
  {
    return false;
  }
  if (hint > -1 && hint < cache.size() && cache[hint].first == inObj)
  {
    return true;
  }

  auto hasInObj = [inObj](const DataObjectPair& pair) { return pair.first == inObj; };
  return std::find_if(cache.begin(), cache.end(), hasInObj) != cache.end();
}

//------------------------------------------------------------------------------
vtkDataObject* vtkDataObjectCache::FindObject(vtkDataObject* inObj, vtkIdType hint) const
{
  const auto& cache = this->Internal->Cache;
  if (!inObj || cache.empty())
  {
    return nullptr;
  }
  if (hint > -1 && hint < cache.size() && cache[hint].first == inObj)
  {
    return cache[hint].second;
  }

  auto hasInObj = [inObj](const DataObjectPair& pair) { return pair.first == inObj; };
  const auto it = std::find_if(cache.begin(), cache.end(), hasInObj);
  return it == cache.end() ? nullptr : it->second;
}

//------------------------------------------------------------------------------
vtkDataObject* vtkDataObjectCache::FindObject(vtkCompositeDataIterator* inIter) const
{
  return FindObject(inIter->GetCurrentDataObject(), inIter->GetCurrentFlatIndex());
}

//------------------------------------------------------------------------------
vtkIdType vtkDataObjectCache::GetCacheSize() const
{
  return this->Internal->Cache.size();
}

//------------------------------------------------------------------------------
vtkIdType vtkDataObjectCache::GetNumberOfInvalidItems() const
{
  vtkIdType count = 0;
  const auto& cache = this->Internal->Cache;
  for (const auto& item : cache)
  {
    if (item.first == nullptr || item.second == nullptr ||
      item.first->GetMTime() > item.second->GetMTime())
    {
      ++count;
    }
  }
}
