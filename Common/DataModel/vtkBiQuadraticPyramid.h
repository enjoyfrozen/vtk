/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBiQuadraticPyramid.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkBiQuadraticPyramid
 * @brief   cell represents a parabolic, 13-node isoparametric pyramid
 *
 * vtkBiQuadraticPyramid is a concrete implementation of vtkNonLinearCell to
 * represent a second order three-dimensional isoparametric 19-node pyramid.
 * The interpolation is the standard finite element, bi-quadratic
 * isoparametric shape function. The cell includes 5 corner nodes, 8 mid-edge nodes,
 * 5 mid-face nodes, and 1 centroid node. The ordering of the nineteen points defining
 * the cell is point ids (0-4, 5-12, 13-17, 18), where point ids 0-4 are the five corner
 * vertices of the pyramid; followed by 8 mid-edge nodes (5-12); followed by
 * 5 mid-face nodes (13-17), and the last node (19) is the centroid node.
 * Note that these mid-edge nodes lie on the edges defined by by (0,1), (1,2), (2,3),
 * (3,0), (0,4), (1,4), (2,4), (3,4), respectively. The mid-face nodes lie on the
 * faces defined by (first corner nodes id's, then mid-edge node id's):
 * quadrilateral face: (0, 3, 2, 1, 8, 7, 6, 5), triangle face 1: (0, 1, 4, 5, 10, 9),
 * triangle face 2: (1, 2, 4, 6, 11, 10), triangle face 3: (2, 3, 4, 7, 12, 11),
 * triangle face 5: (3, 0, 4, 8, 9, 12). The last point is the centroid
 * of all the previous 18 nodes. The parametric location of vertex #4 is [0, 0, 1].
 *
 * @note It should be noted that the parametric coordinates that describe this cell
 * are not distorted like in vtkPyramid and vtkQuadraticPyramid, which are a collapsed
 * hexahedron. They are the actual uniform isoparametric coordinates, which are described
 * in Browning's dissertation (see thanks section), but they are converted to [0,1] space.
 *
 * \verbatim
 * Description of 19-node pyramid from bottom to top (based on the z-axis).
 *
 * base quadrilateral including mid-edge nodes and mid-face node:
 *  3-- 7--2
 *  |      |
 *  8  13  6
 *  |      |
 *  0-- 5--1
 *
 * centroid node:
 *
 *
 *     18
 *
 *
 *
 * mid-face nodes of triangular faces:
 *
 *     16
 *    /  \
 *  17    15
 *    \  /
 *     14
 *
 * mid-edge nodes of triangular faces:
 *
 *   12--11
 *    |  |
 *    9--10
 *
 * top corner(apex):
 *
 *
 *     4
 *
 *
 * \endverbatim
 *
 * @sa
 * vtkQuadraticEdge vtkBiQuadraticTriangle vtkQuadraticTetra
 * vtkQuadraticHexahedron vtkBiQuadraticQuad vtkQuadraticWedge
 *
 * @par Thanks:
 * The shape functions and derivatives could be implemented thanks to
 * the report: R.S. Browning. A Second-Order 19-Node Pyramid Finite
 * Element Suitable for Lumped Mass Explicit Dynamic methods in
 * Nonlinear Solid Mechanics, University of Alabama at Birmingham.
 */

#ifndef vtkBiQuadraticPyramid_h
#define vtkBiQuadraticPyramid_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkNonLinearCell.h"

class vtkQuadraticEdge;
class vtkBiQuadraticQuad;
class vtkBiQuadraticTriangle;
class vtkTetra;
class vtkPyramid;
class vtkDoubleArray;

class VTKCOMMONDATAMODEL_EXPORT vtkBiQuadraticPyramid : public vtkNonLinearCell
{
public:
  static vtkBiQuadraticPyramid* New();
  vtkTypeMacro(vtkBiQuadraticPyramid, vtkNonLinearCell);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Implement the vtkCell API. See the vtkCell API for descriptions
   * of these methods.
   */
  int GetCellType() override { return VTK_BIQUADRATIC_PYRAMID; }
  int GetCellDimension() override { return 3; }
  int GetNumberOfEdges() override { return 8; }
  int GetNumberOfFaces() override { return 5; }
  vtkCell* GetEdge(int edgeId) override;
  vtkCell* GetFace(int faceId) override;
  ///@}

  int CellBoundary(int subId, const double pcoords[3], vtkIdList* pts) override;
  void Contour(double value, vtkDataArray* cellScalars, vtkIncrementalPointLocator* locator,
    vtkCellArray* verts, vtkCellArray* lines, vtkCellArray* polys, vtkPointData* inPd,
    vtkPointData* outPd, vtkCellData* inCd, vtkIdType cellId, vtkCellData* outCd) override;
  int EvaluatePosition(const double x[3], double closestPoint[3], int& subId, double pcoords[3],
    double& dist2, double weights[]) override;
  void EvaluateLocation(int& subId, const double pcoords[3], double x[3], double* weights) override;

  /**
   * Line-edge intersection. Intersection has to occur within [0,1] parametric
   * coordinates and with specified tolerance.
   */
  int IntersectWithLine(const double p1[3], const double p2[3], double tol, double& t, double x[3],
    double pcoords[3], int& subId) override;

  int Triangulate(int index, vtkIdList* ptIds, vtkPoints* pts) override;
  void Derivatives(
    int subId, const double pcoords[3], const double* values, int dim, double* derivs) override;
  double* GetParametricCoords() override;

  /**
   * Clip this quadratic triangle using scalar value provided. Like
   * contouring, except that it cuts the triangle to produce linear
   * triangles.
   */
  void Clip(double value, vtkDataArray* cellScalars, vtkIncrementalPointLocator* locator,
    vtkCellArray* tets, vtkPointData* inPd, vtkPointData* outPd, vtkCellData* inCd,
    vtkIdType cellId, vtkCellData* outCd, int insideOut) override;

  /**
   * Return the center of the bi-quadratic pyramid in parametric coordinates.
   */
  int GetParametricCenter(double pcoords[3]) override;

  static void InterpolationFunctions(const double pcoords[3], double weights[19]);
  static void InterpolationDerivs(const double pcoords[3], double derivs[57]);
  ///@{
  /**
   * Compute the interpolation functions/derivatives
   * (aka shape functions/derivatives)
   */
  void InterpolateFunctions(const double pcoords[3], double weights[19]) override
  {
    vtkBiQuadraticPyramid::InterpolationFunctions(pcoords, weights);
  }
  void InterpolateDerivs(const double pcoords[3], double derivs[57]) override
  {
    vtkBiQuadraticPyramid::InterpolationDerivs(pcoords, derivs);
  }
  ///@}

  /**
   * Given parametric coordinates compute inverse Jacobian transformation
   * matrix. Returns 9 elements of 3x3 inverse Jacobian plus interpolation
   * function derivatives.
   */
  void JacobianInverse(const double pcoords[3], double** inverse, double derivs[57]);

  ///@{
  /**
   * Return the ids of the vertices defining edge/face (`edgeId`/`faceId').
   * Ids are related to the cell, not to the dataset.
   *
   * @note The return type changed. It used to be int*, it is now const vtkIdType*.
   * This is so ids are unified between vtkCell and vtkPoints.
   */
  static const vtkIdType* GetEdgeArray(vtkIdType edgeId);
  static const vtkIdType* GetFaceArray(vtkIdType faceId);
  ///@}

protected:
  vtkBiQuadraticPyramid();
  ~vtkBiQuadraticPyramid() override;

  vtkQuadraticEdge* Edge;
  vtkBiQuadraticTriangle* TriangleFace;
  vtkBiQuadraticQuad* QuadFace;
  vtkTetra* Tetra;
  vtkPyramid* Pyramid;
  vtkDoubleArray* Scalars; // used to avoid New/Delete in contouring/clipping

private:
  vtkBiQuadraticPyramid(const vtkBiQuadraticPyramid&) = delete;
  void operator=(const vtkBiQuadraticPyramid&) = delete;
};
//----------------------------------------------------------------------------
// Return the center of the bi-quadratic pyramid in parametric coordinates.
//
inline int vtkBiQuadraticPyramid::GetParametricCenter(double pcoords[3])
{
  pcoords[0] = pcoords[1] = 0.5;
  // This is different compared to the last node, because the last node
  // is the centroid of the nodes 0-4, instead of the nodes 0-17.
  // So pcoords[2] is defined as followed to pass the TestGenericCell requirement
  pcoords[2] = 283.0 / 456.0;
  return 0;
}

#endif
