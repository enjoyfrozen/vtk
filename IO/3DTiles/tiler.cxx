#include "tiler.h"

#include "vtk3DTilesWriter.h"
#include <vtkCellData.h>
#include <vtkCityGMLReader.h>
#include <vtkCompositeDataIterator.h>
#include <vtkDataObject.h>
#include <vtkDirectory.h>
#include <vtkDoubleArray.h>
#include <vtkIncrementalOctreeNode.h>
#include <vtkIncrementalOctreePointLocator.h>
#include <vtkLogger.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkNew.h>
#include <vtkOBJReader.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>
#include <vtkStringArray.h>
#include <vtksys/SystemTools.hxx>

#include <algorithm>
#include <set>
#include <sstream>

using namespace vtksys;

//------------------------------------------------------------------------------
void SetField(vtkDataObject* obj, const char* name, const char* value)
{
  vtkFieldData* fd = obj->GetFieldData();
  if (!fd)
  {
    vtkNew<vtkFieldData> newfd;
    obj->SetFieldData(newfd);
    fd = newfd;
  }
  vtkNew<vtkStringArray> sa;
  sa->SetNumberOfTuples(1);
  sa->SetValue(0, value);
  sa->SetName(name);
  fd->AddArray(sa);
}

//------------------------------------------------------------------------------
std::array<double, 3> ReadOBJOffset(const char* comment)
{
  std::array<double, 3> translation = { 0, 0, 0 };
  if (comment)
  {
    std::istringstream istr(comment);
    std::array<std::string, 3> axesNames = { "x", "y", "z" };
    for (int i = 0; i < 3; ++i)
    {
      std::string axis;
      std::string s;
      istr >> axis >> s >> translation[i];
      if (istr.fail())
      {
        vtkLog(WARNING, "Cannot read axis " << axesNames[i] << " from comment.");
      }
      if (axis != axesNames[i])
      {
        vtkLog(WARNING, "Invalid axis " << axesNames[i] << ": " << axis);
      }
    }
  }
  else
  {
    vtkLog(WARNING, "nullptr comment.");
  }
  return translation;
}

//------------------------------------------------------------------------------
std::string GetOBJTextureFileName(const std::string& file)
{
  std::string fileNoExt = SystemTools::GetFilenameWithoutExtension(file);
  return fileNoExt + ".png";
}

vtkSmartPointer<vtkMultiBlockDataSet> ReadOBJFiles(int numberOfBuildings, int lod,
  const std::vector<std::string>& files, std::array<double, 3>& fileOffset)
{
  auto root = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  for (int i = 0; i < files.size() && i < numberOfBuildings; ++i)
  {
    vtkNew<vtkOBJReader> reader;
    reader->SetFileName(files[i].c_str());
    reader->Update();
    if (i == 0)
    {
      fileOffset = ReadOBJOffset(reader->GetComment());
    }
    auto polyData = reader->GetOutput();
    std::string textureFileName = GetOBJTextureFileName(files[i]);
    SetField(polyData, "texture_uri", textureFileName.c_str());
    auto building = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    building->SetBlock(0, polyData);
    root->SetBlock(root->GetNumberOfBlocks(), building);
  }
  return root;
}

vtkSmartPointer<vtkMultiBlockDataSet> ReadCityGMLFiles(int numberOfBuildings, int lod,
  const std::vector<std::string>& files, std::array<double, 3>& fileOffset)
{
  if (files.size() > 1)
  {
    vtkLog(WARNING, "Can only process one CityGML file for now.");
  }
  vtkNew<vtkCityGMLReader> reader;
  reader->SetFileName(files[0].c_str());
  reader->SetNumberOfBuildings(numberOfBuildings);
  reader->SetLOD(lod);
  reader->Update();
  vtkSmartPointer<vtkMultiBlockDataSet> root = reader->GetOutput();
  if (!root)
  {
    vtkLog(ERROR, "Expecting vtkMultiBlockDataSet");
    return nullptr;
  }
  std::fill(fileOffset.begin(), fileOffset.end(), 0);
  return root;
}

//------------------------------------------------------------------------------
using ReaderType = vtkSmartPointer<vtkMultiBlockDataSet> (*)(int numberOfBuildings, int lod,
  const std::vector<std::string>& files, std::array<double, 3>& fileOffset);
std::map<std::string, ReaderType> READER = { { ".obj", ReadOBJFiles },
  { ".gml", ReadCityGMLFiles } };

//------------------------------------------------------------------------------
bool isSupported(const char* file)
{
  std::string ext = SystemTools::GetFilenameExtension(file);
  return READER.find(ext) != READER.end();
}

//------------------------------------------------------------------------------
std::vector<std::string> getFiles(const std::vector<std::string>& input)
{
  std::vector<std::string> files;
  for (std::string name : input)
  {
    if (SystemTools::FileExists(name.c_str(), false /*isFile*/))
    {
      if (SystemTools::FileIsDirectory(name))
      {
        // add all supported files from the directory
        vtkNew<vtkDirectory> dir;
        if (!dir->Open(name.c_str()))
        {
          vtkLog(WARNING, "Cannot open directory: " << name);
        }
        for (int i = 0; i < dir->GetNumberOfFiles(); ++i)
        {
          const char* file = dir->GetFile(i);
          if (!SystemTools::FileIsDirectory(file) && isSupported(file))
          {
            files.push_back(name + "/" + file);
          }
        }
      }
      else
      {
        files.push_back(name);
      }
    }
    else
    {
      vtkLog(WARNING, "No such file or directory: " << name);
    }
  }
  return files;
}

//------------------------------------------------------------------------------
void tiler(const std::vector<std::string>& input, const std::string& output, int numberOfBuildings,
  int buildingsPerTile, int lod, const std::vector<double>& inputOffset, bool saveGLTF,
  bool saveTextures, const std::string& srsName, const int utmZone, char utmHemisphere)
{
  std::vector<std::string> files = getFiles(input);
  if (files.empty())
  {
    vtkLog(ERROR, "No valid input files");
    return;
  }
  vtkLog(INFO, "Parsing " << files.size() << " files...")

    std::array<double, 3>
      fileOffset = { { 0, 0, 0 } };
  auto root =
    READER[SystemTools::GetFilenameExtension(files[0])](numberOfBuildings, lod, files, fileOffset);
  std::transform(fileOffset.begin(), fileOffset.end(), inputOffset.begin(), fileOffset.begin(),
    std::plus<double>());

  std::string texturePath = SystemTools::GetFilenamePath(files[0]);

  vtkNew<vtk3DTilesWriter> writer;
  writer->SetInputDataObject(root);
  writer->SetDirectoryName(output.c_str());
  writer->SetTexturePath(texturePath.c_str());
  writer->SetOrigin(&fileOffset[0]);
  writer->SetSaveTextures(saveTextures);
  writer->SetSaveGLTF(saveGLTF);
  if (srsName.empty())
  {
    writer->SetUTMZone(utmZone);
    writer->SetUTMHemisphere(utmHemisphere);
  }
  else
  {
    writer->SetSrsName(srsName.c_str());
  }
  writer->Write();
}
