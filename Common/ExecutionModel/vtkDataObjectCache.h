/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDataObjectCache.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkDataObjectCache
 * @brief   Cache for data objects in composite data algorithm execution
 *
 * vtkDataObjectCache stores pairs of vtkDataObject instances from the input and
 * output of filters, so that repeated calls may reuse output instances if the
 * input instance and the algorithm are unchanged.
 */

#ifndef vtkDataObjectCache_h
#define vtkDataObjectCache_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkObject.h"

class vtkCompositeDataIterator;
class vtkCompositeDataSet;
class vtkDataObject;
class vtkDataObjectCacheInternals;

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkDataObjectCache : public vtkObject
{
public:
  static vtkDataObjectCache* New();
  vtkTypeMacro(vtkDataObjectCache, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Release the allocated memory in the cache
   *
   * Returns whether the cache was modified.
   */
  bool Release();

  /**
   * Clear the cache without releasing allocated memory.
   *
   * Returns whether the cache was modified.
   */
  bool Clear();

  /**
   * Update the cache based on the specified composite input data.
   *
   * Assures that the cache only contains items for leaf objects of specified
   * composite data set, keeping the output leaf object only if it is newer than
   * the input leaf object.
   *
   * Returns whether the cache was modified.
   */
  bool Update(vtkCompositeDataSet* inComposite);

  /**
   * Update the cache based on the specified composite input and output data.
   *
   * Assumes that Update was called with the same input data, i.e. the cache
   * contains up-to-date items only. New output leaf objects are inserted.
   *
   * Returns whether the cache was modified.
   */
  bool Finalize(vtkCompositeDataSet* inComposite, vtkCompositeDataSet* outComposite);

  /**
   * Returns whether an input object exists in the cache.
   */
  bool Contains(vtkDataObject* inObj, vtkIdType hint = -1) const;

  /**
   * Find the output leaf object associated with the specified input leaf
   * object.
   *
   * An index hint may be provided for lookup. If the input object is not found
   * at that index the entire cache is searched.
   *
   * Returns the output leaf object or nullptr
   */
  vtkDataObject* FindObject(vtkDataObject* inObj, vtkIdType hint = -1) const;

  /**
   * Find the output leaf object associated with object pointed at by the
   * iterator.
   *
   * Returns the output leaf object or nullptr
   */
  vtkDataObject* FindObject(vtkCompositeDataIterator* inIter) const;

  /**
   * Return the size of the cache
   */
  vtkIdType GetCacheSize() const;

  /**
   * Return the number of invalid items in the cache
   *
   * An item is invalid if one of the leaf objects is a nullptr or if the output
   * leaf object is older than the input leaf object.
   */
  vtkIdType GetNumberOfInvalidItems() const;

protected:
  vtkDataObjectCache();
  ~vtkDataObjectCache() override;

private:
  vtkDataObjectCacheInternals* Internal = nullptr;

private:
  vtkDataObjectCache(const vtkDataObjectCache&) = delete;
  void operator=(const vtkDataObjectCache&) = delete;
};

#endif
