#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkLogger.h"
#include "vtkNew.h"
#include "vtkSparkline.h"

namespace
{

// Given a UTF-8 string, count the number of glyphs (code points) it contains.
std::size_t numCodePoints(const std::string& ss)
{
  std::size_t count = 0;
  for (std::size_t ii = 0; ii < ss.size(); ++ii)
  {
    count += (ss[ii] & 0xc0) != 0x80;
  }
  return count;
}

}

int TestSparkline(int, char*[])
{
  vtkNew<vtkDoubleArray> dblArr;
  vtkNew<vtkIntArray> intArr;
  std::array<double, 4> dblData{ 0., 2., 1.5, 1. };
  std::array<int, 22> intData{ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  dblArr->SetName("double_test");
  dblArr->SetArray(&dblData[0], 4, 1);
  intArr->SetName("integer_test");
  intArr->SetArray(&intData[0], 22, 1);
  std::string dblSparkNo = vtkSparkline::FromArray(dblArr, -1, 10, false);
  std::string dblSparkYes = vtkSparkline::FromArray(dblArr, -1, 10, true);
  std::string intSparkNo = vtkSparkline::FromArray(intArr, -1, 10, false);
  std::string intSparkYes = vtkSparkline::FromArray(intArr, -1, 10, true);

  std::cout << " 1 = " << dblSparkNo << " length " << numCodePoints(dblSparkNo) << "\n"
            << " 2 = " << dblSparkYes << " length " << numCodePoints(dblSparkYes) << "\n"
            << " 3 = " << intSparkNo << " length " << numCodePoints(intSparkNo) << "\n"
            << " 4 = " << intSparkYes << " length " << numCodePoints(intSparkYes) << "\n";

  if (numCodePoints(dblSparkNo) != 4 || numCodePoints(dblSparkYes) != 10 ||
    numCodePoints(intSparkNo) != 10 || numCodePoints(intSparkYes) != 10)
  {
    std::cerr << "ERROR: Wrong number of code points (expected 4, 10, 10, 10)\n";
    return EXIT_FAILURE;
  }

  if (dblSparkNo != "▁█▆▄" || dblSparkYes != "▁▁▁███▆▆▆▄" || intSparkNo != "█▆▄▃▁▁▃▄▆█" ||
    intSparkYes != "█▆▄▃▁▁▃▄▆█")
  {
    std::cerr << "ERROR: Bar charts are unexpected.\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
