#ifndef RADIOSSANIMFILE_H
#define RADIOSSANIMFILE_H

#include <fstream>
#include <string>
#include <vector>

/**
 * @brief The RadiossAnimFile class is a thin wrapper with helper methods to read formatted data
 * from a file.
 */
class RadiossAnimFile
{
public:
  RadiossAnimFile(const std::string& animFilePath);
  ~RadiossAnimFile() = default;

  template <class T>
  T ReadOne();

  template <class T>
  std::vector<T> ReadVector(size_t size);

  bool ReadIntAsBool();
  std::string ReadString(size_t stringSize);
  std::vector<std::string> ReadStringVector(size_t numberOfStrings, size_t stringSize);
  std::vector<float> ReadFloatVectorFromShorts(size_t size);

private:
  void ReadData(void* dataBuffer, size_t sizeOfData, size_t numberOfData);
  std::ifstream Stream;
};

template <class T>
T RadiossAnimFile::ReadOne()
{
  T value = 0;
  ReadData(&value, sizeof(T), 1);
  return value;
}

template <class T>
std::vector<T> RadiossAnimFile::ReadVector(size_t size)
{
  std::vector<T> values;
  if (size > 0)
  {
    values.resize(size);
    ReadData(&values[0], sizeof(T), size);
  }
  return values;
}

#endif // RADIOSSANIMFILE_H
