/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkmDataSetMapper.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkmDataSetMapper
 * @brief vtkmDatSetMapper is a mapper to map vtkmDataSets to graphics primitives.
 * This class is a placeholder that is overriden when OSPRay is enabled to forward vtkmDataSets
 * directly to the OSPRay renderer nodes.
 */

#ifndef vtkmDataSetMapper_h
#define vtkmDataSetMapper_h

#include "vtkDataSetMapper.h"
#include "vtkRenderingCoreModule.h" // For export macro

VTK_ABI_NAMESPACE_BEGIN
// Forward declarations

class VTKRENDERINGCORE_EXPORT vtkmDataSetMapper : public vtkDataSetMapper
{
public:
  /**
   * Instantiate the class.
   */
  static vtkmDataSetMapper* New();

  ///@{
  /**
   * Standard methods for the VTK class.
   */
  vtkTypeMacro(vtkmDataSetMapper, vtkDataSetMapper);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  ///@}

  /**
   * Overridden to warn about lack of OSPRay if not overridden.
   */
  void Render(vtkRenderer* ren, vtkActor* act) override;

protected:
  vtkmDataSetMapper() {}
  ~vtkmDataSetMapper() {}

  // Helper members
  // see algorithm for more info
  int FillInputPortInformation(int port, vtkInformation* info) override;

private:
  vtkmDataSetMapper(const vtkmDataSetMapper&) = delete;
  void operator=(const vtkmDataSetMapper) = delete;
};

VTK_ABI_NAMESPACE_END
#endif // vtkmDataSetMapper_h
