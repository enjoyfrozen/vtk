/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWebGPUViewNodeFactory.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkWebGPUViewNodeFactory
 * @brief Factory that chooses which vtk view nodes to create for webgpu
 *
 */

#ifndef vtkWebGPUViewNodeFactory_h
#define vtkWebGPUViewNodeFactory_h

// VTK includes
#include "vtkRenderingWebGPUModule.h" // for export macro
#include "vtkViewNodeFactory.h"

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGWEBGPU_EXPORT vtkWebGPUViewNodeFactory : public vtkViewNodeFactory
{
public:
  /**
   * Instantiate the class.
   */
  static vtkWebGPUViewNodeFactory* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkWebGPUViewNodeFactory, vtkViewNodeFactory);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

protected:
  vtkWebGPUViewNodeFactory();
  ~vtkWebGPUViewNodeFactory();

  // Helper members

private:
  vtkWebGPUViewNodeFactory(const vtkWebGPUViewNodeFactory&) = delete;
  void operator=(const vtkWebGPUViewNodeFactory) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkWebGPUViewNodeFactory_h
