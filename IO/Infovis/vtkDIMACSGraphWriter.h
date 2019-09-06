/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkDIMACSGraphWriter
 * @brief   write vtkGraph data to a DIMACS
 * formatted file
 *
 *
 * vtkDIMACSGraphWriter is a sink object that writes
 * vtkGraph data files into a generic DIMACS (.gr) format.
 *
 * Output files contain a problem statement line:
 *
 * p graph <num_verts> <num_edges>
 *
 * Followed by |E| edge descriptor lines that are formatted as:
 *
 * e <source> <target> <weight>
 *
 * Vertices are numbered from 1..n in DIMACS formatted files.
 *
 * See webpage for format details.
 * http://prolland.free.fr/works/research/dsat/dimacs.html
 *
 * @sa
 * vtkDIMACSGraphReader
 *
*/

#ifndef vtkDIMACSGraphWriter_h
#define vtkDIMACSGraphWriter_h

#include "vtkIOInfovisModule.h" // For export macro
#include "vtkDataWriter.h"

class vtkGraph;

class VTKIOINFOVIS_EXPORT vtkDIMACSGraphWriter : public vtkDataWriter
{
public:
  static vtkDIMACSGraphWriter *New();
  vtkTypeMacro(vtkDIMACSGraphWriter,vtkDataWriter);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get the input to this writer.
   */
  vtkGraph* GetInput();
  vtkGraph* GetInput(int port);
  //@}

protected:
  vtkDIMACSGraphWriter() {}
  ~vtkDIMACSGraphWriter() override {}

  void WriteData() override;

  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkDIMACSGraphWriter(const vtkDIMACSGraphWriter&) = delete;
  void operator=(const vtkDIMACSGraphWriter&) = delete;
};

#endif
