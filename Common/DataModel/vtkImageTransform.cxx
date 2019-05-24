/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageTransform.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImageTransform.h"

#include "vtkImageData.h"
#include "vtkDataArray.h"
#include "vtkPointSet.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkSMPTools.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkImageTransform);

//============================================================================
//----------------------------------------------------------------------------
// Functors to support threaded execution
namespace { //anonymous

  template <typename T>
  struct InPlaceTransformPoints
  {
    T *Points;

    InPlaceTransformPoints(double (&m)[3][3], T *pts) : Points(pts) {}

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      T *pIn = this->Points + 3*ptId;
      T *pOut = pIn;

      for ( ; ptId < endPtId; ++ptId )
      {
        // TODO: transform point, inplace with m
        *pOut++ = *pIn++;
        *pOut++ = *pIn++;
        *pOut++ = *pIn++;
      }
    }

    // Interface to vtkSMPTools
    static void Execute(double (&m)[3][3], vtkIdType num, T *pts)
    {
      InPlaceTransformPoints<T> transform(m,pts);
      vtkSMPTools::For(0,num, transform);
    }
  };//InPlaceTransformPoints

  template <typename T>
  struct InPlaceTransformNormals
  {
    T *Normals;

    InPlaceTransformNormals(double (&m)[3][3], T *n) : Normals(n) {}

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      T *nIn = this->Normals + 3*ptId;
      T *nOut = nIn;

      for ( ; ptId < endPtId; ++ptId )
      {
        // TODO: transform point, inplace with m
        *nOut++ = *nIn++;
        *nOut++ = *nIn++;
        *nOut++ = *nIn++;
      }
    }

    // Interface to vtkSMPTools
    static void Execute(double (&m)[3][3], vtkIdType num, T *n)
    {
      InPlaceTransformNormals<T> transform(m,n);
      vtkSMPTools::For(0,num, transform);
    }
  };//InPlaceTransformNormals

  template <typename T>
  struct InPlaceTransformVectors
  {
    T *Vectors;

    InPlaceTransformVectors(double (&m)[3][3], T *n) : Vectors(n) {}

    void operator()(vtkIdType ptId, vtkIdType endPtId)
    {
      T *vIn = this->Vectors + 3*ptId;
      T *vOut = vIn;

      for ( ; ptId < endPtId; ++ptId )
      {
        // TODO: transform point, inplace with m
        *vOut++ = *vIn++;
        *vOut++ = *vIn++;
        *vOut++ = *vIn++;
      }
    }

    // Interface to vtkSMPTools
    static void Execute(double (&m)[3][3], vtkIdType num, T *v)
    {
      InPlaceTransformVectors<T> transform(m,v);
      vtkSMPTools::For(0,num, transform);
    }
  };//InPlaceTransformVectors

}//anonymous namespace



//============================================================================
//----------------------------------------------------------------------------
// Here is the VTK class proper.
vtkImageTransform::vtkImageTransform()
{
}

//----------------------------------------------------------------------------
void vtkImageTransform::TransformPointSet(vtkImageData *im, vtkPointSet *ps)
{
  vtkIdType numPts, numCells;

  // Check input
  if ( im == nullptr || ps == nullptr )
  {
    return;
  }
  // if ( image is axis aligned )
  // {
  //   return;
  // }

  // Make sure points are available
  if ( numPts=ps->GetNumberOfPoints() < 1 )
  {
    return;
  }

  // At this point, transformaion is required.
  // TODO: Do what it take to create transformation matrix from the
  // image orientation/direction provided.
  double m[3][3];

  // Grab the points-related-data and process as appropriate
  vtkDataArray *pts = ps->GetPoints()->GetData();
  vtkImageTransform::TransformPoints(m,pts);

  vtkDataArray *normals = ps->GetPointData()->GetNormals();
  if ( normals != nullptr )
  {
    vtkImageTransform::TransformNormals(m,normals);
  }

  vtkDataArray *vectors = ps->GetPointData()->GetVectors();
  if ( vectors != nullptr )
  {
    vtkImageTransform::TransformVectors(m,vectors);
  }

  // Grab the cells-related-data and process as appropriate
  normals = ps->GetCellData()->GetNormals();
  if ( normals != nullptr )
  {
    vtkImageTransform::TransformNormals(m,normals);
  }

  vectors = ps->GetCellData()->GetVectors();
  if ( vectors != nullptr )
  {
    vtkImageTransform::TransformVectors(m,vectors);
  }
}

//----------------------------------------------------------------------------
void vtkImageTransform::TransformPoints(double (&m)[3][3], vtkDataArray *da)
{
  void *pts = da->GetVoidPointer(0);
  vtkIdType num = da->GetNumberOfTuples();

  switch ( da->GetDataType() )
  {
    vtkTemplateMacro(InPlaceTransformPoints<VTK_TT>::
                     Execute(m,num,(VTK_TT*)pts));
  }
}

//----------------------------------------------------------------------------
void vtkImageTransform::TransformNormals(double (&m)[3][3], vtkDataArray *da)
{
  void *n = da->GetVoidPointer(0);
  vtkIdType num = da->GetNumberOfTuples();

  switch ( da->GetDataType() )
  {
    vtkTemplateMacro(InPlaceTransformNormals<VTK_TT>::
                     Execute(m,num,(VTK_TT*)n));
  }
}

//----------------------------------------------------------------------------
void vtkImageTransform::TransformVectors(double (&m)[3][3], vtkDataArray *da)
{
  void *v = da->GetVoidPointer(0);
  vtkIdType num = da->GetNumberOfTuples();

  switch ( da->GetDataType() )
  {
    vtkTemplateMacro(InPlaceTransformVectors<VTK_TT>::
                     Execute(m,num,(VTK_TT*)v));
  }
}

//----------------------------------------------------------------------------
void vtkImageTransform::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
