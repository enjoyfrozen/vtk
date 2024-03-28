// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkAsyncFrameRecorder.h"
#include "vtkObjectFactory.h"

VTK_ABI_NAMESPACE_BEGIN
//------------------------------------------------------------------------------
vtkAsyncFrameRecorder::vtkAsyncFrameRecorder() = default;

//------------------------------------------------------------------------------
vtkAsyncFrameRecorder::~vtkAsyncFrameRecorder() = default;

//------------------------------------------------------------------------------
void vtkAsyncFrameRecorder::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << "LastSrcWidth: " << this->LastSrcWidth << std::endl;
  os << "LastSrcHeight: " << this->LastSrcHeight << std::endl;
  os << "LastDstWidth: " << this->LastDstWidth << std::endl;
  os << "LastDstHeight: " << this->LastDstHeight << std::endl;
}
VTK_ABI_NAMESPACE_END
