#include "RadiossAnimFile.h"

#include <cstdint>
#include <sstream>

RadiossAnimFile::RadiossAnimFile(const std::string& animFilePath)
{
  this->Stream.open(animFilePath, std::ios::binary);
  if (this->Stream.fail())
  {
    std::stringstream message;
    message << "Fail to open the file: " << animFilePath << std::endl;
    throw std::runtime_error(message.str());
  }
}

bool RadiossAnimFile::ReadIntAsBool()
{
  auto value = ReadOne<int>();
  return value != 0;
}

void RadiossAnimFile::ReadData(void* dataBuffer, size_t sizeOfData, size_t numberOfData)
{
  char* charDataBuffer = static_cast<char*>(dataBuffer);
  this->Stream.read(charDataBuffer, sizeOfData * numberOfData);
  if (this->Stream.fail())
  {
    throw std::runtime_error("Failure while reading data");
  }

  // Endianess for Linux & Windows.
  for (size_t dataIndex = 0; dataIndex < numberOfData; ++dataIndex)
  {
    size_t bufferIndex = dataIndex * sizeOfData;
    if (sizeOfData == 2)
    {
      std::swap(charDataBuffer[bufferIndex + 0], charDataBuffer[bufferIndex + 1]);
    }
    else if (sizeOfData == 4)
    {
      std::swap(charDataBuffer[bufferIndex + 0], charDataBuffer[bufferIndex + 3]);
      std::swap(charDataBuffer[bufferIndex + 1], charDataBuffer[bufferIndex + 2]);
    }
    else if (sizeOfData == 8)
    {

      std::swap(charDataBuffer[bufferIndex + 0], charDataBuffer[bufferIndex + 7]);
      std::swap(charDataBuffer[bufferIndex + 1], charDataBuffer[bufferIndex + 6]);
      std::swap(charDataBuffer[bufferIndex + 2], charDataBuffer[bufferIndex + 5]);
      std::swap(charDataBuffer[bufferIndex + 3], charDataBuffer[bufferIndex + 4]);
    }
    else if (sizeOfData != 1)
    {
      std::stringstream message;
      message << "Unknow data size of: " << sizeOfData;
      throw std::runtime_error(message.str());
    }
  }
}

std::string RadiossAnimFile::ReadString(size_t stringSize)
{
  std::string text;
  if (stringSize > 0)
  {
    text.resize(stringSize);
    ReadData(&text[0], sizeof(char), text.size());
    auto endpos = text.find_first_of('\0');
    if (endpos != std::string::npos)
    {
      text.resize(endpos);
    }
  }
  return text;
}

std::vector<std::string> RadiossAnimFile::ReadStringVector(
  size_t numberOfStrings, size_t stringSize)
{
  std::vector<std::string> strings;

  for (size_t i = 0; i < numberOfStrings; ++i)
  {
    strings.push_back(this->ReadString(stringSize));
  }

  return strings;
}

std::vector<float> RadiossAnimFile::ReadFloatVectorFromShorts(size_t size)
{
  std::vector<float> values;
  if (size > 0)
  {
    auto shorts = this->ReadVector<uint16_t>(size);
    values.resize(size);
    for (size_t i = 0; i < size; ++i)
    {
      values[i] = ((float)shorts[i]) / 3000;
    }
  }
  return values;
}
