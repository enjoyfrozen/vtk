// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkVoronoi2D.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkDelaunay2D.h"
#include "vtkDoubleArray.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkMinimalStandardRandomSequence.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkSMPTools.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTransform.h"

#include <vector>

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkVoronoi2D);

//------------------------------------------------------------------------------
namespace //anonymous
{

// Evaluate the 2D line equation. Normal n is expected to be a unit
// normal. The point o is a point on the line (typically midpoint between two
// Voronoi points).
double EvaluateLine(double x[2], double o[2], double n[2])
{
  return ((x[0] - o[0]) * n[0] + (x[1] - o[1]) * n[1]);
}

// The data structure for representing a Voronoi tile vertex and implicitly,
// a Voronoi tile edge. The tile vertex has a position X, and the current
// value of the half-space clipping function. In counterclockwise direction,
// the PointId refers to the point id in the neighboring tile that, together
// with this tile's point id, produced the edge.
struct VVertex
{
  double X[2];       // position of this vertex
  vtkIdType PointId; // generating point id for the associated edge
  double Val;        // current value of the current half-space clipping function

  VVertex(double x[2], vtkIdType ptId) : X{x[0],x[1]}, PointId(ptId), Val(0.0)
  {
  }

  VVertex(const VVertex& v) = default;
};

// Typedefs defined for convenience.
using VertexRingType = std::vector<VVertex>;
using VertexRingIterator = std::vector<VVertex>::iterator;

// The following represents the convex Voronoi tile defined by an ordered
// (counterclockwise) ring of vertices. Note that during half-space clipping,
// NewVerts accumulates unclipped vertices and intersection vertices to
// redefine the tile. Use of std::vector.swap() is used between Verts and
// NewVerts to reduce copying.
struct VTile
{
  vtkIdType NPts;                   // total number of points in dataset
  const double *Points;             // input dataset points
  vtkIdType PointId;                // generating tile point id (in tile)
  double TileX[2];                  // generating tile point - x-y coordinates
  VertexRingType Verts;             // counterclockwise ordered loop of vertices
  VertexRingType NewVerts;          // accumulate new vertices to modify the tile
  vtkStaticPointLocator2D* Locator; // locator
  double PaddedBounds[4];           // the domain over which Voronoi is calculated
  double Bounds[4];                 // locator bounds
  int Divisions[2];                 // locator binning dimensions
  double H[2];                      // locator spacing
  double Padding2;                  // Bounding box padding distance

  // Instantiate with initial values. Typically tiles consist of 5 to 6
  // vertices. Preallocate for performance.
  VTile()
    : PointId(-1)
    , Locator(nullptr)
  {
    this->TileX[0] = 0.0;
    this->TileX[1] = 0.0;
    this->Verts.reserve(24);
    // Suppress paternalistic compiler warnings
    this->PaddedBounds[0] = this->PaddedBounds[1] = 0.0;
    this->PaddedBounds[2] = this->PaddedBounds[3] = 0.0;
    this->Bounds[0] = this->Bounds[1] = 0.0;
    this->Bounds[2] = this->Bounds[3] = 0.0;
    this->Divisions[0] = this->Divisions[1] = 0;
    this->H[0] = this->H[1] = 0.0;
    this->Padding2 = 1.0;
  }

  // Create an initial tile with a generating point - the resulting tile is
  // just the bounds rectangle, i.e., the four corners of the padded bounds
  // defining the tile containing the generating point. The tile vertices are
  // added in counterclockwise order (i.e., the tile normal is in the
  // z-direction). NOTE: the initial ordering direction of the points
  // controls how all of the tile polygons are generated.
  void Initialize(vtkIdType ptId, const double x[2])
  {
    // The generating tile point.
    this->PointId = ptId;

    // The generating point coordinates for the Voronoi tile.
    this->TileX[0] = x[0];
    this->TileX[1] = x[1];

    // Make sure that the tile is reset (if used multiple times as for
    // example in multiple threads).
    this->Verts.clear();

    // Now for each of the corners of the bounding box, add a tile
    // vertex. Note this is done in counterclockwise ordering. The initial
    // generating point id (<0, [-4,-1]) means that this point is on the
    // boundary. The numbering (-1,-2,-3,-4) corresponds to the top, lhs,
    // bottom, and rhs edges of the bounding box - useful for debugging
    // and trimming the Voronoi Flower when on the boundary.
    double v[2], *bds = this->PaddedBounds;
    v[0] = bds[1];
    v[1] = bds[3];
    this->Verts.emplace_back(VVertex(v,(-1)));

    v[0] = bds[0];
    v[1] = bds[3];
    this->Verts.emplace_back(VVertex(v,(-2)));

    v[0] = bds[0];
    v[1] = bds[2];
    this->Verts.emplace_back(VVertex(v,(-3)));

    v[0] = bds[1];
    v[1] = bds[2];
    this->Verts.emplace_back(VVertex(v,(-4)));
  }

  // Initialize with a convex polygon. The points must be in counterclockwise order
  // (normal in the z-direction). Points must not be coincident. The polygon must be
  // convex.
  void Initialize(
    vtkIdType ptId, const double x[2], vtkPoints* pts, vtkIdType nPts, const vtkIdType* p)
  {
    // The generating tile point
    this->PointId = ptId;

    // The generating point for the Voronoi tile
    this->TileX[0] = x[0];
    this->TileX[1] = x[1];

    // Make sure that the tile is reset.
    this->Verts.clear();

    // Now for each of the points of the polygon, insert a vertex. The initial point
    // id <0 corresponds to the N points of the polygon.
    double v[3];
    for (vtkIdType i = 0; i < nPts; ++i)
    {
      pts->GetPoint(p[i], v);
      this->Verts.emplace_back(VVertex(v,((i+1)*(-1))));
    }
  }

  // Convenience methods for moving around the modulo ring of the vertices.
  VertexRingIterator Previous(VertexRingIterator itr)
  {
    if (itr == this->Verts.begin())
    {
      return this->Verts.end() - 1;
    }
    return (itr - 1);
  }
  VertexRingIterator Next(VertexRingIterator itr)
  {
    if (itr == (this->Verts.end() - 1))
    {
      return this->Verts.begin();
    }
    return (itr + 1);
  }

  // Indicate whether the point provided would produce a half-space that
  // would intersect the tile.
  bool IntersectTile(double x[2])
  {
    // Produce the half-space
    double o[2], normal[2];
    o[0] = (x[0] + this->TileX[0]) / 2.0;
    o[1] = (x[1] + this->TileX[1]) / 2.0;
    normal[0] = x[0] - this->TileX[0];
    normal[1] = x[1] - this->TileX[1];
    vtkMath::Normalize2D(normal);

    // Evaluate all the points of the convex polygon. Positive values indicate
    // an intersection occurs.
    VertexRingIterator tPtr;
    for (tPtr = this->Verts.begin(); tPtr != this->Verts.end(); ++tPtr)
    {
      if (EvaluateLine(tPtr->X, o, normal) >= 0.0)
      {
        return true;
      }
    }
    return false;
  }

  // Populate a polydata with the tile. Used to produce output / for
  // debugging.
  void PopulateTileData(vtkPoints* centers, vtkCellArray* tile, vtkDoubleArray* radii)
  {
    vtkIdType nPts = static_cast<vtkIdType>(this->Verts.size());
    centers->SetNumberOfPoints(nPts);
    radii->SetNumberOfTuples(nPts);
    tile->InsertNextCell(static_cast<int>(nPts));

    vtkIdType i;
    double r;
    VertexRingIterator tPtr;
    for (i = 0, tPtr = this->Verts.begin(); tPtr != this->Verts.end(); ++tPtr, ++i)
    {
      centers->SetPoint(i, tPtr->X[0], tPtr->X[1], 0.0);
      r = sqrt((tPtr->X[0] - this->TileX[0]) * (tPtr->X[0] - this->TileX[0]) +
        (tPtr->X[1] - this->TileX[1]) * (tPtr->X[1] - this->TileX[1]));
      radii->SetTuple1(i, r);
      tile->InsertCellPoint(i);
    }
  }

  // Compute the bounding Voronoi flower circumcircle (i.e., contains all
  // petals of the Voronoi flower).  Returns the radius**2 of the bounding
  // flower circumcircle.
  double ComputeCircumFlower()
  {
    double r2, r2Max=VTK_FLOAT_MIN;
    VertexRingIterator tPtr;

    for (tPtr = this->Verts.begin(); tPtr != this->Verts.end(); ++tPtr)
    {
      r2 = (tPtr->X[0] - this->TileX[0]) * (tPtr->X[0] - this->TileX[0]) +
        (tPtr->X[1] - this->TileX[1]) * (tPtr->X[1] - this->TileX[1]);

      r2Max = ( r2 > r2Max ? r2 : r2Max );
    }

    return (4.0 * r2Max);
  }

  // Determine whether the provided point is within the Voronoi flower
  // error metric. Return true if it is; false otherwise.
  bool InFlower(const double p[3])
  {
    // Check against the flower petals
    VertexRingIterator tPtr;
    for (tPtr = this->Verts.begin(); tPtr != this->Verts.end(); ++tPtr)
    {
      double fr2 = (tPtr->X[0] - this->TileX[0]) * (tPtr->X[0] - this->TileX[0]) +
        (tPtr->X[1] - this->TileX[1]) * (tPtr->X[1] - this->TileX[1]);

      double r2 = (tPtr->X[0] - p[0]) * (tPtr->X[0] - p[0]) +
        (tPtr->X[1] - p[1]) * (tPtr->X[1] - p[1]);

      if ( r2 <= fr2 )
      {
        return true;
      }
    }

    // Point not in the flower
    return false;
  }

  // Clip the convex tile with a 2D half-space line. Return whether there was
  // clip performed or not (1 or 0).  The line is represented by an origin
  // and unit normal.
  int ClipTile(vtkIdType ptId, const double p[3])
  {
    // Create half-space
    double origin[2], normal[2];
    origin[0] = (p[0] + this->TileX[0]) / 2.0;
    origin[1] = (p[1] + this->TileX[1]) / 2.0;
    normal[0] = p[0] - this->TileX[0];
    normal[1] = p[1] - this->TileX[1];
    vtkMath::Normalize2D(normal);

    // Evaluate all the points of the convex polygon. Positive valued points
    // are eventually clipped away from the tile.
    bool intersection;
    VertexRingIterator tPtr;
    for (intersection = false, tPtr = this->Verts.begin(); tPtr != this->Verts.end(); ++tPtr)
    {
      tPtr->Val = EvaluateLine(tPtr->X, origin, normal);
      intersection = (tPtr->Val >= 0.0 ? true : intersection);
    }
    if (!intersection)
    {
      return 0;
    }

    // The tile has been determined to be clipped by the half-space line. Add
    // the remaining tile vertices and new intersection points to modify the
    // tile. Care is taken to preserve the counterclockwise vertex ordering.
    this->NewVerts.clear();
    for (intersection = false, tPtr = this->Verts.begin(); tPtr != this->Verts.end(); ++tPtr)
    {
      // If the vertex is inside the clip, just add it.
      if ( tPtr->Val < 0.0 )
      {
        this->NewVerts.emplace_back(VVertex(*tPtr));
      }

      // Now see if the edge requires clipping. If so, create a new tile
      // vertex. Note that depending on the order of edge, the new vertex
      // has to be treated differently (i.e., the neigboring tile id).
      double t, x[2];
      VertexRingIterator tNext = this->Next(tPtr);
      if ( (tPtr->Val < 0.0 && tNext->Val >= 0.0) ||
           (tPtr->Val >= 0.0 && tNext->Val < 0.0 ) )
      {
        t = ( -tPtr->Val ) / ( tNext->Val - tPtr->Val );
        x[0] = tPtr->X[0] + t * (tNext->X[0] - tPtr->X[0]);
        x[1] = tPtr->X[1] + t * (tNext->X[1] - tPtr->X[1]);
        vtkIdType pId = ( tPtr->Val < 0.0 ? ptId : tPtr->PointId );
        this->NewVerts.emplace_back(VVertex(x,pId));
      } //check for intersecting edge
    } // clip verts & edges

    // Now just swap the newly added vertices to update the tile.
    this->Verts.swap(this->NewVerts);

    return 1;
  }

  // If spoke pruning is requested, then edges that are "small" relative to
  // the length of the spoke are deleted.
  void Prune(double pruneTol2)
  {
    VertexRingIterator tPtr, tNext, endItr;
    for (tPtr = this->Verts.begin(); tPtr != this->Verts.end(); ++tPtr)
    {
      tNext = this->Next(tPtr);
      double eLen2 = (tPtr->X[0]-tNext->X[0])*(tPtr->X[0]-tNext->X[0]) +
        (tPtr->X[1]-tNext->X[1])*(tPtr->X[1]-tNext->X[1]);
      double spokeLen2;
      if ( tPtr->PointId >= 0 )
      {
        const double *px = this->Points + 3*tPtr->PointId;
        spokeLen2 = (this->TileX[0]-px[0])*(this->TileX[0]-px[0]) +
          (this->TileX[1]-px[1])*(this->TileX[1]-px[1]);
      }
      else
      {
        spokeLen2 = this->Padding2;
      }
      tPtr->Val = eLen2 / spokeLen2;
    }
    // Now remove spokes (if any) and erase them
    endItr = std::remove_if(this->Verts.begin(),this->Verts.end(),
                            [&](VVertex& v) { return (v.Val <= pruneTol2); });
    this->Verts.erase(endItr,this->Verts.end());
  }

  // Generate a Voronoi tile by iterative clipping of the tile with nearby
  // points.  Termination of the clipping process occurs when the neighboring
  // points become "far enough" away from the generating point (i.e., the
  // Voronoi Flower error metric is satisfied).
  bool BuildTile(vtkIdList* pIds, vtkDoubleArray *radii2, const double* pts,
                 vtkIdType maxClips, bool prune, double pruneTol2)
  {
    // Ensure there are clips to be performed.
    if ( maxClips <= 0 )
    {
      return true;
    }

    const double* v;
    vtkIdType ptId, numClips = 0, numClipAttempts = 0;
    vtkIdType prevNumClips, numPts = this->NPts;

    // Request neighboring points around the generating point in annular
    // rings. The rings are defined by an inner and outer radius
    // (min,max]. The requested points fall within the annulus, with their
    // radius r:(min<r<=max). The neighboring points are used to perform
    // half-space clipping of the Voronoi tile. (The original tile around the
    // generating point is defined from the bounding box of the domain.) The
    // Voronoi Flower and CircumFlower error metrics are used to terminate
    // the clipping process. The Flower is the set of all Flower Petals
    // (i.e., Delaunay circumcircles) centered at the Voronoi Tile
    // vertices. The CircumFlower is the circle that bounds all petals, i.e.,
    // Voronoi Flower.
    constexpr int QUERY_SIZE = 6;
    double R2 = VTK_FLOAT_MAX;
    double annulusMin2 = 0.0;
    double annulusMax2 = this->Locator->FindNPointsInAnnulus(QUERY_SIZE, this->TileX, pIds, radii2, annulusMin2);
    vtkIdType numPtIds, *pIdsPtr;
    double *radii2Ptr;

    // Now add additional points until they are outside of the Voronoi
    // flower. For speed, we use the bounding Voronoi circumcircle to
    // determine whether points are outside of the flower. Note that in the
    // while() loop below, if the number of points pIds<=0, then all points
    // have been exhausted and the loop is exited.
    while ( (numPtIds = pIds->GetNumberOfIds()) > 0 && annulusMin2 <= R2 && numClips < maxClips )
    {
      pIdsPtr = pIds->GetPointer(0);
      radii2Ptr = radii2->GetPointer(0);
      for ( vtkIdType i=0; i < numPtIds && numClips < maxClips; ++i )
      {
        numClipAttempts++;
        ptId = pIdsPtr[i];
        v = pts + 3 * ptId;
        if ( radii2Ptr[i] <= R2 && this->InFlower(v) &&
             this->ClipTile(ptId, v) )
        {
          R2 = this->ComputeCircumFlower();
          numClips++;
        }
      } // process all points in requested annulus

      // See if circumflower radius is less then radius of annulus request; if so, the
      // Voronoi tile has been formed.
      if ( R2 < annulusMax2 )
      {
        break;
      }

      // Grab the next ring / annulus of points
      annulusMin2 = annulusMax2;
      annulusMax2 = this->Locator->FindNPointsInAnnulus(QUERY_SIZE, this->TileX, pIds, radii2, annulusMin2);
    } // while points still in Voronoi circumflower

    // If requested, remove tile edges (and associated spokes) which are
    // small relative to the spoke length.
    if ( prune )
    {
      this->Prune(pruneTol2);
    }

    return true;
  }
};

// Used to accumulate the points within a thread from each tile. Later in
// Reduce() we composite the output from all of the threads.
struct TileVertex
{
  double X;
  double Y;
  TileVertex(double x, double y)
    : X(x)
    , Y(y)
  {
  }
};
using TileVertexType = std::vector<TileVertex>;

// Spokes are used to track connecting edges between generating points (i.e.,
// a connected edge, or spoke, connects two generating Voronoi points that
// produce a clipping half-space). A sorted list of spokes (sorted around
// each generating point in CCW order) forms a wheel. The spoke and wheel
// edge data structure is used later to optionally validate the topology, and
// ultimately generate the Delaunay triangulation, if requested. Note that
// the Wheels array is essentially an offset into the array of spokes,
// indicating the beginning of a group of n spokes that forms the wheel.
// Each spoke may be classified differently, for example a spoke might
// connect the "infinite" or boundary Voronoi tile, which means it is not
// used to construct the Delaunay triangulation. Also a spoke is associated
// with a particular wheel, the other wheel it is connected to is recorded
// by the spoke Id.
struct Spoke
{
  vtkIdType Id; // Id of the wheel that the spoke is connected to (wheelId,Id)
  unsigned char Classification; // Indicate the classification of this spoke
  enum SpokeClass
  {
    VALID=0, // Valid edge, contributes to the output
    BOUNDARY=1, // Edge connected to the Voronoi boundary
    DEGENERATE=2, // Edge connects two points but in only one direction
    INTERSECTING=3, // Intersecting edge, part of a topological bubble
    SINGLETON=4, // Wheel with only one edge, should never happen
  };
  Spoke() : Id(-1), Classification(VALID) {}
  Spoke(vtkIdType id, unsigned char classification) :
    Id(id), Classification(classification) {}
};

// Typedefs related to edge spoke structure. Note that the WheelsType
// vector is used to keep track of the number of spokes in a Voronoi
// tile, as well as the number of edges in the Voronoi tile (there is
// a one-to-one correspondance between spoke and tile edge).
using WheelsType = std::vector<vtkIdType>;
using SpokeType = std::vector<Spoke>;
using SpokerIterator = SpokeType::iterator;

// Class to manage batches of points. This is used to improve threaded
// performance and reduce memory. Note the "exception" when a subset of items
// is to be processed in a single batch (e.g., single point of interest).
struct BatchManager
{
  vtkIdType Num; //Number of total items to process
  vtkIdType BatchSize; //The desired batch size (clamped by Num)
  vtkIdType NumBatches; //The total number of batches to process
  vtkIdType SubRange[2]; //Define a subrange of items to process
  BatchManager(vtkIdType num, vtkIdType batchSize) : Num(num), BatchSize(batchSize)
  {
    this->NumBatches = static_cast<vtkIdType>(ceil(static_cast<double>(num) / batchSize));
    this->SubRange[0] = this->SubRange[1] = (-1); //indicate no subrange
  }
  void SetSubRange(vtkIdType minRange, vtkIdType maxRange)
  {
    this->SubRange[0] = minRange;
    this->SubRange[1] = maxRange;
  }
  vtkIdType GetNumberOfBatches()
  {
    return ( this->SubRange[0] < 0 ? this->NumBatches : 1);
  }
  vtkIdType GetBatchItemRange(vtkIdType batchNum, vtkIdType &startId, vtkIdType &endId) const
  {
    if ( this->SubRange[0] < 0 )
    {
      startId = batchNum * this->BatchSize;
      endId = startId + this->BatchSize;
    }
    else
    {
      startId = this->SubRange[0];
      endId = this->SubRange[1];
    }
    endId = (endId > this->Num ? this->Num : endId);
    return (endId - startId);
  }
}; //BatchManager

// This is used to track information about each batch. Used to efficiently
// process items (e.g., generated points) in a batch.
struct BatchInfo
{
  vtkIdType Id; //the batch id
  vtkIdType Num; //the number of Voronoi vertices produced in this batch
  BatchInfo(vtkIdType id, vtkIdType num) : Id(id), Num(num) {}
};
// A list of batch information. Used by threads to record the
// batches they've processed.
using BatchInfoType = std::vector<BatchInfo>;

// Track local data on a per-thread basis. In the Reduce() method this
// information will be used to composite the data from each thread into a
// single vtkPolyData output and/or generate the wheel/spoke structure for
// later Delaunay generation.
struct LocalDataType
{
  vtkIdType NumberOfTiles;
  vtkIdType NumberOfPoints;
  int MaxSides;
  BatchInfoType LocalBatches; //the list of batches processed by this thread
  TileVertexType LocalPoints; //x-y coordinates defining the tile vertices
  SpokeType LocalSpokes;  //connecting edges/spokes for each tile
  VTile Tile; //computational tile does the core work
  vtkIdType ThreadId; //assign a thread id [0,NumThreadsUsed)

  LocalDataType()
    : NumberOfTiles(0)
    , NumberOfPoints(0)
    , MaxSides(0)
    , ThreadId(-1)
  {
    this->LocalBatches.reserve(2048);
    this->LocalPoints.reserve(2048);
    this->LocalSpokes.reserve(2048);
  }
};

// Map thread local data to thread id.
using ThreadMapType = std::vector<LocalDataType*>;

// FYI- These helper functions are created to break cyclic dependencies
// between classes.
// Helper function: Given an input point id (or tile id, since there is a
// tile for each point), return the number of points defining the tile
// polygon (i.e., also the number of tile edges, or number of spokes). Only
// call this method after VoronoiTiles::Reduce() has been invoked.
vtkIdType GetWheelOffset(const WheelsType& offsets, vtkIdType id)
{ return offsets[id]; }

// Helper function: Given an input point id (or tile id, since there is a
// tile for each point), return the number of points defining the tile
// polygon (i.e., also the number of tile edges, or number of spokes). Only
// call this method after VoronoiTiles::Reduce() has been invoked.
vtkIdType GetNumberOfSpokes(const WheelsType& offsets, vtkIdType id)
{ return (offsets[id+1] - offsets[id]); }

// Special function to produce the output when a single Voronoi tile is
// requested (i.e., a point of interest).
void GeneratePOITile(vtkIdType poi, const WheelsType& wheels, ThreadMapType &threadMap,
                     double *pts, double z, vtkIdType *conn, vtkIdType *offsets,
                     vtkIdType *scalars, int scalarMode, vtkDataArray *regionIds)
{
  // Get the localData for the single thread that processed the tile.
  LocalDataType& localData = *(threadMap[0]);
  auto pItr = localData.LocalPoints.begin();
  vtkIdType totalTilePts = GetNumberOfSpokes(wheels,0);

  // First copy all the tile points in this tile
  for (auto i=0; i < totalTilePts; ++i, ++pItr)
  {
    *pts++ = pItr->X;
    *pts++ = pItr->Y;
    *pts++ = z;
  } // for all points in this batch

  // Generate the cell array using the more efficient
  // vtkCellArray::SetData() method.
  vtkIdType pId = 0;
  std::generate(conn,conn+totalTilePts, [&] {return pId++;});

  // Now generate the cell offsets for this run of contiguous tiles.
  offsets[0] = 0;

  // Generate cell scalars if requested
  if ( scalars != nullptr )
  {
    // If requested, thread id
    if ( scalarMode == vtkVoronoi2D::THREAD_IDS )
    {
      scalars[0] = 0;
    }
    // the generating point id
    else if ( scalarMode == vtkVoronoi2D::POINT_IDS )
    {
      scalars[0] = poi;
    }
    // Region ids for output tiles
    else if ( scalarMode == vtkVoronoi2D::REGION_IDS && regionIds )
    {
      scalars[0] = regionIds->GetComponent(poi,0);
    }
    // the number of sides of the voronoi tile
    else //if ( scalarMode == vtkVoronoi2D::NUMBER_SIDES )
    {
      scalars[0] = totalTilePts;
    }
  } // if cell/tile scalars generated
}

// Functor used to generate the filter's Voronoi output. The threading is
// across the n threads used to compute the Voronoi tiles. So this is
// effectively a parallel copy to the output polydata.
struct ProduceVoronoiOutput
{
  const BatchManager &Batcher;
  WheelsType &Wheels;
  const ThreadMapType& ThreadMap;
  double *Pts;
  double Z;
  vtkIdType *Conn;
  vtkIdType *Offsets;
  vtkIdType *Scalars;
  int ScalarMode;
  vtkDataArray *RegionIds;

  ProduceVoronoiOutput(const BatchManager &batcher, WheelsType& wheels,
                       ThreadMapType &threadMap, double *pts, double z,
                       vtkIdType *conn, vtkIdType *offsets,
                       vtkIdType *scalars, int scalarMode, vtkDataArray *regionIds)
    : Batcher(batcher)
    , Wheels(wheels)
    , ThreadMap(threadMap)
    , Pts(pts)
    , Z(z)
    , Conn(conn)
    , Offsets(offsets)
    , Scalars(scalars)
    , ScalarMode(scalarMode)
    , RegionIds(regionIds)
  {
  }

  void operator()(vtkIdType threadId, vtkIdType endThreadId)
  {
    const BatchManager &batcher = this->Batcher;
    const WheelsType &wheels = this->Wheels;

    // Produce the primary (Voronoi) output. Traverse each thread, grab its
    // batches and points in each batch, and copy local data into the filter
    // output.
    for ( ; threadId < endThreadId; ++threadId )
    {
      LocalDataType& localData = *(this->ThreadMap[threadId]);
      vtkIdType ptId, endPtId, ptOffset;
      double *pts;
      auto pItr = localData.LocalPoints.begin();

      // Process all point batches in the current thread. Recall that
      // a batch consists of a set of contiguous point ids. Also recall
      // that the point id and the tile id are the same (i.e., for every
      // generating point, a tile is created).
      for ( auto& batchInfo : localData.LocalBatches )
      {
        vtkIdType numBatchPts = batcher.GetBatchItemRange(batchInfo.Id, ptId, endPtId);
        ptOffset = GetWheelOffset(wheels,ptId);
        vtkIdType totalTilePts = batchInfo.Num;

        // First copy all the tile points in this batch
        pts = this->Pts + 3*ptOffset;
        for (auto i=0; i < totalTilePts; ++i, ++pItr)
        {
          *pts++ = pItr->X;
          *pts++ = pItr->Y;
          *pts++ = this->Z;
        } // for all points in this batch

        // Generate the cell array using the more efficient
        // vtkCellArray::SetData() method. Start by generating the
        // connectivity, writing into the output connectivity array.
        vtkIdType pId = ptOffset;
        vtkIdType *connPtr = this->Conn + ptOffset;
        std::generate(connPtr,connPtr+totalTilePts, [&] {return pId++;});

        // Now generate the cell offsets for this run of contiguous tiles.
        // Each point in the batch generates a tile.
        vtkIdType *offsetPtr = this->Offsets + ptId;
        for ( auto i=0; i < numBatchPts; ++i)
        {
          *offsetPtr++ = GetWheelOffset(wheels,ptId+i);
        }

        // Generate cell scalars if requested
        if ( this->Scalars != nullptr )
        {
          vtkIdType *scalars = this->Scalars + ptId;
          // If requested, thread id
          if ( this->ScalarMode == vtkVoronoi2D::THREAD_IDS )
          {
            std::fill_n(scalars,numBatchPts,threadId);
          }
          // the generating point id
          else if ( this->ScalarMode == vtkVoronoi2D::POINT_IDS )
          {
            pId = ptId;
            std::generate(scalars,scalars+numBatchPts, [&] {return pId++;});
          }
          // Region ids for output tiles
          else if ( this->ScalarMode == vtkVoronoi2D::REGION_IDS && this->RegionIds )
          {
            for ( auto i=0; i < numBatchPts; ++i)
            {
              *scalars++ = this->RegionIds->GetComponent(ptId+i,0);
            }
          }
          // the number of sides of the voronoi tile
          else //if ( this->ScalarMode == vtkVoronoi2D::NUMBER_SIDES )
          {
            for ( auto i=0; i < numBatchPts; ++i)
            {
              *scalars++ = GetNumberOfSpokes(wheels,ptId+i);
            }
          }
        } // if cell/tile scalars generated
      }   // for all batches in this thread
    }     // for all threads to process
  }

}; // ProduceVoronoiOutput

// Functor used to generate the filter's Delaunay structures. The threading
// is across the n threads used to compute the Voronoi tiles. So this is
// effectively a parallel generation of the wheels/spokes data structure.
struct ProduceDelaunayWheels
{
  const BatchManager &Batcher;
  const ThreadMapType& ThreadMap;
  const WheelsType& Wheels;
  SpokeType& Spokes;

  ProduceDelaunayWheels(const BatchManager &batcher, ThreadMapType &threadMap,
                        WheelsType& wheels, SpokeType& spokes)
    : Batcher(batcher)
    , ThreadMap(threadMap)
    , Wheels(wheels)
    , Spokes(spokes)
  {
  }

  void operator()(vtkIdType threadId, vtkIdType endThreadId)
  {
    const BatchManager &batcher = this->Batcher;
    const WheelsType &wheels = this->Wheels;
    Spoke *spokes;
    vtkIdType i, numSpokes, ptId;

    // Now copy the spokes into the right spot
    for ( ; threadId < endThreadId; ++threadId )
    {
      LocalDataType& localData = *(this->ThreadMap[threadId]);

      // Loop over all batches in this thread
      auto spItr = localData.LocalSpokes.begin();
      vtkIdType ptId, endPtId;
      for ( auto& batchInfo : localData.LocalBatches )
      {
        batcher.GetBatchItemRange(batchInfo.Id, ptId, endPtId);

        // Loop over all contiguous spokes in this batch
        spokes = this->Spokes.data() + GetWheelOffset(wheels,ptId);
        vtkIdType totalSpokes = batchInfo.Num;
        for ( ; ptId < endPtId; ++ptId )
        {
          vtkIdType numSpokes = GetNumberOfSpokes(wheels,ptId);
          for ( auto i=0; i < numSpokes; ++i, ++spItr, ++spokes)
          {
            spokes->Id = spItr->Id;
            spokes->Classification = spItr->Classification;
          }
        } // for all contiguous points in this batch
      }   // for all batches
    }//across all threads in this batch
  }

}; // ProduceDelaunayWheels

// The threaded core of the algorithm. This could be templated over point
// type, but due to numerical sensitivity we'll just process doubles for now.
struct VoronoiTiles
{
  const BatchManager &Batcher; //Controls processing of tile generating points
  vtkIdType NPts; //The number of input (Voronoi tile generation) points
  const double *Points; //Input points
  vtkIdType PointOfInterest; //When processing just a single point
  vtkDataArray *RegionIds; // Optional region ids to control tessellation
  vtkStaticPointLocator2D *Locator; // Used to (quickly) find nearby points
  double PaddedBounds[4]; //the expanded domain over which Voronoi is calculated
  double Bounds[4]; //locator bounds
  int Divisions[2]; //Internal parameters for computing
  double H[2]; //Locator bin spacing
  double Padding; //The padding distance**2 around the bounding box

  vtkPoints *NewPoints;//New Voronoi points generated
  vtkCellArray *Tiles; //Actual output convex polygons (Voronoi tiles)
  int ScalarMode;//Indicate how to compute scalars
  vtkIdTypeArray *Scalars; //Output Scalars if requested
  vtkIdType MaxClips; //Control the maximum number of half-space clips
  int NumThreadsUsed; //Keep track of the number of threads used

  // This are used to create the spokes and wheels graph used to validate
  // the tessllation and produce a Delaunay triangulation.
  int MaxSides; //Maximum number of sides (i.e., spokes) in a generated Voronoi tile
  WheelsType Wheels; //Wheel/spokes data structure: offset array to spokes
  vtkIdType NumSpokes; //Total number of edges / spokes
  SpokeType Spokes; //Spokes / edges with classification
  bool PruneSpokes; //Indicate whether to prune small edges / spokes
  double PruneTol2; //Specify a spoke prune tolerance
  vtkCellArray *DelTris; //If requested, triangles are placed here for Delaunay

  // Used for controlling filter abort and accessing filter information
  vtkVoronoi2D* Filter;

  // Storage local to each thread. We don't want to allocate working arrays
  // on every thread invocation. Thread local storage saves lots of
  // new/delete (e.g. the PIds).
  vtkSMPThreadLocalObject<vtkIdList> PIds;
  vtkSMPThreadLocalObject<vtkDoubleArray> Radii2;
  vtkSMPThreadLocal<LocalDataType> LocalData;

  VoronoiTiles(BatchManager &batcher, vtkIdType npts, double* points, vtkIdType poi,
               vtkDataArray* regionIds, vtkStaticPointLocator2D* loc, double padding,
               vtkPolyData* output, int scalarMode, vtkPolyData *delOutput,
               vtkIdType maxClips, vtkVoronoi2D* filter)
    : Batcher(batcher)
    , NPts(npts)
    , Points(points)
    , PointOfInterest(poi)
    , RegionIds(regionIds)
    , Locator(loc)
    , Padding(padding)
    , ScalarMode(scalarMode)
    , MaxClips(maxClips)
    , NumThreadsUsed(0)
    , MaxSides(0)
    , NumSpokes(0)
    , DelTris(nullptr)
    , Filter(filter)

  {
    // Tiles and associated points are filled in later in Reduce()
    this->NewPoints = output->GetPoints();
    this->Wheels.resize(this->NPts+1,0); //initialized to zero
    this->Tiles = output->GetPolys();

    // Output scalars may be produced if desired
    this->Scalars = static_cast<vtkIdTypeArray*>(output->GetCellData()->GetScalars());

    // Compute some local data for speed. Just need 2D info because
    // everything is happening in 2D.
    loc->GetBounds(this->Bounds);
    loc->GetDivisions(this->Divisions);
    this->H[0] = (this->Bounds[1] - this->Bounds[0]) / static_cast<double>(this->Divisions[0]);
    this->H[1] = (this->Bounds[3] - this->Bounds[2]) / static_cast<double>(this->Divisions[1]);

    // Define the Voronoi domain by padding out from bounds.
    for (int i = 0; i < 2; ++i)
    {
      this->PaddedBounds[2 * i] = this->Bounds[2 * i] - padding;
      this->PaddedBounds[2 * i + 1] = this->Bounds[2 * i + 1] + padding;
    }

    // Control spoke pruning
    this->PruneSpokes = filter->GetPruneSpokes();
    double pruneTol = filter->GetPruneTolerance();
    this->PruneTol2 = pruneTol * pruneTol;

    // Setup point of interest (for debugging)
    this->PointOfInterest = filter->GetPointOfInterest();

    // Delaunay triangulation computed later (if requested). Note that the
    // input points are the output points of the Delaunay triangulation, so
    // only output triangles need to be generated.
    if ( delOutput != nullptr )
    {
      this->DelTris = delOutput->GetPolys();
    }
  }

  // Allocate a little bit of memory to get started. Set some initial values
  // for each thread for accelerating computation.
  void Initialize()
  {
    vtkIdList*& pIds = this->PIds.Local();
    pIds->Allocate(128); // allocate some memory

    vtkDoubleArray*& radii2 = this->Radii2.Local();
    radii2->Allocate(128); // allocate some memory

    LocalDataType& localData = this->LocalData.Local();
    localData.Tile.NPts = this->NPts;
    localData.Tile.Points = this->Points;
    localData.Tile.Locator = this->Locator;
    localData.Tile.Divisions[0] = this->Divisions[0];
    localData.Tile.Divisions[1] = this->Divisions[1];
    localData.Tile.PaddedBounds[0] = this->PaddedBounds[0];
    localData.Tile.PaddedBounds[1] = this->PaddedBounds[1];
    localData.Tile.PaddedBounds[2] = this->PaddedBounds[2];
    localData.Tile.PaddedBounds[3] = this->PaddedBounds[3];
    localData.Tile.Bounds[0] = this->Bounds[0];
    localData.Tile.Bounds[1] = this->Bounds[1];
    localData.Tile.Bounds[2] = this->Bounds[2];
    localData.Tile.Bounds[3] = this->Bounds[3];
    localData.Tile.H[0] = this->H[0];
    localData.Tile.H[1] = this->H[1];
    localData.Tile.Padding2 = this->Padding * this->Padding;
  }

  void operator()(vtkIdType batchId, vtkIdType endBatchId)
  {
    const BatchManager &batcher = this->Batcher;
    vtkDataArray *regionIds = this->RegionIds;
    vtkIdList*& pIds = this->PIds.Local();
    vtkDoubleArray*& radii2 = this->Radii2.Local();
    LocalDataType& localData = this->LocalData.Local();
    vtkIdType& numTiles = localData.NumberOfTiles;
    vtkIdType& numPoints = localData.NumberOfPoints;
    int &maxSides = localData.MaxSides;
    BatchInfoType& lBatches = localData.LocalBatches;
    TileVertexType& lPoints = localData.LocalPoints;
    SpokeType& lSpokes = localData.LocalSpokes;
    VTile& tile = localData.Tile;
    bool isFirst = vtkSMPTools::GetSingleThread();
    vtkIdType checkAbortInterval = std::min((endBatchId - batchId) / 10 + 1, (vtkIdType)1000);

    // Process the tile generating points in batches. This performs
    // a little better than independent point-by-point processing, and saves
    // some memory as well.
    for (; batchId < endBatchId; ++batchId)
    {
      // Support algorithm interrupts
      if (batchId % checkAbortInterval == 0)
      {
        if (isFirst)
        {
          this->Filter->CheckAbort();
        }
        if (this->Filter->GetAbortOutput())
        {
          break;
        }
      }

      // Process all points in this batch. Record the batch being
      // processed. Remember that the point ids are contiguous in this
      // batch.
      vtkIdType ptId, endPtId;
      batcher.GetBatchItemRange(batchId, ptId, endPtId);
      const double* x = this->Points + 3 * ptId;
      vtkIdType totalTilePts=0;

      for (; ptId < endPtId; ++ptId, x += 3)
      {
        // If the generating point is an outside region, we do not need to
        // process this tile.
        if ( regionIds && regionIds->GetComponent(ptId,0) < 0 )
        {
          continue;
        }

        // Initialize the Voronoi tile
        tile.Initialize(ptId, x);

        // If tile is successfully built, copy the convex tile polygon and
        // points it to thread local storage.
        int maxClips = ( this->MaxClips < this->NPts ? this->MaxClips :
                         (this->NPts > 1 ? (this->NPts-1) : 0) );
        if (tile.BuildTile(pIds, radii2, this->Points, maxClips,
                           this->PruneSpokes, this->PruneTol2))
        {
          // Now accumulate the tile / convex polygon in this thread
          vtkIdType i, nPts = static_cast<vtkIdType>(tile.Verts.size());
          this->Wheels[ptId] = nPts; //initially, the number of points. Later prefix sum for offsets.
          maxSides = (nPts > maxSides ? nPts : maxSides);

          // Now accumulate the tile points for this Voronoi tile. Note that
          // the connectivity is not needed, since no points are shared between
          // tiles. So the connectivity is implicitly ordered.
          for (i = 0; i < nPts; ++i)
          {
            lPoints.emplace_back(TileVertex(tile.Verts[i].X[0],tile.Verts[i].X[1]));
          }

          // If a Delaunay triangulation is requested, gather information for
          // later processing. The spoke edges are used to build the
          // triangulation.  Also classify spokes as being valid, or connected
          // to the Voronoi boundary.
          if ( this->DelTris != nullptr )
          {
            for (i=0; i < nPts; ++i)
            {
              vtkIdType ptId = tile.Verts[i].PointId;
              lSpokes.emplace_back(Spoke(ptId,(ptId<0 ? Spoke::BOUNDARY : Spoke::VALID)));
            }
          }
          totalTilePts += nPts; //total number of tile points generated in this batch
        } // if tile successfully generated
      } // for all points in this batch
      lBatches.emplace_back(BatchInfo(batchId,totalTilePts));
    } // for all batches of points
  }

  void Reduce()
  {
    // Count the total number of points and tiles, plus optionally the number
    // of spokes. For each thread local data, keep track of the point id and
    // tile id offsets to update the cell connectivity list. This will be
    // used later to create the Voronoi output and/or Delaunay output.
    vtkIdType totalPoints = 0;
    this->NumThreadsUsed = 0;
    this->NumSpokes = 0;
    this->MaxSides = 0;

    // This vector maps the local thread information via a thread id
    // with (0 <= threadId < NumThreadsUsed).
    ThreadMapType threadMap;

    // Gather information along with a prefix sum of some information
    // across all the threads.
    for ( auto& localData : this->LocalData)
    {
      threadMap.push_back(&localData);
      this->NumSpokes += localData.LocalSpokes.size();
      this->NumThreadsUsed++;
      this->MaxSides = ( localData.MaxSides > this->MaxSides ?
                         localData.MaxSides : this->MaxSides);
    } // loop over local thread output

    // Prefix sum over tiles to determine connectivity offsets,
    // as well total number of tile points.
    vtkIdType offset;
    for ( vtkIdType id=0; id < this->NPts; ++id )
    {
      offset = this->Wheels[id];
      this->Wheels[id] = totalPoints;
      totalPoints += offset;
    }
    // Cap off the wheels / tile offsets array.
    this->Wheels[this->NPts] = totalPoints;

    // If Voronoi output is requested, produce the output convex polygons
    // (tiles) and associated points.
    bool poi = (this->PointOfInterest >=0 && this->PointOfInterest < this->NPts);
    int outputType = this->Filter->GetOutputType();
    if ( outputType == vtkVoronoi2D::VORONOI ||
         outputType == vtkVoronoi2D::VORONOI_AND_DELAUNAY )
    {
      // When generating a single tile at a PointOfInterest, adjust the tile
      // counts. Otherwise the number of tiles is the number of input points.
      vtkIdType totalTiles = (poi ? 1 : this->NPts);

      // Now copy the data into the global filter output. Points are placed in
      // the x-y plane.
      const double z = this->Points[2];
      this->NewPoints->SetNumberOfPoints(totalPoints);
      double *pts = static_cast<vtkDoubleArray*>(this->NewPoints->GetData())->GetPointer(0);

      // Structures for cell definitions. Directly create the offsets
      // and connectivity for efficiency.
      vtkNew<vtkIdTypeArray> offsets;
      offsets->SetNumberOfTuples(totalTiles+1);
      vtkIdType *offsetsPtr = offsets->GetPointer(0);
      vtkNew<vtkIdTypeArray> connectivity;
      connectivity->SetNumberOfTuples(totalPoints);
      vtkIdType *connectivityPtr = connectivity->GetPointer(0);
      this->Tiles->SetData(offsets, connectivity);

      // If scalars requested, allocate them
      vtkIdType *scalars = nullptr;
      if ( this->Scalars )
      {
        this->Scalars->SetNumberOfTuples(totalTiles);
        scalars = this->Scalars->GetPointer(0);
      }

      // Process the data differently if a point of interest tile
      // is requested. Else, thread the output.
      if ( poi )
      {
        this->Wheels[1] = totalPoints; //end the output cell array after 1 cell
        GeneratePOITile(this->PointOfInterest,this->Wheels,threadMap,
                        pts,z,connectivityPtr,offsetsPtr,
                        scalars,this->ScalarMode,this->RegionIds);
      }
      else
      {
        // Parallel copy the Voronoi-related local thread data (points, cells,
        // scalars) into the filter output.
        ProduceVoronoiOutput vorOutput(this->Batcher,this->Wheels,threadMap,pts,z,
                                       connectivityPtr,offsetsPtr,scalars,this->ScalarMode,
                                       this->RegionIds);
        vtkSMPTools::For(0,this->NumThreadsUsed, vorOutput);
      }

      // Terminate the output offset array
      offsetsPtr[totalTiles] = totalPoints;
    } // If Voronoi tiles output desired

    // Composite the Delaunay info if requested. For each input generation
    // point, create a "wheel" of circumferentially ordered edge spokes. The
    // spokes are placed in a contiguous array with the wheel offset
    // referring to the start of each group of spokes associated with the
    // generating ptId.
    if ( !poi && (outputType == vtkVoronoi2D::DELAUNAY ||
                  outputType == vtkVoronoi2D::VORONOI_AND_DELAUNAY) )
    {
      this->Spokes.resize(this->NumSpokes);

      // Parallel build the Delaunay-related (wheel and spokes) structure.
      ProduceDelaunayWheels delWheels(this->Batcher,threadMap,
                                      this->Wheels,this->Spokes);
      vtkSMPTools::For(0,this->NumThreadsUsed, delWheels);
    }//if Delaunay output desired
  }

  // A factory method to conveniently instantiate and execute the algorithm.
  static int Execute(vtkStaticPointLocator2D *loc, vtkIdType numPts, double *points,
                     vtkDataArray *regionIds, double padding, vtkPolyData *output, int sMode,
                     vtkPolyData *delOutput, vtkIdType pointOfInterest, vtkIdType maxClips,
                     int &maxSides, vtkVoronoi2D* filter);

  // Given two wheel ids, determine whether a valid edge connects the wheels.
  // This assumes that the wheel&spokes data structure have been built.
  bool IsValidSpoke(vtkIdType w0Id, vtkIdType w1Id)
  {
    vtkIdType numSpokes = GetNumberOfSpokes(this->Wheels,w0Id);
    Spoke* spokes = &(this->Spokes[this->Wheels[w0Id]]);

    for (vtkIdType i=0; i < numSpokes; ++i, ++spokes)
    {
      if (spokes->Id == w1Id && spokes->Classification == Spoke::VALID)
      {
        return true;
      }
    } // for all spokes in this wheel
    return false;
  }

}; // VoronoiTiles

// Gather spokes into a wheel. Define some basic operators.  Note that every
// wheel is associated with an input (tile generating) point. So access to
// the wheel and its associated spokes is via point id.
struct Wheel
{
  vtkIdType Id; // The associated point/tile id: so wheelId == pointId
  int NumSpokes; // The number of emanating spokes
  Spoke* Spokes; // A pointer to an ordered array of spokes connected to this wheel

  // Default instantiation.
  Wheel() : Id(0), NumSpokes(0), Spokes(nullptr) {}
  // Instantiate a wheel given a point id.
  Wheel(VoronoiTiles *vt, vtkIdType id) : Id(id)
  {
    this->NumSpokes = GetNumberOfSpokes(vt->Wheels,id);
    this->Spokes = &vt->Spokes[vt->Wheels[id]];
  }
  // Setup the wheel for queries: an efficient form that does not require
  // wheel instantiation.
  void Initialize(VoronoiTiles *vt, vtkIdType id)
  {
    this->Id = id;
    this->NumSpokes = GetNumberOfSpokes(vt->Wheels,id);
    this->Spokes = &vt->Spokes[vt->Wheels[id]];
  }
  // Internal method to return the previous spoke position (clockwise).
  // Ensure that spokeNum < NumSpokes.
  int _Previous(int spokeNum)
  {
    return (spokeNum == 0 ? this->NumSpokes-1 : spokeNum-1);
  }
  // Internal method to return the next spoke position (counterclockwise).
  // Ensure that spokeNum < NumSpokes.
  int _Next(int spokeNum)
  {
    return (spokeNum == this->NumSpokes-1 ? 0 : spokeNum+1);
  }
  // Given a spoke number, return the spoke
  Spoke* GetSpoke(int spokeNum)
  {
    return this->Spokes + spokeNum;
  }

  // Given the connected wheel wInId, return the previous valid edge (wOutId)
  // in clockwise order to the incoming edge (wInId,this->Id). If no valid edge is
  // returned, return <0. To be clear, the returned value wOutId implicitly
  // defines the previous edge (in the clockwise direction)
  // (this->Id,wOudId).  Also, the method ensures that an edge with VALID
  // classification is returned.
  int GetPreviousValidEdge(vtkIdType wInId)
  {
    // Search for incoming spoke
    Spoke *spoke = this->Spokes;
    int i;
    for ( i=0; i < this->NumSpokes; ++i, ++spoke )
    {
      if ( spoke->Id == wInId )
      {
        break;
      }
    }
    if ( i > this->NumSpokes )
    {
      return -1;
    }
    int prev = this->_Previous(i);
    spoke = this->GetSpoke(prev);
    if ( spoke->Classification != Spoke::VALID )
    {
      return (-1);
    }
    else
    {
      return spoke->Id;
    }
  }
  // Return the previous spoke (i.e., in the clockwise direction). Make sure
  // that at least two spokes are available in the wheel.
  Spoke* Previous(int spokeNum)
  {
    int prev = this->_Previous(spokeNum);
    return this->Spokes + prev;
  }
  // Return the next spoke (i.e., in the counterclockwise direction). Make
  // sure that at least two spokes are available in the wheel.
  Spoke* Next(int spokeNum)
  {
    int next = this->_Next(spokeNum);
    return this->Spokes + next;
  }
};

// An optional class for producing the Delaunay triangulation. Basically
// performs initial topological checks and potential corrections, then
// produces triangles from the Voronoi tessellation using the wheels
// and spokes edge structure.
using LoopType = std::vector<vtkIdType>; //list of ids forming triangles
struct Delaunay
{
  VoronoiTiles *VTiles;
  Delaunay(VoronoiTiles *vt) : VTiles(vt) {}

  // Determine whether one or more valid triangles can be formed given a
  // wheel and two adjacent spokes.  This requires checking classification
  // and connectivity between wheels. Note, if more than one triangle can
  // be created, then the loop parameter contains a (convex) loop of points
  // that require triangulation.
  static int
  FormsTriangles(VoronoiTiles* vt, Wheel* inWheel, Spoke* currentSpoke,
                 Spoke* nextSpoke, LoopType& loop)
  {
    // Cull out invalid loops. A loop is valid, and can be processed, if all
    // the wheel/point ids are greater than the current wheel id (this
    // prevents processing the same loop more than once). Also, the initial
    // loop edges must be valid.
    if ( inWheel->Id > currentSpoke->Id || inWheel->Id > nextSpoke->Id ||
         currentSpoke->Classification != Spoke::VALID ||
         nextSpoke->Classification != Spoke::VALID )
    {
      return 0;
    }

    // Let's see if we have a triangle; if not we have a co-circular Delaunay
    // degeneracy, so we'll try to build a valid loop.
    if ( vt->IsValidSpoke(currentSpoke->Id,nextSpoke->Id) )
    {
      return 1; // return a single triangle
    }

    // Let's see if a valid loop can be formed. We start with the
    // two edge segments (currentSpoke->Id,inWheel->Id,nextSpoke->Id) and see
    // if we can form a valid, counterclockwise loop. This means linking
    // edges in counterclockwise CCW order.
    vtkIdType v0=inWheel->Id, v1=currentSpoke->Id;
    vtkIdType nextV, loopEnd=nextSpoke->Id;
    loop.resize(0);
    loop.emplace_back(v0);
    loop.emplace_back(v1);
    Wheel wheel(vt,v1);
    while ( (nextV = wheel.GetPreviousValidEdge(v0)) >= 0 &&
            nextV > inWheel->Id )
    {
      loop.emplace_back(nextV);
      if ( nextV == loopEnd )
      {
        // Successfully traversed a valid loop, return the number
        // of triangles.
        return (loop.size() - 2);
      }
      else
      {
        v0 = v1;
        v1 = nextV;
      }
      // Move to the next wheel
      wheel.Initialize(vt,nextV);
    } // while still traversing a valid loop

    return 0; // nothing to see here
  }

  // Edge classification via SMPTools. Each wheel with center wheelId is
  // processed, spokes/edges (wheelId,ptId) with (wheelId<ptId) are
  // classified according to simple topological rules. (Ensuring that wheelId
  // is minimum (whellId<ptId) elimates repeating edge classification
  // process.) The end result is a edge graph (represented by classified
  // wheel and spokes edge data structure) which can be triangulated. The
  // spokes have a classification, typically VALID or BOUNDARY, but in some
  // cases topological issues may cause spokes to be classified
  // differently. Note that for maximum performance, spokes are never
  // deleted, they are just classified - which means when processing spokes,
  // inspecting the spoke classification may be needed to traverse the
  // wheel/spoke network properly.
  struct ClassifyEdges
  {
    VoronoiTiles *VT;
    bool AllValid;
    ClassifyEdges(VoronoiTiles *vt) : VT(vt), AllValid(false) {}

    // Determine whether a spoke is used by both wheels. Proper edges must
    // be bidirectional and not extend into the boundary. Singleton edges are
    // problematic as well (when building loops later).
    bool IsBiDirectional(vtkIdType wheelId, Wheel& neighborWheel)
    {
      Spoke *spoke = neighborWheel.Spokes;
      for (int i=0; i < neighborWheel.NumSpokes; ++i, ++spoke)
      {
        if ( spoke->Id == wheelId )
        {
          return true;
        }
      }
      return false;
    }

    // Need a dummy Initialize() method to ensure Reduce() is invoked.
    void Initialize()
    {
    }

    void  operator()(vtkIdType wheelId, vtkIdType endWheelId)
    {
      int spokeNum;
      Wheel wheel, neighborWheel;
      Spoke *spoke;

      for ( ; wheelId < endWheelId; ++wheelId )
      {
        wheel.Initialize(this->VT,wheelId);
        spoke = wheel.Spokes;
        for (spokeNum=0; spokeNum < wheel.NumSpokes; ++spokeNum, ++spoke)
        {
          // Ensure processing of this edge only once. The current spoke
          // (v0,v1) must be v0<v1, with v1 non-boundary.
          if ( spoke->Id < 0 )
          {
            spoke->Classification = Spoke::BOUNDARY;
          }
          else
          {
            neighborWheel.Initialize(this->VT,spoke->Id);
            // Visit this spoke only once when (v0<v1)
            if ( wheelId < spoke->Id )
            {
              // Check for unidirectional degeneracy or singleton edge.
              if ( ! this->IsBiDirectional(wheelId, neighborWheel) )
              { // Bidirectional connected if (v0->v1) and (v1->v0)
                spoke->Classification = Spoke::DEGENERATE;
              }
              else if ( wheel.NumSpokes == 1 || neighborWheel.NumSpokes == 1)
              {
                spoke->Classification = Spoke::SINGLETON;
              }
            }
          }
        }//over all spokes for this wheel
      }//for all wheels
    }

    // Roll up validation
    void Reduce()
    {
      this->AllValid = true;
    }
  };

  // This ensure that there are no topological "bubbles" i.e., overlapping
  // portions of the mesh. Edges that fail the butterfly test are classified
  // "INTERSECTING" because the edge intersects other potentially valid
  // edges. Note that when this functor executes, everything but VALID edges
  // are excluded from the test.
  struct ButterflyTest
  {
    VoronoiTiles *VT;
    ButterflyTest(VoronoiTiles *vt) : VT(vt) {}

    void  operator()(vtkIdType wheelId, vtkIdType endWheelId)
    {
      int spokeNum, inSpokeNum, outSpokeNum, connectedSpokeNum;
      Wheel wheel, wNext;
      Spoke *spoke;
      vtkIdType w1Id, wNextId;
      int direction=0; //-1==CCW,1=CW
      bool boundaryEdge;

      for ( ; wheelId < endWheelId; ++wheelId )
      {
        wheel.Initialize(this->VT,wheelId);
        spoke = wheel.Spokes;
        for (spokeNum=0; spokeNum < wheel.NumSpokes; ++spokeNum, ++spoke)
        {

        }//over all spokes
      }//over wheels
    }
  };

  // (See GenerateTriangles() below.) This method counts the number of output
  // triangles generated by each wheel, generating an offset array so that
  // that GenerateTriangles() can later parallel write into the output
  // vtkCellArray containing the triangles. Note: depending on whether we
  // have to watch out for non-valid edges, different wheel traversal methods
  // are used (for performance).
  struct CountTriangles
  {
    VoronoiTiles *VT;
    vtkIdType *WIdx;
    vtkIdType NumTriangles;
    bool AllValid;

    // Used separately be each loop, avoid repeated allocations
    vtkSMPThreadLocal<LoopType> Loop;

    CountTriangles(VoronoiTiles *vt, vtkIdType *widx, bool allValid) :
      VT(vt), WIdx(widx), NumTriangles(0), AllValid(allValid) {}

    void Initialize()
    {
      this->Loop.Local().reserve(32);
    }

    void  operator()(vtkIdType wheelId, vtkIdType endWheelId)
    {
      Wheel wheel;
      int spokeNum;
      Spoke *spoke, *spokeNext;
      int numTris;
      vtkIdType *widx=this->WIdx;
      LoopType &loop = this->Loop.Local();

      for ( ; wheelId < endWheelId; ++wheelId )
      {
        numTris = 0;
        wheel.Initialize(this->VT,wheelId);
        if ( wheel.NumSpokes <= 1 )
        {
          continue; //no triangles can be created with one or fewer spokes
        }

        // Run around spokes, counting triangles generated by each wheel
        spoke = wheel.Spokes;
        for (spokeNum=0; spokeNum < wheel.NumSpokes; ++spokeNum, spoke=spokeNext)
        {
          spokeNext = wheel.Next(spokeNum);
          numTris += Delaunay::FormsTriangles(this->VT,&wheel,spoke,spokeNext,loop);
        } // over all spokes in the current wheel
        widx[wheelId] = numTris;
      } // over allwheels
    } // operator()

    // Perform prefix sum over number of triangles to determine allocation size,
    // and positions to place triangles.
    void Reduce()
    {
      vtkIdType numWheels = this->VT->NPts;
      vtkIdType totalTris = 0;
      vtkIdType numTris;
      vtkIdType *widx = this->WIdx;

      for (auto wheelId=0; wheelId < numWheels; ++wheelId)
      {
        numTris = widx[wheelId];
        widx[wheelId] = totalTris; //three points per triangle
        totalTris += numTris;
      }
      // Record the summation of triangles over all wheels.
      this->WIdx[numWheels] = totalTris;
      this->NumTriangles = totalTris;
    }
  };

  // Triangle generation via SMPTools. The classified wheel and spokes data
  // structure (a graph) is triangulated. Typically the graph consists of
  // mostly 3-edge subloops which are trivially triangulated. Larger loops
  // (corresponding to co-circular degeneracies in the Delaunay
  // triangulation) require a bit more work. Note that only loops
  // (wheelId,ptId0,ptId1,...) are processed when (wheelId<ptId0,
  // wheelId<ptId1,...). (Ensuring that wheelId is the minimum id in the loop
  // elimates duplicate work (i.e., avoids processing the same loop multiple times.)
  // Note: depending on whether we have to watch out for non-valid edges,
  // different wheel traversal methods are used (for performance).
  struct GenerateTriangles
  {
    VoronoiTiles *VT;
    vtkIdType *WIdx;
    vtkIdType *Offsets;
    vtkIdType *Connectivity;
    bool AllValid;
    // Used separately be each loop, avoid repeated allocations
    vtkSMPThreadLocal<LoopType> Loop;

    GenerateTriangles(VoronoiTiles *vt, vtkIdType *widx, vtkIdType *offsets,
                      vtkIdType *conn, bool allValid) :
      VT(vt), WIdx(widx), Offsets(offsets), Connectivity(conn), AllValid(allValid) {}

    void Initialize()
    {
      this->Loop.Local().reserve(32);
    }

    void  operator()(vtkIdType wheelId, vtkIdType endWheelId)
    {
      Wheel wheel;
      int spokeNum;
      Spoke *spoke, *spokeNext;
      int numTris;
      vtkIdType *widx=this->WIdx;
      vtkIdType *offsets=this->Offsets, *o, offset;
      vtkIdType *conn=this->Connectivity, *c;
      LoopType &loop = this->Loop.Local();

      for ( ; wheelId < endWheelId; ++wheelId )
      {
        // Check to see if this wheel generates triangles
        if ( (widx[wheelId+1] - widx[wheelId]) > 0 )
        {
          wheel.Initialize(this->VT,wheelId);
          if ( wheel.NumSpokes <= 1 )
          {
            continue; //no triangles can be created with one or fewer spokes
          }

          // Run around spokes, counting triangles generated by each wheel
          offset = 3*widx[wheelId];
          o = offsets + widx[wheelId];
          c = conn + offset;
          spoke = wheel.Spokes;
          for (spokeNum=0; spokeNum < wheel.NumSpokes; ++spokeNum, spoke=spokeNext)
          {
            spokeNext = wheel.Next(spokeNum);
            if ( (numTris=Delaunay::FormsTriangles(this->VT,&wheel,spoke,spokeNext,loop)) > 0)
            {
              if ( numTris == 1 ) // Generate a single triangle quickly
              {
                *c++ = wheelId;
                *c++ = spoke->Id;
                *c++ = spokeNext->Id;
                *o++ = offset;
                offset += 3;
              }
              else // multiple triangles formed, need to tessellate convex loop
              {
                for (auto tri=0; tri < numTris; ++tri)
                {
                  // Just use a fan triangulation since any triangulation is
                  // of the same quality (since this is a degenerate Delaunay
                  // co-circular loop).
                  *c++ = loop[0];
                  *c++ = loop[tri+1];
                  *c++ = loop[tri+2];
                  *o++ = offset;
                  offset += 3;
                }
              }
            }
          } // over all spokes in the current wheel
        } // if triangles are generated in this wheel
      } // over all wheels
    } // operator()

  void Reduce() {}
  }; // GenerateTriangles

  // Generate the Delaunay triangulation from the Voronoi tessellation.
  static void Execute(VoronoiTiles *vt)
  {
    // Classify edges. We process one wheel at a time.
    vtkIdType numWheels = vt->NPts;

    // At this point in the algorithm, spokes are classified as either VALID
    // or BOUNDARY. If the user chooses, an additional topologcal analysis
    // can be invoked to further classify the spokes around each wheel. If no
    // DEGENERATE or INTERSECTION spokes are found, then a faster Delaunay
    // process is used; otherwise a slower approach is used (i.e., generating
    // the output Delaunay when all spokes are either VALID or BOUNDARY is
    // faster than when DEGENERATE or INTERSECTION spokes exist.)
    bool allValid = !vt->Filter->GetValidate();
    if ( !allValid )
    {
      Delaunay::ClassifyEdges classify(vt);
      vtkSMPTools::For(0, numWheels, classify);
      allValid = classify.AllValid;
      //        Delaunay::ButterflyTest butterfly(vt);
      //        vtkSMPTools::For(0, numWheels, butterfly);
      //      Delaunay::MakeValid makeValid(vt);
    }

    // Generate triangles by processing the Voronoi spoke/wheel graph. We
    // process one wheel at a time. First we need to count the number of
    // output triangles, and then the triangles are actually generated
    // (threaded execution). Note that all edges classified VALID and
    // BOUNDARY are processed, all other classifications are ignored.
    // The widx (wheel idx) is used as the thread partitioning array,
    std::vector<vtkIdType> widx(numWheels+1);
    Delaunay::CountTriangles count(vt,widx.data(),allValid);
    vtkSMPTools::For(0, numWheels, count);
    vtkIdType numTris = count.NumTriangles;

    // Compute the internal vtkCellArray offset and connectivity arrays.
    vtkNew<vtkIdTypeArray> offsets;
    offsets->SetNumberOfTuples(numTris+1);
    vtkIdType *offsetsPtr = offsets->GetPointer(0);
    vtkNew<vtkIdTypeArray> connectivity;
    connectivity->SetNumberOfTuples(numTris*3);
    vtkIdType *connectivityPtr = connectivity->GetPointer(0);
    Delaunay::GenerateTriangles generate(vt,widx.data(),offsetsPtr,connectivityPtr,allValid);
    vtkSMPTools::For(0, numWheels, generate);

    // Now populate the Delaunay triangles array
    offsetsPtr[numTris] = 3*numTris;
    vt->DelTris->SetData(offsets,connectivity);

  } //Execute()

}; //Delaunay


  // Implementation of a factory method to conveniently instantiate and execute
  // the Voronoi and Delaunay algorithms.
  int VoronoiTiles::
  Execute(vtkStaticPointLocator2D *loc, vtkIdType numPts, double *points,
          vtkDataArray *regionIds, double padding, vtkPolyData *output, int sMode,
          vtkPolyData *delOutput, vtkIdType pointOfInterest, vtkIdType maxClips,
          int &maxSides, vtkVoronoi2D* filter)
  {
    // Set up batch processing: either all of the points, or a point of
    // interest.
    BatchManager batcher(numPts,filter->GetBatchSize());
    if ( pointOfInterest >= 0 && pointOfInterest < numPts)
    {
      batcher.SetSubRange(pointOfInterest,pointOfInterest+1);
    }

    // Generate the Voronoi tessellation
    VoronoiTiles vt(batcher, numPts, points, pointOfInterest, regionIds,
                    loc, padding, output, sMode, delOutput, maxClips, filter);
    vtkSMPTools::For(0,batcher.GetNumberOfBatches(), vt);
    maxSides = vt.MaxSides;

    // If requested, generate the Delaunay triangulation
    if ( delOutput != nullptr && (pointOfInterest < 0 || pointOfInterest > numPts) )
    {
      Delaunay::Execute(&vt);
    }

    // Okay get out
    return vt.NumThreadsUsed;
  }

} // anonymous namespace


//================= Begin VTK class proper =====================================
//------------------------------------------------------------------------------
// Construct object
vtkVoronoi2D::vtkVoronoi2D()
{
  this->OutputType = VORONOI; // Voronoi tessellation placed in output 0
  this->Validate = true;
  this->PassPointData = true;
  this->GenerateScalars = NONE;
  this->Padding = 0.01;
  this->Locator = vtkSmartPointer<vtkStaticPointLocator2D>::New();
  this->Locator->SetNumberOfPointsPerBucket(2);
  this->Transform = nullptr;
  this->ProjectionPlaneMode = XY_PLANE;
  this->PointOfInterest = (-1);
  this->MaximumNumberOfTileClips = VTK_ID_MAX;
  this->GenerateVoronoiFlower = false;
  this->Spheres = vtkSmartPointer<vtkSpheres>::New();
  this->BatchSize = 1000;
  this->PruneSpokes = false;
  this->PruneTolerance = 0.001;
  this->MaximumNumberOfSides = 0;
  this->NumberOfThreadsUsed = 0;

  // Optional outputs 2-4 for 2) Delaunay triangulation, 3) Voronoi flower,
  // and 4) Voronoi tile at point of interest
  this->SetNumberOfOutputPorts(4);

  // By default process active point scalars to obtain region ids
  this->SetInputArrayToProcess(
                               0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
}

//------------------------------------------------------------------------------
int vtkVoronoi2D::RequestData(vtkInformation* vtkNotUsed(request),
                              vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the input and output
  vtkPointSet* input = vtkPointSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  vtkDebugMacro(<< "Generating 2D Voronoi Tessellation");

  // Initialize; check input
  vtkIdType pId, numPts;
  vtkPoints* inPoints;
  if ((inPoints = input->GetPoints()) == nullptr || (numPts = inPoints->GetNumberOfPoints()) < 1)
  {
    vtkDebugMacro("Cannot tessellate; need at least 1 input point");
    return 1;
  }

  // If the user specified a transform, apply it to the input data.
  // Only the input points are transformed. Note points are always
  // converted to double.
  vtkSmartPointer<vtkPoints> tPoints;
  if (this->Transform)
  {
    tPoints = vtkSmartPointer<vtkPoints>::New();
    tPoints->SetDataTypeToDouble();
    this->Transform->TransformPoints(inPoints, tPoints);
  }
  else if (this->ProjectionPlaneMode == VTK_BEST_FITTING_PLANE)
  {
    // If the user asked this filter to compute the best fitting plane,
    // proceed to compute the plane and generate a transform that will
    // map the input points into that plane.
    this->SetTransform(vtkDelaunay2D::ComputeBestFittingPlane(input));
    tPoints = vtkSmartPointer<vtkPoints>::New();
    tPoints->SetDataTypeToDouble();
    this->Transform->TransformPoints(inPoints, tPoints);
  }
  else if (inPoints->GetDataType() == VTK_DOUBLE)
  { // fast path no conversion
    tPoints = inPoints;
  }
  else
  { // convert points to double
    tPoints = vtkSmartPointer<vtkPoints>::New();
    tPoints->SetDataTypeToDouble();
    tPoints->SetNumberOfPoints(numPts);
    for (pId = 0; pId < numPts; ++pId)
    {
      tPoints->SetPoint(pId, inPoints->GetPoint(pId));
    }
  }

  // Temporary data object holds points to be tessellated
  vtkNew<vtkPolyData> tInput;
  tInput->SetPoints(tPoints);

  // If auxiliary scalars are to be generated, create the
  // scalars now. This affects both the Voronoi and Delaunay output.
  vtkSmartPointer<vtkIdTypeArray> auxScalars;
  if (this->GenerateScalars != NONE)
  {
    auxScalars = vtkSmartPointer<vtkIdTypeArray>::New();
    auxScalars->SetNumberOfComponents(1);
    auxScalars->SetName("VoronoiScalars");
  }

  // Construct the Voronoi output (if requested). This is
  // output #0
  vtkNew<vtkPoints> newPts; // Used by PointIfInterest
  vtkNew<vtkCellArray> tiles;
  if ( this->OutputType == VORONOI ||
       this->OutputType == VORONOI_AND_DELAUNAY )
  {
    newPts->SetDataTypeToDouble();
    output->SetPoints(newPts);
    output->SetPolys(tiles);
    if ( this->PassPointData )
    {
      output->GetCellData()->PassData(input->GetPointData());
    }
    if ( auxScalars )
    {
      int idx = output->GetCellData()->AddArray(auxScalars);
      output->GetCellData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
    }
  }

  // A locator is used to locate closest points.
  if (!this->Locator)
  {
    vtkErrorMacro(<< "Point locator required\n");
    return 0;
  }
  this->Locator->SetDataSet(tInput);
  this->Locator->BuildLocator();

  // Computational bounds
  double length = tInput->GetLength();
  double padding = this->Padding * length;

  // Optional second output (output #1) the Delaunay triangulation if
  // requested.
  vtkPolyData *delOutput = nullptr;
  if ( this->OutputType == DELAUNAY ||
       this->OutputType == VORONOI_AND_DELAUNAY )
  {
    vtkInformation *outInfo2 = outputVector->GetInformationObject(1);
    delOutput = vtkPolyData::SafeDownCast(
                                          outInfo2->Get(vtkDataObject::DATA_OBJECT()));
    delOutput->SetPoints(tPoints);
    vtkNew<vtkCellArray> delTris;
    delOutput->SetPolys(delTris);
    if ( this->PassPointData )
    {
      delOutput->GetPointData()->PassData(input->GetPointData());
    }
    if ( auxScalars )
    {
      int idx = delOutput->GetPointData()->AddArray(auxScalars);
      delOutput->GetPointData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);
    }
  }

  // If region ids are provided, use these to control the Voronoi tessellation.
  vtkDataArray* regionIds = this->GetInputArrayToProcess(0, inputVector);
  if ( regionIds )
  {
    if ( regionIds->GetNumberOfComponents() > 1 )
    {
      vtkErrorMacro("Region Ids must have 1 component");
      regionIds = nullptr;
    }
  }

  // Process the points to generate Voronoi tiles and the optional
  // Delaunay triangulation.
  double* inPtr = static_cast<vtkDoubleArray*>(tPoints->GetData())->GetPointer(0);
  this->NumberOfThreadsUsed = VoronoiTiles::
    Execute(this->Locator, numPts, inPtr, regionIds, padding, output,
            this->GenerateScalars, delOutput, this->PointOfInterest,
            this->MaximumNumberOfTileClips, this->MaximumNumberOfSides,
            this);

  vtkDebugMacro(<< "Produced " << output->GetNumberOfCells() << " tiles and "
                << output->GetNumberOfPoints() << " points");

  if ( delOutput )
  {
    vtkDebugMacro(<<"Produced Delaunay triangulation with "
                  << delOutput->GetNumberOfCells() << " triangles and "
                  << delOutput->GetNumberOfPoints() << " points");
  }

  // If requested, generate in the third output a representation of the
  // Voronoi flower error metric for the PointOfInterest.
  if (!this->CheckAbort() && this->GenerateVoronoiFlower && this->PointOfInterest >= 0 &&
      this->PointOfInterest < numPts)
  {
    // Get the optional third and fourth outputs
    vtkInformation* outInfo3 = outputVector->GetInformationObject(2);
    vtkPolyData* output3 = vtkPolyData::SafeDownCast(outInfo3->Get(vtkDataObject::DATA_OBJECT()));

    // Populate a Voronoi tile with the output tile (PointOfIntersect). This
    // assumes a single convex polygon has been output.
    double bds[6], tileX[3], x[3], center[3], factor = 3.5;
    vtkIdType npts;
    const vtkIdType* p;
    output->GetBounds(bds);
    output->GetCenter(center);
    tiles->InitTraversal();
    tiles->GetNextCell(npts, p);
    VTile tile;
    tPoints->GetPoint(this->PointOfInterest, tileX);
    tile.Initialize(this->PointOfInterest, tileX, newPts, npts, p);

    // For now generate a zillion points and keep those that intersect the
    // tile.
    vtkNew<vtkPoints> fPts;
    fPts->SetDataTypeToDouble();
    vtkNew<vtkCellArray> fVerts;
    fVerts->InsertNextCell(1);
    vtkIdType i, pid;
    vtkNew<vtkMinimalStandardRandomSequence> random;
    random->Initialize(1177);
    for (i = 0, npts = 0; i < 1000000; ++i)
    {
      x[0] = random->GetNextRangeValue(center[0] + factor * (bds[0] - center[0]),
                                       center[0] + factor * (bds[1] - center[0]));
      x[1] = random->GetNextRangeValue(center[1] + factor * (bds[2] - center[1]),
                                       center[1] + factor * (bds[3] - center[1]));
      x[2] = 0.0;
      if (tile.IntersectTile(x))
      {
        pid = fPts->InsertNextPoint(x);
        fVerts->InsertCellPoint(pid);
        npts++;
      }
    }

    fVerts->UpdateCellCount(npts);
    output3->SetPoints(fPts);
    output3->SetVerts(fVerts);

    // Now update the vtkSpheres implicit function, and create a third output
    // that has the PointOfInterested-associated tile, with scalar values at
    // each point which are the radii of the error circles (and when taken
    // together form the Voronoi Flower).
    vtkInformation* outInfo4 = outputVector->GetInformationObject(3);
    vtkPolyData* output4 = vtkPolyData::SafeDownCast(outInfo4->Get(vtkDataObject::DATA_OBJECT()));

    vtkNew<vtkPoints> centers;
    centers->SetDataTypeToDouble();
    vtkNew<vtkCellArray> singleTile;
    vtkNew<vtkDoubleArray> radii;
    radii->SetName("Voronoi Flower Radii");

    output4->SetPoints(centers);
    output4->SetPolys(singleTile);
    output4->GetPointData()->SetScalars(radii);

    // Update polydata (third output)
    tile.PopulateTileData(centers, singleTile, radii);

    // Update implicit function
    this->Spheres->SetCenters(centers);
    this->Spheres->SetRadii(radii);
  }

  return 1;
}

//------------------------------------------------------------------------------
// This filter produces vtkPolyData
int vtkVoronoi2D::FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info)
{
  // now add our info
  info->Set(vtkDataObject::DATA_TYPE_NAME(), "vtkPolyData");
  return 1;
}

//------------------------------------------------------------------------------
// Since users have access to the locator we need to take into account the
// locator's modified time.
vtkMTimeType vtkVoronoi2D::GetMTime()
{
  vtkMTimeType mTime = this->vtkObject::GetMTime();
  vtkMTimeType time = this->Locator->GetMTime();
  return (time > mTime ? time : mTime);
}

//------------------------------------------------------------------------------
void vtkVoronoi2D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Output Type: " << this->OutputType << "\n";
  os << indent << "Validate: " << (this->Validate ? "On\n" : "Off\n");
  os << indent << "Pass Point Data: " << (this->PassPointData ? "On\n" : "Off\n");
  os << indent << "Generate Scalars: " << this->GenerateScalars << "\n";
  os << indent << "Padding: " << this->Padding << "\n";
  os << indent << "Locator: " << this->Locator << "\n";
  os << indent << "Projection Plane Mode: " << this->ProjectionPlaneMode << "\n";
  os << indent << "Transform: " << (this->Transform ? "specified" : "none") << "\n";
  os << indent << "Point Of Interest: " << this->PointOfInterest << "\n";
  os << indent << "Maximum Number Of Tile Clips: " << this->MaximumNumberOfTileClips << "\n";
  os << indent << "Generate Voronoi Flower: " << (this->GenerateVoronoiFlower ? "On\n" : "Off\n");
  os << indent << "Prune Spokes: " << (this->PruneSpokes ? "On\n" : "Off\n");
  os << indent << "Prune Tolerance: " << this->PruneTolerance << "\n";
  os << indent << "Batch Size: " << this->BatchSize << "\n";
}
VTK_ABI_NAMESPACE_END
