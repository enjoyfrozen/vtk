// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkCamera.h"
#include "vtkColorTransferFunction.h"
#include "vtkDoubleArray.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkImageData.h"
#include "vtkImageReader2.h"
#include "vtkNew.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPlane.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkQuaternion.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"

//----------------------------------------------------------------------------
bool TestCPR(vtkTesting* testing, vtkImageData* image, vtkPolyData* orientedPolyLine,
  bool stretchedMode, bool sliceRendering)
{
  // Standard rendering classes
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetMultiSamples(0);
  renWin->SetAlphaBitPlanes(1);
  renWin->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);
  renderer->SetBackground(0.2, 0.2, 0.2);

  // Create mapper
  vtkNew<vtkGPUVolumeRayCastMapper> mapper;
  mapper->SetInputData(image);
  mapper->SetRenderCurvedPlanarReformation(true);
  mapper->SetCprOrientedPolyLine(orientedPolyLine);
  mapper->AutoAdjustSampleDistancesOff();
  auto length = image->GetLength();
  mapper->SetCprVolumeXYDimensions(length, length);
  if (stretchedMode)
  {
    mapper->SetCprMode(vtkGPUVolumeRayCastMapper::CPRModeType::STRETCHED);
    mapper->SetCprCenterPoint(orientedPolyLine->GetPoint(0)); // The first point of the polyline
  }

  vtkNew<vtkVolume> volume;
  volume->SetMapper(mapper);
  renderer->AddVolume(volume);

  // Actor settings
  vtkNew<vtkPiecewiseFunction> ofun;
  double blackPoint = 700.0;
  double whitePoint = 2000.0;
  if (sliceRendering)
  {
    ofun->AddPoint(blackPoint, 1.0);
    ofun->AddPoint(whitePoint, 1.0);
  }
  else
  {
    ofun->AddPoint(blackPoint, 0.0);
    ofun->AddPoint(whitePoint, 0.5);
  }
  vtkNew<vtkColorTransferFunction> cfun;
  cfun->AddRGBPoint(blackPoint, 0.0, 0.0, 0.0);
  cfun->AddRGBPoint(whitePoint, 1.0, 1.0, 1.0);
  volume->GetProperty()->SetScalarOpacity(ofun);
  volume->GetProperty()->SetColor(cfun);
  volume->GetProperty()->ShadeOff();

  if (sliceRendering)
  {
    // Slice the volume with a plane
    mapper->SetBlendModeToSlice();
    vtkNew<vtkPlane> slicePlane;
    slicePlane->SetOrigin(0, 0, 0);
    slicePlane->SetNormal(0, 1, 0);
    volume->GetProperty()->SetSliceFunction(slicePlane);

    renderer->GetActiveCamera()->SetPosition(0, 1, 0);
    renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
  }
  else
  {
    renderer->GetActiveCamera()->SetPosition(-1, 1, 1);
  }
  renderer->ResetCamera();

  if (!mapper->IsRenderSupported(renWin, volume->GetProperty()))
  {
    std::cout << "Required extensions not supported." << std::endl;
    return true;
  }
  if (testing->IsInteractiveModeSpecified())
  {
    iren->Start();
    return true;
  }
  if (!testing->IsValidImageSpecified())
  {
    return true;
  }
  testing->SetRenderWindow(iren->GetRenderWindow());
  return testing->RegressionTest(0.15) == vtkTesting::PASSED;
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkImageData> GetCprImageData(std::string dataRoot)
{
  std::string headFileName = dataRoot + "/Data/headsq/quarter";
  vtkNew<vtkImageReader2> reader;
  reader->SetDataByteOrderToLittleEndian();
  reader->SetDataExtent(0, 63, 0, 63, 1, 93);
  reader->SetDataSpacing(3.2, 3.2, 1.5);
  reader->SetFilePrefix(headFileName.c_str());
  reader->Update();
  return reader->GetOutput();
}

//----------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> GetCprPolyLine()
{
  vtkNew<vtkPolyData> orientedPolyLine;

  // Create points array
  vtkNew<vtkDoubleArray> pointsData;
  pointsData->SetNumberOfComponents(3);
  pointsData->InsertNextTuple3(0.4, 0.4, 0.0);
  pointsData->InsertNextTuple3(0.4, 0.4, 0.3);
  pointsData->InsertNextTuple3(0.4, 0.4, 0.4);
  pointsData->InsertNextTuple3(0.6, 0.6, 0.6);
  pointsData->InsertNextTuple3(0.6, 0.8, 0.7);
  pointsData->InsertNextTuple3(0.6, 0.8, 1.0);
  pointsData->InsertNextTuple3(0.5, 0.5, 1.0);
  pointsData->InsertNextTuple3(0.5, 0.5, 0.0);
  pointsData->InsertNextTuple3(0.5, 0.5, 1.0);
  pointsData->InsertNextTuple3(0.5, 0.5, 0.0);

  vtkNew<vtkPoints> points;
  points->SetData(pointsData);
  orientedPolyLine->SetPoints(points);

  // Create orientation array
  auto numberOfPoints = pointsData->GetNumberOfTuples();
  auto lastPointIdx = numberOfPoints - 1;
  double lastNormal[3] = { 0, 0, 1 };
  vtkQuaternion<double> lastQuat{ 1, 0, 0, 0 }; // Order: w, x, y, z
  vtkNew<vtkDoubleArray> orientationArray;      // Order: x, y, z, w
  orientationArray->SetNumberOfComponents(4);
  for (vtkIdType i = 0; i < numberOfPoints; ++i)
  {
    // Estimate normal at point i, this is a very rough estimate
    size_t aPointIdx = i > 0 ? i - 1 : 0;
    double* aPoint = points->GetPoint(aPointIdx);
    size_t bPointIdx = i < lastPointIdx ? i + 1 : lastPointIdx;
    double* bPoint = points->GetPoint(bPointIdx);
    double currentNormal[3];
    vtkMath::Subtract(bPoint, aPoint, currentNormal);
    vtkMath::Normalize(currentNormal);
    // Rotate lastQuat using the smallest rotation angle from lastNormal to currentNormal
    // If lastNormal and currentNormal are opposite, keep lastNormal as it is
    // https://stackoverflow.com/a/11741520
    double quaternionW = vtkMath::Dot(lastNormal, currentNormal) + 1;
    if (quaternionW > 0.0001)
    {
      double quaternionXYZ[3];
      vtkMath::Cross(lastNormal, currentNormal, quaternionXYZ);
      vtkQuaternion<double> tempQuat{ quaternionW, quaternionXYZ[0], quaternionXYZ[1],
        quaternionXYZ[2] };
      tempQuat.Normalize();
      lastQuat = tempQuat * lastQuat;
    }
    // Order in the data array: x, y, z, w
    orientationArray->InsertNextTuple4(
      lastQuat.GetX(), lastQuat.GetY(), lastQuat.GetZ(), lastQuat.GetW());
    vtkMath::Assign(currentNormal, lastNormal);
  }
  auto pointData = orientedPolyLine->GetPointData();
  pointData->AddArray(orientationArray);

  // Create id list and use it as a line cell
  vtkNew<vtkIdList> lineCell;
  for (int i = 0; i < numberOfPoints; ++i)
  {
    lineCell->InsertNextId(i);
  }
  vtkNew<vtkCellArray> lines;
  lines->InsertNextCell(lineCell);
  orientedPolyLine->SetLines(lines);
  return orientedPolyLine;
}

//----------------------------------------------------------------------------
int TestGPURayCastCurvedPlanarReformation(int argc, char* argv[])
{
  if (argc < 3)
  {
    return EXIT_SUCCESS;
  }

  std::string cprMode{ argv[1] };
  bool isStretchedMode = cprMode == "stretched";

  std::string volumeRendering{ argv[2] };
  bool isSliceRendering = volumeRendering == "slice";

  // Get imagedata and polyline
  const char* dataRoot = vtkTestUtilities::GetDataRoot(argc, argv);
  auto imageData = GetCprImageData(dataRoot);
  auto polyLine = GetCprPolyLine();

  // Run tests
  vtkNew<vtkTesting> testing;
  testing->AddArguments(argc, argv);
  return TestCPR(testing, imageData, polyLine, isStretchedMode, isSliceRendering) ? EXIT_SUCCESS
                                                                                  : EXIT_FAILURE;
}
