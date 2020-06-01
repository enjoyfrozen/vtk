/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageProbeFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkImageProbeFilter
 * @brief   sample image values at specified point locations
 *
 * vtkImageProbeFilter is a filter that interpolates point attributes (e.g.,
 * scalars, vectors, etc.) at specified point positions within an image.
 * This filter has two inputs: the Input and Source. The Input geometric
 * structure is passed through the filter, and the Output point attributes
 * are interpolated from the Source point attributes (where the Source is
 * a vtkImageData).
 *
 * This filter can be used to resample an image onto a set of arbitrarily
 * placed sample points.  For example, if you have a surface data set
 * (i.e. a vtkPolyData that has been tesselated so that its points are
 * very closely spaced), you can color the polydata from the image points.
 *
 * In general, this filter is similar to vtkProbeFilter except that the
 * Source data is always an image.  The advantages that it provides over
 * vtkProbeFilter is that it is faster, and it can take advantage of
 * advanced image interpolation techniques.
 */

#ifndef vtkImageProbeFilter_h
#define vtkImageProbeFilter_h

#include "vtkDataSetAlgorithm.h"
#include "vtkDataSetAttributes.h" // For vtkDataSetAttributes::FieldList
#include "vtkImagingCoreModule.h" // For export macro

class vtkCharArray;
class vtkIdTypeArray;
class vtkImageData;
class vtkPointData;

class VTKIMAGINGCORE_EXPORT vtkImageProbeFilter : public vtkDataSetAlgorithm
{
public:
  static vtkImageProbeFilter* New();
  vtkTypeMacro(vtkImageProbeFilter, vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify the data set that will be probed at the input points.
   * The Input gives the geometry (the points and cells) for the output,
   * while the Source is probed (interpolated) to generate the scalars,
   * vectors, etc. for the output points based on the point locations.
   */
  void SetSourceData(vtkDataObject* source);
  vtkDataObject* GetSource();
  //@}

  /**
   * Specify the data set that will be probed at the input points.
   * The Input gives the geometry (the points and cells) for the output,
   * while the Source is probed (interpolated) to generate the scalars,
   * vectors, etc. for the output points based on the point locations.
   */
  void SetSourceConnection(vtkAlgorithmOutput* algOutput);

  //@{
  /**
   * Get the list of point ids in the output that contain attribute data
   * interpolated from the source.
   */
  vtkIdTypeArray* GetValidPoints();
  //@}

  //@{
  /**
   * Returns the name of the char array added to the output with values 1 for
   * valid points and 0 for invalid points.
   * Set to "vtkValidPointMask" by default.
   */
  vtkSetStringMacro(ValidPointMaskArrayName);
  vtkGetStringMacro(ValidPointMaskArrayName);
  //@}

  //@{
  /**
   * Shallow copy the input cell data arrays to the output.
   * Off by default.
   */
  vtkSetMacro(PassCellArrays, vtkTypeBool);
  vtkBooleanMacro(PassCellArrays, vtkTypeBool);
  vtkGetMacro(PassCellArrays, vtkTypeBool);
  //@}

  //@{
  /**
   * Shallow copy the input point data arrays to the output
   * Off by default.
   */
  vtkSetMacro(PassPointArrays, vtkTypeBool);
  vtkBooleanMacro(PassPointArrays, vtkTypeBool);
  vtkGetMacro(PassPointArrays, vtkTypeBool);
  //@}

  //@{
  /**
   * Set whether to pass the field-data arrays from the Input i.e. the input
   * providing the geometry to the output. On by default.
   */
  vtkSetMacro(PassFieldArrays, vtkTypeBool);
  vtkBooleanMacro(PassFieldArrays, vtkTypeBool);
  vtkGetMacro(PassFieldArrays, vtkTypeBool);
  //@}

  //@{
  /**
   * Set the tolerance used to compute whether a point in the
   * source is in a cell of the input.  This value is only used
   * if ComputeTolerance is off.
   */
  vtkSetMacro(Tolerance, double);
  vtkGetMacro(Tolerance, double);
  //@}

  //@{
  /**
   * Set whether to use the Tolerance field or precompute the tolerance.
   * When on, the tolerance will be computed and the field
   * value is ignored. On by default.
   */
  vtkSetMacro(ComputeTolerance, bool);
  vtkBooleanMacro(ComputeTolerance, bool);
  vtkGetMacro(ComputeTolerance, bool);
  //@}

protected:
  vtkImageProbeFilter();
  ~vtkImageProbeFilter() override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestUpdateExtent(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  /**
   * Call at end of RequestData() to pass attribute data respecting the
   * PassCellArrays, PassPointArrays, PassFieldArrays flags.
   */
  void PassAttributeData(vtkDataSet* input, vtkDataSet* output);

  /**
   * Equivalent to calling BuildFieldList(); InitializeForProbing(); DoProbing().
   */
  void Probe(vtkDataSet* input, vtkImageData* source, vtkDataSet* output);

  /**
   * Build the field lists. Call this before calling InitializeForProbing().
   */
  void BuildFieldList(vtkImageData* source);

  /**
   * Initialize various arrays which keep track of probing status.
   */
  virtual void InitializeForProbing(vtkDataSet* input, vtkDataSet* output);

  /**
   * Initialize all point attributes to zero.
   */
  virtual void InitializeOutputArrays(vtkPointData* outPD, vtkIdType numPts);

  /**
   * Probe appropriate points
   * srcIdx is the index in the PointList for the given source.
   */
  void DoProbing(vtkDataSet* input, int srcIdx, vtkImageData* source, vtkDataSet* output);

  vtkTypeBool PassCellArrays;
  vtkTypeBool PassPointArrays;
  vtkTypeBool PassFieldArrays;

  double Tolerance;
  bool ComputeTolerance;

  char* ValidPointMaskArrayName;
  vtkIdTypeArray* ValidPoints;
  vtkCharArray* MaskPoints;

  vtkDataSetAttributes::FieldList* CellList;
  vtkDataSetAttributes::FieldList* PointList;

private:
  vtkImageProbeFilter(const vtkImageProbeFilter&) = delete;
  void operator=(const vtkImageProbeFilter&) = delete;

  // Probe only those points that are marked as not-probed by the MaskPoints
  // array.
  void ProbePoints(vtkDataSet* input, vtkImageData* source, int srcIdx, vtkPointData* outPD,
    char* maskArray, vtkIdList* pointIds, vtkIdType startId, vtkIdType endId, bool baseThread);

  class ProbePointsWorklet;

  class vtkVectorOfArrays;
  vtkVectorOfArrays* CellArrays;
};

#endif
