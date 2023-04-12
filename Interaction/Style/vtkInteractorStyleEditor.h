/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleEditor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkInteractorStyleEditor
 * @brief   An editor-like interaction style
 *
 * vtkInteractorStyleEditor is an interactor style resembling the behaviour of
 * many 3d editors. For example Blender.
 *
 * The three mouse buttons are used as follows:
 *
 * Left :   selecting objects
 * Middle : navigation
 * Right :  reserved for context menu
 *
 * although the user can override this if required.
 *
 * The navigation assumes a clear vertical axis (0,0,1) that remains vertical. For
 * many users this feels natural.
 *
 * This style borrows/collects many elements from exising functionality:
 * - box select and area picker
 * - pan
 * - ....
 *
 * The only truely new routine is the camera rotation with is implemented in Rotate and
 * is invoked via VTKIS_ROTATE
 *
 * @sa
 * vtkInteractorStyleTrackballCamera
 */

#ifndef vtkInteractorStyleEditor_h
#define vtkInteractorStyleEditor_h

#include "vtkInteractionStyleModule.h" // For export macro
#include "vtkInteractorStyleTrackballCamera.h"

VTK_ABI_NAMESPACE_BEGIN
class vtkCellPicker;

class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyleEditor : public vtkInteractorStyleTrackballCamera
{
public:
  static vtkInteractorStyleEditor* New();
  vtkTypeMacro(vtkInteractorStyleEditor, vtkInteractorStyleTrackballCamera);
  

  // Override the Rotate method
  void Rotate() override;
  
protected:

  void RotateTurntableBy(float rxf, float ryf);
    

};

VTK_ABI_NAMESPACE_END
#endif

