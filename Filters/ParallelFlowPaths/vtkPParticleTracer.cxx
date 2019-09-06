/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkPParticleTracer.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkPParticleTracer);

vtkPParticleTracer::vtkPParticleTracer()
{
  this->IgnorePipelineTime = 0;
}

int vtkPParticleTracer::OutputParticles(vtkPolyData* poly)
{
  this->Output = poly;
  return 1;
}

void vtkPParticleTracer::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}
