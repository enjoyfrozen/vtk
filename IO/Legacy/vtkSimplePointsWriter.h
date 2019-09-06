/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSimplePointsWriter
 * @brief   write a file of xyz coordinates
 *
 * vtkSimplePointsWriter writes a simple file of xyz coordinates
 *
 * @sa
 * vtkSimplePointsReader
*/

#ifndef vtkSimplePointsWriter_h
#define vtkSimplePointsWriter_h

#include "vtkIOLegacyModule.h" // For export macro
#include "vtkDataSetWriter.h"

class VTKIOLEGACY_EXPORT vtkSimplePointsWriter : public vtkDataSetWriter
{
public:
  static vtkSimplePointsWriter *New();
  vtkTypeMacro(vtkSimplePointsWriter,vtkDataSetWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkGetMacro(DecimalPrecision, int);
  vtkSetMacro(DecimalPrecision, int);

protected:
  vtkSimplePointsWriter();
  ~vtkSimplePointsWriter() override{}

  void WriteData() override;

  int DecimalPrecision;

private:
  vtkSimplePointsWriter(const vtkSimplePointsWriter&) = delete;
  void operator=(const vtkSimplePointsWriter&) = delete;
};

#endif
