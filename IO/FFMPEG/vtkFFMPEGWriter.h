/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkFFMPEGWriter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkFFMPEGWriter
 * @brief   Uses the FFMPEG library to write video files.
 *
 * vtkFFMPEGWriter is an adapter that allows VTK to use the LGPL'd FFMPEG
 * library to write movie files. FFMPEG can create a variety of multimedia
 * file formats and can use a variety of encoding algorithms (codecs).
 * This class creates .avi files containing MP43 encoded video without
 * audio.
 *
 * The FFMPEG multimedia library source code can be obtained from
 * the sourceforge web site at http://ffmpeg.sourceforge.net/download.php
 * or is a tarball along with installation instructions at
 * http://www.vtk.org/files/support/ffmpeg_source.tar.gz
 *
*/

#ifndef vtkFFMPEGWriter_h
#define vtkFFMPEGWriter_h

#include "vtkIOFFMPEGModule.h" // For export macro
#include "vtkGenericMovieWriter.h"

class vtkFFMPEGWriterInternal;

//enum class OutputFormats
//{
//  OUTPUT_FORMAT_AVI,
//  OUTPUT_FORMAT_MP4
//};
//VTK_GENERATE_BITFLAG_OPS(OutputFormats);
//
//std::ostream &operator<<(std::ostream &os, const OutputFormats &format)
//{
//  switch (format)
//  {
//    case OutputFormats::OUTPUT_FORMAT_AVI:
//      os << "avi";
//      return os;
//    case OutputFormats::OUTPUT_FORMAT_MP4:
//      os << "mp4";
//      return os;
//    default:
//      os << "Unsupported format.";
//      return os;
//  }
//}
//
//enum class EncodingMethods
//{
//  ENCODING_METHOD_H264,
//  ENCODING_METHOD_MJPEG
//};
//VTK_GENERATE_BITFLAG_OPS(EncodingMethods);
//
//std::ostream &operator<<(std::ostream &os, const EncodingMethods &method)
//{
//  switch (method)
//  {
//    case EncodingMethods::ENCODING_METHOD_H264:
//      os << "h264";
//      return os;
//    case EncodingMethods::ENCODING_METHOD_MJPEG:
//      os << "mjpeg";
//      return os;
//    default:
//      os << "Unsupported encoding method.";
//      return os;
//  }
//}

class VTKIOFFMPEG_EXPORT vtkFFMPEGWriter : public vtkGenericMovieWriter
{
public:
  static vtkFFMPEGWriter *New();

vtkTypeMacro(vtkFFMPEGWriter, vtkGenericMovieWriter);

  void PrintSelf(ostream &os, vtkIndent indent) override;

  //@{
  /**
   * These methods start writing an Movie file, write a frame to the file
   * and then end the writing process.
   */
  void Start() override;

  void Write() override;

  void End() override;
  //@}

  //@{
  /**
   * Set/Get the compression quality.
   * 0 means worst quality and smallest file size
   * 2 means best quality and largest file size
   */
  vtkSetClampMacro(Quality, int, 0, 2);

  vtkGetMacro(Quality, int);
  //@}

  //@{
  /**
   * Turns on(the default) or off compression.
   * Turning off compression overrides quality setting.
   */
  vtkSetMacro(Compression, bool);

  vtkGetMacro(Compression, bool);

  vtkBooleanMacro(Compression, bool);
  //@}

  //@{
  /**
   * Set/Get the frame rate, in frame/s.
   */
  vtkSetClampMacro(Rate, int, 1, 5000);

  vtkGetMacro(Rate, int);
  //@}

  //@{
  /**
   * Set/Get the bit-rate
   */
  vtkSetMacro(BitRate, int);

  vtkGetMacro(BitRate, int);
  //@}

  //@{
  /**
   * Set/Get the bit-rate tolerance
   */
  vtkSetMacro(BitRateTolerance, int);

  vtkGetMacro(BitRateTolerance, int);
  //@}

  //@{
  /**
   * Set/Get the encoding format. e.g h264
   */
  vtkSetMacro(EncodingMethod, std::string);

  vtkGetMacro(EncodingMethod, std::string);
  //@}

  //@{
  /**
   * Set/Get the name of the encoder. e.g. libopenh264
   */
  vtkSetMacro(CodecName, std::string);

  vtkGetMacro(CodecName, std::string);
  //@}

  //@{
  /**
   * Set/Get the output file format. e.g. mp4
   */
  vtkSetMacro(OutputFormat, std::string);

  vtkGetMacro(OutputFormat, std::string);
  //@}

protected:
  vtkFFMPEGWriter();

  ~vtkFFMPEGWriter();

  vtkFFMPEGWriterInternal *Internals;

  int Initialized;
  int Quality;
  int Rate;
  int BitRate;
  int BitRateTolerance;
  bool Compression;
  std::string EncodingMethod;
  std::string CodecName;
  std::string OutputFormat;

private:
  vtkFFMPEGWriter(const vtkFFMPEGWriter &) = delete;

  void operator=(const vtkFFMPEGWriter &) = delete;
};

#endif
