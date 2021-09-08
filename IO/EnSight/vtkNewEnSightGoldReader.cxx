/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkNewEnSightGoldReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkNewEnSightGoldReader.h"

#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkPartitionedDataSet.h"
#include "vtkPartitionedDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredGrid.h"
#include "vtkUniformGrid.h"

#include "vtksys/FStream.hxx"
#include "vtksys/SystemTools.hxx"

#include <cctype> /* isspace */
#include <unordered_set>

namespace
{
constexpr int MAX_LINE_LENGTH = 256;

enum class GridType
{
  Unknown,
  Uniform,
  Rectilinear,
  Curvilinear,
  Unstructured
};

struct GridOptions
{
  GridType Type = GridType::Unknown;
  bool IBlanked = false;
  bool WithGhost = false;
  bool HasRange = false;
};

void evaluateOption(const char* option, GridOptions& opts)
{
  if (strncmp(option, "curvilinear", 11) == 0)
  {
    opts.Type = GridType::Curvilinear;
  }
  else if (strncmp(option, "rectilinear", 11) == 0)
  {
    opts.Type = GridType::Rectilinear;
  }
  else if (strncmp(option, "uniform", 7) == 0)
  {
    opts.Type = GridType::Uniform;
  }
  else if (strncmp(option, "iblanked", 8) == 0)
  {
    opts.IBlanked = true;
  }
  else if (strncmp(option, "with_ghost", 10) == 0)
  {
    opts.WithGhost = true;
  }
  else if (strncmp(option, "range", 5) == 0)
  {
    opts.HasRange = true;
  }
}

GridOptions getGridOptions(const char* line)
{
  GridOptions opts;
  char block[MAX_LINE_LENGTH];
  char s1[MAX_LINE_LENGTH], s2[MAX_LINE_LENGTH], s3[MAX_LINE_LENGTH], s4[MAX_LINE_LENGTH];
  // format of line
  // block <curvilinear/rectilinear/uniform/''> [iblanked] [with_ghost] [range]
  auto num = sscanf(line, "%s %s %s %s %s", block, s1, s2, s3, s4);
  if (num == 1)
  {
    opts.Type = GridType::Curvilinear;
    return opts;
  }
  evaluateOption(s1, opts);
  evaluateOption(s2, opts);
  evaluateOption(s3, opts);
  evaluateOption(s4, opts);

  if (opts.Type == GridType::Unknown)
  {
    opts.Type = GridType::Curvilinear;
  }

  return opts;
}

struct EnSightFile
{
  std::string FileName;
  vtksys::ifstream* Stream;

  EnSightFile();
  ~EnSightFile();

  bool OpenFile();
  bool ReadNextLine(char result[MAX_LINE_LENGTH]);
  bool ReadLine(char result[MAX_LINE_LENGTH]);
  void SkipNLines(vtkIdType n);
};

//------------------------------------------------------------------------------
EnSightFile::EnSightFile()
{
  this->Stream = nullptr;
}

//------------------------------------------------------------------------------
EnSightFile::~EnSightFile()
{
  if (this->Stream)
  {
    if (this->Stream->is_open())
    {
      this->Stream->close();
    }
    delete this->Stream;
    this->Stream = nullptr;
  }
}

//------------------------------------------------------------------------------
bool EnSightFile::OpenFile()
{
  this->Stream = new vtksys::ifstream(this->FileName, ios::in);
  if (this->Stream->fail())
  {
    delete this->Stream;
    this->Stream = nullptr;
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool EnSightFile::ReadNextLine(char result[MAX_LINE_LENGTH])
{
  bool isComment = true;
  bool lineRead = true;

  while (isComment && lineRead)
  {
    lineRead = this->ReadLine(result);
    if (*result && result[0] != '#')
    {
      size_t len = strlen(result);
      unsigned int i = 0;
      while (i < len && (static_cast<unsigned int>(result[i]) <= 255) && isspace(result[i]))
      {
        ++i;
      }
      // If there was only space characters this is a comment, thus skip it
      if (i != len)
      {
        // The line was not empty, not beginning by '#' and not composed
        // of only white space, this is not a comment
        isComment = false;
      }
    }
  }
  return lineRead;
}

//------------------------------------------------------------------------------
// Internal function to read in a line up to MAX_LINE_LENGTH characters.
// Returns zero if there was an error.
bool EnSightFile::ReadLine(char result[MAX_LINE_LENGTH])
{
  this->Stream->getline(result, MAX_LINE_LENGTH);
  if (this->Stream->fail())
  {
    // Reset the error flag before returning. This way, we can keep working
    // if we handle the error downstream.
    this->Stream->clear();
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void EnSightFile::SkipNLines(vtkIdType n)
{
  char line[MAX_LINE_LENGTH];
  for (vtkIdType i = 0; i < n; i++)
  {
    this->ReadNextLine(line);
  }
}

class EnSightFileStream
{
public:
  bool CheckVersion(const char* casefilename);
  bool ParseCaseFile(const char* casefilename);
  bool ReadGeometry(vtkPartitionedDataSetCollection* output);

private:
  void ParseFormatSection();
  void ParseGeometrySection(const char* line);
  void SetGeometryFileName(const char* fname);

  void CreateUniformGridOutput(
    int partId, const char* name, const GridOptions& opts, vtkPartitionedDataSetCollection* output);
  void CreateRectilinearGridOutput(
    int partId, const char* name, const GridOptions& opts, vtkPartitionedDataSetCollection* output);
  void CreateStructuredGridOutput(
    int partId, const char* name, const GridOptions& opts, vtkPartitionedDataSetCollection* output);

  bool IsSectionHeader(const char* line);

  EnSightFile CaseFile;
  EnSightFile GeometryFile;

  std::vector<std::string> FilePath;
  bool IsGoldFile = false;

  std::string GeometryFileName;
  bool NodeIdsListed = false;
  bool ElementIdsListed = false;
};

//------------------------------------------------------------------------------
bool EnSightFileStream::CheckVersion(const char* casefilename)
{
  this->CaseFile.FileName = casefilename;
  if (!this->CaseFile.OpenFile())
  {
    return false;
  }

  char result[MAX_LINE_LENGTH];
  while (this->CaseFile.ReadNextLine(result))
  {
    if (strncmp(result, "FORMAT", 6) == 0)
    {
      this->ParseFormatSection();
      break;
    }
  }
  return this->IsGoldFile;
}

//------------------------------------------------------------------------------
bool EnSightFileStream::IsSectionHeader(const char* line)
{
  std::unordered_set<std::string> headerSet = { "FORMAT", "GEOMETRY", "VARIABLE", "TIME", "FILE" };
  auto it = headerSet.find(line);
  return (it == headerSet.end() ? false : true);
}

//------------------------------------------------------------------------------
bool EnSightFileStream::ParseCaseFile(const char* casefilename)
{
  // has 5 sections: FORMAT, GEOMETRY, VARIABLE, TIME, FILE
  this->CaseFile.FileName = casefilename;
  this->CaseFile.OpenFile();
  auto parentDir = vtksys::SystemTools::GetParentDirectory(casefilename);
  vtksys::SystemTools::SplitPath(parentDir, this->FilePath);

  char result[MAX_LINE_LENGTH];
  auto continueRead = this->CaseFile.ReadNextLine(result);
  while (continueRead)
  {
    if (strncmp(result, "FORMAT", 6) == 0)
    {
      this->ParseFormatSection();
      if (!this->IsGoldFile)
      {
        vtkGenericWarningMacro("This reader handles only EnSight Gold files");
        return false;
      }
    }
    else if (strncmp(result, "GEOMETRY", 8) == 0)
    {
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
        this->ParseGeometrySection(result);
      }
      continue;
    }
    else if (strncmp(result, "VARIABLE", 8) == 0)
    {
      vtkGenericWarningMacro("VARIABLE section not yet supported");
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else if (strncmp(result, "TIME", 4) == 0)
    {
      vtkGenericWarningMacro("TIME section not yet supported");
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else if (strncmp(result, "FILE", 4) == 0)
    {
      vtkGenericWarningMacro("FILE section not yet supported");
      while ((continueRead = this->CaseFile.ReadNextLine(result)))
      {
        if (this->IsSectionHeader(result))
        {
          break;
        }
      }
      continue;
    }
    else
    {
      vtkGenericWarningMacro("ParseCaseFile: invalid line - " << result);
    }
    continueRead = this->CaseFile.ReadNextLine(result);
  }
  return true;
}

//------------------------------------------------------------------------------
void EnSightFileStream::ParseFormatSection()
{
  char line[MAX_LINE_LENGTH], subLine[MAX_LINE_LENGTH], subLine1[MAX_LINE_LENGTH];
  this->CaseFile.ReadNextLine(line);
  sscanf(line, " %*s %s %s", subLine, subLine1);
  if (strncmp(subLine, "ensight", 7) == 0 && strncmp(subLine1, "gold", 4) == 0)
  {
    this->IsGoldFile = true;
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::SetGeometryFileName(const char* fname)
{
  this->FilePath.emplace_back(fname);
  this->GeometryFileName = vtksys::SystemTools::JoinPath(this->FilePath);
  this->FilePath.pop_back();
}

//------------------------------------------------------------------------------
void EnSightFileStream::ParseGeometrySection(const char* line)
{
  char subLine[MAX_LINE_LENGTH];
  if (strncmp(line, "model:", 6) == 0)
  {
    int timeSet, fileSet;
    if (sscanf(line, " %*s %d %d%*[ \t]%s", &timeSet, &fileSet, subLine) == 3)
    {
      vtkGenericWarningMacro("timesets and filesets not supported yet.");
      this->SetGeometryFileName(subLine);
    }
    else if (sscanf(line, " %*s %d%*[ \t]%s", &timeSet, subLine) == 2)
    {
      vtkGenericWarningMacro("timesets not supported yet.");
      this->SetGeometryFileName(subLine);
    }
    else if (sscanf(line, " %*s %s", subLine) == 1)
    {
      this->SetGeometryFileName(subLine);
    }
  }
  else if (strncmp(line, "measured:", 9) == 0)
  {
    vtkGenericWarningMacro("measured files not supported yet");
  }
  else if (strncmp(line, "match:", 6) == 0)
  {
    vtkGenericWarningMacro("match files not supported yet");
  }
  else if (strncmp(line, "boundary:", 9) == 0)
  {
    vtkGenericWarningMacro("boundary files not supported yet");
  }
  else
  {
    vtkGenericWarningMacro("ParseGeometrySection: invalid line - " << line);
  }
}

//------------------------------------------------------------------------------
bool EnSightFileStream::ReadGeometry(vtkPartitionedDataSetCollection* output)
{
  this->GeometryFile.FileName = this->GeometryFileName;
  if (!this->GeometryFile.OpenFile())
  {
    return false;
  }

  char line[MAX_LINE_LENGTH], subLine[MAX_LINE_LENGTH];
  this->GeometryFile.ReadNextLine(line);
  sscanf(line, " %*s %s", subLine);
  if (strncmp(subLine, "Binary", 6) == 0)
  {
    vtkGenericWarningMacro("Binary not yet supported");
    return false;
  }

  // since ascii, description line 1 already read
  // read next one, but we don't care about it
  this->GeometryFile.ReadLine(line);

  // read node id, which can be off/given/assign/ignore
  this->GeometryFile.ReadNextLine(line);
  sscanf(line, " %*s %*s %s", subLine);
  if (strncmp(subLine, "given", 5) == 0 || strncmp(subLine, "ignore", 6) == 0)
  {
    this->NodeIdsListed = true;
  }

  // similarly for element id
  this->GeometryFile.ReadNextLine(line);
  sscanf(line, " %*s %*s %s", subLine);
  if (strncmp(subLine, "given", 5) == 0 || strncmp(subLine, "ignore", 6) == 0)
  {
    this->ElementIdsListed = true;
  }

  // check if extents section is included and handle if so
  auto lineRead = this->GeometryFile.ReadNextLine(line);
  if (strncmp(line, "extents", 7) == 0)
  {
    // Skipping the extent lines for now.
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.ReadNextLine(line);
    lineRead = this->GeometryFile.ReadNextLine(line); // "part"
  }

  while (lineRead && strncmp(line, "part", 4) == 0)
  {
    this->GeometryFile.ReadNextLine(line);
    int partId = atoi(line) - 1; // EnSight starts #ing at 1.

    this->GeometryFile.ReadNextLine(line); // part description line
    char* partName = strdup(line);

    this->GeometryFile.ReadNextLine(line);
    auto opts = getGridOptions(line);
    switch (opts.Type)
    {
      case GridType::Uniform:
        this->CreateUniformGridOutput(partId, partName, opts, output);
        break;
      case GridType::Rectilinear:
        this->CreateRectilinearGridOutput(partId, partName, opts, output);
        break;
      case GridType::Curvilinear:
        this->CreateStructuredGridOutput(partId, partName, opts, output);
        break;
      case GridType::Unstructured:
        vtkGenericWarningMacro("Unstructured grid not supported yet");
        break;
      default:
        vtkGenericWarningMacro("Grid type not correctly specified");
        break;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
void EnSightFileStream::CreateUniformGridOutput(
  int partId, const char* name, const GridOptions& opts, vtkPartitionedDataSetCollection* output)
{
  vtkNew<vtkUniformGrid> data;
  vtkNew<vtkPartitionedDataSet> pds;
  pds->SetPartition(0, data);
  output->SetPartitionedDataSet(partId, pds);

  // TODO set up assembly
  // and set name

  // read dimensions line
  char line[MAX_LINE_LENGTH];
  this->GeometryFile.ReadNextLine(line);
  int dimensions[3];
  sscanf(line, " %d %d %d", &dimensions[0], &dimensions[1], &dimensions[2]);
  data->SetDimensions(dimensions);

  // TODO if it has range, grab it here

  // read origin lines, each value is on a different line
  float origin[3];
  for (int i = 0; i < 3; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    sscanf(line, " %f", &origin[i]);
  }
  data->SetOrigin(origin[0], origin[1], origin[2]);

  // read spacing lines, each value is on a different line
  float delta[3];
  for (int i = 0; i < 3; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    sscanf(line, " %f", &delta[i]);
  }
  data->SetSpacing(delta[0], delta[1], delta[2]);

  auto numPts = dimensions[0] * dimensions[1] * dimensions[2];
  auto numCells = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);
  if (opts.IBlanked)
  {
    for (vtkIdType i = 0; i < numPts; i++)
    {
      this->GeometryFile.ReadNextLine(line);
      if (!atoi(line))
      {
        data->BlankPoint(i);
      }
    }
  }

  if (opts.WithGhost)
  {
    vtkGenericWarningMacro("ghost flags not supported yet");
    // first a line that says ghost_flags
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numCells);
  }

  if (this->NodeIdsListed)
  {
    vtkGenericWarningMacro("node ids not supported yet");
    // first a line that says node_ids
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numPts);
  }

  if (this->ElementIdsListed)
  {
    vtkGenericWarningMacro("element ids not supported yet");
    // first a line that says element_ids
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numCells);
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::CreateRectilinearGridOutput(
  int partId, const char* name, const GridOptions& opts, vtkPartitionedDataSetCollection* output)
{
  vtkNew<vtkRectilinearGrid> data;
  vtkNew<vtkPartitionedDataSet> pds;
  pds->SetPartition(0, data);
  output->SetPartitionedDataSet(partId, pds);

  // TODO set up assembly
  // and set name

  // read dimensions line
  char line[MAX_LINE_LENGTH];
  this->GeometryFile.ReadNextLine(line);
  int dimensions[3];
  sscanf(line, " %d %d %d", &dimensions[0], &dimensions[1], &dimensions[2]);
  data->SetDimensions(dimensions);

  // TODO if it has range, grab it here

  vtkNew<vtkFloatArray> xCoords;
  vtkNew<vtkFloatArray> yCoords;
  vtkNew<vtkFloatArray> zCoords;
  xCoords->Allocate(dimensions[0]);
  yCoords->Allocate(dimensions[1]);
  zCoords->Allocate(dimensions[2]);

  for (vtkIdType i = 0; i < dimensions[0]; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    float val = atof(line);
    xCoords->InsertTuple(i, &val);
  }
  for (vtkIdType i = 0; i < dimensions[1]; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    float val = atof(line);
    yCoords->InsertTuple(i, &val);
  }
  for (vtkIdType i = 0; i < dimensions[2]; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    float val = atof(line);
    zCoords->InsertTuple(i, &val);
  }
  data->SetXCoordinates(xCoords);
  data->SetYCoordinates(yCoords);
  data->SetZCoordinates(zCoords);

  auto numPts = dimensions[0] * dimensions[1] * dimensions[2];
  auto numCells = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);
  if (opts.IBlanked)
  {
    vtkGenericWarningMacro("iblanked not supported for vtkRectilinearGrid");
    this->GeometryFile.SkipNLines(numPts);
  }

  if (opts.WithGhost)
  {
    vtkGenericWarningMacro("ghost flags not supported yet");
    // first a line that says ghost_flags
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numCells);
  }

  if (this->NodeIdsListed)
  {
    vtkGenericWarningMacro("node ids not supported yet");
    // first a line that says node_ids
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numPts);
  }

  if (this->ElementIdsListed)
  {
    vtkGenericWarningMacro("element ids not supported yet");
    // first a line that says element_ids
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numCells);
  }
}

//------------------------------------------------------------------------------
void EnSightFileStream::CreateStructuredGridOutput(
  int partId, const char* name, const GridOptions& opts, vtkPartitionedDataSetCollection* output)
{
  vtkNew<vtkStructuredGrid> data;
  vtkNew<vtkPartitionedDataSet> pds;
  pds->SetPartition(0, data);
  output->SetPartitionedDataSet(partId, pds);

  // TODO set up assembly
  // and set name

  // read dimensions line
  char line[MAX_LINE_LENGTH];
  this->GeometryFile.ReadNextLine(line);
  int dimensions[3];
  sscanf(line, " %d %d %d", &dimensions[0], &dimensions[1], &dimensions[2]);
  data->SetDimensions(dimensions);

  // TODO if it has range, grab it here

  int numPts = dimensions[0] * dimensions[1] * dimensions[2];
  vtkNew<vtkPoints> points;
  points->SetNumberOfPoints(numPts);

  for (vtkIdType i = 0; i < numPts; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    points->SetPoint(i, atof(line), 0.0, 0.0);
  }
  for (vtkIdType i = 0; i < numPts; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    double point[3];
    points->GetPoint(i, point);
    points->SetPoint(i, point[0], atof(line), point[2]);
  }
  for (vtkIdType i = 0; i < numPts; i++)
  {
    this->GeometryFile.ReadNextLine(line);
    double point[3];
    points->GetPoint(i, point);
    points->SetPoint(i, point[0], point[1], atof(line));
  }
  data->SetPoints(points);

  auto numCells = (dimensions[0] - 1) * (dimensions[1] - 1) * (dimensions[2] - 1);
  if (opts.IBlanked)
  {
    for (int i = 0; i < numPts; i++)
    {
      this->GeometryFile.ReadNextLine(line);
      if (!atoi(line))
      {
        data->BlankPoint(i);
      }
    }
  }

  if (opts.WithGhost)
  {
    vtkGenericWarningMacro("ghost flags not supported yet");
    // first a line that says ghost_flags
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numCells);
  }

  if (this->NodeIdsListed)
  {
    vtkGenericWarningMacro("node ids not supported yet");
    // first a line that says node_ids
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numPts);
  }

  if (this->ElementIdsListed)
  {
    vtkGenericWarningMacro("element ids not supported yet");
    // first a line that says element_ids
    this->GeometryFile.ReadNextLine(line);
    this->GeometryFile.SkipNLines(numCells);
  }
}

} // end anon namespace

struct vtkNewEnSightGoldReader::ReaderImpl
{
  EnSightFileStream FileStream;
};

vtkStandardNewMacro(vtkNewEnSightGoldReader);

//----------------------------------------------------------------------------
vtkNewEnSightGoldReader::vtkNewEnSightGoldReader()
{
  this->SetNumberOfInputPorts(0);
  this->CaseFileName = nullptr;
  this->Impl = new ReaderImpl;
}

//----------------------------------------------------------------------------
vtkNewEnSightGoldReader::~vtkNewEnSightGoldReader()
{
  delete[] this->CaseFileName;
  delete this->Impl;
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::CanReadFile(const char* casefilename)
{
  return (this->Impl->FileStream.CheckVersion(casefilename) ? 1 : 0);
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::RequestInformation(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  if (!this->CaseFileName)
  {
    vtkErrorMacro("CaseFileName is null");
    return 0;
  }

  if (!this->Impl->FileStream.ParseCaseFile(this->CaseFileName))
  {
    vtkErrorMacro("Case file " << this->CaseFileName << " could not be parsed without error");
    return 0;
  }
  return 1;
}

//------------------------------------------------------------------------------
int vtkNewEnSightGoldReader::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  vtkPartitionedDataSetCollection* output =
    vtkPartitionedDataSetCollection::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->Impl->FileStream.ReadGeometry(output))
  {
    vtkErrorMacro("Geometry file could not be read");
    return 0;
  }

  return 1;
}

//------------------------------------------------------------------------------
void vtkNewEnSightGoldReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Case FileName: " << (this->CaseFileName ? this->CaseFileName : "(none)") << "\n";
}
