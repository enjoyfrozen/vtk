/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkStructuredPointsGeometryFilter.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkStructuredPointsGeometryFilter);

vtkStructuredPointsGeometryFilter::vtkStructuredPointsGeometryFilter()
{
  vtkErrorMacro("vtkStructuredPointsGeometryFilter will be deprecated in" << endl <<
                "the next release after VTK 4.0. Please use" << endl <<
                "vtkImageDataGeometryFilter instead" );
}
