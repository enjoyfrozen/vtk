#include "RadiossAnimDataModel.h"

#include "RadiossAnimFile.h"

#include <iostream>
#include <sstream>

// The fileFormat identifier "FASTMAGI10".
const int supportedFileFormat = 0x542c;

RadiossAnimDataModel::RadiossAnimDataModel(const std::string& animFilePath)
{
  ReadFile(animFilePath);
}

RadiossAnimDataModel::~RadiossAnimDataModel() {}

float RadiossAnimDataModel::GetTime() const
{
  return this->Time;
}

const RadiossAnimDataModel::Nodes& RadiossAnimDataModel::GetNodes() const
{
  return this->TheNodes;
}

const RadiossAnimDataModel::Quads& RadiossAnimDataModel::GetQuads() const
{
  return this->TheQuads;
}

void RadiossAnimDataModel::ReadFile(const std::string& animFilePath)
{
  RadiossAnimFile file(animFilePath);

  this->ReadAndCheckFileFormat(file);

  // Run header.
  this->Time = file.ReadOne<float>();
  auto timeDescription = file.ReadString(81);
  auto animationDescription = file.ReadString(81);
  auto runDescription = file.ReadString(81);

  // Flags.
  auto isMassSaved = file.ReadIntAsBool();
  auto isNodeNumberingElementSaved = file.ReadIntAsBool();
  auto is3DGeometrySaved = file.ReadIntAsBool();
  auto is1DGeometrySaved = file.ReadIntAsBool();
  auto isHierarchySaved = file.ReadIntAsBool();
  auto isNodeElementListForTimeHistory = file.ReadIntAsBool();
  auto isNewSkewForTensor2DSaved = file.ReadIntAsBool();
  auto isSPHSaved = file.ReadIntAsBool();
  auto unused1 = file.ReadIntAsBool();
  auto unused2 = file.ReadIntAsBool();

  ReadNodesAndQuads(file, isMassSaved, isNodeNumberingElementSaved, isHierarchySaved);

  if (is3DGeometrySaved)
  {
    ReadHexahedra(file, isMassSaved, isNodeNumberingElementSaved, isHierarchySaved);
  }

  if (is1DGeometrySaved)
  {
    ReadLines(file, isMassSaved, isNodeNumberingElementSaved, isHierarchySaved);
  }

  // TODO: Read hierarchy
  // TODO: Read Node/Element for Time History
  // TODO: Read SPH
}

void RadiossAnimDataModel::ReadAndCheckFileFormat(RadiossAnimFile& file)
{
  auto fileFormat = file.ReadOne<int>();
  if (fileFormat != supportedFileFormat)
  {
    std::stringstream message;
    message << "Unsupported file format: " << fileFormat << std::endl;
    throw std::runtime_error(message.str());
  }
}

void RadiossAnimDataModel::ReadNodesAndQuads(
  RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved, bool isHierarchySaved)
{
  auto numberOfNodes = file.ReadOne<int>();
  auto numberOfQuads = file.ReadOne<int>();
  auto numberOfQuadParts = file.ReadOne<int>();
  auto numberOfNodalScalarArrays = file.ReadOne<int>();
  auto numberOfQuadScalarArrays = file.ReadOne<int>();
  auto numberOfNodalVectorArrays = file.ReadOne<int>();
  auto numberOfQuadTensorArrays = file.ReadOne<int>();
  auto numberOfSkews = file.ReadOne<int>();

  auto skews = file.ReadFloatVectorFromShorts(numberOfSkews * 6);
  auto nodeCoordinates = file.ReadVector<float>(numberOfNodes * 3);
  auto quadConnectivity = file.ReadVector<int>(numberOfQuads * 4);
  auto quadErosionArray = file.ReadVector<char>(numberOfQuads);
  auto quadPartLastIndices = file.ReadVector<int>(numberOfQuadParts);
  auto quadPartNames = file.ReadStringVector(numberOfQuadParts, 50);
  auto nodeNorms = file.ReadFloatVectorFromShorts(numberOfNodes * 3);

  // Arrays
  auto nodeScalarArrayNames = file.ReadStringVector(numberOfNodalScalarArrays, 81);
  auto quadScalarArrayNames = file.ReadStringVector(numberOfQuadScalarArrays, 81);
  std::vector<std::vector<float>> nodeScalarArrays;
  for (size_t arrayIndex = 0; arrayIndex < numberOfNodalScalarArrays; ++arrayIndex)
  {
    nodeScalarArrays.push_back(file.ReadVector<float>(numberOfNodes));
  }
  std::vector<std::vector<float>> quadScalarArrays;
  for (size_t arrayIndex = 0; arrayIndex < numberOfQuadScalarArrays; ++arrayIndex)
  {
    quadScalarArrays.push_back(file.ReadVector<float>(numberOfQuads));
  }
  auto nodeVectorArrayNames = file.ReadStringVector(numberOfNodalVectorArrays, 81);
  std::vector<std::vector<float>> nodeVectorArrays;
  for (size_t arrayIndex = 0; arrayIndex < numberOfNodalVectorArrays; ++arrayIndex)
  {
    nodeVectorArrays.push_back(file.ReadVector<float>(3 * numberOfNodes));
  }
  auto quadTensorArrayNames = file.ReadStringVector(numberOfQuadTensorArrays, 81);
  std::vector<std::vector<float>> quadTensorArrays;
  for (size_t arrayIndex = 0; arrayIndex < numberOfQuadTensorArrays; ++arrayIndex)
  {
    quadTensorArrays.push_back(file.ReadVector<float>(numberOfQuads * 3));
  }

  // Mass
  std::vector<float> quadMassArray;
  std::vector<float> nodeMassArray;
  if (isMassSaved)
  {
    quadMassArray = file.ReadVector<float>(numberOfQuads);
    nodeMassArray = file.ReadVector<float>(numberOfNodes);
  }

  // Internal element & node numbering
  std::vector<int> nodeRadiossIDs;
  std::vector<int> quadRadiossIDs;
  if (isNodeNumberingElementSaved)
  {
    nodeRadiossIDs = file.ReadVector<int>(numberOfNodes);
    quadRadiossIDs = file.ReadVector<int>(numberOfQuads);
  }

  // Hierarchy (unused)
  if (isHierarchySaved)
  {
    auto partSubsets = file.ReadVector<int>(numberOfQuadParts);
    auto partMaterials = file.ReadVector<int>(numberOfQuadParts);
    auto partProperties = file.ReadVector<int>(numberOfQuadParts);
  }

  // -------------------
  // Move the data to the Nodes struct.
  this->TheNodes.Count = numberOfNodes;
  this->TheNodes.Coordinates = std::move(nodeCoordinates);
  // Norm
  this->TheNodes.VectorArrays.push_back({ "Norm", std::move(nodeNorms) });
  // Scalar arrays
  for (size_t arrayIndex = 0; arrayIndex < numberOfNodalScalarArrays; ++arrayIndex)
  {
    this->TheNodes.ScalarFloatArrays.push_back(
      { std::move(nodeScalarArrayNames[arrayIndex]), std::move(nodeScalarArrays[arrayIndex]) });
  }
  // Vector arrays
  for (size_t arrayIndex = 0; arrayIndex < numberOfNodalVectorArrays; ++arrayIndex)
  {
    this->TheNodes.VectorArrays.push_back(
      { std::move(nodeVectorArrayNames[arrayIndex]), std::move(nodeVectorArrays[arrayIndex]) });
  }
  // Mass
  this->TheNodes.ScalarFloatArrays.push_back({ "Mass", std::move(nodeMassArray) });
  // Node numbering
  this->TheNodes.ScalarIntArrays.push_back({ "NODE_ID", std::move(nodeRadiossIDs) });

  // ---------------------
  // Move the data to the Quad struct.
  this->TheQuads.Count = numberOfQuads;
  this->TheQuads.Connectivity = std::move(quadConnectivity);
  for (size_t partIndex = 0; partIndex < numberOfQuadParts; ++partIndex)
  {
    int firstCellIndex = 0;
    if (partIndex > 0)
    {
      firstCellIndex = quadPartLastIndices[partIndex - 1];
    }
    this->TheQuads.Parts.push_back(
      { std::move(quadPartNames[partIndex]), firstCellIndex, quadPartLastIndices[partIndex] - 1 });
  }
  // Erosion
  this->TheQuads.ScalarCharArrays.push_back({ "Erosion", std::move(quadErosionArray) });
  // Scalar arrays
  for (size_t arrayIndex = 0; arrayIndex < numberOfQuadScalarArrays; ++arrayIndex)
  {
    this->TheQuads.ScalarFloatArrays.push_back(
      { std::move(quadScalarArrayNames[arrayIndex]), std::move(quadScalarArrays[arrayIndex]) });
  }
  // Vector arrays
  for (size_t arrayIndex = 0; arrayIndex < numberOfQuadTensorArrays; ++arrayIndex)
  {
    this->TheQuads.VectorArrays.push_back(
      { std::move(quadTensorArrayNames[arrayIndex]), std::move(quadTensorArrays[arrayIndex]) });
  }
  // Mass
  this->TheQuads.ScalarFloatArrays.push_back({ "Mass", std::move(quadMassArray) });
  // Node numbering
  this->TheQuads.ScalarIntArrays.push_back({ "ELEMENT_ID", std::move(quadRadiossIDs) });
}

void RadiossAnimDataModel::ReadHexahedra(
  RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved, bool isHierarchySaved)
{
  auto numberOfHexahedra = file.ReadOne<int>();
  auto numberOfHexahedronParts = file.ReadOne<int>();
  auto numberOfHexahedronScalarArrays = file.ReadOne<int>();
  auto numberOfHexahedronTensorArrays = file.ReadOne<int>();

  auto hexahedronConnectivity = file.ReadVector<int>(numberOfHexahedra * 8);
  auto hexahedronErosionArray = file.ReadVector<unsigned char>(numberOfHexahedra);
  auto hexahedronPartLastIndices = file.ReadVector<int>(numberOfHexahedronParts);
  auto hexahedronPartNames = file.ReadStringVector(numberOfHexahedronParts, 50);
  auto hexahedronScalarArrayNames = file.ReadStringVector(numberOfHexahedronScalarArrays, 81);
  auto hexahedronScalarArrays =
    file.ReadVector<float>(numberOfHexahedronScalarArrays * numberOfHexahedra);
  auto hexahedronTensorArrayNames = file.ReadStringVector(numberOfHexahedronTensorArrays, 81);
  auto hexahedronTensorArrays =
    file.ReadVector<float>(numberOfHexahedra * 6 * numberOfHexahedronTensorArrays);

  // Mass
  std::vector<float> hexahedronMassArray;
  if (isMassSaved)
  {
    hexahedronMassArray = file.ReadVector<float>(numberOfHexahedra);
  }

  // Internal element numbering
  std::vector<int> hexahedronRadiossIDs;
  if (isNodeNumberingElementSaved)
  {
    hexahedronRadiossIDs = file.ReadVector<int>(numberOfHexahedra);
  }

  // Hierarchy (unused)
  if (isHierarchySaved)
  {
    auto partSubsets = file.ReadVector<int>(numberOfHexahedronParts);
    auto partMaterials = file.ReadVector<int>(numberOfHexahedronParts);
    auto partProperties = file.ReadVector<int>(numberOfHexahedronParts);
  }
}

void RadiossAnimDataModel::ReadLines(
  RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved, bool isHierarchySaved)
{
  auto numberOfLines = file.ReadOne<int>();
  auto numberOfLineParts = file.ReadOne<int>();
  auto numberOfLineScalarArrays = file.ReadOne<int>();
  auto numberOfLineTensorArrays = file.ReadOne<int>();
  auto isLineSkewSaved = file.ReadIntAsBool();

  auto lineConnectivity = file.ReadVector<int>(numberOfLines * 2);
  auto lineErosionArray = file.ReadVector<unsigned char>(numberOfLines);
  auto linePartLastIndices = file.ReadVector<int>(numberOfLineParts);
  auto linePartNames = file.ReadStringVector(numberOfLineParts, 50);
  auto lineScalarArrayNames = file.ReadStringVector(numberOfLineScalarArrays, 81);
  auto lineScalarArrays = file.ReadVector<float>(numberOfLineScalarArrays * numberOfLines);
  auto lineTensorArrayNames = file.ReadStringVector(numberOfLineTensorArrays, 81);
  auto lineTensorArrays = file.ReadVector<float>(numberOfLines * 9 * numberOfLineTensorArrays);

  // Skew
  std::vector<float> lineSkewArray;
  if (isLineSkewSaved)
  {
    lineSkewArray = file.ReadVector<float>(numberOfLines);
  }

  // Mass
  std::vector<float> lineMassArray;
  if (isMassSaved)
  {
    lineMassArray = file.ReadVector<float>(numberOfLines);
  }

  // Internal element numbering
  std::vector<int> lineRadiossIDs;
  if (isNodeNumberingElementSaved)
  {
    lineRadiossIDs = file.ReadVector<int>(numberOfLines);
  }

  // Hierarchy (unused)
  if (isHierarchySaved)
  {
    auto partSubsets = file.ReadVector<int>(numberOfLineParts);
    auto partMaterials = file.ReadVector<int>(numberOfLineParts);
    auto partProperties = file.ReadVector<int>(numberOfLineParts);
  }
}
