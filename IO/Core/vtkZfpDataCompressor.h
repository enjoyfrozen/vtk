/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkZfpDataCompressor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkZfpDataCompressor
 * @brief   Data compression using zfp.
 *
 * vtkZfpDataCompressor provides a concrete vtkDataCompressor class
 * using zfp for compressing and uncompressing data.
*/

#ifndef vtkZfpDataCompressor_h
#define vtkZfpDataCompressor_h

#include "vtkIOCoreModule.h" // For export macro
#include "vtkDataCompressor.h"

class VTKIOCORE_EXPORT vtkZfpDataCompressor : public vtkDataCompressor
{
public:
  vtkTypeMacro(vtkZfpDataCompressor,vtkDataCompressor);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkZfpDataCompressor* New();

  // Description:
  // Get the maximum space that may be needed to store data of the
  // given uncompressed size after compression.  This is the minimum
  // size of the output buffer that can be passed to the four-argument
  // Compress method.
  size_t GetMaximumCompressionSpace(size_t size) override;

  // Description:
  // Get/Set the compression level.
  vtkSetClampMacro(CompressionLevel, int, 0, 9);
  vtkGetMacro(CompressionLevel, int);

  // Get/Set the tolerance
  vtkSetMacro(Tolerance, double);
  vtkGetMacro(Tolerance, double);

  // Get/Set 3 component sizes (zfp compresses each component separately)
  vtkSetMacro(Sx, int);
  vtkGetMacro(Sx, int);
  vtkSetMacro(Sy, int);
  vtkGetMacro(Sy, int);
  vtkSetMacro(Sz, int);
  vtkGetMacro(Sz, int);

protected:
  vtkZfpDataCompressor();
  ~vtkZfpDataCompressor() override;

  int CompressionLevel;
  double Tolerance;
  int Sx, Sy, Sz;

  // Compression method required by vtkDataCompressor.
  size_t CompressBuffer(unsigned char const* uncompressedData,
                        size_t uncompressedSize,
                        unsigned char* compressedData,
                        size_t compressionSpace) override;

  // Decompression method required by vtkDataCompressor.
  size_t UncompressBuffer(unsigned char const* compressedData,
                          size_t compressedSize,
                          unsigned char* uncompressedData,
                          size_t uncompressedSize) override;

private:
  vtkZfpDataCompressor(const vtkZfpDataCompressor&) = delete;
  void operator=(const vtkZfpDataCompressor&) = delete;
};

#endif
