/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#ifndef vtkADIOS2VTXReader_h
#define vtkADIOS2VTXReader_h

#include <memory> // std::unique_ptr

#include "vtkIOADIOS2Module.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"

// forward declaring to keep it private
namespace vtx
{
class VTXSchemaManager;
}

class vtkIndent;
class vtkInformation;
class vtkInformationvector;

class VTKIOADIOS2_EXPORT vtkADIOS2VTXReader : public vtkMultiBlockDataSetAlgorithm
{
public:
    static vtkADIOS2VTXReader *New();
    vtkTypeMacro(vtkADIOS2VTXReader, vtkMultiBlockDataSetAlgorithm);
    void PrintSelf(ostream &os, vtkIndent index) override;

    vtkSetStringMacro(FileName);
    vtkGetStringMacro(FileName);

protected:
    vtkADIOS2VTXReader();
    ~vtkADIOS2VTXReader() = default;

    vtkADIOS2VTXReader(const vtkADIOS2VTXReader &) = delete;
    void operator=(const vtkADIOS2VTXReader &) = delete;

    int RequestInformation(vtkInformation *, vtkInformationVector **,
                           vtkInformationVector *outputVector);
    int RequestUpdateExtent(vtkInformation *, vtkInformationVector **,
                            vtkInformationVector *outputVector);
    int RequestData(vtkInformation *, vtkInformationVector **,
                    vtkInformationVector *outputVector);

private:
    char *FileName;
    std::unique_ptr<vtx::VTXSchemaManager> SchemaManager;
};

#endif /* vtkADIOS2VTXReader_h */
