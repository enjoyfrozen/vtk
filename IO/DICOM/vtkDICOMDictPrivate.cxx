/*=========================================================================
This is an automatically generated file.  Include errata for any changes.
=========================================================================*/

// clang-format off

#include "vtkDICOMDictionary.h"
#include "vtkDICOMDictPrivate.h"

namespace {

typedef vtkDICOMVR VR;
typedef vtkDICOMVM VM;
typedef vtkDICOMDictEntry::Entry DictEntry;

// ----- astm.org/diconde/iod/Component -----

DictEntry Dict048Contents[] = {
{ 0x0011, 0x0023, 0, VR::ST, VM::M1TN, "CADFileFormat" },
{ 0x0011, 0x0024, 0, VR::ST, VM::M1TN, "ComponentReferenceSystem" },
{ 0x0011, 0x0025, 0, VR::ST, VM::M1TN, "ComponentManufacturingProcedure" },
{ 0x0011, 0x0028, 0, VR::ST, VM::M1TN, "ComponentManufacturer" },
{ 0x0011, 0x0030, 0, VR::DS, VM::M1TN, "MaterialThickness" },
{ 0x0011, 0x0032, 0, VR::DS, VM::M1TN, "MaterialPipeDiameter" },
{ 0x0011, 0x0034, 0, VR::DS, VM::M1TN, "MaterialIsolationDiameter" },
{ 0x0011, 0x0042, 0, VR::ST, VM::M1TN, "MaterialGrade" },
{ 0x0011, 0x0044, 0, VR::ST, VM::M1TN, "MaterialPropertiesFileID" },
{ 0x0011, 0x0045, 0, VR::ST, VM::M1TN, "MaterialPropertiesFileFormat" },
{ 0x0011, 0x0046, 0, VR::LT, VM::M1, "MaterialNotes" },
{ 0x0011, 0x0050, 0, VR::CS, VM::M1, "ComponentShape" },
{ 0x0011, 0x0052, 0, VR::CS, VM::M1, "CurvatureType" },
{ 0x0011, 0x0054, 0, VR::DS, VM::M1, "OuterDiameter" },
{ 0x0011, 0x0056, 0, VR::DS, VM::M1, "InnerDiameter" },
};

unsigned short Dict048TagHashTable[] = {
    4,    13,    26,     0,     4,     3,    40,     4,    48,     8,
   68,    12,    82,     6,     2,    37,     6,    52,     7,    66,
    9,    69,    11,    80,    14,    86,     5,     0,    35,     1,
   36,     5,    50,    10,    70,    13,    84,
};

unsigned short Dict048KeyHashTable[] = {
    4,    19,    26,     0,     7,     0, 12978,     1, 64356,     3,
21303,     5, 39023,     6, 45243,     7, 37789,    13, 50453,     3,
    2, 37948,     4, 45877,    11, 40760,     5,     8, 44388,     9,
31640,    10, 23401,    12,   930,    14, 28078,
};

vtkDICOMDictionary::Dict Dict048Data = {
"astm.org/diconde/iod/Component",
3,
15,
Dict048TagHashTable,
Dict048KeyHashTable,
Dict048Contents
};

// ----- astm.org/diconde/iod/ComponentStudy -----

DictEntry Dict060Contents[] = {
{ 0x0009, 0x0020, 0, VR::DA, VM::M1, "ExpiryDate" },
};

unsigned short Dict060TagHashTable[] = {
    2,     0,     1,     0,    32,
};

unsigned short Dict060KeyHashTable[] = {
    2,     0,     1,     0, 42404,
};

vtkDICOMDictionary::Dict Dict060Data = {
"astm.org/diconde/iod/ComponentStudy",
1,
1,
Dict060TagHashTable,
Dict060KeyHashTable,
Dict060Contents
};

// ----- astm.org/diconde/iod/NdeCtImage -----

DictEntry Dict134Contents[] = {
{ 0x0009, 0x0002, 0, VR::IS, VM::M1, "LINACEnergy" },
{ 0x0009, 0x0004, 0, VR::IS, VM::M1, "LINACOutput" },
};

unsigned short Dict134TagHashTable[] = {
    2,     0,     2,     0,     2,     1,     4,
};

unsigned short Dict134KeyHashTable[] = {
    2,     0,     2,     0, 11094,     1, 40989,
};

vtkDICOMDictionary::Dict Dict134Data = {
"astm.org/diconde/iod/NdeCtImage",
1,
2,
Dict134TagHashTable,
Dict134KeyHashTable,
Dict134Contents
};

// ----- astm.org/diconde/iod/NdeDxCalibrationData -----

DictEntry Dict138Contents[] = {
{ 0x0009, 0x0040, 0, VR::SQ, VM::M1, "DarkCurrentSequence" },
{ 0x0009, 0x0050, 0, VR::OW, VM::M1, "DarkCurrentCounts" },
{ 0x0009, 0x0060, 0, VR::SQ, VM::M1, "GainCorrectionReferenceSequence" },
{ 0x0009, 0x0070, 0, VR::OW, VM::M1, "AirCounts" },
{ 0x0009, 0x0071, 0, VR::DS, VM::M1, "KVUsedInGainCalibration" },
{ 0x0009, 0x0072, 0, VR::DS, VM::M1, "MAUsedInGainCalibration" },
{ 0x0009, 0x0073, 0, VR::DS, VM::M1, "NumberOfFramesUsedForIntegration" },
{ 0x0009, 0x0074, 0, VR::LO, VM::M1, "FilterMaterialUsedInGainCalibration" },
{ 0x0009, 0x0075, 0, VR::DS, VM::M1, "FilterThicknessUsedInGainCalibration" },
{ 0x0009, 0x0076, 0, VR::DA, VM::M1, "DateOfGainCalibration" },
{ 0x0009, 0x0077, 0, VR::TM, VM::M1, "TimeOfGainCalibration" },
{ 0x0009, 0x0080, 0, VR::OB, VM::M1, "BadPixelImage" },
{ 0x0009, 0x0099, 0, VR::LT, VM::M1, "CalibrationNotes" },
};

unsigned short Dict138TagHashTable[] = {
    4,    13,    24,     0,     4,     0,    64,     3,   112,     6,
  115,     9,   118,     5,     1,    80,     4,   113,     7,   116,
   10,   119,    11,   128,     4,     2,    96,     5,   114,     8,
  117,    12,   153,
};

unsigned short Dict138KeyHashTable[] = {
    4,    11,    28,     0,     3,     0, 38625,     5, 57718,     6,
51679,     8,     1, 46199,     2, 12225,     3, 63476,     7, 45592,
    8, 62295,     9, 25578,    11, 65296,    12,  6343,     2,     4,
11441,    10, 32346,
};

vtkDICOMDictionary::Dict Dict138Data = {
"astm.org/diconde/iod/NdeDxCalibrationData",
3,
13,
Dict138TagHashTable,
Dict138KeyHashTable,
Dict138Contents
};

// ----- astm.org/diconde/iod/NdeUsEquipmentSettings -----

DictEntry Dict226Contents[] = {
{ 0x0009, 0x0020, 0, VR::SQ, VM::M1, "PulserSettingsSequence" },
{ 0x0009, 0x0022, 0, VR::DS, VM::M1, "PulseWidth" },
{ 0x0009, 0x0024, 0, VR::DS, VM::M1, "ExcitationFrequency" },
{ 0x0009, 0x0026, 0, VR::CS, VM::M1, "ModulationType" },
{ 0x0009, 0x0028, 0, VR::DS, VM::M1, "Damping" },
{ 0x0009, 0x0030, 0, VR::SQ, VM::M1, "ReceiverSettingsSequence" },
{ 0x0009, 0x0031, 0, VR::DS, VM::M1, "AcquiredSoundpathLength" },
{ 0x0009, 0x0032, 0, VR::CS, VM::M1, "AcquisitionCompressionType" },
{ 0x0009, 0x0033, 0, VR::IS, VM::M1, "AcquisitionSampleSize" },
{ 0x0009, 0x0034, 0, VR::DS, VM::M1, "RectifierSmoothing" },
{ 0x0009, 0x0035, 0, VR::SQ, VM::M1, "DACSequence" },
{ 0x0009, 0x0036, 0, VR::CS, VM::M1, "DACType" },
{ 0x0009, 0x0038, 0, VR::DS, VM::M1TN, "DACGainPoints" },
{ 0x0009, 0x003A, 0, VR::DS, VM::M1TN, "DACTimePoints" },
{ 0x0009, 0x003C, 0, VR::DS, VM::M1TN, "DACAmplitude" },
{ 0x0009, 0x0040, 0, VR::SQ, VM::M1, "PreAmplifierSettingsSequence" },
{ 0x0009, 0x0050, 0, VR::SQ, VM::M1, "TransmitTransducerSettingsSequence" },
{ 0x0009, 0x0051, 0, VR::SQ, VM::M1, "ReceiveTransducerSettingsSequence" },
{ 0x0009, 0x0052, 0, VR::DS, VM::M1, "IncidentAngle" },
{ 0x0009, 0x0054, 0, VR::ST, VM::M1, "CouplingTechnique" },
{ 0x0009, 0x0056, 0, VR::ST, VM::M1, "CouplingMedium" },
{ 0x0009, 0x0057, 0, VR::DS, VM::M1, "CouplingVelocity" },
{ 0x0009, 0x0058, 0, VR::DS, VM::M1, "CrystalCenterLocationX" },
{ 0x0009, 0x0059, 0, VR::DS, VM::M1, "CrystalCenterLocationZ" },
{ 0x0009, 0x005A, 0, VR::DS, VM::M1, "SoundPathLength" },
{ 0x0009, 0x005C, 0, VR::ST, VM::M1, "DelayLawIdentifier" },
{ 0x0009, 0x0060, 0, VR::SQ, VM::M1, "GateSettingsSequence" },
{ 0x0009, 0x0062, 0, VR::DS, VM::M1, "GateThreshold" },
{ 0x0009, 0x0064, 0, VR::DS, VM::M1, "VelocityOfSound" },
{ 0x0009, 0x0070, 0, VR::SQ, VM::M1, "CalibrationSettingsSequence" },
{ 0x0009, 0x0072, 0, VR::ST, VM::M1, "CalibrationProcedure" },
{ 0x0009, 0x0074, 0, VR::SH, VM::M1, "ProcedureVersion" },
{ 0x0009, 0x0076, 0, VR::DA, VM::M1, "ProcedureCreationDate" },
{ 0x0009, 0x0078, 0, VR::DA, VM::M1, "ProcedureExpirationDate" },
{ 0x0009, 0x007A, 0, VR::DA, VM::M1, "ProcedureLastModifiedDate" },
{ 0x0009, 0x007C, 0, VR::TM, VM::M1TN, "CalibrationTime" },
{ 0x0009, 0x007E, 0, VR::DA, VM::M1TN, "CalibrationDate" },
};

unsigned short Dict226TagHashTable[] = {
   10,    25,    30,    37,    44,    53,    64,    73,    84,     0,
    7,     2,    36,    11,    54,    15,    64,    18,    82,    23,
   89,    28,   100,    32,   118,     2,    24,    90,    36,   126,
    3,     3,    38,     6,    49,    19,    84,     3,     5,    48,
   25,    92,    29,   112,     4,     8,    51,    12,    56,    20,
   86,    33,   120,     5,     0,    32,     7,    50,    21,    87,
   26,    96,    30,   114,     4,     4,    40,    10,    53,    13,
   58,    34,   122,     5,     1,    34,     9,    52,    16,    80,
   27,    98,    31,   116,     4,    14,    60,    17,    81,    22,
   88,    35,   124,
};

unsigned short Dict226KeyHashTable[] = {
   10,    19,    26,    33,    46,    55,    60,    71,    84,     0,
    4,    12, 38433,    26, 17744,    31, 46356,    35, 23932,     3,
   19,   115,    22,  9240,    27,  2338,     3,     5, 29810,     6,
14081,    30, 64756,     6,     3, 54208,     4,  5810,     9, 57087,
   17, 33498,    23,  9240,    33, 17644,     4,    10, 34482,    11,
48147,    13, 46381,    25,  4607,     2,     0,   541,    34,  6300,
    5,     2, 54945,     7,  2428,    20, 15833,    24, 15807,    36,
24637,     6,     1, 40975,    14,  2067,    15, 33800,    21, 46590,
   29, 23248,    32, 39210,     4,     8,  1963,    16, 11284,    18,
56354,    28, 47290,
};

vtkDICOMDictionary::Dict Dict226Data = {
"astm.org/diconde/iod/NdeUsEquipmentSettings",
9,
37,
Dict226TagHashTable,
Dict226KeyHashTable,
Dict226Contents
};

// ----- astm.org/diconde/iod/NdeCtDetector -----

DictEntry Dict243Contents[] = {
{ 0x0009, 0x0011, 0, VR::DS, VM::M1, "InternalDetectorFrameTime" },
{ 0x0009, 0x0012, 0, VR::DS, VM::M1, "NumberOfFramesIntegrated" },
{ 0x0009, 0x0020, 0, VR::SQ, VM::M1, "DetectorTemperatureSequence" },
{ 0x0009, 0x0022, 0, VR::DS, VM::M1, "SensorName" },
{ 0x0009, 0x0024, 0, VR::DS, VM::M1, "HorizontalOffsetOfSensor" },
{ 0x0009, 0x0026, 0, VR::DS, VM::M1, "VerticalOffsetOfSensor" },
{ 0x0009, 0x0028, 0, VR::DS, VM::M1, "SensorTemperature" },
};

unsigned short Dict243TagHashTable[] = {
    2,     0,     7,     0,    17,     1,    18,     2,    32,     3,
   34,     4,    36,     5,    38,     6,    40,
};

unsigned short Dict243KeyHashTable[] = {
    2,     0,     7,     0, 10230,     1, 48680,     2, 59974,     3,
15776,     4, 52933,     5, 36981,     6, 18637,
};

vtkDICOMDictionary::Dict Dict243Data = {
"astm.org/diconde/iod/NdeCtDetector",
1,
7,
Dict243TagHashTable,
Dict243KeyHashTable,
Dict243Contents
};

// ----- astm.org/diconde/iod/NDEGeometry -----

DictEntry Dict266Contents[] = {
{ 0x0021, 0x0002, 0, VR::IS, VM::M1, "CoordinateSystemNumberOfAxes" },
{ 0x0021, 0x0004, 0, VR::SQ, VM::M1, "CoordinateSystemAxesSequence" },
{ 0x0021, 0x0006, 0, VR::ST, VM::M1, "CoordinateSystemAxisDescription" },
{ 0x0021, 0x0008, 0, VR::CS, VM::M1, "CoordinateSystemDataSetMapping" },
{ 0x0021, 0x000A, 0, VR::IS, VM::M1, "CoordinateSystemAxisNumber" },
{ 0x0021, 0x000C, 0, VR::CS, VM::M1, "CoordinateSystemAxisType" },
{ 0x0021, 0x000E, 0, VR::CS, VM::M1, "CoordinateSystemAxisUnits" },
{ 0x0021, 0x0010, 0, VR::OB, VM::M1, "CoordinateSystemAxisValues" },
{ 0x0021, 0x0020, 0, VR::SQ, VM::M1, "CoordinateSystemTransformSequence" },
{ 0x0021, 0x0022, 0, VR::ST, VM::M1, "TransformDescription" },
{ 0x0021, 0x0024, 0, VR::IS, VM::M1, "TransformNumberOfAxes" },
{ 0x0021, 0x0026, 0, VR::IS, VM::M1TN, "TransformOrderOfAxes" },
{ 0x0021, 0x0028, 0, VR::CS, VM::M1, "TransformedAxisUnits" },
{ 0x0021, 0x002A, 0, VR::DS, VM::M1TN, "CoordinateSystemTransformRotationAndScaleMatrix" },
{ 0x0021, 0x002C, 0, VR::DS, VM::M1TN, "CoordinateSystemTransformTranslationMatrix" },
};

unsigned short Dict266TagHashTable[] = {
    4,    13,    26,     0,     4,     2,     6,     5,    12,     9,
   34,    12,    40,     6,     1,     4,     4,    10,     7,    16,
    8,    32,    11,    38,    14,    44,     5,     0,     2,     3,
    8,     6,    14,    10,    36,    13,    42,
};

unsigned short Dict266KeyHashTable[] = {
    4,     9,    20,     0,     2,     1,  2708,     8, 37901,     5,
    0, 31040,     2, 40270,     4, 41957,    11, 59670,    14, 30875,
    8,     3,  7734,     5, 29325,     6,  5608,     7, 22791,     9,
 7361,    10, 48378,    12, 46000,    13, 53828,
};

vtkDICOMDictionary::Dict Dict266Data = {
"astm.org/diconde/iod/NDEGeometry",
3,
15,
Dict266TagHashTable,
Dict266KeyHashTable,
Dict266Contents
};

// ----- astm.org/diconde/iod/NdeIndication -----

DictEntry Dict278Contents[] = {
{ 0x0021, 0x0002, 0, VR::SQ, VM::M1, "EvaluatorSequence" },
{ 0x0021, 0x0004, 0, VR::IS, VM::M1, "EvaluatorNumber" },
{ 0x0021, 0x0006, 0, VR::PN, VM::M1, "EvaluatorName" },
{ 0x0021, 0x0008, 0, VR::IS, VM::M1, "EvaluationAttempt" },
{ 0x0021, 0x0012, 0, VR::SQ, VM::M1, "IndicationSequence" },
{ 0x0021, 0x0014, 0, VR::IS, VM::M1, "IndicationNumber" },
{ 0x0021, 0x0016, 0, VR::SH, VM::M1, "IndicationLabel" },
{ 0x0021, 0x0018, 0, VR::ST, VM::M1, "IndicationDescription" },
{ 0x0021, 0x001A, 0, VR::CS, VM::M1TN, "IndicationType" },
{ 0x0021, 0x001C, 0, VR::CS, VM::M1, "IndicationDisposition" },
{ 0x0021, 0x001E, 0, VR::SQ, VM::M1, "IndicationROISequence" },
{ 0x0021, 0x0030, 0, VR::SQ, VM::M1, "IndicationPhysicalPropertySequence" },
{ 0x0021, 0x0032, 0, VR::SH, VM::M1, "PropertyLabel" },
};

unsigned short Dict278TagHashTable[] = {
    4,    13,    22,     0,     4,     2,     6,     4,    18,     7,
   24,    10,    30,     4,     1,     4,     6,    22,     9,    28,
   12,    50,     5,     0,     2,     3,     8,     5,    20,     8,
   26,    11,    48,
};

unsigned short Dict278KeyHashTable[] = {
    4,    11,    22,     0,     3,     3, 58772,     7, 14745,    11,
15350,     5,     0,  2992,     1, 16576,     5,  4485,     9, 64617,
   10, 17155,     5,     2, 57789,     4, 30474,     6, 61559,     8,
63533,    12,  5016,
};

vtkDICOMDictionary::Dict Dict278Data = {
"astm.org/diconde/iod/NdeIndication",
3,
13,
Dict278TagHashTable,
Dict278KeyHashTable,
Dict278Contents
};

// ----- astm.org/diconde/iod/NdeUsEquipment -----

DictEntry Dict310Contents[] = {
{ 0x0009, 0x0002, 0, VR::SQ, VM::M1, "PulserEquipmentSequence" },
{ 0x0009, 0x0004, 0, VR::CS, VM::M1, "PulserType" },
{ 0x0009, 0x0006, 0, VR::LT, VM::M1, "PulserNotes" },
{ 0x0009, 0x0008, 0, VR::SQ, VM::M1, "ReceiverEquipmentSequence" },
{ 0x0009, 0x000A, 0, VR::CS, VM::M1, "AmplifierType" },
{ 0x0009, 0x000C, 0, VR::LT, VM::M1, "ReceiverNotes" },
{ 0x0009, 0x000E, 0, VR::SQ, VM::M1, "PreAmplifierEquipmentSequence" },
{ 0x0009, 0x000F, 0, VR::LT, VM::M1, "PreAmplifierNotes" },
{ 0x0009, 0x0010, 0, VR::SQ, VM::M1, "TransmitTransducerSequence" },
{ 0x0009, 0x0011, 0, VR::SQ, VM::M1, "ReceiveTransducerSequence" },
{ 0x0009, 0x0012, 0, VR::US, VM::M1, "NumberOfElements" },
{ 0x0009, 0x0013, 0, VR::CS, VM::M1, "ElementShape" },
{ 0x0009, 0x0014, 0, VR::DS, VM::M1, "ElementDimensionA" },
{ 0x0009, 0x0015, 0, VR::DS, VM::M1, "ElementDimensionB" },
{ 0x0009, 0x0016, 0, VR::DS, VM::M1, "ElementPitch" },
{ 0x0009, 0x0017, 0, VR::DS, VM::M1, "MeasuredBeamDimensionA" },
{ 0x0009, 0x0018, 0, VR::DS, VM::M1, "MeasuredBeamDimensionB" },
{ 0x0009, 0x0019, 0, VR::DS, VM::M1, "LocationOfMeasuredBeamDiameter" },
{ 0x0009, 0x001A, 0, VR::DS, VM::M1, "NominalFrequency" },
{ 0x0009, 0x001B, 0, VR::DS, VM::M1, "MeasuredCenterFrequency" },
{ 0x0009, 0x001C, 0, VR::DS, VM::M1, "MeasuredBandwidth" },
};

unsigned short Dict310TagHashTable[] = {
    6,    15,    30,    37,    46,     0,     4,     2,     6,     5,
   12,     8,    16,    15,    23,     7,     0,     2,     3,     8,
    7,    15,    11,    19,    14,    22,    17,    25,    20,    28,
    3,     6,    14,    10,    18,    16,    24,     4,     1,     4,
    4,    10,    13,    21,    19,    27,     3,     9,    17,    12,
   20,    18,    26,
};

unsigned short Dict310KeyHashTable[] = {
    6,    13,    24,    33,    44,     0,     3,     4,  2464,    12,
33598,    16, 57560,     5,     2, 58056,     8,   194,    10, 45651,
   13, 33598,    18, 28212,     4,     1, 52992,     3, 39381,     7,
 5340,    14, 19553,     5,     5, 18592,     6, 27383,     9,  8997,
   11, 64505,    17, 43502,     4,     0, 17865,    15, 57559,    19,
61282,    20, 30908,
};

vtkDICOMDictionary::Dict Dict310Data = {
"astm.org/diconde/iod/NdeUsEquipment",
5,
21,
Dict310TagHashTable,
Dict310KeyHashTable,
Dict310Contents
};

// ----- astm.org/diconde/iod/ComponentSeries -----

DictEntry Dict330Contents[] = {
{ 0x0009, 0x0010, 0, VR::ST, VM::M1, "ActualEnvironmentalConditions" },
{ 0x0009, 0x0040, 0, VR::ST, VM::M1, "EnvironmentalConditions" },
};

unsigned short Dict330TagHashTable[] = {
    2,     0,     2,     0,    16,     1,    64,
};

unsigned short Dict330KeyHashTable[] = {
    2,     0,     2,     0,  8283,     1, 22657,
};

vtkDICOMDictionary::Dict Dict330Data = {
"astm.org/diconde/iod/ComponentSeries",
1,
2,
Dict330TagHashTable,
Dict330KeyHashTable,
Dict330Contents
};

// ----- astm.org/diconde/iod/NdeCtCalibrationData -----

DictEntry Dict390Contents[] = {
{ 0x0009, 0x0040, 0, VR::SQ, VM::M1, "DarkCurrentSequence" },
{ 0x0009, 0x0050, 0, VR::OW, VM::M1, "DarkCurrentCounts" },
{ 0x0009, 0x0060, 0, VR::SQ, VM::M1, "GainCorrectionReferenceSequence" },
{ 0x0009, 0x0070, 0, VR::OW, VM::M1, "AirCounts" },
{ 0x0009, 0x0071, 0, VR::DS, VM::M1, "KVUsedInGainCalibration" },
{ 0x0009, 0x0072, 0, VR::DS, VM::M1, "MAUsedInGainCalibration" },
{ 0x0009, 0x0073, 0, VR::DS, VM::M1, "NumberOfFramesUsedForIntegration" },
{ 0x0009, 0x0074, 0, VR::LO, VM::M1, "FilterMaterialUsedInGainCalibration" },
{ 0x0009, 0x0075, 0, VR::DS, VM::M1, "FilterThicknessUsedInGainCalibration" },
{ 0x0009, 0x0076, 0, VR::DA, VM::M1, "DateOfGainCalibration" },
{ 0x0009, 0x0077, 0, VR::TM, VM::M1, "TimeOfGainCalibration" },
{ 0x0009, 0x0080, 0, VR::OB, VM::M1, "BadPixelImage" },
{ 0x0009, 0x0099, 0, VR::LT, VM::M1, "CalibrationNotes" },
};

unsigned short Dict390TagHashTable[] = {
    4,    13,    24,     0,     4,     0,    64,     3,   112,     6,
  115,     9,   118,     5,     1,    80,     4,   113,     7,   116,
   10,   119,    11,   128,     4,     2,    96,     5,   114,     8,
  117,    12,   153,
};

unsigned short Dict390KeyHashTable[] = {
    4,    11,    28,     0,     3,     0, 38625,     5, 57718,     6,
51679,     8,     1, 46199,     2, 12225,     3, 63476,     7, 45592,
    8, 62295,     9, 25578,    11, 65296,    12,  6343,     2,     4,
11441,    10, 32346,
};

vtkDICOMDictionary::Dict Dict390Data = {
"astm.org/diconde/iod/NdeCtCalibrationData",
3,
13,
Dict390TagHashTable,
Dict390KeyHashTable,
Dict390Contents
};

// ----- astm.org/diconde/iod/NdeDxDetector -----

DictEntry Dict397Contents[] = {
{ 0x0009, 0x0011, 0, VR::DS, VM::M1, "InternalDetectorFrameTime" },
{ 0x0009, 0x0012, 0, VR::DS, VM::M1, "NumberOfFramesIntegrated" },
{ 0x0009, 0x0020, 0, VR::SQ, VM::M1, "DetectorTemperatureSequence" },
{ 0x0009, 0x0022, 0, VR::DS, VM::M1, "SensorName" },
{ 0x0009, 0x0024, 0, VR::DS, VM::M1, "HorizontalOffsetOfSensor" },
{ 0x0009, 0x0026, 0, VR::DS, VM::M1, "VerticalOffsetOfSensor" },
{ 0x0009, 0x0028, 0, VR::DS, VM::M1, "SensorTemperature" },
};

unsigned short Dict397TagHashTable[] = {
    2,     0,     7,     0,    17,     1,    18,     2,    32,     3,
   34,     4,    36,     5,    38,     6,    40,
};

unsigned short Dict397KeyHashTable[] = {
    2,     0,     7,     0, 10230,     1, 48680,     2, 59974,     3,
15776,     4, 52933,     5, 36981,     6, 18637,
};

vtkDICOMDictionary::Dict Dict397Data = {
"astm.org/diconde/iod/NdeDxDetector",
1,
7,
Dict397TagHashTable,
Dict397KeyHashTable,
Dict397Contents
};

vtkDICOMDictionary::Dict *PrivateDictData[] = {
&Dict048Data, &Dict060Data, &Dict134Data, &Dict138Data, &Dict226Data,
&Dict243Data, &Dict266Data, &Dict278Data, &Dict310Data, &Dict330Data,
&Dict390Data, &Dict397Data, NULL
};

} // end anonymous namespace

static unsigned int vtkDICOMDictPrivateInitializerCounter;

vtkDICOMDictPrivateInitializer::vtkDICOMDictPrivateInitializer()
{
  if (vtkDICOMDictPrivateInitializerCounter++ == 0)
  {
    for (vtkDICOMDictionary::Dict **dp = PrivateDictData; *dp != NULL; dp++)
    {
      vtkDICOMDictionary::AddPrivateDictionary(*dp);
    }
  }
}

vtkDICOMDictPrivateInitializer::~vtkDICOMDictPrivateInitializer()
{
  if (--vtkDICOMDictPrivateInitializerCounter == 0)
  {
    for (vtkDICOMDictionary::Dict **dp = PrivateDictData; *dp != NULL; dp++)
    {
      vtkDICOMDictionary::RemovePrivateDictionary((*dp)->Name);
    }
  }
}

// clang-format on
