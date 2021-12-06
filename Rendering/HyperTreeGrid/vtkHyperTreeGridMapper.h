/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperTreeGridMapper.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkHyperTreeGridMapper
 * @brief   map vtkHyperTreeGrid to graphics primitives

 * vtkHyperTreeGridMapper is a class that maps polygonal data (i.e., vtkHyperTreeGrid)
 * to graphics primitives. vtkHyperTreeGridMapper serves as a superclass for
 * device-specific poly data mappers, that actually do the mapping to the
 * rendering/graphics hardware/software.

 * By default, this class use an Adaptive GeometryFilter that extract only
 * the part of the geometry to render. Be careful as this implies that new
 * render my trigger an update of the pipeline to get the new part of the
 * geometry to render.

 * Note: this class has its own module to avoid cyclic dependency between Rendering Core
 * and Filters Hybrid
 * * It need Filters Hybrid for Adaptive2DGeometryFilter
 * * Filters Hybrid need Rendering Core because of Adaptive2DGeometryFilter
 */

#ifndef vtkHyperTreeGridMapper_h
#define vtkHyperTreeGridMapper_h

#include "vtkMapper.h"
#include "vtkSetGet.h"       // Get macro
#include "vtkSmartPointer.h" // For vtkSmartPointer

#include "vtkRenderingHyperTreeGridModule.h" // For export macro

class vtkAdaptiveDataSetSurfaceFilter;
class vtkHyperTreeGrid;
class vtkHyperTreeGridGeometry;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkRenderWindow;
class vtkRenderer;

class VTKRENDERINGHYPERTREEGRID_EXPORT vtkHyperTreeGridMapper : public vtkMapper
{
public:
  static vtkHyperTreeGridMapper* New();
  vtkTypeMacro(vtkHyperTreeGridMapper, vtkMapper);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set the connection for the given input port index.  Each input
   * port of a filter has a specific purpose.  A port may have zero or
   * more connections and the required number is specified by each
   * filter.  Setting the connection with this method removes all
   * other connections from the port.  To add more than one connection
   * use AddInputConnection().

   * The input for the connection is the output port of another
   * filter, which is obtained with GetOutputPort().  Typical usage is

   * filter2->SetInputConnection(0, filter1->GetOutputPort(0)).

   * \link vtkAlgorithm
   */
  using Superclass::SetInputConnection;
  void SetInputConnection(vtkAlgorithmOutput* input) override;
  void SetInputDataObject(int port, vtkDataObject* input) override;
  void SetInputDataObject(vtkDataObject* input) override;
  //@}

  //@{
  /**
   * For this mapper, the bounds correspond to the output for the
   * internal surface filter which may be restricted to the Camera frustum
   * if UseCameraFrustum is on.
   */
  double* GetBounds() override;
  void GetBounds(double bounds[6]) override;
  //@}

  //@{
  /**
   * This boolean control whether or not the mapping should adapt
   * to the Camera frustum during the rendering. Setting this variable
   * to true (default) should provide increased preformances.
   */
  vtkGetMacro(UseCameraFrustum, bool);
  vtkSetMacro(UseCameraFrustum, bool);
  vtkBooleanMacro(UseCameraFrustum, bool);
  //@}

  /**
   * Use the internal PolyData Mapper to do the rendering
   * of the HTG transformed by the current SurfaceFilter:
   * * Adaptive2DGeometryFilter if UseCameraFrustum
   * * GeometryFilter otherwise
   */
  void Render(vtkRenderer* ren, vtkActor* act) override;

  //@{
  /**
   * Bring this algorithm's outputs up-to-date.
   * \link vtkAlgorithm
   */
  using Superclass::Update;
  void Update(int port) override;
  //@}

  /**
   * Fill the input port information objects for this algorithm.  This
   * is invoked by the first call to GetInputPortInformation for each
   * port so subclasses can specify what they can handle.
   * \link vtkAlgorithm
   */
  int FillInputPortInformation(int port, vtkInformation* info) override;

  /**
   * Get the underlying suface filter, used to transfom the input HTG
   * to a PolyData that will be rendered using the PDMapper.
   */
  vtkAlgorithm* GetSurfaceFilter();

protected:
  vtkHyperTreeGridMapper();
  virtual ~vtkHyperTreeGridMapper();

  /**
   * The input exposed here is the output of the SurfaceFilter.
   * It is the piece of PolyData to map and render on the screen.
   */
  vtkPolyData* GetSurfaceFilterInput();

  // Generate the surface to render
  bool UseCameraFrustum = true;
  vtkNew<vtkHyperTreeGridGeometry> GeometryFilter;
  vtkNew<vtkAdaptiveDataSetSurfaceFilter> Adaptive2DGeometryFilter;

  // render the extracted surface, need to be created
  // in device specific subclass
  vtkSmartPointer<vtkPolyDataMapper> PDMapper;

private:
  vtkHyperTreeGridMapper(const vtkHyperTreeGridMapper&) = delete;
  void operator=(const vtkHyperTreeGridMapper&) = delete;
};

#endif
