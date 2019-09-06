/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

/**
 * @class   vtkContextMapper2D
 * @brief   Abstract class for 2D context mappers.
 *
 *
 *
 * This class provides an abstract base for 2D context mappers. They currently
 * only accept vtkTable objects as input.
*/

#ifndef vtkContextMapper2D_h
#define vtkContextMapper2D_h

#include "vtkRenderingContext2DModule.h" // For export macro
#include "vtkAlgorithm.h"

class vtkContext2D;
class vtkTable;
class vtkDataArray;
class vtkAbstractArray;

class VTKRENDERINGCONTEXT2D_EXPORT vtkContextMapper2D : public vtkAlgorithm
{
public:
  vtkTypeMacro(vtkContextMapper2D, vtkAlgorithm);
  void PrintSelf(ostream &os, vtkIndent indent) override;
  static vtkContextMapper2D *New();

  //@{
  /**
   * Set/Get the input for this object - only accepts vtkTable as input.
   */
  virtual void SetInputData(vtkTable *input);
  virtual vtkTable * GetInput();
  //@}

  /**
   * Make the arrays accessible to the plot objects.
   */
  vtkDataArray *GetInputArrayToProcess(int idx,
                                       vtkDataObject* input)
  {
    return this->vtkAlgorithm::GetInputArrayToProcess(idx, input);
  }

  vtkAbstractArray *GetInputAbstractArrayToProcess(int idx,
                                       vtkDataObject* input)
  {
    return this->vtkAlgorithm::GetInputAbstractArrayToProcess(idx, input);
  }

protected:
  vtkContextMapper2D();
  ~vtkContextMapper2D() override;

  /**
   * Specify the types of input we can handle.
   */
  int FillInputPortInformation(int port, vtkInformation *info) override;

private:
  vtkContextMapper2D(const vtkContextMapper2D &) = delete;
  void operator=(const vtkContextMapper2D &) = delete;

};

#endif //vtkContextMapper2D_h
