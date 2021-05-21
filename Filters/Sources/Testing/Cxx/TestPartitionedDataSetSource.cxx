/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestTextSource.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkParametricKlein.h"
#include <cassert>
#include <vtkPartitionedDataSet.h>
#include <vtkPartitionedDataSetSource.h>
#include <vtkSmartPointer.h>

int TestPartitionedDataSetSource(int vtkNotUsed(argc), char* vtkNotUsed(argv)[])
{
  {
    vtkNew<vtkPartitionedDataSetSource> pdsSource;
    pdsSource->SetParametricFunction(vtkSmartPointer<vtkParametricKlein>::New());

    assert(
      "vtkPartitionedDataSetSource::IsEnabledRank(1) must be true" && pdsSource->IsEnabledRank(1));

    pdsSource->DisableRank(1);
    assert("vtkPartitionedDataSetSource::IsEnabledRank(1) must be false" &&
      !pdsSource->IsEnabledRank(1));

    pdsSource->DisableRank(2);
    pdsSource->DisableRank(4);
    pdsSource->EnableRank(4);
    pdsSource->Update();

    // Rank 0 is enabled
    auto pds = pdsSource->GetOutput();
    assert("vtkPartitionedDataSetSource::GetNumberOfPartitions() must be 1" &&
      pds->GetNumberOfPartitions() == 1);

    // Rank 2 is disabled
    pdsSource->UpdatePiece(2, 5, 0);
    pds = pdsSource->GetOutput();
    assert("GetOuput(rank2) returns empty PartitionedDataSet" && pds->GetNumberOfPartitions() == 0);

    // Rank 4 is enabled
    pdsSource->UpdatePiece(4, 5, 0);
    pds = pdsSource->GetOutput();
    assert("vtkPartitionedDataSetSource::GetNumberOfPartitions() (rank4) must be 1" &&
      pds->GetNumberOfPartitions() == 1);
  }

  // Now we specify the number of partitions
  {
    vtkNew<vtkPartitionedDataSetSource> pdsSource;
    pdsSource->SetParametricFunction(vtkSmartPointer<vtkParametricKlein>::New());

    pdsSource->SetNumberOfPartitions(6);
    pdsSource->DisableRank(1);
    pdsSource->DisableRank(2);

    // Rank 0 is enabled
    pdsSource->UpdatePiece(0, 5, 0);
    auto pds = pdsSource->GetOutput();
    assert("vtkPartitionedDataSetSource::GetNumberOfPartitions() must be 2" &&
      pds->GetNumberOfPartitions() == 2);

    // Rank 4 is enabled
    pdsSource->UpdatePiece(4, 5, 0);
    pds = pdsSource->GetOutput();
    assert("vtkPartitionedDataSetSource::GetNumberOfPartitions() (rank4) must be 2" &&
      pds->GetNumberOfPartitions() == 2);
  }

  return EXIT_SUCCESS;
}
