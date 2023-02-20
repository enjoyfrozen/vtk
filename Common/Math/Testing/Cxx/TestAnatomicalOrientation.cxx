/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestAmoebaMinimizer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkAnatomicalOrientation.h"
#include "vtkNew.h"
#include "vtkObject.h"
#include "vtkSetGet.h"

#include <cmath>
#include <sstream>
#include <vector>

bool AssertInverseAxis(
  const vtkAnatomicalOrientation::Axis& first, const vtkAnatomicalOrientation::Axis& second);

namespace
{
constexpr bool SUCCESS = true;
constexpr bool FAILURE = false;
} // anonymous namespace

int TestAnatomicalOrientation(int argc, char*[])
{
  bool result = true; // Assume passing, try to disprove

  auto defaultOrientation = vtkAnatomicalOrientation::LPS;

  // Verify static definitions are valid
  std::vector<vtkAnatomicalOrientation> defaultOrientations = {
    vtkAnatomicalOrientation::LPS,
    vtkAnatomicalOrientation::RAS,
    vtkAnatomicalOrientation::LAS,
  };

  for (auto& orientation : defaultOrientations)
  {
    if (!orientation.IsValid())
    {
      std::ostringstream warning;
      warning << std::string("Invalid orientation: ") << orientation << std::endl;
      vtkGenericWarningMacro(<< warning.str());
      result = FAILURE;
    }
  }

  // Verify axis relations
  using Axis = vtkAnatomicalOrientation::Axis;
  result = result && AssertInverseAxis(Axis::L, Axis::R);
  result = result && AssertInverseAxis(Axis::P, Axis::A);
  result = result && AssertInverseAxis(Axis::S, Axis::I);

  // Verify default construction
  vtkAnatomicalOrientation orientation;
  if (orientation.IsValid() || "---" != orientation.GetAsAcronym())
  {
    vtkGenericWarningMacro(<< "Expected null orientation from default constructor but found "
                           << result);
    result = FAILURE;
  }

  // Verify setting axes
  orientation.SetForAcronym("LAS");
  if (!orientation.IsValid() || "LAS" != orientation.GetAsAcronym())
  {
    vtkGenericWarningMacro(<< "Expected LAS orientation but found " << orientation);
    result = FAILURE;
  }

  orientation.SetForString("right-Anterior-superior", '-');
  if (!orientation.IsValid() || "RAS" != orientation.GetAsAcronym())
  {
    vtkGenericWarningMacro(<< "Expected RAS orientation but found " << orientation);
    result = FAILURE;
  }

  // Verify orientation comparison
  if (vtkAnatomicalOrientation::RAS != orientation)
  {
    vtkGenericWarningMacro(<< "Expected orientations to match: " << orientation << ", "
                           << vtkAnatomicalOrientation::RAS);
    result = FAILURE;
  }
  if (vtkAnatomicalOrientation::LPS == vtkAnatomicalOrientation::RAS)
  {
    vtkGenericWarningMacro(<< "Expected orientation comparison to fail but it succeeded: "
                           << vtkAnatomicalOrientation::LPS << " versus "
                           << vtkAnatomicalOrientation::RAS);
    result = FAILURE;
  }

  // Verify accessors
  if (orientation[0] != orientation.X || orientation[1] != orientation.Y ||
    orientation[2] != orientation.Z)
  {
    vtkGenericWarningMacro(<< "Orientation index vs explicit accessors differ");
    result = FAILURE;
  }

  // Verify transformation between spaces
  constexpr int TRANSFORM_SIZE = 9;
  double lpsToRas[TRANSFORM_SIZE];
  double expectedLpsToRas[TRANSFORM_SIZE] = { -1, 0, 0, 0, -1, 0, 0, 0, 1 };
  vtkAnatomicalOrientation::LPS.GetTransformTo(vtkAnatomicalOrientation::RAS, lpsToRas);
  bool isSameTransform = true;
  for (unsigned int idx = 0; idx < TRANSFORM_SIZE; ++idx)
  {
    isSameTransform = isSameTransform && (lpsToRas[idx] == expectedLpsToRas[idx]);
  }
  if (!isSameTransform)
  {
    vtkGenericWarningMacro(<< "Transform from LPS to RAS space differs from expectation!");
    result = FAILURE;
  }

  double iarToLps[TRANSFORM_SIZE];
  double expectedIarToLps[TRANSFORM_SIZE] = { 0, 0, -1, 0, -1, 0, -1, 0, 0 };
  orientation.SetForAcronym("IAR");
  orientation.GetTransformTo(vtkAnatomicalOrientation::LPS, iarToLps);
  isSameTransform = true;
  for (unsigned int idx = 0; idx < TRANSFORM_SIZE; ++idx)
  {
    isSameTransform = isSameTransform && (iarToLps[idx] == expectedIarToLps[idx]);
  }
  if (!isSameTransform)
  {
    vtkGenericWarningMacro(<< "Transform from IAR to LPS space differs from expectation!");
    result = FAILURE;
  }

  return (result == SUCCESS ? 0 : 1);
}

// Verify two enumerated anatomical axes are inverses of each other
bool AssertInverseAxis(
  const vtkAnatomicalOrientation::Axis& first, const vtkAnatomicalOrientation::Axis& second)
{
  if (first != vtkAnatomicalOrientation::AxisInverse(second) ||
    second != vtkAnatomicalOrientation::AxisInverse(first))
  {
    vtkGenericWarningMacro(<< "Expected inverse axes "
                           << vtkAnatomicalOrientation::AxisToChar(first) << " and "
                           << vtkAnatomicalOrientation::AxisToChar(second));
    return FAILURE;
  }
  else
  {
    return SUCCESS;
  }
}
