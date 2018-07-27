/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMassFlowFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkMassFlowFilter
 * @brief   perform mass flow calculation over a surface
 *
 * vtkMassFlowFilter is a convenience filter that calculates integrated mass
 * flow across a selected group of points or cells. The filter takes two
 * inputs: in the first input any vtkPolyData; on the second an (optional)
 * selection. The selection is described by the contents of the
 * vtkSelection. The output of the filter are the selected portion of the
 * dataset along with a new array, the calculated mass flow (at each
 * point). Additionally, the summed mass flow is produced in the
 * TotalMassFlow instance variable. Note: if the second selection input is
 * not provided, then the entire input vtkPolyData is used to compute mass flow.
 *
 * Note that the internal filter vtkExtractSelection is used when a selection
 * is provided as the optional second input. A method is available to access
 * this internal filter in case modifications to its execution behavior are
 * required.
 *
 * @warning

 * The mass flow is computed by multiplying the density array times the
 * velocity field, dotted by a direction vector (to obtain mass flow in the
 * specified direction). The user has the option to specify a direction, or
 * use the input vtkPolyData surface normals. If no normals are provided, then
 * they are calculated automatically.
 *
 * @warning
 * The input density scalars and velocity vectors are assumed to be real type
 * (float or double). The output mass flow scalars are of double type.
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.
 *
 * @sa
 * vtkSelection vtkSelector vtkSelectionNode vtkExtractSelection
 * vtkArrayCalculator
*/

#ifndef vtkMassFlowFilter_h
#define vtkMassFlowFilter_h

#include "vtkFiltersExtractionModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

#include "vtkSelectionNode.h" // for vtkSelectionNode::SelectionContent
#include "vtkSmartPointer.h"  // for smart pointer

#define VTK_SPECIFIED_DIRECTION 0
#define VTK_SURFACE_NORMALS 1

class vtkSelection;
class vtkSelectionNode;
class vtkSelector;
class vtkExtractSelection;
class vtkPolyDataNormals;


class VTKFILTERSEXTRACTION_EXPORT vtkMassFlowFilter : public vtkPolyDataAlgorithm
{
public:
  //@{
  /**
   * Standard methods for instantiation, type information, and printing.
   */
  static vtkMassFlowFilter *New();
  vtkTypeMacro(vtkMassFlowFilter, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  /**
   * Method to specify the selection connection (2nd input port)
   */
  void SetSelectionConnection(vtkAlgorithmOutput* algOutput)
  {
    this->SetInputConnection(1, algOutput);
  }

  //@{
  /**
   * Method to specify the selection data.
   */
  void SetSelectionData(vtkSelection *selection);
  vtkSelection *GetSelection();
  //@}

  //@{
  /**
   * Specify the name of the input scalar density field.
   */
  vtkSetStringMacro(DensityArrayName);
  vtkGetStringMacro(DensityArrayName);
  //@}

  //@{
  /**
   * Specify the name of the input vector density field.
   */
  vtkSetStringMacro(VelocityArrayName);
  vtkGetStringMacro(VelocityArrayName);
  //@}

  //@{
  /**
   * Specify the name of the input surface normals array. If not specified,
   * the filter will use the normals provided on input.
   */
  vtkSetStringMacro(NormalsArrayName);
  vtkGetStringMacro(NormalsArrayName);
  //@}

  //@{
  /**
   * Specify the name of the output mass flow field.
   */
  vtkSetStringMacro(MassFlowArrayName);
  vtkGetStringMacro(MassFlowArrayName);
  //@}

  //@{
  /**
   * Specify which normals (directions) to use when computing mass
   * flow. Either a specified direction can be used, or surface normals. Note
   * that if surface normals are requested, but not available on input, then
   * they are automatically computed.
   */
  vtkSetMacro(DirectionMode,int);
  vtkGetMacro(DirectionMode,int);
  void SetDirectionModeToSpecifiedDirection()
    {this->SetDirectionMode(VTK_SPECIFIED_DIRECTION);};
  void SetDirectionModeToSurfaceNormals()
    {this->SetDirectionMode(VTK_SURFACE_NORMALS);};
  const char *GetDirectionModeAsString();
  //@}

  //@{
  /**
   * Specify the direction in which the mass flow is to be calculated. This
   * is only used if the DirectionMode is set to SpecifiedDirection.
   */
  vtkSetVector3Macro(Direction,double);
  vtkGetVectorMacro(Direction,double,3);
  //@}

  /**
   * Retrieve the summed mass flow. Note that this value is valid only
   * after successful execution of the filter.
   */
  double GetTotalMassFlow()
  {return this->TotalMassFlow;}

  /**
   * Retrieve the internal vtkExtractSelection filter.
   */
  vtkExtractSelection *GetExtractSelectionFilter();

  /**
   * Retrieve the internal vtkPolyDataNormals filter.
   */
  vtkPolyDataNormals *GetPolyDataNormalsFilter();

  /**
   * The modified time also depends on the internal extract selection filter.
   */
  vtkMTimeType GetMTime() override;

protected:
  vtkMassFlowFilter();
  ~vtkMassFlowFilter() override;

  // Various arrays needed to compute mass flow
  char* DensityArrayName;
  char* VelocityArrayName;
  char* NormalsArrayName;
  char* MassFlowArrayName;

  // Control direction vector
  int DirectionMode;
  double Direction[3];

  // Internal helper filters
  vtkExtractSelection *Extract;
  vtkPolyDataNormals *Normals;

  // The computed mass flow
  double TotalMassFlow;

  /**
   * Sets up empty output dataset
   */
  int RequestData(vtkInformation* request,
                        vtkInformationVector** inputVector,
                        vtkInformationVector* outputVector) override;

  int FillInputPortInformation(int port, vtkInformation* info) override;

private:
  vtkMassFlowFilter(const vtkMassFlowFilter&) = delete;
  void operator=(const vtkMassFlowFilter&) = delete;

};

//@{
/**
 * Return the method of determining the direction vector.
 */
inline const char *vtkMassFlowFilter::GetDirectionModeAsString(void)
{
  if ( this->DirectionMode == VTK_SPECIFIED_DIRECTION )
  {
    return "Specified Direction";
  }
  else
  {
    return "Surface Normals";
  }
}
//@}

#endif
