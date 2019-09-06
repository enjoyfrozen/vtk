/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkGeodesicPath.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkExecutive.h"


//-----------------------------------------------------------------------------
vtkGeodesicPath::vtkGeodesicPath()
{
  this->SetNumberOfInputPorts(1);
}

//-----------------------------------------------------------------------------
vtkGeodesicPath::~vtkGeodesicPath() = default;

//-----------------------------------------------------------------------------
int vtkGeodesicPath::FillInputPortInformation(int port,
                                              vtkInformation *info)
{
  if (port == 0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
    return 1;
  }
  return 0;
}

//-----------------------------------------------------------------------------
void vtkGeodesicPath::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

