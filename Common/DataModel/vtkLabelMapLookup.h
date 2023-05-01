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
 * @brief   provide an efficient numeric label lookup
 *
 *
 * vtkLabelMapLookup is a light weight helper object that enables faster
 * lookup of a segmentation label from a set of labels. It uses caching, and
 * different strategies depending on the size of the set of labels. The basic
 * operation supported is: given a label value, determine whether it is
 * part of a label set. This is important when processing subsets of labels.
 *
 * Note that, due to speed concerns, vtkLabelMapLookup does not inherit from
 * vtkObject hence does not support the usual VTK reference counting.
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
// Determine whether an image label/region has been specified for output.
// This requires looking up an image pixel/scalar value (i.e., the label
// value) and determining whether it is a member of a set of labels.  Since
// this can be relatively expensive when performed many times, different
// lookup classes are used depending on the number of labels specified. A
// cache is used for the common case of repeated queries for the same label
// value.
template <typename T>
struct vtkLabelMapLookup
{
  T         CachedValue;    // the cached label value
  vtkIdType CachedLabelId;  // the ith label (i.e., label id) from the label set
  T         CachedOutValue; // a cache of a label value known to be outside the label set
  bool      CachedOutValueInitialized; // make sure out label value is initialized

  // Create the label map from an array (i.e., set) of label values. Cache
  // the first label value and label id.  This is invoked by derived classes.
  vtkLabelMapLookup(const double* values, vtkIdType vtkNotUsed(numValues))
  {
    this->CachedValue = static_cast<T>(values[0]);
    this->CachedLabelId = 0;
    this->CachedOutValue = static_cast<T>(values[0]);
    this->CachedOutValueInitialized = false;
  }
  virtual ~vtkLabelMapLookup() = default;

  // Returns true if the query for the label value is in cache; otherwise
  // false and the cache needs to be updated.
  bool IsLabelValueInCache(T label, bool& inLabelSet)
  {
    if (label == this->CachedValue)
    {
      inLabelSet = true;
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

  // The following methods are most commonly used.
  // Given a label, indicate whether it is in the set of labels.
  virtual bool IsLabelValue(T label) = 0;

  // Given a label, indicate whether it is in the set of labels. Also return
  // the ith position in the label set used to define the label map.  If the
  // method returns false, then the returned labelId is arbitrary.
  virtual bool IsLabelValue(T label, vtkIdType& labelId) = 0;

  // A factory method for creating the right type of label map based
  // on the number of labels in the set.
  static vtkLabelMapLookup<T>* CreateLabelLookup(const double* values, vtkIdType numLabels);
}; // vtkLabelMapLookup

// Cache a single contour value.
template <typename T>
struct SingleLabelValue : public vtkLabelMapLookup<T>
{
  SingleLabelValue(const double* values)
    : vtkLabelMapLookup<T>(values, 1)
  {
  }
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
template <typename T>
struct LabelVector : public vtkLabelMapLookup<T>
{
  std::vector<T> Map;

  LabelVector(const double* values, vtkIdType numValues)
    : vtkLabelMapLookup<T>(values, numValues)
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
    if (this->IsLabelValueInCache(label, inLabelSet))
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
    return LabelVector<T>::IsLabelValue(label,labelId);
  } // IsLabelValue

}; // LabelVector

// Represent many contour values/labels with a std::unordered_map<>
template <typename T>
struct LabelMap : public vtkLabelMapLookup<T>
{
  // Use a map because we also keep track of the label id.
  std::unordered_map<T,vtkIdType> Map;

  LabelMap(const double* values, vtkIdType numValues)
    : vtkLabelMapLookup<T>(values, numValues)
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
    if (this->IsLabelValueInCache(label, inLabelSet))
    {
      return inLabelSet;
    }

    // Not in cache, check the map
    auto iter = this->Map.find(label);
    if (iter != this->Map.end())
    {
      this->CachedValue = iter->first;
      this->CachedLabelId = iter->second;
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
    return LabelMap<T>::IsLabelValue(label,labelId);
  } // IsLabelValue

}; // LabelMap

// Given a list of label values (represented generically as doubles),
// create the appropriate lookup class and add the label values to
// the collection of labels.
template <typename T>
vtkLabelMapLookup<T>* vtkLabelMapLookup<T>::CreateLabelLookup(
  const double* values, vtkIdType numLabels)
{
  // These cutoffs are empirical and can be changed.
  if (numLabels == 1)
  {
    return new SingleLabelValue<T>(values);
  }
  else if (numLabels < 20)
  {
    return new LabelVector<T>(values, numLabels);
  }
  else
  {
    return new LabelMap<T>(values, numLabels);
  }
}

VTK_ABI_NAMESPACE_END
#endif
// VTK-HeaderTest-Exclude: vtkLabelMapLookup.h
