/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkJSONSceneExporter
 * @brief   Export the content of a vtkRenderWindow into a directory with
 *          a JSON meta file describing the scene along with the http datasets
 *
 * @warning
 * This writer assume LittleEndian by default. Additional work should be done to properly
 * handle endianness.
 */

#ifndef vtkJSONSceneExporter_h
#define vtkJSONSceneExporter_h

#include "vtkExporter.h"
#include "vtkIOExportModule.h" // For export macro

#include <map>    // For string parameter
#include <string> // For string parameter

class vtkActor;
class vtkDataObject;
class vtkDataSet;
class vtkScalarsToColors;

class VTKIOEXPORT_EXPORT vtkJSONSceneExporter : public vtkExporter
{
public:
  static vtkJSONSceneExporter* New();
  vtkTypeMacro(vtkJSONSceneExporter, vtkExporter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify file name of vtk data file to write.
   * This correspond to the root directory of the data to write.
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

protected:
  vtkJSONSceneExporter();
  ~vtkJSONSceneExporter() override;

  void WriteDataObject(ostream& os, vtkDataObject* dataObject, vtkActor* actor);
  std::string ExtractRenderingSetup(vtkActor* actor);
  std::string WriteDataSet(vtkDataSet* dataset, const char* addOnMeta);
  void WriteLookupTable(const char* name, vtkScalarsToColors* lookupTable);

  void WriteData() override;

  char* FileName;
  int DatasetCount;
  std::map<std::string, std::string> LookupTables;

private:
  vtkJSONSceneExporter(const vtkJSONSceneExporter&) = delete;
  void operator=(const vtkJSONSceneExporter&) = delete;
};

#endif
