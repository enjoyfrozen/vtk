/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkTextCodec.h"

const char* vtkTextCodec::Name()
{
  return "";
}


bool vtkTextCodec::CanHandle(const char*)
{
  return false;
}


bool vtkTextCodec::IsValid(istream&)
{
  return false;
}


vtkTextCodec::~vtkTextCodec() = default;


vtkTextCodec::vtkTextCodec() = default;


namespace
{
  class vtkUnicodeStringOutputIterator : public vtkTextCodec::OutputIterator
  {
  public:
    vtkUnicodeStringOutputIterator& operator++(int) override;
    vtkUnicodeStringOutputIterator& operator*() override;
    vtkUnicodeStringOutputIterator& operator=(const vtkUnicodeString::value_type value) override;

    vtkUnicodeStringOutputIterator(vtkUnicodeString& outputString);
    ~vtkUnicodeStringOutputIterator() override;

  private:
    vtkUnicodeStringOutputIterator(const vtkUnicodeStringOutputIterator&) = delete;
    vtkUnicodeStringOutputIterator& operator=(const vtkUnicodeStringOutputIterator&) = delete;

    vtkUnicodeString& OutputString;
    unsigned int StringPosition;
  };

  vtkUnicodeStringOutputIterator& vtkUnicodeStringOutputIterator::operator++(int)
  {
    this->StringPosition++;
    return *this;
  }

  vtkUnicodeStringOutputIterator& vtkUnicodeStringOutputIterator::operator*()
  {
    return *this;
  }

  vtkUnicodeStringOutputIterator& vtkUnicodeStringOutputIterator::operator=(const vtkUnicodeString::value_type value)
  {
    this->OutputString += value;
    return *this;
  }

  vtkUnicodeStringOutputIterator::vtkUnicodeStringOutputIterator(vtkUnicodeString& outputString) :
    OutputString(outputString), StringPosition(0)
  {
  }

  vtkUnicodeStringOutputIterator::~vtkUnicodeStringOutputIterator() = default;
}


vtkUnicodeString vtkTextCodec::ToUnicode(istream& InputStream)
{
  // create an output string stream
  vtkUnicodeString returnString;

  vtkUnicodeStringOutputIterator StringIterator(returnString);
  this->ToUnicode(InputStream, StringIterator);

  return returnString;
}


void vtkTextCodec::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "vtkTextCodec (" << this << ") \n";
  indent = indent.GetNextIndent();
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
}
