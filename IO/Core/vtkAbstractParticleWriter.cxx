/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkAbstractParticleWriter.h"
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Construct with no start and end write methods or arguments.
vtkAbstractParticleWriter::vtkAbstractParticleWriter()
{
  this->TimeStep     = 0;
  this->TimeValue    = 0.0;
  this->FileName     = nullptr;
  this->CollectiveIO = 0;
}
//----------------------------------------------------------------------------
vtkAbstractParticleWriter::~vtkAbstractParticleWriter()
{
  delete []this->FileName;
  this->FileName = nullptr;
}
//----------------------------------------------------------------------------
void vtkAbstractParticleWriter::SetWriteModeToCollective()
{
  this->SetCollectiveIO(1);
}
//----------------------------------------------------------------------------
void vtkAbstractParticleWriter::SetWriteModeToIndependent()
{
  this->SetCollectiveIO(0);
}
//----------------------------------------------------------------------------
void vtkAbstractParticleWriter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
  os << indent << "TimeStep: " << this->TimeStep << endl;
  os << indent << "TimeValue: " << this->TimeValue << endl;
  os << indent << "CollectiveIO: " << this->CollectiveIO << endl;
  os << indent << "FileName: " <<
    (this->FileName ? this->FileName : "NONE") << endl;
}
