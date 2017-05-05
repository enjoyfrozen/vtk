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

namespace
{

struct vtkZfpField
{
  vtkZfpField(zfp_field* field)
  {
    this->Field = field;
  }

  ~vtkZfpField()
  {
    zfp_field_free(this->Field);
  }

  operator zfp_field* () const
  {
    return this->Field;
  }

  zfp_field* Field;
};

struct vtkZfpStream
{
  vtkZfpStream(zfp_stream* stream)
  {
    this->Stream = stream;
  }

  ~vtkZfpStream()
  {
    zfp_stream_close(this->Stream);
  }

  operator zfp_stream* () const
  {
    return this->Stream;
  }

  zfp_stream* Stream;
};

struct vtkZfpBitstream
{
  vtkZfpBitstream(bitstream* bitstream)
  {
    this->Bitstream = bitstream;
  }

  ~vtkZfpBitstream()
  {
    stream_close(this->Bitstream);
  }

  operator bitstream* () const
  {
    return this->Bitstream;
  }

  bitstream* Bitstream;
};

}

//----------------------------------------------------------------------------
size_t
vtkZfpDataCompressor::CompressBuffer(unsigned char const* uncompressedData,
                                      size_t vtkNotUsed(uncompressedSize),
                                      unsigned char* compressedData,
                                      size_t compressionSpace)
{
  vtkZfpStream stream(zfp_stream_open(NULL));
  size_t outsize;
  zfp_type type = zfp_type_none;

  /*Use single precision for now.  Not sure if we need double or not*/
  type = zfp_type_float;
  if ((this->Nx == 0) && (this->Ny == 0) && (this->Nz == 0))
  {
    vtkErrorMacro("Error! Please update zfp dimesnions (Nx, Ny, Nz)!");
    return 0;
  }
  vtkZfpField field(zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz));
  //Setup ZFP Stream
  zfp_stream_set_accuracy(stream, this->Tolerance, type);
  zfp_stream_maximum_size(stream, field);
  zfp_field_set_pointer(field, (char *)(uncompressedData ));
  /*compress using zfp */
  if (this->NumComponents == 3)
  {
    vtkZfpField fieldx(zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz));
    vtkZfpField fieldy(zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz));
    vtkZfpField fieldz(zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz));

    zfp_field_set_stride_3d(fieldx, 3,this->Nx*3, this->Nx*this->Ny*3);
    zfp_field_set_stride_3d(fieldy, 3,this->Nx*3, this->Nx*this->Ny*3);
    zfp_field_set_stride_3d(fieldz, 3,this->Nx*3, this->Nx*this->Ny*3);

    zfp_field_set_pointer(fieldx, (char *)(uncompressedData ));
    zfp_field_set_pointer(fieldy, (char *)(uncompressedData + sizeof(float)));
    zfp_field_set_pointer(fieldz, (char *)(uncompressedData + 2*sizeof(float)));
    int position = 0;
    /*Associate the output data with the zfp stream*/
    {
      vtkZfpBitstream outstream(stream_open(compressedData, compressionSpace));
      zfp_stream_set_bit_stream(stream, outstream);
      outsize = zfp_compress(stream, fieldx);
      position = position + (int)outsize;
    }
    /*Copy compressed stream to output and set size of X*/
    {
      this->SetSx(outsize);
      vtkZfpBitstream outstream(stream_open(compressedData + position, compressionSpace - position));
      zfp_stream_set_bit_stream(stream, outstream);
      outsize = zfp_compress(stream, fieldy);
    }
    {
      position = (int)outsize + position;
      vtkZfpBitstream outstream(stream_open(compressedData + position, compressionSpace - position));
      zfp_stream_set_bit_stream(stream, outstream);
      outsize = zfp_compress(stream, fieldz);
    }
    this->SetSz(outsize);
    outsize = (int)position + outsize;
  }
  else
  {
    /*Associate the output data with the zfp stream*/
    vtkZfpBitstream outstream(stream_open(compressedData, compressionSpace));
    /* associate bit stream with compressed stream */

    zfp_stream_set_bit_stream(stream, outstream);
    outsize = zfp_compress(stream, field);
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
  vtkZfpStream stream(zfp_stream_open(NULL));
  zfp_type type = zfp_type_float;
  vtkZfpField field(zfp_field_3d(NULL, type, this->Nx, this->Ny, this->Nz));
  zfp_stream_set_accuracy(stream, this->Tolerance, type);
  int result;
  /*Uncompress*/
  if (this->NumComponents == 3)
  {
    zfp_field_set_pointer(field, uncompressedData);
    zfp_field_set_stride_3d(field, 3,this->Nx*3, this->Nx*this->Ny*3); //1st part
    /*Associate the compressed data with the zfp stream*/
    {
      vtkZfpBitstream compstream(stream_open((char *)compressedData, this->GetSx()));
      zfp_stream_set_bit_stream(stream, compstream);
      result = zfp_decompress(stream, field);
      if (!result)
      {
         vtkErrorMacro("Zfp error while decompressing data.");
         return 0;
      }
    }
    {
      vtkZfpBitstream compstream(stream_open((char *)(compressedData+this->GetSx()), this->GetSy()));
      zfp_stream_set_bit_stream(stream, compstream);
      zfp_field_set_pointer(field, (char *)(uncompressedData + 1* sizeof(float))); //2nd part
      result = zfp_decompress(stream, field);
      if (!result)
      {
        vtkErrorMacro("Zfp error while decompressing data.");
        return 0;
      }
      zfp_field_set_pointer(field, (char *)(uncompressedData + 2*sizeof(float))); //3rd part
    }
    {
      vtkZfpBitstream compstream(stream_open((char *)(compressedData+this->GetSx()+this->GetSy()), this->GetSz()));
      zfp_stream_set_bit_stream(stream, compstream);
      result = zfp_decompress(stream, field);
      if (!result)
      {
        vtkErrorMacro("Zfp error while decompressing data.");
        return 0;
      }
    }
  }
  else
  {
    /*Associate the compressed data with the zfp stream*/
    vtkZfpBitstream compstream(stream_open((char *)compressedData, compressedSize));
    zfp_stream_set_bit_stream(stream, compstream);
    zfp_field_set_pointer(field, uncompressedData);
    result = zfp_decompress(stream, field);
    if (!result)
    {
      vtkErrorMacro("Zfp error while decompressing data.");
      return 0;
    }
  }
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
