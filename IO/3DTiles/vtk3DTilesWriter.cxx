/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtk3DTilesWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtk3DTilesWriter.h"

#include "vtkCellArray.h"
#include "vtkImageReader2.h"
#include "vtkInformation.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "vtkTransformFilter.h"
#include "vtkXMLPolyDataWriter.h"
#include "vtksys/FStream.hxx"
#include <vtkDataObjectTreeIterator.h>
#include <vtkIncrementalOctreeNode.h>
#include <vtkIncrementalOctreePointLocator.h>
#include <vtkJPEGReader.h>
#include <vtkLogger.h>
#include <vtkPNGReader.h>
#include <vtkPolyDataMapper.h>
#include <vtkStringArray.h>
#include <vtkTexture.h>
#include <vtksys/SystemTools.hxx>

#include "TreeInformation.h"

#include <sstream>

using namespace vtksys;

vtkStandardNewMacro(vtk3DTilesWriter);

namespace
{
//------------------------------------------------------------------------------
/**
 * Add building centers to the octree
 * and create actors to add to the renderer.
 */
vtkSmartPointer<vtkIncrementalOctreePointLocator> BuildOctree(
  std::vector<vtkSmartPointer<vtkCompositeDataSet>>& buildings,
  const std::array<double, 6>& wholeBB, int buildingsPerTile)
{
  vtkNew<vtkPoints> points;
  points->SetDataTypeToDouble();
  vtkNew<vtkIncrementalOctreePointLocator> octree;
  octree->SetMaxPointsPerLeaf(buildingsPerTile);
  octree->InitPointInsertion(points, &wholeBB[0]);

  // TreeInformation::PrintBounds("octreeBB", &wholeBB[0]);
  for (int i = 0; i < buildings.size(); ++i)
  {
    double bb[6];
    buildings[i]->GetBounds(bb);
    double center[3] = { (bb[0] + bb[1]) / 2.0, (bb[2] + bb[3]) / 2, (bb[4] + bb[5]) / 2 };
    octree->InsertNextPoint(center);
    // std::cout << "insert: " << center[0] << ", " << center[1] << ", " << center[2]
    //           << " number of nodes: " << octree->GetNumberOfNodes() << std::endl;
  }
  return octree;
}

//------------------------------------------------------------------------------
vtkSmartPointer<vtkImageReader2> SetupTextureReader(const std::string& texturePath)
{
  std::string ext = SystemTools::GetFilenameLastExtension(texturePath);
  if (ext == ".png")
  {
    return vtkSmartPointer<vtkPNGReader>::New();
  }
  else if (ext == ".jpg")
  {
    return vtkSmartPointer<vtkJPEGReader>::New();
  }
  else
  {
    vtkLog(ERROR, "Invalid type for texture file: " << texturePath);
    return nullptr;
  }
}

//------------------------------------------------------------------------------
std::string GetFieldAsString(vtkDataObject* obj, const char* name)
{
  vtkFieldData* fd = obj->GetFieldData();
  if (!fd)
  {
    return std::string();
  }
  vtkStringArray* sa = vtkStringArray::SafeDownCast(fd->GetAbstractArray(name));
  if (!sa)
  {
    return std::string();
  }
  return sa->GetValue(0);
}

//------------------------------------------------------------------------------
void SaveLevel(const std::string& output, int level, vtkPolyData* poly)
{
  std::ostringstream ostr;
  ostr << output << "/level_" << level << ".vtp";
  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetInputDataObject(poly);
  writer->SetFileName(ostr.str().c_str());
  writer->Write();
}

//------------------------------------------------------------------------------
void AddTextures(const std::string& path,
  std::vector<vtkSmartPointer<vtkCompositeDataSet>>& buildings,
  std::vector<size_t>& buildingActorStart, std::vector<vtkSmartPointer<vtkActor>>& actors,
  vtkRenderer* renderer, bool saveTextures)
{
  for (int i = 0; i < buildings.size(); ++i)
  {
    auto it = vtk::TakeSmartPointer(buildings[i]->NewIterator());
    buildingActorStart.push_back(actors.size());
    for (it->InitTraversal(); !it->IsDoneWithTraversal(); it->GoToNextItem())
    {
      auto surface = it->GetCurrentDataObject();
      if (!vtkPolyData::SafeDownCast(surface))
      {
        vtkLog(WARNING, "Expecting vtkPolyData but got: " << surface->GetClassName());
      }
      vtkNew<vtkPolyDataMapper> mapper;
      mapper->SetInputDataObject(surface);

      auto actor = vtkSmartPointer<vtkActor>::New();
      actor->SetMapper(mapper);
      actors.push_back(actor);
      std::string textureFileName = GetFieldAsString(surface, "texture_uri");
      if (saveTextures && !textureFileName.empty())
      {
        std::string texturePath = path + "/" + textureFileName;
        auto textureReader = SetupTextureReader(texturePath);
        textureReader->SetFileName(texturePath.c_str());
        vtkNew<vtkTexture> texture;
        texture->SetInputConnection(textureReader->GetOutputPort());
        actor->SetTexture(texture);
      }
      renderer->AddActor(actor);
    }
  }
  buildingActorStart.push_back(actors.size());
}

//------------------------------------------------------------------------------
std::array<double, 6> AddBuildingsWithTexture(vtkMultiBlockDataSet* root,
  const std::string& texturePath, const double* fileOffset, vtkRenderer* renderer,
  bool saveTextures,

  std::vector<vtkSmartPointer<vtkCompositeDataSet>>& buildings,
  std::vector<size_t>& buildingActorStart, std::vector<vtkSmartPointer<vtkActor>>& actors,
  std::array<double, 3>& offset)
{
  std::array<double, 6> wholeBB;
  root->GetBounds(&wholeBB[0]);

  // translate the buildings so that the minimum wholeBB is at 0,0,0
  offset = { { wholeBB[0], wholeBB[2], wholeBB[4] } };

  std::cout << "offset: " << offset[0] << ", " << offset[1] << ", " << offset[2] << std::endl;
  vtkNew<vtkTransformFilter> f;
  vtkNew<vtkTransform> t;
  t->Identity();
  t->Translate(-offset[0], -offset[1], -offset[2]);
  f->SetTransform(t);
  f->SetInputData(root);
  f->Update();
  vtkMultiBlockDataSet* tr = vtkMultiBlockDataSet::SafeDownCast(f->GetOutputDataObject(0));
  tr->GetBounds(&wholeBB[0]);

  auto buildingIt = vtk::TakeSmartPointer(tr->NewTreeIterator());
  buildingIt->VisitOnlyLeavesOff();
  buildingIt->TraverseSubTreeOff();
  for (buildingIt->InitTraversal(); !buildingIt->IsDoneWithTraversal(); buildingIt->GoToNextItem())
  {
    auto building = vtkMultiBlockDataSet::SafeDownCast(buildingIt->GetCurrentDataObject());
    if (!building)
    {
      buildings.clear();
      return wholeBB;
    }
    buildings.push_back(building);
  }
  AddTextures(texturePath, buildings, buildingActorStart, actors, renderer, saveTextures);

  std::transform(offset.begin(), offset.end(), fileOffset, offset.begin(), std::plus<double>());

  return wholeBB;
}
};

//------------------------------------------------------------------------------
vtk3DTilesWriter::vtk3DTilesWriter()
{
  this->DirectoryName = nullptr;
  this->TexturePath = nullptr;
  std::fill(this->Origin, this->Origin + 3, 0);
  this->SaveTextures = true;
  this->SaveGLTF = true;
  this->NumberOfBuildingsPerTile = 100;
  this->UTMZone = 0;
  this->UTMHemisphere = 'N';
  this->SrsName = nullptr;
}

//------------------------------------------------------------------------------
vtk3DTilesWriter::~vtk3DTilesWriter()
{
  this->SetDirectoryName(nullptr);
  this->SetTexturePath(nullptr);
}

//------------------------------------------------------------------------------
void vtk3DTilesWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "DirectoryName: " << (this->DirectoryName ? this->DirectoryName : "NONE")
     << indent << "TexturePath: " << (this->TexturePath ? this->TexturePath : "NONE") << endl;
}

//------------------------------------------------------------------------------
int vtk3DTilesWriter::FillInputPortInformation(int port, vtkInformation* info)
{
  if (port == 0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkMultiBlockDataSet");
  }
  return 1;
}

//------------------------------------------------------------------------------
void vtk3DTilesWriter::WriteData()
{
  auto root = vtkMultiBlockDataSet::SafeDownCast(this->GetInput());
  std::vector<vtkSmartPointer<vtkCompositeDataSet>> buildings;
  std::vector<vtkSmartPointer<vtkActor>> actors;
  std::vector<size_t> buildingActorStart;
  std::array<double, 3> offset = { 0, 0, 0 };

  vtkNew<vtkRenderer> renderer;
  auto renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
  renderWindow->AddRenderer(renderer);
  renderWindow->SetSize(640, 480);

  auto wholeBB = AddBuildingsWithTexture(root, this->TexturePath, this->Origin, renderer,
    this->SaveTextures, buildings, buildingActorStart, actors, offset);
  if (buildings.empty())
  {
    vtkLog(ERROR,
      "No buildings read from the input file. "
      "Maybe buildings are on a different LOD. Try changing --lod parameter.");
    return;
  }
  vtkLog(
    INFO, "Processing " << buildings.size() << " buildings and " << actors.size() << " actors...");

  vtkSmartPointer<vtkIncrementalOctreePointLocator> octree =
    BuildOctree(buildings, wholeBB, this->NumberOfBuildingsPerTile);
  TreeInformation treeInformation(octree->GetRoot(), octree->GetNumberOfNodes(), buildings,
    buildingActorStart, offset, actors, renderWindow, this->DirectoryName, this->SrsName,
    this->UTMZone, this->UTMHemisphere);
  treeInformation.Compute();
  vtkLog(INFO, "Generating tileset.json for " << octree->GetNumberOfNodes() << " nodes...");
  treeInformation.Generate3DTiles(std::string(this->DirectoryName) + "/tileset.json");
  // debug - save poly data for each level of the tree.
  int numberOfLevels = octree->GetNumberOfLevels();
  for (int level = 0; level < numberOfLevels; ++level)
  {
    vtkNew<vtkPolyData> octreePoly;
    octree->GenerateRepresentation(
      level, octreePoly, &TreeInformation::GetNodeBounds, &treeInformation);
    treeInformation.AddGeometricError(octreePoly);
    ::SaveLevel(this->DirectoryName, level, octreePoly);
  }
  renderWindow->Render();
  if (this->SaveGLTF)
  {
    treeInformation.SaveGLTF();
  }
}
