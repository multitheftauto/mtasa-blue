/************************************************************************
	filename: 	CEGUIString.cpp
	created:	26/2/2004
	author:		Paul D Turner
	
	purpose:	Implements string class
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "StdInc.h"
#include "CEGUIString.h"
#include "CEGUIStringBidi.h"

#include <iostream>

// Start of CEGUI namespace section
namespace CEGUI
{

// definition of 'no position' value
const String::size_type String::npos = (String::size_type)(-1);


//////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////
String::~String(void)
{
	if (d_reserve > STR_QUICKBUFF_SIZE)
	{
		delete[] d_buffer;
	}
		if (d_encodedbufflen > 0)
	{
		delete[] d_encodedbuff;
        d_encodedbuff = nullptr;
	}
}

bool String::grow(size_type new_size)
{
    // check for too big
    if (max_size() <= new_size)
        std::length_error("Resulting CEGUI::String would be too big");

    // increase, as we always null-terminate the buffer.
    ++new_size;

    if (new_size > d_reserve)
    {
        utf32* temp = new utf32[new_size];

        if (d_reserve > STR_QUICKBUFF_SIZE)
        {
            memcpy(temp, d_buffer, (d_cplength + 1) * sizeof(utf32));
            delete[] d_buffer;
        }
        else
        {
            memcpy(temp, d_quickbuff, (d_cplength + 1) * sizeof(utf32));
        }

        d_buffer = temp;
        d_reserve = new_size;

        return true;
    }

    return false;
}

// perform re-allocation to remove wasted space.
void String::trim(void)
{
    size_type min_size = d_cplength + 1;

    // only re-allocate when not using quick-buffer, and when size can be trimmed
    if ((d_reserve > STR_QUICKBUFF_SIZE) && (d_reserve > min_size))
    {
            // see if we can trim to quick-buffer
        if (min_size <= STR_QUICKBUFF_SIZE)
        {
            memcpy(d_quickbuff, d_buffer, min_size * sizeof(utf32));
            delete[] d_buffer;
            d_reserve = STR_QUICKBUFF_SIZE;
        }
        // re-allocate buffer
        else
        {
            utf32* temp = new utf32[min_size];
            memcpy(temp, d_buffer, min_size * sizeof(utf32));
            delete[] d_buffer;
            d_buffer = temp;
            d_reserve = min_size;
        }

    }

}

// build an internal buffer with the string encoded as utf8 (remains valid until string is modified).
utf8* String::build_utf8_buff(void) const
{
    size_type buffsize = encoded_size(ptr(), d_cplength) + 1;

    if (buffsize > d_encodedbufflen) {

        if (d_encodedbufflen > 0)
        {
            delete[] d_encodedbuff;
            d_encodedbuff = nullptr;
        }

        d_encodedbuff = new utf8[buffsize];
        d_encodedbufflen = buffsize;
    }

    encode(ptr(), d_encodedbuff, buffsize, d_cplength);

    // always add a null at end
    d_encodedbuff[buffsize-1] = ((utf8)0);
    d_encodeddatlen = buffsize;

    return d_encodedbuff;
}

//Get the bidirectional version of the string
String String::bidify(void) const
{
#ifdef DETECT_ARRAY_ISSUES
    doBidiTest ();
#endif
    if ( !System::ms_bBidiEnabled )
        return *this;

    String tmp = *this;

    // Apply in sections separated by \n
    size_type pos = 0;
    while ( true )
    {
        size_type newpos = tmp.find ( '\n', pos );
        if ( newpos == String::npos )
        {
            doBidi(tmp.ptr() + pos, tmp.length() - pos, true, true);
            break;
        }
        else
        if ( newpos - pos > 0 )
        {
            doBidi(tmp.ptr() + pos, newpos - pos, true, true);
            tmp[newpos] = '\n';     // Restore \n (doBidi replaces it with a zero)
        }
        pos = newpos + 1;
    }

    return tmp;
}


//////////////////////////////////////////////////////////////////////////
// Comparison operators
//////////////////////////////////////////////////////////////////////////
bool	operator==(const String& str1, const String& str2)
{
	return (str1.compare(str2) == 0);
}

bool	operator==(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) == 0);
}

bool	operator==(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) == 0);
}

bool	operator==(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) == 0);
}

bool	operator==(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) == 0);
}


bool	operator!=(const String& str1, const String& str2)
{
	return (str1.compare(str2) != 0);
}

bool	operator!=(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) != 0);
}

bool	operator!=(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) != 0);
}

bool	operator!=(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) != 0);
}

bool	operator!=(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) != 0);
}


bool	operator<(const String& str1, const String& str2)
{
	return (str1.compare(str2) < 0);
}

bool	operator<(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) < 0);
}

bool	operator<(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator<(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) < 0);
}

bool	operator<(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) >= 0);
}


bool	operator>(const String& str1, const String& str2)
{
	return (str1.compare(str2) > 0);
}

bool	operator>(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) > 0);
}

bool	operator>(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator>(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) > 0);
}

bool	operator>(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) <= 0);
}


bool	operator<=(const String& str1, const String& str2)
{
	return (str1.compare(str2) <= 0);
}

bool	operator<=(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator<=(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator<=(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) <= 0);
}

bool	operator<=(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) >= 0);
}


bool	operator>=(const String& str1, const String& str2)
{
	return (str1.compare(str2) >= 0);
}

bool	operator>=(const String& str, const std::string& std_str)
{
	return (str.compare(std_str) >= 0);
}

bool	operator>=(const std::string& std_str, const String& str)
{
	return (str.compare(std_str) <= 0);
}

bool	operator>=(const String& str, const utf8* utf8_str)
{
	return (str.compare(utf8_str) >= 0);
}

bool	operator>=(const utf8* utf8_str, const String& str)
{
	return (str.compare(utf8_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// c-string operators
//////////////////////////////////////////////////////////////////////////
bool operator==(const String& str, const char* c_str)
{
	return (str.compare(c_str) == 0);
}

bool operator==(const char* c_str, const String& str)
{
	return (str.compare(c_str) == 0);
}

bool operator!=(const String& str, const char* c_str)
{
	return (str.compare(c_str) != 0);
}

bool operator!=(const char* c_str, const String& str)
{
	return (str.compare(c_str) != 0);
}

bool operator<(const String& str, const char* c_str)
{
	return (str.compare(c_str) < 0);
}

bool operator<(const char* c_str, const String& str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>(const String& str, const char* c_str)
{
	return (str.compare(c_str) > 0);
}

bool operator>(const char* c_str, const String& str)
{
	return (str.compare(c_str) <= 0);
}

bool operator<=(const String& str, const char* c_str)
{
	return (str.compare(c_str) <= 0);
}

bool operator<=(const char* c_str, const String& str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>=(const String& str, const char* c_str)
{
	return (str.compare(c_str) >= 0);
}

bool operator>=(const char* c_str, const String& str)
{
	return (str.compare(c_str) <= 0);
}

//////////////////////////////////////////////////////////////////////////
// Concatenation operator functions
//////////////////////////////////////////////////////////////////////////
String	operator+(const String& str1, const String& str2)
{
	String temp(str1);
	temp.append(str2);
	return temp;
}

String	operator+(const String& str, const std::string& std_str)
{
	String temp(str);
	temp.append(std_str);
	return temp;
}

String	operator+(const std::string& std_str, const String& str)
{
	String temp(std_str);
	temp.append(str);
	return temp;
}

String	operator+(const String& str, const utf8* utf8_str)
{
	String temp(str);
	temp.append(utf8_str);
	return temp;
}

String	operator+(const utf8* utf8_str, const String& str)
{
	String temp(utf8_str);
	temp.append(str);
	return temp;
}

String	operator+(const String& str, utf32 code_point)
{
	String temp(str);
	temp.append(1, code_point);
	return temp;
}

String	operator+(utf32 code_point, const String& str)
{
	String temp(1, code_point);
	temp.append(str);
	return temp;
}

String operator+(const String& str, const char* c_str)
{
	String tmp(str);
	tmp.append(c_str);
	return tmp;
}

String operator+(const char* c_str, const String& str)
{
	String tmp(c_str);
	tmp.append(str);
	return tmp;
}

//////////////////////////////////////////////////////////////////////////
// Output (stream) functions
//////////////////////////////////////////////////////////////////////////
std::ostream& operator<<(std::ostream& s, const String& str)
{
	return s << str.c_str();
}

//////////////////////////////////////////////////////////////////////////
// Modifying operations
//////////////////////////////////////////////////////////////////////////
// swap the contents of str1 and str2
void	swap(String& str1, String& str2)
{
	str1.swap(str2);
}


//////////////////////////////////////////////////////////////////////////
// utf32 to wstring
//////////////////////////////////////////////////////////////////////////
typedef wchar_t utf16;
// encoding functions
// for all:
// src_len is in code units, or 0 for null terminated string.
// dest_len is in code units.
// returns number of code units put into dest buffer.
size_t utf32_To_utf16( const utf32* src, utf16* dest, size_t dest_len, size_t src_len )
{
    size_t destCapacity = dest_len;

    // while there is data in the source buffer,
    for (uint idx = 0; idx < src_len; ++idx)
    {
        utf32	cp = src[idx];

        // check there is enough destination buffer to receive this encoded unit (exit loop & return if not)
        if (destCapacity < 2)
        {
            break;
        }

        if (cp < 0x10000)
        {
            *dest++ = (utf16)cp;
            --destCapacity;
        }
        else
        {
            utf32 vm = cp - 0x10000;
            utf32 vh = vm >> 10;
            utf32 vl = vm & 0x3FF;
            utf32 w1 = 0xD800 + vh;
            utf32 w2 = 0xDC00 + vl;
            *dest++ = (utf16)(w1);
            *dest++ = (utf16)(w2);
            destCapacity -= 2;
       }
    }

    return dest_len - destCapacity;
}

std::wstring String::c_wstring ( void ) const
{
    const utf32* src = this->ptr();
    size_t src_len = this->length();

    size_t dest_len = ( src_len + 10 ) * 2;
    utf16* dest = (utf16*)_alloca( dest_len + 10 );
    size_t numChars = utf32_To_utf16( src, dest, dest_len, src_len );
    return std::wstring ( dest, numChars );
}

} // End of  CEGUI namespace section
