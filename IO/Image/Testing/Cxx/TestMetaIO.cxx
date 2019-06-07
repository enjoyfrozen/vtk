/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestMetaIO.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Test of vtkMetaIO / MetaImage
// .SECTION Description
//

#include "vtkAnatomicalOrientation.h"
#include "vtkDataObject.h"
#include "vtkImageData.h"
#include "vtkImageMathematics.h"
#include "vtkMathUtilities.h"
#include "vtkMatrix3x3.h"
#include "vtkMetaImageReader.h"
#include "vtkMetaImageWriter.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkOutputWindow.h"

#include <cmath>
#include <string>

namespace
{
bool NearlyEquals(double arg1, double arg2, double epsilon)
{
  return std::fabs(arg1 - arg2) < epsilon;
}
bool NearlyEquals(double arg1, double arg2)
{
  return NearlyEquals(arg1, arg2, 1e-6);
}
} // anonymous namespace

int TestMetaIO(int argc, char* argv[])
{
  if (argc <= 1)
  {
    // Example: "TestMetaIO" "path/to/image.mhd" 1 0 0 0 1 0 0 0 1
    cout << "Usage: " << argv[0] << " <meta image file> [expectedDirectionLPS]" << endl;
    return 1;
  }

  const std::string temporaryImageFile = "TestMetaIO.mha";

  // Matrix elements for expected image direction in LPS anatomical space.
  // Must account for both transformation to LPS space and
  // orientation relative to LPS.
  vtkNew<vtkMatrix3x3> expectedDirection;
  if (argc >= 11)
  {
    for (int idx = 0; idx < 9; ++idx)
    {
      expectedDirection->SetElement(idx / 3, idx % 3, std::atof(argv[2 + idx]));
    }
  }
  else
  {
    expectedDirection->Identity();
  }

  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();
  cout << "10, 10, 10 : (1) : " << reader->GetOutput()->GetScalarComponentAsFloat(10, 10, 10, 0)
       << endl;
  cout << "24, 37, 10 : (168) : " << reader->GetOutput()->GetScalarComponentAsFloat(24, 37, 10, 0)
       << endl;

  vtkMetaImageWriter* writer = vtkMetaImageWriter::New();
  writer->SetFileName(temporaryImageFile.c_str());
  writer->SetInputConnection(reader->GetOutputPort());
  writer->Write();

  reader->Delete();
  writer->Delete();

  vtkMetaImageReader* readerStd = vtkMetaImageReader::New();
  readerStd->SetFileName(argv[1]);
  readerStd->Update();

  vtkMetaImageReader* readerNew = vtkMetaImageReader::New();
  readerNew->SetFileName(temporaryImageFile.c_str());
  readerNew->Update();

  double error = 0;
  int* ext = readerStd->GetOutput()->GetExtent();
  for (int z = ext[4]; z <= ext[5]; z += 2)
  {
    for (int y = ext[2]; y <= ext[3]; y++)
    {
      for (int x = ext[0]; x <= ext[1]; x++)
      {
        error += fabs(readerStd->GetOutput()->GetScalarComponentAsFloat(x, y, z, 0) -
          readerNew->GetOutput()->GetScalarComponentAsFloat(x, y, z, 0));
      }
    }
  }

  if (error > 1)
  {
    cerr << "Error: Image difference on read/write = " << error << endl;
    return 1;
  }

  cout << "Buffer passes tolerance. Error = " << error << endl;

  // Verify input direction matrix transforms from the parameterized input anatomical orientation
  // to the Left-Posterior-Superior (LPS) anatomical orientation assumed by VTK.
  auto inputDirection = readerStd->GetOutput()->GetDirectionMatrix();
  // Verify write/read returns same direction in LPS space.
  // Note that MetaImageWriter is implemented so that the image is always
  // written out with respect to LPS anatomical space.
  auto outputDirection = readerNew->GetOutput()->GetDirectionMatrix();
  bool isInputDirectionSame = true;
  bool isOutputDirectionSame = true;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      isInputDirectionSame = isInputDirectionSame &&
        NearlyEquals(expectedDirection->GetElement(i, j), inputDirection->GetElement(i, j));
      isOutputDirectionSame = isOutputDirectionSame &&
        NearlyEquals(expectedDirection->GetElement(i, j), outputDirection->GetElement(i, j));
    }
  }
  if (!isInputDirectionSame)
  {
    vtkGenericWarningMacro(<< "Input direction differs from expectation!");
    inputDirection->Print(std::cerr);
    expectedDirection->Print(std::cerr);
    return 1;
  }
  if (!isOutputDirectionSame)
  {
    vtkGenericWarningMacro(<< "Output direction differs from expectation!");
    outputDirection->Print(std::cerr);
    expectedDirection->Print(std::cerr);
    return 1;
  }

  readerStd->Delete();
  readerNew->Delete();

  return 0;
}
