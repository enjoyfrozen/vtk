/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSparkline.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * @class   vtkSparkline
 * @brief   Serialize an array as a series of printable unicode characters.
 *
 * This class will generate unicode text that encodes a vtkDataArray into
 * a utf-8 bar chart. The text is a string holding a single line of text
 * with at most N characters (you provide N).
 *
 * For example, a sparkline for a vtkDoubleArray holding `{0., 2., 1.5., 1.}`
 * will yield "▁█▆▄". You can set a maximum or fixed width for
 * the sparkline and the array will be sampled as needed.
 */

#ifndef vtkSparkline_h
#define vtkSparkline_h

#include "vtkCommonMiscModule.h" // For export macro
#include "vtkObject.h"

#include <string>

class vtkDataArray;

class VTKCOMMONMISC_EXPORT vtkSparkline : public vtkObject
{
public:
  static vtkSparkline* New();
  vtkTypeMacro(vtkSparkline, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**\brief Return a string holding a sparkline of the array.
   *
   * If the \a data array has tuples with more than 1 component,
   * the \a component parameter selects the component to plot.
   * A value of -1 (the default) will plot the L2 norm of each tuple.
   *
   * The \a width is the maximum number of characters the sparkline
   * should occupy.
   *
   * If \a expand is false and the array is shorter than \a width,
   * then the returned string will be less than \a width glyphs.
   * Otherwise, the returned string will be exactly \a width
   * unicode glyphs long. (Note that each glyph is encoded as
   * a multi-byte UTF-8 sequence.)
   */
  static std::string FromArray(
    vtkDataArray* data, int component = -1, vtkIdType width = 25, bool expand = false);

protected:
  vtkSparkline() = default;
  ~vtkSparkline() override = default;

  static double GetComponent(vtkDataArray* data, vtkIdType tupleId, int component);

private:
  vtkSparkline(const vtkSparkline&) = delete;
  void operator=(const vtkSparkline&) = delete;
};

#endif
