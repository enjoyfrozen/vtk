/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkLSDynaSummaryParser
 *
 * This is a helper class used by vtkLSDynaReader to read XML files.
 * @sa
 * vtkLSDynaReader
*/

#ifndef vtkLSDynaSummaryParser_h
#define vtkLSDynaSummaryParser_h

#include "vtkIOLSDynaModule.h" // For export macro
#include "vtkXMLParser.h"
#include "vtkStdString.h" //needed for vtkStdString

class LSDynaMetaData;
class VTKIOLSDYNA_EXPORT vtkLSDynaSummaryParser : public vtkXMLParser
{
public:
  vtkTypeMacro(vtkLSDynaSummaryParser,vtkXMLParser);
  static vtkLSDynaSummaryParser* New();
  void PrintSelf(ostream &os, vtkIndent indent) override;



  /// Must be set before calling Parse();
  LSDynaMetaData* MetaData;

protected:
  vtkLSDynaSummaryParser();
  ~vtkLSDynaSummaryParser() override { };

  void StartElement(const char* name, const char** atts) override;
  void EndElement(const char* name) override;
  void CharacterDataHandler(const char* data, int length) override;

  vtkStdString PartName;
  int PartId;
  int PartStatus;
  int PartMaterial;
  int InPart;
  int InDyna;
  int InName;

private:
  vtkLSDynaSummaryParser( const vtkLSDynaSummaryParser& ) = delete;
  void operator = ( const vtkLSDynaSummaryParser& ) = delete;
};

#endif //vtkLSDynaReader_h
