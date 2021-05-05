#include "cxxopts.hpp"
#include "tiler.h"

#include <vtkLogger.h>

int main(int argc, char** argv)
{
  cxxopts::Options options(
    "tiler", "Converts large 3D geospatial datasets to the 3D Tiles format.");
  options.show_positional_help();
  try
  {
    vtkLogger::Init(argc, argv);

    options.add_options()("b,buildings_per_tile",
      "Maximum number of buildings per tile. "
      "Optional.",
      cxxopts::value<int>()->default_value("10"))("h,help", "Print this message.")("i,input",
      "Input files (obj or citygml) or directories. "
      "We read all files of a known type from each directory and "
      "add them to the list. "
      "These can be passed as positional parameters as well.",
      cxxopts::value<std::vector<std::string>>())("dont_save_gltf",
      "Create only tileset.json not the B3DM files",
      cxxopts::value<bool>()->default_value("false"))("l,lod",
      "Level of detail to be read (if available)"
      "Optional.",
      cxxopts::value<int>()->default_value("2"))

      ("dont_save_textures", "Don't save textures even if available",
        cxxopts::value<bool>()->default_value("false"))("n,number_of_buildings",
        "Maximum number of buildings. "
        "Optional.",
        cxxopts::value<int>()->default_value("2147483647"))("o,output",
        "A directory where the 3d-tiles dataset is created. ", cxxopts::value<std::string>())(
        "srs_name", "", cxxopts::value<std::string>())("utm_hemisphere",
        "UTM hemisphere for the OBJ file coordinates "
        "(can be N or S).",
        cxxopts::value<char>())("t,translation",
        "Translation for x,y,z. Optional. "
        "The translation can be also read as a comment in the OBJ file "
        "using the following format at the top of the file:\n"
        "#x offset: ...\n"
        "#y offset: ...\n"
        "#z offset: ...\n"
        "When both are available, they are added up.",
        cxxopts::value<std::vector<double>>()->default_value("0.0,0.0,0.0"))("utm_zone",
        "UTM zone for the OBJ file coordinates "
        "(can be 1 to 61).",
        cxxopts::value<int>());
    options.parse_positional("input");
    auto result = options.parse(argc, argv);

    if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      return 0;
    }
    int buildingsPerTile = result["buildings_per_tile"].as<int>();
    int numberOfBuildings = result["number_of_buildings"].as<int>();
    int lod = result["lod"].as<int>();
    ;
    std::vector<double> translation = result["translation"].as<std::vector<double>>();
    if (translation.size() != 3)
    {
      vtkLog(ERROR, << "Expect three parameters for translation.");
      std::cout << options.help() << std::endl;
      return 1;
    }
    if (result.count("input") == 0)
    {
      vtkLog(ERROR, << "Input files or directories are missing." << endl);
      std::cout << options.help() << std::endl;
      return 1;
    }
    if (result.count("output") == 0)
    {
      vtkLog(ERROR, << "Output directory is missing." << endl);
      std::cout << options.help() << std::endl;
      return 1;
    }
    auto input = result["input"].as<std::vector<std::string>>();
    auto output = result["output"].as<std::string>();
    bool dontSaveGLTF = result["dont_save_gltf"].as<bool>();
    bool dontSaveTextures = result["dont_save_textures"].as<bool>();
    int utmZone = 0;
    char utmHemisphere = 'N';
    std::string srsName;
    if (result.count("utm_zone") && result.count("utm_hemisphere"))
    {
      utmZone = result["utm_zone"].as<int>();
      utmHemisphere = result["utm_hemisphere"].as<char>();
      utmHemisphere = toupper(utmHemisphere);
      if (utmHemisphere == 'N' && utmHemisphere == 'S')
      {
        vtkLog(ERROR, "utm_hemisphere has to be N or S but is: " << utmHemisphere);
        return 1;
      }
    }
    else if (result.count("srs_name"))
    {
      srsName = result["srs_name"].as<std::string>();
    }
    else
    {
      vtkLog(ERROR, << "Error: srs_name or utm_zone/utm_hemisphere are missing.");
      std::cout << options.help() << std::endl;
      return 1;
    }
    tiler(input, output, numberOfBuildings, buildingsPerTile, lod, translation, !dontSaveGLTF,
      !dontSaveTextures, srsName, utmZone, utmHemisphere);
    return 0;
  }
  catch (const std::runtime_error& e)
  {
    vtkLog(ERROR, << e.what());
  }
  catch (const cxxopts::OptionException& e)
  {
    vtkLog(ERROR, << e.what());
    std::cout << options.help() << std::endl;
    return 1;
  }
}
