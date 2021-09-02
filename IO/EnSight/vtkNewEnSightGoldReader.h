/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkNewEnSightGoldReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/**
 * @class vtkNewEnSightGoldReader.h
 * @brief class to read EnSight Gold ASCII files
 *
 *
 */

#ifndef vtkNewEnSightGoldReader_h
#define vtkNewEnSightGoldReader_h

#include "vtkIOEnSightModule.h" // For export macro
#include "vtkPartitionedDataSetCollectionAlgorithm.h"

class vtkPartitionedDataSetCollection;

class VTKIOENSIGHT_EXPORT vtkNewEnSightGoldReader : public vtkPartitionedDataSetCollectionAlgorithm
{
public:
  static vtkNewEnSightGoldReader* New();
  vtkTypeMacro(vtkNewEnSightGoldReader, vtkPartitionedDataSetCollectionAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkSetStringMacro(CaseFileName);
  vtkGetStringMacro(CaseFileName);

  int CanReadFile(VTK_FILEPATH const char* casefilename);

protected:
  vtkNewEnSightGoldReader();
  ~vtkNewEnSightGoldReader() override;

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  char* CaseFileName;

private:
  vtkNewEnSightGoldReader(const vtkNewEnSightGoldReader&) = delete;
  void operator=(const vtkNewEnSightGoldReader&) = delete;

  struct ReaderImpl;
  ReaderImpl* Impl;
};

#endif
