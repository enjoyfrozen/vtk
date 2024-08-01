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
  auto isNodeElementListForTH = file.ReadIntAsBool();
  auto isNewSkewForTensor2DSaved = file.ReadIntAsBool();
  auto isSPHSaved = file.ReadIntAsBool();
  auto unused1 = file.ReadIntAsBool();
  auto unused2 = file.ReadIntAsBool();

  Read2DGeometry(file);
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

void RadiossAnim::Read2DGeometry(RadiossAnimFile& file)
{
  auto numberOfNodes = file.ReadOne<int>();
  auto numberOfQuads = file.ReadOne<int>();
  auto numberOfParts = file.ReadOne<int>();
  auto numberOfNodalScalarArrays = file.ReadOne<int>();
  auto numberOfQuadScalarArrays = file.ReadOne<int>();
  auto numberOfNodalVectorArrays = file.ReadOne<int>();
  auto numberOfQuadTensorArrays = file.ReadOne<int>();
  auto numberOfSkews = file.ReadOne<int>();

  auto skews = file.ReadFloatVectorFromShorts(numberOfSkews * 6);
  auto nodeCoordinates = file.ReadVector<float>(numberOfNodes * 3);
  auto quadConnectivity = file.ReadVector<int>(numberOfQuads * 4);
  auto quadErosionStatus = file.ReadVector<char>(numberOfQuads);
  auto partLastQuadIndices = file.ReadVector<int>(numberOfParts);
  auto partNames = file.ReadStringVector(numberOfParts, 50);
  auto nodeNorms = file.ReadFloatVectorFromShorts(numberOfNodes * 3);

  // Arrays
  auto scalarArrayNames =
    file.ReadStringVector(numberOfNodalScalarArrays + numberOfQuadScalarArrays, 81);
  auto nodeScalarArrays = file.ReadVector<float>(numberOfNodalScalarArrays * numberOfNodes);
  auto elementScalarArrays = file.ReadVector<float>(numberOfQuadScalarArrays * numberOfQuads);
  auto nodeVectorArrayNames = file.ReadStringVector(numberOfNodalVectorArrays, 81);
  auto nodeVectorArrays = file.ReadVector<float>(3 * numberOfNodes * numberOfNodalVectorArrays);
  auto quadTensorArrayNames = file.ReadStringVector(numberOfQuadTensorArrays, 81);
  auto quadTensorArrays = file.ReadVector<float>(numberOfQuads * 3 * numberOfQuadTensorArrays);
}
