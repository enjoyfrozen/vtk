/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPParticleTracer
 * @brief   A Parallel Particle tracer for unsteady vector fields
 *
 * vtkPParticleTracer is a filter that integrates a vector field to generate
 *
 *
 * @sa
 * vtkPParticleTracerBase has the details of the algorithms
*/

#ifndef vtkPParticleTracer_h
#define vtkPParticleTracer_h

#include "vtkSmartPointer.h" // For protected ivars.
#include "vtkPParticleTracerBase.h"

#include "vtkFiltersParallelFlowPathsModule.h" // For export macro

class  VTKFILTERSPARALLELFLOWPATHS_EXPORT vtkPParticleTracer: public vtkPParticleTracerBase
{
 public:
  vtkTypeMacro(vtkPParticleTracer,vtkPParticleTracerBase)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkPParticleTracer *New();

 protected:
  vtkPParticleTracer();
  ~vtkPParticleTracer(){}
  virtual int OutputParticles(vtkPolyData* poly) override;
private:
  vtkPParticleTracer(const vtkPParticleTracer&) = delete;
  void operator=(const vtkPParticleTracer&) = delete;
};


#endif
