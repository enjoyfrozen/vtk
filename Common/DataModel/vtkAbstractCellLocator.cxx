/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAbstractCellLocator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkAbstractCellLocator.h"

#include "vtkArrayDispatch.h"
#include "vtkCellArray.h"
#include "vtkDataArrayRange.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkGenericCell.h"
#include "vtkIdList.h"
#include "vtkImageData.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSMPTools.h"
#include "vtkTransform.h"

#include "vtk_eigen.h"
#include VTK_EIGEN(Geometry)

//------------------------------------------------------------------------------
vtkAbstractCellLocator::LinearTransformationInformation::LinearTransformationInformation()
  : UseTransform(false)
{
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::LinearTransformationInformation::InverseTransformPointIfNeeded(
  const double x[3], double xtransform[3])
{
  if (!this->UseTransform)
  {
    xtransform[0] = x[0];
    xtransform[1] = x[1];
    xtransform[2] = x[2];
  }
  else
  {
    this->InverseTransform->InternalTransformPoint(x, xtransform);
  }
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::LinearTransformationInformation::TransformPointIfNeeded(
  const double x[3], double xtransform[3])
{
  if (!this->UseTransform)
  {
    xtransform[0] = x[0];
    xtransform[1] = x[1];
    xtransform[2] = x[2];
  }
  else
  {
    this->Transform->InternalTransformPoint(x, xtransform);
  }
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::LinearTransformationInformation::InverseTransformNormalIfNeeded(
  const double n[3], double ntransform[3])
{
  if (!this->UseTransform)
  {
    ntransform[0] = n[0];
    ntransform[1] = n[1];
    ntransform[2] = n[2];
  }
  else
  {
    this->InverseTransform->InternalTransformNormal(n, ntransform);
  }
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::LinearTransformationInformation::TransformNormalIfNeeded(
  const double n[3], double ntransform[3])
{
  if (!this->UseTransform)
  {
    ntransform[0] = n[0];
    ntransform[1] = n[1];
    ntransform[2] = n[2];
  }
  else
  {
    this->Transform->InternalTransformNormal(n, ntransform);
  }
}

//------------------------------------------------------------------------------
vtkAbstractCellLocator::vtkAbstractCellLocator()
{
  this->CacheCellBounds = 1;
  this->CellBounds = nullptr;
  this->MaxLevel = 8;
  this->Level = 0;
  this->RetainCellLists = 1;
  this->NumberOfCellsPerNode = 32;
  this->UseExistingSearchStructure = 0;
}

//------------------------------------------------------------------------------
vtkAbstractCellLocator::~vtkAbstractCellLocator() = default;

//------------------------------------------------------------------------------
static vtkSmartPointer<vtkPoints> GetPoints(vtkDataSet* ds)
{
  auto points = vtkSmartPointer<vtkPoints>::New();
  if (auto pointSet = vtkPointSet::SafeDownCast(ds))
  {
    points->ShallowCopy(pointSet->GetPoints());
  }
  else if (auto imageData = vtkImageData::SafeDownCast(ds))
  {
    points->SetDataTypeToDouble();
    points->SetNumberOfPoints(imageData->GetNumberOfPoints());
    vtkSMPTools::For(0, imageData->GetNumberOfPoints(), [&](vtkIdType begin, vtkIdType end) {
      double point[3];
      for (vtkIdType i = begin; i < end; ++i)
      {
        imageData->GetPoint(i, point);
        points->SetPoint(i, point);
      }
    });
  }
  else if (auto recGrid = vtkRectilinearGrid::SafeDownCast(ds))
  {
    recGrid->GetPoints(points);
  }
  else
  {
    vtkGenericWarningMacro(<< "Unsupported dataset type: " << ds->GetClassName());
  }
  return points;
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::CopyInitialPoints()
{
  // copy the initial points
  this->LinearTransformationInfo.InitialPoints->ShallowCopy(GetPoints(this->DataSet));
}

//------------------------------------------------------------------------------
struct ComputeTransformationWorker
{
  Eigen::Matrix3d RotationMatrix;
  Eigen::Vector3d TranslationVector;

  template <typename PointsArray>
  void FastTransformComputation(
    PointsArray* points1, PointsArray* points2, bool& validTransformation)
  {
    using ValueType = typename PointsArray::ValueType;
    using MatrixX = Eigen::Matrix<ValueType, Eigen::Dynamic, Eigen::Dynamic>;
    using Matrix3 = Eigen::Matrix<ValueType, 3, 3>;
    using Vector3 = Eigen::Matrix<ValueType, 3, 1>;
    auto p1 = Eigen::Map<MatrixX>(points1->GetPointer(0), 3, (points1)->GetNumberOfTuples());
    auto p2 = Eigen::Map<MatrixX>(points2->GetPointer(0), 3, (points2)->GetNumberOfTuples());
    // find the rotation and translation matrix between 2 sets of points
    Vector3 p1BaryCenter = p1.rowwise().mean();
    Vector3 p2BaryCenter = p2.rowwise().mean();
    auto centeredP1 = p1.colwise() - p1BaryCenter;
    auto centeredP2 = p2.colwise() - p2BaryCenter;
    auto covarianceMatrix = centeredP2 * centeredP1.transpose();
    Eigen::JacobiSVD<MatrixX> svd(covarianceMatrix, Eigen::ComputeFullV | Eigen::ComputeFullU);
    // both matrices are 3x3
    auto matrixV = svd.matrixV();
    auto& matrixU = svd.matrixU();
    Matrix3 rotationMatrix = matrixV * matrixU.transpose();
    // there is reflection
    if (rotationMatrix.determinant() < 0)
    {
      matrixV.col(2) *= -1;
      rotationMatrix = matrixV * matrixU.transpose();
    }
    Vector3 translationVector = -rotationMatrix * p2BaryCenter + p1BaryCenter;
    // calculate the root mean squared error between the actual p1 and replicated p2
    auto rotatedP2 = (rotationMatrix * p2).colwise() + translationVector;
    auto errorMatrix = rotatedP2 - p1;
    double rootMeanSquaredError = std::sqrt(errorMatrix.array().square().sum() / p1.cols());
    // check if p2 is a linear transformation of p1
    if (rootMeanSquaredError <= 0.001)
    {
      validTransformation = true;
      this->RotationMatrix = rotationMatrix.template cast<double>();
      this->TranslationVector = translationVector.template cast<double>();
      ;
    }
    else
    {
      validTransformation = false;
      vtkGenericWarningMacro(
        "Points are not close enough to be considered a linear transformation. "
        << rootMeanSquaredError);
    }
  }

  template <typename PointsArray1, typename PointsArray2>
  void operator()(PointsArray1* points1, PointsArray2* points2, bool& validTransformation)
  {
    auto p1Range = vtk::DataArrayTupleRange<3>(points1);
    auto p2Range = vtk::DataArrayTupleRange<3>(points2);
    Eigen::MatrixXd p1, p2;
    p1.resize(3, p1Range.size());
    p2.resize(3, p2Range.size());
    vtkSMPTools::For(0, p1Range.size(), [&](vtkIdType begin, vtkIdType end) {
      for (vtkIdType i = begin; i < end; i++)
      {
        p1(0, i) = p1Range[i][0];
        p1(1, i) = p1Range[i][1];
        p1(2, i) = p1Range[i][2];
        p2(0, i) = p2Range[i][0];
        p2(1, i) = p2Range[i][1];
        p2(2, i) = p2Range[i][2];
      }
    });

    // find the rotation and translation matrix between 2 sets of points
    Eigen::Vector3d p1BaryCenter = p1.rowwise().mean();
    Eigen::Vector3d p2BaryCenter = p2.rowwise().mean();
    auto centeredP1 = p1.colwise() - p1BaryCenter;
    auto centeredP2 = p2.colwise() - p2BaryCenter;
    auto covarianceMatrix = centeredP2 * centeredP1.transpose();
    Eigen::JacobiSVD<Eigen::MatrixXd> svd(
      covarianceMatrix, Eigen::ComputeFullV | Eigen::ComputeFullU);
    // both matrices are 3x3
    auto matrixV = svd.matrixV();
    auto& matrixU = svd.matrixU();
    Eigen::Matrix3d rotationMatrix = matrixV * matrixU.transpose();
    // there is reflection
    if (rotationMatrix.determinant() < 0)
    {
      matrixV.col(2) *= -1;
      rotationMatrix = matrixV * matrixU.transpose();
    }
    Eigen::Vector3d translationVector = -rotationMatrix * p2BaryCenter + p1BaryCenter;
    // calculate the root mean squared error between the actual p1 and replicated p2
    auto rotatedP2 = (rotationMatrix * p2).colwise() + translationVector;
    auto errorMatrix = rotatedP2 - p1;
    double rootMeanSquaredError = std::sqrt(errorMatrix.array().square().sum() / p1.cols());
    // check if p2 is a linear transformation of p1
    if (rootMeanSquaredError <= 0.001)
    {
      validTransformation = true;
      this->RotationMatrix = rotationMatrix;
      this->TranslationVector = translationVector;
    }
    else
    {
      validTransformation = false;
      vtkGenericWarningMacro(
        "Points are not close enough to be considered a linear transformation. "
        << rootMeanSquaredError);
    }
  }

  void DefineTransform(vtkTransform* transform, vtkTransform* inverseTransform)
  {
    auto transposeRotationMatrix = this->RotationMatrix.transpose();
    double matrix[4][4];
    vtkMatrix4x4::Identity(*matrix);
    for (uint8_t i = 0; i < 3; ++i)
    {
      for (uint8_t j = 0; j < 3; ++j)
      {
        matrix[i][j] = transposeRotationMatrix(i, j);
      }
    }
    transform->SetMatrix(*matrix);
    auto negativeTranslationVector = -this->TranslationVector;
    transform->Translate(
      negativeTranslationVector[0], negativeTranslationVector[1], negativeTranslationVector[2]);
    transform->Update();
    vtkMatrix4x4::Invert(transform->GetMatrix()->GetData(), *matrix);
    inverseTransform->SetMatrix(*matrix);
    inverseTransform->Update();
  }
};

//------------------------------------------------------------------------------
bool vtkAbstractCellLocator::ComputeTransformation()
{
  auto initialPointsData = this->LinearTransformationInfo.InitialPoints->GetData();
  vtkNew<vtkPoints> newPoints;
  newPoints->ShallowCopy(GetPoints(this->DataSet));
  auto newPointsData = newPoints->GetData();
  if (initialPointsData->GetNumberOfTuples() != newPointsData->GetNumberOfTuples())
  {
    vtkErrorMacro("The new dataset has different number of points from the old one.");
    return false;
  }
  bool validTransformation = false;
  ComputeTransformationWorker worker;
  // first try to use the fast version which does not copy the points
  if (vtkDoubleArray::SafeDownCast(initialPointsData) &&
    vtkDoubleArray::SafeDownCast(newPointsData))
  {
    auto initialPointsDataDouble = vtkDoubleArray::SafeDownCast(initialPointsData);
    auto newPointsDataDouble = vtkDoubleArray::SafeDownCast(newPointsData);
    worker.FastTransformComputation<vtkDoubleArray>(
      initialPointsDataDouble, newPointsDataDouble, validTransformation);
  }
  else if (vtkFloatArray::SafeDownCast(initialPointsData) &&
    vtkFloatArray::SafeDownCast(newPointsData))
  {
    auto initialPointsDataFloat = vtkFloatArray::SafeDownCast(initialPointsData);
    auto newPointsDataFloat = vtkFloatArray::SafeDownCast(newPointsData);
    worker.FastTransformComputation<vtkFloatArray>(
      initialPointsDataFloat, newPointsDataFloat, validTransformation);
  }
  else
  {
    using Dispatcher = vtkArrayDispatch::Dispatch2BySameValueType<vtkArrayDispatch::Reals>;
    if (!Dispatcher::Execute(initialPointsData, newPointsData, worker, validTransformation))
    {
      worker(initialPointsData, newPointsData, validTransformation);
    }
  }
  if (validTransformation)
  {
    worker.DefineTransform(
      this->LinearTransformationInfo.Transform, this->LinearTransformationInfo.InverseTransform);
  }
  return validTransformation;
}

//------------------------------------------------------------------------------
bool vtkAbstractCellLocator::StoreCellBounds()
{
  if (this->CellBounds)
  {
    return false;
  }
  if (!this->DataSet)
  {
    return false;
  }
  // Allocate space for cell bounds storage, then fill
  vtkIdType numCells = this->DataSet->GetNumberOfCells();
  this->CellBoundsSharedPtr = std::make_shared<std::vector<double>>(numCells * 6);
  this->CellBounds = this->CellBoundsSharedPtr->data();

  // This is done to cause non-thread safe initialization to occur due to
  // side effects from GetCellBounds().
  this->DataSet->GetCellBounds(0, &this->CellBounds[0]);

  vtkSMPTools::For(1, numCells, [&](vtkIdType begin, vtkIdType end) {
    for (vtkIdType cellId = begin; cellId < end; cellId++)
    {
      this->DataSet->GetCellBounds(cellId, &this->CellBounds[cellId * 6]);
    }
  });
  return true;
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::FreeCellBounds()
{
  this->CellBoundsSharedPtr.reset();
  this->CellBounds = nullptr;
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::UpdateInternalWeights()
{
  if (this->WeightsTime > this->MTime || !this->DataSet)
  {
    return;
  }

  this->Weights.resize(this->DataSet->GetMaxCellSize());
  this->WeightsTime.Modified();
}

//------------------------------------------------------------------------------
bool vtkAbstractCellLocator::IsInBounds(const double bounds[6], const double x[3], const double tol)
{
  return (bounds[0] - tol) <= x[0] && x[0] <= (bounds[1] + tol) && (bounds[2] - tol) <= x[1] &&
    x[1] <= (bounds[3] + tol) && (bounds[4] - tol) <= x[2] && x[2] <= (bounds[5] + tol);
}

//------------------------------------------------------------------------------
int vtkAbstractCellLocator::IntersectWithLine(const double p1[3], const double p2[3], double tol,
  double& t, double x[3], double pcoords[3], int& subId)
{
  vtkIdType cellId = -1;
  return this->IntersectWithLine(p1, p2, tol, t, x, pcoords, subId, cellId);
}

//------------------------------------------------------------------------------
int vtkAbstractCellLocator::IntersectWithLine(const double p1[3], const double p2[3], double tol,
  double& t, double x[3], double pcoords[3], int& subId, vtkIdType& cellId)
{
  int returnVal;
  returnVal = this->IntersectWithLine(p1, p2, tol, t, x, pcoords, subId, cellId, this->GenericCell);
  return returnVal;
}

//------------------------------------------------------------------------------
int vtkAbstractCellLocator::IntersectWithLine(const double vtkNotUsed(p1)[3],
  const double vtkNotUsed(p2)[3], double vtkNotUsed(tol), double& vtkNotUsed(t),
  double vtkNotUsed(x)[3], double vtkNotUsed(pcoords)[3], int& vtkNotUsed(subId),
  vtkIdType& vtkNotUsed(cellId), vtkGenericCell* vtkNotUsed(cell))
{
  vtkErrorMacro(<< "The locator class - " << this->GetClassName()
                << " does not yet support IntersectWithLine");
  return 0;
}

//------------------------------------------------------------------------------
int vtkAbstractCellLocator::IntersectWithLine(const double vtkNotUsed(p1)[3],
  const double vtkNotUsed(p2)[3], vtkPoints* vtkNotUsed(points), vtkIdList* vtkNotUsed(cellIds))
{
  vtkErrorMacro(<< "The locator class - " << this->GetClassName()
                << " does not yet support this IntersectWithLine interface");
  return 0;
}

//------------------------------------------------------------------------------
int vtkAbstractCellLocator::IntersectWithLine(
  const double p1[3], const double p2[3], const double tol, vtkPoints* points, vtkIdList* cellIds)
{
  return this->IntersectWithLine(p1, p2, tol, points, cellIds, this->GenericCell);
}

//------------------------------------------------------------------------------
int vtkAbstractCellLocator::IntersectWithLine(const double vtkNotUsed(p1)[3],
  const double vtkNotUsed(p2)[3], double vtkNotUsed(tol), vtkPoints* vtkNotUsed(points),
  vtkIdList* vtkNotUsed(cellIds), vtkGenericCell* vtkNotUsed(cell))
{
  vtkErrorMacro(<< "The locator class - " << this->GetClassName()
                << " does not yet support this IntersectWithLine interface");
  return 0;
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::FindClosestPoint(
  const double x[3], double closestPoint[3], vtkIdType& cellId, int& subId, double& dist2)
{
  this->FindClosestPoint(x, closestPoint, this->GenericCell, cellId, subId, dist2);
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::FindClosestPoint(const double x[3], double closestPoint[3],
  vtkGenericCell* cell, vtkIdType& cellId, int& subId, double& dist2)
{
  int inside;
  double radius = vtkMath::Inf();
  double point[3] = { x[0], x[1], x[2] };
  this->FindClosestPointWithinRadius(
    point, radius, closestPoint, cell, cellId, subId, dist2, inside);
}

//------------------------------------------------------------------------------
vtkIdType vtkAbstractCellLocator::FindClosestPointWithinRadius(double x[3], double radius,
  double closestPoint[3], vtkGenericCell* cell, vtkIdType& cellId, int& subId, double& dist2)
{
  int inside;
  return this->FindClosestPointWithinRadius(
    x, radius, closestPoint, cell, cellId, subId, dist2, inside);
}

//------------------------------------------------------------------------------
vtkIdType vtkAbstractCellLocator::FindClosestPointWithinRadius(
  double x[3], double radius, double closestPoint[3], vtkIdType& cellId, int& subId, double& dist2)
{
  int inside;
  return this->FindClosestPointWithinRadius(
    x, radius, closestPoint, this->GenericCell, cellId, subId, dist2, inside);
}

//------------------------------------------------------------------------------
vtkIdType vtkAbstractCellLocator::FindClosestPointWithinRadius(double vtkNotUsed(x)[3],
  double vtkNotUsed(radius), double vtkNotUsed(closestPoint)[3], vtkGenericCell* vtkNotUsed(cell),
  vtkIdType& vtkNotUsed(cellId), int& vtkNotUsed(subId), double& vtkNotUsed(dist2),
  int& vtkNotUsed(inside))
{
  vtkErrorMacro(<< "The locator class - " << this->GetClassName()
                << " does not yet support FindClosestPointWithinRadius");
  return 0;
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::FindCellsWithinBounds(
  double* vtkNotUsed(bbox), vtkIdList* vtkNotUsed(cells))
{
  vtkErrorMacro(<< "The locator class - " << this->GetClassName()
                << " does not yet support FindCellsWithinBounds");
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::FindCellsAlongLine(
  const double p1[3], const double p2[3], double tolerance, vtkIdList* cells)
{
  this->IntersectWithLine(p1, p2, tolerance, nullptr, cells, nullptr);
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::FindCellsAlongPlane(const double vtkNotUsed(o)[3],
  const double vtkNotUsed(n)[3], double vtkNotUsed(tolerance), vtkIdList* vtkNotUsed(cells))
{
  vtkErrorMacro(<< "The locator " << this->GetClassName()
                << " does not yet support FindCellsAlongPlane");
}

//------------------------------------------------------------------------------
vtkIdType vtkAbstractCellLocator::FindCell(double x[3])
{
  this->UpdateInternalWeights();
  double dist2 = 0, pcoords[3];
  return this->FindCell(x, dist2, this->GenericCell, pcoords, this->Weights.data());
}

//------------------------------------------------------------------------------
vtkIdType vtkAbstractCellLocator::FindCell(
  double x[3], double tol2, vtkGenericCell* GenCell, double pcoords[3], double* weights)
{
  int subId;
  return this->FindCell(x, tol2, GenCell, subId, pcoords, weights);
}

//------------------------------------------------------------------------------
vtkIdType vtkAbstractCellLocator::FindCell(
  double x[3], double tol2, vtkGenericCell* GenCell, int& subId, double pcoords[3], double* weights)
{
  vtkIdType returnVal = -1;
  //
  static bool warning_shown = false;
  if (!warning_shown)
  {
    vtkWarningMacro(<< this->GetClassName() << " Does not implement FindCell"
                    << " Reverting to slow DataSet implementation");
    warning_shown = true;
  }
  //
  if (this->DataSet)
  {
    returnVal = this->DataSet->FindCell(x, nullptr, GenCell, 0, tol2, subId, pcoords, weights);
  }
  return returnVal;
}

//------------------------------------------------------------------------------
bool vtkAbstractCellLocator::InsideCellBounds(double x[3], vtkIdType cell_ID)
{
  if (this->CacheCellBounds && !this->LinearTransformationInfo.UseTransform)
  {
    return vtkAbstractCellLocator::IsInBounds(&this->CellBounds[cell_ID * 6], x);
  }
  else
  {
    double cellBounds[6];
    this->DataSet->GetCellBounds(cell_ID, cellBounds);
    return vtkAbstractCellLocator::IsInBounds(cellBounds, x);
  }
}

//------------------------------------------------------------------------------
bool vtkAbstractCellLocator::InsideCellBoundsInternal(double x[3], vtkIdType cell_ID)
{
  if (this->CacheCellBounds)
  {
    return vtkAbstractCellLocator::IsInBounds(&this->CellBounds[cell_ID * 6], x);
  }
  else
  {
    double cellBounds[6];
    this->DataSet->GetCellBounds(cell_ID, cellBounds);
    return vtkAbstractCellLocator::IsInBounds(cellBounds, x);
  }
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::GetCellBounds(vtkIdType cellId, double*& cellBoundsPtr)
{
  if (this->CacheCellBounds)
  {
    cellBoundsPtr = &this->CellBounds[cellId * 6];
  }
  else
  {
    this->DataSet->GetCellBounds(cellId, cellBoundsPtr);
  }
}

//------------------------------------------------------------------------------
void vtkAbstractCellLocator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Cache Cell Bounds: " << this->CacheCellBounds << "\n";
  os << indent << "Retain Cell Lists: " << (this->RetainCellLists ? "On\n" : "Off\n");
  os << indent << "Number of Cells Per Bucket: " << this->NumberOfCellsPerNode << "\n";
}
