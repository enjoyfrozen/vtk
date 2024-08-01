#include "RadiossAnim.h"

#include "RadiossAnimFile.h"

#include <iostream>
#include <sstream>

// The fileFormat identifier "FASTMAGI10".
const int supportedFileFormat = 0x542c;

RadiossAnim::RadiossAnim(const std::string& animFilePath)
{
  ReadFile(animFilePath);
}

RadiossAnim::~RadiossAnim() {}

void RadiossAnim::ReadFile(const std::string& animFilePath)
{
  RadiossAnimFile file(animFilePath);

  this->ReadAndCheckFileFormat(file);

  // Run header.
  auto time = file.ReadOne<float>();
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

  Read2DGeometry(file, isMassSaved, isNodeNumberingElementSaved, isHierarchySaved);

  if (is3DGeometrySaved)
  {
    Read3DGeometry(file, isMassSaved, isNodeNumberingElementSaved, isHierarchySaved);
  }

  if (is1DGeometrySaved)
  {
    Read1DGeometry(file, isMassSaved, isNodeNumberingElementSaved, isHierarchySaved);
  }

  // TODO: Read hierarchy
  // TODO: Read Node/Element for Time History
  // TODO: Read SPH
}

void RadiossAnim::ReadAndCheckFileFormat(RadiossAnimFile& file)
{
  auto fileFormat = file.ReadOne<int>();
  if (fileFormat != supportedFileFormat)
  {
    std::stringstream message;
    message << "Unsupported file format: " << fileFormat << std::endl;
    throw std::runtime_error(message.str());
  }
}

void RadiossAnim::Read2DGeometry(
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
  auto scalarArrayNames =
    file.ReadStringVector(numberOfNodalScalarArrays + numberOfQuadScalarArrays, 81);
  auto nodeScalarArrays = file.ReadVector<float>(numberOfNodalScalarArrays * numberOfNodes);
  auto quadScalarArrays = file.ReadVector<float>(numberOfQuadScalarArrays * numberOfQuads);
  auto nodeVectorArrayNames = file.ReadStringVector(numberOfNodalVectorArrays, 81);
  auto nodeVectorArrays = file.ReadVector<float>(3 * numberOfNodes * numberOfNodalVectorArrays);
  auto quadTensorArrayNames = file.ReadStringVector(numberOfQuadTensorArrays, 81);
  auto quadTensorArrays = file.ReadVector<float>(numberOfQuads * 3 * numberOfQuadTensorArrays);

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
}

void RadiossAnim::Read3DGeometry(
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

  std::vector<float> hexahedronMassArray;
  if (isMassSaved)
  {
    hexahedronMassArray = file.ReadVector<float>(numberOfHexahedra);
  }

  std::vector<int> hexahedronRadiossIDs;
  if (isNodeNumberingElementSaved)
  {
    hexahedronRadiossIDs = file.ReadVector<int>(numberOfHexahedra);
  }

  if (isHierarchySaved)
  {
    auto partSubsets = file.ReadVector<int>(numberOfHexahedronParts);
    auto partMaterials = file.ReadVector<int>(numberOfHexahedronParts);
    auto partProperties = file.ReadVector<int>(numberOfHexahedronParts);
  }
}

void RadiossAnim::Read1DGeometry(
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

  std::vector<float> lineSkewArray;
  if (isLineSkewSaved)
  {
    lineSkewArray = file.ReadVector<float>(numberOfLines);
  }

  std::vector<float> lineMassArray;
  if (isMassSaved)
  {
    lineMassArray = file.ReadVector<float>(numberOfLines);
  }

  std::vector<int> lineRadiossIDs;
  if (isNodeNumberingElementSaved)
  {
    lineRadiossIDs = file.ReadVector<int>(numberOfLines);
  }

  if (isHierarchySaved)
  {
    auto partSubsets = file.ReadVector<int>(numberOfLineParts);
    auto partMaterials = file.ReadVector<int>(numberOfLineParts);
    auto partProperties = file.ReadVector<int>(numberOfLineParts);
  }
}
