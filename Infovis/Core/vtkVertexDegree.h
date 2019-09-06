/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkVertexDegree
 * @brief   Adds an attribute array with the degree of each vertex
 *
 *
 * Adds an attribute array with the degree of each vertex. By default the name
 * of the array will be "VertexDegree", but that can be changed by calling
 * SetOutputArrayName("foo");
*/

#ifndef vtkVertexDegree_h
#define vtkVertexDegree_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkGraphAlgorithm.h"

class VTKINFOVISCORE_EXPORT vtkVertexDegree : public vtkGraphAlgorithm
{
public:
  static vtkVertexDegree *New();

  vtkTypeMacro(vtkVertexDegree, vtkGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the output array name. If no output array name is
   * set then the name 'VertexDegree' is used.
   */
  vtkSetStringMacro(OutputArrayName);
  //@}

protected:
  vtkVertexDegree();
  ~vtkVertexDegree() override;

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;

private:

  char* OutputArrayName;

  vtkVertexDegree(const vtkVertexDegree&) = delete;
  void operator=(const vtkVertexDegree&) = delete;
};

#endif

