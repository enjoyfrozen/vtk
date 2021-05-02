/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTGAReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkTGAReader
 * @brief   read PNG files
 *
 * vtkTGAReader is a source object that reads PNG files.
 * It should be able to read most any PNG file
 *
 * @sa
 * vtkPNGWriter
 */

#ifndef vtkTGAReader_h
#define vtkTGAReader_h

#include "vtkIOImageModule.h" // For export macro
#include "vtkImageReader2.h"

class VTKIOIMAGE_EXPORT vtkTGAReader : public vtkImageReader2
{
public:
  static vtkTGAReader* New();
  vtkTypeMacro(vtkTGAReader, vtkImageReader2);

  /**
   * Is the given file a valid TGA file?
   */
  int CanReadFile(const char* fname) override;

  /**
   * Get the file extensions for this format.
   * Returns a string with a space separated list of extensions in
   * the format .extension
   */
  const char* GetFileExtensions() override { return ".tga"; }

  /**
   * Return a descriptive name for the file format that might be useful in a GUI.
   */
  const char* GetDescriptiveName() override { return "Targa"; }

protected:
  vtkTGAReader() = default;
  ~vtkTGAReader() override = default;

  void ExecuteInformation() override;

private:
  vtkTGAReader(const vtkTGAReader&) = delete;
  void operator=(const vtkTGAReader&) = delete;
};
#endif
