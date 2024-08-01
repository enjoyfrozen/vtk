#ifndef RADIOSSANIM_H
#define RADIOSSANIM_H

#include <cstdio>
#include <string>

class RadiossAnimFile;

/**
 * @brief The RadiossAnim class read radioss anim file and store the data model.
 */
class RadiossAnim
{
public:
  RadiossAnim(const std::string& animFilePath);
  ~RadiossAnim();

private:
  void ReadFile(const std::string& animFilePath);
  void ReadAndCheckFileFormat(RadiossAnimFile& file);
  void Read2DGeometry(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);
  void Read3DGeometry(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);
  void Read1DGeometry(RadiossAnimFile& file, bool isMassSaved, bool isNodeNumberingElementSaved,
    bool isHierarchySaved);
};

#endif // RADIOSSANIM_H
