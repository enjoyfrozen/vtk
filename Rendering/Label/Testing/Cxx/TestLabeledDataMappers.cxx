/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestLabeledDataMappers.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// this test verifies vtkLabelDataMapper and vtkFastLabelDataMapper

#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

#include "vtkActor.h"
#include "vtkActor2D.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkCellCenters.h"
#include "vtkFastLabeledDataMapper.h"
#include "vtkIdFilter.h"
#include "vtkLabeledDataMapper.h"
#include "vtkNew.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyDataMapper2D.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSelectVisiblePoints.h"
#include "vtkSphereSource.h"
#include "vtkTestingInteractor.h"
#include "vtkTextProperty.h"

#include "vtkXMLPolyDataWriter.h"
int TestLabeledDataMappers(int argc, char* argv[])
{
  // Create sphere
  vtkNew<vtkSphereSource> sphere;
  // sphere->SetRadius(3000);
  sphere->SetCenter(10.0, 0.0, 10.0);

  // Generate ids for labeling
  vtkNew<vtkIdFilter> ids;
  ids->SetInputConnection(sphere->GetOutputPort());
  ids->PointIdsOn();
  ids->CellIdsOn();
  ids->FieldDataOn();
  ids->Update();
  vtkPolyData* pd = (vtkPolyData*)ids->GetOutput();

  vtkNew<vtkFastLabeledDataMapper> ldm;
  ldm->SetInputConnection(ids->GetOutputPort());
  ldm->SetLabelModeToLabelFieldData();

  vtkNew<vtkPolyDataMapper> pdm;
  pdm->SetInputConnection(ids->GetOutputPort());

  vtkNew<vtkActor> pointLabels;
  pointLabels->SetMapper(ldm);
  // pointLabels->SetMapper(pdm);

  // Rendering setup
  vtkNew<vtkRenderer> ren;
  ren->AddActor(pointLabels);
  ren->SetBackground(.5, .5, 6.);
  ren->GetActiveCamera()->Zoom(.55);

  vtkNew<vtkRenderWindow> renWin;
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);
  renWin->AddRenderer(ren);
  renWin->SetMultiSamples(0);
  renWin->SetSize(500, 500);
  renWin->Render();

  int retVal = 0; // vtkRegressionTestImage( renWin );
  // if ( retVal == vtkRegressionTester::DO_INTERACTOR)
  //{
  iren->Start();
  //}
  return !retVal;
}
