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
  OSPMaterial oMaterial = vtkOSPRayMaterialHelpers::MakePropertyMaterial(orn, oRenderer, property,
    ambientColor, diffuseColor, specularf, opacity, pt_avail, useCustomMaterial, mats, material);
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
    vtkWarningMacro(<< "Scalar coloring only supported via texture map");
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
