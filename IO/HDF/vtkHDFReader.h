/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHDFReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkHDFReader
 * @brief   VTK's HDF format reader.
 *
 */

#ifndef vtkHDFReader_h
#define vtkHDFReader_h

#include "vtkDataSetAlgorithm.h"
#include "vtkIOHDFModule.h" // For export macro

class vtkAbstractArray;
class vtkCallbackCommand;
class vtkDataArraySelection;
class vtkDataSet;
class vtkDataSetAttributes;
class vtkInformationVector;
class vtkInformation;
class vtkCommand;

class VTKIOHDF_EXPORT vtkHDFReader : public vtkDataSetAlgorithm
{
public:
  static vtkHDFReader* New();
  vtkTypeMacro(vtkHDFReader, vtkDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Get/Set the name of the input file.
   */
  vtkSetStringMacro(FileName);
  vtkGetStringMacro(FileName);
  //@}

  /**
   * Test whether the file (type) with the given name can be read by this
   * reader. If the file has a newer version than the reader, we still say
   * we can read the file type and we fail later, when we try to read the file.
   * This enables clients (ParaView) to distinguish between failures when we
   * need to look for another reader and failures when we don't.
   */
  virtual int CanReadFile(const char* name);

  //@{
  /**
   * Get the output as a vtkDataSet pointer.
   */
  vtkDataSet* GetOutputAsDataSet();
  vtkDataSet* GetOutputAsDataSet(int index);
  //@}

  //@{
  /**
   * Get the data array selection tables used to configure which data
   * arrays are loaded by the reader.
   */
  virtual vtkDataArraySelection* GetPointDataArraySelection();
  virtual vtkDataArraySelection* GetCellDataArraySelection();
  //@}

  //@{
  /**
   * Get the number of point or cell arrays available in the input.
   */
  int GetNumberOfPointArrays();
  int GetNumberOfCellArrays();
  //@}

  //@{
  /**
   * Get the name of the point or cell array with the given index in
   * the input.
   */
  /**
   * How many attribute types we have. This always returns 2: point and cell
   * attribute types.
   */
  constexpr static int GetNumberOfAttributeTypes() { return 2; }
  const char* GetPointArrayName(int index);
  const char* GetCellArrayName(int index);
  //@}

  //@{
  /**
   * Get/Set whether the point or cell array with the given name is to
   * be read.
   */
  int GetPointArrayStatus(const char* name);
  int GetCellArrayStatus(const char* name);
  void SetPointArrayStatus(const char* name, int status);
  void SetCellArrayStatus(const char* name, int status);
  //@}

  //@{
  /**
   * Set/get the ErrorObserver for the internal reader
   * This is useful for applications that want to catch error messages.
   */
  void SetReaderErrorObserver(vtkCommand*);
  vtkGetObjectMacro(ReaderErrorObserver, vtkCommand);
  //@}

protected:
  vtkHDFReader();
  ~vtkHDFReader() override;

  // Test if the reader can read a file with the given version number.
  virtual int CanReadFileVersion(int major, int minor);

  // Setup the output with no data available.  Used in error cases.
  virtual void SetupEmptyOutput();

  // Setup the output's information.
  virtual void SetupOutputInformation(vtkInformation* vtkNotUsed(outInfo)) {}

  // Setup the output's data with allocation.
  virtual void SetupOutputData();

  bool Read(vtkInformation* outInfo, vtkImageData* data);
  bool Read(vtkInformation* outInfo, vtkUnstructuredGrid* data);

  // Callback registered with the SelectionObserver.
  static void SelectionModifiedCallback(
    vtkObject* caller, unsigned long eid, void* clientdata, void* calldata);

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestDataObject(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

private:
  vtkHDFReader(const vtkHDFReader&) = delete;
  void operator=(const vtkHDFReader&) = delete;

protected:
  // The input file's name.
  char* FileName;

  // The array selections.
  // in the same order as vtkDataObject::AttributeTypes: POINT, CELL
  vtkDataArraySelection* DataArraySelection[2];

  // The observer to modify this object when the array selections are
  // modified.
  vtkCallbackCommand* SelectionObserver;

  // Whether there was an error reading the file in RequestInformation.
  int InformationError;

  // Whether there was an error reading the file in RequestData.
  int DataError;

  // Whether there was an error reading the HDF.
  int ReadError;

  // For structured data keep track of dimensions empty of cells.  For
  // unstructured data these are always zero.  This is used to support
  // 1-D and 2-D cell data.
  int AxesEmpty[3];
  int WholeExtent[6];
  double Origin[3];
  double Spacing[3];

  vtkDataObject* GetCurrentOutput();
  vtkInformation* GetCurrentOutputInformation();

  class Implementation;
  Implementation* Impl;

private:
  int TimeStepWasReadOnce;

  vtkDataObject* CurrentOutput;
  vtkInformation* CurrentOutputInformation;

private:
  vtkCommand* ReaderErrorObserver;
};

#endif
