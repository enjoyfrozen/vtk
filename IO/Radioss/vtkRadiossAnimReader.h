// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#ifndef vtkRadiossAnimReader_h
#define vtkRadiossAnimReader_h

#include "vtkIORadiossModule.h" // For export macro
#include "vtkPartitionedDataSetCollectionAlgorithm.h"

#include <memory>
#include <string> // For std::string

VTK_ABI_NAMESPACE_BEGIN

class RadiossAnimDataModel;
/**
 * @class vtkRadiossAnimReader
 *
 * A reader for the Radioss and OpenRadioss output Anim file format.
 *
 */
class VTKIORADIOSS_EXPORT vtkRadiossAnimReader : public vtkPartitionedDataSetCollectionAlgorithm
{
public:
  static vtkRadiossAnimReader* New();
  vtkTypeMacro(vtkRadiossAnimReader, vtkPartitionedDataSetCollectionAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  ///@{
  /**
   * Specifies the name of the .anim file to be loaded.
   */
  vtkSetMacro(FileName, std::string);
  vtkGetMacro(FileName, std::string);
  ///@}

protected:
  vtkRadiossAnimReader();
  ~vtkRadiossAnimReader() override;

  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkRadiossAnimReader(const vtkRadiossAnimReader&) = delete;
  void operator=(const vtkRadiossAnimReader&) = delete;

  std::string FileName;
  std::unique_ptr<class RadiossAnimDataModel> RadiossAnimDataModel;
};

VTK_ABI_NAMESPACE_END

#endif
