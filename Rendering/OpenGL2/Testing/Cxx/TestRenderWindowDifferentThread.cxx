/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Mace.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkActor.h"
#include "vtkConeSource.h"
#include "vtkGlyph3D.h"
#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"

#include <future>
#include <sstream>
#include <string>
#include <thread>

int Start(int argc, char* argv[])
{
  vtkLogger::SetThreadName("Render Thread");
  std::hash<std::thread::id> tid_hash{};
  uint32_t tid = tid_hash(std::this_thread::get_id());
  vtkLog(INFO, << "Rendering on " << tid);
  vtkNew<vtkRenderer> renderer;
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer(renderer);
  vtkNew<vtkRenderWindowInteractor> iren;
  // renWin->UseOffScreenBuffersOn();
  iren->SetRenderWindow(renWin);

  renderer->SetBackground(0.2, 0.3, 0.4);
  renWin->SetSize(300, 300);

  // interact with data
  renWin->Render();

  int retVal = vtkRegressionTestImage(renWin);

  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    iren->Start();
  }
  return !retVal;
}

int TestRenderWindowDifferentThread(int argc, char* argv[])
{
  std::launch policy = std::launch::deferred;
  for (int i = 0; i < argc; ++i)
  {
    if (std::string(argv[i]) == "-async")
    {
      policy = std::launch::async;
      break;
    }
  }
  vtkLogger::SetStderrVerbosity(vtkLogger::VERBOSITY_INFO);
  std::future<int> fut = std::async(policy, &Start, argc, argv);
  std::hash<std::thread::id> tid_hash{};
  uint32_t tid = tid_hash(std::this_thread::get_id());
  vtkLog(INFO, << "Main thread " << tid);
  int result = vtkRegressionTester::FAILED;
  result = fut.get();
  vtkLog(INFO, << "result=" << result);
  return result;
}
