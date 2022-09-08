/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestTrackballSingularityStyle.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include <sstream>

#include <vtkDebugLeaks.h>

#include <vtkCamera.h>
#include <vtkInteractorEventRecorder.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkProperty.h>
#include <vtkRegressionTestImage.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "vtkTrackballStyleTestUtils.h"

//------------------------------------------------------------------------------
int TestTrackballSingularityStyle(int argc, char* argv[])
{
  std::ostringstream strm;
  strm << "TestTrackballSingularityStyle Setup" << endl;
  vtkNew<vtkActor> coneActorX, coneActorY, coneActorZ;
  SetupCone(coneActorX, { 1.0, 0.0, 0.0 });
  SetupCone(coneActorY, { 0.0, 1.0, 0.0 });
  SetupCone(coneActorZ, { 0.0, 0.0, 1.0 });

  strm << "TestTrackballSingularityStyle: Window" << endl;
  vtkNew<vtkRenderer> renderer;
  renderer->AddActor(coneActorX);
  renderer->AddActor(coneActorY);
  renderer->AddActor(coneActorZ);
  renderer->SetBackground(0.5, 0.5, 0.5);
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetMultiSamples(0);
  renderWindow->SetSize(500, 500);
  renderWindow->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
  renderWindowInteractor->SetRenderWindow(renderWindow);
  vtkNew<vtkInteractorStyleTrackballCamera> trackball;
  trackball->SetRotationModel(vtkTrackballRotationModel::Singularity);
  renderWindowInteractor->SetInteractorStyle(trackball);

  renderer->GetActiveCamera()->SetPosition(0.0, -20.0, 0.0);
  renderer->GetActiveCamera()->SetFocalPoint(0.0, 0.0, 0.0);
  renderer->GetActiveCamera()->SetViewUp(0.0, 0.0, 1.0);

  strm << "TestTrackballSingularityStyle: Recorder" << endl;
  vtkNew<vtkInteractorEventRecorder> recorder;
  recorder->SetInteractor(renderWindowInteractor);
  const bool isRecording = false;
  if (isRecording)
  {
    recorder->SetFileName("record.log");
    recorder->Record();
    recorder->EnabledOn();
  }
  else
  {
    recorder->ReadFromInputStringOn();
    recorder->SetInputString(TestRecordEventLog_MouseDown_PartialUpRight);

    renderWindowInteractor->Initialize();
    renderer->Render();

    recorder->Play();
    // Remove the observers so we can go interactive. Without this the "-I" testing option fails.
    recorder->Off();
  }

  strm << "TestTrackballSingularityStyle: Verification" << endl;
  int retVal = vtkRegressionTestImage(renderWindow);
  if (retVal == vtkRegressionTester::DO_INTERACTOR || isRecording)
  {
    renderWindowInteractor->Start();
    recorder->Stop();
    recorder->Off();
    retVal = vtkRegressionTester::PASSED;
  }
  return !retVal;
}
