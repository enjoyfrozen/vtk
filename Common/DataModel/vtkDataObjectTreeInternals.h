/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkDataObjectTreeInternals
 *
*/

#ifndef vtkDataObjectTreeInternals_h
#define vtkDataObjectTreeInternals_h

#include "vtkDataObject.h"
#include "vtkInformation.h"
#include "vtkSmartPointer.h"

#include <vector>

//-----------------------------------------------------------------------------
// Item in the VectorOfDataObjects.
struct vtkDataObjectTreeItem
{
  vtkSmartPointer<vtkDataObject> DataObject;
  vtkSmartPointer<vtkInformation> MetaData;

  vtkDataObjectTreeItem(vtkDataObject* dobj =nullptr, vtkInformation* info=nullptr)
  {
    this->DataObject = dobj;
    this->MetaData = info;
  }
};

//-----------------------------------------------------------------------------
class vtkDataObjectTreeInternals
{
public:
  typedef std::vector<vtkDataObjectTreeItem> VectorOfDataObjects;
  typedef VectorOfDataObjects::iterator Iterator;
  typedef VectorOfDataObjects::reverse_iterator ReverseIterator;

  VectorOfDataObjects Children;
};


//-----------------------------------------------------------------------------
class vtkDataObjectTreeIndex : public std::vector<unsigned int>
{
  int IsValid()
  {
    return (this->size()> 0);
  }
};

#endif


// VTK-HeaderTest-Exclude: vtkDataObjectTreeInternals.h
