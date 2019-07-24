/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPlaneClipDataSet.cxx

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

#include "vtkBoundingBox.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPlane.h"
#include "vtkPlaneClipDataSet.h"
#include "vtkPointData.h"
#include "vtkSmartPointer.h"
#include "vtkTableBasedClipDataSet.h"
#include "vtkUnstructuredGrid.h"

#include <vector>
#include <string>

vtkStandardNewMacro(vtkPlaneClipDataSet);
// basic constructor
vtkPlaneClipDataSet::vtkPlaneClipDataSet()
{
    // initialize Box to invalid box here
    vtkBoundingBox bbox;
    SetBox(bbox);
}

void vtkPlaneClipDataSet::SetBox(const vtkBoundingBox& bbox)
{
    bbox.GetBounds(box);
    this->SetPlaneClip();
}

vtkSmartPointer<vtkPlane> setPlane(double center[3], double normal[3])
{
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    plane->SetOrigin(center);
    plane->SetNormal(normal);

    return plane;
}

// convert the bounding box to a set of clipping planes
void vtkPlaneClipDataSet::SetPlaneClip()
{
    // get the xmin, xmax, ymin, ymax, zmin and zmax
    allPlanes.clear();

    // three midpoints of eachbound
    double xMid = (box[0] + box[1])/2.0;
    double yMid = (box[2] + box[3])/2.0;
    double zMid = (box[4] + box[5])/2.0;

    for (int i = 0; i < 6; i++)
    {
        // xmin and xmax
        if (i < 2)
        {
            double center[3] = {box[i], yMid, zMid};
            double normal[3] = {1.0-i*2.0, 0.0, 0.0};
            vtkSmartPointer<vtkPlane> plane = setPlane(center, normal);
            allPlanes.push_back(plane);
        }
        // ymin and ymax
        else if (i < 4)
        {
            double center[3] = {xMid, box[i], zMid};
            double normal[3] = {0.0, 5.0-i*2.0, 0.0};
            vtkSmartPointer<vtkPlane> plane = setPlane(center, normal);
            allPlanes.push_back(plane);
        }
        // zmin and zmax
        else
        {
            double center[3] = {xMid, yMid, box[i]};
            double normal[3] = {0.0, 0.0, 9.0-i*2.0};
            vtkSmartPointer<vtkPlane> plane = setPlane(center, normal);
            allPlanes.push_back(plane);
        }
    }
}

// helper function for generating output
vtkSmartPointer<vtkDataSet> ClipPlane(vtkDataSet* dataset, vtkSmartPointer<vtkPlane> plane)
{
    // make the clip
    auto clipper = vtkSmartPointer<vtkTableBasedClipDataSet>::New();
    clipper->SetInputData(dataset);
    clipper->SetClipFunction(plane);
    clipper->SetValue(0);
    clipper->Update();

    // obtain the clipped dataset
    auto clipperOutput = vtkUnstructuredGrid::SafeDownCast(clipper->GetOutputDataObject(0));
    if (clipperOutput &&
        (clipperOutput->GetNumberOfCells() > 0 || clipperOutput->GetNumberOfPoints() > 0))
    {
        return clipperOutput;
    }
    return nullptr;
}

// helper function for generating output
vtkSmartPointer<vtkDataSet> ApplyPlaneClip(const vtkSmartPointer<vtkDataSet> dataset,
                                            std::vector< vtkSmartPointer<vtkPlane> > allPlanes)
{
    // clip all planes
    auto clipperOutput = ClipPlane(dataset, allPlanes[0]);
    for (int i = 1; i < 6; i++)
    {
        clipperOutput = ClipPlane(clipperOutput, allPlanes[i]);
    }

    return clipperOutput;
}

int vtkPlaneClipDataSet::RequestData(vtkInformation *vtkNotUsed(request),
                                     vtkInformationVector **inputVector,
                                     vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    // get the input and output
    vtkSmartPointer<vtkDataSet> input = vtkDataSet::SafeDownCast(
        inInfo->Get(vtkDataObject::DATA_OBJECT()));
    vtkSmartPointer<vtkDataSet> output = vtkDataSet::SafeDownCast(
        outInfo->Get(vtkDataObject::DATA_OBJECT()));

    // shallow copy the output
    output->ShallowCopy(ApplyPlaneClip(input, allPlanes));

    return 1;
}
