/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkObjectIdMap
 * @brief   class used to assign Id to any VTK object and be able
 * to retrieve it base on its id.
*/

#ifndef vtkObjectIdMap_h
#define vtkObjectIdMap_h

#include "vtkObject.h"
#include "vtkWebCoreModule.h" // needed for exports

class VTKWEBCORE_EXPORT vtkObjectIdMap : public vtkObject
{
public:
  static vtkObjectIdMap* New();
  vtkTypeMacro(vtkObjectIdMap, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Retrieve a unique identifier for the given object or generate a new one
   * if its global id was never requested.
   */
  vtkTypeUInt32 GetGlobalId(vtkObject* obj);

  /**
   * Retrieve a vtkObject based on its global id. If not found return nullptr
   */
  vtkObject* GetVTKObject(vtkTypeUInt32 globalId);

  /**
   * Assign an active key (string) to an existing object.
   * This is usually used to provide another type of access to specific
   * vtkObject that we want to retrieve easily using a string.
   * Return the global Id of the given registered object
   */
  vtkTypeUInt32 SetActiveObject(const char* objectType, vtkObject* obj);

  /**
   * Retrieve a previously stored object based on a name
   */
  vtkObject* GetActiveObject(const char* objectType);

  /**
   * Remove any internal reference count due to internal Id/Object mapping
   */
  void FreeObject(vtkObject* obj);

protected:
  vtkObjectIdMap();
  ~vtkObjectIdMap() override;

private:
  vtkObjectIdMap(const vtkObjectIdMap&) = delete;
  void operator=(const vtkObjectIdMap&) = delete;

  struct vtkInternals;
  vtkInternals* Internals;

};

#endif
