/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkHierarchicalBoxDataSet
 * @brief   Backwards compatibility class
 *
 *
 * An empty class for backwards compatibility
 *
 * @sa
 * vtkUniformGridAM vtkOverlappingAMR vtkNonOverlappingAMR
*/

#ifndef vtkHierarchicalBoxDataSet_h
#define vtkHierarchicalBoxDataSet_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkOverlappingAMR.h"

class vtkInformation;
class vtkInformationVector;

class VTKCOMMONDATAMODEL_EXPORT vtkHierarchicalBoxDataSet:
  public vtkOverlappingAMR
{
public:
  static vtkHierarchicalBoxDataSet *New();
  vtkTypeMacro(vtkHierarchicalBoxDataSet,vtkOverlappingAMR);
  void PrintSelf(ostream &os, vtkIndent indent) override;

  /**
   * Return a new iterator (the iterator has to be deleted by user).
   */
  VTK_NEWINSTANCE vtkCompositeDataIterator* NewIterator() override;

  /**
   * Return class name of data type (see vtkType.h for definitions).
   */
  int GetDataObjectType() override {return VTK_HIERARCHICAL_BOX_DATA_SET;}

  //@{
  /**
   * Retrieve an instance of this class from an information object.
   */
  static vtkHierarchicalBoxDataSet* GetData(vtkInformation* info);
  static vtkHierarchicalBoxDataSet* GetData(vtkInformationVector* v, int i=0);
  //@}

protected:
  vtkHierarchicalBoxDataSet();
  ~vtkHierarchicalBoxDataSet() override;

private:
  vtkHierarchicalBoxDataSet(const vtkHierarchicalBoxDataSet&) = delete;
  void operator=(const vtkHierarchicalBoxDataSet&) = delete;
};

#endif
