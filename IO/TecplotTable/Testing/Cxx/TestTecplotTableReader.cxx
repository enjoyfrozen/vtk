/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include <vtkTecplotTableReader.h>
#include <vtkTable.h>
#include <vtkTestUtilities.h>

// This tests the ability to read a Tecplot table. The test file contains residuals from a CFD calculation.
int TestTecplotTableReader(int argc, char *argv[])
{
  char* filename = vtkTestUtilities::ExpandDataFileName(
    argc, argv, "Data/residuals.dat");
  vtkTecplotTableReader *reader = vtkTecplotTableReader::New();
  reader->SetFileName(filename);
  delete [] filename;
  reader->OutputPedigreeIdsOn();
  reader->Update();

  vtkTable* table = reader->GetOutput();
  table->Dump();
  cout << "Printing reader info..." << endl;
  reader->Print(cout);

  if (table->GetNumberOfRows() != 171)
  {
    cout << "ERROR: Wrong number of rows: " << table->GetNumberOfRows()<<endl;
    return 1;
  }
  if (table->GetNumberOfColumns() != 11 + 1) // one extra for pedigree ids
  {
    cout << "ERROR: Wrong number of columns: " << table->GetNumberOfColumns()<<endl;
    return 1;
  }

  reader->Delete();

  return 0;
}
