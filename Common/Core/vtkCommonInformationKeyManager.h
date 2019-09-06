/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkCommonInformationKeyManager
 * @brief   Manages key types in vtkCommon.
 *
 * vtkCommonInformationKeyManager is included in the header of any
 * subclass of vtkInformationKey defined in the vtkCommon library.
 * It makes sure that the table of keys is created before and
 * destroyed after it is used.
*/

#ifndef vtkCommonInformationKeyManager_h
#define vtkCommonInformationKeyManager_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkSystemIncludes.h"

#include "vtkDebugLeaksManager.h" // DebugLeaks exists longer than info keys.

class vtkInformationKey;

class VTKCOMMONCORE_EXPORT vtkCommonInformationKeyManager
{
public:
  vtkCommonInformationKeyManager();
  ~vtkCommonInformationKeyManager();

  /**
   * Called by constructors of vtkInformationKey subclasses defined in
   * vtkCommon to register themselves with the manager.  The
   * instances will be deleted when vtkCommon is unloaded on
   * program exit.
   */
  static void Register(vtkInformationKey* key);

private:
  // Unimplemented
  vtkCommonInformationKeyManager(const vtkCommonInformationKeyManager&);
  vtkCommonInformationKeyManager& operator=(
    const vtkCommonInformationKeyManager&);

  static void ClassInitialize();
  static void ClassFinalize();
};

// This instance will show up in any translation unit that uses key
// types defined in vtkCommon or that has a singleton.  It will
// make sure vtkCommonInformationKeyManager's vector of keys is
// initialized before and destroyed after it is used.
static vtkCommonInformationKeyManager vtkCommonInformationKeyManagerInstance;

#endif
// VTK-HeaderTest-Exclude: vtkCommonInformationKeyManager.h
