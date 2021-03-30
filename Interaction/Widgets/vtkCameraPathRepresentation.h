/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCameraPathRepresentation.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkCameraPathRepresentation
 * @brief   representation for a camera path.
 *
 * vtkCameraPathRepresentation is a vtkWidgetRepresentation for a camera path.
 * This 3D widget defines a camera path that can be interactively manipulated in a
 * scene. The camera path has camera handles, that can be added and deleted, plus they
 * can be picked on the path itself to be translated in the scene.
 * This representation is used with vtkCameraPathWidget.
 * @sa
 * vtkCameraPathWidget
 */

#ifndef vtkCameraPathRepresentation_h
#define vtkCameraPathRepresentation_h

#include "vtkAbstractSplineRepresentation.h"
#include "vtkInteractionWidgetsModule.h" // For export macro

class vtkCameraHandleSource;
class vtkCamera;

class VTKINTERACTIONWIDGETS_EXPORT vtkCameraPathRepresentation
  : public vtkAbstractSplineRepresentation
{
public:
  static vtkCameraPathRepresentation* New();
  vtkTypeMacro(vtkCameraPathRepresentation, vtkAbstractSplineRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Get the latest modified Camera index.
   */
  vtkGetMacro(LastModifiedCamera, int);

  using vtkAbstractSplineRepresentation::SetHandlePosition;
  void SetHandlePosition(int handle, double x, double y, double z) override;

  //@{
  /**
   * Set/Get the focal point of the camera handles. Call GetNumberOfHandles
   * to determine the valid range of handle indices.
   */
  virtual void SetHandleFocalPoint(int handle, double x, double y, double z);
  virtual void SetHandleFocalPoint(int handle, double xyz[3]);
  virtual void GetHandleFocalPoint(int handle, double xyz[3]);
  virtual const double* GetHandleFocalPoint(int handle);
  //@}

  //@{
  /**
   * Set / Get the current handle position.
   */
  virtual void SetCurrentHandlePosition(double x, double y, double z);
  virtual const double* GetCurrentHandlePosition() VTK_SIZEHINT(3);
  //@}

  //@{
  /**
   * Set / Get the current handle focal point.
   */
  virtual void SetCurrentHandleFocalPoint(double x, double y, double z);
  virtual const double* GetCurrentHandleFocalPoint() VTK_SIZEHINT(3);
  //@}

  /**
   * Sets the representation to be a curve interpolating multiple cameras pointing to their focal
   * point. If set to false, the cameras are only represented as spheres.
   * Default is true.
   */
  void SetDirectional(bool val) override;

  /**
   * Adjust the number of camera handles while keeping the same path.
   * Delete and Allocate Handles as needed.
   */
  void SetNumberOfHandles(int npts) override;

  /**
   * Set the parametric spline object.
   */
  void SetParametricSpline(vtkParametricSpline* spline) override;

  /**
   * Convenience method to allocate and set the Camera handles from a vtkPoints
   * instance (corresponding to the camera positions).
   * If the first and last points are the same, the spline sets
   * Closed to the on InteractionState and disregards the last point, otherwise Closed
   * remains unchanged.
   */
  void InitializeHandles(vtkPoints* points) override;

  /**
   * Method that satisfy vtkWidgetRepresentation API.
   * Updates the spline in relation with the handles positions
   * and updates vtkWidgetRepresentation::InitialLength
   * (useful for the sizing methods).
   */
  void BuildRepresentation() override;

  /**
   * Redefinition of the parent function in order to call modified() whenever there is
   * an interaction with the widget.
   */
  void WidgetInteraction(double e[2]) override;

  /**
   * Add a default camera to the path at the index position.
   * By default, interpolate the position and the focal point from the two surrounding existing
   * cameras. If no camera exist before this call, create a new one at (0, 0, 0) and looking at (1,
   * 0, 0). If only one exist, the new camera is a copy of the existing, translated by (1, 1, 1)
   */
  void AddDefaultCamera(int index);

  /**
   * Add a camera to the path.
   */
  void AddCameraAt(vtkCamera* camera, int index);

  /**
   * Remove a camera from the path.
   */
  void DeleteCameraAt(int index);

protected:
  vtkCameraPathRepresentation();
  ~vtkCameraPathRepresentation() override = default;

  /**
   * Resize the camera handles on demand.
   */
  void SizeHandles() override;

  /**
   * Delete all camera handles.
   */
  void ClearHandles() override;

  /**
   * Creates a new handle from a vtkcamera and
   * insert it in the handles collection.
   */
  void InsertCamera(vtkCamera* camera, int index);

  /**
   * Create/Recreate npts default camera handles.
   */
  void CreateDefaultHandles(int npts) override;

  /**
   * Recreate the handles according to a
   * number of points equal to newNPts.
   * The old number of handles must be entered
   * as oldNPts.
   * It uses the current spline to recompute
   * the positions of the new handles.
   */
  void ReconfigureHandles(int newNPts, int oldNPts);

  void ReconfigureHandles(int newNpts) override;

  /**
   * Specialized method to insert a camera handle on the camera path.
   */
  int InsertHandleOnLine(double* pos) override;

  /**
   * Specialized method to erase a camera handle from the camera path.
   */
  void EraseHandle(const int&) override;

  // Specialized methods to access handles
  vtkActor* GetHandleActor(int index) override;
  vtkHandleSource* GetHandleSource(int index) override;
  virtual int GetHandleIndex(vtkProp* prop) override;

private:
  vtkCameraPathRepresentation(const vtkCameraPathRepresentation&) = delete;
  void operator=(const vtkCameraPathRepresentation&) = delete;

  void RebuildRepresentation();

  void UpdateConfiguration(int npts);

  std::vector<vtkSmartPointer<vtkCameraHandleSource>> CameraHandles;
  std::vector<vtkSmartPointer<vtkActor>> HandleActors;

  int LastModifiedCamera;
};

#endif
