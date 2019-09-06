/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#define VTK_STREAMS_FWD_ONLY // like wrapper-generated sources
#include "vtkSystemIncludes.h"

#include <cstdio> // test covers NOT including <iostream>
#include <string>

int TestOStreamWrapper(int, char *[])
{
  int failed = 0;
  std::string const expect = "hello, world: 1";
  std::string actual;
  std::string s = "hello, world";
  vtkOStrStreamWrapper vtkmsg;
  vtkmsg << s << ": " << 1;
  actual = vtkmsg.str();
  vtkmsg.rdbuf()->freeze(0);
  if(actual != expect)
  {
    failed = 1;
    fprintf(stderr, "Expected '%s' but got '%s'\n",
            expect.c_str(), actual.c_str());
  }
  return failed;
}
