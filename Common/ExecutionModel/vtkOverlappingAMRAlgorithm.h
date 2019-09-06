/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkOverlappingAMRAlgorithm
 *
 *
 *  A base class for all algorithms that take as input vtkOverlappingAMR and
 *  produce vtkOverlappingAMR.
*/

#ifndef vtkOverlappingAMRAlgorithm_h
#define vtkOverlappingAMRAlgorithm_h

#include "vtkCommonExecutionModelModule.h" // For export macro
#include "vtkUniformGridAMRAlgorithm.h"

class vtkOverlappingAMR;
class vtkInformation;

class VTKCOMMONEXECUTIONMODEL_EXPORT vtkOverlappingAMRAlgorithm :
  public vtkUniformGridAMRAlgorithm
{
  public:
    static vtkOverlappingAMRAlgorithm* New();
    vtkTypeMacro(vtkOverlappingAMRAlgorithm,vtkUniformGridAMRAlgorithm);
    void PrintSelf(ostream& os, vtkIndent indent) override;

    //@{
    /**
     * Get the output data object for a port on this algorithm
     */
    vtkOverlappingAMR* GetOutput();
    vtkOverlappingAMR* GetOutput(int);
    //@}

  protected:
    vtkOverlappingAMRAlgorithm();
    ~vtkOverlappingAMRAlgorithm() override;

    //@{
    /**
     * See algorithm for more info.
     */
    int FillOutputPortInformation(int port, vtkInformation* info) override;
    int FillInputPortInformation(int port, vtkInformation* info) override;
    //@}

  private:
    vtkOverlappingAMRAlgorithm(const vtkOverlappingAMRAlgorithm&) = delete;
    void operator=(const vtkOverlappingAMRAlgorithm&) = delete;
};

#endif /* VTKOVERLAPPINGAMRALGORITHM_H_ */
