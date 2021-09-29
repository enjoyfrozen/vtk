/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPStreamTracer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkPStreamTracer
 * @brief    parallel streamline generators
 *
 * This class implements parallel streamline generators. By default this filter
 * will aggregate seed sources from all ranks into a single dataset. For performance
 * issue it is still possible to force the filter to use the rank-local seed source
 * and avoid the aggregation.
 * @sa
 * vtkStreamTracer
 */

#ifndef vtkPStreamTracer_h
#define vtkPStreamTracer_h

#include "vtkSmartPointer.h" // This is a leaf node. No need to use PIMPL to avoid compile time penalty.
#include "vtkStreamTracer.h"

class vtkAbstractInterpolatedVelocityField;
class vtkMultiProcessController;

class PStreamTracerPoint;
class vtkOverlappingAMR;
class AbstractPStreamTracerUtils;

#include "vtkFiltersParallelFlowPathsModule.h" // For export macro

class VTKFILTERSPARALLELFLOWPATHS_EXPORT vtkPStreamTracer : public vtkStreamTracer
{
public:
  vtkTypeMacro(vtkPStreamTracer, vtkStreamTracer);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/Get the controller use in compositing (set to the global controller
   * by default) If not using the default, this must be called before any
   * other methods.
   */
  virtual void SetController(vtkMultiProcessController* controller);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  ///@}

  ///@{
  /**
   * If true then the filter consider that the whole seed source is available on all ranks.
   * Else the filter will aggregate all seed sources from all ranks and merge their points.
   *
   * Default is false.
   */
  vtkSetMacro(UseLocalSeedSource, bool);
  vtkGetMacro(UseLocalSeedSource, bool);
  vtkBooleanMacro(UseLocalSeedSource, bool);
  ///@}

  static vtkPStreamTracer* New();

protected:
  vtkPStreamTracer();
  ~vtkPStreamTracer() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  vtkMultiProcessController* Controller;

  vtkAbstractInterpolatedVelocityField* Interpolator;
  void SetInterpolator(vtkAbstractInterpolatedVelocityField*);

  int EmptyData;
  bool UseLocalSeedSource = false;

private:
  vtkPStreamTracer(const vtkPStreamTracer&) = delete;
  void operator=(const vtkPStreamTracer&) = delete;

  void Trace(vtkDataSet* input, int vecType, const char* vecName, PStreamTracerPoint* pt,
    vtkSmartPointer<vtkPolyData>& output, vtkAbstractInterpolatedVelocityField* func,
    int maxCellSize);

  bool TraceOneStep(
    vtkPolyData* traceOut, vtkAbstractInterpolatedVelocityField*, PStreamTracerPoint* pt);

  void Prepend(vtkPolyData* path, vtkPolyData* headh);
  int Rank;
  int NumProcs;

  friend class AbstractPStreamTracerUtils;
  vtkSmartPointer<AbstractPStreamTracerUtils> Utils;
};
#endif
