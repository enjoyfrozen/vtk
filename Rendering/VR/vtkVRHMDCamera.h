/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkVRHMDCamera
 * @brief   A superclass for HMD style cameras
 *
 */

#ifndef vtkVRHMDCamera_h
#define vtkVRHMDCamera_h

#include "vtkNew.h"               // ivars
#include "vtkRenderingVRModule.h" // For export macro
#include "vtkVRCamera.h"

class vtkRenderer;
class vtkMatrix4x4;

class VTKRENDERINGVR_EXPORT vtkVRHMDCamera : public vtkVRCamera
{
public:
  vtkTypeMacro(vtkVRHMDCamera, vtkVRCamera);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Implement base class method.
   */
  void Render(vtkRenderer* ren) override;

  void GetKeyMatrices(vtkRenderer* ren, vtkMatrix4x4*& WCVCMatrix, vtkMatrix3x3*& normalMatrix,
    vtkMatrix4x4*& VCDCMatrix, vtkMatrix4x4*& WCDCMatrix) override;

  /**
   * Provides a matrix to go from physical coordinates to projection coordinates
   * for the eye currently being rendered. Just e.g. LeftEyeToProjection *
   * PhysicalToLeftEye
   */
  void GetPhysicalToProjectionMatrix(vtkMatrix4x4*& physicalToProjectionMatrtix) override;

protected:
  vtkVRHMDCamera();
  ~vtkVRHMDCamera() override;

  // you must provide these two methods in your subclass
  virtual void UpdateWorldToEyeMatrices(vtkRenderer*) = 0;
  virtual void UpdateEyeToProjectionMatrices(vtkRenderer*) = 0;

  // all the matrices below are stored in VTK convention
  // as A = Mx where x is a column vector.

  // adds in the physical to hmd part
  vtkNew<vtkMatrix4x4> PhysicalToLeftEyeMatrix;
  vtkNew<vtkMatrix4x4> PhysicalToRightEyeMatrix;

  // adds in the world to physical part
  vtkNew<vtkMatrix4x4> WorldToLeftEyeMatrix;
  vtkNew<vtkMatrix4x4> WorldToRightEyeMatrix;

  // we basically get these from OpenVR
  vtkNew<vtkMatrix4x4> LeftEyeToProjectionMatrix;
  vtkNew<vtkMatrix4x4> RightEyeToProjectionMatrix;

  // computed using the above matrices
  vtkNew<vtkMatrix4x4> PhysicalToProjectionMatrixForLeftEye;
  vtkNew<vtkMatrix4x4> PhysicalToProjectionMatrixForRightEye;

  vtkNew<vtkMatrix4x4> PhysicalToHMDMatrix;
  vtkNew<vtkMatrix4x4> WorldToPhysicalMatrix;

private:
  vtkVRHMDCamera(const vtkVRHMDCamera&) = delete;
  void operator=(const vtkVRHMDCamera&) = delete;
};

#endif
