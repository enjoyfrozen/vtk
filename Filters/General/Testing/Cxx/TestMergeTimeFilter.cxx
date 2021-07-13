/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestMergeTimeFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMergeTimeFilter.h"

#include "vtkExodusIIReader.h"
#include "vtkInformation.h"
#include "vtkNew.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkTemporalShiftScale.h"
#include "vtkTestUtilities.h"

int TestMergeTimeFilter(int argc, char* argv[])
{
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/can.ex2");
  vtkNew<vtkExodusIIReader> reader;
  reader->SetFileName(fname);
  delete[] fname;

  vtkNew<vtkTemporalShiftScale> shifter;
  shifter->SetInputConnection(reader->GetOutputPort(0));
  vtkNew<vtkMergeTimeFilter> merger;
  merger->SetInputConnection(reader->GetOutputPort(0));
  merger->AddInputConnection(shifter->GetOutputPort(0));

  shifter->SetPreShift(0.002);
  merger->SetTolerance(0.00004);
  merger->Update();
  vtkInformation* info = merger->GetOutputInformation(0);
  int nbOfTimesteps = info->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  auto values = info->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  if (info->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS()) != 64)
  {
    std::cerr << "Wrong number of Timesteps for union " << nbOfTimesteps << std::endl;
    for (int i = 0; i < nbOfTimesteps; i++)
      std::cerr << values[i] << std::endl;

    return EXIT_FAILURE;
  }

  merger->UseIntersectionOn();
  merger->Update();
  info = merger->GetOutputInformation(0);
  nbOfTimesteps = info->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  values = info->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  if (nbOfTimesteps != 24)
  {
    std::cerr << "Wrong number of Timesteps for intersection " << nbOfTimesteps << std::endl;
    for (int i = 0; i < nbOfTimesteps; i++)
      std::cerr << values[i] << std::endl;
    return EXIT_FAILURE;
  }

  shifter->SetPreShift(0.00);
  shifter->SetScale(2);
  merger->SetTolerance(0.001);
  merger->UseIntersectionOff();
  merger->UseRelativeToleranceOn();
  merger->Update();
  info = merger->GetOutputInformation(0);
  nbOfTimesteps = info->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  values = info->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  if (nbOfTimesteps != 66)
  {
    std::cerr << "Wrong number of Timesteps for relative " << nbOfTimesteps << std::endl;
    for (int i = 0; i < nbOfTimesteps; i++)
      std::cerr << values[i] << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
