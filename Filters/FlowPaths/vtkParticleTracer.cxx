/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkParticleTracer.h"
#include "vtkObjectFactory.h"
#include "vtkSetGet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

vtkObjectFactoryNewMacro(vtkParticleTracer)

vtkParticleTracer::vtkParticleTracer()
{
  this->IgnorePipelineTime = 0;
}

int vtkParticleTracer::OutputParticles(vtkPolyData* poly)
{
  this->Output = poly;
  return 1;
}

void vtkParticleTracer::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}
