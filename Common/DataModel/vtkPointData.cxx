/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPointData.h"

#include "vtkDataArray.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPointData);

void vtkPointData::NullPoint (vtkIdType ptId)
{
  vtkFieldData::Iterator it(this);
  vtkDataArray* da;
  for(da=it.Begin(); !it.End(); da=it.Next())
  {
    if (da)
    {
      int length = da->GetNumberOfComponents();
      float* tuple = new float[length];
      for(int j=0; j<length; j++)
      {
        tuple[j] = 0;
      }
      da->InsertTuple(ptId, tuple);
      delete[] tuple;
    }
  }
}

void vtkPointData::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
