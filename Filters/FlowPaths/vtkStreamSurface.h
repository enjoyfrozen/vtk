/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkStreamSurface.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

   This software is distributed WITHOUT ANY WARRANTY; without even
   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
   PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkStreamSurface
 * @brief   advect a stream surface in a vector field
 *
 * depending on UseIterativeSeeding, the simple or iterative version is called
 * the iterative version produces the good surfaces, but takes longer
 *
 * @par Thanks:
 * Developed by Roxana Bujack and Karen Tsai at Los Alamos National Laboratory under LDRD 20190143ER
 */
#ifndef vtkStreamSurface_h
#define vtkStreamSurface_h

#include <vtkFiltersFlowPathsModule.h> // For export macro
#include <vtkImageData.h>
#include <vtkPolyDataAlgorithm.h>
#include <vtkStreamTracer.h>

class VTKFILTERSFLOWPATHS_EXPORT vtkStreamSurface : public vtkStreamTracer
{
public:
  static vtkStreamSurface* New();
  vtkTypeMacro(vtkStreamSurface, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetMacro(UseIterativeSeeding, bool);
  vtkGetMacro(UseIterativeSeeding, bool);

protected:
  vtkStreamSurface();
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkStreamSurface(const vtkStreamSurface&) = delete;
  void operator=(const vtkStreamSurface&) = delete;

  /**
   * first advect all point in seeds and then connect the resulting streamlines to a surface
   * field is the vector values dataset in which the streamsurface is advected
   * seeds is the polydata with the start curve
   * output is the final streamsurface
   */
  void AdvectSimple(vtkImageData* field, vtkPolyData* seeds, vtkPolyData* output);

  /**
   * loop: 1.advect one step at a time
   * 2. then form surface strip and add it to existing surface
   * 3. then check if points have diverged and insert new ones if necessary
   * field is the vecotr values dataset in which the streamsurface is advected
   * seeds is the polydata with the start curve
   * output is the final streamsurface
   * @param field: vector field in which the surfave is advected
   * @param seeds: initial values
   * @param output: the final surface
   */
  void AdvectIterative(vtkImageData* field, vtkPolyData* seeds, vtkPolyData* output);

  /**
   * depending on this boolen the simple or iterative version is called
   */
  bool UseIterativeSeeding;
};
#endif
