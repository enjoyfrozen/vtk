/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAbstractCellLinks.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"

//----------------------------------------------------------------------------
vtkAbstractCellLinks::vtkAbstractCellLinks()
{
  this->SequentialProcessing = false;
  this->Type = vtkAbstractCellLinks::LINKS_NOT_DEFINED;
}

//----------------------------------------------------------------------------
vtkAbstractCellLinks::~vtkAbstractCellLinks() = default;

//----------------------------------------------------------------------------
int vtkAbstractCellLinks::
ComputeType(vtkIdType maxPtId, vtkIdType maxCellId, vtkCellArray *ca)
{
  vtkIdType numEntries = ca->GetNumberOfConnectivityEntries();
  vtkIdType max = maxPtId;
  max = (maxCellId > max ? maxCellId : max);
  max = (numEntries > max ? numEntries : max);

  if ( max < VTK_UNSIGNED_SHORT_MAX )
  {
    return vtkAbstractCellLinks::STATIC_CELL_LINKS_USHORT;
  }
  // for 64bit IDS we might be able to use a unsigned int instead
#if defined(VTK_USE_64BIT_IDS) && VTK_SIZEOF_INT == 4
  else if ( max < static_cast<vtkIdType>(VTK_UNSIGNED_INT_MAX) )
  {
    return vtkAbstractCellLinks::STATIC_CELL_LINKS_UINT;
  }
#endif
  return vtkAbstractCellLinks::STATIC_CELL_LINKS_IDTYPE;
}

//----------------------------------------------------------------------------
void vtkAbstractCellLinks::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Sequential Processing: "
     << (this->SequentialProcessing ? "true\n" : "false\n");
  os << indent << "Type: " << this->Type << "\n";
}
