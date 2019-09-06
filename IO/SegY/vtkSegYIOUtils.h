/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#ifndef vtkSegYIOUtils_h
#define vtkSegYIOUtils_h

#include <fstream>

class vtkSegYIOUtils
{
public:
  char readChar(std::ifstream& in);
  short readShortInteger(std::streamoff pos, std::ifstream& in);
  short readShortInteger(std::ifstream& in);
  int readLongInteger(std::streamoff pos, std::ifstream& in);
  int readLongInteger(std::ifstream& in);
  float readFloat(std::ifstream& in);
  float readIBMFloat(std::ifstream& in);
  unsigned char readUChar(std::ifstream& in);
  void swap(char* a, char* b);
  static vtkSegYIOUtils* Instance();
  std::streamoff getFileSize(std::ifstream& in);

  bool IsBigEndian;

private:
  vtkSegYIOUtils();
  bool checkIfBigEndian()
  {
    unsigned short a = 0x1234;
    if (*((unsigned char*)&a) == 0x12)
      return true;
    return false;
  }
};

#endif // vtkSegYIOUtils_h
// VTK-HeaderTest-Exclude: vtkSegYIOUtils.h
