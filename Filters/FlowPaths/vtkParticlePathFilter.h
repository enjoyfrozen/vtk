/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkParticlePathFilter
 * @brief   A Parallel Particle tracer for unsteady vector fields
 *
 * vtkParticlePathFilter is a filter that integrates a vector field to generate particle paths
 *
 *
 * @sa
 * vtkParticlePathFilterBase has the details of the algorithms
*/

#ifndef vtkParticlePathFilter_h
#define vtkParticlePathFilter_h

#include "vtkFiltersFlowPathsModule.h" // For export macro
#include "vtkSmartPointer.h" // For protected ivars.
#include "vtkParticleTracerBase.h"
#include <vector> // For protected ivars

class VTKFILTERSFLOWPATHS_EXPORT ParticlePathFilterInternal
{
public:
  ParticlePathFilterInternal():Filter(nullptr){}
  void Initialize(vtkParticleTracerBase* filter);
  virtual ~ParticlePathFilterInternal(){}
  virtual int OutputParticles(vtkPolyData* poly);
  void SetClearCache(bool clearCache)
  {
    this->ClearCache = clearCache;
  }
  bool GetClearCache()
  {
    return this->ClearCache;
  }
  void Finalize();
  void Reset();

private:
  vtkParticleTracerBase* Filter;
  // Paths doesn't seem to work properly. it is meant to make connecting lines
  // for the particles
  std::vector<vtkSmartPointer<vtkIdList> > Paths;
  bool ClearCache; // false by default
};

class VTKFILTERSFLOWPATHS_EXPORT vtkParticlePathFilter: public vtkParticleTracerBase
{
public:
  vtkTypeMacro(vtkParticlePathFilter,vtkParticleTracerBase)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkParticlePathFilter *New();

protected:
  vtkParticlePathFilter();
  ~vtkParticlePathFilter() override;
  vtkParticlePathFilter(const vtkParticlePathFilter&) = delete;
  void operator=(const vtkParticlePathFilter&) = delete;

  void ResetCache() override;
  int OutputParticles(vtkPolyData* poly) override;
  void InitializeExtraPointDataArrays(vtkPointData* outputPD) override;
  void AppendToExtraPointDataArrays(vtkParticleTracerBaseNamespace::ParticleInformation &) override;

  void Finalize() override;

  //
  // Store any information we need in the output and fetch what we can
  // from the input
  //
  int RequestInformation(vtkInformation* request,
                         vtkInformationVector** inputVector,
                         vtkInformationVector* outputVector) override;

  ParticlePathFilterInternal It;

private:
  vtkDoubleArray* SimulationTime;
  vtkIntArray* SimulationTimeStep;
};

#endif
