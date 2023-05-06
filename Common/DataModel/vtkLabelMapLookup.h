/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLabelMapLookup.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkLabelMapLookup
 * @brief   perform mapping from label values to label ids; evaluate
 *          whether a label belongs to a set of labels.
 *
 * Determine whether an image label/region has been selected for processing.
 * This requires looking up an image pixel/scalar value (i.e., the label
 * value) and determining whether it is a member of a set of labels.  Since
 * this can be relatively expensive when performed many times, different
 * lookup classes are used depending on the number of labels specified. A
 * cache is used for the common case of repeated queries for the same label
 * value.
 *
 * The class also performs mapping from a label value, to the ith label
 * (i.e., label id) used to define the label set (in the method
 * CreateLabelLookup).
 *
 * Note that, due to speed concerns, vtkLabelMapLookup does not inherit from
 * vtkObject hence does not support the usual VTK reference counting.
 *
 * @warning
 * vtkLabelMapLookup is not thread safe. This is due to the use of local
 * caching for quick label lookup.
 *
 * @sa
 * vtkSurfaceNets2D vtkSurfaceNets3D vtkDiscreteFlyingEdgesClipper2D
 */

#ifndef vtkLabelMapLookup_h
#define vtkLabelMapLookup_h

#include "vtkCommonDataModelModule.h"

#include <unordered_map>
#include <vector>

VTK_ABI_NAMESPACE_BEGIN

/**
 * The class is templated on the type of labels, and the type of label values
 * originally used to define the label map/set. (These types may vary because
 * a generic API to define the label set may be expressed in terms of generic
 * doubles or vtkIdTypes, but the actual labels may be in another type.)
 */
template <typename T, typename TSet>
class vtkLabelMapLookup
{
public:
  /**
   * Create the label map from an array (i.e., set) of label values. Cache
   * the first label value and label id.  This is invoked by derived classes.
   */
  vtkLabelMapLookup(const TSet* values, vtkIdType vtkNotUsed(numValues))
  {
    this->CachedValue = static_cast<T>(values[0]);
    this->CachedLabelId = 0;
    this->CachedOutValue = static_cast<T>(values[0]);
    this->CachedOutValueInitialized = false;
  }

  virtual ~vtkLabelMapLookup() = default;

  /**
   * The following methods are most commonly used.
   * Given a label, indicate whether it is in the set of labels.
   */
  virtual bool IsLabelValue(T label) = 0;

  /**
   * Given a label, indicate whether it is in the set of labels. Also return
   * the ith position in the label set used to define the label map.  If the
   * method returns false, then the returned labelId is arbitrary.
   */
  virtual bool IsLabelValue(T label, vtkIdType& labelId) = 0;

  /**
   * Factory methods for creating the right type of label map based
   * on the number of labels in the set.
   */
  static vtkLabelMapLookup<T,TSet>* CreateLabelLookup(const TSet* values, vtkIdType numLabels);

protected:
  T         CachedValue;    // the cached label value
  vtkIdType CachedLabelId;  // the ith label (i.e., label id) from the label set
  T         CachedOutValue; // a cache of a label value known to be outside the label set
  bool      CachedOutValueInitialized; // make sure out label value is initialized

  /**
   * Returns true if the query for the label value is in cache; otherwise
   * false and the cache needs to be updated.
   */
  bool IsLabelValueInCache(T label, bool& inLabelSet, vtkIdType& labelId)
  {
    if (label == this->CachedValue)
    {
      inLabelSet = true;
      labelId = this->CachedLabelId;
      return true;
    }
    else if (this->CachedOutValueInitialized && label == this->CachedOutValue)
    {
      inLabelSet = false;
      return true;
    }
    else
    {
      return false;
    }
  }

}; // vtkLabelMapLookup

// Cache a single contour value.
template <typename T, typename TSet>
class SingleLabelValue : public vtkLabelMapLookup<T,TSet>
{
public:
  SingleLabelValue(const TSet* values)
    : vtkLabelMapLookup<T,TSet>(values, 1) {}

  bool IsLabelValue(T label) override { return label == this->CachedValue; }

  bool IsLabelValue(T label, vtkIdType& labelId) override
  {
    if ( label == this->CachedValue )
    {
      labelId = this->CachedLabelId;
      return true;
    }
    return false;
  }

}; // SingleLabelValue

// Represent a small number contour values/labels with a std::vector<>. It is
// typically faster than an unordered_map for small label sets.
template <typename T, typename TSet>
class LabelVector : public vtkLabelMapLookup<T,TSet>
{
public:
  LabelVector(const TSet* values, vtkIdType numValues)
    : vtkLabelMapLookup<T,TSet>(values, numValues)
  {
    for (vtkIdType labelId = 0; labelId < numValues; labelId++)
    {
      Map.push_back(static_cast<T>(values[labelId]));
    }
  }

  bool IsLabelValue(T label, vtkIdType& labelId) override
  {
    bool inLabelSet;
    // Check the cache
    if (this->IsLabelValueInCache(label, inLabelSet, labelId))
    {
      return inLabelSet;
    }

    // Not in the cache, check the vector
    for ( labelId=0; labelId < this->Map.size(); ++labelId )
    {
      if ( label == this->Map[labelId] )
      {
        this->CachedValue = label;
        this->CachedLabelId = labelId;
        return true;
      }
    }

    // Otherwise, update the cache out label value
    this->CachedOutValue = label;
    this->CachedOutValueInitialized = true;
    return false;
  } // IsLabelValue

  bool IsLabelValue(T label) override
  {
    vtkIdType labelId;
    return LabelVector<T,TSet>::IsLabelValue(label,labelId);
  } // IsLabelValue

protected:
  std::vector<T> Map;

}; // LabelVector

// Represent many contour values/labels with a std::unordered_map<>
template <typename T, typename TSet>
class LabelMap : public vtkLabelMapLookup<T,TSet>
{
public:
  LabelMap(const TSet* values, vtkIdType numValues)
    : vtkLabelMapLookup<T,TSet>(values, numValues)
  {
    for (vtkIdType labelId = 0; labelId < numValues; labelId++)
    {
      this->Map[static_cast<T>(values[labelId])] = labelId;
    }
  }

  bool IsLabelValue(T label, vtkIdType& labelId) override
  {
    bool inLabelSet;
    // Check the cache
    if (this->IsLabelValueInCache(label, inLabelSet, labelId))
    {
      return inLabelSet;
    }

    // Not in cache, check the map
    auto iter = this->Map.find(label);
    if (iter != this->Map.end())
    {
      this->CachedValue = iter->first;
      this->CachedLabelId = iter->second;
      labelId = iter->second;
      return true;
    }
    else
    {
      this->CachedOutValue = label;
      this->CachedOutValueInitialized = true;
      return false;
    }
  } // IsLabelValue

  bool IsLabelValue(T label) override
  {
    vtkIdType labelId;
    return LabelMap<T,TSet>::IsLabelValue(label,labelId);
  } // IsLabelValue

protected:
  // Use a map because we also keep track of the label id.
  std::unordered_map<T,vtkIdType> Map;

}; // LabelMap

// Given a list of label values (represented generically by type TSet),
// create the appropriate lookup class and add the label values to
// the collection of labels.
template <typename T, typename TSet>
vtkLabelMapLookup<T,TSet>* vtkLabelMapLookup<T,TSet>::CreateLabelLookup(
  const TSet* values, vtkIdType numLabels)
{
  // These cutoffs are empirical and can be changed.
  if (numLabels == 1)
  {
    return new SingleLabelValue<T,TSet>(values);
  }
  else if (numLabels < 20)
  {
    return new LabelVector<T,TSet>(values, numLabels);
  }
  else
  {
    return new LabelMap<T,TSet>(values, numLabels);
  }
}

VTK_ABI_NAMESPACE_END
#endif
// VTK-HeaderTest-Exclude: vtkLabelMapLookup.h
