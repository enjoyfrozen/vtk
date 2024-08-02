#ifndef RADIOSSANIM_H
#define RADIOSSANIM_H

#include <cstdio>
#include <string>
#include <vector>

class RadiossAnimFile;

/**
 * @brief The RadiossAnim class read radioss anim file and store the data model.
 */
class RadiossAnim
{
public:
  RadiossAnim(const std::string& animFilePath);
  ~RadiossAnim();

  struct FloatArray
  {
    std::string name;
    std::vector<float> values;
  };

  struct IntArray
  {
    std::string name;
    std::vector<int> values;
  };

  struct Nodes
  {
    size_t Count;
    std::vector<float> Coordinates;
    std::vector<FloatArray> ScalarFloatArrays;
    std::vector<IntArray> ScalarIntArrays;
    std::vector<FloatArray> VectorArrays;
  };

  float GetTime() const;
  const Nodes& GetNodes() const;

private:
  void ReadFile(const std::string& animFilePath);
  void ReadAndCheckFileFormat(RadiossAnimFile& file);
  void Read2DGeometry(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);
  void Read3DGeometry(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);
  void Read1DGeometry(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);

  float Time = 0.f;
  Nodes TheNodes = { 0, {}, {}, {}, {} };
};

#endif // RADIOSSANIM_H
