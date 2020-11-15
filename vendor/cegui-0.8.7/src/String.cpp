/***********************************************************************
	created:	26/2/2004
	author:		Paul D Turner
	
	purpose:	Implements string class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "CEGUI/String.h"

#if CEGUI_STRING_CLASS == CEGUI_STRING_CLASS_UNICODE

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
	if (d_reserve > CEGUI_STR_QUICKBUFF_SIZE)
	{
		delete[] d_buffer;
	}
		if (d_encodedbufflen > 0)
	{
		delete[] d_encodedbuff;
	}
}

bool String::grow(size_type new_size)
{
    // check for too big
    if (max_size() <= new_size)
        CEGUI_THROW(
            std::length_error("Resulting CEGUI::String would be too big"));

    // increase, as we always null-terminate the buffer.
    ++new_size;

    if (new_size > d_reserve)
    {
        utf32* temp = CEGUI_NEW_ARRAY_PT(utf32, new_size, String);

        if (d_reserve > CEGUI_STR_QUICKBUFF_SIZE)
        {
            memcpy(temp, d_buffer, (d_cplength + 1) * sizeof(utf32));
            CEGUI_DELETE_ARRAY_PT(d_buffer, utf32, d_reserve, String);
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
    if ((d_reserve > CEGUI_STR_QUICKBUFF_SIZE) && (d_reserve > min_size))
    {
            // see if we can trim to quick-buffer
        if (min_size <= CEGUI_STR_QUICKBUFF_SIZE)
        {
            memcpy(d_quickbuff, d_buffer, min_size * sizeof(utf32));
            CEGUI_DELETE_ARRAY_PT(d_buffer, utf32, d_reserve, String);
            d_reserve = CEGUI_STR_QUICKBUFF_SIZE;
        }
        // re-allocate buffer
        else
        {
            utf32* temp = CEGUI_NEW_ARRAY_PT(utf32, min_size, String);
            memcpy(temp, d_buffer, min_size * sizeof(utf32));
            CEGUI_DELETE_ARRAY_PT(d_buffer, utf32, d_reserve, String);
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
            CEGUI_DELETE_ARRAY_PT(d_encodedbuff, utf8, d_encodedbufflen, String);
        }

        d_encodedbuff = CEGUI_NEW_ARRAY_PT(utf8, buffsize, String);
        d_encodedbufflen = buffsize;
    }

    encode(ptr(), d_encodedbuff, buffsize, d_cplength);

    // always add a null at end
    d_encodedbuff[buffsize-1] = ((utf8)0);
    d_encodeddatlen = buffsize;

    return d_encodedbuff;
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

} // End of  CEGUI namespace section

#endif
