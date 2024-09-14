/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSparkline.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkSparkline.h"
#include "vtkDataArray.h"
#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include <array>
#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>

vtkStandardNewMacro(vtkSparkline);

void vtkSparkline::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

std::string vtkSparkline::FromArray(vtkDataArray* data, int component, vtkIdType width, bool expand)
{
  std::string result;
  if (!data)
  {
    return result;
  }
  vtkIdType nn = data->GetNumberOfTuples();
  if (nn <= 0)
  {
    return result;
  }
  // U+2581	▁	e2 96 81	LOWER ONE EIGHTH BLOCK
  // U+2582	▂	e2 96 82	LOWER ONE QUARTER BLOCK
  // U+2583	▃	e2 96 83	LOWER THREE EIGHTHS BLOCK
  // U+2584	▄	e2 96 84	LOWER HALF BLOCK
  // U+2585	▅	e2 96 85	LOWER FIVE EIGHTHS BLOCK
  // U+2586	▆	e2 96 86	LOWER THREE QUARTERS BLOCK
  // U+2587	▇	e2 96 87	LOWER SEVEN EIGHTHS BLOCK
  // U+2588	█	e2 96 88	FULL BLOCK
  std::array<double, 2> range;
  data->GetRange(range.data(), component);
  double nd = static_cast<double>(nn);
  if (expand || nn > width)
  {
    result.resize(width * 3);
    for (vtkIdType ii = 0; ii < width; ++ii)
    {
      vtkIdType jj = static_cast<vtkIdType>(std::floor(((nd - 1) * (ii + 0.25)) / (width - 1)));
      double yy = vtkSparkline::GetComponent(data, jj, component);
      double hh = range[1] == 0.0 ? 0.5 : (yy - range[0]) / (range[1] - range[0]);
      unsigned char bb = static_cast<char>(std::floor(7 * hh)) + 0x81;
      result[3 * ii + 0] = static_cast<char>(0xe2);
      result[3 * ii + 1] = static_cast<char>(0x96);
      result[3 * ii + 2] = bb;
    }
  }
  else
  {
    result.resize(nn * 3);
    for (vtkIdType ii = 0; ii < nn; ++ii)
    {
      vtkIdType jj = static_cast<vtkIdType>(std::floor(((nd - 1) * (ii + 0.25)) / (nd - 1)));
      double yy = vtkSparkline::GetComponent(data, jj, component);
      double hh = range[1] == 0.0 ? 0.5 : (yy - range[0]) / range[1];
      unsigned char bb = static_cast<char>(std::floor(7 * hh)) + 0x81;
      result[3 * ii + 0] = static_cast<char>(0xe2);
      result[3 * ii + 1] = static_cast<char>(0x96);
      result[3 * ii + 2] = bb;
    }
  }
  return result;
}

double vtkSparkline::GetComponent(vtkDataArray* data, vtkIdType tupleId, int component)
{
  if (component < 0)
  {
    double accum = 0;
    for (int cc = 0; cc < data->GetNumberOfComponents(); ++cc)
    {
      double yy = data->GetComponent(tupleId, cc);
      accum += yy * yy;
    }
    return std::sqrt(accum);
  }
  return data->GetComponent(tupleId, component);
}
