/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPStreaklineFilter
 * @brief   A Parallel Particle tracer for unsteady vector fields
 *
 * vtkPStreaklineFilter is a filter that integrates a vector field to generate
 *
 *
 * @sa
 * vtkPStreaklineFilterBase has the details of the algorithms
*/

#ifndef vtkPStreaklineFilter_h
#define vtkPStreaklineFilter_h

#include "vtkSmartPointer.h" // For protected ivars.
#include "vtkStreaklineFilter.h" //for utility
#include "vtkPParticleTracerBase.h"
#include "vtkFiltersParallelFlowPathsModule.h" // For export macro

class  VTKFILTERSPARALLELFLOWPATHS_EXPORT vtkPStreaklineFilter: public vtkPParticleTracerBase
{
 public:
  vtkTypeMacro(vtkPStreaklineFilter,vtkPParticleTracerBase)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkPStreaklineFilter *New();

 protected:
  vtkPStreaklineFilter();
  ~vtkPStreaklineFilter(){}
  vtkPStreaklineFilter(const vtkPStreaklineFilter&) = delete;
  void operator=(const vtkPStreaklineFilter&) = delete;
  virtual int OutputParticles(vtkPolyData* poly) override;
  virtual void Finalize() override;

  StreaklineFilterInternal It;
};


#endif
