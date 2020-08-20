/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestMP4Writer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "vtkImageMandelbrotSource.h"
#include "vtkImageMapToColors.h"
#include "vtkLogger.h"
#include "vtkLookupTable.h"
#include "vtkMP4Writer.h"
#include "vtksys/SystemTools.hxx"

int TestMP4Writer(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  vtkNew<vtkImageMandelbrotSource> fractal;
  fractal->SetWholeExtent(0, 247, 0, 247, 0, 0);
  fractal->SetProjectionAxes(0, 1, 2);
  fractal->SetOriginCX(-1.75, -1.25, 0, 0);
  fractal->SetSizeCX(2.5, 2.5, 2, 1.5);
  fractal->SetMaximumNumberOfIterations(100);

  vtkNew<vtkImageCast> cast;
  cast->SetInputConnection(fractal->GetOutputPort());
  cast->SetOutputScalarTypeToUnsignedChar();

  vtkNew<vtkLookupTable> table;
  table->SetTableRange(0, 100);
  table->SetNumberOfColors(100);
  table->Build();
  table->SetTableValue(99, 0, 0, 0);

  vtkNew<vtkImageMapToColors> colorize;
  colorize->SetOutputFormatToRGB();
  colorize->SetLookupTable(table);
  colorize->SetInputConnection(cast->GetOutputPort());

  // Clear out results from previous runs of this test.
  const char* fileName = "TestMP4Writer.mp4";
  vtksys::SystemTools::RemoveFile(fileName);

  vtkNew<vtkMP4Writer> w;

  w->SetInputConnection(colorize->GetOutputPort());
  w->SetFileName(fileName);
  vtkLog(INFO, "Writing file " << fileName);
  w->Start();
  for (int cc = 2; cc < 99; cc++)
  {
    fractal->SetMaximumNumberOfIterations(cc);
    table->SetTableRange(0, cc);
    table->SetNumberOfColors(cc);
    table->ForceBuild();
    table->SetTableValue(cc - 1, 0, 0, 0);
    w->Write();
  }
  w->End();
  vtkLog(INFO, "Done writing file TestMP4Writer.mp4...");

  bool exists = (int)vtksys::SystemTools::FileExists("TestMP4Writer.mp4");
  unsigned long length = vtksys::SystemTools::FileLength("TestMP4Writer.mp4");
  bool success = true;
  if (!exists)
  {
    success = false;
    vtkLog(ERROR, "Test failing because TestMP4Writer.mp4 file doesn't exist...");
  }
  if (0 == length)
  {
    success = false;
    vtkLog(ERROR, "Test failing because TestMP4Writer.mp4 file has zero length...");
  }

  return success ? 0 : -1;
}
