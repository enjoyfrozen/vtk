/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWeakReference.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkWeakReference.h"
#include "vtkObjectFactory.h"

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkWeakReference);

//------------------------------------------------------------------------------
vtkWeakReference::vtkWeakReference() = default;

//------------------------------------------------------------------------------
vtkWeakReference::~vtkWeakReference() = default;

//------------------------------------------------------------------------------
void vtkWeakReference::Set(vtkObject* object)
{
  this->Object.Reset(object);
}

//------------------------------------------------------------------------------
vtkObject* vtkWeakReference::Get()
{
  auto obj = this->GetOwned();
  // Extract the pointer. The caller doesn't know if it owns this or not, so it
  // cannot be passed back with a new reference without leaking in existing
  // code.
  vtkObject* obj_ptr = obj;
  // XXX(thread-safety): This may not be valid after this function returns if
  // the chart is released on other threads. Previous code had problems with
  // this, so this is no worse than before.
  return obj_ptr;
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkObject> vtkWeakReference::GetOwned() const
{
  return this->Object.Lock();
}
