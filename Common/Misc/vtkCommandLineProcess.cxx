/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCommandLineProcess.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkCommandLineProcess.h"
#include "vtkObjectFactory.h"

#include "vtksys/Process.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace details
{
// trim from start
static inline void ltrim(std::string& s)
{
  s.erase(s.begin(),
    std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

// trim from end
static inline void rtrim(std::string& s)
{
  s.erase(
    std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
    s.end());
}

// trim from both ends
static inline void trim(std::string& s)
{
  ltrim(s);
  rtrim(s);
}
}

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkCommandLineProcess);

//------------------------------------------------------------------------------
vtkCommandLineProcess::vtkCommandLineProcess()
{
  this->SetStdErr("");
  this->SetStdOut("");
}

//------------------------------------------------------------------------------
vtkCommandLineProcess::~vtkCommandLineProcess()
{
  this->SetStdErr(nullptr);
  this->SetStdOut(nullptr);
}

//------------------------------------------------------------------------------
void vtkCommandLineProcess::Execute()
{
  if (this->Command && strlen(this->Command))
  {
    std::string command(this->GetCommand());
    details::trim(command);

    // split command line into command and parameters
    std::istringstream iss(command);
    std::vector<std::string> stringOwnership;
    do
    {
      std::string sub;
      iss >> sub;
      if (sub.empty())
      {
        continue;
      }
      stringOwnership.emplace_back(sub);
    } while (iss);

    // get a vector of C string for vtksys
    std::vector<const char*> stringViewC(stringOwnership.size());
    for (std::size_t i = 0; i < stringOwnership.size(); ++i)
    {
      stringViewC[i] = stringOwnership[i].c_str();
    }
    stringViewC.push_back(nullptr);

    // Configure and launch process
    vtksysProcess* process = vtksysProcess_New();
    vtksysProcess_SetCommand(process, &stringViewC[0]);
    vtksysProcess_SetPipeShared(process, vtksysProcess_Pipe_STDOUT, 0);
    vtksysProcess_SetPipeShared(process, vtksysProcess_Pipe_STDERR, 0);
    vtksysProcess_SetTimeout(process, std::max(0.0, this->Timeout));
    vtksysProcess_Execute(process);

    // Get ouput streams
    char* data = nullptr;
    int length = 0;
    int pipe;
    std::string out;
    std::string err;
    // While loop needed because there is a limit to the buffer size of
    // the vtksysProcess streams. If output is too big we have to append.
    do
    {
      char* cp;
      int length;
      pipe = vtksysProcess_WaitForData(process, &cp, &length, nullptr);
      switch (pipe)
      {
        case vtksysProcess_Pipe_STDOUT:
          out.append(std::string(cp, length));
          break;

        case vtksysProcess_Pipe_STDERR:
          err.append(std::string(cp, length));
          break;
      }
    } while (pipe != vtksysProcess_Pipe_None);

    // Exit properly
    vtksysProcess_WaitForExit(process, nullptr);
    vtksysProcess_Delete(process);

    // Trim last whitespaces
    details::rtrim(out);
    details::rtrim(err);
    this->SetStdOut(out.c_str());
    this->SetStdErr(err.c_str());
  }
}

//------------------------------------------------------------------------------
void vtkCommandLineProcess::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Command: " << this->GetCommand() << std::endl;
  os << indent << "Timeout: " << this->GetTimeout() << std::endl;
}
