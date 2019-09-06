/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkOpenGLVertexBufferObjectCache.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"
#include "vtkOpenGLVertexBufferObject.h"


// ----------------------------------------------------------------------------
vtkStandardNewMacro(vtkOpenGLVertexBufferObjectCache);

// ----------------------------------------------------------------------------
vtkOpenGLVertexBufferObjectCache::vtkOpenGLVertexBufferObjectCache() = default;

// ----------------------------------------------------------------------------
vtkOpenGLVertexBufferObjectCache::~vtkOpenGLVertexBufferObjectCache() = default;

void vtkOpenGLVertexBufferObjectCache::RemoveVBO(
  vtkOpenGLVertexBufferObject *vbo)
{
  vtkOpenGLVertexBufferObjectCache::VBOMap::iterator iter =
    this->MappedVBOs.begin();
  while(iter != this->MappedVBOs.end())
  {
    if(iter->second == vbo)
    {
      iter->first->UnRegister(this);
      this->MappedVBOs.erase(iter++);
    }
    else
    {
      ++iter;
    }
  }
}

// ----------------------------------------------------------------------------
vtkOpenGLVertexBufferObject* vtkOpenGLVertexBufferObjectCache::GetVBO(
  vtkDataArray *array, int destType)
{
  // Check array is valid
  if (array == nullptr || array->GetNumberOfTuples() == 0)
  {
    vtkErrorMacro( << "Cannot get VBO for empty array.");
    return nullptr;
  }

  // Look for VBO in map
  VBOMap::const_iterator iter = this->MappedVBOs.find(array);
  if (iter != this->MappedVBOs.end())
  {
    vtkOpenGLVertexBufferObject* vbo = iter->second;
    vbo->SetDataType(destType);
    vbo->Register(this);
    return vbo;
  }

  // If vbo not found, create new one
  // Initialize new vbo
  vtkOpenGLVertexBufferObject* vbo = vtkOpenGLVertexBufferObject::New();
  vbo->SetCache(this);
  vbo->SetDataType(destType);
  array->Register(this);

  // Add vbo to map
  this->MappedVBOs[array] = vbo;
  return vbo;
}

// ----------------------------------------------------------------------------
void vtkOpenGLVertexBufferObjectCache::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
