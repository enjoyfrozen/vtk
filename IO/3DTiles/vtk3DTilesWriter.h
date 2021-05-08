/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtk3DTilesWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtk3DTilesWriter
 * @brief   Converts a vtkMultiBlockDataSet (as created by vtkCityGMLReader) into
 *          3D Tiles format.
 */

#ifndef vtk3DTilesWriter_h
#define vtk3DTilesWriter_h

#include "vtkIO3DTilesModule.h" // For export macro
#include "vtkWriter.h"

class VTKIO3DTILES_EXPORT vtk3DTilesWriter : public vtkWriter
{
public:
  static vtk3DTilesWriter* New();
  void PrintSelf(ostream& os, vtkIndent indent) override;
  vtkTypeMacro(vtk3DTilesWriter, vtkWriter);

  ///@{
  /**
   * Accessor for name of the directory where 3DTiles data is written
   */
  vtkSetStringMacro(DirectoryName);
  vtkGetStringMacro(DirectoryName);
  ///@}

  ///@{
  /**
   * Path used to prefix all texture paths stored as fields in the input data.
   * @see vtkCityGMLReader
   */
  vtkSetStringMacro(TexturePath);
  vtkGetStringMacro(TexturePath);
  ///@}

  //@{
  /**
   * Data coordinates are relative to this origin. To get the actuall
   * coordinates data has to be translated with the Origin.  Note this
   * is an input / output parameter. The value passed as input
   * parameter is modified and the data is translated such that the
   * min corner of the data bounding box is at position 0.
   */
  vtkSetVector3Macro(Origin, double);
  vtkGetVector3Macro(Origin, double);
  //@}

  //@{
  /**
   * Save textures as part of the 3D Tiles dataset. Default true.
   * Otherwise save only the mesh.
   */
  vtkSetMacro(SaveTextures, bool);
  vtkGetMacro(SaveTextures, bool);
  vtkBooleanMacro(SaveTextures, bool);
  //@

  //@{
  /**
   * Save GLTF (B3DM) files as part of the 3D Tiles dataset. Default true.
   * Otherwise save only the tileset (JSON) file. This is mainly used for
   * debugging.
   */
  vtkSetMacro(SaveGLTF, bool);
  vtkGetMacro(SaveGLTF, bool);
  vtkBooleanMacro(SaveGLTF, bool);
  //@

  //@{
  /**
   * Maximum number of buildings per tile. Default is 100.
   */
  vtkSetMacro(NumberOfBuildingsPerTile, int);
  vtkGetMacro(NumberOfBuildingsPerTile, int);
  vtkBooleanMacro(NumberOfBuildingsPerTile, int);
  //@

  //@{
  /**
   * Set the UTM zone and hemisphere.
   */
  vtkSetMacro(UTMZone, int);
  vtkSetMacro(UTMHemisphere, char);
  vtkGetMacro(UTMZone, int);
  vtkGetMacro(UTMHemisphere, char);
  //@

  ///@{
  /**
   * Set the spatial reference system (SRC) also known as coordinate reference system (CRS)
   * such as EPSG:2263. This takes precedence over the UTM zone and hemisphere.
   */
  vtkSetStringMacro(SrsName);
  vtkGetStringMacro(SrsName);
  ///@}

protected:
  vtk3DTilesWriter();
  ~vtk3DTilesWriter() override;

  // Only accepts vtkMultiBlockData
  int FillInputPortInformation(int port, vtkInformation* info) override;

  // Implementation of Write()
  void WriteData() override;

  char* DirectoryName;
  char* TexturePath;
  double Origin[3];
  bool SaveTextures;
  bool SaveGLTF;
  int NumberOfBuildingsPerTile;
  int UTMZone;
  char UTMHemisphere;
  char* SrsName;

private:
  vtk3DTilesWriter(const vtk3DTilesWriter&) = delete;
  void operator=(const vtk3DTilesWriter&) = delete;
};

#endif // vtk3DTilesWriter_h
