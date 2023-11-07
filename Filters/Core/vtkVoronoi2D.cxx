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

// Evaluate 2D line equation. Normal n is expected to be a unit normal. The
// point o is a point on the line (typically midpoint between two Voronoi
// points).
double EvaluateLine(double x[2], double o[2], double n[2])
{
  return ((x[0] - o[0]) * n[0] + (x[1] - o[1]) * n[1]);
}

// The data structure for representing a Voronoi tile vertex and implicitly,
// a Voronoi tile edge. The tile vertex has a position X, and current value
// of the half-space clipping function. In counterclockwise direction, the
// PointId refers to the point id in the neighboring tile that, together with
// this tile's point id, produced the edge.
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
  vtkIdType PointId;                // generating tile point id (in tile)
  double TileX[2];                  // generating tile point - x-y coordinates
  VertexRingType Verts;             // counterclockwise ordered loop of vertices
  VertexRingType NewVerts;          // accumulate new vertices to modify the tile
  vtkStaticPointLocator2D* Locator; // locator
  double PaddedBounds[4];           // the domain over which Voronoi is calculated
  double Bounds[4];                 // locator bounds
  int Divisions[2];                 // locator binning dimensions
  double H[2];                      // locator spacing

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
    // (-1) generating point id means that this point is on the boundary.
    double v[2], *bds = this->PaddedBounds;
    v[0] = bds[1];
    v[1] = bds[3];
    this->Verts.emplace_back(VVertex(v,(-1)));

    v[0] = bds[0];
    v[1] = bds[3];
    this->Verts.emplace_back(VVertex(v,(-1)));

    v[0] = bds[0];
    v[1] = bds[2];
    this->Verts.emplace_back(VVertex(v,(-1)));

    v[0] = bds[1];
    v[1] = bds[2];
    this->Verts.emplace_back(VVertex(v,(-1)));
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

    // Now for each of the points of the polygon, insert a vertex.
    double v[3];
    for (vtkIdType i = 0; i < nPts; ++i)
    {
      pts->GetPoint(p[i], v);
      this->Verts.emplace_back(VVertex(v,(-1)));
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
  void PopulatePolyData(vtkPoints* centers, vtkCellArray* tile, vtkDoubleArray* radii)
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

  // Generate a Voronoi tile by iterative clipping of the tile with nearby
  // points.  Termination of the clipping process occurs when the neighboring
  // points become "far enough" away from the generating point (i.e., the
  // Voronoi Flower error metric is satisfied).
  bool BuildTile(vtkIdList* pIds, vtkDoubleArray *radii2, const double* pts,
                 vtkIdType maxClips)
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
  vtkIdType Id; // Id of a spoke (wheelId,Id) that a wheel is connected to
  unsigned char Classification; // Indicate the classification of this spoke
  enum SpokeClass
  {
    VALID=0, // Valid edge, contributes to output mesh and/or graph
    INTERSECTING=1, // Intersecting edge, part of a topological bubble
    SINGLETON=2, // Edge with one or more end points used by only one wheel
    DEGENERATE=4, // Edge connects two points but in only one direction
    BOUNDARY=8 // Connected to the Voronoi boundary
  };
  Spoke() : Id(-1), Classification(VALID) {}
  Spoke(vtkIdType id) : Id(id), Classification(VALID) {}
};
// Typedefs related to edge spoke structure
using SpokerIterator = std::vector<Spoke>::iterator;

// Track local data on a per-thread basis. In the Reduce() method this
// information will be used to composite the data from each thread into a
// single vtkPolyData output.
struct LocalDataType
{
  vtkIdType NumberOfTiles;
  vtkIdType NumberOfPoints;
  int MaxSides;
  std::vector<vtkIdType> LocalPtIds; //generating point for each tile
  std::vector<vtkIdType> LocalTiles; //vertex indices for each tile
  std::vector<TileVertex> LocalPoints; //x-y coordinates for each tile point
  std::vector<Spoke> LocalSpokes;  //connecting edges/spokes for each tile
  VTile Tile;
  vtkIdType Offset;

  LocalDataType()
    : NumberOfTiles(0)
    , NumberOfPoints(0)
    , MaxSides(0)
    , Offset(0)
  {
    this->LocalPtIds.reserve(2048);
    this->LocalTiles.reserve(2048);
    this->LocalPoints.reserve(2048);
    this->LocalSpokes.reserve(2048);
  }
};

// The threaded core of the algorithm. This could be templated over point
// type, but due to numerical sensitivity we'll just do double for now.
struct VoronoiTiles
{
  const double *Points; //Input points
  vtkIdType NPts; //The number of input (Voronoi generation) points
  vtkStaticPointLocator2D *Locator; // Used to (quickly) find nearby points
  double PaddedBounds[4]; //the expanded domain over which Voronoi is calculated
  double Bounds[4]; //locator bounds
  int Divisions[2]; //Internal parameters for computing
  double H[2]; //Locator bin spacing
  double BucketRadius; //The circle radius enclosing a locator bin
  vtkPoints *NewPoints;//New Voronoi points generated
  std::vector<vtkIdType> NumPts; //Array of (number of points) for each tile
  vtkCellArray *Tiles; //Actual output convex polygons
  int ScalarMode;//How to compute scalars
  vtkIdTypeArray *Scalars; //Scalars if requested
  vtkIdType MaxClips; //Control the maximum number of half-space clips
  int NumThreadsUsed; //Keep track of the number of threads used

  // This are used to create the spokes and wheels graph used to validate
  // the tessllation and produce a Delaunay triangulation.
  int MaxSides; //Maximum number of sides found in a generated Voronoi tile
  std::vector<vtkIdType> Wheels; //Wheel/spokes data structure: offset array to spokes
  vtkIdType NumSpokes; //Total number of edges / spokes
  std::vector<Spoke> Spokes; //Spokes / edges with classification
  vtkCellArray *DelTris; //If requested, triangles are placed here for Delaunay

  // Used for controlling filter abort
  vtkVoronoi2D* Filter;

  // Storage local to each thread. We don't want to allocate working arrays
  // on every thread invocation. Thread local storage saves lots of
  // new/delete (e.g. the PIds).
  vtkSMPThreadLocalObject<vtkIdList> PIds;
  vtkSMPThreadLocalObject<vtkDoubleArray> Radii2;
  vtkSMPThreadLocal<LocalDataType> LocalData;

  VoronoiTiles(vtkIdType npts, double* points, vtkStaticPointLocator2D* loc, double padding,
               vtkPolyData* output, int scalarMode, vtkPolyData *delOutput, vtkIdType maxClips,
               vtkVoronoi2D* filter)
    : Points(points)
    , NPts(npts)
    , Locator(loc)
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
    this->NumPts.resize(this->NPts,0); //initialized to zero
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
  }

  void operator()(vtkIdType ptId, vtkIdType endPtId)
  {
    vtkIdList*& pIds = this->PIds.Local();
    vtkDoubleArray*& radii2 = this->Radii2.Local();
    LocalDataType& localData = this->LocalData.Local();
    vtkIdType& numTiles = localData.NumberOfTiles;
    vtkIdType& numPoints = localData.NumberOfPoints;
    int &maxSides = localData.MaxSides;
    std::vector<vtkIdType>& lPtIds = localData.LocalPtIds;
    std::vector<vtkIdType>& lTiles = localData.LocalTiles;
    std::vector<TileVertex>& lPoints = localData.LocalPoints;
    std::vector<Spoke>& lSpokes = localData.LocalSpokes;
    VTile& tile = localData.Tile;
    const double* x = this->Points + 3 * ptId;
    bool isFirst = vtkSMPTools::GetSingleThread();
    vtkIdType checkAbortInterval = std::min((endPtId - ptId) / 10 + 1, (vtkIdType)1000);

    for (; ptId < endPtId; ++ptId, x += 3)
    {
      if (ptId % checkAbortInterval == 0)
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

      // Initialize the Voronoi tile
      tile.Initialize(ptId, x);

      // If tile is successfully built, copy the convex tile polygon and
      // points it to thread local storage.
      int maxClips = ( this->MaxClips < this->NPts ? this->MaxClips :
                       (this->NPts > 1 ? (this->NPts-1) : 0) );
      if (tile.BuildTile(pIds, radii2, this->Points, maxClips))
      {
        // Now accumulate the tile / convex polygon in this thread
        lPtIds.emplace_back(ptId);
        vtkIdType i, nPts = static_cast<vtkIdType>(tile.Verts.size());
        this->NumPts[ptId] = nPts;
        maxSides = (nPts > maxSides ? nPts : maxSides);

        // Now accumulate the tile indices and points for this Voronoi tile
        for (i = 0; i < nPts; ++i)
        {
          lTiles.emplace_back((i+numPoints));
          lPoints.emplace_back(TileVertex(tile.Verts[i].X[0],tile.Verts[i].X[1]));
        }

        // If a Delaunay triangulation is requested, gather information for
        // later processing. The spoke edges are used to build the triangulation.
        if ( this->DelTris != nullptr )
        {
          for (i=0; i < nPts; ++i)
          {
            lSpokes.emplace_back(Spoke(tile.Verts[i].PointId));
          }
        }

        numTiles++;
        numPoints += nPts;
      } // if tile built
    }   // for all points
  }

  void Reduce()
  {
    // Count the total number of cells and connectivity storage required,
    // plus the number of points and optionally the number of spokes. Keep
    // track of the point id offset to update the cell connectivity list. This
    // will create the Voronoi output and/or Delaunay output.
    // TODO: Parallelize the copying of thread output into the Voronoi and
    // Delaunay output. Basically create an indexing array so that the
    // output of each local thread is written to a prescriped position in the
    // filter's output.
    vtkIdType totalTiles = 0;
    vtkIdType connSize = 0;
    vtkIdType totalPoints = 0;
    vtkIdType offset = totalPoints;
    this->NumThreadsUsed = 0;
    this->NumSpokes = 0;
    this->MaxSides = 0;
    vtkSMPThreadLocal<LocalDataType>::iterator ldItr;
    vtkSMPThreadLocal<LocalDataType>::iterator ldEnd = this->LocalData.end();
    for ( auto& localData : this->LocalData)
    {
      totalTiles += localData.NumberOfTiles;
      connSize += static_cast<vtkIdType>(localData.LocalTiles.size());
      totalPoints += localData.NumberOfPoints;
      this->NumSpokes += localData.LocalSpokes.size();
      localData.Offset = offset;
      offset = totalPoints;
      this->NumThreadsUsed++;
      this->MaxSides = ( localData.MaxSides > this->MaxSides ?
                         localData.MaxSides : this->MaxSides);
    } // loop over local thread output

    // If Voronoi output is requested, produce the output convex polygons
    // and associated points.
    int outputType = this->Filter->GetOutputType();
    if ( outputType == vtkVoronoi2D::VORONOI ||
         outputType == vtkVoronoi2D::VORONOI_AND_DELAUNAY )
    {
      // Now copy the data: points and cell connectivity. Points are placed in
      // the x-y plane. Cell connectivities must be updated with new point
      // offsets to reference the correct global point id. Note that the order
      // of the points and cells is somewhat random depending on when the
      // various threads process the input data.
      const double z = this->Points[2];
      this->NewPoints->SetNumberOfPoints(totalPoints);
      double *pts = static_cast<vtkDoubleArray*>(this->NewPoints->GetData())->GetPointer(0);
      std::vector<TileVertex>::iterator tvItr, tvEnd;
      std::vector<vtkIdType>::iterator pItr, pEnd;

      // Structures for cell definitions. Directly create the offsets
      // and connectivity for efficiency.
      vtkNew<vtkIdTypeArray> offsets;
      offsets->SetNumberOfTuples(totalTiles+1);
      vtkIdType *offsetsPtr = offsets->GetPointer(0);
      vtkNew<vtkIdTypeArray> connectivity;
      connectivity->SetNumberOfTuples(connSize);
      vtkIdType *connectivityPtr = connectivity->GetPointer(0);
      std::vector<vtkIdType>::iterator cItr;
      this->Tiles->SetData(offsets, connectivity);

      // If scalars requested, allocate them
      vtkIdType *scalars = nullptr;
      if ( this->Scalars )
      {
        this->Scalars->SetNumberOfTuples(totalTiles);
        scalars = this->Scalars->GetPointer(0);
      }

      // Produce the primary (Voronoi) output. Traverse each tile and
      // copy local data into the filter output.
      totalPoints = 0;
      offset = 0;
      vtkIdType threadId=0;
      for ( auto& localData : this->LocalData)
      {
        // Copy all the points from this thread
        tvEnd = localData.LocalPoints.end();
        for ( tvItr = localData.LocalPoints.begin(); tvItr != tvEnd; ++tvItr )
        {
          *pts++ = (*tvItr).X;
          *pts++ = (*tvItr).Y;
          *pts++ = z;
        }

        // Copy tiles into cell array: define the offsets and connectivity
        // arrays.  Use the more efficient vtkCellArray::SetData() method.
        cItr = localData.LocalTiles.begin();
        pEnd = localData.LocalPtIds.end();
        for ( pItr = localData.LocalPtIds.begin(); pItr != pEnd; )
        {
          // Offsets
          *offsetsPtr++ = offset;
          vtkIdType nPts = this->NumPts[*pItr++];
          offset += nPts;
          // Connectivity
          for (vtkIdType i=0; i < nPts; ++i)
          {
            *connectivityPtr++ = totalPoints + *cItr++;
          }
        }

        // Copy cell scalars if requested
        if ( scalars != nullptr )
        {
          // If requested, thread id
          if ( this->ScalarMode == vtkVoronoi2D::THREAD_IDS )
          {
            std::size_t j, nCells = localData.NumberOfTiles;
            for (j=0; j < nCells; ++j)
            {
              *scalars++ = threadId;
            }
          }
          // the generating point id
          else if ( this->ScalarMode == vtkVoronoi2D::POINT_IDS )
          {
            pEnd = localData.LocalPtIds.end();
            for ( pItr = localData.LocalPtIds.begin(); pItr != pEnd; )
            {
              *scalars++ = *pItr++;
            }
          }
          // the number of sides of the voronoi tile
          else //if ( this->ScalarMode == vtkVoronoi2D::NUMBER_SIDES )
          {
            pEnd = localData.LocalPtIds.end();
            for ( pItr = localData.LocalPtIds.begin(); pItr != pEnd; )
            {
              *scalars++ = this->NumPts[*pItr++];
            }
          }
        }//Produce scalars

        threadId++;
        totalPoints += localData.NumberOfPoints;
      }//for each thread

      // Terminate offset array
      *offsetsPtr = offset;
    } // If Voronoi tiles output desired

    // Composite the Delaunay info if requested. For each input generation
    // point, create a "wheel" of circumferentially ordered edge spokes. The
    // spokes are placed in a contiguous array with the wheel offset
    // referring to the start of each group of spokes associated with the
    // generating ptId. To construct the spokes and wheels, accessible by
    // ptId, a prefix sum over the NumPts array is required to build an
    // ordered list of wheels (and associated spokes).
    if ( outputType == vtkVoronoi2D::DELAUNAY ||
         outputType == vtkVoronoi2D::VORONOI_AND_DELAUNAY )
    {
      this->Wheels.resize(this->NPts);
      this->Spokes.resize(this->NumSpokes);
      Spoke *spokes;
      vtkIdType i, numSpokes, offset, ptId;
      std::vector<vtkIdType>::iterator pItr;
      std::vector<Spoke>::iterator spItr, spEnd;

      // Perform a prefix sum in preparation to copy the spokes into an
      // ordered (by ptId) grouping of spokes.
      offset = 0;
      for (i=0; i < this->NPts; ++i)
      {
        this->Wheels[i] = offset;
        offset += this->NumPts[i];
      }

      // Now copy the spokes into the right spot
      for ( auto& localData : this->LocalData)
      {
        pItr = localData.LocalPtIds.begin();
        spEnd = localData.LocalSpokes.end();
        for ( spItr = localData.LocalSpokes.begin(); spItr != spEnd; )
        {
          ptId = *pItr++;
          numSpokes = this->NumPts[ptId];
          spokes = this->Spokes.data() + this->Wheels[ptId];

          for ( i=0; i < numSpokes; ++i, ++spItr, ++spokes)
          {
            spokes->Id = spItr->Id;
            spokes->Classification = spItr->Classification;
          }
        }//within this local thread data
      }//across all threads
    }//if Delaunay output desired
  }

  // A factory method to conveniently instantiate and execute the algorithm.
  static int Execute(vtkStaticPointLocator2D *loc, vtkIdType numPts, double *points,
                     double padding, vtkPolyData *output, int sMode,
                     vtkPolyData *delOutput, vtkIdType pointOfInterest, vtkIdType maxClips,
                     int &maxSides, vtkVoronoi2D* filter);

  // Given two wheel ids, determine whether a valid edge connects the wheels.
  // This assumes that the wheel&spokes data structure have been built.
  bool IsValidSpoke(vtkIdType w0Id, vtkIdType w1Id)
  {
    vtkIdType numSpokes = this->NumPts[w0Id];
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
// wheel is associated with a point. So access to the wheel and its
// associated spokes is via point id.
struct Wheel
{
  vtkIdType Id; // The associated point id
  int NumSpokes; // The number of emanating spokes
  Spoke* Spokes; // A pointer to an ordered array of spokes connected to this wheel

  // Default instantiation.
  Wheel() : Id(0), NumSpokes(0), Spokes(nullptr) {}
  // Instantiate a wheel given a point id.
  Wheel(VoronoiTiles *vt, vtkIdType id) : Id(id)
  {
    this->NumSpokes = vt->NumPts[id];
    this->Spokes = &vt->Spokes[vt->Wheels[id]];
  }
  // Setup the wheel for queries: an efficient form that does not require
  // wheel instantiation.
  void Initialize(VoronoiTiles *vt, vtkIdType id)
  {
    this->Id = id;
    this->NumSpokes = vt->NumPts[id];
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
// produces triangles from the Voronoi tessellation; in particular the wheels
// and spokes edge structure.
struct Delaunay
{
  VoronoiTiles *VTiles;
  Delaunay(VoronoiTiles *vt) : VTiles(vt) {}

  // Determine whether a valid triangle can be formed given a wheel and two adjacent
  // spokes.  This requires checking classification and connectivity between
  // wheels.
  static bool FormsTriangle(VoronoiTiles* vt, Wheel* wheel,
                            Spoke* currentSpoke, Spoke* nextSpoke)
  {
    // Make sure the spokes are valid. If so, see if the two connected wheels
    // connect with each other. Recall that to avoid parallel contention,
    // whe wheel can create a triangle only if has the smallest point id.
    if ( (wheel->Id < currentSpoke->Id && wheel->Id < nextSpoke->Id) &&
         currentSpoke->Classification == Spoke::VALID &&
         nextSpoke->Classification == Spoke::VALID &&
         vt->IsValidSpoke(currentSpoke->Id,nextSpoke->Id) )
    {
      return true;
    }

    return false;
  }

  // Edge classification via SMPTools. Each wheel with center wheelId is
  // processed, spokes/edges (wheelId,ptId) with (wheelId<ptId) are
  // classified according to simple topological rules. (Ensuring that wheelId
  // is minimum (whellId<ptId) elimates repeating edge classification
  // process.) The end result is a edge graph (represented by classified
  // wheel and spoke edge data structure) which can be triangulated.
  struct ClassifyEdges
  {
    VoronoiTiles *VT;
    ClassifyEdges(VoronoiTiles *vt) : VT(vt) {}

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

  // (See GenerateTriangles below.) This method counts the number of output
  // triangles generated by each wheel, generating an offset array so that
  // that GenerateTriangles() can write in parallel into the output
  // vtkCellArray containing the triangles.
  struct CountTriangles
  {
    VoronoiTiles *VT;
    vtkIdType *WIdx;
    vtkIdType NumTriangles;

    CountTriangles(VoronoiTiles *vt, vtkIdType *widx) :
      VT(vt), WIdx(widx), NumTriangles(0) {}

    // Need a dummy Initialize() method to ensure Reduce() is invoked.
    void Initialize()
    {
    }

    void  operator()(vtkIdType wheelId, vtkIdType endWheelId)
    {
      Wheel wheel;
      int spokeNum;
      Spoke *spoke, *spokeNext;
      int numTris;
      vtkIdType *widx=this->WIdx;

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
          if (Delaunay::FormsTriangle(this->VT,&wheel,spoke,spokeNext))
          {
            numTris++;
          }
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

  // Triangle generation via SMPTools. The classified wheel and spoke
  // structure (a graph) is triangulated. Typically the graph consists of
  // mostly 3-edge subloops which are trivially triangulated. Larger loops
  // (corresponding to degeneracies in the Delaunay triangulation) require a
  // bit more work. Note that only loops (wheelId,ptId0,ptId1,...) are
  // processed when (wheelId<ptId0,ptId1,...). (Ensuring that wheelId is
  // the minimum is in the loop elimates duplicate work (i.e., processing
  // loops multiple times.)
  struct GenerateTriangles
  {
    VoronoiTiles *VT;
    vtkIdType *WIdx;
    vtkIdType *Offsets;
    vtkIdType *Connectivity;
    GenerateTriangles(VoronoiTiles *vt, vtkIdType *widx, vtkIdType *offsets, vtkIdType *conn) :
      VT(vt), WIdx(widx), Offsets(offsets), Connectivity(conn) {}

    void  operator()(vtkIdType wheelId, vtkIdType endWheelId)
    {
      Wheel wheel;
      int spokeNum;
      Spoke *spoke, *spokeNext;
      int numTris;
      vtkIdType *widx=this->WIdx;
      vtkIdType *offsets=this->Offsets, *o, offset;
      vtkIdType *conn=this->Connectivity, *c;

      for ( ; wheelId < endWheelId; ++wheelId )
      {
        // Check to see if this wheel generates triangles
        if ( (widx[wheelId+1] - widx[wheelId]) > 0 )
        {
          numTris = 0;
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
            if (Delaunay::FormsTriangle(this->VT,&wheel,spoke,spokeNext))
            {
              *c++ = wheelId;
              *c++ = spoke->Id;
              *c++ = spokeNext->Id;
              *o++ = offset;
              offset += 3;
            }
          } // over all spokes in the current wheel
        } // if triangles are generated in this wheel
      } // over allwheels
    }
  }; // GenerateTriangles

  // Generate the Delaunay triangulation from the Voronoi tessellation.
  static void Execute(VoronoiTiles *vt)
  {
    // Classify edges. We process one wheel at a time.
    vtkIdType numWheels = vt->NPts;

    // Invoke optional validation process if requested. This may further
    // modify spoke classification.
    if (vt->Filter->GetValidate())
    {
      Delaunay::ClassifyEdges classify(vt);
      vtkSMPTools::For(0, numWheels, classify);
      Delaunay::ButterflyTest butterfly(vt);
      vtkSMPTools::For(0, numWheels, butterfly);
      //      Delaunay::MakeValid makeValid(vt);
    }

    // Generate triangles by processing the Voronoi spoke/wheel graph. We
    // process one wheel at a time. First we need to count the number of
    // output triangles, and then the triangles are actually generated
    // (threaded execution). Note that all edges classified VALID and
    // BOUNDARY are processed, all other classifications are ignored.
    // The widx (wheel idx) is used as the thread partitioning array,
    std::vector<vtkIdType> widx(numWheels+1);
    Delaunay::CountTriangles count(vt,widx.data());
    vtkSMPTools::For(0, numWheels, count);
    vtkIdType numTris = count.NumTriangles;
    cout << "Num Triangles: " << count.NumTriangles << "\n";

    // Compute the internal vtkCellArray offset and connectivity arrays.
    vtkNew<vtkIdTypeArray> offsets;
    offsets->SetNumberOfTuples(numTris+1);
    vtkIdType *offsetsPtr = offsets->GetPointer(0);
    vtkNew<vtkIdTypeArray> connectivity;
    connectivity->SetNumberOfTuples(numTris*3);
    vtkIdType *connectivityPtr = connectivity->GetPointer(0);
    Delaunay::GenerateTriangles generate(vt,widx.data(),offsetsPtr,connectivityPtr);
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
        double padding, vtkPolyData *output, int sMode,
        vtkPolyData *delOutput, vtkIdType pointOfInterest, vtkIdType maxClips,
        int &maxSides, vtkVoronoi2D* filter)
{
  // Generate the Voronoi tessellation
  VoronoiTiles vt(numPts, points, loc, padding, output, sMode, delOutput, maxClips, filter);

  // Either full blown tessellation or just local to a point
  if ( pointOfInterest < 0 || pointOfInterest >= numPts)
  {
    vtkSMPTools::For(0, numPts, vt);
  }
  else
  {
    vtkSMPTools::For(pointOfInterest, pointOfInterest+1, vt);
  }
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
  this->Validate = false;
  this->GenerateScalars = NONE;
  this->Padding = 0.01;
  this->Locator = vtkSmartPointer<vtkStaticPointLocator2D>::New();
  this->Locator->SetNumberOfPointsPerBucket(2);
  this->Transform = nullptr;
  this->ProjectionPlaneMode = XY_PLANE;
  this->PointOfInterest = (-1);
  this->MaximumNumberOfTileClips = VTK_ID_MAX;
  this->GenerateVoronoiFlower = false;
  this->NumberOfThreadsUsed = 0;
  this->Spheres = vtkSmartPointer<vtkSpheres>::New();
  this->MaximumNumberOfSides = 0;

  // Optional outputs 2-4 for 2) Delaunay triangulation, 3) Voronoi flower,
  // and 4) Voronoi tile at point of interest
  this->SetNumberOfOutputPorts(4);
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
    if (this->GenerateScalars != NONE)
    {
      vtkNew<vtkIdTypeArray> ts;
      ts->SetNumberOfComponents(1);
      int idx = output->GetCellData()->AddArray(ts);
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
  }

  // Process the points to generate Voronoi tiles and optional
  // Delaunay triangulation.
  double* inPtr = static_cast<vtkDoubleArray*>(tPoints->GetData())->GetPointer(0);
  this->NumberOfThreadsUsed = VoronoiTiles::
    Execute(this->Locator, numPts, inPtr, padding, output,
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
    for (i = 0, npts = 0; i < 1000000; ++i)
    {
      x[0] = vtkMath::Random(
        center[0] + factor * (bds[0] - center[0]), center[0] + factor * (bds[1] - center[0]));
      x[1] = vtkMath::Random(
        center[1] + factor * (bds[2] - center[1]), center[1] + factor * (bds[3] - center[1]));
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
    tile.PopulatePolyData(centers, singleTile, radii);

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
  os << indent << "Generate Scalars: " << this->GenerateScalars << "\n";
  os << indent << "Padding: " << this->Padding << "\n";
  os << indent << "Locator: " << this->Locator << "\n";
  os << indent << "Projection Plane Mode: " << this->ProjectionPlaneMode << "\n";
  os << indent << "Transform: " << (this->Transform ? "specified" : "none") << "\n";
  os << indent << "Point Of Interest: " << this->PointOfInterest << "\n";
  os << indent << "Maximum Number Of Tile Clips: " << this->MaximumNumberOfTileClips << "\n";
  os << indent << "Generate Voronoi Flower: " << (this->GenerateVoronoiFlower ? "On\n" : "Off\n");
}
VTK_ABI_NAMESPACE_END
