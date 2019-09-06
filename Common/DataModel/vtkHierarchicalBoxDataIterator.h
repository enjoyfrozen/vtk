/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHierarchicalBoxDataIterator
 *
 *
 *  Empty class for backwards compatibility.
*/

#ifndef vtkHierarchicalBoxDataIterator_h
#define vtkHierarchicalBoxDataIterator_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkUniformGridAMRDataIterator.h"

class VTKCOMMONDATAMODEL_EXPORT vtkHierarchicalBoxDataIterator :
  public vtkUniformGridAMRDataIterator
{
  public:
    static vtkHierarchicalBoxDataIterator* New();
    vtkTypeMacro(vtkHierarchicalBoxDataIterator,vtkUniformGridAMRDataIterator);
    void PrintSelf(ostream &os, vtkIndent indent) override;

  protected:
    vtkHierarchicalBoxDataIterator();
    ~vtkHierarchicalBoxDataIterator() override;

  private:
    vtkHierarchicalBoxDataIterator(const vtkHierarchicalBoxDataIterator&) = delete;
    void operator=(const vtkHierarchicalBoxDataIterator&) = delete;
};

#endif /* VTKHIERARCHICALBOXDATAITERATOR_H_ */
