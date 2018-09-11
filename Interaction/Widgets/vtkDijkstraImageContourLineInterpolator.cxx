/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkDijkstraImageContourLineInterpolator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkDijkstraImageContourLineInterpolator.h"

#include "vtkCellArray.h"
#include "vtkCellLocator.h"
#include "vtkContourRepresentation.h"
#include "vtkDijkstraImageGeodesicPath.h"
#include "vtkImageActor.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkMatrix4x4.h"

vtkStandardNewMacro(vtkDijkstraImageContourLineInterpolator);

//----------------------------------------------------------------------
vtkDijkstraImageContourLineInterpolator
::vtkDijkstraImageContourLineInterpolator()
{
  this->DijkstraImageGeodesicPath = vtkDijkstraImageGeodesicPath::New();
  this->CostImage = nullptr;
  this->CostImageToObliquePlaneMatrix = nullptr;
  this->ObliquePlaneToCostImageMatrix = nullptr;
}

//----------------------------------------------------------------------
vtkDijkstraImageContourLineInterpolator
::~vtkDijkstraImageContourLineInterpolator()
{
  this->DijkstraImageGeodesicPath->Delete();
  this->CostImage = nullptr;
  if ( this->CostImageToObliquePlaneMatrix )
  {
    this->CostImageToObliquePlaneMatrix->Delete();
  }
  if ( this->ObliquePlaneToCostImageMatrix )
  {
    this->ObliquePlaneToCostImageMatrix->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkDijkstraImageContourLineInterpolator::SetCostImage( vtkImageData *arg )
{
  if ( this->CostImage == arg )
  {
    return;
  }

  this->CostImage = arg;
  if ( this->CostImage )
  {
    this->DijkstraImageGeodesicPath->SetInputData( this->CostImage );
  }
}

//----------------------------------------------------------------------------
// The CostImage that is set for the contour line interpolator is a
// vtkImageData object. Those objects do not contain specific information
// about where they live in space. If for instance, an oblique slice is
// intended, we need to have a transformation that maps coordinates from
// the CostImage to the intended oblique plane/slice. The point placer
// will return world coordinates that are located in the oblique plane, so
// we also need the inverse of the transformation to have a mapping between
// points selected by the point placer and the CostImage (and vice versa).
void vtkDijkstraImageContourLineInterpolator::SetCostImageToObliqueMatrix( vtkMatrix4x4 *arg )
{
  if ( this->CostImageToObliquePlaneMatrix )
    {
    this->CostImageToObliquePlaneMatrix->Delete();
    }
  this->CostImageToObliquePlaneMatrix = vtkMatrix4x4::New();
  this->CostImageToObliquePlaneMatrix->DeepCopy(arg);

  // We also need the inverse of the transformation in order to map
  // points from the point placer onto the CostImage.
  if ( this->ObliquePlaneToCostImageMatrix )
    {
    // We already added a matrix before, delete the object
    // we had so far, in order to make a new one
    this->ObliquePlaneToCostImageMatrix->Delete();
    }
  this->ObliquePlaneToCostImageMatrix = vtkMatrix4x4::New();
  this->ObliquePlaneToCostImageMatrix->DeepCopy(this->CostImageToObliquePlaneMatrix);
  this->ObliquePlaneToCostImageMatrix->Invert();
}


//----------------------------------------------------------------------
int vtkDijkstraImageContourLineInterpolator::InterpolateLine(
  vtkRenderer* vtkNotUsed(ren), vtkContourRepresentation *rep,
  int idx1, int idx2 )
{
  // if the user didn't set the image, try to get it from the actor
  if ( !this->CostImage )
  {

    vtkImageActorPointPlacer *placer =
      vtkImageActorPointPlacer::SafeDownCast(rep->GetPointPlacer());

    if ( !placer )
    {
      return 1;
    }

    vtkImageActor* actor = placer->GetImageActor();
    if ( !actor || !(this->CostImage = actor->GetInput()) )
    {
      return 1;
    }
    this->DijkstraImageGeodesicPath->SetInputData( this->CostImage );
  }

  double p1[3], p2[3];
  rep->GetNthNodeWorldPosition( idx1, p1 );
  rep->GetNthNodeWorldPosition( idx2, p2 );

  if ( this->ObliquePlaneToCostImageMatrix )
    {
    // A transform has been added that maps the CostImage to some
    // oblique slice. In this case we need to convert the world
    // coordinates we just retrieved (from the oblique slice) to
    // the coordinates in the CostImage
    double oblique_p1[4] = {p1[0], p1[1], p1[2], 1.0};
    double *cost_image_p1 = this->ObliquePlaneToCostImageMatrix->MultiplyDoublePoint(oblique_p1);
    p1[0] = cost_image_p1[0];
    p1[1] = cost_image_p1[1];
    p1[2] = cost_image_p1[2];
    double oblique_p2[4] = {p2[0], p2[1], p2[2], 1.0};
    double *cost_image_p2 = this->ObliquePlaneToCostImageMatrix->MultiplyDoublePoint(oblique_p2);
    p2[0] = cost_image_p2[0];
    p2[1] = cost_image_p2[1];
    p2[2] = cost_image_p2[2];
    }

  vtkIdType beginVertId = this->CostImage->FindPoint( p1 );
  vtkIdType endVertId = this->CostImage->FindPoint( p2 );

  // Could not find the starting and ending cells. We can't interpolate.
  if ( beginVertId == -1 || endVertId == -1 )
  {
    return 0;
  }

  int nnodes = rep->GetNumberOfNodes();

  if ( this->DijkstraImageGeodesicPath->GetRepelPathFromVertices() && nnodes > 2 )
  {
    vtkPoints* verts = vtkPoints::New();
    double pt[3];
    for( int i = 0; i < nnodes; ++i )
    {
      if( i == idx1 ) continue;

      for( int j = 0; j < rep->GetNumberOfIntermediatePoints( i ); ++j )
      {
          rep->GetIntermediatePointWorldPosition( i, j, pt );
          verts->InsertNextPoint( pt );
      }
    }
    this->DijkstraImageGeodesicPath->SetRepelVertices( verts );
    verts->Delete();
  }
  else
  {
    this->DijkstraImageGeodesicPath->SetRepelVertices( nullptr );
  }

  this->DijkstraImageGeodesicPath->SetStartVertex( endVertId );
  this->DijkstraImageGeodesicPath->SetEndVertex( beginVertId );
  this->DijkstraImageGeodesicPath->Update();

  vtkPolyData *pd = this->DijkstraImageGeodesicPath->GetOutput();

  vtkIdType npts = 0, *pts = nullptr;
  pd->GetLines()->InitTraversal();
  pd->GetLines()->GetNextCell( npts, pts );

  for ( int i = 0; i < npts; ++i )
  {

    if ( this->CostImageToObliquePlaneMatrix )
        {
        // A transform has been added that maps the CostImage to some
        // oblique slice. In this case we need to convert the world
        // coordinates we got positioned on the cost image to
        // the oblique coordinates that the point placer uses.
        double *point_on_cost_image = pd->GetPoint( pts[i] );
        double full_point_on_cost_image[4] = {point_on_cost_image[0],
                                              point_on_cost_image[1],
                                              point_on_cost_image[2],
                                              1.0};
        double *point_to_oblique = this->CostImageToObliquePlaneMatrix->MultiplyDoublePoint(full_point_on_cost_image);
        double point_to_oblique_xyz[3] = {point_to_oblique[0],
                                          point_to_oblique[1],
                                          point_to_oblique[2]};

        rep->AddIntermediatePointWorldPosition( idx1, point_to_oblique_xyz );
        }
    else
        {
        rep->AddIntermediatePointWorldPosition( idx1, pd->GetPoint( pts[i] )  );
        }
  }

  return 1;
}

//----------------------------------------------------------------------
void vtkDijkstraImageContourLineInterpolator::PrintSelf(
                              ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "DijkstraImageGeodesicPath: " << this->DijkstraImageGeodesicPath << endl;
  os << indent << "CostImage: " << this->GetCostImage() << endl;
  if ( this->CostImageToObliquePlaneMatrix )
    {
    double axes[16];
    vtkMatrix4x4::DeepCopy(axes, this->CostImageToObliquePlaneMatrix);
    os << indent << "CostImage to ObliquePlane transform:";
    for (int k = 0; k < 16; k++)
      {
      os << ((k % 4 == 0) ? "\n\t" : "") << axes[k]
         << (k != 15 ? ", " : "\n");
      }
    }
  if ( this->ObliquePlaneToCostImageMatrix )
    {
    double axes[16];
    vtkMatrix4x4::DeepCopy(axes, this->ObliquePlaneToCostImageMatrix);
    os << indent << "ObliquePlane to CostImage transform:";
    for (int k = 0; k < 16; k++)
      {
      os << ((k % 4 == 0) ? "\n\t" : "") << axes[k]
         << (k != 15 ? ", " : "\n");
      }
    }
}
