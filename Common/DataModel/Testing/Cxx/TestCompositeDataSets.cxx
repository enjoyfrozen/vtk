/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkCompositeDataIterator.h"
#include "vtkDataObjectTree.h"
#include "vtkDataObjectTreeIterator.h"
#include "vtkInformation.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkSmartPointer.h"
#include "vtkStdString.h"
#include "vtkUniformGrid.h"
#include "vtkUniformGridAMR.h"

#include <iostream>
#include <vector>


//------------------------------------------------------------------------------
int TestCompositeDataSets(int , char *[])
{
  int errors = 0;


  return( errors );
}
