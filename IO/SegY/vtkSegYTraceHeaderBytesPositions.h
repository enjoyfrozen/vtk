/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkSegYTraceHeaderBytesPositions_h
#define vtkSegYTraceHeaderBytesPositions_h

class vtkSegYTraceHeaderBytesPositions
{
public:
  int TraceNumber;
  int InlineNumber;
  int CrosslineNumber;
  int TraceNumberWithinEnsemble;
  int CoordinateMultiplier;
  int CoordinateUnits;
  int NumberSamples;
  int SampleInterval;

public:
  vtkSegYTraceHeaderBytesPositions()
  {
    initDefaultValues();
  }

private:
  void initDefaultValues()
  {
    TraceNumber = 0;
    InlineNumber = 8;
    CrosslineNumber = 20;
    TraceNumberWithinEnsemble = 24;
    CoordinateMultiplier = 70;
    CoordinateUnits = 88;
    NumberSamples = 114;
    SampleInterval = 116;
  }
};

#endif // vtkSegYTraceHeaderBytesPositions_h
// VTK-HeaderTest-Exclude: vtkSegYTraceHeaderBytesPositions.h
