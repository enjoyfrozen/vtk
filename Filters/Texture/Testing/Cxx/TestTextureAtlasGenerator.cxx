// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkSphereSource.h"
#include "vtkTestUtilities.h"
#include "vtkTextureAtlasGenerator.h"
#include "vtkXMLPolyDataReader.h"

#include "vtkXMLPolyDataWriter.h"

//------------------------------------------------------------------------------
int TestTextureAtlasGenerator(int argc, char* argv[])
{
  int retVal = EXIT_SUCCESS;

  vtkNew<vtkSphereSource> sphere;

  vtkNew<vtkTextureAtlasGenerator> textureAtlas;

  textureAtlas->SetInputConnection(0, sphere->GetOutputPort());
  textureAtlas->Update();

  vtkNew<vtkXMLPolyDataReader> meshReader;
  meshReader->SetFileName(
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/texture_atlas_generator_mesh.vtp"));
  meshReader->Update();

  vtkNew<vtkXMLPolyDataReader> atlasReader;
  atlasReader->SetFileName(
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/texture_atlas_generator_atlas.vtp"));
  atlasReader->Update();

  if (!vtkTestUtilities::CompareDataObjects(
        meshReader->GetOutputDataObject(0), textureAtlas->GetOutputDataObject(0)))
  {
    vtkLog(ERROR, "Output mesh is wrong.");
    retVal = EXIT_FAILURE;
  }

  if (!vtkTestUtilities::CompareDataObjects(
        atlasReader->GetOutputDataObject(0), textureAtlas->GetOutputDataObject(1)))
  {
    vtkLog(ERROR, "Output atlas is wrong.");
    retVal = EXIT_FAILURE;
  }

  return retVal;
}
