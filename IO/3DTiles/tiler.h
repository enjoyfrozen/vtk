#ifndef _tiler_h
#define _tiler_h

#include <string>
#include <vector>

void tiler(const std::vector<std::string>& input, const std::string& output, int numberOfBuildings,
  int buildingsPerTile, int lod, const std::vector<double>& translation, bool saveGLTF,
  bool saveTextures, const std::string& srsName, int utmZone, char utmHemisphere);

#endif
