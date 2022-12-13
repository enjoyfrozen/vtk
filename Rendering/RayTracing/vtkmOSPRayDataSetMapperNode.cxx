/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkmOSPRayDataSetMapperNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkmOSPRayDataSetMapperNode.h"

#include "vtkActor.h"
#include "vtkFloatArray.h"
#include "vtkOSPRayActorNode.h"
#include "vtkOSPRayMaterialHelpers.h"
#include "vtkOSPRayRendererNode.h"
#include "vtkObjectFactory.h"
#include "vtkPiecewiseFunction.h"
#include "vtkPointData.h"
#include "vtkProperty.h"
#include "vtkScalarsToColors.h"
#include "vtkTexture.h"
#include "vtkUnsignedCharArray.h"
#include "vtkmDataSet.h"
#include "vtkmDataSetMapper.h"
#include <vtkm/cont/DataSet.h>

#include <map>

#define VTKM_OSPRAY_ZERO_COPY 0

//-------------------------------------------------------------------------------------------------
namespace vtkospds
{
VTK_ABI_NAMESPACE_BEGIN
//------------------------------------------------------------------------------
OSPMaterial MakeActorMaterial(vtkOSPRayRendererNode* orn, OSPRenderer oRenderer,
  vtkProperty* property, double* ambientColor, double* diffuseColor, float* specularf,
  double opacity, bool pt_avail, bool& useCustomMaterial, std::map<std::string, OSPMaterial>& mats,
  const std::string& materialName)
{
  RTW::Backend* backend = orn->GetBackend();
  useCustomMaterial = false;
  if (backend == nullptr)
  {
    return OSPMaterial();
  }

  float lum = static_cast<float>(vtkOSPRayActorNode::GetLuminosity(property));

  float diffusef[] = { static_cast<float>(diffuseColor[0] * property->GetDiffuse()),
    static_cast<float>(diffuseColor[1] * property->GetDiffuse()),
    static_cast<float>(diffuseColor[2] * property->GetDiffuse()) };
  if (lum > 0.0)
  {
    OSPMaterial oMaterial = vtkOSPRayMaterialHelpers::NewMaterial(orn, oRenderer, "luminous");
    ospSetVec3f(oMaterial, "color", diffusef[0], diffusef[1], diffusef[2]);
    ospSetFloat(oMaterial, "intensity", lum);
    return oMaterial;
  }

  if (pt_avail && property->GetMaterialName())
  {
    if (std::string("Value Indexed") == property->GetMaterialName())
    {
      vtkOSPRayMaterialHelpers::MakeMaterials(
        orn, oRenderer, mats); // todo: do an mtime check to avoid doing this when unchanged
      std::string requested_mat_name = materialName;
      if (!requested_mat_name.empty() && requested_mat_name != "Value Indexed")
      {
        useCustomMaterial = true;
        return vtkOSPRayMaterialHelpers::MakeMaterial(orn, oRenderer, requested_mat_name.c_str());
      }
    }
    else
    {
      useCustomMaterial = true;
      return vtkOSPRayMaterialHelpers::MakeMaterial(orn, oRenderer, property->GetMaterialName());
    }
  }

  OSPMaterial oMaterial;
  if (pt_avail && property->GetInterpolation() == VTK_PBR)
  {
    oMaterial = vtkOSPRayMaterialHelpers::NewMaterial(orn, oRenderer, "principled");

    ospSetVec3f(oMaterial, "baseColor", diffusef[0], diffusef[1], diffusef[2]);
    ospSetFloat(oMaterial, "metallic", static_cast<float>(property->GetMetallic()));
    ospSetFloat(oMaterial, "roughness", static_cast<float>(property->GetRoughness()));
    ospSetFloat(oMaterial, "opacity", static_cast<float>(opacity));
    // As OSPRay seems to not recalculate the refractive index of the base layer
    // we need to recalculate, from the effective reflectance of the base layer (with the
    // coat), the ior of the base that will produce the same reflectance but with the air
    // with an ior of 1.0
    double baseF0 = property->ComputeReflectanceOfBaseLayer();
    const double exteriorIor = 1.0;
    double baseIor = vtkProperty::ComputeIORFromReflectance(baseF0, exteriorIor);
    ospSetFloat(oMaterial, "ior", static_cast<float>(baseIor));
    float edgeColor[3] = { static_cast<float>(property->GetEdgeTint()[0]),
      static_cast<float>(property->GetEdgeTint()[1]),
      static_cast<float>(property->GetEdgeTint()[2]) };
    ospSetVec3f(oMaterial, "edgeColor", edgeColor[0], edgeColor[1], edgeColor[2]);
    ospSetFloat(oMaterial, "anisotropy", static_cast<float>(property->GetAnisotropy()));
    ospSetFloat(oMaterial, "rotation", static_cast<float>(property->GetAnisotropyRotation()));
    ospSetFloat(oMaterial, "baseNormalScale", static_cast<float>(property->GetNormalScale()));
    ospSetFloat(oMaterial, "coat", static_cast<float>(property->GetCoatStrength()));
    ospSetFloat(oMaterial, "coatIor", static_cast<float>(property->GetCoatIOR()));
    ospSetFloat(oMaterial, "coatRoughness", static_cast<float>(property->GetCoatRoughness()));
    float coatColor[] = { static_cast<float>(property->GetCoatColor()[0]),
      static_cast<float>(property->GetCoatColor()[1]),
      static_cast<float>(property->GetCoatColor()[2]) };
    ospSetVec3f(oMaterial, "coatColor", coatColor[0], coatColor[1], coatColor[2]);
    ospSetFloat(oMaterial, "coatNormal", static_cast<float>(property->GetCoatNormalScale()));
  }
  else
  {
    oMaterial = vtkOSPRayMaterialHelpers::NewMaterial(orn, oRenderer, "obj");

    float ambientf[] = { static_cast<float>(ambientColor[0] * property->GetAmbient()),
      static_cast<float>(ambientColor[1] * property->GetAmbient()),
      static_cast<float>(ambientColor[2] * property->GetAmbient()) };

    float specPower = static_cast<float>(property->GetSpecularPower());
    float specAdjust = 2.0f / (2.0f + specPower);
    specularf[0] =
      static_cast<float>(property->GetSpecularColor()[0] * property->GetSpecular() * specAdjust);
    specularf[1] =
      static_cast<float>(property->GetSpecularColor()[1] * property->GetSpecular() * specAdjust);
    specularf[2] =
      static_cast<float>(property->GetSpecularColor()[2] * property->GetSpecular() * specAdjust);

    ospSetVec3f(oMaterial, "ka", ambientf[0], ambientf[1], ambientf[2]);
    if (property->GetDiffuse() == 0.0)
    {
      // a workaround for ParaView, remove when ospray supports Ka
      ospSetVec3f(oMaterial, "kd", ambientf[0], ambientf[1], ambientf[2]);
    }
    else
    {
      ospSetVec3f(oMaterial, "kd", diffusef[0], diffusef[1], diffusef[2]);
    }
    ospSetVec3f(oMaterial, "ks", specularf[0], specularf[1], specularf[2]);
    ospSetFloat(oMaterial, "ns", specPower);
    ospSetFloat(oMaterial, "d", static_cast<float>(opacity));
  }

  return oMaterial;
}

//------------------------------------------------------------------------------
OSPMaterial MakeActorMaterial(vtkOSPRayRendererNode* orn, OSPRenderer oRenderer,
  vtkProperty* property, double* ambientColor, double* diffuseColor, float* specularf,
  double opacity)
{
  bool dontcare1;
  std::map<std::string, OSPMaterial> dontcare2;
  return MakeActorMaterial(orn, oRenderer, property, ambientColor, diffuseColor, specularf, opacity,
    false, dontcare1, dontcare2, "");
}

//------------------------------------------------------------------------------
void MakeCellMaterials(vtkOSPRayRendererNode* orn, OSPRenderer oRenderer, vtkDataSet* ds,
  vtkMapper* mapper, vtkScalarsToColors* s2c, std::map<std::string, OSPMaterial> mats,
  std::vector<OSPMaterial>& ospMaterials, vtkUnsignedCharArray* vColors, float* specColor,
  float specPower, float opacity)
{
  RTW::Backend* backend = orn->GetBackend();
  if (backend == nullptr)
    return;
  vtkAbstractArray* scalars = nullptr;
  bool try_mats = s2c->GetIndexedLookup() && s2c->GetNumberOfAnnotatedValues() && !mats.empty();
  if (try_mats)
  {
    int cflag2 = -1;
    scalars = mapper->GetAbstractScalars(ds, mapper->GetScalarMode(), mapper->GetArrayAccessMode(),
      mapper->GetArrayId(), mapper->GetArrayName(), cflag2);
  }
  int numColors = vColors->GetNumberOfTuples();
  int width = vColors->GetNumberOfComponents();
  for (int i = 0; i < numColors; i++)
  {
    bool found = false;
    if (scalars)
    {
      vtkVariant v = scalars->GetVariantValue(i);
      vtkIdType idx = s2c->GetAnnotatedValueIndex(v);
      if (idx > -1)
      {
        std::string name(s2c->GetAnnotation(idx));
        if (mats.find(name) != mats.end())
        {
          OSPMaterial oMaterial = mats[name];
          ospCommit(oMaterial);
          ospMaterials.push_back(oMaterial);
          found = true;
        }
      }
    }
    if (!found)
    {
      double* color = vColors->GetTuple(i);
      OSPMaterial oMaterial;
      oMaterial = vtkOSPRayMaterialHelpers::NewMaterial(orn, oRenderer, "obj");
      float diffusef[] = { static_cast<float>(color[0]) / (255.0f),
        static_cast<float>(color[1]) / (255.0f), static_cast<float>(color[2]) / (255.0f) };
      float localOpacity = 1.f;
      if (width >= 4)
      {
        localOpacity = static_cast<float>(color[3]) / (255.0f);
      }
      ospSetVec3f(oMaterial, "kd", diffusef[0], diffusef[1], diffusef[2]);
      float specAdjust = 2.0f / (2.0f + specPower);
      float specularf[] = { specColor[0] * specAdjust, specColor[1] * specAdjust,
        specColor[2] * specAdjust };
      ospSetVec3f(oMaterial, "ks", specularf[0], specularf[1], specularf[2]);
      ospSetFloat(oMaterial, "ns", specPower);
      ospSetFloat(oMaterial, "d", opacity * localOpacity);
      ospCommit(oMaterial);
      ospMaterials.push_back(oMaterial);
    }
  }
}

//------------------------------------------------------------------------------
float MapThroughPWF(double in, vtkPiecewiseFunction* scaleFunction)
{
  double out = in;
  if (!scaleFunction)
  {
    out = in;
  }
  else
  {
    out = scaleFunction->GetValue(in);
  }
  return static_cast<float>(out);
}

//----------------------------------------------------------------------------
OSPGeometricModel RenderAsSpheres(std::vector<osp::vec3f>& vertices, double pointSize,
  vtkDataArray* scaleArray, vtkPiecewiseFunction* scaleFunction, bool useCustomMaterial,
  OSPMaterial actorMaterial, vtkImageData* vColorTextureMap, bool sRGB, int numTextureCoordinates,
  float* textureCoordinates, int numCellMaterials, std::vector<OSPMaterial>& CellMaterials,
  int numPointColors, osp::vec4f* PointColors, int numPointValueTextureCoords,
  float* pointValueTextureCoords, RTW::Backend* backend)
{
  if (backend == nullptr)
    return OSPGeometry();
  OSPGeometry ospMesh = ospNewGeometry("sphere");
  OSPGeometricModel ospGeoModel = ospNewGeometricModel(ospMesh);

  size_t numSpheres = vertices.size();
  std::vector<osp::vec3f> vdata;
  std::vector<float> radii;
  vdata.reserve(numSpheres);
  if (scaleArray != nullptr)
  {
    radii.reserve(numSpheres);
  }
  for (size_t i = 0; i < vertices.size(); i++)
  {
    vdata.emplace_back(vertices[i]);
    if (scaleArray != nullptr)
    {
      radii.emplace_back(MapThroughPWF(*scaleArray->GetTuple(i), scaleFunction));
    }
  }
  OSPData positionData = ospNewCopyData1D(vdata.data(), OSP_VEC3F, vdata.size());
  ospCommit(positionData);
  ospSetObject(ospMesh, "sphere.position", positionData);
  if (scaleArray != nullptr)
  {
    OSPData radiiData = ospNewCopyData1D(radii.data(), OSP_FLOAT, radii.size());
    ospCommit(radiiData);
    ospSetObject(ospMesh, "sphere.radius", radiiData);
  }
  else
  {
    ospSetFloat(ospMesh, "radius", pointSize);
  }

  // send the texture map and texture coordinates over
  bool _hastm = false;
  if (numTextureCoordinates || numPointValueTextureCoords)
  {
    _hastm = true;

    if (numPointValueTextureCoords)
    {
      // using 1D texture for point value LUT
      std::vector<osp::vec2f> tc(numSpheres);
      for (size_t i = 0; i < numSpheres; i++)
      {
        float t1;
        int index1 = i;
        t1 = pointValueTextureCoords[index1 + 0];
        tc[i] = osp::vec2f{ t1, 0 };
      }
      OSPData tcs = ospNewCopyData1D(tc.data(), OSP_VEC2F, numSpheres);
      ospCommit(tcs);
      ospSetObject(ospMesh, "sphere.texcoord", tcs);
    }
    else if (numTextureCoordinates)
    {
      // 2d texture mapping
      float* itc = textureCoordinates;
      std::vector<osp::vec2f> tc(numSpheres);
      for (size_t i = 0; i < numSpheres; i++)
      {
        float t1, t2;
        int index1 = i;
        t1 = itc[index1 * 2 + 0];
        t2 = itc[index1 * 2 + 1];
        tc[i] = osp::vec2f{ t1, t2 };
      }
      OSPData tcs = ospNewCopyData1D(tc.data(), OSP_VEC2F, numSpheres);
      ospCommit(tcs);
      ospSetObject(ospMesh, "sphere.texcoord", tcs);
    }
  }

  OSPData _cmats = nullptr;
  OSPData _PointColors = nullptr;
  bool perCellColor = false;
  bool perPointColor = false;
  if (!useCustomMaterial)
  {
    if (vColorTextureMap && _hastm)
    {
      OSPTexture t2d = vtkOSPRayMaterialHelpers::VTKToOSPTexture(backend, vColorTextureMap, sRGB);
      ospSetObject(actorMaterial, "map_kd", ((OSPTexture)(t2d)));
      ospCommit(actorMaterial);
      ospRelease(t2d);
    }
    else if (numCellMaterials)
    {
      // per cell color
      perCellColor = true;
      std::vector<OSPMaterial> perCellMats;
      for (size_t i = 0; i < numSpheres; i++)
      {
        perCellMats.push_back(CellMaterials[i]);
      }
      _cmats = ospNewCopyData1D(&perCellMats[0], OSP_MATERIAL, numSpheres);
      ospCommit(_cmats);
      ospSetObject(ospGeoModel, "material", _cmats);
    }
    else if (numPointColors)
    {
      // per point color
      perPointColor = true;
      std::vector<osp::vec4f> perPointColors;
      for (size_t i = 0; i < numSpheres; i++)
      {
        perPointColors.push_back(PointColors[i]);
      }
      _PointColors = ospNewCopyData1D(&perPointColors[0], OSP_VEC4F, numSpheres);
      ospCommit(_PointColors);
      ospSetObject(ospGeoModel, "color", _PointColors);
    }
  }

  if (actorMaterial && !perCellColor && !perPointColor)
  {
    ospCommit(actorMaterial);
    ospSetObjectAsData(ospGeoModel, "material", OSP_MATERIAL, actorMaterial);
  }
  ospCommit(ospMesh);
  ospCommit(ospGeoModel);
  ospRelease(ospMesh);
  ospRelease(_cmats);
  ospRelease(_PointColors);

  return ospGeoModel;
}

VTK_ABI_NAMESPACE_END
}

VTK_ABI_NAMESPACE_BEGIN
//-------------------------------------------------------------------------------------------------
vtkStandardNewMacro(vtkmOSPRayDataSetMapperNode);

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::Invalidate(bool prepass)
{
  if (prepass)
  {
    this->RenderTime = 0;
  }
}

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::Render(bool prepass)
{
  if (prepass)
  {
    // Do the rendering
    // we use a lot of params from our parent
    vtkOSPRayActorNode* aNode = vtkOSPRayActorNode::SafeDownCast(this->Parent);
    vtkActor* act = vtkActor::SafeDownCast(aNode->GetRenderable());

    if (act->GetVisibility() == false)
    {
      return;
    }
    vtkOSPRayRendererNode* orn =
      static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));

    // if there are no changes, just reuse last result
    vtkMTimeType inTime = aNode->GetMTime();
    if (this->RenderTime >= inTime)
    {
      this->RenderGeometricModels();
      return;
    }
    this->RenderTime = inTime;
    this->ClearGeometricModels();

    vtkmDataSet* ds = nullptr;
    vtkmDataSetMapper* mapper = vtkmDataSetMapper::SafeDownCast(act->GetMapper());
    if (mapper && mapper->GetNumberOfInputPorts() > 0)
    {
      ds = vtkmDataSet::SafeDownCast(mapper->GetInput());
    }
    if (ds)
    {
      vtkProperty* property = act->GetProperty();
      double ambient[3];
      double diffuse[3];
      property->GetAmbientColor(ambient);
      property->GetDiffuseColor(diffuse);
      this->ORenderDataSet(
        orn->GetORenderer(), aNode, ds, ambient, diffuse, property->GetOpacity(), "");
    }
    this->RenderGeometricModels();
  }
}

//------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
VTK_ABI_NAMESPACE_END

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::RenderGeometricModels()
{
  vtkOSPRayRendererNode* orn =
    static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));

  for (auto instance : this->Instances)
  {
    orn->Instances.emplace_back(instance);
  }
}

//-------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::ClearGeometricModels()
{
  vtkOSPRayRendererNode* orn =
    static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));

  RTW::Backend* backend = orn->GetBackend();

  // free up whatever we did last time
  for (auto instance : this->Instances)
  {
    ospRelease(instance);
  }
  this->Instances.clear();
}

//------------------------------------------------------------------------------------------------
void vtkmOSPRayDataSetMapperNode::ORenderDataSet(void* renderer, vtkOSPRayActorNode* aNode,
  vtkmDataSet* ds, double* ambientColor, double* diffuseColor, double opacity, std::string material)
{
  vtkOSPRayRendererNode* orn =
    static_cast<vtkOSPRayRendererNode*>(this->GetFirstAncestorOfType("vtkOSPRayRendererNode"));
  RTW::Backend* backend = orn->GetBackend();
  if (backend == nullptr)
    return;

  OSPRenderer oRenderer = static_cast<OSPRenderer>(renderer);
  vtkActor* act = vtkActor::SafeDownCast(aNode->GetRenderable());
  vtkProperty* property = act->GetProperty();

  // TODO: Perform transformations (actor and texture)

  // make geometry
  auto vtkmds = ds->GetVtkmDataSet();
  auto numPoints = vtkmds.GetCoordinateSystem().GetNumberOfPoints();
  vtkDebugMacro(<< "Number Of Points in vtkmDataSet: " << numPoints);
  if (numPoints == 0)
  {
    return;
  }

  OSPData position;
#if VTKM_OSPRAY_ZERO_COPY
  auto points = ds->GetPoints();
  void* vertices = points->GetVoidPointer(0);
  position = new ospNewSharedData1D(vertices, OSP_VEC3F, numPoints);
#else
  std::vector<osp::vec3f> vertices(numPoints);
  double x[3];
  for (vtkIdType i = 0; i < numPoints; ++i)
  {
    ds->GetPoint(i, x);
    vertices[i] =
      osp::vec3f{ static_cast<float>(x[0]), static_cast<float>(x[1]), static_cast<float>(x[2]) };
  }
  position = ospNewCopyData1D(&vertices[0], OSP_VEC3F, numPoints);
#endif

  ospCommit(position);

  // choosing sphere and cylinder radii (for points and lines) that
  // approximate pointsize and linewidth
  vtkMapper* mapper = act->GetMapper();
  double length = 1.0;
  if (mapper)
  {
    length = mapper->GetLength();
  }
  int scalingMode = vtkOSPRayActorNode::GetEnableScaling(act);
  double pointSize = length / 1000.0 * property->GetPointSize();
  // double lineWidth = length / 1000.0 * property->GetLineWidth();
  if (scalingMode == vtkOSPRayActorNode::ALL_EXACT)
  {
    pointSize = property->GetPointSize();
    // lineWidth = property->GetLineWidth();
  }
  // finer control over sphere and cylinders sizes
  vtkDataArray* scaleArray = nullptr;
  vtkPiecewiseFunction* scaleFunction = nullptr;
  if (mapper && scalingMode > vtkOSPRayActorNode::ALL_APPROXIMATE)
  {
    vtkInformation* mapInfo = mapper->GetInformation();
    char* scaleArrayName = (char*)mapInfo->Get(vtkOSPRayActorNode::SCALE_ARRAY_NAME());
    scaleArray = ds->GetPointData()->GetArray(scaleArrayName);
    if (scalingMode != vtkOSPRayActorNode::EACH_EXACT)
    {
      scaleFunction =
        vtkPiecewiseFunction::SafeDownCast(mapInfo->Get(vtkOSPRayActorNode::SCALE_FUNCTION()));
    }
  }

  // now ask mapper to do most of the work and provide us with
  // colors per cell and colors or texture coordinates per point
  vtkUnsignedCharArray* vColors = nullptr;
  vtkFloatArray* vColorCoordinates = nullptr;
  vtkImageData* pColorTextureMap = nullptr;
  int cellFlag = -1; // mapper tells us which
  if (mapper)
  {
    mapper->MapScalars(ds, 1.0, cellFlag);
    vColors = mapper->GetColorMapColors();
    vColorCoordinates = mapper->GetColorCoordinates();
    pColorTextureMap = mapper->GetColorTextureMap();
  }

  if (vColors || (vColorCoordinates && pColorTextureMap))
  {
    // OSPRay scales the color mapping with the solid color but OpenGL backend does not do it.
    // set back to white to workaround this difference.
    std::fill(diffuseColor, diffuseColor + 3, 1.0);
  }

  // per actor material
  float specularf[3];
  bool useCustomMaterial = false;
  std::map<std::string, OSPMaterial> mats;
  std::set<OSPMaterial> uniqueMats;
  const std::string rendererType =
    orn->GetRendererType(vtkRenderer::SafeDownCast(orn->GetRenderable()));
  bool pt_avail =
    rendererType == std::string("pathtracer") || rendererType == std::string("optix pathtracer");
  OSPMaterial oMaterial = vtkospds::MakeActorMaterial(orn, oRenderer, property, ambientColor,
    diffuseColor, specularf, opacity, pt_avail, useCustomMaterial, mats, material);
  ospCommit(oMaterial);
  uniqueMats.insert(oMaterial);

  // texture
  int numTextureCoordinates = 0;
  std::vector<osp::vec2f> textureCoordinates;
  if (vtkDataArray* da = ds->GetPointData()->GetTCoords())
  {
    numTextureCoordinates = da->GetNumberOfTuples();
    textureCoordinates.resize(numTextureCoordinates);
    for (int i = 0; i < numTextureCoordinates; i++)
    {
      textureCoordinates[i] = osp::vec2f(
        { static_cast<float>(da->GetTuple(i)[0]), static_cast<float>(da->GetTuple(i)[1]) });
    }
    numTextureCoordinates = numTextureCoordinates * 2;
  }
  vtkTexture* texture = nullptr;
  if (property->GetInterpolation() == VTK_PBR)
  {
    texture = property->GetTexture("albedoTex");
  }
  else
  {
    texture = act->GetTexture();
  }
  vtkImageData* vColorTextureMap = nullptr;
  // vtkImageData* vNormalTextureMap = nullptr;
  // vtkImageData* vMaterialTextureMap = nullptr;
  // vtkImageData* vAnisotropyTextureMap = nullptr;
  // vtkImageData* vCoatNormalTextureMap = nullptr;

  bool sRGB = false;

  if (texture)
  {
    sRGB = texture->GetUseSRGBColorSpace();
    vColorTextureMap = texture->GetInput();
    ospSetVec3f(oMaterial, "kd", 1.0f, 1.0f, 1.0f);
    ospCommit(oMaterial);
  }

  // colors from point and cell arrays
  int numCellMaterials = 0;
  std::vector<OSPMaterial> cellMaterials;
  int numPointColors = 0;
  std::vector<osp::vec4f> pointColors;
  int numPointValueTextureCoords = 0;
  std::vector<float> pointValueTextureCoords;
  if (vColors)
  {
    if (cellFlag == 2 && mapper->GetFieldDataTupleId() > -1)
    {
      // color comes from field data entry
      bool use_material = false;
      // check if the field data content says to use a material lookup
      vtkScalarsToColors* s2c = mapper->GetLookupTable();
      bool try_mats = s2c->GetIndexedLookup() && s2c->GetNumberOfAnnotatedValues() && !mats.empty();
      if (try_mats)
      {
        int cflag2 = -1;
        vtkAbstractArray* scalars = mapper->GetAbstractScalars(ds, mapper->GetScalarMode(),
          mapper->GetArrayAccessMode(), mapper->GetArrayId(), mapper->GetArrayName(), cflag2);
        vtkVariant v = scalars->GetVariantValue(mapper->GetFieldDataTupleId());
        vtkIdType idx = s2c->GetAnnotatedValueIndex(v);
        if (idx > -1)
        {
          std::string name(s2c->GetAnnotation(idx));
          if (mats.find(name) != mats.end())
          {
            // yes it does!
            oMaterial = mats[name];
            ospCommit(oMaterial);
            use_material = true;
          }
        }
      }
      if (!use_material)
      {
        // just use the color for the field data value
        int numComp = vColors->GetNumberOfComponents();
        unsigned char* colorPtr = vColors->GetPointer(0);
        colorPtr = colorPtr + mapper->GetFieldDataTupleId() * numComp;
        // this setting (and all the other scalar colors)
        // really depends on mapper->ScalarMaterialMode
        // but I'm not sure Ka is working currently so leaving it on Kd
        float fdiffusef[] = { static_cast<float>(colorPtr[0] * property->GetDiffuse() / 255.0f),
          static_cast<float>(colorPtr[1] * property->GetDiffuse() / 255.0f),
          static_cast<float>(colorPtr[2] * property->GetDiffuse() / 255.0f) };
        ospSetVec3f(oMaterial, "kd", fdiffusef[0], fdiffusef[1], fdiffusef[2]);
        ospCommit(oMaterial);
      }
    }
    else if (cellFlag == 1)
    {
      // color or material on cell
      vtkScalarsToColors* s2c = mapper->GetLookupTable();
      vtkospds::MakeCellMaterials(orn, oRenderer, ds, mapper, s2c, mats, cellMaterials, vColors,
        specularf, float(property->GetSpecularPower()), opacity);
      numCellMaterials = static_cast<int>(cellMaterials.size());
      for (OSPMaterial mat : cellMaterials)
      {
        uniqueMats.insert(mat);
      }
    }
    else if (cellFlag == 0)
    {
      // color on point interpolated RGB
      numPointColors = vColors->GetNumberOfTuples();
      pointColors.resize(numPointColors);
      for (int i = 0; i < numPointColors; i++)
      {
        unsigned char* color = vColors->GetPointer(4 * i);
        pointColors[i] = osp::vec4f{ color[0] / 255.0f, color[1] / 255.0f, color[2] / 255.0f,
          (color[3] / 255.0f) * static_cast<float>(opacity) };
      }
      ospSetVec3f(oMaterial, "kd", 1.0f, 1.0f, 1.0f);
      ospCommit(oMaterial);
    }
  }
  else
  {
    if (vColorCoordinates && pColorTextureMap)
    {
      // color on point interpolated values (subsequently colormapped via 1D LUT)
      numPointValueTextureCoords = vColorCoordinates->GetNumberOfTuples();
      pointValueTextureCoords.resize(numPointValueTextureCoords);
      float* tc = vColorCoordinates->GetPointer(0);
      for (int i = 0; i < numPointValueTextureCoords; i++)
      {
        float v = *tc;
        v = ((v >= 1.0f) ? 0.99999f : ((v < 0.0f) ? 0.0f : v)); // clamp [0..1)
        pointValueTextureCoords[i] = v;
        tc += 2;
      }
      // vColorTextureMap = pColorTextureMap;
      ospSetVec3f(oMaterial, "kd", 1.0f, 1.0f, 1.0f);
      ospCommit(oMaterial);
    }
  }

  // render the vertices as spheres
  this->GeometricModels.emplace_back(
    vtkospds::RenderAsSpheres(vertices, pointSize, scaleArray, scaleFunction, useCustomMaterial,
      oMaterial, vColorTextureMap, sRGB, numTextureCoordinates, (float*)textureCoordinates.data(),
      numCellMaterials, cellMaterials, numPointColors, pointColors.data(),
      numPointValueTextureCoords, (float*)pointValueTextureCoords.data(), backend));

  ospRelease(position);
#if !(VTKM_OSPRAY_ZERO_COPY)
  vertices.clear();
#endif

  for (auto it : mats)
  {
    uniqueMats.insert(it.second);
  }

  for (OSPMaterial mat : uniqueMats)
  {
    ospRelease(mat);
  }

  for (auto g : this->GeometricModels)
  {
    OSPGroup group = ospNewGroup();
    OSPData data = ospNewCopyData1D(&g, OSP_GEOMETRIC_MODEL, 1);
    ospCommit(data);
    ospRelease(g);
    ospSetObject(group, "geometry", data);
    ospCommit(group);
    ospRelease(data);
    OSPInstance instance = ospNewInstance(group); // valgrind reports instance is lost
    ospCommit(instance);
    ospRelease(group);
    this->Instances.emplace_back(instance);
  }

  this->GeometricModels.clear();
}

VTK_ABI_NAMESPACE_END
