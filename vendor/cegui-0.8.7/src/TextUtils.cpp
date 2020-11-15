/***********************************************************************
    created:    30/5/2004
    author:     Paul D Turner

    purpose:    Implementation of text support class
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
#include "CEGUI/TextUtils.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
    Constants
*************************************************************************/
const String    TextUtils::DefaultWhitespace(" \n\t\r");
const String    TextUtils::DefaultAlphanumerical("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
const String    TextUtils::DefaultWrapDelimiters(" \n\t\r");



/*************************************************************************
    return a String containing the the next word in a String.
*************************************************************************/
String TextUtils::getNextWord(const String& str, String::size_type start_idx, const String& delimiters)
{
    String::size_type   word_start = str.find_first_not_of(delimiters, start_idx);

    if (word_start == String::npos)
    {
        word_start = start_idx;
    }

    String::size_type   word_end = str.find_first_of(delimiters, word_start);

    if (word_end == String::npos)
    {
        word_end = str.length();
    }

    return str.substr(start_idx, (word_end - start_idx));
}


/*************************************************************************
    Return the index of the first character of the word at 'idx'.
*************************************************************************/
String::size_type TextUtils::getWordStartIdx(const String& str, String::size_type idx)
{
    String  temp = str.substr(0, idx);

    trimTrailingChars(temp, DefaultWhitespace);

    if (temp.length() <= 1)
    {
        return 0;
    }

    // identify the type of character at 'pos'
    if (String::npos != DefaultAlphanumerical.find(temp[temp.length() - 1]))
    {
        idx = temp.find_last_not_of(DefaultAlphanumerical);
    }
    // since whitespace was stripped, character must be a symbol
    else
    {
        idx = temp.find_last_of(DefaultAlphanumerical + DefaultWhitespace);
    }

    // make sure we do not go past end of string (+1)
    if (idx == String::npos)
    {
        return 0;
    }
    else
    {
        return idx + 1;
    }

}


/*************************************************************************
    Return the index of the first character of the word after the word
    at 'idx'.
*************************************************************************/
String::size_type TextUtils::getNextWordStartIdx(const String& str, String::size_type idx)
{
    String::size_type str_len = str.length();

    // do some checks for simple cases
    if ((idx >= str_len) || (str_len == 0))
    {
        return str_len;
    }

    // is character at 'idx' alphanumeric
    if (String::npos != DefaultAlphanumerical.find(str[idx]))
    {
        // find position of next character that is not alphanumeric
        idx = str.find_first_not_of(DefaultAlphanumerical, idx);
    }
    // is character also not whitespace (therefore a symbol)
    else if (String::npos == DefaultWhitespace.find(str[idx]))
    {
        // find index of next character that is either alphanumeric or whitespace
        idx = str.find_first_of(DefaultAlphanumerical + DefaultWhitespace, idx);
    }

    // check result at this stage.
    if (String::npos == idx)
    {
        idx = str_len;
    }
    else
    {
        // if character at 'idx' is whitespace
        if (String::npos != DefaultWhitespace.find(str[idx]))
        {
            // find next character that is not whitespace
            idx = str.find_first_not_of(DefaultWhitespace, idx);
        }

        if (String::npos == idx)
        {
            idx = str_len;
        }

    }

    return idx;
}


/*************************************************************************
    Trim all characters from the set specified in \a chars from the
    begining of 'str'.
*************************************************************************/
void TextUtils::trimLeadingChars(String& str, const String& chars)
{
    String::size_type idx = str.find_first_not_of(chars);

    if (idx != String::npos)
    {
        str.erase(0, idx);
    }
    else
    {
        str.erase();
    }

}


/*************************************************************************
    Trim all characters from the set specified in \a chars from the end
    of 'str'.
*************************************************************************/
void TextUtils::trimTrailingChars(String& str, const String& chars)
{
    String::size_type idx = str.find_last_not_of(chars);

    if (idx != String::npos)
    {
        str.resize(idx + 1);
    }
    else
    {
        str.erase();
    }

}

} // End of  CEGUI namespace section
