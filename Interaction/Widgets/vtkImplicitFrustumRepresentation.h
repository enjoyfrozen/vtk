// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
/**
 * @class vtkImplicitFrustumRepresentation
 * @brief defining the representation for a vtkImplicitFrustumWidget
 *
 * This class is a concrete representation for the
 * vtkImplicitFrustumWidget. It represents an infinite frustum
 * defined by its inner/outer radiuses, its origin, and its axis. The frustum is placed
 * within its associated bounding box and the intersection of the
 * frustum with the bounding box is shown to visually indicate the
 * orientation and position of the representation. This frustum
 * representation can be manipulated by using the
 * vtkImplicitFrustumWidget to adjust the frustum angle, axis,
 * and/or origin point. (Note that the bounding box is defined during
 * invocation of the superclass' PlaceWidget() method.)
 *
 * To use this representation, you normally specify inner and outer radii, origin,
 * and axis, and a resolution for the frustum. Finally, place the widget and
 * its representation in the scene using PlaceWidget().
 *
 * @sa
 * vtkImplicitFrustumWidget vtkFrustum
 */

#ifndef vtkImplicitFrustumRepresentation_h
#define vtkImplicitFrustumRepresentation_h

#include "vtkEllipseArcSource.h"
#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkOrientationRepresentation.h"
#include "vtkOrientationWidget.h"
#include "vtkSphereHandleRepresentation.h"
#include "vtkVector.h" // For vtkVector3d
#include "vtkWidgetRepresentation.h"
#include "vtkWrappingHints.h" // For VTK_MARSHALAUTO

VTK_ABI_NAMESPACE_BEGIN
class vtkActor;
class vtkPolyDataMapper;
class vtkConeSource;
class vtkLineSource;
class vtkSphereSource;
class vtkTubeFilter;
class vtkPlanes;
class vtkProperty;
class vtkImageData;
class vtkOutlineFilter;
class vtkPolyData;
class vtkBox;
class vtkCellPicker;
class vtkFrustum;

#define VTK_MAX_FRUSTUM_RESOLUTION 2048

class VTKINTERACTIONWIDGETS_EXPORT VTK_MARSHALAUTO vtkImplicitFrustumRepresentation
  : public vtkWidgetRepresentation
{
public:
  // Manage the state of the widget
  enum InteractionStateType
  {
    Outside = 0,
    Moving, // Generic state set by the widget
    MovingOutline,
    MovingOrigin,
    RotatingAxis,
    Scaling,
    AdjustingHorizontalAngle,
    AdjustingVerticalAngle,
    AdjustingNearPlaneDistance,
    AdjustingYaw,
    AdjustingPitch,
    AdjustingRoll,
    TranslatingOrigin
  };

  static vtkImplicitFrustumRepresentation* New();
  vtkTypeMacro(vtkImplicitFrustumRepresentation, vtkWidgetRepresentation);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Get the origin of the frustum representation. The origin is located along the
   * frustum axis.
   * Default is (0, 0, 0)
   */
  void SetOrigin(double x, double y, double z);
  void SetOrigin(double x[3]);
  double* GetOrigin() VTK_SIZEHINT(3);
  void GetOrigin(double xyz[3]) const;
  ///@}

  ///@{
  /**
   * Set/Get the axis of rotation for the frustum. If the axis is not
   * specified as a unit vector, it will be normalized.
   * Default is the Y-Axis (0, 1, 0)
   */
  void SetAxis(double x, double y, double z);
  void SetAxis(double a[3]);
  double* GetAxis() VTK_SIZEHINT(3);
  void GetAxis(double a[3]) const;
  ///@}

  ///@{
  /**
   */
  double GetHorizontalAngle() const;
  void SetHorizontalAngle(double angle);
  ///@}

  ///@{
  /**
   */
  double GetVerticalAngle() const;
  void SetVerticalAngle(double angle);
  ///@}

  ///@{
  /**
   */
  double GetNearPlaneDistance() const;
  void SetNearPlaneDistance(double angle);
  ///@}

  ///@{
  /**
   * Force the frustum widget to be aligned with one of the x-y-z axes.
   * If one axis is set on, the other two will be set off.
   * Remember that when the state changes, a ModifiedEvent is invoked.
   * This can be used to snap the frustum to the axes if it is originally
   * not aligned.
   * Default to false.
   */
  void SetAlongXAxis(bool);
  vtkGetMacro(AlongXAxis, bool);
  vtkBooleanMacro(AlongXAxis, bool);
  void SetAlongYAxis(bool);
  vtkGetMacro(AlongYAxis, bool);
  vtkBooleanMacro(AlongYAxis, bool);
  void SetAlongZAxis(bool);
  vtkGetMacro(AlongZAxis, bool);
  vtkBooleanMacro(AlongZAxis, bool);
  ///@}

  ///@{
  /**
   * Enable/disable the drawing of the frustum. In some cases the frustum
   * interferes with the object that it is operating on (e.g., the
   * frustum interferes with the cut surface it produces resulting in
   * z-buffer artifacts.) By default it is off.
   */
  void SetDrawFrustum(bool draw);
  vtkGetMacro(DrawFrustum, bool);
  vtkBooleanMacro(DrawFrustum, bool);
  ///@}

  ///@{
  /**
   * Set/Get the resolution of the frustum. This is the number of
   * polygonal facets used to approximate the
   * surface (for rendering purposes). A TODO is used under
   * the hood to provide an exact surface representation.
   * Defaults to 128.
   */
  vtkSetClampMacro(Resolution, int, 8, VTK_MAX_FRUSTUM_RESOLUTION);
  vtkGetMacro(Resolution, int);
  ///@}

  ///@{
  /**
   * Turn on/off tubing of the wire outline of the frustum
   * intersection (against the bounding box). The tube thickens the
   * line by wrapping with a vtkTubeFilter.
   * Defaults to true.
   */
  vtkSetMacro(Tubing, bool);
  vtkGetMacro(Tubing, bool);
  vtkBooleanMacro(Tubing, bool);
  ///@}

  ///@{
  /**
   * Turn on/off the ability to translate the bounding box by moving it
   * with the mouse.
   * Defaults to true.
   */
  vtkSetMacro(OutlineTranslation, bool);
  vtkGetMacro(OutlineTranslation, bool);
  vtkBooleanMacro(OutlineTranslation, bool);
  ///@}

  ///@{
  /**
   * Turn on/off the ability to move the widget outside of the bounds
   * specified in the PlaceWidget() invocation.
   * Defaults to true.
   */
  vtkSetMacro(OutsideBounds, bool);
  vtkGetMacro(OutsideBounds, bool);
  vtkBooleanMacro(OutsideBounds, bool);
  ///@}

  ///@{
  /**
   * Set/Get the bounds of the widget representation. PlaceWidget can also be
   * used to set the bounds of the widget but it may also have other effects
   * on the internal state of the representation. Use this function when only
   * the widget bounds need to be modified.
   */
  vtkSetVector6Macro(WidgetBounds, double);
  double* GetWidgetBounds();
  ///@}

  ///@{
  /**
   * Turn on/off whether the frustum should be constrained to the widget bounds.
   * If on, the origin will not be allowed to move outside the set widget bounds.
   * This is the default behaviour.
   * If off, the origin can be freely moved. The widget outline will change accordingly.
   * Defaults to true.
   */
  vtkSetMacro(ConstrainToWidgetBounds, bool);
  vtkGetMacro(ConstrainToWidgetBounds, bool);
  vtkBooleanMacro(ConstrainToWidgetBounds, bool);
  ///@}

  ///@{
  /**
   * Turn on/off the ability to scale the widget with the mouse.
   * Defaults to true.
   */
  vtkSetMacro(ScaleEnabled, bool);
  vtkGetMacro(ScaleEnabled, bool);
  vtkBooleanMacro(ScaleEnabled, bool);
  ///@}

  /**
   * Grab the polydata that defines the frustum. The polydata contains
   * polygons that are clipped by the bounding box.
   */
  void GetPolyData(vtkPolyData* pd);

  /**
   * Satisfies the superclass API.  This will change the state of the widget
   * to match changes that have been made to the underlying PolyDataSource.
   */
  void UpdatePlacement();

  ///@{
  /**
   * Get the properties on the axis (line and frustum).
   */
  vtkGetObjectMacro(AxisProperty, vtkProperty);
  vtkGetObjectMacro(SelectedAxisProperty, vtkProperty);
  ///@}

  ///@{
  /**
   * Get the frustum properties. The properties of the frustum when selected
   * and unselected can be manipulated.
   */
  vtkGetObjectMacro(FrustumProperty, vtkProperty);
  vtkGetObjectMacro(SelectedFrustumProperty, vtkProperty);
  ///@}

  ///@{
  /**
   * Get the frustum radii properties. The properties of the frustum inner and outer radii when
   * selected and unselected can be manipulated.
   */
  vtkGetObjectMacro(EdgeHandleProperty, vtkProperty);
  vtkGetObjectMacro(SelectedEdgeHandleProperty, vtkProperty);
  ///@}

  ///@{
  /**
   * Get the property of the outline.
   */
  vtkGetObjectMacro(OutlineProperty, vtkProperty);
  vtkGetObjectMacro(SelectedOutlineProperty, vtkProperty);
  ///@}

  ///@{
  /**
   * Set the color of all the widgets handles (edges, axis, selected frustum)
   * and their color during interaction. Foreground color applies to the outlines and unselected
   * frustum.
   */
  void SetInteractionColor(double, double, double);
  void SetInteractionColor(double c[3]) { this->SetInteractionColor(c[0], c[1], c[2]); }
  void SetHandleColor(double, double, double);
  void SetHandleColor(double c[3]) { this->SetHandleColor(c[0], c[1], c[2]); }
  void SetForegroundColor(double, double, double);
  void SetForegroundColor(double c[3]) { this->SetForegroundColor(c[0], c[1], c[2]); }
  ///@}

  ///@{
  /**
   * Methods to interface with the vtkImplicitFrustumWidget.
   */
  int ComputeInteractionState(int X, int Y, int modify = 0) override;
  void PlaceWidget(double bounds[6]) override;
  void BuildRepresentation() override;
  void StartWidgetInteraction(double eventPos[2]) override;
  void WidgetInteraction(double newEventPos[2]) override;
  void EndWidgetInteraction(double newEventPos[2]) override;
  ///@}

  ///@{
  /**
   * Methods supporting the rendering process.
   */
  double* GetBounds() override;
  void GetActors(vtkPropCollection* pc) override;
  void ReleaseGraphicsResources(vtkWindow*) override;
  int RenderOpaqueGeometry(vtkViewport*) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport*) override;
  vtkTypeBool HasTranslucentPolygonalGeometry() override;
  ///@}

  ///@{
  /**
   * Specify a translation distance used by the BumpFrustum() method. Note that the
   * distance is normalized; it is the fraction of the length of the bounding
   * box of the wire outline.
   * Defaults to 0.01.
   */
  vtkSetClampMacro(BumpDistance, double, 0.000001, 1);
  vtkGetMacro(BumpDistance, double);
  ///@}

  /**
   * Translate the frustum in the direction of the view vector by the
   * specified BumpDistance. The dir parameter controls which
   * direction the pushing occurs, either in the same direction as the
   * view vector, or when negative, in the opposite direction. The factor
   * controls what percentage of the bump is used.
   */
  void BumpFrustum(int dir, double factor);

  /**
   * Push the frustum the distance specified along the view
   * vector. Positive values are in the direction of the view vector;
   * negative values are in the opposite direction. The distance value
   * is expressed in world coordinates.
   */
  void PushFrustum(double distance);

  /**
   * The interaction state may be set from a widget (e.g.,
   * vtkImplicitFrustumWidget) or other object. This controls how the
   * interaction with the widget proceeds. Normally this method is used as
   * part of a handshaking process with the widget: First
   * ComputeInteractionState() is invoked that returns a state based on
   * geometric considerations (i.e., cursor near a widget feature), then
   * based on events, the widget may modify this further.
   */
  vtkSetClampMacro(InteractionState, InteractionStateType, InteractionStateType::Outside,
    InteractionStateType::TranslatingOrigin);

  ///@{
  /**
   * Sets the visual appearance of the representation based on the
   * state it is in. This state is usually the same as InteractionState.
   */
  virtual void SetRepresentationState(InteractionStateType);
  vtkGetMacro(RepresentationState, InteractionStateType);
  ///@}

  /*
   * Register internal Pickers within PickingManager
   */
  void RegisterPickers() override;

  ///@{
  /**
   * Gets/Sets the constraint axis for translations.
   * Defaults to Axis::NONE
   **/
  vtkGetMacro(TranslationAxis, int);
  vtkSetClampMacro(TranslationAxis, int, Axis::NONE, Axis::ZAxis);
  ///@}

  ///@{
  /**
   * Toggles constraint translation axis on/off.
   */
  void SetXTranslationAxisOn() { this->TranslationAxis = Axis::XAxis; }
  void SetYTranslationAxisOn() { this->TranslationAxis = Axis::YAxis; }
  void SetZTranslationAxisOn() { this->TranslationAxis = Axis::ZAxis; }
  void SetTranslationAxisOff() { this->TranslationAxis = Axis::NONE; }
  ///@}

  /**
   * Returns true if ConstrainedAxis
   **/
  bool IsTranslationConstrained() { return this->TranslationAxis != Axis::NONE; }

  void GetFrustum(vtkFrustum* frustum) const;

protected:
  vtkImplicitFrustumRepresentation();
  ~vtkImplicitFrustumRepresentation() override;

private:
  enum class FrustumFace : int
  {
    Near = 0,
    Bottom,
    Right,
    Top,
    Left,
  };

  struct ArrowHandle
  {
    vtkNew<vtkLineSource> LineSource;
    vtkNew<vtkPolyDataMapper> LineMapper;
    vtkNew<vtkActor> LineActor;

    vtkNew<vtkConeSource> HeadSource;
    vtkNew<vtkPolyDataMapper> HeadMapper;
    vtkNew<vtkActor> HeadActor;

    ArrowHandle();
  };

  struct SphereHandle
  {
    vtkNew<vtkSphereSource> Source;
    vtkNew<vtkPolyDataMapper> Mapper;
    vtkNew<vtkActor> Actor;

    SphereHandle();
  };

  struct EdgeHandle
  {
    vtkNew<vtkPolyData> PolyData;
    vtkNew<vtkTubeFilter> Tuber;
    vtkNew<vtkPolyDataMapper> Mapper;
    vtkNew<vtkActor> Actor;

    EdgeHandle();
  };

  struct EllipseHandle
  {
    vtkNew<vtkEllipseArcSource> Source;
    vtkNew<vtkTubeFilter> Tuber;
    vtkNew<vtkPolyDataMapper> Mapper;
    vtkNew<vtkActor> Actor;

    EllipseHandle();
  };

  vtkImplicitFrustumRepresentation(const vtkImplicitFrustumRepresentation&) = delete;
  void operator=(const vtkImplicitFrustumRepresentation&) = delete;

  void HighlightFrustum(bool highlight);
  void HighlightOriginHandle(bool highlight);
  void HighlightAxis(bool highlight);
  void HighlightOutline(bool highlight);
  void HighlightFarPlaneVerticalHandle(bool highlight);
  void HighlightFarPlaneHorizontalHandle(bool highlight);
  void HighlighNearPlaneHandle(bool highlight);

  // Methods to manipulate the frustum
  void Rotate(
    double X, double Y, const vtkVector3d& p1, const vtkVector3d& p2, const vtkVector3d& vpn);
  void TranslateFrustum(const vtkVector3d& p1, const vtkVector3d& p2);
  void TranslateOutline(const vtkVector3d& p1, const vtkVector3d& p2);
  void TranslateOrigin(const vtkVector3d& p1, const vtkVector3d& p2);
  void TranslateOriginOnAxis(const vtkVector3d& p1, const vtkVector3d& p2);
  void AdjustHorizontalAngle(
    const vtkVector2d& eventPosition, const vtkVector3d& p1, const vtkVector3d& p2);
  void AdjustVerticalAngle(
    const vtkVector2d& eventPosition, const vtkVector3d& p1, const vtkVector3d& p2);
  void AdjustNearPlaneDistance(
    const vtkVector2d& eventPosition, const vtkVector3d& p1, const vtkVector3d& p2);
  void AdjustYaw(const vtkVector3d& prevPickPoint, const vtkVector3d& pickPoint);
  void AdjustPitch(const vtkVector3d& prevPickPoint, const vtkVector3d& pickPoint);
  void AdjustRoll(const vtkVector3d& prevPickPoint, const vtkVector3d& pickPoint);

  void Scale(const vtkVector3d& p1, const vtkVector3d& p2, double X, double Y);

  void SizeHandles();

  // Generate the frustum polydata, cropped by the bounding box
  void BuildFrustum();

  void UpdateFrustumTransform();

  // The actual frustum we're manipulating
  vtkNew<vtkFrustum> Frustum;

  InteractionStateType RepresentationState = InteractionStateType::Outside;
  int TranslationAxis = Axis::NONE;

  // Keep track of event positions
  vtkVector3d LastEventPosition = { 0., 0., 0. };

  // Controlling the push operation
  double BumpDistance = 0.01;

  // Controlling ivars
  bool AlongXAxis = false;
  bool AlongYAxis = false;
  bool AlongZAxis = false;

  // The facet resolution for rendering purposes.
  int Resolution = 128;

  // The bounding box is represented by a single voxel image data
  vtkNew<vtkImageData> Box;
  vtkNew<vtkOutlineFilter> Outline;
  vtkNew<vtkPolyDataMapper> OutlineMapper;
  vtkNew<vtkActor> OutlineActor;
  bool OutlineTranslation = true; // whether the outline can be moved
  bool ScaleEnabled = true;       // whether the widget can be scaled
  bool OutsideBounds = true;      // whether the widget can be moved outside input's bounds
  vtkVector<double, 6> WidgetBounds;
  bool ConstrainToWidgetBounds = true;

  vtkNew<vtkPolyData> FrustumPD;
  vtkNew<vtkPolyDataMapper> FrustumMapper;
  vtkNew<vtkActor> FrustumActor;
  bool DrawFrustum = true;

  vtkVector3d UpAxis = { 0, 0, -1 };
  vtkNew<vtkOrientationRepresentation> Orientation;

  // Optional tubes are represented by extracting boundary edges and tubing
  EdgeHandle FarPlaneVerticalHandle;
  EdgeHandle FarPlaneHorizontalHandle;
  EdgeHandle NearPlaneEdgesHandle;
  SphereHandle NearPlaneCenterHandle;
  bool Tubing = true; // control whether tubing is on

  ArrowHandle ViewUpHandle;

  // Axis representation
  ArrowHandle AxisHandle;
  EllipseHandle RollHandle;

  // Origin positioning handle
  SphereHandle OriginHandle;

  // Do the picking
  vtkNew<vtkCellPicker> Picker;
  vtkNew<vtkCellPicker> FrustumPicker;

  // Properties used to control the appearance of selected objects and
  // the manipulator in general.
  vtkNew<vtkProperty> AxisProperty;
  vtkNew<vtkProperty> SelectedAxisProperty;
  vtkNew<vtkProperty> FrustumProperty;
  vtkNew<vtkProperty> SelectedFrustumProperty;
  vtkNew<vtkProperty> OutlineProperty;
  vtkNew<vtkProperty> SelectedOutlineProperty;
  vtkNew<vtkProperty> EdgeHandleProperty;
  vtkNew<vtkProperty> SelectedEdgeHandleProperty;
  vtkNew<vtkProperty> OriginHandleProperty;
  vtkNew<vtkProperty> SelectedOriginHandleProperty;

  vtkNew<vtkBox> BoundingBox;
};

VTK_ABI_NAMESPACE_END
#endif
