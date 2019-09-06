/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#include "vtkRenderTimingTests.h"

/*=========================================================================
The main entry point
=========================================================================*/
int main( int argc, char *argv[] )
{
  // create the timing framework
  vtkRenderTimings a;

  // add the tests
  a.TestsToRun.push_back(new surfaceTest("Surface", false, false));
  a.TestsToRun.push_back(new surfaceTest("SurfaceColored", true, false));
  a.TestsToRun.push_back(new surfaceTest("SurfaceWithNormals", false, true));
  a.TestsToRun.push_back(
    new surfaceTest("SurfaceColoredWithNormals", true, true));

  a.TestsToRun.push_back(new glyphTest("Glyphing"));

  a.TestsToRun.push_back(new moleculeTest("Molecule"));
  a.TestsToRun.push_back(new moleculeTest("MoleculeAtomsOnly",true));

  a.TestsToRun.push_back(new volumeTest("Volume", false));
  a.TestsToRun.push_back(new volumeTest("VolumeWithShading", true));

  a.TestsToRun.push_back(new depthPeelingTest("DepthPeeling", false));
  a.TestsToRun.push_back(new depthPeelingTest("DepthPeelingWithNormals", true));

  a.TestsToRun.push_back(new manyActorTest("ManyActors"));

  // process them
  return a.ParseCommandLineArguments(argc, argv);
}
