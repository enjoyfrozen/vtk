#include "vtkCommandLineProcess.h"
#include "vtkNew.h"

#include <iostream>
#include <string>

int TestCommandLineProcess(int, char*[])
{
  vtkNew<vtkCommandLineProcess> process;
  process->SetCommand("echo Hello World");
  process->Execute();
  std::string out = process->GetStdOut();
  std::string err = process->GetStdErr();

  if (out != "Hello World")
  {
    std::cerr << "FAILED: wrong command output" << std::endl;
    return EXIT_FAILURE;
  }
  if (!err.empty())
  {
    std::cerr << "FAILED: there is output in the error stream" << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
