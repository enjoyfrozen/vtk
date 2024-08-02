// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause
#ifndef vtkRadiossAnimReader_h
#define vtkRadiossAnimReader_h

#include "RadiossAnimDataModel.h"
#include "vtkIORadiossModule.h" // For export macro
#include "vtkPartitionedDataSetCollectionAlgorithm.h"

#include <memory>
#include <string> // For std::string
#include <unordered_map>

VTK_ABI_NAMESPACE_BEGIN

class vtkPoints;
class vtkIntArray;

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

  void ExtractPartPoints(int minCellIndex, int maxCellIndex,
    const std::vector<int>& radiossCellConnectivity,
    const std::vector<float>& radiossPointCoordinates, int numberOfPointsPerCell, vtkPoints* points,
    vtkIntArray* radiossNodeIDs, std::unordered_map<int, vtkIdType>& radiossPointIdToVTKPointIndex);
  void ExtractPartPointData(const RadiossAnimDataModel::Quads& radiossQuads,
    const RadiossAnimDataModel::Nodes& radiossNodes, vtkIntArray* radiossNodeIDs,
    std::vector<vtkSmartPointer<vtkAbstractArray>>& pointDataArrays);
  std::string ExtractPartName(const std::string& modelPartName);

  std::string FileName;
  std::unique_ptr<class RadiossAnimDataModel> RadiossAnimDataModel;
};

VTK_ABI_NAMESPACE_END

#endif
