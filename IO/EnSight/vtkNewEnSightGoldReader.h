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
 * @brief class to read EnSight Gold files
 *
 * vtkNewEnSightGoldReader is a class to read EnSight Gold files into vtk.
 * This reader produces a vtkPartitionedDataSetCollection.
 *
 * This reader is still in progress. Currently there is support for reading uniform,
 * rectilinear, and curvilinear grids. ASCII and C Binary files are both supported by
 * this reader. Fortran binary support will be added in the future.
 */

#ifndef vtkNewEnSightGoldReader_h
#define vtkNewEnSightGoldReader_h

#include "vtkIOEnSightModule.h" // For export macro
#include "vtkPartitionedDataSetCollectionAlgorithm.h"

class vtkDataArraySelection;
class vtkPartitionedDataSetCollection;

class VTKIOENSIGHT_EXPORT vtkNewEnSightGoldReader : public vtkPartitionedDataSetCollectionAlgorithm
{
public:
  static vtkNewEnSightGoldReader* New();
  vtkTypeMacro(vtkNewEnSightGoldReader, vtkPartitionedDataSetCollectionAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Set/Get the case file name.
   */
  vtkSetStringMacro(CaseFileName);
  vtkGetStringMacro(CaseFileName);
  ///@}

  /**
   * Checks version information in the case file to determine if the file
   * can be read by this reader.
   */
  int CanReadFile(VTK_FILEPATH const char* casefilename);

  /**
   * Block (Part in EnSight terminology) selection, to determine which blocks/parts
   * are loaded.
   */
  vtkDataArraySelection* GetBlockSelection();

  /**
   * Overridden to take into account mtimes for vtkDataArraySelection instances.
   */
  vtkMTimeType GetMTime() override;

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
