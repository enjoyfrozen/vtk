/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkNetworkHierarchy
 * @brief   Filter that takes a graph and makes a
 * tree out of the network ip addresses in that graph.
 *
 *
 * Use SetInputArrayToProcess(0, ...) to set the array to that has
 * the network ip addresses.
 * Currently this array must be a vtkStringArray.
*/

#ifndef vtkNetworkHierarchy_h
#define vtkNetworkHierarchy_h

#include "vtkInfovisCoreModule.h" // For export macro
#include "vtkTreeAlgorithm.h"

class vtkStdString;

class VTKINFOVISCORE_EXPORT vtkNetworkHierarchy : public vtkTreeAlgorithm
{
public:
  static vtkNetworkHierarchy* New();
  vtkTypeMacro(vtkNetworkHierarchy,vtkTreeAlgorithm);

  //@{
  /**
   * Used to store the ip array name
   */
  vtkGetStringMacro(IPArrayName);
  vtkSetStringMacro(IPArrayName);
  //@}

  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkNetworkHierarchy();
  ~vtkNetworkHierarchy() override;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

  int FillOutputPortInformation(
    int vtkNotUsed(port), vtkInformation* info) override;
  int FillInputPortInformation(
    int vtkNotUsed(port), vtkInformation* info) override;

private:
  vtkNetworkHierarchy(const vtkNetworkHierarchy&) = delete;
  void operator=(const vtkNetworkHierarchy&) = delete;

  // Internal helper functions
  unsigned int ITON(const vtkStdString& ip);
  void GetSubnets(unsigned int packedIP, int *subnets);

  char *IPArrayName;

};

#endif

