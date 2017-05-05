/*=========================================================================
  Program:   Visualization Toolkit
  Module:    vtkZfpDataCompressor.cxx

  Wrapper for ZFP created by Stephen Hamilton
  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkZfpDataCompressor.h"
#include "vtkObjectFactory.h"
#include "vtk_zfp.h"


vtkStandardNewMacro(vtkZfpDataCompressor);

//----------------------------------------------------------------------------
vtkZfpDataCompressor::vtkZfpDataCompressor()
{ //This needs to be modified for zfp values.
  //this->CompressionLevel = Z_DEFAULT_COMPRESSION;
  this->Tolerance = 1e-4; /*This is the default. */
}

//----------------------------------------------------------------------------
vtkZfpDataCompressor::~vtkZfpDataCompressor()
{
}

//----------------------------------------------------------------------------
void vtkZfpDataCompressor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "CompressionLevel: " << this->CompressionLevel << endl;
  os << indent << "ZFP Extent Nx: " << this->Nx << ", Ny: " << this->Ny << ", Nz: " << this->Nz << endl;
  os << indent << "Tolerance: " << this->Tolerance << endl;
  os << indent << "NumComponents: " << this->NumComponents << endl;
}

//----------------------------------------------------------------------------
size_t
vtkZfpDataCompressor::CompressBuffer(unsigned char const* uncompressedData,
                                      size_t vtkNotUsed(uncompressedSize),
                                      unsigned char* compressedData,
                                      size_t compressionSpace)
{
  zfp_stream* zfp_stream;
  size_t outsize;
  bitstream* outstream;
  bitstream* tempstream;
  zfp_field* field, *fieldx, *fieldy, *fieldz;
  zfp_type type = zfp_type_none;
  zfp_stream = zfp_stream_open(NULL);

  /*Use single precision for now.  Not sure if we need double or not*/
  type = zfp_type_float;
  if ((this->Nx == 0) && (this->Ny == 0) && (this->Nz == 0))
  {
    vtkErrorMacro("Error! Please update zfp dimesnions (Nx, Ny, Nz)!");
    return 0;
  }
  field = zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz);
  //Setup ZFP Stream
  zfp_stream_set_accuracy(zfp_stream, this->Tolerance, type);
  zfp_stream_maximum_size(zfp_stream, field);
  zfp_field_set_pointer(field, (char *)(uncompressedData ));
  /*compress using zfp */
  if (this->NumComponents == 3)
  {
    fieldx = zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz);
    fieldy = zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz);
    fieldz = zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz);

    zfp_field_set_stride_3d(fieldx, 3,this->Nx*3, this->Nx*this->Ny*3);
    zfp_field_set_stride_3d(fieldy, 3,this->Nx*3, this->Nx*this->Ny*3);
    zfp_field_set_stride_3d(fieldz, 3,this->Nx*3, this->Nx*this->Ny*3);

    zfp_field_set_pointer(fieldx, (char *)(uncompressedData ));
    zfp_field_set_pointer(fieldy, (char *)(uncompressedData + sizeof(float)));
    zfp_field_set_pointer(fieldz, (char *)(uncompressedData + 2*sizeof(float)));
    int position = 0;
    /*Associate the output data with the zfp stream*/
    outstream = stream_open(compressedData, compressionSpace);
    zfp_stream_set_bit_stream(zfp_stream, outstream);
    outsize = zfp_compress(zfp_stream, fieldx);
    position = position + (int)outsize;
    stream_close(outstream);
    /*Copy compressed stream to output and set size of X*/
    this->SetSx(outsize);
    outstream = stream_open(compressedData+position, compressionSpace-position);
    zfp_stream_set_bit_stream(zfp_stream, outstream);
    outsize = zfp_compress(zfp_stream, fieldy);
    stream_close(outstream);
    this->SetSy(outsize);
    position = (int)outsize + position;
    outstream = stream_open(compressedData + position, compressionSpace-position);
    zfp_stream_set_bit_stream(zfp_stream, outstream);
    outsize = zfp_compress(zfp_stream, fieldz);
    stream_close(outstream);
    this->SetSz(outsize);
    outsize = (int)position + outsize;
  }
  else
  {
    /*Associate the output data with the zfp stream*/
    outstream = stream_open(compressedData, compressionSpace);
    /* associate bit stream with compressed stream */

    zfp_stream_set_bit_stream(zfp_stream, outstream);
    outsize = zfp_compress(zfp_stream, field);
    //vtkWarningMacro("Single component.  Compressed output size: " << outsize);
    stream_close(outstream);
  }
  if (outsize == 0)
  {
    vtkErrorMacro("Zfp error while compressing data.");
    return 0;
  }
  return outsize;
}

//----------------------------------------------------------------------------
size_t
vtkZfpDataCompressor::UncompressBuffer(unsigned char const* compressedData,
                                        size_t compressedSize,
                                        unsigned char* uncompressedData,
                                        size_t vtkNotUsed(uncompressedSize))
{
  zfp_stream* zfp_stream;
  zfp_stream = zfp_stream_open(NULL);
  bitstream* compstream;
  zfp_type type = zfp_type_float;
  zfp_field* field =zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz);
  zfp_stream_set_accuracy(zfp_stream, this->Tolerance, type);
  int result;
  /*Uncompress*/
  if (this->NumComponents == 3)
  {
    zfp_field_set_pointer(field, uncompressedData);
    zfp_field_set_stride_3d(field, 3,this->Nx*3, this->Nx*this->Ny*3); //1st part
    /*Associate the compressed data with the zfp stream*/
    compstream = stream_open((char *)compressedData, this->GetSx());
    zfp_stream_set_bit_stream(zfp_stream, compstream);
    result = zfp_decompress(zfp_stream, field);
    if (!result)
    {
      vtkErrorMacro("Zfp error while decompressing data.");
      return 0;
    }
    stream_close(compstream);
    compstream = stream_open((char *)(compressedData+this->GetSx()), this->GetSy());
    zfp_stream_set_bit_stream(zfp_stream, compstream);
    zfp_field_set_pointer(field, (char *)(uncompressedData + 1* sizeof(float))); //2nd part
    result = zfp_decompress(zfp_stream, field);
    if (!result)
    {
      vtkErrorMacro("Zfp error while decompressing data.");
      return 0;
    }
    zfp_field_set_pointer(field, (char *)(uncompressedData + 2*sizeof(float))); //3rd part
    compstream = stream_open((char *)(compressedData+this->GetSx()+this->GetSy()), this->GetSz());
    zfp_stream_set_bit_stream(zfp_stream, compstream);
    result = zfp_decompress(zfp_stream, field);
    if (!result)
    {
      vtkErrorMacro("Zfp error while decompressing data.");
      return 0;
    }
  }
  else
  {
    /*Associate the compressed data with the zfp stream*/
    compstream = stream_open((char *)compressedData, compressedSize);
    zfp_stream_set_bit_stream(zfp_stream, compstream);
    zfp_field_set_pointer(field, uncompressedData);
    result = zfp_decompress(zfp_stream, field);
    if (!result)
    {
      vtkErrorMacro("Zfp error while decompressing data.");
      return 0;
    }
  }
  zfp_field_free(field);
  zfp_stream_close(zfp_stream);
  return result;
}

//----------------------------------------------------------------------------
/* Change the following to zfp function */
size_t
vtkZfpDataCompressor::GetMaximumCompressionSpace(size_t size)
{
  // ZLib specifies that destination buffer must be 0.1% larger + 12 bytes.
  return size + (size+999)/1000 + 12;
}
