// SPDX-FileCopyrightText: Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
// SPDX-License-Identifier: BSD-3-Clause

#include <vtkDelimitedTextReader.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkStringArray.h>
#include <vtkTable.h>
#include <vtkTestUtilities.h>

namespace
{
//------------------------------------------------------------------------------
bool CheckOutput(vtkTable* table, int nbCols, int nbRows)
{
  if (table->GetNumberOfRows() != nbRows)
  {
    cout << "ERROR: Wrong number of rows: " << table->GetNumberOfRows() << endl;
    return false;
  }
  if (table->GetNumberOfColumns() != nbCols)
  {
    cout << "ERROR: Wrong number of columns: " << table->GetNumberOfColumns() << endl;
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool TestReadFromString()
{
  std::string inputString = ",awesomeness,fitness,region\r\nAbby,1,2,china\r\nBob,5,0.2,US\r\nCatie,3,0."
                       "3,UK\r\nDavid,2,100,UK\r\nGrace,4,20,US\r\nIlknur,6,5,Turkey\r\n";
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetHaveHeaders(true);
  reader->SetReadFromInputString(true);
  reader->SetInputString(inputString);
  reader->SetDetectNumericColumns(true);
  reader->Update();

  return CheckOutput(reader->GetOutput(), 4, 6);
}

//------------------------------------------------------------------------------
bool TestDefault(int argc, char* argv[])
{
  char* filepath = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/authors.csv");
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetFileName(filepath);
  reader->Update();
  delete filepath;
  
  return CheckOutput(reader->GetOutput(), 6, 7);
}

//------------------------------------------------------------------------------
bool TestHeaders(int argc, char* argv[])
{
  char* filepath = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/authors.csv");
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetFileName(filepath);
  reader->Update();
  delete filepath;

  bool ret = CheckOutput(reader->GetOutput(), 6, 7);

  reader->SetHaveHeaders(true);
  reader->Update();
  ret |= CheckOutput(reader->GetOutput(), 6, 6);

  return ret;
}

//------------------------------------------------------------------------------
bool TestDelimiters(int argc, char* argv[])
{
  char* filepath = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/delimited.txt");
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetFileName(filepath);
  reader->SetHaveHeaders(true);
  reader->Update();
  delete filepath;

  bool ret = CheckOutput(reader->GetOutput(), 1, 5);

  reader->SetFieldDelimiterCharacters(":");
  reader->Update();
  vtkTable* table = reader->GetOutput();
  ret |= CheckOutput(table, 4, 5);

  auto column = table->GetColumnByName("My Field Name 2");
  if (!column)
  {
    std::cout << "ERROR: column <My Field Name 2> not found.\n";
  }
  auto stringCol = vtkStringArray::SafeDownCast(column);
  std::string data = stringCol->GetValue(2);
  if (data != "String:Delimiters")
  {
    std::cout << "ERROR: string delimiter failed. Has <" << data << "> \n";
  }

  // merging delimiters
  filepath = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/delimited2.txt");
  reader->SetFileName(filepath);
  reader->SetFieldDelimiterCharacters(",");
  reader->MergeConsecutiveDelimitersOn();
  reader->Update();
  delete filepath;

  ret |= CheckOutput(reader->GetOutput(), 9, 1);

  column = table->GetColumnByName("Sam");
  if (!column)
  {
    std::cout << "ERROR: column <Sam> not found.\n";
  }
  stringCol = vtkStringArray::SafeDownCast(column);
  data = stringCol->GetValue(0);
  if (data != "line")
  {
    std::cout << "ERROR: string delimiter failed. Has <" << data << "> \n";
  }


  return ret;
}

//------------------------------------------------------------------------------
bool TestNumericsDefaultToString()
{
  std::string inputString = "Int, Str, Double\n";
  inputString += "1, _2_, 3.1";
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetHaveHeaders(true);
  reader->SetStringDelimiter('_');
  reader->SetReadFromInputString(true);
  reader->SetInputString(inputString);
  reader->Update();

  vtkNew<vtkStringArray> Int;
  Int->SetName("Int");
  Int->InsertNextValue("1");
  vtkNew<vtkStringArray> Str;
  Str->SetName("Str");
  Str->InsertNextValue("2");
  vtkNew<vtkStringArray> Double;
  Double->SetName("Double");
  Double->InsertNextValue("3.1");
  vtkNew<vtkTable> expectedTable;
  expectedTable->SetNumberOfRows(1);
  expectedTable->AddColumn(Int);
  expectedTable->AddColumn(Str);
  expectedTable->AddColumn(Double);

  vtkTable* output = reader->GetOutput();

  return vtkTestUtilities::CompareDataObjects(output, expectedTable);
}

//------------------------------------------------------------------------------
bool TestNumericsDetectType()
{
  std::string inputString = "Int, Str, Double\n";
  inputString += "1, _2_, 3.1\n";
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetHaveHeaders(true);
  reader->SetStringDelimiter('_');
  reader->SetReadFromInputString(true);
  reader->SetInputString(inputString);
  reader->SetDetectNumericColumns(true);
  reader->Update();

  vtkNew<vtkIntArray> Int;
  Int->SetName("Int");
  Int->InsertNextValue(1);
  vtkNew<vtkStringArray> Str;
  Str->SetName("Str");
  Str->InsertNextValue("2");
  vtkNew<vtkDoubleArray> Double;
  Double->SetName("Double");
  Double->InsertNextValue(3.1);
  vtkNew<vtkTable> expectedTable;
  expectedTable->AddColumn(Int);
  expectedTable->AddColumn(Str);
  expectedTable->AddColumn(Double);

  vtkTable* output = reader->GetOutput();
  return vtkTestUtilities::CompareDataObjects(output, expectedTable);
}

//------------------------------------------------------------------------------
bool TestNumericsConvertType()
{
  std::string inputString = "Double1, Str, Double\n";
  inputString += "1, 2, 3.1\n";
  // second col becomes string
  inputString += "1, _2_, 3.1\n";
  // first col becomes double
  inputString += "1.1, 2.2, 3\n";
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetHaveHeaders(true);
  reader->SetStringDelimiter('_');
  reader->SetReadFromInputString(true);
  reader->SetInputString(inputString);
  reader->SetDetectNumericColumns(true);

  reader->Update();

  vtkNew<vtkDoubleArray> Double1;
  Double1->SetName("Double1");
  Double1->InsertNextValue(1);
  Double1->InsertNextValue(1);
  Double1->InsertNextValue(1.1);
  vtkNew<vtkStringArray> Str;
  Str->SetName("Str");
  Str->InsertNextValue("2");
  Str->InsertNextValue("2");
  Str->InsertNextValue("2.2");
  vtkNew<vtkDoubleArray> Double;
  Double->SetName("Double");
  Double->InsertNextValue(3.1);
  Double->InsertNextValue(3.1);
  Double->InsertNextValue(3.);
  vtkNew<vtkTable> expectedTable;
  expectedTable->AddColumn(Double1);
  expectedTable->AddColumn(Str);
  expectedTable->AddColumn(Double);

  vtkTable* output = reader->GetOutput();
  return vtkTestUtilities::CompareDataObjects(output, expectedTable);
}

//------------------------------------------------------------------------------
bool TestNumerics()
{
  if (!::TestNumericsDefaultToString())
  {
    std::cout << "Test default to string failed.\n";
    return false;
  }
  else if (!::TestNumericsDetectType())
  {
    std::cout << "Test column type detection failed.\n";
    return false;
  }
  else if (!::TestNumericsConvertType())
  {
    std::cout << "Test column type conversion failed.\n";
    return false;
  }

  return true;
}

//------------------------------------------------------------------------------
bool TestCharSets(int argc, char* argv[])
{
  char* filepath = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/delimitedUTF16LE.txt");
  vtkNew<vtkDelimitedTextReader> reader;
  reader->SetFileName(filepath);
  reader->SetHaveHeaders(true);
  reader->SetFieldDelimiterCharacters(":");
  reader->SetUnicodeCharacterSet("UTF-16LE");
  reader->Update();
  delete filepath;
  
  bool ret = CheckOutput(reader->GetOutput(), 4, 5);

  filepath = vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/delimitedUTF16BE.txt");
  reader->SetFileName(filepath);
  reader->SetUnicodeCharacterSet("UTF-16BE");
  reader->Update();
  delete filepath;

  ret |= CheckOutput(reader->GetOutput(), 4, 5);

  return ret;
}

};

//------------------------------------------------------------------------------
int TestDelimitedTextReader(int argc, char* argv[])
{
  if (!::TestDefault(argc, argv))
  {
    std::cout << "Test Default failed.\n";
  }
  else if (!::TestHeaders(argc, argv))
  {
    std::cout << "Test Headers failed\n";
  }
  else if (!::TestDelimiters(argc, argv))
  {
    std::cout << "Test Delimiters failed.\n";
  }
  else if (!::TestReadFromString())
  {
    std::cout << "Test Read From String failed.\n";
  }
  else if (!::TestCharSets(argc, argv))
  {
    std::cout << "Test CharSets failed.\n";
  }
  else if (!::TestNumerics())
  {
    std::cout << "Test Numerics failed.\n";
  }
  else
  {
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
