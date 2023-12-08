// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#include "vtkHyperTreeGridProbeFilterUtilities.h"

#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkFloatArray.h"
#include "vtkMath.h"
#include "vtkSMPTools.h"
#include "vtkStringArray.h"
#include "vtkType.h"

namespace vtkHyperTreeGridProbeFilterUtilities
{
VTK_ABI_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
void FillDefaultArray(vtkAbstractArray* array)
{
  // This function could be improved with array dispatch
  if (auto* strArray = vtkStringArray::SafeDownCast(array))
  {
    vtkSMPTools::For(0, strArray->GetNumberOfValues(), [strArray](vtkIdType start, vtkIdType end) {
      for (vtkIdType i = start; i < end; ++i)
      {
        strArray->SetValue(i, "");
      }
    });
  }
  else if (auto* doubleArray = vtkArrayDownCast<vtkDoubleArray>(array))
  {
    doubleArray->Fill(vtkMath::Nan());
  }
  else if (auto* floatArray = vtkArrayDownCast<vtkFloatArray>(array))
  {
    floatArray->Fill(vtkMath::Nan());
  }
  else if (auto* dArray = vtkArrayDownCast<vtkDataArray>(array))
  {
    // Deal with SOA of float/doubles
    if (dArray->GetDataType() == VTK_FLOAT || dArray->GetDataType() == VTK_DOUBLE)
    {
      dArray->Fill(vtkMath::Nan());
    }
    else
    {
      dArray->Fill(0);
    }
  }
  else
  {
    vtkGenericWarningMacro("Array is not a vtkDataArray nor is it a vtkStringArray and will not be "
                           "filled with default values.");
  }
}

VTK_ABI_NAMESPACE_END
} // end of namespace.
