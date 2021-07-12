/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkCommandLineProcess.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef vtkCommandLineProcess_h
#define vtkCommandLineProcess_h

#include "vtkCommonMiscModule.h" // For export macro
#include "vtkObject.h"

/**
 * @class   vtkCommandLineProcess
 * @brief   Launch a process on the current machine and get its output
 *
 * Launch a process on the current machine and get its standard output and
 * standard error output. The executable needs to be available in the current
 * PATH.
 */
class VTKCOMMONMISC_EXPORT vtkCommandLineProcess : public vtkObject
{
public:
  static vtkCommandLineProcess* New();
  vtkTypeMacro(vtkCommandLineProcess, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  vtkCommandLineProcess();
  virtual ~vtkCommandLineProcess() override;

  /**
   * Execute the command currently set if any.
   * This will update the StdOut and StdErr properties.
   */
  void Execute();

  //@{
  /**
   * Set/Get command timeout in seconds.
   */
  vtkSetMacro(Timeout, double);
  vtkGetMacro(Timeout, double);
  //@}

  //@{
  /**
   * Set/Get command to execute.
   */
  vtkGetStringMacro(Command);
  vtkSetStringMacro(Command);
  //@}

  //@{
  /**
   * Get output of the current command.
   */
  vtkGetStringMacro(StdOut);
  vtkGetStringMacro(StdErr);
  //@}

protected:
  vtkSetStringMacro(StdOut);
  vtkSetStringMacro(StdErr);

private:
  double Timeout = 5;
  char* Command = nullptr;

  char* StdOut = nullptr;
  char* StdErr = nullptr;
};

#endif // vtkCommandLineProcess_h
