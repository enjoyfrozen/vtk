#/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataCache.txx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * This is internal to vtkDataCache and should not be used publicly.
 */
#include "vtkCommand.h"      // for vtkCommand
#include "vtkSmartPointer.h" // for vtkSmartPointer
#include "vtkWeakPointer.h"  // for vtkSmartPointer
#include "vtkTuple.h"        // for vtkTuple

#include <map> // for std::multimap

class vtkDataCache;

namespace DataCacheNS
{
static size_t HashCombine(size_t one, size_t two)
{
  // from: https://stackoverflow.com/a/35991300/2640084
  /// seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
  return one ^ (two + 0x9e3779b9 + (one << 6) + (one >> 2));
}

template <typename T>
size_t ComputeHash(T const& item) noexcept
{
  return std::hash<T>{}(item);
}

template <>
size_t ComputeHash(vtkObject* const& item) noexcept
{
  return ComputeHash(item ? item->GetMTime() : 0);
}

template <typename T>
size_t ComputeHash(vtkSmartPointer<T> const& item) noexcept
{
  return ComputeHash(item.GetPointer());
}

template <typename T>
size_t ComputeHash(vtkWeakPointer<T> const& item) noexcept
{
  return ComputeHash(item.GetPointer());
}

template <typename T, int Size>
size_t ComputeHash(vtkTuple<T, Size> const& item) noexcept
{
  size_t hash = ComputeHash(item[0]);
  for (int cc=1; cc < Size; ++cc)
  {
    hash = HashCombine(hash, ComputeHash(item[cc]));
  }
  return hash;
}

template <typename T1, typename... Args>
size_t ComputeHash(T1 const& item, Args... args) noexcept
{
  return HashCombine(ComputeHash(item), ComputeHash(args...));
}

class VTKCOMMONCORE_EXPORT KeyT
{
public:
  template <typename... Args>
  KeyT(Args... args)
    : Hash(ComputeHash(args...))
  {
  }

  size_t GetHash() const { return this->Hash; }

  KeyT(const KeyT&) = default;
  KeyT& operator=(const KeyT&) = default;
  bool operator==(const KeyT& k) const noexcept { return this->GetHash() == k.GetHash(); }
  bool operator!=(const KeyT& k) const noexcept { return this->GetHash() != k.GetHash(); }

private:
  size_t Hash = 0;
};
} // namespace DataCacheNS

// custom specialization of std::hash can be injected in namespace std
namespace std
{
template <>
struct hash<DataCacheNS::KeyT>
{
  std::size_t operator()(DataCacheNS::KeyT const& key) const noexcept { return key.GetHash(); }
};
} // namespace std

namespace DataCacheNS
{
class VTKCOMMONCORE_EXPORT ObserverT
{
  std::multimap<vtkObject*, unsigned long> ObserverIds;
  vtkDataCache* DataCache;
  KeyT DataKey;
  KeyT ObserverKey;

public:
  template <typename... Args>
  ObserverT(vtkDataCache* cache, const KeyT& dataKey, const KeyT& observerKey, Args... args);
  ~ObserverT();
  void Cleanup();
  void Monitor(vtkObject* object);
};

template <typename T>
void Monitor(ObserverT* observer, T const&){};

template <>
void Monitor(ObserverT* observer, vtkObject* const& obj)
{
  if (obj && observer)
  {
    observer->Monitor(obj);
  }
};

template <typename T>
void Monitor(ObserverT* observer, vtkSmartPointer<T> const& obj)
{
  Monitor(observer, obj.GetPointer());
}

template <typename T>
void Monitor(ObserverT* observer, vtkWeakPointer<T> const& obj)
{
  Monitor(observer, obj.GetPointer());
}

template <typename T1, typename... Args>
void Monitor(ObserverT* observer, T1 const& t, Args... args)
{
  Monitor(observer, t);
  Monitor(observer, args...);
}

template <typename... Args>
ObserverT::ObserverT(
  vtkDataCache* cache, const KeyT& dataKey, const KeyT& observerKey, Args... args)
  : DataCache(cache)
  , DataKey(dataKey)
  , ObserverKey(observerKey)
{
  DataCacheNS::Monitor(this, args...);
}

} // namespace DataCacheNS
