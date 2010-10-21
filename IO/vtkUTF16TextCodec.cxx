/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkUTF16TextCodec.cxx

Copyright (c)
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2010 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/

#include <vtkUTF16TextCodec.h>

#include <vtkObjectFactory.h>
#include <vtkTextCodecFactory.h>

#include <stdexcept>


vtkStandardNewMacro(vtkUTF16TextCodec);

vtkTextCodec* vtkUTF16TextCodecFromCallback()
{
   return vtkUTF16TextCodec::New() ;
}

class vtkUTF16TextCodecRegister
{
  public:
    vtkUTF16TextCodecRegister()
    {
      vtkTextCodecFactory::RegisterCreateCallback(vtkUTF16TextCodecFromCallback) ;
    }
} ;


static vtkUTF16TextCodecRegister foo ;

namespace
{
  /////////////////////////////////////////////////////////////////////////////////////////
  // utf16_to_unicode

  vtkTypeUInt32 utf16_to_unicode_next(const bool big_endian, istream& InputStream)
  {
    vtkTypeUInt8 first_byte = InputStream.get() ;

    if(InputStream.eof())
      throw vtkstd::runtime_error("Premature end-of-sequence extracting UTF-16 code unit.");
    vtkTypeUInt8 second_byte = InputStream.get() ;

    vtkTypeUInt32 returnCode =
      big_endian ? first_byte << 8 | second_byte : second_byte << 8 | first_byte;

    if(returnCode >= 0xd800 && returnCode <= 0xdfff)
      {
      if(InputStream.eof())
        throw vtkstd::runtime_error("Premature end-of-sequence extracting UTF-16 trail surrogate first byte.");
      vtkTypeUInt8 third_byte = InputStream.get() ;

      if(InputStream.eof())
        throw vtkstd::runtime_error("Premature end-of-sequence extracting UTF-16 trail surrogate second byte.");
      vtkTypeUInt8 fourth_byte = InputStream.get() ;

      const vtkTypeUInt32 second_code_unit =
        big_endian ? third_byte << 8 | fourth_byte : fourth_byte << 8 | third_byte;
      if(second_code_unit >= 0xdc00 && second_code_unit <= 0xdfff)
        {
        returnCode = vtkTypeUInt32 (vtkTypeInt32 (returnCode << 10) +
                                    vtkTypeInt32 (second_code_unit) +
                                    (0x10000 - (0xd800 << 10) - 0xdc00));
        }
      else
        {
        throw vtkstd::runtime_error("Invalid UTF-16 trail surrogate.");
        }
      }
    return returnCode ;
  }


  void utf16_to_unicode(const bool big_endian, istream& InputStream,
                        vtkTextCodec::OutputIterator& output)
  {
    while(!InputStream.eof())
      {
      unsigned int nAvail = InputStream.rdbuf()->in_avail() ;
      istream::pos_type StreamPos = InputStream.tellg() ;
      if (1 < nAvail)
        {
        const vtkTypeUInt32 code_unit = utf16_to_unicode_next(big_endian, InputStream) ;
        *output++ = code_unit;
        }
      else
        break ;
      }
  }


  // iterator to use in testing validity - throws all input away.
  class testIterator : public vtkTextCodec::OutputIterator
  {
  public:
    virtual testIterator& operator++(int) {return *this ;}
    virtual testIterator& operator*() {return *this ;}
    virtual testIterator& operator=(const vtkUnicodeString::value_type value) {return *this ;}

    testIterator() {}
    ~testIterator() {}

  private:
    testIterator(const testIterator&) ; // Not implemented
    const testIterator& operator=(const testIterator&) ; // Not Implemented
  } ;


} // end anonymous namespace


vtkUTF16TextCodec::vtkUTF16TextCodec() : vtkTextCodec(), _endianExplicitlySet(false),
                                         _bigEndian(true)
{
}


vtkUTF16TextCodec::~vtkUTF16TextCodec()
{
}


const char* vtkUTF16TextCodec::Name()
{
  return "UTF-16" ;
}


const bool vtkUTF16TextCodec::CanHandle(const char* NameString)
{
  if (0 == strcmp(NameString, "UTF-16"))
    {
    _endianExplicitlySet = false ;
    return true ;
    }
  else if (0 == strcmp(NameString, "UTF-16BE"))
    {
    SetBigEndian(true) ;
    return true ;
    }
  else if (0 == strcmp(NameString, "UTF-16LE"))
    {
    SetBigEndian(false) ;
    return true ;
    }
  else
    return false ;
}


void vtkUTF16TextCodec::SetBigEndian(bool state)
{
  _endianExplicitlySet = true ;
   _bigEndian = state ;
}


void vtkUTF16TextCodec::FindEndianness(istream& InputStream)
{
  _endianExplicitlySet = false ;

  if(InputStream.rdbuf()->in_avail() > 1)
    {
    istream::char_type c1, c2 ;
    InputStream.get(c1) ;
    InputStream.get(c2) ;

    if(static_cast<unsigned char>(c1) == 0xfe &&
       static_cast<unsigned char>(c2) == 0xff)
      _bigEndian = true ;

    else if(static_cast<unsigned char>(c1) == 0xff &&
            static_cast<unsigned char>(c2) == 0xfe)
      _bigEndian = false ;

    else
      throw vtkstd::runtime_error("Cannot detect UTF-16 endianness.  Try 'UTF-16BE' or 'UTF-16LE' instead.");
    }
  else
    throw vtkstd::runtime_error("Cannot detect UTF-16 endianness.  Try 'UTF-16BE' or 'UTF-16LE' instead.");

}


bool vtkUTF16TextCodec::IsValid(istream& InputStream)
{
  bool returnBool = true ;
  // get the position of the stream so we can restore it when we are
  // done
  istream::pos_type StreamPos = InputStream.tellg() ;

  try
    {
    if (!_endianExplicitlySet)
      FindEndianness(InputStream) ;

    testIterator junk ;
    utf16_to_unicode(_bigEndian, InputStream, junk) ;
    }
  catch(...)
    {
    returnBool = false ;
    }

  // reset the stream
  InputStream.clear() ;
  InputStream.seekg(StreamPos) ;

  return returnBool ;
}


void vtkUTF16TextCodec::ToUnicode(istream& InputStream, vtkTextCodec::OutputIterator& output)
{
  if (!_endianExplicitlySet)
     FindEndianness(InputStream) ;

  utf16_to_unicode(_bigEndian, InputStream, output) ;
}


vtkUnicodeString::value_type vtkUTF16TextCodec::NextUnicode(istream& InputStream)
{
  return utf16_to_unicode_next(_bigEndian, InputStream) ;
}


void vtkUTF16TextCodec::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "vtkUTF16TextCodec (" << this << ") \n" ;
  indent = indent.GetNextIndent();
  this->Superclass::PrintSelf(os, indent.GetNextIndent()) ;
}
