/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataCache.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkDataCache
 * @brief
 *
 */

#ifndef vtkDataCache_h
#define vtkDataCache_h

#include "vtkDataCache.txx" // for DataCacheNS
#include "vtkObject.h"
#include <memory> // for std::unique_ptr

class VTKCOMMONCORE_EXPORT vtkDataCache : public vtkObject
{
public:
  vtkTypeMacro(vtkDataCache, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Provides access to the singleton vtkDataCache.
   */
  static vtkDataCache* GetInstance();

  template <typename T, typename... KeyT>
  vtkSmartPointer<T> GetCachedData(KeyT... key) const
  {
    DataCacheNS::KeyT keyPack(key...);
    return T::SafeDownCast(this->GetCachedDataAsObject(keyPack));
  }

  template <typename... KeyT>
  void AddToCache(vtkObject* context, vtkObjectBase* data, KeyT... key)
  {
    DataCacheNS::KeyT dataKey(key...);
    DataCacheNS::KeyT observerKey(context, key...);
    auto observer =
      std::make_shared<DataCacheNS::ObserverT>(this, dataKey, observerKey, context, key...);
    this->AddToCacheInternal(context, data, dataKey, observerKey, observer);
  }

  size_t GetNumberOfItems() const;

protected:
  vtkDataCache();
  ~vtkDataCache() override;

private:
  vtkDataCache(const vtkDataCache&) = delete;
  void operator=(const vtkDataCache&) = delete;
  static vtkDataCache* New();

  class vtkInternals;
  std::unique_ptr<vtkInternals> Internals;

  vtkSmartPointer<vtkObjectBase> GetCachedDataAsObject(const DataCacheNS::KeyT& key) const;
  void AddToCacheInternal(vtkObjectBase* context, vtkObjectBase* data,
    const DataCacheNS::KeyT& dataKey, const DataCacheNS::KeyT& observerKey,
    std::shared_ptr<DataCacheNS::ObserverT>& observer);
  void Remove(const DataCacheNS::KeyT& dataKey, const DataCacheNS::KeyT& observerKey);

  friend class DataCacheNS::ObserverT;
};

#endif
