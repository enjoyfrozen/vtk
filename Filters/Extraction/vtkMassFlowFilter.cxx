/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMassFlowFilter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMassFlowFilter.h"

#include "vtkExtractSelection.h"
#include "vtkPolyDataNormals.h"
#include "vtkExecutive.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"
#include "vtkSMPTools.h"

vtkStandardNewMacro(vtkMassFlowFilter);

//----------------------------------------------------------------------------
// These templated classes perform mass flow computation. One uses a specified
// direction, the other surface normals at each point.
namespace {

  // Compute mass flow when direction specified
  template <typename TDens, typename TVel>
  struct ComputeMassFlowInDirection
  {
    const TDens *Density;
    const TVel  *Velocity;
    double Direction[3];
    double *MassFlow;

    ComputeMassFlowInDirection(const TDens *dens, const TVel *vel, double *dir,
                               double *massF) :
      Density(dens), Velocity(vel), MassFlow(massF)
    {
      this->Direction[0] = dir[0];
      this->Direction[1] = dir[1];
      this->Direction[2] = dir[2];
    }

    void operator() (vtkIdType ptId, vtkIdType end)
    {
      const TDens *dens = this->Density + ptId;
      const TVel *vel = this->Velocity + 3*ptId;
      double *d = this->Direction;
      double *mf = this->MassFlow + ptId;

      for ( ; ptId < end; ++ptId, ++dens, vel+=3, ++mf )
      {
        *mf = *dens * (d[0]*vel[0] + d[1]*vel[1] + d[2]*vel[2]);
      }
    }

    static void Execute(vtkIdType numPts, TDens *dens, TVel *vel,
                        double *dir, double *massF)
    {
      ComputeMassFlowInDirection cmf(dens, vel, dir, massF);
      vtkSMPTools::For(0, numPts, cmf);
    }
  };

  // Compute mass flow when surface normals provided
  template <typename TDens, typename TVel, typename TNorm>
  struct ComputeMassFlow
  {
    const TDens *Density;
    const TVel  *Velocity;
    const TNorm *Normals;
    double *MassFlow;

    ComputeMassFlow(const TDens *dens, const TVel *vel, const TNorm *n,
                    double *massF) :
      Density(dens), Velocity(vel), Normals(n), MassFlow(massF)
    {
    }

    void operator() (vtkIdType ptId, vtkIdType end)
    {
      const TDens *dens = this->Density + ptId;
      const TVel *vel = this->Velocity + 3*ptId;
      const TNorm *n = this->Normals + 3*ptId;
      double *mf = this->MassFlow + ptId;

      for ( ; ptId < end; ++ptId, ++dens, vel+=3, n+=3, ++mf )
      {
        *mf = *dens * (n[0]*vel[0]+n[1]*vel[1]+n[2]*vel[2]);
      }
    }

    static void Execute(vtkIdType numPts, TDens *dens, TVel *vel,
                        TNorm *n, double *massF)
    {
      ComputeMassFlow cmf(dens, vel, n, massF);
      vtkSMPTools::For(0, numPts, cmf);
    }
  };

  // Compute mass flow for non-real types
  struct GeneralComputeMassFlow
  {
    vtkDataArray *Density;
    vtkDataArray *Velocity;
    vtkDataArray *Normals;
    double *Direction;
    double *MassFlow;

    GeneralComputeMassFlow(vtkDataArray *dens, vtkDataArray *vel,
                           vtkDataArray *n, double *dir, double *massF) :
      Density(dens), Velocity(vel), Normals(n), Direction(dir), MassFlow(massF)
    {
    }

    void operator() (vtkIdType ptId, vtkIdType end)
    {
      double dens[1], vel[3], n[3], *d=this->Direction;
      double *mf = this->MassFlow + ptId;

      for ( ; ptId < end; ++ptId, ++mf )
      {
        this->Density->GetTuple(ptId,dens);
        this->Velocity->GetTuple(ptId,vel);
        if ( d != nullptr )
        {
          n[0]  = d[0];
          n[1]  = d[1];
          n[2]  = d[2];
        }
        else
        {
          this->Normals->GetTuple(ptId,n);
        }
        *mf = dens[0] * (n[0]*vel[0]+n[1]*vel[1]+n[2]*vel[2]);
      }
    }

    static void Execute(vtkIdType numPts, vtkDataArray *dens, vtkDataArray *vel,
                        vtkDataArray *n, double *dir, double *massF)
    {
      GeneralComputeMassFlow cmf(dens, vel, n, dir, massF);
      vtkSMPTools::For(0, numPts, cmf);
    }
  };

}//anonymous namespace

//----------------------------------------------------------------------------
vtkMassFlowFilter::vtkMassFlowFilter()
{
  this->DensityArrayName = nullptr;
  this->SetDensityArrayName("Density");

  this->VelocityArrayName = nullptr;
  this->SetVelocityArrayName("Velocity");

  this->NormalsArrayName = nullptr;

  this->MassFlowArrayName = nullptr;
  this->SetMassFlowArrayName("Mass Flow");

  this->DirectionMode = VTK_SURFACE_NORMALS;
  this->Direction[0] = 0.0;
  this->Direction[1] = 0.0;
  this->Direction[2] = 1.0;

  this->TotalMassFlow = 0.0;

  this->Extract = vtkExtractSelection::New();
  this->Normals = vtkPolyDataNormals::New();

  this->SetNumberOfInputPorts(2);
}

//----------------------------------------------------------------------------
vtkMassFlowFilter::~vtkMassFlowFilter()
{
  this->SetDensityArrayName(nullptr);
  this->SetVelocityArrayName(nullptr);
  this->SetNormalsArrayName(nullptr);
  this->SetMassFlowArrayName(nullptr);

  this->Extract->Delete();
  this->Extract = nullptr;

  this->Normals->Delete();
  this->Normals = nullptr;
}

//----------------------------------------------------------------------------
// Specify the selection data.
void vtkMassFlowFilter::
SetSelectionData(vtkSelection *s)
{
  this->SetInputData(1, s);
}

//----------------------------------------------------------------------------
// Get a pointer to a selection object from the second input.
vtkSelection *vtkMassFlowFilter::GetSelection()
{
  return vtkSelection::SafeDownCast(
    this->GetExecutive()->GetInputData(1, 0));
}

//----------------------------------------------------------------------------
int vtkMassFlowFilter::
FillInputPortInformation(int port, vtkInformation* info)
{
  if (port==0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
  }
  else
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkSelection");
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 1);
  }
  return 1;
}

//----------------------------------------------------------------------------
int vtkMassFlowFilter::RequestData(
  vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector ,
  vtkInformationVector* outputVector)
{
  vtkPolyData* input2 = vtkPolyData::GetData(inputVector[0], 0);
  vtkSelection* selection = vtkSelection::GetData(inputVector[1], 0);
  vtkPolyData* output = vtkPolyData::GetData(outputVector, 0);

  // Initialize
  this->TotalMassFlow = 0.0;

  // If no input, error
  if ( !input2 )
  {
    vtkErrorMacro(<<"No input specified");
    return 0;
  }

  // If no selection, then the entire input polydata is processed. Otherwise
  // extract the selection.
  vtkPolyData *input = input2;
  if ( selection )
  {
    this->Extract->SetInputData(input);
    this->Extract->SetSelectionData(selection);
    this->Extract->Update();
    input = vtkPolyData::SafeDownCast(this->Extract->GetOutput());
  }
  vtkIdType numPts = input->GetNumberOfPoints();
  if ( numPts <= 0 )
  {
    vtkErrorMacro(<<"No flow surface");
    return 0;
  }

  // Check to see that the arrays specified are available in the input, and have
  // the right number of components
  if ( this->DensityArrayName == nullptr )
  {
    vtkErrorMacro("No density array specified");
    return 0;
  }
  vtkDataArray *densArray = input->GetPointData()->GetArray(this->DensityArrayName);
  if ( densArray == nullptr || densArray->GetNumberOfComponents() != 1 )
  {
    vtkErrorMacro("No valid density array found");
    return 0;
  }

  if ( this->VelocityArrayName == nullptr )
  {
    vtkErrorMacro("No velocity array specified");
    return 0;
  }
  vtkDataArray *velArray = input->GetPointData()->GetArray(this->VelocityArrayName);
  if ( velArray == nullptr || velArray->GetNumberOfComponents() != 3 )
  {
    vtkErrorMacro("No valid velocity array found");
    return 0;
  }

  // Make sure that appropriate direction information is available
  vtkDataArray *normals=nullptr;
  int normalsType;
  double *dir=nullptr;
  if ( this->DirectionMode == VTK_SPECIFIED_DIRECTION )
  {
    normalsType = VTK_DOUBLE;
    if ( vtkMath::Normalize(this->Direction) <= 0.0 )
    {
      vtkErrorMacro("Invalid mass flow direction");
      return 0;
    }
    dir = this->Direction;
  }
  else //surface normals needed, may have to compute them
  {
    normals = input->GetPointData()->GetNormals();
    if ( normals == nullptr )
    {
      this->Normals->SetInputData(input);
      this->Normals->SplittingOff();
      this->Normals->Update();
      normals = this->Normals->GetOutput()->GetPointData()->GetNormals();
    }
    if ( normals == nullptr || normals->GetNumberOfTuples() != numPts )
    {
      vtkErrorMacro("No surface normals");
      return 0;
    }
    normalsType = normals->GetDataType();
  }

  // Prepare the output
  if ( this->MassFlowArrayName == nullptr )
  {
    this->SetMassFlowArrayName("Mass Flow");
  }
  vtkDoubleArray *massFlowArray = vtkDoubleArray::New();
  massFlowArray->SetName(this->MassFlowArrayName);
  massFlowArray->SetNumberOfComponents(1);
  massFlowArray->SetNumberOfTuples(numPts);
  double *massF = static_cast<double*>(massFlowArray->GetVoidPointer(0));

  // Now process the input and produce the output. A fast path exists
  // for real types.
  int densType = densArray->GetDataType();
  int velType = velArray->GetDataType();

  // Check to see whether fast path can be used
  if ( (densType != VTK_FLOAT && densType != VTK_DOUBLE) ||
       (velType != VTK_FLOAT && velType != VTK_DOUBLE) ||
       (normalsType != VTK_FLOAT && normalsType != VTK_DOUBLE) )
  {
    GeneralComputeMassFlow::
      Execute(numPts, densArray, velArray, normals, dir, massF);
  }

  else //fast path
  {
    int combo = (densType == VTK_FLOAT ? 0 : 1);
    combo |= (velType == VTK_FLOAT ? 0 : 2);
    combo |= (normalsType == VTK_FLOAT ? 0 : 4);
    if ( this->DirectionMode == VTK_SPECIFIED_DIRECTION )
    {
      switch (combo)
      {
        case 0: case 4:
          ComputeMassFlowInDirection<float,float>::
            Execute(numPts,(float*)densArray->GetVoidPointer(0),
                    (float*)velArray->GetVoidPointer(0), dir, massF);
          break;
        case 1: case 5:
          ComputeMassFlowInDirection<double,float>::
            Execute(numPts,(double*)densArray->GetVoidPointer(0),
                    (float*)velArray->GetVoidPointer(0), dir, massF);
          break;
        case 2: case 6:
          ComputeMassFlowInDirection<float,double>::
            Execute(numPts,(float*)densArray->GetVoidPointer(0),
                    (double*)velArray->GetVoidPointer(0), dir, massF);
          break;
        case 3: case 7:
          ComputeMassFlowInDirection<double,double>::
            Execute(numPts,(double*)densArray->GetVoidPointer(0),
                    (double*)velArray->GetVoidPointer(0), dir, massF);
          break;
      }
    }
    else // VTK_SURFACE_NORMALS
    {
      switch (combo)
      {
        case 0:
          ComputeMassFlow<float,float,float>::
            Execute(numPts, (float*)densArray->GetVoidPointer(0),
                    (float*)velArray->GetVoidPointer(0),
                    (float*)normals->GetVoidPointer(0), massF);
          break;
        case 1:
          ComputeMassFlow<double,float,float>::
            Execute(numPts, (double*)densArray->GetVoidPointer(0),
                    (float*)velArray->GetVoidPointer(0),
                    (float*)normals->GetVoidPointer(0), massF);
          break;
        case 2:
          ComputeMassFlow<float,double,float>::
            Execute(numPts, (float*)densArray->GetVoidPointer(0),
                    (double*)velArray->GetVoidPointer(0),
                    (float*)normals->GetVoidPointer(0), massF);
          break;
        case 3:
          ComputeMassFlow<double,double,float>::
            Execute(numPts, (double*)densArray->GetVoidPointer(0),
                    (double*)velArray->GetVoidPointer(0),
                    (float*)normals->GetVoidPointer(0), massF);
          break;
        case 4:
          ComputeMassFlow<float,float,double>::
            Execute(numPts, (float*)densArray->GetVoidPointer(0),
                    (float*)velArray->GetVoidPointer(0),
                    (double*)normals->GetVoidPointer(0), massF);
          break;
        case 5:
          ComputeMassFlow<double,float,double>::
            Execute(numPts, (double*)densArray->GetVoidPointer(0),
                    (float*)velArray->GetVoidPointer(0),
                    (double*)normals->GetVoidPointer(0), massF);
          break;
        case 6:
          ComputeMassFlow<float,double,double>::
            Execute(numPts, (float*)densArray->GetVoidPointer(0),
                    (double*)velArray->GetVoidPointer(0),
                    (double*)normals->GetVoidPointer(0), massF);
          break;
        case 7:
          ComputeMassFlow<double,double,double>::
            Execute(numPts, (double*)densArray->GetVoidPointer(0),
                    (double*)velArray->GetVoidPointer(0),
                    (double*)normals->GetVoidPointer(0), massF);
      }
    }
  }

  // Sum the total mass flow
  for ( vtkIdType ptId=0; ptId < numPts; ++ptId )
  {
    this->TotalMassFlow += massF[ptId];
  }

  // Now clean up as necessary
  output->ShallowCopy(input);
  output->GetPointData()->AddArray(massFlowArray);
  massFlowArray->Delete();

  return 1;
}


//----------------------------------------------------------------------------
vtkExtractSelection *vtkMassFlowFilter::
GetExtractSelectionFilter()
{
  return this->Extract;
}

//----------------------------------------------------------------------------
vtkPolyDataNormals *vtkMassFlowFilter::
GetPolyDataNormalsFilter()
{
  return this->Normals;
}

//----------------------------------------------------------------------------
// Overload standard modified time function. If the plane definition is modified,
// then this object is modified as well.
vtkMTimeType vtkMassFlowFilter::GetMTime()
{
  vtkMTimeType mTime=this->Superclass::GetMTime();

  vtkMTimeType mTime2=this->Extract->GetMTime();
  mTime = (mTime > mTime2 ? mTime : mTime2);

  mTime2=this->Normals->GetMTime();
  mTime = (mTime > mTime2 ? mTime : mTime2);

  return mTime;
}

//----------------------------------------------------------------------------
void vtkMassFlowFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Density Array Name: "
     << (this->DensityArrayName == nullptr ? "(none)" : this->DensityArrayName) << "\n";

  os << indent << "Velocity Array Name: "
     << (this->VelocityArrayName == nullptr ? "(none)" : this->VelocityArrayName) << "\n";

  os << indent << "Mass Flow Array Name: "
     << (this->MassFlowArrayName == nullptr ? "(none)" : this->MassFlowArrayName) << "\n";

  os << indent << "Direction Mode: " << this->GetDirectionModeAsString() << "\n";

  os << indent << "Direction: ("
     << this->Direction[0] << ", "
     << this->Direction[1] << ", "
     << this->Direction[2] << ")\n";

  os << indent << "Total Mass Flow: " << this->TotalMassFlow << endl;
}
