/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkResliceCursorThickLineRepresentation.h"
#include "vtkImageSlabReslice.h"
#include "vtkImageReslice.h"
#include "vtkResliceCursor.h"
#include "vtkImageData.h"
#include "vtkObjectFactory.h"
#include "vtkImageMapToColors.h"
#include <cmath>
#include <algorithm>

#include <sstream>

vtkStandardNewMacro(vtkResliceCursorThickLineRepresentation);


//----------------------------------------------------------------------
vtkResliceCursorThickLineRepresentation::vtkResliceCursorThickLineRepresentation()
{
  this->CreateDefaultResliceAlgorithm();
}

//----------------------------------------------------------------------
vtkResliceCursorThickLineRepresentation::~vtkResliceCursorThickLineRepresentation() = default;

//----------------------------------------------------------------------
void vtkResliceCursorThickLineRepresentation::CreateDefaultResliceAlgorithm()
{
  if (this->Reslice)
  {
    this->Reslice->Delete();
  }

  // Override superclass implementation to create a vtkImageSlabReslice here.
  this->Reslice = vtkImageSlabReslice::New();
}

//----------------------------------------------------------------------------
void vtkResliceCursorThickLineRepresentation
::SetResliceParameters( double outputSpacingX, double outputSpacingY,
    int extentX, int extentY )
{
  vtkImageSlabReslice *thickReslice
    = vtkImageSlabReslice::SafeDownCast(this->Reslice);

  if (thickReslice)
  {

    // Set the default color the minimum scalar value
    double range[2];
    vtkImageData::SafeDownCast(thickReslice->GetInput())->
      GetScalarRange( range );
    thickReslice->SetBackgroundLevel(range[0]);

    // Set the usual parameters.

    this->ColorMap->SetInputConnection(thickReslice->GetOutputPort());
    thickReslice->TransformInputSamplingOff();
    thickReslice->SetResliceAxes(this->ResliceAxes);
    thickReslice->SetOutputSpacing(outputSpacingX, outputSpacingY, 1);
    thickReslice->SetOutputOrigin(0.5*outputSpacingX, 0.5*outputSpacingY, 0);
    thickReslice->SetOutputExtent(0, extentX-1, 0, extentY-1, 0, 0);

    vtkResliceCursor *rc = this->GetResliceCursor();
    thickReslice->SetSlabThickness(rc->GetThickness()[0]);
    double spacing[3];
    rc->GetImage()->GetSpacing(spacing);


    // Perhaps we should multiply this by 0.5 for nyquist
    const double minSpacing = std::min(
        std::min(spacing[0], spacing[1]), spacing[2]);

    // Set the slab resolution the minimum spacing. Reasonable default
    thickReslice->SetSlabResolution(minSpacing);
  }
}

//----------------------------------------------------------------------
void vtkResliceCursorThickLineRepresentation::PrintSelf(ostream& os, vtkIndent indent)
{
  //Superclass typedef defined in vtkTypeMacro() found in vtkSetGet.h
  this->Superclass::PrintSelf(os,indent);
}
