/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkVoronoi3D.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkVoronoi3D.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDoubleArray.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkSMPTools.h"
#include "vtkSpheres.h"
#include "vtkStaticPointLocator.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkUnstructuredGrid.h"

#include <vector>

vtkStandardNewMacro(vtkVoronoi3D);

//------------------------------------------------------------------------------
// Internal data structures and methods to generate 3D Voronoi
// tessellations. The filter operates by performing polyhedral clipping by
// processing only neighborhood points falling within a distance measure (the
// Voronoi flower).  The clipping operation is done independently in each
// thread, and then composited together to produce the filter output.
//
// Implementation notes: std::vector is used instead of std::list due to
// performance reasons (to minimize costly new/delete). What's happening is
// that repeated plane clips are applied to an initial polyhedron. It is
// believed that the use of std::vector is acceptable because the number of
// points/faces composing a Voronoi polyhedron is usually on the order of <
// 100 - i.e., relatively small numbers. It would not be too difficult to
// change out vector with a std::list if desired.  The kernel methods on
// vtkPolyhedron below are implemented in serial since the amount of data
// processed is small. However, the Voronoi3D algorithm is threaded by
// processing multiple Voronoi kernels simultaneously, and then composing the
// results into the filter output.
namespace
{
  // Voronoi kernel methods follow =============================================

  // Because vector are being used as a core data structure, and entities in
  // the tessellation are dynamically created and destroyed, rather than pay
  // the price of creating and deleting memory, we simply mark the
  // topological entities in the appropriate list as being an active part of
  // the tessellation (Alive) or not (Dead).
  enum EntityStatus
  {
    Dead = 0,
    Alive = 1
  };

  // Indicate whether a face is inside, outside, or cut by the current
  // current clip plane.
  enum FaceClassification
  {
    Outside = 0,
    Intersects = 1,
    Inside = 2
  };

  // Represent the geometric position of a point, as well as its evaluation
  // against the current clip plane. The status indicates whether the
  // point is still in use in the tessellation. The Id is used later on when
  // the polyhedron is appended to the output.
  struct PolyPoint
  {
    double X[3];  // Vertex position in 3-space
    double Value; // Evaluation against the current clip plane
    int Status;   // Is the point used to define the current polyhedron?
    vtkIdType Id; // This is an assigned id while compositing points

    PolyPoint(double x[3]) : X{x[0],x[1],x[2]}, Value(0.0), Status(Alive), Id(-1) {}
    PolyPoint(double x, double y, double z) : X{x,y,z}, Value(0.0), Status(Alive), Id(-1) {}
  };
  using PPoints = std::vector<PolyPoint>; // list of points

  // List of point ids. The ids index into the polyhedron point list.
  // Special methods are available for circumferentially sorting.
  struct PolyVerts : public std::vector<vtkIdType>
  {
    double *XC;
    int Axis0;
    int Axis1;
    PPoints *Points;
    void Sort(double *xc, int axis0, int axis1, PPoints *pts);
  }; //PolyVerts
  using PVerts = PolyVerts;

  // A comparison function for circumferential sorting. We need to access
  // member data in the sort process which this functor provides. Sorting is
  // counterclockwise starting at the 3 o'clock position (i.e., a point on the +x
  // axis is zero degrees).  This custom functor is used to avoid trig
  // functions.
  struct CompareLess
  {
    const PolyVerts &Verts;
    CompareLess(const PolyVerts &pv) : Verts(pv) {}
    bool operator()(vtkIdType ai, vtkIdType bi)
    {
      PolyPoint& a = (*this->Verts.Points)[ai];
      PolyPoint& b = (*this->Verts.Points)[bi];
      const double *center = this->Verts.XC;
      int axis0 = this->Verts.Axis0;
      int axis1 = this->Verts.Axis1;

      // First perform half-space comparisons to order points. Note that
      // certain comparisons around ==0 are ignored because of convexity
      // properties of Voronoi faces.
      if ( ((a.X[axis1]-center[axis1]) >= 0) && ((b.X[axis1]-center[axis1]) < 0) )
      {
        return true;
      }
      if ( ((a.X[axis1]-center[axis1]) < 0) && ((b.X[axis1]-center[axis1]) >= 0) )
      {
        return false;
      }

      // If here, use the cross product to determine which side of the edge
      // (a,center) point b is on.
      double cp = ((a.X[axis0]-center[axis0])*(b.X[axis1]-center[axis1])) -
        ((b.X[axis0]-center[axis0])*(a.X[axis1]-center[axis1]));

      return ( cp < 0 ? true : false );
    }
  };

  void PolyVerts::Sort(double *xc, int axis0, int axis1, PPoints *pts)
  {
    this->XC = xc;
    this->Axis0 = axis0;
    this->Axis1 = axis1;
    this->Points = pts;

    std::sort(this->begin(),this->end(), CompareLess(*this));
  }

  // Create an edge table that is used during face clipping to represent
  // clipped edges.  The table does not represent all of the edges in the
  // polyhedron, it's used during edge clipping and typically holds a small
  // number of clipped edges.
  struct PolyEdge
  {
    vtkIdType V0; // the points (v0,v1) form an edge
    vtkIdType V1;
    vtkIdType PtId; // id is the intersection point id associated with the edge.

    PolyEdge(vtkIdType v0, vtkIdType v1, vtkIdType id) : V0(v0), V1(v1), PtId(id) {}
  }; //PolyEdge

  struct PolyEdges : public std::vector<PolyEdge>
  {
    vtkIdType IsEdge(vtkIdType v0, vtkIdType v1)
    {
      for ( auto e=0; e < this->size(); ++e )
      {
        PolyEdge& edge = (*this)[e];
        if ( (edge.V0 == v0 && edge.V1 == v1) || (edge.V0 == v1 && edge.V1 == v0) )
        {
          return edge.PtId;
        }
      }
      return -1;
    }
    void InsertEdge(const PolyEdge& pe)
    {
      this->emplace_back(pe);
    }
  }; //PolyEdges
  using PEdges = PolyEdges;

  // Represent a face of the polyhedron. The notion of the "spoke" is important here.
  // The spokes of a Voronoi cell are the edges that connect the generating point id
  // with the neighboring points, each spoke produces a Voronoi tile face. These spokes
  // are the dual Delaunay edges. Note that id a SpokeId=(-1), then it connects to
  // the boundary of the Voronoi computational space, i.e., the Voronoi tile is unbounded.
  struct PolyFace
  {
    PVerts FaceVerts;   // The points defining the face
    int Status;         // Is this face used to define the current polyhedron?
    vtkIdType SpokeId;  // The neighboring point id that generated this face (the spoke)

    PolyFace(vtkIdType numPts, const vtkIdType *pts, vtkIdType spokeId) :
      Status(Alive), SpokeId(spokeId)
    {
      for ( auto i=0; i < numPts; ++i )
      {
        this->FaceVerts.push_back(pts[i]);
      }
    }
  };
  using PFaces = std::vector<PolyFace>;

  struct Polyhedron
  {
    // Core data representation for the polyhedron
    vtkIdType PtId; // The defining Voronoi point id
    PPoints Points; // The vertices which define the polyhedron
    PFaces Faces;   // The faces which define the polyhedron

    // Internal structures for computation.
    // Collect interior or edge intersection verts to form a new face.
    // Each non-empty clip will create one new face, and modify other faces.
    PVerts NewFaceIds;
    PEdges EdgeTable;

    // Instantiate and configure internal computational structures.
    Polyhedron()
    {
      this->Points.reserve(256);
      this->Faces.reserve(256);

      this->NewFaceIds.reserve(256);
      this->EdgeTable.reserve(256);
    }

    // Initialize the polyhedron with a bounding box. This creates eight
    // vertices and six faces. Note that any data contained in the polyhedron
    // is replaced with these initial points and faces. A non-zero value is
    // returned on successful initialization.
    int Initialize(double bbox[6])
    {
      Points.clear();
      Faces.clear();

      // Make sure the bounds defines a volume
      if ( (bbox[1]-bbox[0]) <= 0.0 || (bbox[3]-bbox[2]) <= 0.0 ||
           (bbox[5]-bbox[4]) <= 0.0 )
      {
        return 0;
      }

      // Add the eight points: use a hex-style point ordering
      this->Points.emplace_back(PolyPoint(bbox[0],bbox[2],bbox[4]));
      this->Points.emplace_back(PolyPoint(bbox[1],bbox[2],bbox[4]));
      this->Points.emplace_back(PolyPoint(bbox[1],bbox[3],bbox[4]));
      this->Points.emplace_back(PolyPoint(bbox[0],bbox[3],bbox[4]));
      this->Points.emplace_back(PolyPoint(bbox[0],bbox[2],bbox[5]));
      this->Points.emplace_back(PolyPoint(bbox[1],bbox[2],bbox[5]));
      this->Points.emplace_back(PolyPoint(bbox[1],bbox[3],bbox[5]));
      this->Points.emplace_back(PolyPoint(bbox[0],bbox[3],bbox[5]));

      // Add the six faces. The ids are implicit ids in that they
      // point to the position of a point in the Points list.
      vtkIdType pts[4];
      pts[0] = 0; pts[1] = 4; pts[2] = 7; pts[3] = 3;
      this->Faces.emplace_back(PolyFace(4,pts,-1));
      pts[0] = 1; pts[1] = 2; pts[2] = 6; pts[3] = 5;
      this->Faces.emplace_back(PolyFace(4,pts,-1));
      pts[0] = 0; pts[1] = 1; pts[2] = 5; pts[3] = 4;
      this->Faces.emplace_back(PolyFace(4,pts,-1));
      pts[0] = 3; pts[1] = 7; pts[2] = 6; pts[3] = 2;
      this->Faces.emplace_back(PolyFace(4,pts,-1));
      pts[0] = 0; pts[1] = 3; pts[2] = 2; pts[3] = 1;
      this->Faces.emplace_back(PolyFace(4,pts,-1));
      pts[0] = 4; pts[1] = 5; pts[2] = 6; pts[3] = 7;
      this->Faces.emplace_back(PolyFace(4,pts,-1));

      return 1;
    } // Initialize

    // Return the number of active points in the polyhedron.
    vtkIdType GetNumberOfPoints()
    {
      vtkIdType numPts=0;
      for ( PolyPoint& pp : this->Points )
      {
        if ( pp.Status == Alive )
        {
          numPts++;
        }
      }
      return numPts;
    } // GetNumberOfPoints

    // Return the number of active faces in the polyhedron.
    vtkIdType GetNumberOfFaces()
    {
      vtkIdType numFaces=0;
      for ( PolyFace& pf : this->Faces )
      {
        if ( pf.Status == Alive )
        {
          numFaces++;
        }
      }
      return numFaces;
    } // GetNumberOfFaces

    // Evaluate the points and faces against the clip plane defined by the
    // point p and normalized normal n.  This method updates the value of the
    // points and the classification of the faces, and returns a
    // classification indicating whether the points are inside or outside of
    // the half space, or intersected by the plane. Note that a point is
    // "inside" if it evaluates <=0; "outside" otherwise.
    int Evaluate(double p[3], double n[3])
    {
      int numOut=0;
      int numIn=0;

      // Loop over all the Alive points and evaluate them. Also, if a point
      // is outside, it will be marked Dead in anticipation of no longer
      // being used after the clipping operation completes.
      for ( PolyPoint& pp : this->Points )
      {
        if ( pp.Status == Alive )
        {
          pp.Value = vtkPlane::Evaluate(n, p, pp.X);
          if ( pp.Value > 0 )
          {
            pp.Status = Dead;
            numOut++;
          }
          else
          {
            numIn++;
          }
        }  // if an Alive point
      }    // for all points

      // Indicate whether the set of points are inside, outside, or straddle
      // the clip plane. Convex properties means that if the points are all
      // outside, then the polyhedron is outside etc.
      if ( numIn > 0 && numOut == 0)
      {
        return Inside;
      }
      else if ( numOut > 0 && numIn == 0)
      {
        return Outside;
      }
      else
      {
        return Intersects;
      }
    } // Evaluate

    // Classify the face (Outside,Inside,Intersects).
    int ClassifyFace(PolyFace& pf)
    {
      int numOut=0;
      int numIn=0;

      // Loop over all the face points and determine in/out.
      PPoints& points = this->Points;
      PVerts& faceVerts = pf.FaceVerts;
      for ( vtkIdType pId : faceVerts )
      {
        if ( points[pId].Value > 0 )
        {
          numOut++;
        }
        else
        {
          numIn++;
        }
      } // for all face points

      // Indicate whether the set of points are inside, outside, or straddle
      // (intersect) the clip plane.
      if ( numIn > 0 && numOut == 0)
      {
        return Inside;
      }
      else if ( numOut > 0 && numIn == 0)
      {
        return Outside;
      }
      else
      {
        return Intersects;
      }
    } // ClassifyFace

    // Actually modify the polyhedron data structures by performing a clip
    // operation. Clipping may also generate some new intersection points
    // which are kept track of in the edge table (and used later to construct
    // a new face).
    void ClipFace(PolyFace& pf)
    {
      // Traverse all the current face points. Those that are inside are
      // retained. Edges that are clipped generate new points which are
      // added to the edge table, which is used to generate a new face.
      // Note that the points are inserted in order so that a loop - which
      // becomes a face - is created/
      size_t numPts = pf.FaceVerts.size();
      for ( auto i=0; i < numPts; ++i )
      {
        vtkIdType p0 = pf.FaceVerts[i];
        vtkIdType p1 = pf.FaceVerts[(i+1) % numPts];

        PolyPoint& pv0 = this->Points[p0];
        PolyPoint& pv1 = this->Points[p1];

        // Insert the existing face point if inside the clip half space.
        if ( pv0.Value <= 0.0 )
        {
          this->NewFaceIds.push_back(p0);
        }

        // Intersect the edge if clipped. Add a new point to the face loop.
        // Also make sure the edge has bot been previously inserted.
        if ( (pv0.Value <= 0.0 && pv1.Value > 0.0) ||
             (pv0.Value > 0.0 && pv1.Value <= 0.0) )
        {
          vtkIdType newId = this->EdgeTable.IsEdge(p0,p1);
          if ( newId < 0 )
          {
            double x, y, z;
            double t = -pv0.Value / (pv1.Value - pv0.Value);
            x = pv0.X[0] + t * (pv1.X[0]-pv0.X[0]);
            y = pv0.X[1] + t * (pv1.X[1]-pv0.X[1]);
            z = pv0.X[2] + t * (pv1.X[2]-pv0.X[2]);
            this->Points.emplace_back(PolyPoint(x,y,z));
            newId = this->Points.size() - 1;
            this->NewFaceIds.push_back(newId);
            this->EdgeTable.InsertEdge(PolyEdge(p0,p1,newId));
          }
        } // If clipped edge
      } // For all face vertices

      // Now redefine the face connectivity
      pf.FaceVerts.clear();
      for (auto i=0; i < this->NewFaceIds.size(); ++i)
      {
        pf.FaceVerts[i] = this->NewFaceIds[i];
      }
    } // ClipFace

    // Given the edge table containing the points composing a new face,
    // assemble the points into an ordered list of points defining the face.
    // The normal defines the plane on which radially sorting of the points.
    // The ordering of points proceeds as follows: 1) the points are projected
    // onto the best x-y-z plane; 2) the center of the points is computed; 3) the
    // points are ordered around the center without using trig functions; and
    // 4) the ordered points are used to create a new face which is added to
    // the polyhedron.
    void CreateFace(double n[3], vtkIdType spokeId)
    {
      vtkIdType numPts = this->EdgeTable.size();

      // Transfer points from the edge table to the list of face points.
      for ( auto i=0; i < numPts; ++i )
      {
        this->NewFaceIds[i] = this->EdgeTable[i].PtId;
      }

      // From the face normal, find the best projection plane defined by
      // (axis0,axis1).
      int axis0, axis1;
      if (fabs(n[0]) > fabs(n[1]))
      {
        if (fabs(n[0]) > fabs(n[2]))
        {
          axis0 = 1;
          axis1 = 2;
        }
        else
        {
          axis0 = 0;
          axis1 = 1;
        }
      }
      else
      {
        if (fabs(n[1]) > fabs(n[2]))
        {
          axis0 = 0;
          axis1 = 2;
        }
        else
        {
          axis0 = 0;
          axis1 = 1;
        }
      }

      // Find the center of the points.
      double xave[3] = {0,0,0};
      for ( auto i=0; i < numPts; ++i )
      {
        PolyPoint& pp = this->Points[this->NewFaceIds[i]];
        xave[axis0] += pp.X[axis0];
        xave[axis1] += pp.X[axis1];
      }
      xave[0] /= static_cast<double>(numPts);
      xave[1] /= static_cast<double>(numPts);
      xave[2] /= static_cast<double>(numPts);

      // Radially sort the points around the center.
      this->NewFaceIds.Sort(xave,axis0,axis1,&this->Points);

      // Create a new face and add it to the polyhedra.
      this->Faces.emplace_back(PolyFace(numPts,this->NewFaceIds.data(),spokeId));

    } // CreateFace

    // Clip the polyhedron with the plane provided. This typically will
    // modify the polyhedron, including generating new clip points, modifying
    // several faces, and generating a new face.  Initialize() must be called
    // before repeated calls to Clip(). The spokeId is the neighboring point
    // that is generating this Voronoi clip.
    void Clip(double x[3], double n[3], vtkIdType spokeId)
    {
      // The first thing to do is to evaluate the polyhedron points against
      // the clip plane. Note that points with value >0 are outside the
      // polyhedron; points with value <=0 are inside.
      this->Evaluate(x,n);

      // Loop over all active faces. Faces that are inside or all outside
      // are processed quickly. Clipping faces however takes some work.
      this->EdgeTable.clear();
      for ( PolyFace& pf : this->Faces )
      {
        if ( pf.Status == Alive )
        {
          this->NewFaceIds.clear();
          int faceClass = this->ClassifyFace(pf);
          if ( faceClass == Outside )
          {
            pf.Status = Dead; // kill/delete the face
          }
          else if ( faceClass == Inside )
          {
            ; // do nothing, face retained as is
          }
          else
          {
            this->ClipFace(pf); // clip face, add clip points
          }
        } // if face is active
      } // for all active faces

      // If faces have been clipped, then we construct a new face
      // and add it to the polyhedron.
      if ( this->EdgeTable.size() > 0 )
      {
        this->CreateFace(n,spokeId);
      }
    } // Clip

    // Produce a polyhedron and append it to the output polyhedra list
    // provided; i.e., convert the Polyhedron data structures into a VTK-like
    // arrays of points and faces.  Note that in a later compositing process,
    // point renumbering will occur as multiple polyhedra are
    // appended. Return the number of faces produced.
    struct OutputPolyhedron
    {
      // This is just used to accumulate the points and faces as each
      // polyhedron is processed. It also facilitates the renumbering of points
      // and cell connectivity.
      std::vector<vtkIdType> PtId; // the Voronoi point is used to generate this polyhedron
      struct ppoint {double X[3]; ppoint(double *x): X{x[0],x[1],x[2]} {} };
      std::vector<ppoint> Points; // the accumulated points of all polyhedra
      std::vector<vtkIdType> Faces; // the accumulated faces of all polyhedra (npts,p0,p1,...)

      std::vector<vtkIdType> NumPoints; // the number of points per polyhedron
      std::vector<vtkIdType> NumFaces; // the number of faces per polyhedron
      std::vector<vtkIdType> PointOffsets; // the point numbering offsets per polyhedron
      std::vector<vtkIdType> FaceOffsets; // the start of the faces per polyhedron

      // Method to append current polyhedron to a list of polyhedra.
      // int ProducePolyhedron(OutputPolyhedra& polys)
      // {
      //   return 0;
      // } // ProducePolyhedron
    }; // OutputPolyhedra
  }; // Polyhedron

  // Voronoi3D threaded algorithm methods follow ===============================
  // The class Polyhedron defined previously is used in multiple threads to
  // independently clip the Voronoi polyhedron associated with each input
  // point. The number of clips is controlled by selecting only those nearby,
  // neighboring points which fall within a local distance metric (the
  // Voronoi Flower).


} // anonymous namespace

//================= Begin VTK class proper =====================================
//------------------------------------------------------------------------------
// Construct object
vtkVoronoi3D::vtkVoronoi3D()
{
  this->Padding = 0.01;
  this->Tolerance = 0.00001; // currently hidden
  this->Locator = vtkSmartPointer<vtkStaticPointLocator>::New();
  this->Locator->SetNumberOfPointsPerBucket(2);
  this->GenerateScalars = NONE;
  this->PointOfInterest = (-1);
  this->MaximumNumberOfTileClips = VTK_ID_MAX;
  this->GenerateVoronoiFlower = false;
  this->NumberOfThreadsUsed = 0;
  this->Spheres = vtkSmartPointer<vtkSpheres>::New();

  // Optional second and third outputs for Voroni flower
  this->SetNumberOfOutputPorts(3);
}

//------------------------------------------------------------------------------
int vtkVoronoi3D::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPointSet* input = vtkPointSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid* output = vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro(<< "Generating 3D Voronoi Tessellation");

  Polyhedron p;
  double bbox[6] = {-0.5,0.5, -1,1, -1.5,1.5};
  p.Initialize(bbox);
  cout << "Initialize(NumPts,NumFaces): " << p.GetNumberOfPoints() << ", " << p.GetNumberOfFaces() << "\n";

  double n[3] = {1,0,0};
  double x[3] = {0,0,0};

  p.Clip(x,n,0);
  cout << "Clip(NumPts,NumFaces): " << p.GetNumberOfPoints() << ", " << p.GetNumberOfFaces() << "\n";

  p.Initialize(bbox);
  cout << "Initialize(NumPts,NumFaces): " << p.GetNumberOfPoints() << ", " << p.GetNumberOfFaces() << "\n";

  return 1;
}

//------------------------------------------------------------------------------
int vtkVoronoi3D::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  // Describe output info
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkUnstructuredGrid");
  return 1;
}

//------------------------------------------------------------------------------
// Since users have access to the locator we need to take into account the
// locator's modified time.
vtkMTimeType vtkVoronoi3D::GetMTime()
{
  vtkMTimeType mTime = this->vtkObject::GetMTime();
  vtkMTimeType time = this->Locator->GetMTime();
  return (time > mTime ? time : mTime);
}

//------------------------------------------------------------------------------
void vtkVoronoi3D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Padding: " << this->Padding << "\n";
  os << indent << "Tolerance: " << this->Tolerance << "\n";
  os << indent << "Locator: " << this->Locator << "\n";
  os << indent << "Generate Scalars: " << this->GenerateScalars << "\n";
  os << indent << "Point Of Interest: " << this->PointOfInterest << "\n";
  os << indent << "Maximum Number Of Tile Clips: " << this->MaximumNumberOfTileClips << "\n";
  os << indent << "Generate Voronoi Flower: " << (this->GenerateVoronoiFlower ? "On\n" : "Off\n");
}
