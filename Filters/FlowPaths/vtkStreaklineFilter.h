/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkStreaklineFilter
 * @brief   A Parallel Particle tracer for unsteady vector fields
 *
 * vtkStreaklineFilter is a filter that integrates a vector field to generate streak lines
 *
 *
 * @sa
 * vtkParticleTracerBase has the details of the algorithms
*/

#ifndef vtkStreaklineFilter_h
#define vtkStreaklineFilter_h

#include "vtkFiltersFlowPathsModule.h" // For export macro
#include "vtkSmartPointer.h" // For protected ivars.
#include "vtkParticleTracerBase.h"

class VTKFILTERSFLOWPATHS_EXPORT StreaklineFilterInternal
{
public:
  StreaklineFilterInternal():Filter(nullptr){}
  void Initialize(vtkParticleTracerBase* filter);
  virtual ~StreaklineFilterInternal(){}
  virtual int OutputParticles(vtkPolyData* poly);
  void Finalize();
  void Reset();
private:
  vtkParticleTracerBase* Filter;

};


class VTKFILTERSFLOWPATHS_EXPORT vtkStreaklineFilter: public vtkParticleTracerBase
{
 public:
  vtkTypeMacro(vtkStreaklineFilter,vtkParticleTracerBase)
  void PrintSelf(ostream& os, vtkIndent indent) override;

  static vtkStreaklineFilter *New();

 protected:
  vtkStreaklineFilter();
  ~vtkStreaklineFilter() override{}
  vtkStreaklineFilter(const vtkStreaklineFilter&) = delete;
  void operator=(const vtkStreaklineFilter&) = delete;
  int OutputParticles(vtkPolyData* poly) override;
  void Finalize() override;

  StreaklineFilterInternal It;
};


#endif
