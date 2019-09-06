/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkGraphGeodesicPath.h"

#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkExecutive.h"


//-----------------------------------------------------------------------------
vtkGraphGeodesicPath::vtkGraphGeodesicPath()
{
  this->StartVertex = 0;
  this->EndVertex   = 0;
}

//-----------------------------------------------------------------------------
vtkGraphGeodesicPath::~vtkGraphGeodesicPath() = default;

//-----------------------------------------------------------------------------
void vtkGraphGeodesicPath::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "StartVertex: " << this->StartVertex << endl;
  os << indent << "EndVertex: "   << this->EndVertex   << endl;
}

