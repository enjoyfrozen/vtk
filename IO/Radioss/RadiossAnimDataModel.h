#ifndef RADIOSSANIMDATAMODEL_H
#define RADIOSSANIMDATAMODEL_H

#include <cstdio>
#include <string>
#include <vector>

class RadiossAnimFile;

/**
 * @brief The RadiossAnim class read radioss anim file and store the data model.
 */
class RadiossAnimDataModel
{
public:
  RadiossAnimDataModel(const std::string& animFilePath);
  ~RadiossAnimDataModel();

  template <class T>
  struct Array
  {
    std::string name;
    std::vector<T> values;
  };

  struct Nodes
  {
    size_t Count = 0;
    std::vector<float> Coordinates;
    std::vector<Array<float>> ScalarFloatArrays;
    std::vector<Array<int>> ScalarIntArrays;
    std::vector<Array<float>> VectorArrays;
  };

  struct Part
  {
    std::string Name;
    int FirstCellIndex;
    int LastCellIndex;
  };

  struct Quads
  {
    size_t Count = 0;
    std::vector<int> Connectivity;
    std::vector<Part> Parts;
    std::vector<Array<float>> ScalarFloatArrays;
    std::vector<Array<int>> ScalarIntArrays;
    std::vector<Array<char>> ScalarCharArrays;
    std::vector<Array<float>> VectorArrays;
  };

  float GetTime() const;
  const Nodes& GetNodes() const;
  const Quads& GetQuads() const;

private:
  void ReadFile(const std::string& animFilePath);
  void ReadAndCheckFileFormat(RadiossAnimFile& file);
  void ReadNodesAndQuads(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);
  void ReadHexahedra(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);
  void ReadLines(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);

  float Time = 0.f;
  Nodes TheNodes;
  Quads TheQuads;
};

#endif // RADIOSSANIMDATAMODEL_H
