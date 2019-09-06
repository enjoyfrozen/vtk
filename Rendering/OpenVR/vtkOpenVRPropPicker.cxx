/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkOpenVRPropPicker.h"

#include "vtkAssemblyNode.h"
#include "vtkAssemblyPath.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkMath.h"
#include "vtkCamera.h"
#include "vtkBox.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor3D.h"
#include "vtkTransform.h"

vtkStandardNewMacro(vtkOpenVRPropPicker);

vtkOpenVRPropPicker::vtkOpenVRPropPicker()
{
}

vtkOpenVRPropPicker::~vtkOpenVRPropPicker()
{
}

// set up for a pick
void vtkOpenVRPropPicker::Initialize()
{
#ifndef VTK_LEGACY_SILENT
  vtkErrorMacro("This class is deprecated: Please use vtkPropPicker directly instead of this class");
#endif
  this->vtkAbstractPropPicker::Initialize();
}

void vtkOpenVRPropPicker::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
