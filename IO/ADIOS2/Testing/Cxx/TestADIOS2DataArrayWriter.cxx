// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include "vtkADIOS2DataArrayWriter.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkLogger.h"
#include "vtkTestUtilities.h"
#include "vtkTypeInt32Array.h"

#include <string>
#include <vector>
#include <vtksys/SystemTools.hxx>

#include <adios2.h>

#if VTK_MODULE_ENABLE_VTK_ParallelMPI
#include "vtkMPIController.h"
#endif

namespace
{
std::string GetTestFileName(int argc, char** argv, const std::string& basename)
{
  char* testDirectory =
    vtkTestUtilities::GetArgOrEnvOrDefault("-T", argc, argv, "VTK_TEMP_DIR", "Testing/Temporary");
  std::vector<std::string> directoryPathComponents;
  vtksys::SystemTools::SplitPath(testDirectory, directoryPathComponents);
  directoryPathComponents.push_back(basename);
  delete[] testDirectory;
  return vtksys::SystemTools::JoinPath(directoryPathComponents);
}
}

// Create a bp file using vtkADIOS2DataArrayWriter and then read it using plain adios2 API.
int TestADIOS2DataArrayWriter(int argc, char* argv[])
{
  const std::string filepath = ::GetTestFileName(argc, argv, "TestADIOS2DataArrayWriter.bp");
  vtksys::SystemTools::RemoveADirectory(filepath);

#if VTK_MODULE_ENABLE_VTK_ParallelMPI
  vtkNew<vtkMPIController> controller;
  controller->Initialize(&argc, &argv);

  vtkMultiProcessController::SetGlobalController(controller);
  size_t rank = controller->GetLocalProcessId();
#else
  size_t rank = 0;
#endif

  vtkNew<vtkDoubleArray> dArray;
  vtkNew<vtkTypeInt32Array> intArray;
  for (int i = 0; i < 10; i++)
  {
    dArray->InsertNextValue(i / 10.0 + rank);
    intArray->InsertNextValue(i + 10 * rank);
  }
  const std::string testString = "This is a test string";

  vtkNew<vtkADIOS2DataArrayWriter> writer;
  writer->SetFileName(filepath.c_str());
  writer->SetWriterType(vtkADIOS2DataArrayWriter::ADIOS2WriterType::BP4);
  writer->Open();
  writer->AddArray(dArray, "double_array");
  writer->AddArray(intArray, "int_array");
  writer->AddAttribute(testString, "testString");
  writer->Close();

  // now use ADIOS2  API to read in the file
  adios2::ADIOS adios;
  adios2::IO io = adios.DeclareIO("Input");
  adios2::Engine reader = io.Open(filepath, adios2::Mode::ReadRandomAccess);

  if (!reader)
  {
    vtkLogF(ERROR, "Could not open file");
    return EXIT_FAILURE;
  }

  adios2::Variable<double> dVariable = io.InquireVariable<double>("double_array");
  if (!dVariable)
  {
    vtkLogF(ERROR, "Variable double_array does not exist in %s", filepath.c_str());
    return EXIT_FAILURE;
  }
  std::vector<double> dData;
  reader.Get<double>(dVariable, dData, adios2::Mode::Sync);

  size_t startIndex = rank * 10;
  size_t endIndex = (rank + 1) * 10;

  auto epsilon_compare = [](double a, double b) {
    return std::abs(a - b) < std::numeric_limits<double>::epsilon();
  };

  if (!std::equal(
        dData.begin() + startIndex, dData.begin() + endIndex, dArray->Begin(), epsilon_compare))
  {
    vtkLogF(ERROR, "expected values in double arrays do not match");
    return EXIT_FAILURE;
  }

  adios2::Variable<int32_t> iVariable = io.InquireVariable<int32_t>("int_array");
  if (!iVariable)
  {
    vtkLogF(ERROR, "Variable int_array does not exist in %s", filepath.c_str());
    return EXIT_FAILURE;
  }
  std::vector<int32_t> iData;
  reader.Get<int32_t>(iVariable, iData, adios2::Mode::Sync);

  if (!std::equal(iData.begin() + startIndex, iData.begin() + endIndex, intArray->Begin()))
  {
    vtkLogF(ERROR, "expected values in integer arrays do not match");
    return EXIT_FAILURE;
  }

  adios2::Attribute<std::string> attribute = io.InquireAttribute<std::string>("testString");
  if (!attribute)
  {
    vtkLogF(ERROR, "Attribute testString does not exist in %s", filepath.c_str());
    return EXIT_FAILURE;
  }

  if (attribute.Data()[0] != testString)
  {
    vtkLogF(ERROR, "expected value in testString attribute is wrong");
    return EXIT_FAILURE;
  }

#if VTK_MODULE_ENABLE_VTK_ParallelMPI
  controller->Finalize();
#endif

  return EXIT_SUCCESS;
}
