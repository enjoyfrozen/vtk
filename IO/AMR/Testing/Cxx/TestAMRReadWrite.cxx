/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
// .NAME Test of vtkSimplePointsReader and vtkSimplePointsWriter
// .SECTION Description
//
#include "vtkSmartPointer.h"
#include "vtkSetGet.h"
#include "vtkOverlappingAMR.h"
#include "vtkUniformGrid.h"
#include "vtkNew.h"
#include "vtkAMREnzoReader.h"
#include "vtkTestUtilities.h"
#include "vtkCompositeDataWriter.h"

int TestAMRReadWrite( int argc, char *argv[] )
{
  char* fname = vtkTestUtilities::ExpandDataFileName(argc, argv,"Data/AMR/Enzo/DD0010/moving7_0010.hierarchy");

  vtkNew<vtkAMREnzoReader> reader;
  reader->SetFileName(fname);
  reader->SetMaxLevel(8);
  reader->SetCellArrayStatus( "TotalEnergy",1);
  reader->Update();

  vtkSmartPointer<vtkOverlappingAMR> amr;
  amr = vtkOverlappingAMR::SafeDownCast(reader->GetOutputDataObject(0));
  amr->Audit();


  vtkNew<vtkCompositeDataWriter> writer;
  writer->SetFileName();
  writer->Write();


  return EXIT_SUCCESS;
}
