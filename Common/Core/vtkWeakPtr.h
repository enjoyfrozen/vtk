/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWeakPtr.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * @class   vtkWeakPtr
 * @brief   a weak reference to a vtkObject.
 *
 * A weak reference to a vtkObject, which means that assigning
 * a vtkObject to the vtkWeakPtr does not affect the reference count of the
 * vtkObject.
 *
 * \code
 * vtkTable *table = vtkTable::New();
 * vtkWeakPtr<vtkTable> weakTable = table;
 * \endcode
 *
 * Some time later the table may be deleted, but if it is tested for null then
 * the weak pointer will not leave a dangling pointer.
 *
 * \code
 * table->Delete();
 * if (auto strongTable = weakTable.Lock())
 * {
 *   // Never executed as the weak table pointer will be null here
 *   cout << "Number of columns in table: " << strongTable->GetNumberOfColumns()
 *        << endl;
 * }
 * \endcode
 *
 * Note that, unlike vtkWeakPointer, the pointer itself is only accessible
 * after calling lock to avoid the situation of another thread deleting the
 * last instance between the check and its usage inside the conditional.
 */

#ifndef vtkWeakPtr_h
#define vtkWeakPtr_h

#include "vtkMeta.h"   // for IsComplete
#include "vtkObject.h" // for vtkObject

#include <memory>      // for std::shared_ptr
#include <mutex>       // for std::mutex
#include <type_traits> // for std::is_base_of

template <typename T>
class vtkWeakPtr
{
  // These static asserts only fire when the function calling CheckTypes is
  // used. Thus, this smart pointer class may still be used as a member variable
  // with a forward declared T, so long as T is defined by the time the calling
  // function is used.
  template <typename U = T>
  static void CheckTypes() noexcept
  {
    static_assert(vtk::detail::IsComplete<T>::value,
      "vtkWeakPtr<T>'s T type has not been defined. Missing include?");
    static_assert(vtk::detail::IsComplete<U>::value,
      "Cannot store an object with undefined type in vtkWeakPtr. Missing "
      "include?");
    static_assert(
      std::is_base_of<T, U>::value, "Argument type is not compatible with vtkWeakPtr<T>'s T type.");
    static_assert(std::is_base_of<vtkObject, T>::value,
      "vtkWeakPtr can only be used with subclasses of vtkObject.");
  }

public:
  ///@{
  /**
   * Default construction.
   */
  vtkWeakPtr() noexcept = default;
  ///@}

  ///@{
  /**
   * Pointer construction and assignment.
   */
  vtkWeakPtr(T* r)
    : Block(r ? r->GetWeakControlBlock() : nullptr)
  {
  }
  template <typename U>
  vtkWeakPtr(U* r)
    : Block(r ? r->GetWeakControlBlock() : nullptr)
  {
    vtkWeakPtr::CheckTypes<U>();
  }
  vtkWeakPtr& operator=(T* r)
  {
    this->Block = r ? r->GetWeakControlBlock() : nullptr;
    return *this;
  }
  template <typename U>
  vtkWeakPtr& operator=(U* r)
  {
    this->Block = r ? r->GetWeakControlBlock() : nullptr;
    vtkWeakPtr::CheckTypes<U>();
    return *this;
  }
  ///@}

  ///@{
  /**
   * Copy construction and assignment.
   */
  vtkWeakPtr(const vtkWeakPtr& r) = default;
  template <typename U>
  vtkWeakPtr(const vtkWeakPtr<U>& r)
    : Block(r.Block)
  {
    vtkWeakPtr::CheckTypes<U>();
  }
  vtkWeakPtr& operator=(const vtkWeakPtr& r) = default;
  template <typename U>
  vtkWeakPtr& operator=(const vtkWeakPtr<U>& r)
  {
    this->Block = r.Block;
    vtkWeakPtr::CheckTypes<U>();
    return *this;
  }
  ///@}

  ///@{
  /**
   * Move construction and assignment.
   */
  vtkWeakPtr(vtkWeakPtr&& r) noexcept = default;
  template <typename U>
  vtkWeakPtr(vtkWeakPtr<U>&& r) noexcept
    : Block(std::move(r.Block))
  {
    vtkWeakPtr::CheckTypes<U>();
  }
  vtkWeakPtr& operator=(vtkWeakPtr&& r) noexcept = default;
  template <typename U>
  vtkWeakPtr& operator=(vtkWeakPtr<U>&& r) noexcept
  {
    this->Block = std::move(r.Block);
    vtkWeakPtr::CheckTypes<U>();
    return *this;
  }
  ///@}

  ///@{
  ~vtkWeakPtr() noexcept = default;
  ///@}

  ///@{
  /**
   * Check whether the held object is valid or not.
   *
   * Note that this does not guarantee the object is valid at any future time
   * as the last reference can be dropped by another thread between an
   * `IsValid` check and a `Lock`.
   */
  bool IsValid() const { return this->Block && this->Block->Object; }
  ///@}

  ///@{
  /**
   * Obtain a new reference to the held object, if available.
   *
   * "It is better to ask for forgiveness than permission."
   */
  VTK_NEWINSTANCE
  T* Lock(vtkObject* owner = nullptr) const
  {
    if (this->Block)
    {
      // Ensure that while we're working on the block, another thread does not
      // come in and remove the last reference out from under us.
      std::lock_guard<std::mutex> guard(this->Block->Mutex);
      (void)guard;

      if (T* obj = static_cast<T*>(this->Block->Object))
      {
        // Add a reference.
        obj->Register(owner);
        return obj;
      }
    }

    return nullptr;
  }
  ///@}

  ///@{
  /**
   * Check whether the held object is valid or not.
   *
   * Note that this does not guarantee the object is valid at any future time
   * as the last reference can be dropped by another thread between any check
   * and a `Lock`.
   */
  operator bool() const { return this->IsValid(); }
  bool operator!() const { return !this->IsValid(); }
  ///@}

  ///@{
  bool operator==(const vtkWeakPtr& r) const { return this->Block == r.Block; }
  bool operator!=(const vtkWeakPtr& r) const { return this->Block != r.Block; }
  ///@}

private:
  std::shared_ptr<vtkObject::WeakControlBlock> Block;
};

#endif

// VTK-HeaderTest-Exclude: vtkWeakPtr.h
