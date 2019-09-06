/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "vtkASCIITextCodec.h"

#include "vtkObjectFactory.h"
#include "vtkTextCodecFactory.h"

#include <stdexcept>

vtkStandardNewMacro(vtkASCIITextCodec);

const char* vtkASCIITextCodec::Name() {return "US-ASCII" ;}

bool vtkASCIITextCodec::CanHandle(const char* NameStr)
{
  if (0 == strcmp(NameStr, "US-ASCII") || 0 == strcmp(NameStr, "ASCII"))
    return true ;
  else
    return false ;
}

bool vtkASCIITextCodec::IsValid(istream& InputStream)
{
  bool returnBool = true ;

  // get the position of the stream so we can restore it when we are done
  istream::pos_type StreamPos = InputStream.tellg() ;

  // check the code points for non-ascii characters
  while (!InputStream.eof())
  {
    vtkTypeUInt32 CodePoint = InputStream.get() ;

    if(!InputStream.eof() && CodePoint > 0x7f)
    {
      returnBool = false ;
      break ;
    }
  }

  // reset the stream
  InputStream.clear() ;
  InputStream.seekg(StreamPos) ;

  return returnBool ;
}


void vtkASCIITextCodec::ToUnicode(istream& InputStream,
                                  vtkTextCodec::OutputIterator& output)
{
  while (!InputStream.eof())
  {
    vtkTypeUInt32 CodePoint = InputStream.get();

    if(!InputStream.eof())
    {
      if (CodePoint > 0x7f)
        throw std::runtime_error("Detected a character that isn't valid US-ASCII.");

      *output++ = CodePoint;
    }
  }
}


vtkUnicodeString::value_type vtkASCIITextCodec::NextUnicode(istream& InputStream)
{
  vtkTypeUInt32 CodePoint = InputStream.get();

  if (!InputStream.eof())
  {
    if(CodePoint > 0x7f)
      throw std::runtime_error("Detected a character that isn't valid US-ASCII.");

    return CodePoint ;
  }
  else
    return 0 ;
}

vtkASCIITextCodec::vtkASCIITextCodec() : vtkTextCodec()
{
}


vtkASCIITextCodec::~vtkASCIITextCodec() = default;


void vtkASCIITextCodec::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "vtkASCIITextCodec (" << this << ") \n" ;
  indent = indent.GetNextIndent();
  this->Superclass::PrintSelf(os, indent.GetNextIndent()) ;
}
