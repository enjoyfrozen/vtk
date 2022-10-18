/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkEmbreePointLocator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkEmbreePointLocator.h"

#include "vtkDataSet.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"

#include <vector>

#include <embree3/rtcore.h>

class vtkEmbreePointLocator::vtkInternals
{
public:
  vtkInternals(vtkEmbreePointLocator* self)
    : self(self)
  {
    this->device = rtcNewDevice(NULL);
    this->scene = rtcNewScene(this->device);
  }

  ~vtkInternals()
  {
    rtcReleaseScene(this->scene);
    rtcReleaseDevice(this->device);
  }
  RTCDevice device;
  RTCScene scene;
  RTCGeometry geom = nullptr;
  vtkEmbreePointLocator* self;
  double triangle_epsilon = 0.0;
  std::vector<std::pair<double, int>> closestList;
  double furthest;
  int numClosest;
};

struct vec3f
{
  float x, y, z;
};

struct ClosestPointResult
{
  ClosestPointResult()
    : primID(RTC_INVALID_GEOMETRY_ID)
    , geomID(RTC_INVALID_GEOMETRY_ID)
  {
  }

  vec3f p;
  unsigned int primID;
  unsigned int geomID;
  vtkEmbreePointLocator::vtkInternals* self;
};

bool closestPointFunc(RTCPointQueryFunctionArguments* args)
{
  assert(args->userPtr);
  const unsigned int geomID = args->geomID;
  const unsigned int primID = args->primID;

  RTCPointQueryContext* context = args->context;

  // query position in world space
  vec3f q = { args->query->x, args->query->y, args->query->z };
  float Q[3] = { q.x, q.y, q.z };

  // rely on fact that we made one triangle for each point with a well known function to recover it
  vtkEmbreePointLocator::vtkInternals* self = ((ClosestPointResult*)args->userPtr)->self;
  double* vNext = self->self->GetDataSet()->GetPoint(primID);

  vec3f p = { float(vNext[0]), float(vNext[1]), float(vNext[2]) };
  float P[3] = { p.x, p.y, p.z };
  float d = std::sqrt(vtkMath::Distance2BetweenPoints(Q, P));

  /*
   * Store result in userPtr and update the query radius if we found a point
   * closer to the query position. This is optional but allows for faster
   * traversal (due to better culling).
   */
  if (d < args->query->radius)
  {
    args->query->radius = d;
    ClosestPointResult* result = (ClosestPointResult*)args->userPtr;
    result->p = vec3f({ float(vNext[0]), float(vNext[1]), float(vNext[2]) });
    result->primID = primID;
    result->geomID = geomID;
    return true; // Return true to indicate that the query radius changed.
  }
  return false;
}

bool closestPointsFunc(RTCPointQueryFunctionArguments* args)
{
  assert(args->userPtr);
  const unsigned int geomID = args->geomID;
  const unsigned int primID = args->primID;

  RTCPointQueryContext* context = args->context;

  // query position in world space
  float Q[3] = { args->query->x, args->query->y, args->query->z };
  vec3f q = { args->query->x, args->query->y, args->query->z };

  vtkEmbreePointLocator::vtkInternals* self = ((ClosestPointResult*)args->userPtr)->self;
  double* vNext = self->self->GetDataSet()->GetPoint(primID);
  vec3f p = { float(vNext[0]), float(vNext[1]), float(vNext[2]) };
  float vert[3] = { p.x, p.y, p.z };
  float d = std::sqrt(vtkMath::Distance2BetweenPoints(Q, vert));
  if (d > args->query->radius)
  {
    return false;
  }
  if (self->closestList.size() < self->numClosest || self->numClosest == -1)
  {
    self->closestList.push_back(std::make_pair(d, primID));
    if (d > self->furthest)
    {
      self->furthest = d;
    }
    ClosestPointResult* result = (ClosestPointResult*)args->userPtr;
    result->p = vec3f({ float(vNext[0]), float(vNext[1]), float(vNext[2]) });
    result->primID = primID;
    result->geomID = geomID;
    return false;
  }
  else
  {
    if (d > self->furthest)
    {
      return false;
    }
    for (int i = 0; i < self->closestList.size(); i++)
    {
      if (self->closestList[i].first == self->furthest)
      {
        self->closestList[i] = std::make_pair(d, primID);
        break;
      }
    }
    // TODO: results I am getting from embree seem to be mostly sorted
    // if that is always true we can optimize, for example new max is always last element
    double maxD = self->closestList[0].first;
    for (int i = 1; i < self->closestList.size(); i++)
    {
      if (self->closestList[i].first > maxD)
      {
        maxD = self->closestList[i].first;
      }
    }
    self->furthest = maxD;
    /*
     * Store result in userPtr and update the query radius if we found a point
     * closer to the query position. This is optional but allows for faster
     * traversal (due to better culling).
     */
    if (maxD < args->query->radius)
    {
      args->query->radius = self->furthest;
      ClosestPointResult* result = (ClosestPointResult*)args->userPtr;
      result->p = vec3f({ float(vNext[0]), float(vNext[1]), float(vNext[2]) });
      result->primID = primID;
      result->geomID = geomID;
      return true; // Return true to indicate that the query radius changed.
    }
  }
  return false;
}

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkEmbreePointLocator);

//------------------------------------------------------------------------------
vtkEmbreePointLocator::vtkEmbreePointLocator()
{
  this->Internals = new vtkEmbreePointLocator::vtkInternals(this);
}

//------------------------------------------------------------------------------
vtkEmbreePointLocator::~vtkEmbreePointLocator()
{
  delete this->Internals;
}

//------------------------------------------------------------------------------
void vtkEmbreePointLocator::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//------------------------------------------------------------------------------
void vtkEmbreePointLocator::BuildLocator()
{
  vtkIdType numPts;

  // don't rebuild if build time is newer than modified and dataset modified time
  if (this->BuildTime > this->MTime && this->BuildTime > this->DataSet->GetMTime())
  {
    return;
  }

  if (!this->DataSet || (numPts = this->DataSet->GetNumberOfPoints()) < 1)
  {
    vtkErrorMacro(<< "No points to locate.");
    return;
  }

  // TODO: rtcPointQuery does not yet support TYPE_POINT for mysterious reasons,
  // so I'm using TYPE_TRIANGLE instead of TYPE_POINTS and faking it with small triangles.
  // Push embree team to support that or think about starting with a vtkCellLocator example.
  this->Internals->triangle_epsilon = this->DataSet->GetLength() / 1000.0 / 200.0;
  const double& triangle_epsilon = this->Internals->triangle_epsilon;

  RTCGeometry geom = rtcNewGeometry(this->Internals->device, RTC_GEOMETRY_TYPE_TRIANGLE);
  // TODO: replace with shared buffer, once we resolve the TYPE_POINTS thing above

  float* vertices = (float*)rtcSetNewGeometryBuffer(
    geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), numPts * 3);

  unsigned* indices = (unsigned*)rtcSetNewGeometryBuffer(
    geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned), numPts);

  for (unsigned int v = 0; v < numPts; v++)
  {
    double* vNext = this->DataSet->GetPoint(v);
    vertices[v * 9 + 0] = vNext[0];
    vertices[v * 9 + 1] = vNext[1] + triangle_epsilon;
    vertices[v * 9 + 2] = vNext[2];
    vertices[v * 9 + 3] = vNext[0] - triangle_epsilon;
    vertices[v * 9 + 4] = vNext[1] - triangle_epsilon;
    vertices[v * 9 + 5] = vNext[2];
    vertices[v * 9 + 6] = vNext[0] + triangle_epsilon;
    vertices[v * 9 + 7] = vNext[1] - triangle_epsilon;
    vertices[v * 9 + 8] = vNext[2];

    indices[v * 3 + 0] = v * 3 + 0;
    indices[v * 3 + 1] = v * 3 + 1;
    indices[v * 3 + 2] = v * 3 + 2;
  }

  rtcCommitGeometry(geom);

  rtcAttachGeometry(this->Internals->scene, geom);
  rtcReleaseGeometry(geom);
  rtcSetSceneBuildQuality(this->Internals->scene, RTC_BUILD_QUALITY_HIGH);

  rtcCommitScene(this->Internals->scene);
  this->BuildTime.Modified();
}

//------------------------------------------------------------------------------
void vtkEmbreePointLocator::FreeSearchStructure()
{
  rtcReleaseScene(this->Internals->scene);
}

//------------------------------------------------------------------------------
void vtkEmbreePointLocator::GenerateRepresentation(
  int vtkNotUsed(level), vtkPolyData* vtkNotUsed(pd))
{
  // acceleration structure is internal to embree and not readily accessible externally
  return;
}

//------------------------------------------------------------------------------
vtkIdType vtkEmbreePointLocator::FindClosestPoint(const double x[3])
{
  this->BuildLocator();
  double dist2;
  return this->FindClosestPointWithinRadius(vtkMath::Inf(), x, dist2);
}

//------------------------------------------------------------------------------
void vtkEmbreePointLocator::FindClosestNPoints(int N, const double x[3], vtkIdList* result)
{
  this->BuildLocator();

  RTCPointQuery query;
  query.x = x[0];
  query.y = x[1];
  query.z = x[2];
  query.radius = vtkMath::Inf();
  query.time = 0.f;

  result->Initialize();

  this->Internals->numClosest = N;
  this->Internals->closestList.clear();
  this->Internals->furthest = 0;

  ClosestPointResult qresult;
  qresult.self = this->Internals;

  RTCPointQueryContext context;
  rtcInitPointQueryContext(&context);
  rtcPointQuery(this->Internals->scene, &query, &context, closestPointsFunc, (void*)&qresult);
  std::sort(this->Internals->closestList.begin(), this->Internals->closestList.end());
  for (int i = 0; i < this->Internals->closestList.size(); i++)
  {
    result->InsertNextId((int)this->Internals->closestList[i].second);
  }
}

//------------------------------------------------------------------------------
vtkIdType vtkEmbreePointLocator::FindClosestPointWithinRadius(
  double radius, const double x[3], double& dist2)
{
  this->BuildLocator();
  RTCPointQuery query;
  query.x = x[0];
  query.y = x[1];
  query.z = x[2];
  query.radius = radius;
  query.time = 0.f;

  ClosestPointResult qresult;
  qresult.self = this->Internals;

  RTCPointQueryContext context;
  rtcInitPointQueryContext(&context);
  rtcPointQuery(this->Internals->scene, &query, &context, closestPointFunc, (void*)&qresult);

  double vert[3] = { qresult.p.x, qresult.p.y, qresult.p.z };
  dist2 = vtkMath::Distance2BetweenPoints(x, vert);
  return (int)qresult.primID;
}

//------------------------------------------------------------------------------
void vtkEmbreePointLocator::FindPointsWithinRadius(
  double radius, const double x[3], vtkIdList* result)
{
  this->BuildLocator();
  RTCPointQuery query;
  query.x = x[0];
  query.y = x[1];
  query.z = x[2];
  query.radius = radius;
  query.time = 0.f;

  result->Initialize();

  this->Internals->numClosest = -1;
  this->Internals->closestList.clear();
  this->Internals->furthest = 0;

  ClosestPointResult qresult;
  qresult.self = this->Internals;

  RTCPointQueryContext context;
  rtcInitPointQueryContext(&context);
  rtcPointQuery(this->Internals->scene, &query, &context, closestPointsFunc, (void*)&qresult);
  std::sort(this->Internals->closestList.begin(), this->Internals->closestList.end());
  for (int i = 0; i < this->Internals->closestList.size(); i++)
  {
    result->InsertNextId((int)this->Internals->closestList[i].second);
  }
}
