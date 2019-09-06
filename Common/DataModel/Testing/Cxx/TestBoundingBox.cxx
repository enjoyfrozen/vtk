/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkMath.h"
#include "vtkBoundingBox.h"
#include "vtkSmartPointer.h"

#include <limits>

#define TestBoundingBoxFailMacro(b,msg) if(!(b)){std::cerr <<msg<<std::endl;return EXIT_FAILURE;}

int TestBoundingBox(int,char *[])
{
  {
    double n[3]={-1,0.5,0};
    double p[3]={-1,-1,-1};
    double bb[6] = {-1,1,-1,1,-1,1};
    vtkBoundingBox bbox(bb);
    bool res = bbox.IntersectPlane(p,n);
    bbox.GetBounds(bb);
    TestBoundingBoxFailMacro(res && bb[0]==-1 && bb[1]==0,"Intersect Plane Failed!")
  }
  {
    double n[3]={0,0,1};
    double p[3]={0,0,0};
    double bb[6] = {-1,1,-1,1,-1,1};
    vtkBoundingBox bbox(bb);
    bool res = bbox.IntersectPlane(p,n);
    bbox.GetBounds(bb);
    TestBoundingBoxFailMacro(res && bb[4]==0 && bb[5]==1,"Intersect Plane Failed!")
  }
  {
    double n[3]={0,0,-1};
    double p[3]={0,0,0};
    double bb[6] = {-1,1,-1,1,-1,1};
    vtkBoundingBox bbox(bb);
    bool res = bbox.IntersectPlane(p,n);
    bbox.GetBounds(bb);
    TestBoundingBoxFailMacro(res && bb[4]==-1 && bb[5]==0,"Intersect Plane Failed!")
  }
  {
    double n[3]={0,-1,0};
    double p[3]={0,0,0};
    double bb[6] = {-1,1,-1,1,-1,1};
    vtkBoundingBox bbox(bb);
    bool res = bbox.IntersectPlane(p,n);
    bbox.GetBounds(bb);
    TestBoundingBoxFailMacro(res && bb[2]==-1 && bb[3]==0,"Intersect Plane Failed!")
  }

  {
    double n[3]={1,1,1};
    double p[3]={0,0,0};
    double bb[6] = {-1,1,-1,1,-1,1};
    vtkBoundingBox bbox(bb);
    bool res = bbox.IntersectPlane(p,n);
    bbox.GetBounds(bb);
    TestBoundingBoxFailMacro( !res
      && bb[0] ==-1 && bb[1]==1
      && bb[2] ==-1 && bb[3]==1
      && bb[4] ==-1 && bb[5] ==1,"Intersect Plane Failed!")
  }
  {
    double bb[6];
    vtkBoundingBox invalidBBox;
    invalidBBox.GetBounds(bb);
    vtkBoundingBox bbox(bb);
    TestBoundingBoxFailMacro(!bbox.IsValid(), "Bounding box from invalid bounds Failed!");
  }
  return EXIT_SUCCESS;
}
