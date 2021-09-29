/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestPartitionedDataSetCollectionConvertors.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkActor.h"
#include "vtkCompositePolyDataMapper2.h"
#include "vtkConvertToMultiBlockDataSet.h"
#include "vtkIOSSReader.h"
#include "vtkNew.h"
#include "vtkPlane.h"
#include "vtkPlaneCutter.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"

int TestCompositeDataSetPlaneCutter(int argc, char** argv)
{
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/can.ex2");
  vtkNew<vtkIOSSReader> inputPDCReader;
  inputPDCReader->SetFileName(fname);
  inputPDCReader->Update();
  delete[] fname;

  vtkNew<vtkConvertToMultiBlockDataSet> inputMBDConverter;
  inputMBDConverter->SetInputConnection(inputPDCReader->GetOutputPort());

  // The cut plane
  vtkNew<vtkPlane> plane;
  plane->SetOrigin(0, 0, 0);
  plane->SetNormal(1, 1, 1);

  // Accelerated cutter 0
  vtkNew<vtkPlaneCutter> cut0;
  cut0->SetInputConnection(inputMBDConverter->GetOutputPort());
  cut0->SetPlane(plane);
  cut0->ComputeNormalsOff();

  vtkNew<vtkCompositePolyDataMapper2> sCutterMapper0;
  sCutterMapper0->SetInputConnection(cut0->GetOutputPort());
  sCutterMapper0->ScalarVisibilityOff();

  vtkNew<vtkActor> sCutterActor0;
  sCutterActor0->SetMapper(sCutterMapper0);
  sCutterActor0->GetProperty()->SetColor(1, 1, 1);

  // Accelerated cutter 1
  vtkNew<vtkPlaneCutter> cut1;
  cut1->SetInputConnection(inputPDCReader->GetOutputPort());
  cut1->SetPlane(plane);
  cut1->ComputeNormalsOff();

  vtkNew<vtkCompositePolyDataMapper2> sCutterMapper1;
  sCutterMapper1->SetInputConnection(cut1->GetOutputPort());
  sCutterMapper1->ScalarVisibilityOff();

  vtkNew<vtkActor> sCutterActor1;
  sCutterActor1->SetMapper(sCutterMapper1);
  sCutterActor1->GetProperty()->SetColor(1, 1, 1);

  // Create the RenderWindow, Renderers and both Actors
  vtkNew<vtkRenderer> ren0;
  vtkNew<vtkRenderer> ren1;
  vtkNew<vtkRenderWindow> renWin;
  renWin->SetMultiSamples(0);
  renWin->AddRenderer(ren0);
  renWin->AddRenderer(ren1);
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  // Add the actors to the renderer, set the background and size
  ren0->AddActor(sCutterActor0);
  ren1->AddActor(sCutterActor1);
  ren0->SetBackground(0, 0, 0);
  ren1->SetBackground(0, 0, 0);
  ren0->SetViewport(0, 0, 0.5, 1);
  ren1->SetViewport(0.5, 0, 1, 1);
  renWin->SetSize(600, 300);
  ren0->ResetCamera();
  ren1->ResetCamera();
  iren->Initialize();
  renWin->Render();

  int retVal = vtkRegressionTestImage(renWin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
    retVal = vtkRegressionTester::PASSED;
  }
  return (retVal == vtkRegressionTester::PASSED) ? EXIT_SUCCESS : EXIT_FAILURE;
}
