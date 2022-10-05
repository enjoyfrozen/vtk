/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCylindricalGridSource.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/

#include "vtkCylindricalGridSource.h"

#include "vtkCellType.h"
#include "vtkIdList.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include <cmath>
#include <map>

class vtkCylindricalGridSource::Impl
{
public:
  Impl() = default;
  ~Impl() = default;

  //@{
  /**
   * Calculates the number of intermediate points which need to be added to ensure all intervals are
   * smaller than the max angle.
   */
  static int CalculateNumberOfIntermediatePointsRequired(double p1, double p2, double maximumAngle);
  //@}

  //@{
  /**
   * Sanitizes polar angles to within a single revolution.
   *
   * If a cell is defined as existing from 390 -> 420 degrees, it simplifies the math to know that
   * is actually starting at 30 degrees.
   */
  static double LocatePolarAngleInFirstRevolution(double p1);
  //@}

  static constexpr double TWO_PI_PRECISION_FACTOR =
    1.9999999; // to avoid point duplication with precision errors

  struct Point
  {
    double x, y, z;
    bool operator<(const Point& rhs) const
    {
      if (x != rhs.x)
      {
        return x < rhs.x;
      }
      if (y != rhs.y)
      {
        return y < rhs.y;
      }
      return z < rhs.z;
    }
  };
  typedef std::map<Point, vtkIdType> UniquePointIndexLookupTable;

  static vtkIdType CreateUniquePoint(double r, double p, double z,
    UniquePointIndexLookupTable& pointLookup, vtkSmartPointer<vtkPoints> points);
  static std::vector<vtkIdType> CreatePointLine(double r, double p1, double p2, double z,
    int numIntermediatePoints, UniquePointIndexLookupTable& pointLookup,
    vtkSmartPointer<vtkPoints> points);
};

int vtkCylindricalGridSource::Impl::CalculateNumberOfIntermediatePointsRequired(
  double p1, double p2, double maximumAngle)
{
  double delta = p2 - p1;
  double numberOfSegmentsRequired = std::ceil(std::fabs(delta / maximumAngle));
  return std::max(0, static_cast<int>(numberOfSegmentsRequired - 1));
}

double vtkCylindricalGridSource::Impl::LocatePolarAngleInFirstRevolution(double p1)
{
  // Remove Multiple Revolutions
  double pi2 = 2.0 * vtkMath::Pi();
  vtkIdType numberOfRevolutions = static_cast<vtkIdType>(p1 / pi2);
  p1 -= static_cast<double>(numberOfRevolutions) * pi2;

  // Correct for first rotation negative values and numerical precision cases.
  if (p1 < 0.0)
  {
    p1 += pi2;
  }

  return p1;
}

vtkIdType vtkCylindricalGridSource::Impl::CreateUniquePoint(double r, double p, double z,
  vtkCylindricalGridSource::Impl::UniquePointIndexLookupTable& pointLookup,
  vtkSmartPointer<vtkPoints> points)
{
  p = LocatePolarAngleInFirstRevolution(p);

  Point point{ r, p, z };
  auto itr = pointLookup.find(point);
  if (itr != pointLookup.end())
  {
    return itr->second;
  }
  auto pointID = pointLookup.size();
  pointLookup[point] = pointID;

  double x = r * cos(p);
  double y = r * sin(p);
  points->InsertNextPoint(x, y, z);
  return pointID;
}

std::vector<vtkIdType> vtkCylindricalGridSource::Impl::CreatePointLine(double r, double p1,
  double p2, double z, int numIntermediatePoints,
  vtkCylindricalGridSource::Impl::UniquePointIndexLookupTable& pointLookup,
  vtkSmartPointer<vtkPoints> points)
{
  std::vector<vtkIdType> pointIDs;
  pointIDs.push_back(CreateUniquePoint(r, p1, z, pointLookup, points));

  if (numIntermediatePoints > 0)
  {
    auto delta = (p2 - p1) / static_cast<double>(numIntermediatePoints + 1);
    for (int intPointIndex = 0; intPointIndex < numIntermediatePoints; intPointIndex++)
    {
      auto pInt = p1 + static_cast<double>(intPointIndex + 1) * delta;
      if (std::fabs(pInt - p1) <= TWO_PI_PRECISION_FACTOR * vtkMath::Pi())
      {
        pointIDs.push_back(CreateUniquePoint(r, pInt, z, pointLookup, points));
      }
      else
      {
        pointIDs.push_back(*pointIDs.begin());
      }
    }
  }

  if (std::fabs(p2 - p1) <= TWO_PI_PRECISION_FACTOR * vtkMath::Pi())
  {
    pointIDs.push_back(CreateUniquePoint(r, p2, z, pointLookup, points));
  }
  else
  {
    pointIDs.push_back(*pointIDs.begin());
  }

  return pointIDs;
}

vtkStandardNewMacro(vtkCylindricalGridSource);
vtkCylindricalGridSource::vtkCylindricalGridSource()
  : Grid(vtkSmartPointer<vtkUnstructuredGrid>::New())
  , UseDegrees(true)
{
  this->SetNumberOfInputPorts(0);

  // Set the default maximum angle to disable inserting intermediate points. Set slightly
  // larger than one revolution to avoid precision errors on 360 degree cells.
  SetMaximumAngle(361);
}

int vtkCylindricalGridSource::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  // get the output info object
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // get the output
  vtkUnstructuredGrid* output =
    vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // load grid
  output->ShallowCopy(this->Grid);

  return 1;
}

void vtkCylindricalGridSource::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "MaximumAngle: " << this->MaximumAngle << endl;
  os << indent << "UseDegrees: " << this->UseDegrees << endl;
}

void vtkCylindricalGridSource::SetMaximumAngle(double maxAngle)
{
  if (this->UseDegrees)
  {
    maxAngle = vtkMath::RadiansFromDegrees(maxAngle);
  }

  this->MaximumAngle = std::max(
    vtkMath::RadiansFromDegrees(0.1), std::min(maxAngle, vtkMath::RadiansFromDegrees(361.0)));
}

double vtkCylindricalGridSource::GetMaximumAngle()
{
  if (this->UseDegrees)
  {
    return vtkMath::DegreesFromRadians(this->MaximumAngle);
  }
  return this->MaximumAngle;
}

void vtkCylindricalGridSource::InsertCylindricalCell(
  double r1, double r2, double p1, double p2, double z1)
{
  // Initialize Parameters
  if (this->UseDegrees)
  {
    p1 = vtkMath::RadiansFromDegrees(p1);
    p2 = vtkMath::RadiansFromDegrees(p2);
  }

  auto numIntermediatePoints =
    Impl::CalculateNumberOfIntermediatePointsRequired(p1, p2, this->MaximumAngle);

  // Create Data Structures Required
  auto points = this->Grid->GetPoints();
  if (points == nullptr)
  {
    vtkNew<vtkPoints> newPoints;
    this->Grid->SetPoints(newPoints);
    points = this->Grid->GetPoints();
  }
  auto cpo = points->GetNumberOfPoints(); // cell point offset

  // Generate all the required points for the cell and list of indices for making faces
  Impl::UniquePointIndexLookupTable uniquePoints;
  auto innerPoints =
    Impl::CreatePointLine(r1, p1, p2, z1, numIntermediatePoints, uniquePoints, points); // Inner
  auto outerPoints =
    Impl::CreatePointLine(r2, p2, p1, z1, numIntermediatePoints, uniquePoints, points); // Outer

  std::vector<vtkIdType> pointIDs;
  pointIDs.insert(pointIDs.end(), innerPoints.begin(), innerPoints.end());
  pointIDs.insert(pointIDs.end(), outerPoints.begin(), outerPoints.end());

  // Prepare to make faces
  vtkIdType edgePoints = numIntermediatePoints + 2;
  std::vector<vtkIdType> faces;
  faces.push_back(edgePoints - 1);

  // Face
  for (vtkIdType i = 0; i < edgePoints - 1; i++)
  {
    faces.push_back(4);
    faces.push_back(cpo + pointIDs[i]);
    faces.push_back(cpo + pointIDs[i + 1]);
    faces.push_back(cpo + pointIDs[(2 * edgePoints - 1) - i - 1]);
    faces.push_back(cpo + pointIDs[(2 * edgePoints - 1) - i]);
  }

  // Save Cells in Grid
  vtkNew<vtkIdList> faceList;
  for (auto& face : faces)
  {
    faceList->InsertNextId(face);
  }
  this->Grid->InsertNextCell(VTK_POLYHEDRON, faceList);
}

void vtkCylindricalGridSource::InsertCylindricalCell(
  double r1, double r2, double p1, double p2, double z1, double z2)
{
  // Initialize Parameters
  if (this->UseDegrees)
  {
    p1 = vtkMath::RadiansFromDegrees(p1);
    p2 = vtkMath::RadiansFromDegrees(p2);
  }

  auto numIntermediatePoints =
    Impl::CalculateNumberOfIntermediatePointsRequired(p1, p2, this->MaximumAngle);

  // Create Data Structures Required
  auto points = this->Grid->GetPoints();
  if (points == nullptr)
  {
    vtkNew<vtkPoints> newPoints;
    this->Grid->SetPoints(newPoints);
    points = this->Grid->GetPoints();
  }
  auto cpo = points->GetNumberOfPoints(); // cell point offset

  // Generate all the required points for the cell and list of indices for making faces
  Impl::UniquePointIndexLookupTable uniquePoints;
  auto innerBottomPoints = Impl::CreatePointLine(
    r1, p1, p2, z1, numIntermediatePoints, uniquePoints, points); // Inner Bottom
  auto outerBottomPoints = Impl::CreatePointLine(
    r2, p2, p1, z1, numIntermediatePoints, uniquePoints, points); // Outer Bottom
  auto innerTopPoints =
    Impl::CreatePointLine(r1, p1, p2, z2, numIntermediatePoints, uniquePoints, points); // Inner Top
  auto outerTopPoints =
    Impl::CreatePointLine(r2, p2, p1, z2, numIntermediatePoints, uniquePoints, points); // Outer Top

  std::vector<vtkIdType> pointIDs;
  pointIDs.insert(pointIDs.end(), innerBottomPoints.begin(), innerBottomPoints.end());
  pointIDs.insert(pointIDs.end(), outerBottomPoints.begin(), outerBottomPoints.end());
  pointIDs.insert(pointIDs.end(), innerTopPoints.begin(), innerTopPoints.end());
  pointIDs.insert(pointIDs.end(), outerTopPoints.begin(), outerTopPoints.end());

  // Prepare to make faces
  vtkIdType edgePoints = numIntermediatePoints + 2;
  std::vector<vtkIdType> faces;
  auto facesTopBottom = 2 * (edgePoints - 1);
  auto facesInsideOutside = 2 * (edgePoints - 1);
  auto facesEnds =
    ((p2 - p1) >= (vtkCylindricalGridSource::Impl::TWO_PI_PRECISION_FACTOR * vtkMath::Pi())) ? 0
                                                                                             : 2;
  faces.push_back(facesTopBottom + facesInsideOutside + facesEnds);

  // Bottom
  for (vtkIdType i = 0; i < edgePoints - 1; i++)
  {
    faces.push_back(4);
    faces.push_back(cpo + pointIDs[i]);
    faces.push_back(cpo + pointIDs[i + 1]);
    faces.push_back(cpo + pointIDs[(2 * edgePoints - 1) - i - 1]);
    faces.push_back(cpo + pointIDs[(2 * edgePoints - 1) - i]);
  }

  // Top
  for (vtkIdType i = 0; i < edgePoints - 1; i++)
  {
    faces.push_back(4);
    faces.push_back(cpo + pointIDs[(2 * edgePoints) + i]);
    faces.push_back(cpo + pointIDs[(2 * edgePoints) + i + 1]);
    faces.push_back(cpo + pointIDs[(4 * edgePoints - 1) - i - 1]);
    faces.push_back(cpo + pointIDs[(4 * edgePoints - 1) - i]);
  }

  // Inside
  for (vtkIdType i = 0; i < edgePoints - 1; i++)
  {
    auto bottom = i;
    auto top = 2 * edgePoints + i;
    faces.push_back(4);
    faces.push_back(cpo + pointIDs[bottom]);
    faces.push_back(cpo + pointIDs[bottom + 1]);
    faces.push_back(cpo + pointIDs[top + 1]);
    faces.push_back(cpo + pointIDs[top]);
  }

  // Outside
  for (vtkIdType i = 0; i < edgePoints - 1; i++)
  {
    auto bottom = edgePoints + i;
    auto top = 3 * edgePoints + i;
    faces.push_back(4);
    faces.push_back(cpo + pointIDs[bottom]);
    faces.push_back(cpo + pointIDs[bottom + 1]);
    faces.push_back(cpo + pointIDs[top + 1]);
    faces.push_back(cpo + pointIDs[top]);
  }

  // Caps -> If the cell is not a complete rotation, it needs side faces.
  // Omit if the cell completes a full rotation to avoid inserting unnecessary internal geometry
  if (facesEnds)
  {
    faces.push_back(4);
    faces.push_back(cpo + pointIDs[0]);
    faces.push_back(cpo + pointIDs[2 * edgePoints]);
    faces.push_back(cpo + pointIDs[4 * edgePoints - 1]);
    faces.push_back(cpo + pointIDs[2 * edgePoints - 1]);

    faces.push_back(4);
    faces.push_back(cpo + pointIDs[edgePoints - 1]);
    faces.push_back(cpo + pointIDs[edgePoints]);
    faces.push_back(cpo + pointIDs[3 * edgePoints]);
    faces.push_back(cpo + pointIDs[3 * edgePoints - 1]);
  }

  // Save Cells in Grid
  vtkNew<vtkIdList> faceList;
  for (auto& face : faces)
  {
    faceList->InsertNextId(face);
  }
  this->Grid->InsertNextCell(VTK_POLYHEDRON, faceList);
}
