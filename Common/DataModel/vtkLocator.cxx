/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLocator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkLocator.h"

#include "vtkArrayDispatch.h"
#include "vtkDataArrayRange.h"
#include "vtkDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkGarbageCollector.h"
#include "vtkImageData.h"
#include "vtkPointSet.h"
#include "vtkPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkSMPTools.h"
#include "vtkTransform.h"

#include "vtk_eigen.h"
#include VTK_EIGEN(Geometry)

//------------------------------------------------------------------------------
void vtkLocator::InitialPointsInformation::InverseTransformPointIfNeeded(
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
void vtkLocator::InitialPointsInformation::TransformPointIfNeeded(
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
void vtkLocator::InitialPointsInformation::InverseTransformNormalIfNeeded(
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
void vtkLocator::InitialPointsInformation::TransformNormalIfNeeded(
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
vtkCxxSetObjectMacro(vtkLocator, DataSet, vtkDataSet);

//------------------------------------------------------------------------------
vtkLocator::vtkLocator()
{
  this->DataSet = nullptr;
  this->Tolerance = 0.001;
  this->Automatic = 1;
  this->MaxLevel = 8;
  this->Level = 8;
  this->UseExistingSearchStructure = 0;
}

//------------------------------------------------------------------------------
vtkLocator::~vtkLocator()
{
  // commented out because of compiler problems in g++
  //  this->FreeSearchStructure();
  this->SetDataSet(nullptr);
}

//------------------------------------------------------------------------------
void vtkLocator::Initialize()
{
  // free up hash table
  this->FreeSearchStructure();
}

//------------------------------------------------------------------------------
void vtkLocator::Update()
{
  if (!this->DataSet)
  {
    vtkErrorMacro(<< "Input not set!");
    return;
  }
  if ((this->MTime > this->BuildTime) || (this->DataSet->GetMTime() > this->BuildTime))
  {
    this->BuildLocator();
  }
}

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
void vtkLocator::CopyInitialPoints()
{
  // copy the initial points
  this->InitialPointsInfo.Points->ShallowCopy(GetPoints(this->DataSet));
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
bool vtkLocator::ComputeTransformation()
{
  auto initialPointsData = this->InitialPointsInfo.Points->GetData();
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
      this->InitialPointsInfo.Transform, this->InitialPointsInfo.InverseTransform);
  }
  return validTransformation;
}

//------------------------------------------------------------------------------
void vtkLocator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  if (this->DataSet)
  {
    os << indent << "DataSet: " << this->DataSet << "\n";
  }
  else
  {
    os << indent << "DataSet: (none)\n";
  }

  os << indent << "Automatic: " << (this->Automatic ? "On\n" : "Off\n");
  os << indent << "Tolerance: " << this->Tolerance << "\n";
  os << indent << "Build Time: " << this->BuildTime.GetMTime() << "\n";
  os << indent << "MaxLevel: " << this->MaxLevel << "\n";
  os << indent << "Level: " << this->Level << "\n";
  os << indent << "UseExistingSearchStructure: " << this->UseExistingSearchStructure << "\n";
  os << indent << "SupportLinearTransformation: " << this->SupportLinearTransformation << "\n";
}

//------------------------------------------------------------------------------
void vtkLocator::ReportReferences(vtkGarbageCollector* collector)
{
  this->Superclass::ReportReferences(collector);
  vtkGarbageCollectorReport(collector, this->DataSet, "DataSet");
}
