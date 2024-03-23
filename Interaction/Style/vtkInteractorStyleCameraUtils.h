/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleCameraUtils.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkInteractorStyleCameraUtils
 * @brief   utilities for camera manipulation
 *
 * vtkInteractorStyleCameraUtils provides a series of routines used for moving the camera which can
 * be shared and reused across multiple different interactor styles, or called directly by an
 * application to allow for camera movement by background processes regardless of the selected
 * interactor.
 */

#ifndef vtkInteractorStyleCameraUtils_h
#define vtkInteractorStyleCameraUtils_h

#include "vtkInteractionStyleModule.h" // For export macro
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

enum class vtkDollyModel : int
{
  Centered = 0, // Dolly relative to screen center
  Targeted      // Dolly centered on target point (mouse position)
};

enum class vtkTrackballRotationModel : int
{
  AzimuthElevation = 0,
  Singularity,
  WorldZ_ScreenX
};

enum class vtkZoomDirection : int
{
  ZoomingIn = 0,
  ZoomingOut
};

class VTKINTERACTIONSTYLE_EXPORT vtkInteractorStyleCameraUtils
{
public:
  /*
   * The minimum value for parallel projection's parallel scale determining how much the view can be
   * zoomed in.
   *
   * This value was determined empirically to be as low as possible without affecting the accuracy
   * of view elements such as camera rotations and ruler bars.
   */
  static constexpr double MINIMUM_PARALLEL_SCALE_VALUE = 1.0e-16;

  /*
   * The maximum value for parallel projection's parallel scale determining how much the view can be
   * zoomed out.
   *
   * The value for this constant was determined empirically to be significantly high considering the
   * observed models and the precision of the cross-sectional view ruler bar numbers.
   */
  static constexpr double MAXIMUM_PARALLEL_SCALE_VALUE = 1.0e100;

  /*
   * Determines whether the specified parallel projection value is within the allowed zooming
   * bounds, considering whether the caller is zooming in or out of the view.
   *
   * This type of validation is only needed when using parallel projection. For perspective
   * projection, VTK already limits the camera's view angle between 0.00000001 and 179.0 degrees
   * (see vtkCamera::SetViewAngle()). The zooming direction is considered by this method to prevent
   * zooming outside the bounds but to allow zooming back within bounds if already outside of it.
   *
   * isZoomingIn: Whether the caller is zooming in or out of the view.
   * parallelScale: Whether the specified parallel projection value is within the allowed zooming
   * bounds.
   */
  static bool IsParallelProjectionZoomingWithinBounds(
    vtkZoomDirection zoomDirection, double parallelScale);

  /*
   * Determines whether zooming is valid if the provided renderer is using parallel projection.
   *
   * renderer: The renderer being zoomed in/out.
   * isZoomingIn: Whether the view is being zoomed in or out.
   *
   * returns: Whether zooming is valid if the provided renderer is using parallel projection.
   */
  static bool IsParallelProjectionZoomingValid(
    vtkRenderer* renderer, vtkZoomDirection zoomDirection);

  /*
   * Rotates the camera
   *
   * renderer: Current renderer (which contains the camera)
   * phi: Degrees to rotate around screen X (right handed rotation)
   * theta: Degrees to rotate around world Z (right handed rotation)
   * autoAdjustCameraClippingRange: Whether to adjust the clipping planes so moved polydata stays
   * within a visible range
   * lightFollowCamera: Should the light follow the camera
   *
   * This method is static so applications which use multiple camera interactors
   * can always have access to this rotation if they wish to have autotomatic
   * rotation happening in the background without user interation.
   */
  static void RotateCameraAroundWorldZScreenX(vtkRenderer* renderer, double phi, double theta,
    bool autoAdjustCameraClippingRange, bool lightFollowCamera);

  /*
   * Moves the camera forward/backward based on the specified factor centered around the current
   * mouse cursor position. This allows a user to point at an area of interest and zoom in on that
   * point while keeping it in view, even on the edges of the viewport.
   *
   * interactor: The current interactor
   * renderer: Current renderer (which contains the camera)
   * factor: Dolly factor to zoom by
   */
  static void DollyTargeted(
    vtkRenderWindowInteractor* interactor, vtkRenderer* renderer, double factor);

  /*
   * Generates a string representation for the vtkDollyModel enum
   */
  static const char* DollyModelToString(vtkDollyModel model);

  /*
   * Generates a string representation for the vtkTrackballRotationModel enum
   */
  static const char* TrackballRotationModelToString(vtkTrackballRotationModel model);
};

#endif

// VTK-HeaderTest-Exclude: vtkInteractorStyleCameraUtils.h
