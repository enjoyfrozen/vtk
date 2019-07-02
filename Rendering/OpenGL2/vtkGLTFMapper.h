/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGLTFMapper.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkGLTFMapper
 * @brief   Mapper for composite datasets issued from the glTF 2.0 reader.
 *
 * vtkGLTFMapper is a mapper for the rendering of glTF 2.0 datasets.
 * It enables glTF 2.0 features that can not be exploited with standard mappers, such as:
 * * Animated scene graph on the GPU
 * * Animated model Skinning on the GPU
 * * Animated model Morphing on the GPU
 * * Automatic and exhaustive glTF 2.0 material application and support
 * * Alpha masking
 * * Usage of two sets of texture coordinates
 *
 * How it works:
 * The vtkGLTFReader filter uses field data to save skinning, morphing and material information to
 * its output dataset. vtkGLTFMapper will look for this field data in its input dataset, and render
 * the models accordingly.
 *
 * How to use:
 * * Use vtkGLTFReader to read from a glTF 2.0 file.
 * * Use vtkGLTFReader::GetGLTFTexture() to create the vtkTexture objects from the glTF model's
 * images.
 * * Pass these textures to this mapper, using vtkGLTFMapper::SetTextures(). It is important that
 * the order of textures is identical to the order from vtkGLTFReader.
 * * Use this mapper to render the reader's output dataset
 *
 * @sa vtkGLTFReader vtkGLTFImporter vtkCompositePolyDataMapper2
 */

#ifndef vtkGLTFMapper_h
#define vtkGLTFMapper_h

#include "vtkCompositePolyDataMapper2.h"
#include "vtkRenderingOpenGL2Module.h" // For export macro
#include "vtkSmartPointer.h"           // For SmartPointer

#include <vector> // For vector

class vtkTexture;

class VTKRENDERINGOPENGL2_EXPORT vtkGLTFMapper : public vtkCompositePolyDataMapper2
{
public:
  static vtkGLTFMapper* New();
  vtkTypeMacro(vtkGLTFMapper, vtkCompositePolyDataMapper2);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Specifiy the texture that this mapper should use for rendering.
   * The textures and their indices should match the textures that would be read from the glTF
   * document being rendered.
   * vtkGLTFReader has accessors for those textures.
   */
  void SetTextures(const std::vector<vtkSmartPointer<vtkTexture> >& textures);

protected:
  vtkCompositeMapperHelper2* CreateHelper() override;

  vtkGLTFMapper();
  ~vtkGLTFMapper() = default;

  std::vector<vtkSmartPointer<vtkTexture> > Textures;

  /**
   Overriden to create the helpers we need.
   */
  void Render(vtkRenderer* ren, vtkActor* act) override;

  /**
   * Copy mapper values to the helper.
   */
  void CopyMapperValuesToHelper(vtkCompositeMapperHelper2* helper) override;

private:
  vtkGLTFMapper(const vtkGLTFMapper&) = delete;
  void operator=(const vtkGLTFMapper&) = delete;
};

#endif
