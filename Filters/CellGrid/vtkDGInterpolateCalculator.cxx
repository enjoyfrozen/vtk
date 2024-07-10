// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkDGInterpolateCalculator.h"
#include "vtkCellAttribute.h"
#include "vtkCellGrid.h"
#include "vtkCellMetadata.h"
#include "vtkDGInvokeOperator.h"
#include "vtkDGOperatorEntry.h"
#include "vtkDoubleArray.h"
#include "vtkIdTypeArray.h"
#include "vtkObjectFactory.h"
#include "vtkTypeInt64Array.h"
#include "vtkVectorOperators.h"

#include "vtk_eigen.h"
#include VTK_EIGEN(Eigen)

#include <sstream>

using namespace vtk::literals;

VTK_ABI_NAMESPACE_BEGIN

vtkStandardNewMacro(vtkDGInterpolateCalculator);

void vtkDGInterpolateCalculator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  vtkIndent i2 = indent.GetNextIndent();
  os << indent << "FieldEvaluator:\n";
  this->FieldEvaluator.PrintSelf(os, i2);
  os << indent << "ShapeEvaluator:\n";
  this->ShapeEvaluator.PrintSelf(os, i2);
  os << indent << "FieldDerivative:\n";
  this->FieldDerivative.PrintSelf(os, i2);
  os << indent << "ShapeDerivative:\n";
  this->ShapeDerivative.PrintSelf(os, i2);

  os << indent << "FieldValues: " << this->FieldValues << "\n";
  os << indent << "ShapeConnectivity: " << this->ShapeConnectivity << "\n";
  os << indent << "ShapeValues: " << this->ShapeValues << "\n";
}

void vtkDGInterpolateCalculator::Evaluate(
  vtkIdType cellId, const vtkVector3d& rst, std::vector<double>& value)
{
  std::array<double, 3> arst{ rst[0], rst[1], rst[2] };
  // value.resize(this->FieldBasisOp.OperatorSize * );
  this->FieldEvaluator.Invoke(1, &cellId, arst.data(), value.data());

  // Now, for H(curl) elements, transform the resulting vectors by the inverse Jacobian
  // (of the cell's *shape* function).
  static std::unordered_set<vtkStringToken> isCurl( { "HCURL"_token, "HCurl"_token, "Hcurl"_token,
    "hcurl"_token } );
  if (isCurl.find(this->FieldCellInfo.FunctionSpace) != isCurl.end())
  {
    static thread_local std::vector<double> spatialDeriv;
    this->ShapeDerivative.Invoke(1, &cellId, , arst.data(), spatialDeriv);

    // Use spatialDeriv as Jacobian and solve J * xx = value (which transforms "value"
    // from the parameter space into world coordinates), then write the results
    // back into value.
    Eigen::Map<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>> map(spatialDeriv.data());
    // Eigen::HouseholderQR<Eigen::Matrix3d> solver(map);
    std::size_t numValueVectors = value.size() / 3;
    for (std::size_t ii = 0; ii < numValueVectors; ++ii)
    {
      Eigen::Vector3d edelt(value[3 * ii + 0], value[3 * ii + 1], value[3 * ii + 2]);
      auto xx = map * edelt;
      // auto xx = solver.solve(edelt);
      for (int jj = 0; jj < 3; ++jj)
      {
        value[3 * ii + jj] = xx[jj];
      }
    }
  }

  // For H(div) elements, transform the resulting vectors by the forward Jacobian
  // (of the cell's *shape* function) divided by the determinant of the Jacobian.
  static std::unordered_set<vtkStringToken> isDiv( { "HDIV"_token, "HDiv"_token, "Hdiv"_token,
    "hdiv"_token } );
  if (isDiv.find(this->FieldCellInfo.FunctionSpace) != isDiv.end())
  {
    static thread_local std::vector<double> jac;
    this->ShapeDerivative(1, &cellId, arst, jac.data());

    double jdet;
    double diag = std::abs(jac[0]) + std::abs(jac[4]) + std::abs(jac[8]);
    if (this->Dimension == 3)
    {
      jdet =
        jac[0] * (jac[4] * jac[8] - jac[7] * jac[5]) -
        jac[1] * (jac[3] * jac[8] - jac[6] * jac[5]) +
        jac[2] * (jac[3] * jac[7] - jac[6] * jac[4]);
    }
    else
    {
      jdet = jac[0] * jac[4] - jac[3] * jac[1];
    }
    double ijdet = (std::abs(jdet/diag) > 1e-7) ? 1.0 / jdet : 0.0;

    std::size_t numValueVectors = value.size() / 3;
    for (std::size_t ii = 0; ii < numValueVectors; ++ii)
    {
      std::array<double, 3> xx{{ value[3 * ii], value[3 * ii + 1], value[3 * ii + 2] }};
      value[3 * ii    ] = (jac[0] * xx[0] + jac[1] * xx[1] + jac[2] * xx[2]) * ijdet;
      value[3 * ii + 1] = (jac[3] * xx[0] + jac[4] * xx[1] + jac[5] * xx[2]) * ijdet;
      value[3 * ii + 2] = (jac[6] * xx[0] + jac[7] * xx[1] + jac[8] * xx[2]) * ijdet;
    }
  }
}

void vtkDGInterpolateCalculator::Evaluate(
  vtkIdTypeArray* cellIds, vtkDataArray* rst, vtkDataArray* result)
{
  vtkDoubleArray* drst = vtkDoubleArray::SafeDownCast(rst);
  // Convert parameters to doubles as needed.
  if (!drst)
  {
    this->LocalRST->DeepCopy(rst);
    drst = this->LocalRST.GetPointer();
  }
  vtkDoubleArray* dresult = vtkDoubleArray::SafeDownCast(result);
  if (!dresult)
  {
    dresult = this->LocalField.GetPointer();
  }

  assert(cellIds->GetNumberOfTuples() == rst->GetNumberOfTuples());

  vtkIdType numEvals = cellIds->GetNumberOfTuples();
  dresult->SetNumberOfComponents(this->Field->GetNumberOfComponents());
  dresult->SetNumberOfTuples(cellIds->GetNumberOfTuples());
  this->FieldEvaluator.Invoke(numEvals, cellIds->GetPointer(0), drst->GetPointer(0),
    dresult->GetPointer(0));

  // Now, for H(curl) elements, transform the resulting vectors by the inverse Jacobian
  // (of the cell's *shape* function).
  static std::unordered_set<vtkStringToken> isCurl( { "HCURL"_token, "HCurl"_token, "Hcurl"_token,
    "hcurl"_token } );
  if (isCurl.find(this->FieldCellInfo.FunctionSpace) != isCurl.end())
  {
    static thread_local std::vector<double> spatialDeriv;
    spatialDeriv.resize(9 * numEvals);
    this->ShapeDerivative.Invoke(
      numEval, cellIds->GetPointer(0), drst->GetPointer(0), spatialDeriv.data());

    // Use spatialDeriv as Jacobian and solve J * xx = value (which transforms "value"
    // from the parameter space into world coordinates), then write the results
    // back into value.
    vtkIdType numVecPerEval = this->Field->GetNumberOfComponents() / 3;
    // We could parallelize this, but vtkDGInterpolateCalculator is intended to be
    // called from within threaded code, which could cause problems.
    for (vtkIdType ee = 0; ee < numEvals; ++ee)
    {
      Eigen::Map<Eigen::Matrix<double, 3, 3, Eigen::RowMajor>> map(spatialDeriv.data() + 9 * ee);
      Eigen::HouseholderQR<Eigen::Matrix3d> solver(map);
      for (vtkIdType vv = 0; vv < numVecPerEval; ++vv)
      {
        Eigen::Vector3d edelt;
        dresult->GetTuple(vv, edelt.data());
        auto xx = solver.solve(edelt);
        dresult->SetTuple(vv, xx.data());
      }
    }
  }

  // For H(div) elements, transform the resulting vectors by the forward Jacobian
  // (of the cell's *shape* function) divided by the determinant of the Jacobian.
  static std::unordered_set<vtkStringToken> isDiv( { "HDIV"_token, "HDiv"_token, "Hdiv"_token,
    "hdiv"_token } );
  if (isDiv.find(this->FieldCellInfo.FunctionSpace) != isDiv.end())
  {
    static thread_local std::vector<double> spatialDeriv;
    spatialDeriv.resize(9 * numEvals);
    this->ShapeDerivative.Invoke(
      numEval, cellIds->GetPointer(0), drst->GetPointer(0), spatialDeriv.data());
    // Evaluate the determinant of each Jacobian matrix in spatialDeriv and
    // transform the vectors computed above.
    vtkIdType numVecPerEval = this->Field->GetNumberOfComponents() / 3;
    // We could parallelize this, but vtkDGInterpolateCalculator is intended to be
    // called from within threaded code, which could cause problems.
    for (vtkIdType ee = 0; ee < numEvals; ++ee)
    {
      double* jac = spatialDeriv.data() + 9 * ee;
      double jdet;
      double diag = std::abs(jac[0]) + std::abs(jac[4]) + std::abs(jac[8]);
      if (this->Dimension == 3)
      {
        jdet =
          jac[0] * (jac[4] * jac[8] - jac[7] * jac[5]) -
          jac[1] * (jac[3] * jac[8] - jac[6] * jac[5]) +
          jac[2] * (jac[3] * jac[7] - jac[6] * jac[4]);
      }
      else
      {
        jdet = jac[0] * jac[4] - jac[3] * jac[1];
      }
      double ijdet = (std::abs(jdet/diag) > 1e-7) ? 1.0 / jdet : 0.0;

      double* value = dresult->GetPointer(0) + ee * numVecPerEval * 3;
      for (int vv = 0; vv < numVecPerEval; ++vv)
      {
        std::array<double, 3> xx{{ value[3 * vv], value[3 * vv + 1], value[3 * vv + 2] }};
        value[3 * vv    ] = (jac[0] * xx[0] + jac[1] * xx[1] + jac[2] * xx[2]) * ijdet;
        value[3 * vv + 1] = (jac[3] * xx[0] + jac[4] * xx[1] + jac[5] * xx[2]) * ijdet;
        value[3 * vv + 2] = (jac[6] * xx[0] + jac[7] * xx[1] + jac[8] * xx[2]) * ijdet;
      }
    }
  }

  // Finally, if we were given a non-vtkDoubleArray, copy the results
  // back into the output array.
  if (dresult != result)
  {
    result->DeepCopy(dresult);
  }
}

bool vtkDGInterpolateCalculator::AnalyticDerivative() const
{
  // XXX(c++14)
#if __cplusplus < 201400L
  if (this->FieldCellInfo.FunctionSpace == "HGRAD"_token)
  {
    return true;
  }
#else
  switch (this->FieldCellInfo.FunctionSpace.GetId())
  {
    case "HGRAD"_hash:
      return true;
    default:
      break;
  }
#endif
  return false;
}

void vtkDGInterpolateCalculator::EvaluateDerivative(
  vtkIdType cellId, const vtkVector3d& rst, std::vector<double>& jacobian, double neighborhood)
{
  if (!this->FieldGradientOp || !this->AnalyticDerivative())
  {
    // We don't have an analytic derivative; approximate it.
    return this->Superclass::EvaluateDerivative(cellId, rst, jacobian, neighborhood);
  }

  this->FieldDerivative.Invoke(1, &cellId, rst.GetData(), jacobian.data());
  // TODO: Transform gradient if needed by HCurl/HDiv shape-gradient?
  //       That is already approximated above because of AnalyticDerivative()…
}

void vtkDGInterpolateCalculator::EvaluateDerivative(
  vtkIdTypeArray* cellIds, vtkDataArray* rst, vtkDataArray* result)
{
  if (!this->FieldGradientOp || !this->AnalyticDerivative())
  {
    // We don't have an analytic derivative; approximate it.
    return this->Superclass::EvaluateDerivative(cellIds, rst, result);
  }

  vtkIdType numEvals = cellIds->GetNumberOfTuples();
  vtkDoubleArray* drst = vtkDoubleArray::SafeDownCast(rst);
  // Convert parameters to doubles as needed.
  if (!drst)
  {
    this->LocalRST->DeepCopy(rst);
    drst = this->LocalRST.GetPointer();
  }
  vtkDoubleArray* dresult = vtkDoubleArray::SafeDownCast(result);
  if (!dresult)
  {
    dresult = this->LocalField.GetPointer();
  }

  this->FieldDerivative.Invoke(numEvals, cellIds, drst, dresult);

  if (dresult != result)
  {
    result->DeepCopy(dresult);
  }
}

vtkSmartPointer<vtkCellAttributeCalculator> vtkDGInterpolateCalculator::PrepareForGrid(
  vtkCellMetadata* cell, vtkCellAttribute* field)
{
  auto* dgCell = vtkDGCell::SafeDownCast(cell);
  if (!dgCell)
  {
    return nullptr;
  }
  if (!field)
  {
    return nullptr;
  }

  // Clone ourselves for this new context.
  vtkNew<vtkDGInterpolateCalculator> result;

  auto* grid = cell->GetCellGrid();
  auto* shape = grid->GetShapeAttribute();
  vtkStringToken cellType = dgCell->GetClassName();
  auto shapeCellInfo = shape->GetCellTypeInfo(cellType);
  // Shape functions must be (1) continuous and (2) have HGRAD/Lagrange basis
  // or, if the shape is a vtkDGVert, be constant and have a trivially null gradient.
  if ((cellType != "vtkDGVert"_token && shapeCellInfo.FunctionSpace != "HGRAD"_token &&
        shapeCellInfo.FunctionSpace != "Lagrange"_token) ||
    (cellType == "vtkDGVert"_token && shapeCellInfo.FunctionSpace != "constant"_token))
  {
    vtkErrorMacro("Unsupported combination of cell shape function "
      << "space \"" << shapeCellInfo.FunctionSpace.Data() << "\" and/or "
      << "DOF sharing (" << (shapeCellInfo.DOFSharing.IsValid() ? "C" : "D") << ").");
    return nullptr;
  }
  result->ShapeBasisOp = dgCell->GetOperatorEntry("Basis"_token, shapeCellInfo);
  result->ShapeGradientOp = dgCell->GetOperatorEntry("BasisGradient"_token, shapeCellInfo);
  result->ShapeCellInfo = shapeCellInfo;

  auto fieldCellInfo = field->GetCellTypeInfo(cellType);
  result->Field = field;
  result->FieldBasisOp = dgCell->GetOperatorEntry("Basis"_token, fieldCellInfo);
  result->FieldGradientOp = dgCell->GetOperatorEntry("BasisGradient"_token, fieldCellInfo);
  result->FieldCellInfo = fieldCellInfo;

  result->Dimension = dgCell->GetDimension();
  result->CellShape = dgCell->GetShape();

  auto& shapeArrays = shapeCellInfo.ArraysByRole;
  result->ShapeValues = vtkDataArray::SafeDownCast(shapeArrays["values"_token]);
  result->ShapeConnectivity = shapeCellInfo.DOFSharing.IsValid()
    ? vtkDataArray::SafeDownCast(shapeArrays["connectivity"_token])
    : nullptr;

  auto& fieldArrays = fieldCellInfo.ArraysByRole;
  result->FieldValues = vtkDataArray::SafeDownCast(fieldArrays["values"_token]);
  result->FieldConnectivity = fieldCellInfo.DOFSharing.IsValid()
    ? vtkDataArray::SafeDownCast(fieldArrays["connectivity"_token])
    : nullptr;

  if (shapeCellInfo.DOFSharing.IsValid())
  {
    if (!result->ShapeConnectivity || !result->ShapeConnectivity->IsIntegral())
    {
      vtkErrorMacro("Shape connectivity array must exist and be integer-valued.");
      return nullptr;
    }
  }

  if (fieldCellInfo.DOFSharing.IsValid())
  {
    if (!result->FieldConnectivity || !result->FieldConnectivity->IsIntegral())
    {
      vtkErrorMacro("Field connectivity array must exist and be integer-valued.");
      return nullptr;
    }
  }

  return result;
}

VTK_ABI_NAMESPACE_END
