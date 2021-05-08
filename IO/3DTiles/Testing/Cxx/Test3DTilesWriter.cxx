#include "vtk3DTilesWriter.h"
#include "vtkTesting.h"
#include <vtkCellData.h>
#include <vtkCityGMLReader.h>
#include <vtkCompositeDataIterator.h>
#include <vtkDataObject.h>
#include <vtkDirectory.h>
#include <vtkDoubleArray.h>
#include <vtkGLTFReader.h>
#include <vtkIncrementalOctreeNode.h>
#include <vtkIncrementalOctreePointLocator.h>
#include <vtkLogger.h>
#include <vtkMathUtilities.h>
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

vtkSmartPointer<vtkMultiBlockDataSet> ReadOBJFiles(int numberOfBuildings, int vtkNotUsed(lod),
  const std::vector<std::string>& files, std::array<double, 3>& fileOffset)
{
  auto root = vtkSmartPointer<vtkMultiBlockDataSet>::New();
  for (size_t i = 0; i < files.size() && i < static_cast<size_t>(numberOfBuildings); ++i)
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
struct Input
{
  Input()
  {
    this->Data = nullptr;
    std::fill(InputOrigin.begin(), InputOrigin.end(), 0);
    std::fill(OutputOrigin.begin(), OutputOrigin.end(), 0);
  }
  vtkSmartPointer<vtkMultiBlockDataSet> Data;
  std::array<double, 3> InputOrigin;
  std::array<double, 3> OutputOrigin;
};

Input tiler(const std::vector<std::string>& input, const std::string& output, int numberOfBuildings,
  int buildingsPerTile, int lod, const std::vector<double>& inputOffset, bool saveGLTF,
  bool saveTextures, const std::string& srsName, const int utmZone, char utmHemisphere)
{
  Input ret;
  std::vector<std::string> files = getFiles(input);
  if (files.empty())
  {
    vtkLog(ERROR, "No valid input files");
    return ret;
  }
  vtkLog(INFO, "Parsing " << files.size() << " files...")

    std::array<double, 3>
      fileOffset = { { 0, 0, 0 } };
  ret.Data =
    READER[SystemTools::GetFilenameExtension(files[0])](numberOfBuildings, lod, files, fileOffset);
  std::transform(fileOffset.begin(), fileOffset.end(), inputOffset.begin(), fileOffset.begin(),
    std::plus<double>());
  ret.InputOrigin = fileOffset;

  std::string texturePath = SystemTools::GetFilenamePath(files[0]);

  vtkNew<vtk3DTilesWriter> writer;
  writer->SetInputDataObject(ret.Data);
  writer->SetDirectoryName(output.c_str());
  writer->SetTexturePath(texturePath.c_str());
  writer->SetOrigin(&fileOffset[0]);
  writer->SetSaveTextures(saveTextures);
  writer->SetNumberOfBuildingsPerTile(buildingsPerTile);
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
  std::copy(writer->GetOrigin(), writer->GetOrigin() + 3, ret.OutputOrigin.begin());
  return ret;
}

bool TrianglesDiffer(Input& in, std::string gltfFileName)
{
  auto it = vtk::TakeSmartPointer(in.Data->NewIterator());
  vtkPolyData* input = vtkPolyData::SafeDownCast(it->GetCurrentDataObject());
  if (!input)
  {
    std::cerr << "Cannot read input data" << std::endl;
    return true;
  }
  vtkNew<vtkGLTFReader> reader;
  reader->SetFileName(gltfFileName.c_str());
  reader->Update();
  vtkMultiBlockDataSet* mbOutput = reader->GetOutput();
  it = vtk::TakeSmartPointer(mbOutput->NewIterator());
  vtkPolyData* output = vtkPolyData::SafeDownCast(it->GetCurrentDataObject());
  if (!output)
  {
    std::cerr << "Cannot read output data" << std::endl;
    return true;
  }
  vtkPoints* inputPoints = input->GetPoints();
  vtkPoints* outputPoints = output->GetPoints();
  for (int i = 0; i < 3; ++i)
  {
    std::array<double, 3> inputPoint;
    inputPoints->GetPoint(i, &inputPoint[0]);
    std::transform(in.InputOrigin.begin(), in.InputOrigin.end(), inputPoint.begin(),
      inputPoint.begin(), std::plus<double>());
    std::array<double, 3> outputPoint;
    outputPoints->GetPoint(i, &outputPoint[0]);
    std::transform(in.OutputOrigin.begin(), in.OutputOrigin.end(), outputPoint.begin(),
      outputPoint.begin(), std::plus<double>());
    for (size_t j = 0; j < inputPoint.size(); ++j)
    {
      if (!vtkMathUtilities::NearlyEqual(inputPoint[j], outputPoint[j], 0.001))
      {
        std::cerr << "input point: " << inputPoint[j]
                  << " differ than output point: " << outputPoint[j] << " at position: " << j
                  << std::endl;
        return true;
      }
    }
  }
  return false;
}

int Test3DTilesWriter(int argc, char* argv[])
{
  vtkNew<vtkTesting> testHelper;
  testHelper->AddArguments(argc, argv);
  if (!testHelper->IsFlagSpecified("-D"))
  {
    std::cerr << "Error: -D /path/to/data was not specified.";
    return EXIT_FAILURE;
  }
  if (!testHelper->IsFlagSpecified("-T"))
  {
    std::cerr << "Error: -T /path/to/temp_directory was not specified.";
    return EXIT_FAILURE;
  }

  std::string dataRoot = testHelper->GetDataRoot();
  std::string tempDirectory = testHelper->GetTempDirectory();

  Input in =
    tiler(std::vector<std::string>{ { dataRoot + "/Data/3DTiles/jacksonville-triangle.obj" } },
      tempDirectory + "/jacksonville-3dtiles", 1, 1, 2, std::vector<double>{ { 0, 0, 0 } },
      true /*saveGLTF*/, false /*saveTextures*/, "", 17, 'N');
  if (!in.Data)
  {
    return EXIT_FAILURE;
  }
  if (TrianglesDiffer(in, tempDirectory + "/jacksonville-3dtiles/0/0.gltf"))
  {
    return EXIT_FAILURE;
  }
  if (SystemTools::TextFilesDiffer(dataRoot + "/Data/3DTiles/jacksonville-tileset.json",
        tempDirectory + "/jacksonville-3dtiles/tileset.json"))
  {
    std::cerr << "Jacksonville data produced a different tileset than expected" << std::endl;
    return EXIT_FAILURE;
  }
  in = tiler(std::vector<std::string>{ { dataRoot + "/Data/3DTiles/berlin-triangle.gml" } },
    tempDirectory + "/berlin-3dtiles", 1, 1, 2, std::vector<double>{ { 0, 0, 0 } },
    true /*saveGLTF*/, false /*saveTextures*/, "", 33, 'N');
  if (!in.Data)
  {
    return EXIT_FAILURE;
  }
  if (TrianglesDiffer(in, tempDirectory + "/berlin-3dtiles/0/0.gltf"))
  {
    return EXIT_FAILURE;
  }
  if (SystemTools::TextFilesDiffer(dataRoot + "/Data/3DTiles/berlin-tileset.json",
        tempDirectory + "/berlin-3dtiles/tileset.json"))
  {
    std::cerr << "Berlin data produced a different tileset than expected" << std::endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
