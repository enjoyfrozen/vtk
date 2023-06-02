/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestCategoricalColors.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkColorSeries.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkLogger.h"
#include "vtkLookupTable.h"
#include "vtkNew.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkUnsignedCharArray.h"

#include <map>
#include <numeric>
#include <random>
#include <sstream>
#include <string>

#include <cstdio> // For EXIT_SUCCESS

namespace
{
//------------------------------------------------------------------------------
//! Get a hexadecimal string of the RGBA colors.
std::string RGBAToHexString(const unsigned char* rgba)
{
  std::ostringstream os;
  for (int i = 0; i < 4; ++i)
  {
    os << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(rgba[i]);
  }
  return os.str();
}

bool vtkTest(const std::string& val, const unsigned char* rgba, const std::string& expectedColor)
{
  auto v = "0x" + RGBAToHexString(rgba);
  if (expectedColor != v)
  {
    vtkLog(ERROR, "For data '" << val << "', got " << v << ", expected " << expectedColor);
    return false;
  }
  return true;
}

bool DoPerformanceTest(vtkLookupTable* lut, int numAnnotations, vtkIdType numScalars)
{
  lut->ResetAnnotations();
  vtkNew<vtkIntArray> values;
  vtkNew<vtkStringArray> labels;
  values->SetNumberOfTuples(numAnnotations);
  labels->SetNumberOfTuples(numAnnotations);
  for (int cc = 0; cc < numAnnotations; ++cc)
  {
    values->SetValue(cc, cc);
    labels->SetValue(cc, "annotation_" + std::to_string(cc));
  }
  lut->SetAnnotations(values, labels);

  vtkNew<vtkDoubleArray> data;
  data->SetNumberOfTuples(numScalars);

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, numAnnotations - 1);
  std::generate_n(data->GetPointer(0), numScalars, [&]() { return dis(gen); });

  {
    vtkLogScopeF(INFO, "MapScalars");
    auto color = vtk::TakeSmartPointer(lut->MapScalars(data, VTK_RGBA, 0));
    return color != nullptr;
  }
}
}

int TestCategoricalColors(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  bool res = true;
  // Create the LUT and add some annotations.
  vtkNew<vtkLookupTable> lut;
  lut->SetAnnotation(0., "Zero");
  lut->SetAnnotation(.5, "Half");
  lut->SetAnnotation(1., "Ichi");
  lut->SetAnnotation(1., "One");
  lut->SetAnnotation(2., "Ni");
  lut->SetAnnotation(2., "Two");
  lut->SetAnnotation(3, "San");
  lut->SetAnnotation(4, "Floor");
  lut->SetAnnotation(5, "Hive");
  lut->SetAnnotation(6, "Licks");
  lut->SetAnnotation(7, "Leaven");
  lut->SetAnnotation(9, "Kyuu");
  lut->RemoveAnnotation(2.);

  vtkNew<vtkColorSeries> palettes;
#if 0
  vtkIdType numSchemes = palettes->GetNumberOfColorSchemes();
  for (vtkIdType i = 0; i < numSchemes; ++ i)
  {
    palettes->SetColorScheme(i);
    vtkLogF(INFO, "%lld: %s", i, palettes->GetColorSchemeName().c_str());
  }
#endif
  palettes->SetColorSchemeByName("Brewer Qualitative Accent");
  palettes->BuildLookupTable(lut);

  std::map<double, std::string> expectedColors;
  expectedColors[0] = "0x7fc97fff";
  expectedColors[9] = "0x7fc97fff";
  expectedColors[1] = "0xfdc086ff";
  expectedColors[2] = "0x800000ff";
  expectedColors[3] = "0xffff99ff";
  expectedColors[0.5] = "0xbeaed4ff";
  expectedColors[-999] = "0x800000ff"; // NaN

  res = vtkTest("0.", lut->MapValue(0.), expectedColors[0]) && res;
  res = vtkTest("3.", lut->MapValue(3.), expectedColors[3]) && res;
  res = vtkTest("9.", lut->MapValue(9.), expectedColors[9]) && res;
  res = vtkTest("9", lut->MapValue(9), expectedColors[9]) && res;

  vtkNew<vtkDoubleArray> data;
  data->InsertNextValue(0.);
  data->InsertNextValue(9.);
  data->InsertNextValue(1.);
  data->InsertNextValue(2.);
  data->InsertNextValue(3.);
  data->InsertNextValue(.5);

  auto color = vtk::TakeSmartPointer(lut->MapScalars(data, VTK_RGBA, 0));
  for (vtkIdType i = 0; i < color->GetNumberOfTuples(); ++i)
  {
    double value = data->GetTuple1(i);
    res = vtkTest(std::to_string(value), color->GetPointer(i * 4), expectedColors[value]) && res;
  }

  res = vtkTest("NanColor", lut->GetNanColorAsUnsignedChars(), expectedColors[-999]) && res;

  {
    vtkLogScopeF(INFO, "With Indexed Lookup");
    res = DoPerformanceTest(lut, 256, 100000000) && res;
  }

  {
    vtkLogScopeF(INFO, "Without Indexed Lookup");
    lut->IndexedLookupOff();
    res = DoPerformanceTest(lut, 256, 100000000) && res;
  }
  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}
