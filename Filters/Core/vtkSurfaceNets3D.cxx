/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSurfaceNets3D.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSurfaceNets3D.h"

#include "vtkArrayDispatch.h"
#include "vtkCellArray.h"
#include "vtkCellArrayIterator.h"
#include "vtkCellData.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "vtkImageTransform.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLabelMapLookup.h"
#include "vtkLogger.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkSMPTools.h"
#include "vtkStreamingDemandDrivenPipeline.h"

#include <memory>

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkSurfaceNets3D);

//============================================================================
// The generation of surface nets consists of two major steps: 1) Extract a
// boundary surface from the labeled data, and 2) smooth the surface to
// improve its quality. (In the case of 3D, the "surface" is either a quad or
// triangle mesh.) Note that the smoothing of the surface requires smoothing
// stencils, which connect points in the center of cubes to potential
// points from face neighbors, and is used in an iterative smoothing
// process. In this implementation of surface nets, a
// vtkConstrainedSmoothingFilter performs the smoothing.
//
// A templated surface nets extraction algorithm implementation follows. It
// uses a edge-by-edge parallel algorithm (aka flying edges approach) for
// performance. There are four passes to surface extraction algorithm: 1)
// classify x-edges. 2) classify y-z-edges, 3) perform a prefix sum to
// determine where to write / allocate output data, and 4) a output
// generation pass (i.e., generate points, polygons, and optional scalar
// data). An optional fifth step smooths this output mesh to improve mesh
// quailty.
//
// Some terminology: Eight voxel points (which in VTK is point-associated data) are
// combined to create regular hexahedron (which in VTK are voxel
// cells). (Note that since surface nets operates on the "dual" of a standard
// VTK image, there is potential confusion for the meaning of a voxel. In the
// surface nets algorithm, a voxel is a region of constant value surrounding
// a data point, and a cube/voxel cell is the hexahedral region connecting eight
// voxels). We associate a cube triad with each voxel which is composed of
// the three voxel edges emanating from the voxel origin (lower-left corner)
// in the +x, +y, +z directions.  The triad carries information about the
// classification of the three voxel edges and associated cube. Triads are
// combined (from neighboring voxels) to create cube edge and face "cases"
// which in turn defines the polygonals and smoothing stencils to
// generate. This triad information is also aggregated to configure the
// filter output, and controls the generation of the output boundary polygons
// (and smoothing stencils).
//
// Implementation detail: a triad is associated with each voxel, except on
// the boundaries. On the boundaries, a layer of extra triads "pads" the
// volume.  This is done to simplify the generation of the surface net (i.e.,
// due to typical boundary effects when processing images), and to enable the
// resulting boundary edges to extend 1/2 cube beyond the edges of the
// volume, since we are stretching VTK's definition of a voxel (value at a
// point) to be a region of constant value.
//
// The reason for triads is that they can be independently computed in
// parallel (without race conditions), and then later combined to provide
// information about the voxel cube that they define. The triads are combined
// to produce a 12-bit "edge case" number, and a 6-bit "face case"
// number. The edge case number indicates, for each of the 12 cube edges,
// which edges are "intersected" meaning the end values of an edge are in two
// separate labeled regions. The face case number is used to define a
// smoothing stencil: for each of the six cube faces, which faces are
// connected via a smoothing edge to their face neighbor.
//
// The edge case number ranges from [0,2**12), considering the XIntersection,
// YIntersection, and ZIntersection bits from all of the contributing triads
// on each of the 12 cube edges; and the face case number ranges from
// [0,2**6) (indicating stencil connections on the 6 cube faces). There is a
// dependent relationship between the edge and face case numbers. For every
// intersected edge, then the two cube faces using that edge will also be
// "intersected" by a smoothing stencil connection. Therefore an edge case
// number can be converted into a a face case number. Note because the edge
// case number exceeds what is representable by a 8-bit unsigned char, the
// edge case number is represented by a 16-bit unsigned short (but is
// converted to a 8-bit unsigned char, face case number as needed). (One
// interesting note about edge cases: although as implemented here the 12
// cube edges can be set independently, this is geometrically impossible
// since if one edge is intersected, than at least two others must be as
// well. It would be a fun little research project to flesh out the
// relationships between edge cases and face cases etc.)

namespace
{ // anonymous

// The SurfaceNets struct below implements the core of the surface nets
// algorithm. It uses a flying edges approach to parallel process data
// edge-by-edge, which provides edge-based parallel tasking, reduces the
// number of voxel lookups and eliminates costly coincident point merging.
//
// Logically, the surface extraction portion of the Surface Nets algorithm is
// implemented over four passes.  In Pass#1 and Pass#2, the triads are
// classified and used to gather information about the cube. In particular,
// the information gathered is whether the x-edge, y-edge, and/or z-edge
// requires "intersection", whether a point needs to be inserted into the
// cube, and whether the origin point of the triad is inside of any labeled
// region, or outside. In Pass#3, a prefix sum is used to characterize the
// output, and allocate the appropriate output arrays. Finally, in Pass#4,
// the final output points, surface, and smoothing stencils are
// produced. Following the surface extraction, an optional smoothing
// operation is used to improve the quality of the output. Prior to
// smoothing, a quad polygon surface mesh is produced; but if smoothing
// occurs the quad polygon mesh is (typically) triangulated since smoothing
// generally causes the quads to become non-planar.
//
// A key concept of this implementation is EdgeMetaData (often referred to as
// eMD[5]). The edge meta data maintains information about each volume x-edge
// (i.e., row) which is necessary for threading the implementation. The
// information maintained is: eMD[0]- the number points produced along the
// x-row; eMD[1]- the number of quad primitives produced from this row;
// eMD[2]- the number of stencil edges; and the eMD[3],eMD[4]- xMin_i and
// xMax_i (minimum index of first intersection, maximum index of intersection
// for row i, the so-called trim edges used for computational trimming). Note
// that portions of the eMD is transformed: initially eMD[0-2] it retains
// counts for the number of points, number of quads, and number of stencil
// edges, respectively. In Pass3 a prefix sum operation is used to accumulate
// this information in preparation for the final output data generation
// Pass4. Also, the eMD refers to a y-z "plane" of edges (including padding).
//
// Another way to look at this: the edge meta data characterizes each row of
// voxel triads. eMD keeps track of the the number of points, quads, and stencil
// edges generated by a row of voxel triads. It also maintains a clipped region
// [xMin_i,xMax_i) or [xL,xR) along the edge of voxel triads in which primitives
// may be generated (i.e., tracks computational trimming). The edge meta data
// provides bookkeeping necessary to support threaded computing.

// Some structs and typedefs to clarify code.
using TriadType = unsigned char;
using EdgeCaseType = unsigned short;
using FaceCaseType = unsigned char;

template <typename T>
struct SurfaceNets
{
  // The triad classification carries information on five different bits.
  // The first bit Bit 1 indicates whether the origin of the triad is inside
  // or outside of *any* labeled region. Bit 2 indicates whether the x-edge
  // needs intersection (i.e., a surface net passes through it); Bit 3
  // whether the y-edge needs intersection; and Bit 4 whether the z-edge
  // needs intersection. (Triad edges require intersection when the two end
  // point values are not equal to one another, and at least one of the end
  // point values is "Inside" a labeled region.)  Finally, the fifth bit is
  // used to indicate whether a point will be generated in the voxel cube/cell
  // associated with a triad. This fifth bit (ProducePoint) is used to
  // simplify and speed up code.
  enum TriadClassification
  {
    Outside = 0,       // triad origin point is outside of any labeled region
    Inside = 1,        // triad origin inside of some labeled region
    XIntersection = 2, // triad x-axis requires intersection
    YIntersection = 4, // triad y-axis requires intersection
    ZIntersection = 8, // triad z-axis requires intersection
    ProducePoint = 16  // the cube associated with this point will produce a point
  };

  // Given a pointer to a voxel's triad, first determine the seven triad cases
  // (from the voxels on the cube: (x,y,z); ([x+1],y,z); (x,[y+1],z);
  // ([x+1],[y+1],z); (x,y,[z+1]); ([x+1],y,[z+1]); (x,[y+1],[z+1]), and then
  // compute the edge case number for this voxel cube. Note that a resulting
  // value of zero means that the cube is not intersected (i.e., no edge is
  // intersected). This method assumes that the tPtr is not on the boundary
  // of the padded volume.
  EdgeCaseType GetEdgeCase(TriadType* tPtr)
  {
    TriadType triads[7];
    triads[0] = *tPtr;
    triads[1] = *(tPtr + 1);
    triads[2] = *(tPtr + this->TriadDims[0]);
    triads[3] = *(tPtr + 1 + this->TriadDims[0]);
    triads[4] = *(tPtr + this->TriadSliceOffset);
    triads[5] = *(tPtr + 1 + this->TriadSliceOffset);
    triads[6] = *(tPtr + this->TriadDims[0] + this->TriadSliceOffset);

    // Process the selected twelve edges from the seven triads to produce an
    // edge case number. The triad numbering is the same as a vtkVoxel point
    // numbering. The edge numbering is also the same as a vtkVoxel edge
    // numbering: first the four voxel x-edges, then the four y-edges, then the
    // four voxel z-edges.
    // x-edges
    EdgeCaseType eCase = ( triads[0] & 2 ) >> 1;
    eCase |= ( triads[2] & 2 );
    eCase |= ( triads[4] & 2 ) << 1;
    eCase |= ( triads[6] & 2 ) << 2;
    // y-edges
    eCase |= ( triads[0] & 4 ) << 2;
    eCase |= ( triads[1] & 4 ) << 3;
    eCase |= ( triads[4] & 4 ) << 4;
    eCase |= ( triads[5] & 4 ) << 5;
    // z-edges
    eCase |= ( triads[0] & 8 ) << 5;
    eCase |= ( triads[1] & 8 ) << 6;
    eCase |= ( triads[2] & 8 ) << 7;
    eCase |= ( triads[3] & 8 ) << 8;

    return eCase;
  }

  // Given a cube edge case, convert it to a cube face case. While this could
  // be done through a table, the size of the table is large enough that a
  // procedural approach simplifies the code. Basically, each intersected
  // cube edge will activate two cube faces.
  FaceCaseType GetFaceCase(EdgeCaseType edgeCase)
  {
    FaceCaseType fCase = 0;

    // Process each of the cube's twelve edges. If edge is set, then set
    // the two faces using the edge.
    if ( edgeCase & 1 ) //edge 0
    {
      fCase |= 20; //faces 2 & 4
    }
    if ( edgeCase & 2 ) //edge 1
    {
      fCase |= 24; //faces 3 & 4
    }
    if ( edgeCase & 4 ) //edge 2
    {
      fCase |= 36; //faces 2 & 5
    }
    if ( edgeCase & 8 ) //edge 3
    {
      fCase |= 40; //faces 3 & 5
    }
    if ( edgeCase & 16 ) //edge 4
    {
      fCase |= 17; //faces 0 & 4
    }
    if ( edgeCase & 32 ) //edge 5
    {
      fCase |= 18; //faces 1 & 4
    }
    if ( edgeCase & 64 ) //edge 6
    {
      fCase |= 33; //faces 0 & 5
    }
    if ( edgeCase & 128 ) //edge 7
    {
      fCase |= 34; //faces 1 & 5
    }
    if ( edgeCase & 256 ) //edge 8
    {
      fCase |= 5; //faces 0 & 2
    }
    if ( edgeCase & 512 ) //edge 9
    {
      fCase |= 6; //faces 1 & 2
    }
    if ( edgeCase & 1024 ) //edge 10
    {
      fCase |= 9; //faces 0 & 3
    }
    if ( edgeCase & 2048 ) //edge 11
    {
      fCase |= 10; //faces 1 & 3
    }

    return fCase;
  }

  // Obtain information indicating whether quad polygons are to be generated
  // from the triad specified.  A triad may produce up to three quad polygons
  // corresponding to the lower left corner of a voxel. One is a x-y quad; a
  // x-z quad, and a y-z quad.
  static bool GenerateXYQuad(TriadType triad)
  {
    return ((triad & SurfaceNets::ZIntersection) > 0);
  }
  static bool GenerateXZQuad(TriadType triad)
  {
    return ((triad & SurfaceNets::YIntersection) > 0);
  }
  static bool GenerateYZQuad(TriadType triad)
  {
    return ((triad & SurfaceNets::XIntersection) > 0);
  }
  static bool ProducesQuad(TriadType triad)
  {
    return ((triad & 14) > 0);
  }
  static unsigned char GetNumberOfQuads(TriadType triad)
  {
    unsigned char numQuads = (SurfaceNets::GenerateXYQuad(triad) ? 1 : 0);
    numQuads += (SurfaceNets::GenerateXZQuad(triad) ? 1 : 0);
    numQuads += (SurfaceNets::GenerateYZQuad(triad) ? 1 : 0);
    return numQuads;
  }

  // This smoothing stencil table is indexed by the cube face case.  For each
  // cube cell, up to six stencil edges may be generated corresponding to
  // connections to each of the cell's six face neighbors. The table consists
  // of: 1) the number of edge connections, and 2) 0/1 values indicating
  // which of the six edge are to be generated. The table of stencil cases is
  // generated programatically; if stencil optimization is on, the stencils
  // are designed to perform better on sharp edges or "T" junctions, so the
  // stencils may not be fully connected as might be expected. Note that the
  // cube's faces are numbered as defined by a vtkVoxel cell (i.e., so that
  // the ordering of stencil edges is -x,+x,-y,+y,-z,+z).
  static const unsigned char StencilCases[64][7];
  static unsigned char GetNumberOfStencilEdges(FaceCaseType caseNum)
  { return SurfaceNets::StencilCases[caseNum][0]; }
  static const unsigned char* GetStencilEdges(FaceCaseType caseNum)
  { return SurfaceNets::StencilCases[caseNum]; }
  void GenerateStencils(bool optStencils);

  // Return whether a triad, and its associated cube, requires the generation
  // of a point.
  bool ProducesPoint(TriadType triad)
  {
    return ((triad & SurfaceNets::ProducePoint) > 0);
  }

  // Input and output data.
  T* Scalars; // input image scalars
  float* NewPts; // output points
  vtkCellArray* NewQuads; // output quad polygons
  T* NewScalars; // output 2-component cell scalars if requested
  vtkCellArray* NewStencils; // output smoothing stencils

  // Internal variable to handle label processing.
  vtkIdType NumLabels;
  const double *LabelValues;
  T BackgroundLabel; // the label of any outside region

  // Internal variables used by the various algorithm methods. Interfaces VTK
  // image data in an efficient form more convenient to the algorithm.
  vtkIdType Dims[3];
  int Min0;
  int Max0;
  int Inc0;
  int Min1;
  int Max1;
  int Inc1;
  int Min2;
  int Max2;
  int Inc2;

  // Algorithm-derived data for bookkeeping data locations
  // when parallel computing.
  TriadType* Triads;
  vtkIdType TriadDims[3];
  vtkIdType TriadSliceOffset;
  static const int EdgeMetaDataSize = 5;
  vtkIdType NumberOfEdges;
  vtkIdType* EdgeMetaData;

  // Instantiate key data members.
  SurfaceNets()
  : Triads(nullptr)
  , TriadDims{0,0,0}
  , TriadSliceOffset(0)
  , NumberOfEdges(0)
  , EdgeMetaData(nullptr)
  , Scalars(nullptr)
  , NewPts(nullptr)
  , NewQuads(nullptr)
  , NewScalars(nullptr)
  , NewStencils(nullptr)
  , NumLabels(0)
  , LabelValues(nullptr)
  , BackgroundLabel(0)
  {
  }

  // Place holder for now in case fancy bit fiddling is needed later.
  void SetTriadClassification(TriadType* tPtr, TriadType tCase)
  { *tPtr = tCase; }

  // Classify the triad y-edges. Use the triad cases at both ends of the y-edge
  // first; if necessary, access the voxel values.
  unsigned char ClassifyYEdge(T* inPtr, vtkIdType i, TriadType case0, TriadType case1)
  {
    unsigned char inout0 = (case0 & 0x1);
    unsigned char inout1 = (case1 & 0x1);
    if ( inout0 == inout1 )
    {
      if ( inout0 == Outside )
      { // both triad origins are outside
        return 0;
      }
      else
      { // both triad origins are inside, need to check regions
        T s0 = *(inPtr+i);
        T s1 = *(inPtr+i+this->Inc1);
        return ( s0 == s1 ? 0 : YIntersection );
      }
    }
    else
    { // one triad origin point is inside, one outside, so y-edge-intersection
      return YIntersection;
    }
  } // ClassifyYEdge

  // Classify the triad z-edges. Use the triad cases at both ends of the z-edge
  // first; if necessary, access the voxel values.
  unsigned char ClassifyZEdge(T* inPtr, vtkIdType i, TriadType case0, TriadType case1)
  {
    unsigned char inout0 = (case0 & 0x1);
    unsigned char inout1 = (case1 & 0x1);
    if ( inout0 == inout1 )
    {
      if ( inout0 == Outside )
      { // both triad origins are outside
        return 0;
      }
      else
      { // both triad origins are inside, need to check regions
        T s0 = *(inPtr+i);
        T s1 = *(inPtr+i+this->Inc2);
        return ( s0 == s1 ? 0 : ZIntersection );
      }
    }
    else
    { // one triad origin point is inside, one outside, so z-edge-intersection
      return ZIntersection;
    }
  } // ClassifyZEdge

  // Composite the trimming information to determine which portion of the
  // volume x-edge (row,slice) to process. Since processing occurs across 3x3
  // bundles of edges, we need to composite the metadata from these nine
  // edges to determine trimming. Also get the 3x3 triads and 3x3 bundle of
  // edge meta data.
  void GetTrimmedEdges(vtkIdType row, vtkIdType slice,
                       vtkIdType &xL, vtkIdType &xR,
                       vtkIdType *ePtrs[9], TriadType* tPtrs[9])
  {
    // Grab the meta data for the 3x3 bundle of rows. Watch out for
    // bundles near the (-x,-y,-z) boundaries. (The (+x,+y,+z) boundaries
    // are always okay due to the nature of the padding, and iteration
    // over rows and slices).
    vtkIdType *EMD = this->EdgeMetaData;
    vtkIdType *dims = this->TriadDims;
    vtkIdType size = this->EdgeMetaDataSize;
    TriadType* triads = this->Triads;
    vtkIdType sliceOffset = this->TriadSliceOffset;

    // Initialize the triads and edge meta data. This simplifies
    // the code.
    std::fill_n(ePtrs,9,nullptr);
    std::fill_n(tPtrs,9,nullptr);

    // These portions of the bundle are always valid, with no boundary issues.
    ePtrs[4] = EMD + (slice*dims[1] + row)*size; //current edge row
    tPtrs[4] = triads + row*dims[0] + slice*sliceOffset;

    ePtrs[5] = ePtrs[4] + size; // to the right of the current edge
    tPtrs[5] = tPtrs[4] + dims[0];

    ePtrs[7] = ePtrs[4] + dims[1]*size; // above the current edge
    tPtrs[7] = tPtrs[4] + sliceOffset;

    ePtrs[8] = ePtrs[7] + size; // above and to the right of the current edge
    tPtrs[8] = tPtrs[7] + dims[0];

    // May be near the -x,-y,-z boundaries.
    // If at origin of y-z plane.
    if ( row!=0 && slice!=0 )
    {
      ePtrs[0] = ePtrs[4] - size - dims[1]*size;
      tPtrs[0] = tPtrs[4] - dims[0] - sliceOffset;
    }

    if ( slice!=0 ) //if not on -z boundary
    {
      ePtrs[1] = ePtrs[4] - dims[1]*size;
      tPtrs[1] = tPtrs[4] - sliceOffset;

      ePtrs[2] = ePtrs[4] + size - dims[1]*size;
      tPtrs[2] = tPtrs[4] + dims[0] - sliceOffset;
    }

    if ( row!=0 ) //if not on -y boundary
    {
      ePtrs[3] = ePtrs[4] - size;
      tPtrs[3] = tPtrs[4] - dims[0];

      ePtrs[6] = ePtrs[4] - size + dims[1]*size;
      tPtrs[6] = tPtrs[4] - dims[0] + sliceOffset;
    }

    // Determine the trim over 3x3 bundle of metadata. This relies
    // on the earlier initialization of eMD.
    vtkIdType *eMD;
    xL = this->TriadDims[0];
    xR = 0;
    for ( auto i=0; i<9; ++i )
    {
      eMD = ePtrs[i];
      if (eMD != nullptr)
      {
        xL = (eMD[3] < xL ? eMD[3] : xL);
        xR = (eMD[4] > xR ? eMD[4] : xR);
      }
    }
  } // GetTrimmedEdges

  // The following two methods are used to help generate output points,
  // polygons, stencils, and scalar data. They manage the numbering of points
  // for each row of cubes. This avoids having to use a locator to merge
  // coincident points.  The x-row iterator works across 3x3 bundles of
  // volume x-edges, with the current edge being processed in the center of
  // the bundle. The edge bundle meta data is passed in to initialize the
  // point ids.
  void InitRowIterator(vtkIdType* ePtrs[9], vtkIdType pIds[9])
  {
    for ( auto idx=0; idx < 9; ++idx)
    {
      vtkIdType *eMD=ePtrs[idx];
      pIds[idx] = (eMD!=nullptr ? eMD[0] : -1);
    }
  }

  // Increment the point ids which are used to generate points, quads, and
  // stencils. The point ids are incremented if the current cube, or the cube
  // above or below have points generated inside of them.  Note that the
  // point ids refer to the nine edges in the 3x3 edge bundle centered around
  // the current edge being processed.
  void AdvanceRowIterator(vtkIdType i, TriadType* tPtrs[9], vtkIdType pIds[9])
  {
    for ( auto idx=0; idx < 9; ++idx)
    {
      if ( tPtrs[idx] && this->ProducesPoint(*(tPtrs[idx]+i)) )
      {
        pIds[idx]++;
      }
    }
  }

  // Given an i,j,k triad index, create a new point in the center of the
  // triad. It is possible for some points to be generated outside of the
  // actual image (i.e., in the padded boundary triads).  The point is
  // generated in image space, later it will be transformed into world space
  // via vtkImageTransform. (Recall that the volume is padded out in the
  // x-y-z directions.)
  void GeneratePoint(vtkIdType ptId, vtkIdType i, vtkIdType j, vtkIdType k)
  {
    float *x = this->NewPts + 3*ptId;
    x[0] = this->Min0 + static_cast<float>(i) - 0.5,
    x[1] = this->Min1 + static_cast<float>(j) - 0.5;
    x[2] = this->Min2 + static_cast<float>(k) - 0.5;
  }

  // Produce the output polygons (quads) for this triad. Note that at most
  // three quads (the "lower left" quads) corresponding to edge numbers
  // (0, 4, 8) can be produced (i.e., for the edges of the voxel triad).
  struct GenerateQuadsImpl
  {
    template <typename CellStateT>
    void operator()(
      CellStateT& state, TriadType triad, vtkIdType* pIds, vtkIdType& quadId)
    {
      using ValueType = typename CellStateT::ValueType;
      auto* offsets = state.GetOffsets();
      auto* conn = state.GetConnectivity();

      auto offsetRange = vtk::DataArrayValueRange<1>(offsets);
      auto offsetIter = offsetRange.begin() + quadId;
      auto connRange = vtk::DataArrayValueRange<1>(conn);
      auto connIter = connRange.begin() + (quadId * 4);

      if ( SurfaceNets::GenerateYZQuad(triad) )
      {
        *offsetIter++ = static_cast<ValueType>(4 * quadId++);
        *connIter++ = pIds[4];
        *connIter++ = pIds[3];
        *connIter++ = pIds[0];
        *connIter++ = pIds[1]; //quad is normal to the x edge
      }

      if ( SurfaceNets::GenerateXZQuad(triad) )
      {
        *offsetIter++ = static_cast<ValueType>(4 * quadId++);
        *connIter++ = pIds[4];
        *connIter++ = pIds[1];
        *connIter++ = pIds[1] - 1;
        *connIter++ = pIds[4] - 1; //normal to the y edge
      }

      if ( SurfaceNets::GenerateXYQuad(triad) )
      {
        *offsetIter++ = static_cast<ValueType>(4 * quadId++);
        *connIter++ = pIds[4];
        *connIter++ = pIds[4] - 1;
        *connIter++ = pIds[3] - 1;
        *connIter++ = pIds[3]; //normal to the z edge
      }

    } // operator()
  }; // GenerateQuadsImpl

  // Finalize the quads array: after all the quads are inserted,
  // the last offset has to be added to complete the offsets array.
  struct FinalizeQuadsOffsetsImpl
  {
    template <typename CellStateT>
    void operator()(CellStateT& state, vtkIdType numQuads)
    {
      using ValueType = typename CellStateT::ValueType;
      auto* offsets = state.GetOffsets();
      auto offsetRange = vtk::DataArrayValueRange<1>(offsets);
      auto offsetIter = offsetRange.begin() + numQuads;
      *offsetIter = static_cast<ValueType>(4*numQuads);
    }
  };

  // Produce the smoothing stencils for this cube.
  struct GenerateStencilImpl
  {
    template <typename CellStateT>
    void operator()(
      CellStateT& state, FaceCaseType fCase, vtkIdType pIds[9], vtkIdType& sOffset)
    {
      // The point on which the stencil operates
      vtkIdType pId = pIds[4];

      using ValueType = typename CellStateT::ValueType;
      auto* offsets = state.GetOffsets();
      auto* conn = state.GetConnectivity();

      auto offsetRange = vtk::DataArrayValueRange<1>(offsets);
      auto offsetIter = offsetRange.begin() + pId;
      auto connRange = vtk::DataArrayValueRange<1>(conn);
      auto connIter = connRange.begin() + sOffset;

      // Create the stencil. Note that for stencils with just one connection
      // (e.g., on the boundary of the image), the stencil point is "locked"
      // in place to prevent any motion to avoid shrinkage etc.
      vtkIdType numEdges = SurfaceNets::GetNumberOfStencilEdges(fCase);
      *offsetIter++ = sOffset;
      sOffset += numEdges;

      if ( numEdges == 1 )
      {
        *connIter = pId;
        return;
      }

      // Create up to six stencil edges connecting the cube edge face
      // neighbors.
      const unsigned char *sEdges = SurfaceNets::GetStencilEdges(fCase);

      // Lower neighbor
      if ( sEdges[1] )
      {
        *connIter++ = pIds[0];
      }

      // Upper neighbor
      if ( sEdges[2] )
      {
        *connIter++ = pIds[2];
      }

      // Left neighbor
      if ( sEdges[3] )
      {
        *connIter++ = pId - 1;
      }

      // Right neighbor
      if ( sEdges[4] )
      {
        *connIter++ = pId + 1;
      }
    }  // operator()
  }; // GenerateStencilImpl

  // Finalize the stencils (cell) array: after all the stencils are inserted,
  // the last offset has to be added to complete the internal offsets array.
  struct FinalizeStencilsOffsetsImpl
  {
    template <typename CellStateT>
    void operator()(CellStateT& state, vtkIdType numPts, vtkIdType numSEdges)
    {
      using ValueType = typename CellStateT::ValueType;
      auto* offsets = state.GetOffsets();
      auto offsetRange = vtk::DataArrayValueRange<1>(offsets);
      auto offsetIter = offsetRange.begin() + numPts;
      *offsetIter = static_cast<ValueType>(numSEdges);
    }
  };

  // Given a triad i,j,k return the voxel value. Note that the
  // triad i,j,k is shifted by 1 due to the padding of the image
  // with boundary triads.
  T GetVoxelForTriad(vtkIdType i, vtkIdType row, vtkIdType slice)
  {
    return *(this->Scalars + (slice-1)*this->Inc2 +
             (row-1)*this->Inc1 + (i-1)*this->Inc0);
  }

  // Helper function writes the scalar 2-tuple.
  void WriteScalarTuple(T s0, T s1, T backgroundLabel,vtkIdType& scalarId)
  {
    T* scalars = this->NewScalars + 2*scalarId;

    if ( s0 == backgroundLabel || (s1 != backgroundLabel && s0 > s1) )
    {
      // Background label is placed last; s0<s1 if both inside
      std::swap(s0,s1);
    }

    *scalars++ = s0; //write 2-tuple
    *scalars++ = s1;
    ++scalarId;
  } // WriteScalarTuple

  // Generate the 2-tuple scalar cell data for the generated quads. Since
  // quads can be created from the voxel triad, at most only three
  // scalar 2-tuples can be created.
  void GenerateScalars(vtkIdType i, vtkIdType row, vtkIdType slice,
                       TriadType triad, TriadType* tPtrs[9],
                       vtkIdType& scalarId)
  {
    T backgroundLabel = this->BackgroundLabel;
    T s0, s1;

    // The current voxel value
    s0 = this->GetVoxelForTriad(i,row,slice);
    vtkIdType vid = i + row*this->TriadDims[0] + slice*this->TriadSliceOffset;

    // Process the three potential tuples corresponding to the
    // triad planes.
    if ( SurfaceNets::GenerateXYQuad(triad) )
    {
      s1 = this->GetVoxelForTriad(i,row,slice+1);
      this->WriteScalarTuple(s0,s1,backgroundLabel,scalarId);
    }

    if ( SurfaceNets::GenerateXZQuad(triad) )
    {
      s1 = this->GetVoxelForTriad(i,row+1,slice);
      this->WriteScalarTuple(s0,s1,backgroundLabel,scalarId);
    }

    if ( SurfaceNets::GenerateYZQuad(triad) )
    {
      s1 = this->GetVoxelForTriad(i+1,row,slice);
      this->WriteScalarTuple(s0,s1,backgroundLabel,scalarId);
    }
  } // GenerateScalars

  // The following are methods supporting the four passes of the
  // surface nets extraction.

  // The first pass is used to classify the x-edges of the triads.
  // Threading integration via SMPTools; this method processes a
  // single x-edge.
  void ClassifyXEdges(T* inPtr, vtkIdType row, vtkIdType slice, vtkLabelMapLookup<T>* lMap); // PASS 1

  // The second pass is used to classify the y- and z-edges of the triads.
  // This method processes an x-row of cubes.
  void ClassifyYZEdges(T* inPtr, vtkIdType row, vtkIdType slice); // PASS 2

  // The third pass is a prefix sum over the edge metadata to determine where
  // the algorithm should write its output, and then allocate output. This is
  // a serial method.
  void ProduceCubeCases(vtkIdType group, int edgeNum,
                        vtkIdType numRowPairs, vtkIdType numSlicePairs);
  void ConfigureOutput(vtkPoints* newPts, vtkCellArray* newQuads,
                       vtkDataArray* newScalars, vtkCellArray* stencils); // PASS 3

  // The fourth pass produces the output geometry (i.e., points) and topology
  // (quads and smoothing stencils). It processes an x-row of cubes.
  void GenerateOutput(vtkIdType row, vtkIdType slice); // PASS 4

}; // SurfaceNets

// Initialize the smoothing stencil cases.
template <class T>
const unsigned char SurfaceNets<T>::StencilCases[64][7] = {
  { 0, 0, 0, 0, 0, 0, 0 }, //case 0
  { 1, 1, 0, 0, 0, 0, 0 }, //case 1
  { 1, 0, 1, 0, 0, 0, 0 }, //case 2
  { 2, 1, 1, 0, 0, 0, 0 }, //case 3
  { 1, 0, 0, 1, 0, 0, 0 }, //case 4
  { 2, 1, 0, 1, 0, 0, 0 }, //case 5
  { 2, 0, 1, 1, 0, 0, 0 }, //case 6
  { 3, 1, 1, 1, 0, 0, 0 }, //case 7
  { 1, 0, 0, 0, 1, 0, 0 }, //case 8
  { 2, 1, 0, 0, 1, 0, 0 }, //case 9
  { 2, 0, 1, 0, 1, 0, 0 }, //case 10
  { 3, 1, 1, 0, 1, 0, 0 }, //case 11
  { 2, 0, 0, 1, 1, 0, 0 }, //case 12
  { 3, 1, 0, 1, 1, 0, 0 }, //case 13
  { 3, 0, 1, 1, 1, 0, 0 }, //case 14
  { 4, 1, 1, 1, 1, 0, 0 }, //case 15
  { 1, 0, 0, 0, 0, 1, 0 }, //case 16
  { 2, 1, 0, 0, 0, 1, 0 }, //case 17
  { 2, 0, 1, 0, 0, 1, 0 }, //case 18
  { 3, 1, 1, 0, 0, 1, 0 }, //case 19
  { 2, 0, 0, 1, 0, 1, 0 }, //case 20
  { 3, 1, 0, 1, 0, 1, 0 }, //case 21
  { 3, 0, 1, 1, 0, 1, 0 }, //case 22
  { 4, 1, 1, 1, 0, 1, 0 }, //case 23
  { 2, 0, 0, 0, 1, 1, 0 }, //case 24
  { 3, 1, 0, 0, 1, 1, 0 }, //case 25
  { 3, 0, 1, 0, 1, 1, 0 }, //case 26
  { 4, 1, 1, 0, 1, 1, 0 }, //case 27
  { 3, 0, 0, 1, 1, 1, 0 }, //case 28
  { 4, 1, 0, 1, 1, 1, 0 }, //case 29
  { 4, 0, 1, 1, 1, 1, 0 }, //case 30
  { 5, 1, 1, 1, 1, 1, 0 }, //case 31
  { 1, 0, 0, 0, 0, 0, 1 }, //case 32
  { 2, 1, 0, 0, 0, 0, 1 }, //case 33
  { 2, 0, 1, 0, 0, 0, 1 }, //case 34
  { 3, 1, 1, 0, 0, 0, 1 }, //case 35
  { 2, 0, 0, 1, 0, 0, 1 }, //case 36
  { 3, 1, 0, 1, 0, 0, 1 }, //case 37
  { 3, 0, 1, 1, 0, 0, 1 }, //case 38
  { 4, 1, 1, 1, 0, 0, 1 }, //case 39
  { 2, 0, 0, 0, 1, 0, 1 }, //case 40
  { 3, 1, 0, 0, 1, 0, 1 }, //case 41
  { 3, 0, 1, 0, 1, 0, 1 }, //case 42
  { 4, 1, 1, 0, 1, 0, 1 }, //case 43
  { 3, 0, 0, 1, 1, 0, 1 }, //case 44
  { 4, 1, 0, 1, 1, 0, 1 }, //case 45
  { 4, 0, 1, 1, 1, 0, 1 }, //case 46
  { 5, 1, 1, 1, 1, 0, 1 }, //case 47
  { 2, 0, 0, 0, 0, 1, 1 }, //case 48
  { 3, 1, 0, 0, 0, 1, 1 }, //case 49
  { 3, 0, 1, 0, 0, 1, 1 }, //case 50
  { 4, 1, 1, 0, 0, 1, 1 }, //case 51
  { 3, 0, 0, 1, 0, 1, 1 }, //case 52
  { 4, 1, 0, 1, 0, 1, 1 }, //case 53
  { 4, 0, 1, 1, 0, 1, 1 }, //case 54
  { 5, 1, 1, 1, 0, 1, 1 }, //case 55
  { 3, 0, 0, 0, 1, 1, 1 }, //case 56
  { 4, 1, 0, 0, 1, 1, 1 }, //case 57
  { 4, 0, 1, 0, 1, 1, 1 }, //case 58
  { 5, 1, 1, 0, 1, 1, 1 }, //case 59
  { 4, 0, 0, 1, 1, 1, 1 }, //case 60
  { 5, 1, 0, 1, 1, 1, 1 }, //case 61
  { 5, 0, 1, 1, 1, 1, 1 }, //case 62
  { 6, 1, 1, 1, 1, 1, 1 }, //case 63
};

// This internal function procedurally generates the stencil cases
// StencilCases[64][7]. Recall that for each case, the first number is the
// number of active edges in the stencil; the next six numbers are 0/1 values
// indicating whether the ith edge is active. If the StencilOptimization is
// enabled, the stencils for sharp edges and "T" junctions are modified to
// smooth along edges and planes, respectively.
template <class T>
void SurfaceNets<T>::GenerateStencils(bool optStencils)
{
  int faceCase;
  for ( faceCase = 0; faceCase < 64; ++faceCase)
  {
    int f0 = ((faceCase & 1) > 0 ? 1 : 0);
    int f1 = ((faceCase & 2) > 0 ? 1 : 0);
    int f2 = ((faceCase & 4) > 0 ? 1 : 0);
    int f3 = ((faceCase & 8) > 0 ? 1 : 0);
    int f4 = ((faceCase & 16) > 0 ? 1 : 0);
    int f5 = ((faceCase & 32) > 0 ? 1 : 0);
    int totalFaces = f0 + f1 + f2 + f3 + f4 + f5;
    cout << "  { ";
    cout << totalFaces << ", ";
    cout << f0 << ", ";
    cout << f1 << ", ";
    cout << f2 << ", ";
    cout << f3 << ", ";
    cout << f4 << ", ";
    cout << f5;
    cout << " }, //case " << faceCase << "\n";
  }
}

// Implementations of the four passes of the surface nets boundary extraction
// process.

//------------------------------------------------------------------------------
// Support PASS 1: Process a single x-row and associated triad's x-axis for
// each voxel on that row.  Trim intersections along the row. Note that only triads
// associated with a voxel are processed: the padded / partial triads are treated
// as special cases. A note about these trimmed edges: the idea is to limit
// the amount of work to be performed. In Pass1/ClassifyXEdges all voxels
// have to be visited, but we can begin culling out "outside" regions. Then
// progressively the trim edges (XL,XR / EdgeMetaData[3],EdgeMetaData[4]) are
// squeezed down so that when generating the output many fewer triads / cubes
// need to be processed.
template <typename T>
void SurfaceNets<T>::ClassifyXEdges(T* inPtr, vtkIdType row, vtkIdType slice,
                                    vtkLabelMapLookup<T>* lMap)
{
  T s0, s1 = (*inPtr); //s1 first voxel in row
  bool isLV0, isLV1 = lMap->IsLabelValue(s1);
  vtkIdType numTriads = this->TriadDims[0];
  TriadType* rowTriadPtr = this->Triads +
    row * this->TriadDims[0] + slice * this->TriadSliceOffset;
  vtkIdType* eMD = this->EdgeMetaData +
    (slice*this->TriadDims[1] + row) * this->EdgeMetaDataSize;
  vtkIdType minInt = numTriads, maxInt = 0;

  // Run along the entire x-edge classifying the triad x axes. Be careful
  // with the padded triads: only process triads whose origin is on a voxel.
  // Note that the ith triad corresponds to the (i-1) image voxel.
  for (vtkIdType i=0; i < (numTriads-1); ++i)
  {
    // This handles the left-hand edge of the slice as well as setting up for
    // the next triad.
    TriadType* tPtr = rowTriadPtr + i;
    s0 = s1;
    isLV0 = isLV1;

    if (i == (numTriads - 2)) // Edge of slice
    {
      s1 = s0;
      isLV1 = isLV0;
    }
    else
    {
      // Processing triads which are associated with voxels.
      s1 = static_cast<T>(*(inPtr + i * this->Inc0));
      isLV1 = ( s0 == s1 ? isLV0 : lMap->IsLabelValue(s1) );
    }

    // Is the current triad origin vertex a label value?
    TriadType tCase = ( isLV0 ? SurfaceNets::Inside : SurfaceNets::Outside );

    // Is the current x-edge split (i.e., different labels on each end).
    if ((isLV0 || isLV1) && s0 != s1)
    {
      tCase |= SurfaceNets::XIntersection;
    }

    // If the triad origin is inside a labeled region, or the x-edge
    // intersects the surface net, then the voxels will have to be
    // processed.
    if (isLV0 || tCase > SurfaceNets::Outside)
    {
      this->SetTriadClassification(tPtr, tCase);
      minInt = (i < minInt ? i : minInt);
      maxInt = i + 1;
    } // if contour interacts with this triad
  }   // for all triad-x-edges along this image x-edge

  // The beginning and ending of intersections along the edge is used for
  // computational trimming.
  eMD[3] = minInt;
  eMD[4] = (maxInt < numTriads ? maxInt : numTriads);
} // ClassifyXEdges

//------------------------------------------------------------------------------
// Support PASS 2: Classify the yz-axis portion of the triads along a single
// x-row of triads. Note that only actual rows and slices containing data (i.e., not padded
// data) are processed by this method.
template <typename T>
void SurfaceNets<T>::ClassifyYZEdges(T* inPtr, vtkIdType row, vtkIdType slice)
{
  // Classify y- and z- triad edges.
  // Triad cases(tPtr,tCase): this row, the next row (y-classification), and the
  // next slice (z-classification).
  vtkIdType numTriads = this->TriadDims[0];
  TriadType* tPtr = this->Triads + row*numTriads + slice*this->TriadSliceOffset;
  TriadType* tPtrY = tPtr + this->TriadDims[0];
  TriadType* tPtrZ = tPtr + this->TriadSliceOffset;

  // Edge metadata: this edge eMD, in the y-direction, and the z-direction.
  vtkIdType* eMD = this->EdgeMetaData +
    (row + slice*this->TriadDims[1]) * EdgeMetaDataSize;
  vtkIdType* eMDY = eMD + EdgeMetaDataSize;
  vtkIdType* eMDZ = eMD + this->TriadDims[1]*EdgeMetaDataSize;

  // Determine whether this row of cubes needs processing. If no x-edge
  // intersections exist in this row or the rows in the y and z directions;
  // and the y- and z-rows have the same voxel value as this row, then this row can be skipped.
  if ( (eMD[3] == numTriads) && (eMDY[3] == numTriads) && (eMDZ[3] == numTriads) &&
       (*tPtr == *tPtrY) && (*tPtr == *tPtrZ) )
  {
    return; // there are no x-, y-, or z-ints, thus no contour, skip row of cubes.
  }

  // Classify all the triad y- and z-edges, excluding the padded triads.
  TriadType tCase, tCaseY, tCaseZ;
  for ( vtkIdType i=1; i < (numTriads-1); ++i )
  {
    tCase = *(tPtr+i);
    tCaseY = *(tPtrY+i);
    tCaseZ = *(tPtrZ+i);
    tCase |= this->ClassifyYEdge(inPtr,(i-1),tCase,tCaseY);
    tCase |= this->ClassifyZEdge(inPtr,(i-1),tCase,tCaseZ);
    this->SetTriadClassification(tPtr+i,tCase);
  } // for all cubes in this volume x-row

} // ClassifyYZEdges


// Process the cubes in a row, combining triads to determine the cube cases.
// If a cube case is non-zero, then a point will be generated in the
// cube, as well as a stencil and possibly some quad polygons. To simplify
// the code, a bit is set in the triad corresponding to the cube (ProducePoint).
// Because the triads form four rows are combined to produce a cube case,
// setting this bit could produce a race condition. Thus the processing of cubes
// is 4-way interleaved to avoid races condition i.e., 0<=whichEdge<4 with a
// group defined as the bundle of four neighboring edges with origin (x,y,z)
// in the +y,+z directions.
template <typename T>
void SurfaceNets<T>::ProduceCubeCases(vtkIdType group, int whichEdge,
                                      vtkIdType numRowPairs, vtkIdType numSlicePairs)
{
  vtkIdType numTriads = this->TriadDims[0];
  vtkIdType row = 2*(group % numRowPairs) + (whichEdge % 2);
  vtkIdType slice = 2*(group / numRowPairs) + (whichEdge / 2);

  // Make sure we don't process bogus triads, including the +/-y,z padding.
  if ( row >= (this->TriadDims[1] - 1) || slice >= (this->TriadDims[2] - 1) )
  {
    return; // don't process padded edges
  }

  vtkIdType edgeNum = row + slice*numTriads;
  vtkIdType minInt=numTriads, maxInt=0;

  // Grab the triad data for this row, and the meta data for this row.
  TriadType* tPtr = this->Triads + row*numTriads + slice*this->TriadSliceOffset;
  vtkIdType *eMD = this->EdgeMetaData + edgeNum * EdgeMetaDataSize;

  // Loop across voxels in this row. We need to determine the case number of each
  // voxel from the seven triads that contribute to the complete edge case. (The
  // eighth triad contributes nothing to the edge case.)
  for ( auto i=0; i < (numTriads-1); ++i )
  {
    EdgeCaseType eCase = this->GetEdgeCase(tPtr+i);
    if ( eCase > 0 ) // then a point must be generated in this voxel
    {
      // Set the bit indicating the triad's cube will generate a point
      TriadType triad = *(tPtr+i);
      triad |= SurfaceNets::ProducePoint;
      this->SetTriadClassification(tPtr+i,triad);

      // Update metadata for this volume edge
      eMD[0]++; //number of points generated
      eMD[1] += this->GetNumberOfQuads(triad); //number of quads
      eMD[2] += this->GetNumberOfStencilEdges(this->GetFaceCase(eCase)); //stencil edges

      // Edge trimming
      minInt = (i < minInt ? i : minInt);
      maxInt = i + 1;
    } // if produces a point
  } // for all triads on this row
  eMD[3] = minInt;
  eMD[4] = (maxInt < numTriads ? maxInt : numTriads);
} // ProduceCubeCases


//------------------------------------------------------------------------------
// PASS 3: Triad classification is complete. Now combine the triads to produce
// cube cases, which indicate whether points, quads, and stencils are to
// be generated. A prefix sum is used to sum up and determine beginning point,
// quad, and stencil numbers for each row. The trim edges per row may also be
// updated (to avoid processing cubes during output generation).
template <typename T> void SurfaceNets<T>::
ConfigureOutput(vtkPoints* newPts, vtkCellArray* newQuads,
                vtkDataArray *newScalars, vtkCellArray* stencils)
{
  //  using ValueType = vtk::GetAPIType<T>;

  // Traverse all rows, combining triads to determine cube cases. Using the
  // case, sum up the number of points, quads, and stencils generated for
  // each row. Note that to avoid race conditions, row processing is interleaved
  // (i.e., groups of four rows: +/-y +/-z volume edges).
  vtkIdType numRows = this->TriadDims[1];
  vtkIdType numRowPairs = (numRows-1)/2 + 1;
  vtkIdType numSlices = this->TriadDims[2];
  vtkIdType numSlicePairs = (numSlices-1)/2 + 1;
  vtkIdType numGroups = numRowPairs * numSlicePairs;

  // Process the four edges that compose a group in order. The four edges form
  // a 2x2 bundle, in the order (j,k),(j+1,k),(j,k+1),(j+1,k+1).
  for ( auto edgeNum=0; edgeNum < 4; ++edgeNum )
  {
    // Edge groups consist of four neighboring volume x-edges (+/-y,+/-z). Process
    // in interleaving fashion to avoid races.
    vtkSMPTools::For(0, numGroups, [this,edgeNum,numRowPairs,numSlicePairs]
                     (vtkIdType group, vtkIdType endGroup) {
      for ( ; group < endGroup; ++group )
      {
        this->ProduceCubeCases(group,edgeNum,numRowPairs,numSlicePairs);
      }
    });
  }

  // Begin prefix sum to determine the point, quad, and stencil number
  // offsets for each row.
  vtkIdType *eMD;
  vtkIdType row, slice, numPts, numQuads, numSEdges;

  // Accumulate the total number of points, quads, and stencil edges
  // across all the image x-rows.
  vtkIdType numOutPts = 0;
  vtkIdType numOutQuads = 0;
  vtkIdType numOutSEdges = 0;

  // Prefix sum to build offsets into the output points, quads, and
  // stencils. We process all edge metadata.
  for (slice = 0; slice < numSlices; ++slice)
  {
    for (row = 0; row < numRows; ++row)
    {
      eMD = this->EdgeMetaData + (slice*this->TriadDims[1] + row) * this->EdgeMetaDataSize;
      numPts = eMD[0];
      numQuads = eMD[1];
      numSEdges = eMD[2];

      eMD[0] = numOutPts;
      eMD[1] = numOutQuads;
      eMD[2] = numOutSEdges;

      numOutPts += numPts;
      numOutQuads += numQuads;
      numOutSEdges += numSEdges;
    }//for rows in this slice
  }//for slices

  // Output can now be allocated.
  if (numOutPts > 0)
  {
    // Points, which are floats
    newPts->SetNumberOfPoints(numOutPts);
    vtkFloatArray *fPts = static_cast<vtkFloatArray*>(newPts->GetData());
    this->NewPts = fPts->GetPointer(0);

    // Boundaries, a set of quads contained in vtkCellArray
    newQuads->ResizeExact(numOutQuads, 4*numOutQuads);
    newQuads->Visit(FinalizeQuadsOffsetsImpl{},numOutQuads);
    this->NewQuads = newQuads;

    // Scalars, which are of type T and 2-components
    newScalars->SetNumberOfTuples(numOutQuads);
    //    this->NewScalars = static_cast<T*>( static_cast<T*>(newScalars)->GetPointer(0) );
    this->NewScalars = static_cast<T*>(newScalars->GetVoidPointer(0));

    // Smoothing stencils, which are represented by a vtkCellArray
    stencils->ResizeExact(numOutPts, numOutSEdges);
    stencils->Visit(FinalizeStencilsOffsetsImpl{},numOutPts,numOutSEdges);
    this->NewStencils = stencils;
  }
} // ConfigureOutput

//------------------------------------------------------------------------------
// PASS 4: Process the x-row triads to generate output primitives, including
// point coordinates, quad primitives, and smoothing stencils. This is the
// fourth pass of the algorithm. Implementation notes: the image origin,
// spacing, and orientation is taken into account later when
// vtkImageTransform::TransformPointSet() is invoked.  When generating the
// points below, computations are performed in canonical image space. Also,
// to generate points, quads, and stencils, the point ids are determined by
// advancing the starting point ids from the current triad row, as well as the
// rows immediately surrounding the current row (i.e., all those rows to
// to which stencil edges connect to, as well as generated quads). This forms
// a 3x3 bundle of volume edges / voxel rows centered on the current x-row.
template <typename T>
void SurfaceNets<T>::GenerateOutput(vtkIdType row, vtkIdType slice)
{
  // This volume edge's meta data, and the neighboring edge.
  vtkIdType* eMD = this->EdgeMetaData +
    (slice*this->TriadDims[1] + row) * this->EdgeMetaDataSize;
  vtkIdType* eMDNei = eMD + this->EdgeMetaDataSize;

  // Return if there is nothing to do (i.e., no points, quads or stencils to
  // generate). eMD[0] is the number of points generated along this volume
  // x-edge. If (eMDNei[0]-eMD[0])<=0 nothing is produced along the edge (i.e.,
  // no points generated) so it can be skipped.
  if (eMDNei[0] <= eMD[0])
  {
    return;
  }

  // Given a volume x-edge to process (defined by [row,slice]), determine the
  // trim edges and the 3x3 row triad cases centered around the current
  // x-edge.
  vtkIdType xL, xR; // computational trim edges
  TriadType* tPtrs[9];  // pointers to the 3x3 bundle of row triad cases
  vtkIdType *eMDPtrs[9]; // pointers to the 3x3 bundle of edge meta data
  this->GetTrimmedEdges(row, slice, xL, xR, eMDPtrs, tPtrs);
  TriadType* tPtr = tPtrs[4]; // triad pointers for current row

  // Initialize the point numbering process using a row iterator. This uses
  // the information gathered from the prefix sum (Pass3) and contained in
  // the edge meta data to obtain point numbers/ids, and the number/size of
  // quads and stencils. The pIds[9] are the current starting point ids for
  // rows surrounding the current edge (in total, a 3x3 stencil, which
  // includes in the center of the stencil, the current edge).  The point ids
  // are initialized with the edge meta data, and advanced as a function of
  // the nine triads[9] along the nine edges.
  vtkIdType pIds[9];
  this->InitRowIterator(eMDPtrs,pIds);
  vtkIdType quadId = eMD[1]; //starting quad id for this row
  vtkIdType sOffset = eMD[2]; //starting stencil offset for this row
  vtkIdType scalarId = quadId; //starting scalar id to generate 2-tuples

  // Now traverse all the voxels in this row, generating points, quads,
  // stencils, and optional scalar data. Points are only generated from the
  // current row; quad segments from the current cube from the three upper
  // right edges; and stencils connecting a cube's point to six possible face
  // neighbors.
  for (auto i = xL; i < xR; ++i)
  {
    // See if any points or quads are to be generated in this voxel.
    TriadType triad = *(tPtr+i);
    if ( this->ProducesPoint(triad) )
    {
      // Output a point in the center of the voxel.
      this->GeneratePoint(pIds[4],i,row,slice);

      // Produce quads if necessary.
      if ( this->ProducesQuad(triad) )
      {
        this->NewQuads->Visit(GenerateQuadsImpl{}, triad, pIds, quadId);
        this->GenerateScalars(i,row,slice,triad,tPtrs,scalarId);
      }

      // If a point is generated, then smoothing stencils are required (i.e.,
      // stencils indicate how the generated point is connected to other
      // points). Up to six connections corresponding to six face neighbors
      // may be generated.
      FaceCaseType fCase = this->GetFaceCase(this->GetEdgeCase(tPtr+i));
      this->NewStencils->Visit(GenerateStencilImpl{}, fCase, pIds, sOffset);
    } // if need to generate a point

    // Need to increment the point ids.
    this->AdvanceRowIterator(i,tPtrs,pIds);
  } // for all triads on this row

} // GenerateOutput

// This worker controls the overall algorithm flow, and handles templated
// dispatch based on the input scalar type. It also interfaces the algorithm
// to the vtkSMPTools / threading infrastructure.
struct NetsWorker
{
  // PASS 1: Process all triads on the given x-rows to classify triad
  // x-axis. Interface to vtkSMPTools::For(). Note that triad row i
  // corresponds to image row (i-1) (due to padding). Also note that looking
  // up labels can be expensive, so a vtkLabelMapLookup is used to accelerate
  // the lookup process. Note that the parallel for (vtkSMPTools::For()) is
  // over the volume slices. Empirically this tends to provide better load
  // balancing / reduce threading overhead and therefore slightly better
  // performance.
  template <typename T>
  struct Pass1
  {
    SurfaceNets<T>* Algo;
    // The label map lookup caches information, so to avoid race conditions,
    // an instance per thread must be created.
    vtkSMPThreadLocal<vtkLabelMapLookup<T>*> LMap;
    Pass1(SurfaceNets<T>* algo) { this->Algo = algo; }
    void Initialize()
    {
      this->LMap.Local() =
        vtkLabelMapLookup<T>::CreateLabelLookup(Algo->LabelValues,Algo->NumLabels);
    }
    void operator()(vtkIdType slice, vtkIdType endSlice)
    {
      vtkLabelMapLookup<T> *lMap = this->LMap.Local();
      T *rowPtr, *slicePtr = this->Algo->Scalars + (slice-1) * this->Algo->Inc2;

      // Process slice-by-slice. Note that the bottom and top slices are not
      // processed (they have been 0-initialized).
      for (; slice < endSlice; ++slice)
      {
        // Process only triads associated with voxels (i.e., no padded voxels).
        rowPtr = slicePtr;
        for ( vtkIdType row = 1; row < (this->Algo->TriadDims[1]-1); ++row)
        {
          this->Algo->ClassifyXEdges(rowPtr, row, slice, lMap);
          rowPtr += this->Algo->Inc1;
        } // for all rows in this slice
        slicePtr += this->Algo->Inc2;
      } // for all slices in this batch
    }
    void Reduce()
    {
      // Delete all of the label map lookups
      for ( auto lmItr=this->LMap.begin(); lmItr != this->LMap.end(); ++lmItr )
      {
        delete *lmItr;
      } // over all threads

      // Note that unlike vtkSurfaceNets2D, the edge meta data has been
      // initialized to a "do not process" state so nothing else needs be
      // done.
    }
  }; // Pass1 dispatch

  // PASS 2: Process all cubes on the given x-rows to classify triad y-z-axes,
  // and classify cubes. Interface to vtkSMPTools::For(). Note that triad row i
  // corresponds to image row (i-1) (i.e., the triads pad out the volume).
  template <typename T>
  struct Pass2
  {
    SurfaceNets<T>* Algo;
    Pass2(SurfaceNets<T>* algo) { this->Algo = algo; }
    void operator()(vtkIdType slice, vtkIdType endSlice)
    {
      T *rowPtr, *slicePtr = this->Algo->Scalars + (slice-1) * this->Algo->Inc2;

      // Process slice-by-slice. Note that the bottom and top slices are not
      // processed (they have been 0-initialized).
      for (; slice < endSlice; ++slice)
      {
        // Process only triads associated with voxels (i.e., no padded voxels).
        rowPtr = slicePtr;
        for ( vtkIdType row = 1; row < (this->Algo->TriadDims[1]-1); ++row)
        {
          this->Algo->ClassifyYZEdges(rowPtr, row, slice);
          rowPtr += this->Algo->Inc1;
        } // for all rows in this slice
        slicePtr += this->Algo->Inc2;
      } // for all slices in this batch
    }
  }; // Pass2 dispatch

  // PASS 3: Configure and allocate output based on classification of
  // the first two passes.
  template<typename T>
  void Pass3(SurfaceNets<T>* algo, vtkPoints* newPts, vtkCellArray* newQuads,
             vtkDataArray *newScalars, vtkCellArray* stencils)
  {
    algo->ConfigureOutput(newPts, newQuads, newScalars, stencils);
  } // Pass3

  // PASS 4: Process all cubes on given volume slices to produce
  // output. Interface to vtkSMPTools::For().
  template <typename T>
  struct Pass4
  {
    SurfaceNets<T>* Algo;
    Pass4(SurfaceNets<T>* algo) { this->Algo = algo; }
    void operator()(vtkIdType slice, vtkIdType endSlice)
    {
      // Note that there is no need to process the last slice.
      for (; slice < endSlice; ++slice)
      {
        // Note that there is no need to process the last triad on this row.
        for ( vtkIdType row=0; row < (this->Algo->TriadDims[1]-1); ++row)
        {
          this->Algo->GenerateOutput(row,slice);
        } // for all rows in this slice
      } // for all slices in this batch
    }
  }; // Pass4 dispatch

  // Dispatch to SurfaceNets.
  template <typename ST>
  void operator()(ST* scalarsArray, vtkSurfaceNets3D* self, vtkImageData* input, int* updateExt,
                  vtkPoints* newPts, vtkCellArray* newQuads, vtkDataArray *newScalars,
                  vtkCellArray* stencils)
  {
    // The type of data carried by the scalarsArray
    using ValueType = vtk::GetAPIType<ST>;

    // The update extent may be different than the extent of the image.
    // The only problem with using the update extent is that one or two
    // sources enlarge the update extent.  This behavior is slated to be
    // eliminated.
    vtkIdType incs[3];
    input->GetIncrements(incs);
    int* ext = input->GetExtent();

    // Capture information for subsequent processing. Make sure that we are
    // processing a 3D image / volume.
    SurfaceNets<ValueType> algo;
    if ( updateExt[0] >= updateExt[1] || updateExt[2] >= updateExt[3] ||
         updateExt[4] >= updateExt[5] )
    {
      vtkLog(ERROR, "Expecting 3D data (volume).");
    }

    //    algo.GenerateStencils(false);

    algo.Min0 = updateExt[0];
    algo.Max0 = updateExt[1];
    algo.Inc0 = incs[0];
    algo.Min1 = updateExt[2];
    algo.Max1 = updateExt[3];
    algo.Inc1 = incs[1];
    algo.Min2 = updateExt[4];
    algo.Max2 = updateExt[5];
    algo.Inc2 = incs[2];

    // Now allocate the working arrays. The Triads array tracks case# for
    // each voxel triad (and the corresponding cube).  Note that each input
    // image voxel has an associated triad, and the "grid" of triads is
    // padded out in the +/-x, +/-y, and +/-z directions (i.e., on all sides
    // of the volume).  This simplifies the generation of the surface net,
    // but be aware that the triads on the boundaries of the volume are
    // treated specially.  Note that the allocation of the triads initializes
    // them to zero; we depend on this as the initial triad classification.
    algo.Dims[0] = algo.Max0 - algo.Min0 + 1;
    algo.Dims[1] = algo.Max1 - algo.Min1 + 1;
    algo.Dims[2] = algo.Max2 - algo.Min2 + 1;
    algo.TriadDims[0] = algo.Dims[0] + 2; //padded in the +/-x direction
    algo.TriadDims[1] = algo.Dims[1] + 2; //padded in the +/-y direction
    algo.TriadDims[2] = algo.Dims[2] + 2; //padded in the +/-z direction
    algo.TriadSliceOffset = algo.TriadDims[0] * algo.TriadDims[1];
    algo.Triads = new TriadType[algo.TriadSliceOffset * algo.TriadDims[2]]();

    // Also allocate the characterization (metadata) array for all of the x
    // volume edges, including the padded out triads. So the x-edge metadata is
    // defined on the y-z plane. This edge metadata array (often referred to
    // as eMD[5]) tracks 0) the number points added along each x-row; as well
    // as 1) the number of quad primitives; 2) the number of stencil edges;
    // and the 3) xMin_i and 4) xMax_i (minimum index of first intersection,
    // maximum index of intersection for row i, so-called trim edges used for
    // computational trimming). Note that the edge meta data eMD[0-2] is zero
    // initialized, while eMD[3,4] is initialized to a "do not process" state
    // which will likely by updated in pass1, pass2, or pass3.
    algo.NumberOfEdges = algo.TriadDims[1] * algo.TriadDims[2]; //y-z plane of edges
    algo.EdgeMetaData = new vtkIdType[algo.NumberOfEdges * algo.EdgeMetaDataSize]();
    for ( auto eNum=0; eNum < algo.NumberOfEdges; ++eNum )
    {
      vtkIdType *eMD = algo.EdgeMetaData + eNum*algo.EdgeMetaDataSize;
      eMD[3] = algo.TriadDims[0];
      eMD[4] = 0;
    }

    // Compute the starting offset location for scalar data.  We may be operating
    // on a part of the volume.
    ValueType* scalars = static_cast<ValueType*>( static_cast<ST*>(scalarsArray)->GetPointer(0) );
    algo.Scalars = scalars + incs[0] * (updateExt[0] - ext[0]) + incs[1] * (updateExt[2] - ext[2]) +
      incs[2] * (updateExt[4] - ext[4]) + self->GetArrayComponent();

    // This algorithm executes just once no matter how many contour/label
    // values, requiring a fast lookup as to whether a data/voxel value is a
    // contour value, or should be considered part of the background. In
    // Pass1, instances of vtkLabelMapLookup<T> are created (per thread)
    // which performs the fast label lookup.
    algo.NumLabels = self->GetNumberOfLabels();
    algo.LabelValues = self->GetValues();
    algo.BackgroundLabel = static_cast<ValueType>(self->GetBackgroundLabel());

    // Now execute the four passes of the surface nets boundary extraction
    // algorithm.

    // Classify the triad x-edges: note that the +/-z boundary-padded triads
    // are not processed. The threads are processing one z-slice of x-edges at
    // a time. Empirically this performs a little better than processing each
    // edge separately.
    Pass1<ValueType> pass1(&algo);
    vtkSMPTools::For(1, algo.TriadDims[2]-1, pass1);

    // Classify the triad y-z-edges; finalize the triad classification.
    // Note that the last padded z-slice of triads is not processed.
    Pass2<ValueType> pass2(&algo);
    vtkSMPTools::For(1, algo.TriadDims[2]-1, pass2);

    // Prefix sum to determine the size and character of the output, and
    // then allocate it.
    Pass3(&algo, newPts, newQuads, newScalars, stencils);

    // Generate the output points, quads, and scalar data. The threads process
    // data slice-by-slice. Note that the last (padded) slice is not
    // processed.
    Pass4<ValueType> pass4(&algo);
    vtkSMPTools::For(0, algo.TriadDims[2]-1, pass4);

    // Clean up and return
    delete[] algo.Triads;
    delete[] algo.EdgeMetaData;
  }

}; // NetsWorker

// This function is used to smooth the output points and quads to produce a
// more pleasing result. Because of smoothing, the quads typically become
// non-planar and are usually decomposed into triangles (although this can
// be controlled by the user.)
void SmoothOutput(vtkPolyData* geomCache, vtkCellArray* stencils,
                  vtkPolyData* output,
                  vtkConstrainedSmoothingFilter* smoother)
{
  vtkLog(INFO, "Smoothing output");

  // Smooth the data and replace the output points.
  smoother->SetInputData(geomCache);
  smoother->SetSmoothingStencils(stencils);
  smoother->Update();

  // Shallow copy / replace points.
  output->CopyStructure(smoother->GetOutput());
  output->GetCellData()->PassData(smoother->GetOutput()->GetCellData());
} // SmoothOutput

// Helper functions to convert the quad output mesh to a different type.
// Transform the input tri strip to two triangles, and write the triangles
// to the output cell array.
struct ConvertToTrisImpl
{
  template <typename CellStateT>
  void operator()(CellStateT& state, vtkIdType cellId, vtkIdType* ptIds)
  {
    using ValueType = typename CellStateT::ValueType;
    auto* offsets = state.GetOffsets();
    auto* conn = state.GetConnectivity();

    auto offsetRange = vtk::DataArrayValueRange<1>(offsets);
    auto offsetItr = offsetRange.begin() + 2 * cellId;
    auto connRange = vtk::DataArrayValueRange<1>(conn);
    auto connItr = connRange.begin() + (cellId * 6);

    // Add two triangles
    *offsetItr++ = static_cast<ValueType>(6 * cellId);
    *connItr++ = ptIds[0];
    *connItr++ = ptIds[1];
    *connItr++ = ptIds[2];

    *offsetItr = static_cast<ValueType>(6 * cellId + 3);
    *connItr++ = ptIds[1];
    *connItr++ = ptIds[0];
    *connItr++ = ptIds[3];
  }
};

// Complete the cell array offsets.
struct FinalizeMeshConversionImpl
{
  template <typename CellStateT>
  void operator()(CellStateT& state, vtkIdType numCells, vtkIdType connSize)
  {
    using ValueType = typename CellStateT::ValueType;
    auto* offsets = state.GetOffsets();
    auto offsetRange = vtk::DataArrayValueRange<1>(offsets);
    auto offsetIter = offsetRange.begin() + numCells;
    *offsetIter = static_cast<ValueType>(connSize);
  }
};

// Functor to drive the threaded conversion of a quad output mesh to
// a different type (i.e., triangles).
struct TransformMesh
{
  vtkPoints *Points;
  vtkCellArray *QuadMesh;
  vtkCellArray *OutputMesh;
  bool ConvertToTris;
  vtkIdType NumOutputCells;
  vtkIdType OutputConnSize;

  // Each thread has a cell array iterator to avoid constant allocation.
  vtkSMPThreadLocal<vtkSmartPointer<vtkCellArrayIterator>> QuadIterator;

  TransformMesh(vtkPoints *pts, vtkCellArray *qMesh, bool convertToTris) :
    Points(pts), QuadMesh(qMesh), OutputMesh(nullptr), ConvertToTris(convertToTris),
    NumOutputCells(0), OutputConnSize(0)
  {
  }

  void Initialize()
  {
    this->QuadIterator.Local().TakeReference(this->QuadMesh->NewIterator());
  }

  void operator()(vtkIdType cellId, vtkIdType endCellId)
  {
    vtkCellArrayIterator* qIter = this->QuadIterator.Local();
    vtkIdType npts;
    const vtkIdType* pts;
    double x0[3], x1[3], x2[3], x3[3];
    vtkIdType conn[4];

    for ( ; cellId < endCellId; ++cellId )
    {
      qIter->GetCellAtId(cellId, npts, pts);

      this->Points->GetPoint(pts[0],x0);
      this->Points->GetPoint(pts[1],x1);
      this->Points->GetPoint(pts[2],x2);
      this->Points->GetPoint(pts[3],x3);

      double d02 = vtkMath::Distance2BetweenPoints(x0,x2);
      double d13 = vtkMath::Distance2BetweenPoints(x1,x3);
      // The "connectivity" is defined by bisecting edge, and then
      // converted to triangles.
      if ( d02 < d13 )
      {
        conn[0] = pts[0];
        conn[1] = pts[2];
        conn[2] = pts[3];
        conn[3] = pts[1];
      }
      else
      {
        conn[0] = pts[1];
        conn[1] = pts[3];
        conn[2] = pts[0];
        conn[3] = pts[2];
      }

      this->OutputMesh->Visit(ConvertToTrisImpl{}, cellId, conn);
    } //over this batch of cells
  }

  void Reduce()
  {
    this->OutputMesh->
      Visit(FinalizeMeshConversionImpl{},this->NumOutputCells,this->OutputConnSize);
  }
};

// Transform quad mesh to triangles
struct TransformMeshToTris : public TransformMesh
{
  // This constructor sets up the transformation process.
  TransformMeshToTris(vtkPoints *pts, vtkCellArray *qMesh) :
    TransformMesh(pts, qMesh, true)
  {
    this->ConvertToTris = true;
    this->NumOutputCells = 2 * qMesh->GetNumberOfCells();
    this->OutputConnSize = 6 * qMesh->GetNumberOfCells();
  }
};


// This function is used to triangulate the output quads produced by the
// Surface Nets boundary extraction, and after subsequent smoothing (if
// any). It basically replaces the output cells with new cells of the
// appropriate type. The input to this method is a quad mesh. The conversion
// process is threaded.
void TransformMeshType(int outputMeshType, vtkPolyData *output)
{
  // Ensure that we have a specfic type, and that we are not requesting
  // quads which are already available.
  outputMeshType = ( outputMeshType == vtkSurfaceNets3D::MESH_TYPE_DEFAULT ?
                     vtkSurfaceNets3D::MESH_TYPE_TRIANGLES : outputMeshType );
  vtkLog(INFO, "Transforming output mesh type to: " << outputMeshType);

  if ( outputMeshType == vtkSurfaceNets3D::MESH_TYPE_QUADS )
  {
    return; //nothing needs to be done
  }

  // Grab the current quad mesh, and convert it to triangles.
  vtkPoints *pts = output->GetPoints();
  vtkCellArray *qMesh = output->GetPolys();
  vtkIdType numCells = qMesh->GetNumberOfCells();

  TransformMeshToTris tMesh(pts,qMesh);
  vtkSMPTools::For(0,numCells, tMesh);
}

} // anonymous namespace

//============================================================================
//------------------------------------------------------------------------------
// Here is the VTK class proper.
vtkSurfaceNets3D::vtkSurfaceNets3D()
{
  this->Labels = vtkSmartPointer<vtkContourValues>::New();
  this->BackgroundLabel = 0;
  this->ArrayComponent = 0;
  this->OutputMeshType = MESH_TYPE_DEFAULT;

  this->Smoothing = true;
  this->Smoother = vtkSmartPointer<vtkConstrainedSmoothingFilter>::New();

  this->DataCaching = true;
  this->GeometryCache = vtkSmartPointer<vtkPolyData>::New();
  this->StencilsCache = vtkSmartPointer<vtkCellArray>::New();

  // by default process active point scalars
  this->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
}

//------------------------------------------------------------------------------
// Overload standard modified time function. If label values or the smoothing
// filter are modified, then this object is modified as well.
vtkMTimeType vtkSurfaceNets3D::GetMTime()
{
  vtkMTimeType mTime = this->Superclass::GetMTime();
  vtkMTimeType mTime2 = this->Labels->GetMTime();
  mTime = ( mTime > mTime2 ? mTime : mTime2 );
  mTime2 = this->Smoother->GetMTime();

  return (mTime2 > mTime ? mTime2 : mTime);
}

//------------------------------------------------------------------------------
// Surface nets filter specialized to 3D images (i.e., volumes).
//
int vtkSurfaceNets3D::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkLog(INFO, "Executing Surface Nets 3D");

  // Get the information objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Get the input and output
  vtkImageData* input = vtkImageData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Determine whether boundary extraction is necessary, or whether we can jump
  // directly to smoothing (and reuse the geometry cache).
  if ( ! this->DataCaching || this->IsCacheEmpty() ||
       this->Superclass::GetMTime() > this->SmoothingTime )
  {
    // Make sure there is data to output.
    vtkIdType numLabels = this->GetNumberOfLabels();
    if (numLabels < 1)
    {
      return 1;
    }

    int* ext = inInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT());
    vtkDataArray* inScalars = this->GetInputArrayToProcess(0, inputVector);
    if (inScalars == nullptr)
    {
      vtkLog(ERROR, "Scalars must be defined for surface nets");
      return 1;
    }

    int numComps = inScalars->GetNumberOfComponents();
    if (this->ArrayComponent >= numComps)
    {
      vtkLog(ERROR, "Scalars have " << numComps << " components. "
             << "ArrayComponent must be smaller than " << numComps);
      return 1;
    }

    // Create necessary objects to hold the output. We will defer the
    // actual allocation once the output size is determined.
    vtkNew<vtkCellArray> newQuads;
    vtkNew<vtkPoints> newPts;
    newPts->SetDataTypeToFloat(); // hardwired to float
    vtkSmartPointer<vtkDataArray> newScalars;

    // Note that the output scalars are the same type T as the input
    // scalars due to the use of NewInstance().
    newScalars.TakeReference(inScalars->NewInstance());
    newScalars->SetNumberOfComponents(2);
    newScalars->SetName("BoundaryLabels");

    // SurfaceNets requires a smoothing stencil to smooth the output
    // edges. Later the stencil will be allocated and populated as the output
    // is generated.
    vtkNew<vtkCellArray> stencils;

    // The templated algorithm goes here. Dispatch on input scalar type. Note
    // that since all VTK types are processed, we don't need dispatch
    // fallback to vtkDataArray. Note that there is a fastpath when
    // generating output scalars when only a single segmented region is being
    // extracted.
    using NetsDispatch = vtkArrayDispatch::DispatchByValueType<vtkArrayDispatch::AllTypes>;
    NetsWorker netsWorker;
    if ( ! NetsDispatch::Execute(inScalars, netsWorker, this, input, ext,
                                 newPts, newQuads, newScalars, stencils) )
    {
      vtkLog(ERROR, "Unsupported data type"); //shouldn't happen because all types are supported
      return 1;
    }

    vtkLog(INFO, "Extracted: " << newPts->GetNumberOfPoints() << " points, "
           << newQuads->GetNumberOfCells() << " quads");

    // Update ourselves.
    output->SetPoints(newPts);
    output->SetPolys(newQuads);

    // Add the label cell data, this 2-tuple indicates what regions/labels are
    // on either side of the surface polygons.
    int idx = output->GetCellData()->AddArray(newScalars);
    output->GetCellData()->SetActiveAttribute(idx, vtkDataSetAttributes::SCALARS);

    // Transform results into physical space. It's necessary to do this
    // before smoothing.
    vtkImageTransform::TransformPointSet(input, output);

    // For now let's stash the data. If caching is disabled, we'll flush it
    // at the end.
    this->CacheData(output,stencils);

  } //Extract boundary geometry

  // If smoothing is to occur, then do it now. It has to be done after image
  // transformation. The smoothing process will replace the current output
  // points. Make sure there is something to smooth.
  vtkCellArray *stencils = this->StencilsCache;
  bool smoothing = false;
  if ( stencils && stencils->GetNumberOfCells() > 0 &&
       this->Smoothing && this->Smoother->GetNumberOfIterations() > 0 )
  {
    smoothing = true;
    SmoothOutput(this->GeometryCache, this->StencilsCache, output, this->Smoother);
  }
  else
  {
    output->CopyStructure(this->GeometryCache);
    output->GetCellData()->PassData(this->GeometryCache->GetCellData());
  }

  // Modify the type of output mesh if necessary. This changes the type
  // of polygons composing the output mesh. By default, the type is
  // quadrilaterals.
  if ( (smoothing && this->OutputMeshType != MESH_TYPE_QUADS) ||
       (!smoothing && this->OutputMeshType == MESH_TYPE_TRIANGLES) )
  {
    TransformMeshType(this->OutputMeshType, output);
  }

  this->SmoothingTime.Modified();

  // Flush the cache if caching is disabled.
  if ( ! this->DataCaching )
  {
    this->GeometryCache = nullptr;
    this->StencilsCache = nullptr;
  }

  return 1;
}

//------------------------------------------------------------------------------
bool vtkSurfaceNets3D::IsCacheEmpty()
{
  return ( ! this->StencilsCache || this->GeometryCache->GetNumberOfPoints() < 1 );
}

//------------------------------------------------------------------------------
void vtkSurfaceNets3D::CacheData(vtkPolyData* pd, vtkCellArray* stencils)
{
  if ( this->DataCaching )
  {
    this->GeometryCache->CopyStructure(pd);
    this->GeometryCache->GetCellData()->PassData(pd->GetCellData());

    this->StencilsCache = stencils;
  }
}

//------------------------------------------------------------------------------
int vtkSurfaceNets3D::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkImageData");
  return 1;
}

//------------------------------------------------------------------------------
void vtkSurfaceNets3D::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  this->Labels->PrintSelf(os, indent.GetNextIndent());

  os << indent << "Background Label: " << this->BackgroundLabel << "\n";
  os << indent << "Array Component: " << this->ArrayComponent << endl;
  os << indent << "Output Mesh Type: " << this->OutputMeshType << endl;

  os << indent << "Smoothing: " << (this->Smoothing ? "On\n" : "Off\n");
  os << indent << "Smoother: " << this->Smoother.Get() << "\n";

  os << indent << "Data Caching: " << (this->DataCaching ? "On\n" : "Off\n");

}
VTK_ABI_NAMESPACE_END
