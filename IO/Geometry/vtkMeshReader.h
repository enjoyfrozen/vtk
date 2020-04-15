/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMeshReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkMeshReader
 * @brief   reads a dataset in ".mesh" file format (NetGen, others)
 *
 */
#ifndef vtkMeshReader_h
#define vtkMeshReader_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkMultiBlockDataSetAlgorithm.h"

class VTKIOGEOMETRY_EXPORT vtkMeshReader : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkMeshReader* New();
  vtkTypeMacro(vtkMeshReader, vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Specify the file name of the Fluent case file to read.
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  virtual int CanReadFile(const char* fname);

protected:
  vtkMeshReader();
  ~vtkMeshReader() override;
  // int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

  char* FileName;

private:
  vtkMeshReader(const vtkMeshReader&) = delete;
  void operator=(const vtkMeshReader&) = delete;
};
#endif // vtkMeshReader_h
