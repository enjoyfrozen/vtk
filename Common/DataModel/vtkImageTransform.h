/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImageTransform.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkImageTransform
 * @brief   helper class to transform output of non-axis-aligned images
 *
 * vtkImageTransform is a helper class to transform the output of
 * image filters (i.e., filter that input vtkImageData) where the
 * input image data is non-axis-aligned (i.e., its
 * orientation/direction is non-identify). Basically, what is
 * happening is that a non-axis-aligned image is processed as if it is
 * axis-aligned, and then the output of the filter (e.g., vtkPolyData)
 * is transformed with methods from this class using the image
 * orientation/direction. The transformation process is threaded with
 * vtkSMPTools for performance. Note that the transformation occurs in
 * place so no memory allocation is required.
 *
 * Typically in application the single method TransformPointSet() is
 * invoked to transform the output of an image algorithm (assuming
 * that the image's direction/orientation matrix is
 * non-identity). Note that vtkPointSets encompass vtkPolyData as well
 * as vtkUnstructuredGrids. In the future other output types may be
 * added. Note that specific methods for transforming points, normals,
 * and vectors is also provided by this class in case additional
 * output data arrays need to be transformed (since
 * TransformPointSet() only processes data arrays labeled as points,
 * normals, and vectors).
 *
 * @warning
 * This class has been threaded with vtkSMPTools. Using TBB or other
 * non-sequential type (set in the CMake variable
 * VTK_SMP_IMPLEMENTATION_TYPE) may improve performance significantly.
 *
 * @sa
 * vtkFlyingEdges3D
 */

#ifndef vtkImageTransform_h
#define vtkImageTransform_h

#include "vtkCommonDataModelModule.h" // For export macro
#include "vtkObject.h"

class vtkImageData;
class vtkPointSet;
class vtkDataArray;

class VTKCOMMONDATAMODEL_EXPORT vtkImageTransform : public vtkObject
{
public:
  //@{
  /**
   * Standard methods for construction, type information, printing.
   */
  static vtkImageTransform *New();
  vtkTypeMacro(vtkImageTransform,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  /**
   * Given a vtkImageData (and hence its associated orientation
   * matrix), and an instance of vtkPointSet, transform its points, as
   * well as any normals and vectors, associated with the
   * vtkPointSet. This is a convenience function, internally it calls
   * TransformPoints(), TransformNormals(), and/or TransformVectors()
   * as appropriate. Note that both the normals and vectors associated
   * with the point and cell data are transformed.
   */
  static void TransformPointSet(vtkImageData *im, vtkPointSet *ps);

  /**
   * Given x-y-z points represented by a vtkDataArray,
   * transform the points using the matrix provided.
   */
  static void TransformPoints(double (&m)[3][3], vtkDataArray *da);

  /**
   * Given three-component normals represented by a vtkDataArray,
   * transform the normals using the matrix provided.
   */
  static void TransformNormals(double (&m)[3][3], vtkDataArray *da);

  /**
   * Given three-component vectors represented by a vtkDataArray,
   * transform the vectors using the matrix provided.
   */
  static void TransformVectors(double (&m)[3][3], vtkDataArray *da);


protected:
  vtkImageTransform();
  ~vtkImageTransform() {}

private:
  vtkImageTransform(const vtkImageTransform&) = delete;
  void operator=(const vtkImageTransform&) = delete;
};

#endif
