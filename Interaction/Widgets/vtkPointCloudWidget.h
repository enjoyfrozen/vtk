/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPointCloudWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPointCloudWidget
 * @brief   query, select, and manipulate a point from a point cloud
 *
 * This 3D widget allows the user to query, select, and optionally manipulate
 * one point from a large collection of points. Typically it is paired with
 * an additional widget to provide local point editing (e.g., move a point,
 * modify associated attribute values).
 *
 * @sa
 * vtkPointWidget vtkTensorSizingWidget
 */

#ifndef vtkPointCloudWidget_h
#define vtkPointCloudWidget_h

#include "vtkAbstractWidget.h"
#include "vtkInteractionWidgetsModule.h" // For export macro

class vtkPointCloudRepresentation;


class VTKINTERACTIONWIDGETS_EXPORT vtkPointCloudWidget : public vtkAbstractWidget
{
public:
  //@{
  /**
   * Standard methods for instantiation, type information, and printing.
   */
  static vtkPointCloudWidget* New();
  vtkTypeMacro(vtkPointCloudWidget, vtk3DWidget);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  /**
   * Specify an instance of vtkWidgetRepresentation used to represent this
   * widget in the scene. Note that the representation is a subclass of vtkProp
   * so it can be added to the renderer independent of the widget.
   */
  void SetRepresentation(vtkPointCloudRepresentation* r)
  {
    this->Superclass::SetWidgetRepresentation(reinterpret_cast<vtkWidgetRepresentation*>(r));
  }

  /**
   * Create the default widget representation if one is not set. By default,
   * this is an instance of the vtkPointCloudRepresentation class.
   */
  void CreateDefaultRepresentation() override;

  /**
   * Override superclasses' SetEnabled() method because the line
   * widget must enable its internal handle widgets.
   */
  void SetEnabled(int enabling) override;

protected:
  vtkPointCloudWidget();
  ~vtkPointCloudWidget() override;

  int WidgetState;
  enum _WidgetState
  {
    Start = 0,
    Active
  };

  // These methods handle events
  static void MoveAction(vtkAbstractWidget*);
  static void SelectAction(vtkAbstractWidget*);
  static void QueryAction(vtkAbstractWidget*);
  static void EndSelectAction(vtkAbstractWidget*);

private:
  vtkPointCloudWidget(const vtkPointCloudWidget&) = delete;
  void operator=(const vtkPointCloudWidget&) = delete;
};

#endif
