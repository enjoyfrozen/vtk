/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkFidesReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef vtkOMFReader_h
#define vtkOMFReader_h

#include "vtkDataObjectAlgorithm.h"
#include "vtkIOOMFModule.h" // For export macro

class vtkDataArraySelection;
class vtkStringArray;

/**
 * @class   vtkOMFReader
 * @brief   Read Open Mining Format files
 *
 * vtkOMFReader reads OMF files. Details about the OMF format can be
 * found at https://omf.readthedocs.io/en/stable/index.html.
 * The reader outputs a vtkPartitionedDataSetCollection, where each
 * vtkPartitionedDataSet is one OMF element (point set, line set,
 * surface, or volume).
 */
class VTKIOOMF_EXPORT vtkOMFReader : public vtkDataObjectAlgorithm
{
  vtkTypeMacro(vtkOMFReader, vtkDataObjectAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Construct a new reader instance.
   */
  static vtkOMFReader* New();

  //@{
  /**
   * Accessor for name of the OMF file to read
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  //@{
  /**
   * Data element selection to determine which data elements in the OMF
   * file should be loaded. By default all elements' status are set to true/enabled,
   * so they will be loaded.
   */
  bool GetDataElementArrayStatus(const char* name);
  void SetDataElementArrayStatus(const char* name, int status);
  int GetNumberOfDataElementArrays();
  const char* GetDataElementArrayName(int index);
  vtkDataArraySelection* GetDataElementArraySelection();
  //@}

  /**
   * Overridden to take into account mtimes for vtkDataArraySelection instances.
   */
  vtkMTimeType GetMTime() override;

protected:
  vtkOMFReader();
  ~vtkOMFReader() override;

  int RequestDataObject(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  int FillOutputPortInformation(int port, vtkInformation* info) override;

  char* FileName;

private:
  vtkOMFReader(const vtkOMFReader&) = delete;
  void operator=(const vtkOMFReader&) = delete;

  struct ReaderImpl;
  ReaderImpl* Impl;
};

#endif
