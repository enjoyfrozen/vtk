/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAbstractSplineRepresentation.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkAbstractSplineRepresentation.h"

#include "vtkCellPicker.h"
#include "vtkDataArrayRange.h"
#include "vtkDoubleArray.h"
#include "vtkParametricFunctionSource.h"
#include "vtkParametricSpline.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
vtkAbstractSplineRepresentation::vtkAbstractSplineRepresentation()
{
  // Initialize pipeline configuration
  this->ParametricFunctionSource->SetScalarModeToNone();
  this->ParametricFunctionSource->GenerateTextureCoordinatesOff();
  this->ParametricFunctionSource->SetUResolution(this->Resolution);

  this->LineMapper->SetResolveCoincidentTopologyToPolygonOffset();
  this->LineActor->SetMapper(this->LineMapper);
  this->LineMapper->SetInputConnection(this->ParametricFunctionSource->GetOutputPort());
}

//------------------------------------------------------------------------------
vtkAbstractSplineRepresentation::~vtkAbstractSplineRepresentation()
{
  this->CleanRepresentation();
}

//------------------------------------------------------------------------------
void vtkAbstractSplineRepresentation::CleanRepresentation()
{
  this->SetParametricSpline(nullptr);
}

//------------------------------------------------------------------------------
void vtkAbstractSplineRepresentation::SetParametricSplineInternal(vtkParametricSpline* spline)
{
  if (this->ParametricSpline != spline)
  {
    // to avoid destructor recursion
    vtkParametricSpline* temp = this->ParametricSpline;
    this->ParametricSpline = spline;
    if (temp != nullptr)
    {
      temp->UnRegister(this);
    }
    if (this->ParametricSpline != nullptr)
    {
      this->ParametricSpline->Register(this);
      this->ParametricFunctionSource->SetParametricFunction(this->ParametricSpline);
    }
  }

  this->Modified();
}

//------------------------------------------------------------------------------
void vtkAbstractSplineRepresentation::SetParametricSpline(vtkParametricSpline* spline)
{
  this->SetParametricSplineInternal(spline);
}

//------------------------------------------------------------------------------
vtkDoubleArray* vtkAbstractSplineRepresentation::GetHandlePositions()
{
  return vtkArrayDownCast<vtkDoubleArray>(this->ParametricSpline->GetPoints()->GetData());
}

//------------------------------------------------------------------------------
void vtkAbstractSplineRepresentation::SetResolution(int resolution)
{
  if (this->Resolution == resolution || resolution < (this->NumberOfHandles - 1))
  {
    return;
  }

  this->Resolution = resolution;
  this->ParametricFunctionSource->SetUResolution(this->Resolution);
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkAbstractSplineRepresentation::GetPolyData(vtkPolyData* pd)
{
  if (!pd)
  {
    vtkErrorMacro(<< "ERROR: Invalid or nullptr polydata\n");
    return;
  }
  this->ParametricFunctionSource->Update();
  pd->ShallowCopy(this->ParametricFunctionSource->GetOutput());
}

//------------------------------------------------------------------------------
double vtkAbstractSplineRepresentation::GetSummedLength()
{
  vtkPoints* points = this->ParametricFunctionSource->GetOutput()->GetPoints();
  int npts = points->GetNumberOfPoints();

  if (npts < 2)
  {
    return 0.0;
  }

  double a[3];
  double b[3];
  double sum = 0.0;
  int i = 0;
  points->GetPoint(i, a);

  // check in which case we are: even or odd number of points
  // (else the last while loop iteration would go too far for an even number)
  int imax = (npts % 2 == 0) ? npts - 2 : npts - 1;

  while (i < imax)
  {
    points->GetPoint(i + 1, b);
    sum += sqrt(vtkMath::Distance2BetweenPoints(a, b));
    i = i + 2;
    points->GetPoint(i, a);
    sum = sum + sqrt(vtkMath::Distance2BetweenPoints(a, b));
  }

  if (npts % 2 == 0)
  {
    points->GetPoint(i + 1, b);
    sum += sqrt(vtkMath::Distance2BetweenPoints(a, b));
  }

  return sum;
}

//------------------------------------------------------------------------------
void vtkAbstractSplineRepresentation::SetNumberOfHandles(int npts)
{
  if (this->NumberOfHandles == npts)
  {
    return;
  }

  if (npts < 0)
  {
    vtkErrorMacro(<< "Cannot set a negative number of handles.");
    return;
  }

  vtkDebugMacro(<< "setting NumberOfHandles to " << npts);

  if (npts == 0)
  {
    this->ClearHandles();
    this->NumberOfHandles = 0;
    this->CleanRepresentation();
    return;
  }

  // Ensure no handle is highlighted
  this->HighlightHandle(nullptr);

  if (this->GetParametricSpline() && this->NumberOfHandles > 1)
  {
    this->ReconfigureHandles(npts);
  }
  else
  {
    // reallocate the handles
    this->CreateDefaultHandles(npts);
  }

  this->NumberOfHandles = npts;
}

//------------------------------------------------------------------------------
int vtkAbstractSplineRepresentation::InsertHandleOnLine(double* pos)
{
  if (this->NumberOfHandles < 2 || pos == nullptr)
  {
    return -1;
  }

  vtkIdType id = this->LinePicker->GetCellId();
  if (id == -1)
  {
    return -1;
  }

  vtkIdType pickedSubId = this->LinePicker->GetSubId();
  vtkNew<vtkPolyData> spline;
  this->GetPolyData(spline);
  vtkCell* polyline = spline->GetCell(0);

  //@{
  // unused returned values from EvaluatePosition below.
  double closest[3];
  double pcoords[3];
  double dist2;
  int npts = polyline->GetNumberOfPoints();
  double w[npts];
  //@}

  int subId;

  auto points = this->GetHandlePositions();
  auto pointsRange = vtk::DataArrayTupleRange<3>(points);
  int index = 0;
  for (auto pointRef : pointsRange)
  {
    double point[3];
    pointRef.GetTuple(point);
    // get subid for given point.
    polyline->EvaluatePosition(point, closest, subId, pcoords, dist2, w);
    if (subId > pickedSubId)
    {
      break;
    }
    index++;
  }

  return index;
}

//------------------------------------------------------------------------------
void vtkAbstractSplineRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ParametricSpline: (" << this->ParametricSpline << "\n";
  if (this->ParametricSpline)
  {
    this->ParametricSpline->PrintSelf(os, indent.GetNextIndent());
    os << indent << ")\n";
  }
  else
  {
    os << "none)\n";
  }

  os << indent << "Resolution: " << this->Resolution << "\n";
}
