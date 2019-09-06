/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPolyLine
 * @brief   cell represents a set of 1D lines
 *
 * vtkPolyLine is a concrete implementation of vtkCell to represent a set
 * of 1D lines.
*/

#ifndef vtkPolyLine_h
#define vtkPolyLine_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkCell.h"

class vtkPoints;
class vtkCellArray;
class vtkLine;
class vtkDataArray;
class vtkIncrementalPointLocator;
class vtkCellData;

class VTKCOMMONDATAMODEL_EXPORT vtkPolyLine : public vtkCell
{
public:
  static vtkPolyLine *New();
  vtkTypeMacro(vtkPolyLine,vtkCell);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Given points and lines, compute normals to lines. These are not true
   * normals, they are "orientation" normals used by classes like vtkTubeFilter
   * that control the rotation around the line. The normals try to stay pointing
   * in the same direction as much as possible (i.e., minimal rotation) w.r.t the
   * firstNormal (computed if nullptr). Always returns 1 (success).
   */
  static int GenerateSlidingNormals(vtkPoints *, vtkCellArray *, vtkDataArray *);
  static int GenerateSlidingNormals(vtkPoints *, vtkCellArray *, vtkDataArray *,
                                    double* firstNormal);
  //@}

  //@{
  /**
   * See the vtkCell API for descriptions of these methods.
   */
  int GetCellType() override {return VTK_POLY_LINE;};
  int GetCellDimension() override {return 1;};
  int GetNumberOfEdges() override {return 0;};
  int GetNumberOfFaces() override {return 0;};
  vtkCell *GetEdge(int vtkNotUsed(edgeId)) override {return nullptr;};
  vtkCell *GetFace(int vtkNotUsed(faceId)) override {return nullptr;};
  int CellBoundary(int subId, const double pcoords[3], vtkIdList *pts) override;
  void Contour(double value, vtkDataArray *cellScalars,
               vtkIncrementalPointLocator *locator, vtkCellArray *verts,
               vtkCellArray *lines, vtkCellArray *polys,
               vtkPointData *inPd, vtkPointData *outPd,
               vtkCellData *inCd, vtkIdType cellId, vtkCellData *outCd) override;
  void Clip(double value, vtkDataArray *cellScalars,
            vtkIncrementalPointLocator *locator, vtkCellArray *lines,
            vtkPointData *inPd, vtkPointData *outPd,
            vtkCellData *inCd, vtkIdType cellId, vtkCellData *outCd,
            int insideOut) override;
  int EvaluatePosition(const double x[3], double closestPoint[3],
                       int& subId, double pcoords[3],
                       double& dist2, double weights[]) override;
  void EvaluateLocation(int& subId, const double pcoords[3], double x[3],
                        double *weights) override;
  int IntersectWithLine(const double p1[3], const double p2[3], double tol, double& t,
                        double x[3], double pcoords[3], int& subId) override;
  int Triangulate(int index, vtkIdList *ptIds, vtkPoints *pts) override;
  void Derivatives(int subId, const double pcoords[3], const double *values,
                   int dim, double *derivs) override;
  int IsPrimaryCell() override {return 0;}
  //@}

  /**
   * Return the center of the point cloud in parametric coordinates.
   */
  int GetParametricCenter(double pcoords[3]) override;

protected:
  vtkPolyLine();
  ~vtkPolyLine() override;

  vtkLine *Line;

private:
  vtkPolyLine(const vtkPolyLine&) = delete;
  void operator=(const vtkPolyLine&) = delete;
};

#endif


