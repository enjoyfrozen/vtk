/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBoxClipDataSet.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*----------------------------------------------------------------------------
 Copyright (c) Sandia Corporation
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/

/**
 * @class   vtkPlaneClipDataSet
 * @brief   clip an unstructured grid
 *
 *
 * Clipping means that is actually 'cuts' through the cells of the dataset,
 * returning tetrahedral cells inside of the box.
 * The output of this filter is an unstructured grid.
 *
 * This filter can be configured to compute a second output. The
 * second output is the part of the cell that is clipped away. Set the
 * GenerateClippedData boolean on if you wish to access this output data.
 *
 * The vtkPlaneClipDataSet will triangulate all types of 3D cells (i.e, create tetrahedra).
 * This is necessary to preserve compatibility across face neighbors.
 *
 * Using this filter is similar to using vtkBoxClipDataSet
 * 1) Set orientation
 *       SetPlaneClip(xmin,xmax,ymin,ymax,zmin,zmax)
 * 3) Execute clipping Update();
 */

#ifndef vtkPlaneClipDataSet_h
#define vtkPlaneClipDataSet_h

#include "vtkBoundingBox.h"
#include "vtkDataSet.h"
#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPlane.h"
#include "vtkSmartPointer.h"
#include "vtkUnstructuredGridAlgorithm.h"

#include <vector>

class VTKFILTERSGENERAL_EXPORT vtkPlaneClipDataSet : public vtkUnstructuredGridAlgorithm
{
public:
    vtkTypeMacro(vtkPlaneClipDataSet, vtkUnstructuredGridAlgorithm);
    vtkGetVector6Macro(Box, double);

    static vtkPlaneClipDataSet *New();
    void SetBox(const vtkBoundingBox& bbox);

protected:
    vtkPlaneClipDataSet();
    ~vtkPlaneClipDataSet() override {};

    //generate output data
    int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) override;
    double Box[6];

  private:
    vtkPlaneClipDataSet(const vtkPlaneClipDataSet&) = delete;
    void operator=(const vtkPlaneClipDataSet&) = delete;
};

#endif
