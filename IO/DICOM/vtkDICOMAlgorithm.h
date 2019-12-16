/*=========================================================================

  Program: DICOM for VTK

  Copyright (c) 2012-2019 David Gobbi
  All rights reserved.
  See Copyright.txt or http://dgobbi.github.io/bsd3.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * \class vtkDICOMAlgorithm
 * \brief Superclass for DICOM image filters
 *
 * This class allows the DICOM meta data to be carried by the VTK pipeline.
 * Subclasses should always call Superclass::RequestInformation() and
 * Superclass::RequestData within their own RequestInformation and
 * RequestData methods.
 */

#ifndef vtkDICOMAlgorithm_h
#define vtkDICOMAlgorithm_h

#include "vtkDICOMConfig.h"   // For configuration details
#include "vtkIODICOMModule.h" // For export macro
#include "vtkThreadedImageAlgorithm.h"

class vtkLookupTable;
class vtkInformationDataObjectKey;
class vtkInformationDoubleVectorKey;
class vtkDICOMMetaData;
class vtkDICOMPerFilePalette;

//----------------------------------------------------------------------------
class VTKIODICOM_EXPORT vtkDICOMAlgorithm : public vtkThreadedImageAlgorithm
{
public:
  vtkTypeMacro(vtkDICOMAlgorithm, vtkThreadedImageAlgorithm);

  //@{
  //! Static method for construction.
  static vtkDICOMAlgorithm* New();

  //! Print information about this object.
  void PrintSelf(ostream& os, vtkIndent indent) override;
  //@}

  //@{
  //! The information key for the meta data.
  static vtkInformationDataObjectKey* META_DATA();

  //! A key to get the patient matrix from the VTK pipeline.
  static vtkInformationDoubleVectorKey* PATIENT_MATRIX();
  //@}

protected:
  vtkDICOMAlgorithm();
  ~vtkDICOMAlgorithm() override;

  //@{
  //! Get the information object that holds the meta data for the given input.
  vtkInformation* GetMetaDataInformation(
    vtkInformationVector** inputVector, int inputPort, int inputConnection);

  //! Copy the meta data from an input connection to an output port.
  /*!
   *  This is called by RequestInformation. To copy the meta data to
   *  all output ports, use "-1" as the output port.
   */
  virtual void CopyMetaDataToOutputInformation(vtkInformationVector** inputVector, int inputPort,
    int inputConnection, vtkInformationVector* outputVector, int outputPort);

  //! Copy the meta data from the output information to the output data.
  /*!
   *  This is called by RequestData.
   */
  virtual void CopyMetaDataToOutputData(vtkInformation* outInfo, vtkDataObject* outData);
  //@}

  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  int RequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;

  void ThreadedRequestData(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector, vtkImageData*** inData, vtkImageData** outData, int ext[6],
    int id) override;

private:
  vtkDICOMAlgorithm(const vtkDICOMAlgorithm&) = delete;
  void operator=(const vtkDICOMAlgorithm&) = delete;
};

#endif // vtkDICOMAlgorithm_h
