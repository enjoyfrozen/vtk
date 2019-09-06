/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
#include "vtkRowQuery.h"

#include "vtkObjectFactory.h"
#include "vtkStdString.h"
#include "algorithm"
#include "vtkVariantArray.h"

#include <cctype>


vtkRowQuery::vtkRowQuery()
{
  this->CaseSensitiveFieldNames = false;
}

vtkRowQuery::~vtkRowQuery() = default;

void vtkRowQuery::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "CaseSensitiveFieldNames: "
    << this->CaseSensitiveFieldNames << endl;
}

int vtkRowQuery::GetFieldIndex(const char* name)
{
  vtkStdString lcSearchName(name);
  std::transform(lcSearchName.begin(),
                    lcSearchName.end(),
                    lcSearchName.begin(),
                    (int(*)(int))tolower);

  int index;
  bool found = false;
  for (index = 0; index < this->GetNumberOfFields(); index++)
  {
    if (this->CaseSensitiveFieldNames)
    {
      if (!strcmp(name, this->GetFieldName(index)))
      {
        found = true;
        break;
      }
    }
    else
    {
      vtkStdString fieldName(this->GetFieldName(index));
      std::transform(fieldName.begin(),
                        fieldName.end(),
                        fieldName.begin(),
                        (int(*)(int))tolower);
      if (lcSearchName == fieldName)
      {
        found = true;
        break;
      }
    }
  }
  if (found)
  {
    return index;
  }
  return -1;
}


bool vtkRowQuery::NextRow(vtkVariantArray* rowArray)
{
  if (!this->NextRow())
  {
    return false;
  }
  rowArray->Reset();
  for (int col = 0; col < this->GetNumberOfFields(); col++)
  {
    rowArray->InsertNextValue(this->DataValue(col));
  }
  return true;
}

