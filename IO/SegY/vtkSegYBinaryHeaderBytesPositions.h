/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkSegYBinaryHeaderBytesPositions_h
#define vtkSegYBinaryHeaderBytesPositions_h

class vtkSegYBinaryHeaderBytesPositions
{
public:
  int JobID;
  int LineNumber;
  int ReelNumber;
  int NumberTracesPerEnsemble;
  int NumberAuxTracesPerEnsemble;
  int SampleInterval;
  int SampleIntervalOriginal;
  int NumSamplesPerTrace;
  int NumSamplesPerTraceOriginal;
  int FormatCode;
  int EnsembleType;
  int MeasurementSystem;
  int ByteOrderingDetection;
  int MajorVersion;
  int MinorVersion;
  int FixedLengthFlag;

public:
  vtkSegYBinaryHeaderBytesPositions() { initDefaultValues(); }

private:
  void initDefaultValues()
  {
    // Default data field positions
    JobID = 3200;
    LineNumber = 3204;
    ReelNumber = 3208;
    NumberTracesPerEnsemble = 3212;
    NumberAuxTracesPerEnsemble = 3214;
    SampleInterval = 3216;
    SampleIntervalOriginal = 3218;
    NumSamplesPerTrace = 3220;
    NumSamplesPerTraceOriginal = 3222;
    FormatCode = 3224;
    EnsembleType = 3228;
    MeasurementSystem = 3254;
    ByteOrderingDetection = 3296;
    MajorVersion = 3500;
    MinorVersion = 3501;
    FixedLengthFlag = 3502;
  }
};


#endif // vtkSegYBinaryHeaderBytesPositions_h
// VTK-HeaderTest-Exclude: vtkSegYBinaryHeaderBytesPositions.h
