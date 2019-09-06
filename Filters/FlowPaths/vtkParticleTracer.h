/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkParticleTracer
 * @brief   A Parallel Particle tracer for unsteady vector fields
 *
 * vtkParticleTracer is a filter that integrates a vector field to advect particles
 *
 *
 * @sa
 * vtkParticleTracerBase has the details of the algorithms
*/

#ifndef vtkParticleTracer_h
#define vtkParticleTracer_h

#include "vtkFiltersFlowPathsModule.h" // For export macro
#include "vtkSmartPointer.h" // For protected ivars.
#include "vtkParticleTracerBase.h"

class VTKFILTERSFLOWPATHS_EXPORT vtkParticleTracer: public vtkParticleTracerBase
{
 public:
  vtkTypeMacro(vtkParticleTracer,vtkParticleTracerBase)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkParticleTracer *New();

 protected:
  vtkParticleTracer();
  ~vtkParticleTracer() override{}
  vtkParticleTracer(const vtkParticleTracer&) = delete;
  void operator=(const vtkParticleTracer&) = delete;
  int OutputParticles(vtkPolyData* poly) override;
};


#endif
