/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGLTFMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGLTFMapper.h"

#include "vtkActor.h"
#include "vtkCellData.h"
#include "vtkCommand.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataArray.h"
#include "vtkDataObjectTreeIterator.h"
#include "vtkDoubleArray.h"
#include "vtkFieldData.h"
#include "vtkFloatArray.h"
#include "vtkHardwareSelector.h"
#include "vtkImageAppendComponents.h"
#include "vtkImageData.h"
#include "vtkImageExtractComponents.h"
#include "vtkImageResize.h"
#include "vtkMatrix4x4.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGL.h"
#include "vtkOpenGLCellToVTKCellMap.h"
#include "vtkOpenGLHelper.h"
#include "vtkOpenGLIndexBufferObject.h"
#include "vtkOpenGLTexture.h"
#include "vtkOpenGLVertexArrayObject.h"
#include "vtkOpenGLVertexBufferObject.h"
#include "vtkOpenGLVertexBufferObjectGroup.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkScalarsToColors.h"
#include "vtkShader.h"
#include "vtkShaderProgram.h"
#include "vtkTexture.h"

#include <array>
#include <sstream>

#include "vtkCompositePolyDataMapper2Internal.h"

namespace
{
typedef std::pair<vtkTexture*, std::string> texinfo;

//----------------------------------------------------------------------------
// Replacement for std::to_string as it is not supported by certain compilers
template<typename T>
std::string value_to_string(const T& val)
{
  std::ostringstream ss;
  ss << val;
  return ss.str();
}

//-----------------------------------------------------------------------------
// Contains the different possible texture indices, extraced from a polyData's field data
typedef struct GLTFMaterialTextures
{
  int BaseColorTextureIndex = -1;
  int MaterialTextureIndex = -1;
  int OcclusionTextureIndex = -1;
  int EmissiveTextureIndex = -1;
  int NormalTextureIndex = -1;
} GLTFMaterialTextures;

//-----------------------------------------------------------------------------
// Contains different glTF material properties, extracted from a polyData's field data
typedef struct GLTFMaterialValues
{
  // Values for vtkProperty
  std::array<double, 4> BaseColorFactor = { { 1.0, 1.0, 1.0, 1.0 } };
  float MetallicFactor = 1.0;
  float RoughnessFactor = 1.0;
  float OcclusionStrength = 1.0;
  std::array<double, 3> EmissiveFactor = { { 0.0, 0.0, 0.0 } };
  float NormalScale = 1.0;

  // Alpha blending configuration
  bool HasOpaqueAlphaMode = false;
  bool HasBlendAlphaMode = false;
  bool HasMaskAlphaMode = false;
  float AlphaCutOff = 1.0;

  bool DoubleSided = false;

  // Whether the second set of texture coordinates should be used
  bool BaseColorUseAlternateUVSet = false;
  bool MaterialUseAlternateUVSet = false;
  bool OcclusionUseAlternateUVSet = false;
  bool EmissiveUseAlternateUVSet = false;
  bool NormalUseAlternateUVSet = false;
  bool HasAlternateUVSet()
  {
    return BaseColorUseAlternateUVSet || MaterialUseAlternateUVSet || OcclusionUseAlternateUVSet ||
      EmissiveUseAlternateUVSet || NormalUseAlternateUVSet;
  }

  bool IsORMTextureCreated = false;
} GLTFMaterialValues;

//-----------------------------------------------------------------------------
class vtkGLTFMapperHelper : public vtkCompositeMapperHelper2
{
public:
  vtkTypeMacro(vtkGLTFMapperHelper, vtkCompositeMapperHelper2);
  static vtkGLTFMapperHelper* New()
  {
    vtkGLTFMapperHelper* ret = new vtkGLTFMapperHelper;
    ret->InitializeObjectBase();
    return ret;
  }

  void PrintSelf(ostream& os, vtkIndent indent) override
  {
    this->Superclass::PrintSelf(os, indent);
    os << indent << "Skinning: " << (this->EnableSkinning ? "On" : "Off") << "\n";
    if (this->EnableSkinning)
    {
      os << indent << "Number of joints: " << this->NumberOfJoints << "\n";
    }

    os << indent << "Morphing: " << (this->EnableMorphing ? "On" : "Off") << "\n";
    if (this->EnableMorphing)
    {
      os << indent << "Number of position morph targets: " << this->NumberOfPositionTargets << "\n";
      os << indent << "Number of normal morph targets: " << this->NumberOfNormalTargets << "\n";
      os << indent << "Number of tangent morph targets: " << this->NumberOfTangentTargets << "\n";
    }

    os << indent << "Has normals: " << (this->HasNormals ? "Yes" : "No") << "\n";
    os << indent << "Has tangents: " << (this->HasTangents ? "Yes" : "No") << "\n";
    os << indent << "Has scalars: " << (this->HasScalars ? "Yes" : "No") << "\n";

    os << indent << this->Textures.size() << " textures:"
       << "\n";
    for (auto texture : this->Textures)
    {
      texture->PrintSelf(os, indent.GetNextIndent());
    }
  }

  void SetTextures(const std::vector<vtkSmartPointer<vtkTexture> >& textures);

  // Helper configuration parameters
  bool EnableSkinning = false;
  int NumberOfJoints = 0;

  bool EnableMorphing = false;
  unsigned int NumberOfPositionTargets = 0;
  unsigned int NumberOfNormalTargets = 0;
  unsigned int NumberOfTangentTargets = 0;

  bool HasNormals = false;
  bool HasTangents = false;
  bool HasScalars = false;

  GLTFMaterialTextures MaterialTextures;
  GLTFMaterialValues MaterialValues;

  vtkSmartPointer<vtkProperty> BaseProperty;

protected:
  vtkGLTFMapperHelper()
  {
    this->SetVBOShiftScaleMethod(vtkOpenGLVertexBufferObject::DISABLE_SHIFT_SCALE);
  }

  ~vtkGLTFMapperHelper() override = default;

  /**
   * Add glTF related tags to vtkOpenGLPolyDataMapper's vertex shader
   */
  void AddGLTFVertexShaderTags(std::string& VSSource);

  /**
   * Adds skinning implementation to the vertex shader
   */
  void AddSkinningToShader(std::string& VSSource);

  /**
   * Adds morphing implementation to the vertex shader
   */
  void AddMorphingToShader(std::string& VSSource);

  /**
   * Adds the necessary glTF modifications to vtkOpenGLPolyDataMapper's vertex and fragment shaders
   */
  void ReplaceShaderValues(
    std::map<vtkShader::Type, vtkShader*> shaders, vtkRenderer*, vtkActor*) override;

  /**
   * Applies glTF-related values to the actor's vtkProperty
   */
  void RenderPieceStart(vtkRenderer* ren, vtkActor* actor) override;

  /**
   * Restores the actor's vtkProperty to its original state
   */
  void RenderPieceFinish(vtkRenderer* ren, vtkActor* actor) override;

  /**
   * Sets glTF-related uniforms before rendering
   */
  void SetShaderValues(
    vtkShaderProgram* prog, vtkCompositeMapperHelperData* hdata, size_t primOffset) override;

  /**
   * Uploads glTF-related buffers before rendering
   */
  void AppendOneBufferObject(vtkRenderer* ren, vtkActor* act, vtkCompositeMapperHelperData* hdata,
    vtkIdType& flat_index, std::vector<unsigned char>& colors, std::vector<float>& norms) override;

  /**
   * The array of textures that this mappers uses. Images and indices should be identical to the
   * glTF document's image data.
   */
  std::vector<vtkSmartPointer<vtkTexture> > Textures;

private:
  vtkGLTFMapperHelper(const vtkGLTFMapperHelper&) = delete;
  void operator=(const vtkGLTFMapperHelper&) = delete;
};

typedef std::map<vtkPolyData*, vtkCompositeMapperHelperData*>::iterator dataIter;
typedef std::map<const std::string, vtkCompositeMapperHelper2*>::iterator helpIter;

//-----------------------------------------------------------------------------
// reads a 16-component tuple from a dataArray, and convert it to a float vector to be uploaded as
// glsl uniform mat4
std::vector<float> Matrix4x4FromVtkDataArrayToFloatVector(vtkDataArray* array)
{
  std::vector<float> matrix(16, 0);
  if (array == nullptr)
  {
    vtkErrorWithObjectMacro(
      nullptr, "Error reading matrix from vtkDataArray: The array doesn't exist.");
    return matrix;
  }
  if (array->GetNumberOfValues() < 16)
  {
    vtkErrorWithObjectMacro(nullptr, "Error reading matrix from vtkDataArray: Not enough values.");
    return matrix;
  }
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      matrix[i * 4 + j] = static_cast<float>(*(array->GetTuple(j * 4 + i)));
    }
  }
  return matrix;
}

//-----------------------------------------------------------------------------
// Checks whether the specified texture exists, with warnings if textures are missing.
bool CheckForValidTextureIndex(
  int textureIndex, const std::vector<vtkSmartPointer<vtkTexture> >& textures)
{
  if (textureIndex >= static_cast<int>(textures.size()))
  {
    vtkWarningWithObjectMacro(nullptr,
      "Cannot access the specified texture at index "
        << textureIndex
        << ". Consider using vtkGLTFMapper::SetTexture() to set the "
           "correct textures.");
    return false;
  }
  return textureIndex >= 0;
}

//-----------------------------------------------------------------------------
// Reads material values from vtkFieldData
void FieldDataToMaterialValues(vtkSmartPointer<vtkFieldData> fieldData,
  vtkSmartPointer<vtkPointData> pointData, GLTFMaterialValues& material)
{
  if (fieldData->HasArray("BaseColorMultiplier"))
  {
    fieldData->GetArray("BaseColorMultiplier")->GetTuple(0, material.BaseColorFactor.data());
    if (fieldData->GetArray("BaseColorMultiplier")->GetNumberOfComponents() == 3)
    {
      material.BaseColorFactor[3] = 1.0;
    }
  }
  if (fieldData->HasArray("OcclusionRoughnessMetallic"))
  {
    auto tuple = fieldData->GetArray("OcclusionRoughnessMetallic")->GetTuple3(0);
    material.OcclusionStrength = tuple[0];
    material.MetallicFactor = tuple[1];
    material.RoughnessFactor = tuple[2];
  }
  if (fieldData->HasArray("EmissiveMultiplier"))
  {
    fieldData->GetArray("EmissiveMultiplier")->GetTuple(0, material.EmissiveFactor.data());
  }
  if (fieldData->HasArray("NormalMultiplier"))
  {
    material.NormalScale = fieldData->GetArray("NormalMultiplier")->GetTuple3(0)[0];
  }
  if (fieldData->HasArray("ForceOpaque"))
  {
    material.HasOpaqueAlphaMode = true;
  }
  else if (fieldData->HasArray("AlphaCutoff"))
  {
    material.HasMaskAlphaMode = true;
    material.AlphaCutOff = fieldData->GetArray("AlphaCutoff")->GetTuple1(0);
  }
  else
  {
    material.HasBlendAlphaMode = true;
  }
  if (fieldData->HasArray("DoubleSided"))
  {
    material.DoubleSided = fieldData->GetArray("DoubleSided")->GetTuple1(0) == 1;
  }

  // When to use the second set of texture coordinates.
  if (pointData->HasArray("TEXCOORD_1"))
  {
    if (fieldData->HasArray("BaseColorTexCoordIndex"))
    {
      material.BaseColorUseAlternateUVSet =
        fieldData->GetArray("BaseColorTexCoordIndex")->GetTuple1(0) != 0;
    }
    if (fieldData->HasArray("EmissiveTexCoordIndex"))
    {
      material.EmissiveUseAlternateUVSet =
        fieldData->GetArray("EmissiveTexCoordIndex")->GetTuple1(0) != 0;
    }
    if (fieldData->HasArray("MetallicRoughnessTexCoordIndex"))
    {
      material.MaterialUseAlternateUVSet =
        fieldData->GetArray("MetallicRoughnessTexCoordIndex")->GetTuple1(0) != 0;
    }
    if (fieldData->HasArray("NormalTexCoordIndex"))
    {
      material.NormalUseAlternateUVSet =
        fieldData->GetArray("NormalTexCoordIndex")->GetTuple1(0) != 0;
    }
    if (fieldData->HasArray("OcclusionTexCoordIndex"))
    {
      material.OcclusionUseAlternateUVSet =
        fieldData->GetArray("OcclusionTexCoordIndex")->GetTuple1(0) != 0;
    }
  }
}

//-----------------------------------------------------------------------------
// Reads texture indices from vtkFieldData
void FieldDataToMaterialTextures(
  vtkSmartPointer<vtkFieldData> fieldData, GLTFMaterialTextures& material)
{
  if (fieldData == nullptr)
  {
    return;
  }
  if (fieldData->HasArray("BaseColorTextureIndex"))
  {
    material.BaseColorTextureIndex = fieldData->GetArray("BaseColorTextureIndex")->GetTuple1(0);
  }
  if (fieldData->HasArray("MetallicRoughnessTextureIndex"))
  {
    material.MaterialTextureIndex =
      fieldData->GetArray("MetallicRoughnessTextureIndex")->GetTuple1(0);
  }
  if (fieldData->HasArray("OcclusionTextureIndex"))
  {
    material.OcclusionTextureIndex = fieldData->GetArray("OcclusionTextureIndex")->GetTuple1(0);
  }
  if (fieldData->HasArray("EmissiveTextureIndex"))
  {
    material.EmissiveTextureIndex = fieldData->GetArray("EmissiveTextureIndex")->GetTuple1(0);
  }
  if (fieldData->HasArray("NormalTextureIndex"))
  {
    material.NormalTextureIndex = fieldData->GetArray("NormalTextureIndex")->GetTuple1(0);
  }
}

//-----------------------------------------------------------------------------
void ApplyMaterialValuesToVTKPRoperty(
  vtkSmartPointer<vtkProperty> property, GLTFMaterialValues& materialValues)
{
  if (property == nullptr)
  {
    return;
  }

  property->SetColor(materialValues.BaseColorFactor.data());
  property->SetOpacity(materialValues.BaseColorFactor[3]);
  property->SetEmissiveFactor(materialValues.EmissiveFactor.data());
  property->SetOcclusionStrength(materialValues.OcclusionStrength);
  property->SetMetallic(materialValues.MetallicFactor);
  property->SetRoughness(materialValues.RoughnessFactor);
  property->SetNormalScale(materialValues.NormalScale);
  property->SetBackfaceCulling(!materialValues.DoubleSided);
}

//-----------------------------------------------------------------------------
void ApplyMaterialTexturesToVTKProperty(vtkSmartPointer<vtkProperty> property,
  const GLTFMaterialTextures& materialTextures, GLTFMaterialValues& materialValues,
  std::vector<vtkSmartPointer<vtkTexture> >& textures)
{
  if (property == nullptr)
  {
    return;
  }

  property->SetInterpolationToPBR();

  property->RemoveTexture("albedoTex");
  property->RemoveTexture("emissiveTex");
  property->RemoveTexture("materialTex");
  property->RemoveTexture("normalTex");
  property->RemoveTexture("occlusionTex");

  bool useMR = CheckForValidTextureIndex(materialTextures.MaterialTextureIndex, textures) &&
    !materialValues.MaterialUseAlternateUVSet;
  bool useOcclusion = CheckForValidTextureIndex(materialTextures.OcclusionTextureIndex, textures) &&
    !materialValues.OcclusionUseAlternateUVSet;

  // While glTF 2.0 uses two different textures for Ambient Occlusion and Metallic/Roughness
  // values, VTK only uses one, so we merge both textures into one.
  // If an Ambient Occlusion texture is present, we merge its first channel into the
  // metallic/roughness texture (AO is r, Roughness g and Metallic b) If no Ambient
  // Occlusion texture is present, we need to fill the metallic/roughness texture's first
  // channel with 255
  if (!materialValues.IsORMTextureCreated)
  {

    if (useOcclusion)
    {
      auto aoTex = textures[materialTextures.OcclusionTextureIndex];
      auto aoImage = vtkImageData::SafeDownCast(aoTex->GetInputDataObject(0, 0));
      if (useMR)
      {
        auto pbrTex = textures[materialTextures.MaterialTextureIndex];
        auto pbrImage = vtkImageData::SafeDownCast(pbrTex->GetInputDataObject(0, 0));
        // Fill red channel with AO values
        vtkNew<vtkImageExtractComponents> redAO;
        // If sizes are different, resize the AO texture to the R/M texture's size
        std::array<vtkIdType, 3> aoSize = { { 0 } };
        std::array<vtkIdType, 3> pbrSize = { { 0 } };
        aoImage->GetDimensions(aoSize.data());
        pbrImage->GetDimensions(pbrSize.data());
        // compare dimensions
        if (aoSize != pbrSize)
        {
          vtkNew<vtkImageResize> resize;
          resize->SetInputData(aoImage);
          resize->SetOutputDimensions(pbrSize[0], pbrSize[1], pbrSize[2]);
          resize->Update();
          redAO->SetInputConnection(resize->GetOutputPort(0));
        }
        else
        {
          redAO->SetInputData(aoImage);
        }
        redAO->SetComponents(0);
        vtkNew<vtkImageExtractComponents> gbPbr;
        gbPbr->SetInputData(pbrImage);
        gbPbr->SetComponents(1, 2);
        vtkNew<vtkImageAppendComponents> append;
        append->AddInputConnection(redAO->GetOutputPort());
        append->AddInputConnection(gbPbr->GetOutputPort());
        append->SetOutput(pbrImage);
        append->Update();
        pbrTex->SetInputData(pbrImage);
      }
      else
      {
        aoImage->GetPointData()->GetScalars()->FillComponent(1, 255);
        aoImage->GetPointData()->GetScalars()->FillComponent(2, 255);
      }
    }
    else if (useMR)
    {
      auto pbrTex = textures[materialTextures.MaterialTextureIndex];
      auto pbrImage = vtkImageData::SafeDownCast(pbrTex->GetInputDataObject(0, 0));
      pbrImage->GetPointData()->GetScalars()->FillComponent(0, 255);
    }
    materialValues.IsORMTextureCreated = true;
  }

  if (CheckForValidTextureIndex(materialTextures.BaseColorTextureIndex, textures) &&
    !materialValues.BaseColorUseAlternateUVSet)
  {
    textures[materialTextures.BaseColorTextureIndex]->SetUseSRGBColorSpace(true);
    property->SetBaseColorTexture(textures[materialTextures.BaseColorTextureIndex]);
  }
  if (CheckForValidTextureIndex(materialTextures.EmissiveTextureIndex, textures) &&
    !materialValues.EmissiveUseAlternateUVSet)
  {
    textures[materialTextures.EmissiveTextureIndex]->SetUseSRGBColorSpace(true);
    property->SetEmissiveTexture(textures[materialTextures.EmissiveTextureIndex]);
  }
  if (useMR)
  {
    property->SetORMTexture(textures[materialTextures.MaterialTextureIndex]);
  }
  else if (useOcclusion)
  {
    property->SetORMTexture(textures[materialTextures.OcclusionTextureIndex]);
  }
  if (CheckForValidTextureIndex(materialTextures.NormalTextureIndex, textures) &&
    !materialValues.NormalUseAlternateUVSet)
  {
    property->SetNormalTexture(textures[materialTextures.NormalTextureIndex]);
  }
}

//-----------------------------------------------------------------------------
int CountNumberOfAttributeTargets(
  vtkSmartPointer<vtkPointData> pointData, const std::string& attributeSuffix)
{
  int count = 0;
  while (pointData->HasArray(("target" + value_to_string(count) + attributeSuffix).c_str()))
  {
    ++count;
  }
  return count;
}
}

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkGLTFMapper);

//----------------------------------------------------------------------------
vtkGLTFMapper::vtkGLTFMapper()
{
  this->SetVBOShiftScaleMethod(vtkOpenGLVertexBufferObject::DISABLE_SHIFT_SCALE);
}

//----------------------------------------------------------------------------
void vtkGLTFMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Number of textures: " << this->Textures.size() << "\n";
  for (auto texture : this->Textures)
  {
    texture->PrintSelf(os, indent.GetNextIndent());
  }
}

//----------------------------------------------------------------------------
void vtkGLTFMapperHelper::SetTextures(const std::vector<vtkSmartPointer<vtkTexture> >& textures)
{
  this->Textures = textures;
}

//----------------------------------------------------------------------------
void vtkGLTFMapperHelper::SetShaderValues(
  vtkShaderProgram* prog, vtkCompositeMapperHelperData* hdata, size_t primOffset)
{
  this->Superclass::SetShaderValues(prog, hdata, primOffset);

  auto fieldData = hdata->Data->GetFieldData();
  auto pointData = hdata->Data->GetPointData();

  // Global node transform. Always present.
  auto globalTransformArray = fieldData->GetArray("globalTransform");
  if (globalTransformArray)
  {
    auto globalTransform = Matrix4x4FromVtkDataArrayToFloatVector(globalTransformArray);
    if (!prog->SetUniformMatrix4x4("glTFNodeTransform", globalTransform.data()))
    {
      vtkWarningMacro("Could not set uniform glTFNodeTransform");
    }
    // Compute and pass inverse transpose transform for normals and tangents
    std::vector<double> normalMatrixData(globalTransform.begin(), globalTransform.end());
    vtkMatrix4x4::Transpose(normalMatrixData.data(), normalMatrixData.data());
    vtkMatrix4x4::Invert(normalMatrixData.data(), normalMatrixData.data());
    if (!prog->SetUniformMatrix4x4("glTFNodeNormalTransform",
          std::vector<float>(normalMatrixData.begin(), normalMatrixData.end()).data()))
    {
      vtkWarningMacro("Could not set uniform glTFNodeNormalTransform");
    }
  }

  // Read current morphing weights from fieldData and pass them as uniform
  if (this->EnableMorphing)
  {
    std::vector<float> weightsVector;
    if (fieldData->HasArray("morphingWeights"))
    {
      auto weightsArray = fieldData->GetArray("morphingWeights");
      weightsVector = std::vector<float>(weightsArray->GetNumberOfValues());
      for (int i = 0; i < weightsArray->GetNumberOfValues(); i++)
      {
        weightsVector[i] = weightsArray->GetTuple1(i);
      }
    }
    else
    {
      weightsVector = std::vector<float>(4, 0);
    }
    prog->SetUniform1fv(
      "morphingWeights", static_cast<int>(weightsVector.size()), weightsVector.data());
  }

  // Read current joint matrices from fieldData and pass them as uniform mat4[numberOfJoints]
  if (this->EnableSkinning)
  {
    std::vector<float> jointMatrixData;
    jointMatrixData.reserve(16 * this->NumberOfJoints);
    for (int i = 0; i < this->NumberOfJoints; i++)
    {
      const std::string arrayName = "jointMatrix_" + value_to_string(i);
      auto jointMatrixArray = fieldData->GetArray(arrayName.c_str());
      std::vector<float> matrixVector = Matrix4x4FromVtkDataArrayToFloatVector(jointMatrixArray);
      jointMatrixData.insert(jointMatrixData.end(), matrixVector.begin(), matrixVector.end());
    }

    if (!prog->SetUniformMatrix4x4v("jointMatrices", this->NumberOfJoints, jointMatrixData.data()))
    {
      vtkWarningMacro("Could not set uniform jointMatrices");
    }

    if (this->HasNormals)
    {
      std::vector<float> jointNormalMatrixData;
      jointNormalMatrixData.reserve(16 * this->NumberOfJoints);
      for (int i = 0; i < this->NumberOfJoints; i++)
      {
        const std::string arrayName = "jointNormalMatrix_" + value_to_string(i);
        auto jointNormalMatrixArray = fieldData->GetArray(arrayName.c_str());
        std::vector<float> matrixVector =
          Matrix4x4FromVtkDataArrayToFloatVector(jointNormalMatrixArray);
        jointNormalMatrixData.insert(
          jointNormalMatrixData.end(), matrixVector.begin(), matrixVector.end());
      }

      if (!prog->SetUniformMatrix4x4v(
            "jointNormalMatrices", this->NumberOfJoints, jointNormalMatrixData.data()))
      {
        vtkWarningMacro("Could not set uniform jointNormalMatrices");
      }
    }
  }

  // Fragment shader multipliers and alpha cutoff
  FieldDataToMaterialValues(fieldData, pointData, this->MaterialValues);
  prog->SetUniform3f("diffuseColorUniform", this->MaterialValues.BaseColorFactor.data());
  prog->SetUniform3f("emissiveFactorUniform", this->MaterialValues.EmissiveFactor.data());
  prog->SetUniformf("metallicUniform", this->MaterialValues.MetallicFactor);
  prog->SetUniformf("roughnessUniform", this->MaterialValues.RoughnessFactor);
  prog->SetUniformf("opacityUniform", this->MaterialValues.BaseColorFactor[3]);
  if (this->MaterialTextures.OcclusionTextureIndex >= 0)
  {
    prog->SetUniformf("aoStrengthUniform", this->MaterialValues.OcclusionStrength);
  }
  if (this->MaterialTextures.NormalTextureIndex >= 0)
  {
    prog->SetUniformf("normalScaleUniform", this->MaterialValues.NormalScale);
  }
}

//-----------------------------------------------------------------------------
void vtkGLTFMapper::CopyMapperValuesToHelper(vtkCompositeMapperHelper2* helper)
{
  vtkCompositePolyDataMapper2::CopyMapperValuesToHelper(helper);
  vtkGLTFMapperHelper::SafeDownCast(helper)->SetTextures(this->Textures);
  helper->SetColorModeToDirectScalars();
  helper->SetInterpolateScalarsBeforeMapping(true);
}

//-----------------------------------------------------------------------------
void vtkGLTFMapperHelper::RenderPieceStart(vtkRenderer* ren, vtkActor* actor)
{
  if (this->BaseProperty == nullptr)
  {
    // Save the actor's current property
    this->BaseProperty = vtkSmartPointer<vtkProperty>::New();
    this->BaseProperty->DeepCopy(actor->GetProperty());
  }

  // Set textures and values to vtkProperty
  ApplyMaterialTexturesToVTKProperty(
    actor->GetProperty(), this->MaterialTextures, this->MaterialValues, this->Textures);
  ApplyMaterialValuesToVTKPRoperty(actor->GetProperty(), this->MaterialValues);
  this->Superclass::RenderPieceStart(ren, actor);
  actor->GetProperty()->Render(actor, ren);
}

//-----------------------------------------------------------------------------
void vtkGLTFMapperHelper::RenderPieceFinish(vtkRenderer* ren, vtkActor* actor)
{
  actor->GetProperty()->PostRender(actor, ren);
  this->Superclass::RenderPieceFinish(ren, actor);

  // Restore property
  actor->GetProperty()->DeepCopy(this->BaseProperty);
}

//-----------------------------------------------------------------------------
void vtkGLTFMapperHelper::AddGLTFVertexShaderTags(std::string& VSSource)
{
  const std::string VSDec = "//VTK::Picking::Dec";
  const std::string VSImpl = "//VTK::Clip::Impl";

  vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::NodeUniforms\n\n" + VSDec);
  if (this->EnableSkinning)
  {
    vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::SkinningUniforms\n\n" + VSDec);
    vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::SkinningAttributes\n\n" + VSDec);
    vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::ComputeSkinningMatrix\n\n" + VSDec);
    vtkShaderProgram::Substitute(
      VSSource, VSDec, "//VTK::GLTF::ComputeSkinningNormalMatrix\n\n" + VSDec);
  }
  if (this->EnableMorphing)
  {
    vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::MorphingUniforms\n\n" + VSDec);
    vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::MorphTargets\n\n" + VSDec);
  }

  vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::ComputePosition\n\n" + VSDec);
  vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::ComputeNormal\n\n" + VSDec);
  vtkShaderProgram::Substitute(VSSource, VSDec, "//VTK::GLTF::ComputeTangent\n\n" + VSDec);
}

//-----------------------------------------------------------------------------
void vtkGLTFMapperHelper::ReplaceShaderValues(
  std::map<vtkShader::Type, vtkShader*> shaders, vtkRenderer* ren, vtkActor* actor)
{
  auto vertexShader = shaders[vtkShader::Vertex];
  auto VSSource = vertexShader->GetSource();

  this->AddGLTFVertexShaderTags(VSSource);

  // Node transform and normal transform declaration as uniforms
  vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::NodeUniforms",
    "\nuniform mat4 glTFNodeTransform = mat4(1);\n"
    "uniform mat4 glTFNodeNormalTransform = mat4(1);\n");

  // Define ComputePosition(). The function returns the vertex's position after skinning and
  // morphing have been applied.
  vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::ComputePosition",
    "vec4 computePosition()\n"
    "{\n"
    "  vec4 pos = vertexMC;\n\n"
    "  //VTK::GLTF::MorphingPosition\n\n"
    "  //VTK::GLTF::SkinningPosition\n\n"
    "  return pos;\n"
    "}\n");

  // Transform and output position
  vtkShaderProgram::Substitute(VSSource, "//VTK::PositionVC::Impl",
    "vec4 glTFPosition = glTFNodeTransform * computePosition();\n"
    "  vertexVCVSOutput = MCVCMatrix * glTFPosition;\n"
    "  gl_Position = MCDCMatrix * glTFPosition;\n");

  if (this->HasNormals)
  {
    // Define ComputeNormal(). The function returns the vertex's normal after skinning and morphing
    // have been applied.
    vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::ComputeNormal",
      "vec3 computeNormal()\n"
      "{\n"
      "  vec3 n = normalMC;\n\n"
      "  //VTK::GLTF::MorphingNormal\n\n"
      "  //VTK::GLTF::SkinningNormal\n\n"
      "  return n;\n"
      "}\n");

    // Transform and output normals
    vtkShaderProgram::Substitute(VSSource, "//VTK::Normal::Impl",
      "  normalVCVSOutput = normalMatrix * normalize((glTFNodeNormalTransform * "
      "vec4(computeNormal(), 0.0)).xyz);\n//VTK::Tangent::Impl\n");
  }

  if (this->HasTangents &&
    CheckForValidTextureIndex(this->MaterialTextures.NormalTextureIndex, this->Textures))
  {
    vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::ComputeTangent",
      // Define ComputeTangent(). The function returns the vertex's tangent after skinning and
      // morphing have been applied.
      "vec3 computeTangent()\n"
      "{\n"
      "  vec3 t = tangentMC;\n\n"
      "  //VTK::GLTF::MorphingTangent\n\n"
      "  //VTK::GLTF::SkinningTangent\n\n"
      "  return t;\n"
      "}\n");

    // Transform and output tangents
    vtkShaderProgram::Substitute(VSSource, "//VTK::Tangent::Impl",
      "  tangentVCVSOutput =  normalMatrix * (glTFNodeNormalTransform * "
      "vec4(computeTangent(), 0.0)).xyz;");
  }

  if (this->EnableMorphing)
  {
    this->AddMorphingToShader(VSSource);
  }

  if (this->EnableSkinning)
  {
    this->AddSkinningToShader(VSSource);
  }

  vertexShader->SetSource(VSSource);

  this->Superclass::ReplaceShaderValues(shaders, ren, actor);
}

//-----------------------------------------------------------------------------
void vtkGLTFMapperHelper::AppendOneBufferObject(vtkRenderer* ren, vtkActor* act,
  vtkCompositeMapperHelperData* hdata, vtkIdType& flat_index, std::vector<unsigned char>& colors,
  std::vector<float>& norms)
{
  auto pointData = hdata->Data->GetPointData();

  // Upload joints and weights attributes for skinning
  if (this->EnableSkinning && pointData->HasArray("WEIGHTS_0") && pointData->HasArray("JOINTS_0"))
  {
    auto weightsArray = pointData->GetArray("WEIGHTS_0");
    this->VBOs->AppendDataArray("weights", weightsArray, vtkTypeTraits<float>::VTK_TYPE_ID);

    auto jointsArray = pointData->GetArray("JOINTS_0");
    this->VBOs->AppendDataArray("joints", jointsArray, vtkTypeTraits<float>::VTK_TYPE_ID);
  }

  // Upload all target attributes for morphing
  if (this->EnableMorphing)
  {
    int maxNumberOfTargets =
      vtkMath::Max(vtkMath::Max(this->NumberOfPositionTargets, this->NumberOfNormalTargets),
        this->NumberOfTangentTargets);

    for (int i = 0; i < maxNumberOfTargets; i++)
    {
      for (auto suffix : { "_position", "_normal", "_tangent" })
      {
        std::string arrayName = "target" + value_to_string(i) + suffix;
        if (pointData->HasArray(arrayName.c_str()))
        {
          this->VBOs->AppendDataArray(arrayName.c_str(), pointData->GetArray(arrayName.c_str()),
            vtkTypeTraits<float>::VTK_TYPE_ID);
        }
      }
    }
  }

  this->Superclass::AppendOneBufferObject(ren, act, hdata, flat_index, colors, norms);
}

//-----------------------------------------------------------------------------
vtkCompositeMapperHelper2* vtkGLTFMapper::CreateHelper()
{
  return vtkGLTFMapperHelper::New();
}

//-----------------------------------------------------------------------------
void vtkGLTFMapper::SetTextures(const std::vector<vtkSmartPointer<vtkTexture> >& textures)
{
  this->Textures = textures;
}

// ---------------------------------------------------------------------------
// Description:
// Method initiates the mapping process. Generally sent by the actor
// as each frame is rendered.
void vtkGLTFMapper::Render(vtkRenderer* ren, vtkActor* actor)
{
  this->RenderedList.clear();

  // Make sure that we have been properly initialized.
  if (ren->GetRenderWindow()->CheckAbortStatus())
  {
    return;
  }

  if (this->GetInputAlgorithm() == nullptr)
  {
    return;
  }

  if (!this->Static)
  {
    this->InvokeEvent(vtkCommand::StartEvent, nullptr);
    this->GetInputAlgorithm()->Update();
    this->InvokeEvent(vtkCommand::EndEvent, nullptr);
  }

  if (this->GetInputDataObject(0, 0) == nullptr)
  {
    vtkErrorMacro(<< "No input!");
    return;
  }

  // the first step is to gather up the polydata based on their
  // signatures (aka have normals, have scalars etc)
  if (this->HelperMTime < this->GetInputDataObject(0, 0)->GetMTime() ||
    this->HelperMTime < this->GetMTime())
  {
    // clear old helpers
    for (helpIter hiter = this->Helpers.begin(); hiter != this->Helpers.end(); ++hiter)
    {
      hiter->second->ClearMark();
    }
    this->HelperDataMap.clear();

    vtkCompositeDataSet* input = vtkCompositeDataSet::SafeDownCast(this->GetInputDataObject(0, 0));

    if (input)
    {
      vtkSmartPointer<vtkDataObjectTreeIterator> iter =
        vtkSmartPointer<vtkDataObjectTreeIterator>::New();
      iter->SetDataSet(input);
      iter->SkipEmptyNodesOn();
      iter->VisitOnlyLeavesOn();
      for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
      {
        unsigned int flatIndex = iter->GetCurrentFlatIndex();
        vtkDataObject* dso = iter->GetCurrentDataObject();
        vtkPolyData* pd = vtkPolyData::SafeDownCast(dso);

        if (!pd || !pd->GetPoints())
        {
          continue;
        }

        auto pointData = pd->GetPointData();
        auto fieldData = pd->GetFieldData();

        bool hasScalars = pointData->GetScalars() != nullptr;
        bool hasNormals =
          (pointData->GetNormals() != nullptr || pd->GetCellData()->GetNormals() != nullptr);
        bool hasTangents = pointData->GetTangents() != nullptr;

        bool hasSkinning = fieldData->HasArray("jointMatrix_0") == 1;
        // Count number of skin joints
        int numberOfJoints = 0;
        if (hasSkinning)
        {
          while (fieldData->HasArray(("jointMatrix_" + value_to_string(numberOfJoints)).c_str()))
          {
            ++numberOfJoints;
          }
        }

        bool hasMorphing = fieldData->HasArray("morphingWeights") == 1;
        // Count number of targets for position, normals and tangents
        int numberOfPositionTargets = 0;
        int numberOfNormalTargets = 0;
        int numberOfTangentTargets = 0;
        if (hasMorphing)
        {
          numberOfPositionTargets = CountNumberOfAttributeTargets(pointData, "_position");
          numberOfNormalTargets = CountNumberOfAttributeTargets(pointData, "_normal");
          numberOfTangentTargets = CountNumberOfAttributeTargets(pointData, "_tangent");
        }

        // Extract texture indices from the polydata's field data
        GLTFMaterialTextures materialTextures;
        FieldDataToMaterialTextures(fieldData, materialTextures);

        // Extract material values from the polydata's field data
        GLTFMaterialValues materialValues;
        FieldDataToMaterialValues(fieldData, pointData, materialValues);

        if (materialValues.HasAlternateUVSet())
        {
          vtkWarningMacro("This mapper does not support more than one set of texture coordinates.");
        }

        std::ostringstream toString;
        toString.str("");
        toString.clear();

        // Create the helper's signature
        toString << (hasScalars ? 1 : 0) << (hasNormals ? 1 : 0) << (hasTangents ? 1 : 0)
                 << numberOfJoints << numberOfPositionTargets << numberOfNormalTargets
                 << numberOfTangentTargets << materialTextures.BaseColorTextureIndex
                 << materialTextures.EmissiveTextureIndex << materialTextures.MaterialTextureIndex
                 << materialTextures.NormalTextureIndex << materialTextures.OcclusionTextureIndex
                 << (materialValues.HasBlendAlphaMode ? 1 : 0)
                 << (materialValues.HasOpaqueAlphaMode ? 1 : 0)
                 << (materialValues.HasMaskAlphaMode ? 1 : 0);

        vtkCompositeMapperHelper2* helper = nullptr;
        helpIter found = this->Helpers.find(toString.str());
        if (found == this->Helpers.end())
        {
          helper = this->CreateHelper();
          helper->SetParent(this);

          // Configure the helper
          vtkSmartPointer<vtkGLTFMapperHelper> glTFHelper =
            vtkGLTFMapperHelper::SafeDownCast(helper);
          glTFHelper->EnableSkinning = hasSkinning;
          glTFHelper->NumberOfJoints = numberOfJoints;
          glTFHelper->EnableMorphing = hasMorphing;
          glTFHelper->NumberOfPositionTargets = numberOfPositionTargets;
          glTFHelper->NumberOfNormalTargets = numberOfNormalTargets;
          glTFHelper->NumberOfTangentTargets = numberOfTangentTargets;
          glTFHelper->HasNormals = hasNormals;
          glTFHelper->HasTangents = hasTangents;
          glTFHelper->HasScalars = hasScalars;
          glTFHelper->MaterialTextures = materialTextures;
          glTFHelper->MaterialValues = materialValues;
          this->Helpers.insert(std::make_pair(toString.str(), helper));
        }
        else
        {
          helper = found->second;
        }
        this->CopyMapperValuesToHelper(helper);
        helper->SetMarked(true);
        this->HelperDataMap[pd] = helper->AddData(pd, flatIndex);
      }
    }
    else
    {
      vtkErrorMacro("Found vtkPolyData as mapper input.");
      return;
    }

    // delete unused old helpers/data
    for (helpIter hiter = this->Helpers.begin(); hiter != this->Helpers.end();)
    {
      hiter->second->RemoveUnused();
      if (!hiter->second->GetMarked())
      {
        hiter->second->ReleaseGraphicsResources(ren->GetVTKWindow());
        hiter->second->Delete();
        this->Helpers.erase(hiter++);
      }
      else
      {
        ++hiter;
      }
    }
    this->HelperMTime.Modified();
  }

  // rebuild the render values if needed
  this->TempState.Clear();
  this->TempState.Append(actor->GetProperty()->GetMTime(), "actor mtime");
  this->TempState.Append(this->GetMTime(), "this mtime");
  this->TempState.Append(this->HelperMTime, "helper mtime");
  this->TempState.Append(
    actor->GetTexture() ? actor->GetTexture()->GetMTime() : 0, "texture mtime");
  if (this->RenderValuesState != this->TempState)
  {
    this->RenderValuesState = this->TempState;
    vtkProperty* prop = actor->GetProperty();
    vtkScalarsToColors* lut = this->GetLookupTable();
    if (lut)
    {
      // Ensure that the lookup table is built
      lut->Build();
    }

    // Push base-values on the state stack.
    this->BlockState.Visibility.push(true);
    this->BlockState.Pickability.push(true);
    this->BlockState.Opacity.push(prop->GetOpacity());
    this->BlockState.AmbientColor.push(vtkColor3d(prop->GetAmbientColor()));
    this->BlockState.DiffuseColor.push(vtkColor3d(prop->GetDiffuseColor()));
    this->BlockState.SpecularColor.push(vtkColor3d(prop->GetSpecularColor()));

    unsigned int flat_index = 0;
    this->BuildRenderValues(ren, actor, this->GetInputDataObject(0, 0), flat_index);

    this->BlockState.Visibility.pop();
    this->BlockState.Pickability.pop();
    this->BlockState.Opacity.pop();
    this->BlockState.AmbientColor.pop();
    this->BlockState.DiffuseColor.pop();
    this->BlockState.SpecularColor.pop();
  }

  this->InitializeHelpersBeforeRendering(ren, actor);

  for (helpIter hiter = this->Helpers.begin(); hiter != this->Helpers.end(); ++hiter)
  {
    vtkCompositeMapperHelper2* helper = hiter->second;
    helper->RenderPiece(ren, actor);

    std::vector<vtkPolyData*> pdl = helper->GetRenderedList();
    for (unsigned int i = 0; i < pdl.size(); ++i)
    {
      this->RenderedList.push_back(pdl[i]);
    }
  }
}

//---------------------------------------------------------------------------
void AddAttributeMorphingToShader(std::string& VSSource, int numberOfTargets,
  const std::string& calculationStart, const std::string& calculationEnd,
  const std::string& attributeName, const std::string& calculationTag)
{
  if (numberOfTargets <= 0)
  {
    return;
  }

  std::stringstream calculation;
  calculation << calculationStart;
  // Generate the string for the sum of target attributes multiplied by their respective weights
  for (int i = 0; i < numberOfTargets; i++)
  {
    std::string uniformName = "target" + value_to_string(i) + '_' + attributeName;
    vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::MorphTargets",
      "in vec3 " + uniformName + ";\n" + "//VTK::GLTF::MorphTargets");
    calculation << "morphingWeights[" << i << "] * " << uniformName;
    if (i < numberOfTargets - 1)
    {
      calculation << " + ";
    }
  }
  calculation << calculationEnd;
  vtkShaderProgram::Substitute(VSSource, calculationTag, calculation.str());
}

//---------------------------------------------------------------------------
void vtkGLTFMapperHelper::AddMorphingToShader(std::string& VSSource)
{
  // Declare weights as uniform
  vtkShaderProgram::Substitute(
    VSSource, "//VTK::GLTF::MorphingUniforms", "uniform float morphingWeights[8];\n");
  // Position
  AddAttributeMorphingToShader(VSSource, this->NumberOfPositionTargets, "pos += vec4(", ", 0.0);",
    "position", "//VTK::GLTF::MorphingPosition");
  // Normal
  AddAttributeMorphingToShader(
    VSSource, this->NumberOfNormalTargets, "n += ", ";", "normal", "//VTK::GLTF::MorphingNormal");
  // Tangent
  AddAttributeMorphingToShader(VSSource, this->NumberOfTangentTargets, "t += ", ";", "tangent",
    "//VTK::GLTF::MorphingTangent");
}

//-----------------------------------------------------------------------------
void vtkGLTFMapperHelper::AddSkinningToShader(std::string& VSSource)
{
  // Declare vertex weight and joint attributes
  vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::SkinningAttributes",
    "//VTK::GLTF::SkinningAttributes\n\n"
    "in vec4 joints;\n"
    "in vec4 weights;\n");

  // Declare joint matrices as uniforms
  vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::SkinningUniforms",
    "//VTK::GLTF::SkinningUniforms\n\n"
    "uniform mat4 jointMatrices"
    "[" +
      value_to_string(this->NumberOfJoints) + "];\n\n");

  // Declare ComputeSkinningMatrix()
  vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::ComputeSkinningMatrix",
    "mat4 computeSkinningMatrix()\n"
    "{\n"
    "  mat4 skinMat = weights.x * jointMatrices[int(joints.x)]\n"
    "               + weights.y * jointMatrices[int(joints.y)]\n"
    "               + weights.z * jointMatrices[int(joints.z)]\n"
    "               + weights.w * jointMatrices[int(joints.w)];\n"
    "  return skinMat;\n"
    "}\n");

  // Apply skinning to the vertex position
  vtkShaderProgram::Substitute(
    VSSource, "//VTK::GLTF::SkinningPosition", "pos = computeSkinningMatrix() * pos;\n");

  if (this->HasNormals)
  {
    // Declare joint normal matrices as uniforms
    vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::SkinningUniforms",
      "uniform mat4 jointNormalMatrices"
      "[" +
        value_to_string(this->NumberOfJoints) + "];\n\n");

    // Declare ComputeSkinningNormalMatrix()
    vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::ComputeSkinningNormalMatrix",
      "mat4 computeNormalSkinningMatrix()\n"
      "{\n"
      "  mat4 normalSkinMat = weights.x * jointNormalMatrices[int(joints.x)]\n"
      "               + weights.y * jointNormalMatrices[int(joints.y)]\n"
      "               + weights.z * jointNormalMatrices[int(joints.z)]\n"
      "               + weights.w * jointNormalMatrices[int(joints.w)];\n"
      "  return normalSkinMat;\n"
      "}\n");

    // Apply skinning to the vertex normal
    vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::SkinningNormal",
      "n = (computeNormalSkinningMatrix() * vec4(n, 0.0)).xyz;\n");
  }

  if (this->HasTangents)
  {
    // Apply skinning to the vertex tangent
    vtkShaderProgram::Substitute(VSSource, "//VTK::GLTF::SkinningTangent",
      "t = (computeSkinningMatrix() * vec4(t, 1.0)).xyz;\n");
  }
}
