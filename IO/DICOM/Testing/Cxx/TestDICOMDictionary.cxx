#include "vtkDICOMDictEntry.h"
#include "vtkDICOMDictionary.h"

#include <sstream>

#include <stdlib.h>
#include <string.h>

// macro for performing tests
#define TestAssert(t)                                                                              \
  if (!(t))                                                                                        \
  {                                                                                                \
    cout << exename << ": Assertion Failed: " << #t << "\n";                                       \
    cout << __FILE__ << ":" << __LINE__ << "\n";                                                   \
    cout.flush();                                                                                  \
    rval |= 1;                                                                                     \
  }

int TestDICOMDictionary(int argc, char* argv[])
{
  int rval = 0;
  const char* exename = (argc > 0 ? argv[0] : "TestDICOMDictionary");

  // remove path portion of exename
  const char* cp = exename + strlen(exename);
  while (cp != exename && cp[-1] != '\\' && cp[-1] != '/')
  {
    --cp;
  }
  exename = cp;

  // test a valid entry
  vtkDICOMDictEntry e = vtkDICOMDictionary::FindDictEntry(DC::Modality);
  std::string name = "Modality";
  TestAssert(e.IsValid());
  TestAssert(e.GetTag() == DC::Modality);
  TestAssert(e.GetVR() == vtkDICOMVR::CS);
  TestAssert(e.GetVM() == vtkDICOMVM::M1);
  TestAssert(name == e.GetName());
  TestAssert(e.IsRetired() == 0);

  // test lookup via string
  e = vtkDICOMDictionary::FindDictEntry(name.c_str());
  TestAssert(e.IsValid());
  TestAssert(e.GetTag() == DC::Modality);
  TestAssert(e.GetVR() == vtkDICOMVR::CS);
  TestAssert(e.GetVM() == vtkDICOMVM::M1);
  TestAssert(name == e.GetName());
  TestAssert(e.IsRetired() == 0);

  // test an invalid entry
  e = vtkDICOMDictionary::FindDictEntry(vtkDICOMTag(0x0002, 0xFFFF));
  TestAssert(!e.IsValid());
  e = vtkDICOMDictionary::FindDictEntry("Wombat");
  TestAssert(!e.IsValid());
  e = vtkDICOMDictionary::FindDictEntry("");
  TestAssert(!e.IsValid());
  e = vtkDICOMDictEntry();
  TestAssert(!e.IsValid());

  // test private dictionary
  vtkDICOMTag ptag(0x0009, 0x0011);
  std::string pkey = "InternalDetectorFrameTime";
  e = vtkDICOMDictionary::FindDictEntry(ptag, "astm.org/diconde/iod/NdeDxDetector");
  TestAssert(e.IsValid());
  TestAssert(e.GetTag() == ptag);
  TestAssert(e.GetVR() == vtkDICOMVR::DS);
  TestAssert(e.GetVM() == vtkDICOMVM::M1);
  TestAssert(pkey == e.GetName());
  e = vtkDICOMDictionary::FindDictEntry(pkey.c_str(), "astm.org/diconde/iod/NdeDxDetector");
  TestAssert(e.IsValid());
  TestAssert(e.GetTag() == ptag);
  TestAssert(e.GetVR() == vtkDICOMVR::DS);
  TestAssert(e.GetVM() == vtkDICOMVM::M1);
  TestAssert(pkey == e.GetName());
  e = vtkDICOMDictionary::FindDictEntry("", "astm.org/diconde/iod/NdeDxDetector");
  TestAssert(!e.IsValid());

  return rval;
}
