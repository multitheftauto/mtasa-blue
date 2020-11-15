/***********************************************************************
    created:    Thu May 31 2012
    author:     Paul D Turner <paul@cegui.org.uk>
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2012 Paul D Turner & The CEGUI Development Team
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
#include "CEGUI/IconvStringTranscoder.h"
#include "CEGUI/Exceptions.h"
#include <vector>
#include <iconv.h>
#include <errno.h>
#include <locale>

namespace CEGUI
{
//----------------------------------------------------------------------------//
class IconvHelper
{
    std::string d_fromCode;
    std::string d_toCode;
    iconv_t d_cd;

public:
    //------------------------------------------------------------------------//
    IconvHelper(const std::string& tocode, const std::string& fromcode) :
        d_fromCode(fromcode),
        d_toCode(tocode),
        d_cd(iconv_open(d_toCode.c_str(), d_fromCode.c_str()))
    {
        if (d_cd == reinterpret_cast<iconv_t>(-1))
            CEGUI_THROW(InvalidRequestException(String(
                "Failed to create conversion descriptor from \"") +
                d_fromCode.c_str() + "\" to \"" + d_toCode.c_str() + "\"."));
    }

    //------------------------------------------------------------------------//
    ~IconvHelper()
    {
        iconv_close(d_cd);
    }

    //------------------------------------------------------------------------//
    size_t iconv(const char** inbuf, size_t* inbytesleft,
                 char** outbuf, size_t* outbytesleft)
    {
#ifdef CEGUI_ICONV_USES_CONST_INBUF
        return ::iconv(d_cd, inbuf, inbytesleft, outbuf, outbytesleft);
#else
        return ::iconv(d_cd, const_cast<char**>(inbuf), inbytesleft,
                       outbuf, outbytesleft);
#endif
    }

    //------------------------------------------------------------------------//
    void throwErrorException(int err)
    {
        std::string reason;

        if (err == EINVAL)
            reason = "Incomplete " + d_fromCode + " sequence.";
        else if (err == EILSEQ)
            reason = "Invalid " + d_fromCode + " sequence.";
        else
            reason = "Unknown error.";

        CEGUI_THROW(InvalidRequestException(String(
            "Failed to convert from \"") + d_fromCode.c_str() +
            "\" to \"" + d_toCode.c_str() + "\": " + reason.c_str()));
    }

    //------------------------------------------------------------------------//
};

//----------------------------------------------------------------------------//
// Helper to use iconv to perform some transcode operation.
template<typename T>
static T* iconvTranscode(IconvHelper& ich, const char* in_buf, size_t in_len)
{
    // Handle empty strings
    if (in_len == 0)
    {
        T* ret_buff = CEGUI_NEW_ARRAY_PT(T, 1, CEGUI::BufferAllocator);
        ret_buff[0] = 0;
        return ret_buff;
    }

    std::vector<T CEGUI_VECTOR_ALLOC(T)> out_vec;
    out_vec.resize(in_len);
    size_t out_count = 0;

    while (true)
    {
        char* out_buf =
            reinterpret_cast<char*>(&out_vec[out_count]);
        const size_t start_out_bytes_left =
            (out_vec.size() - out_count) * sizeof(T);
        size_t out_bytes_left = start_out_bytes_left;

        const size_t result = ich.iconv(&in_buf, &in_len,
                                        &out_buf, &out_bytes_left);

        out_count +=
            (start_out_bytes_left - out_bytes_left) / sizeof(T);

        if (result != static_cast<size_t>(-1))
        {
            T* ret_buff = CEGUI_NEW_ARRAY_PT(T, out_count + 1,
                                             CEGUI::BufferAllocator);
            memcpy(ret_buff, &out_vec[0], out_count * sizeof(T));
            ret_buff[out_count] = 0;
            return ret_buff;
        }

        if (errno != E2BIG)
            break;

        out_vec.resize(out_vec.size() + 8); // this is some arbitrary number
    }

    ich.throwErrorException(errno);

    // this is there mostly to silence compiler warnings, this code should
    // never be executed
    return 0;
}

//----------------------------------------------------------------------------//
// Helper tp return length of zero terminated string of Ts
template<typename T>
static size_t getStringLength(const T* buffer)
{
    const T* b = buffer;
    while (*b++);

    return b - buffer - 1;
}

//----------------------------------------------------------------------------//
// Helper to correctly delete a buffer returned from iconvTranscode
template<typename T>
static void deleteTranscodeBuffer(T* buffer)
{
    CEGUI_DELETE_ARRAY_PT(
        buffer, T, getStringLength(buffer) + 1, CEGUI::BufferAllocator);
}

//----------------------------------------------------------------------------//
// Helper to transcode a buffer and return a string class built from it.
template<typename String_T, typename CodeUnit_T>
static String_T iconvTranscode(IconvHelper& ich,
                               const char* in_buf, size_t in_len)
{
    CodeUnit_T* tmp = iconvTranscode<CodeUnit_T>(ich, in_buf, in_len);
    String_T result(tmp);
    deleteTranscodeBuffer(tmp);

    return result;
}

//----------------------------------------------------------------------------//
// Helper to detect the platform endianess at run time.
bool is_big_endian(void)
{
    union
    {
        uint32 i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1;
}

//----------------------------------------------------------------------------//
IconvStringTranscoder::IconvStringTranscoder()
{
    UTF16PE = is_big_endian() ? "UTF-16BE" : "UTF-16LE";
}

//----------------------------------------------------------------------------//
uint16* IconvStringTranscoder::stringToUTF16(const String& input) const
{
    IconvHelper ich(UTF16PE, "UTF-8");
    return iconvTranscode<uint16>(
        ich, input.c_str(), getStringLength(input.c_str()));
}

//----------------------------------------------------------------------------//
std::wstring IconvStringTranscoder::stringToStdWString(const String& input) const
{
    IconvHelper ich("WCHAR_T", "UTF-8");
    return iconvTranscode<std::wstring, wchar_t>(
        ich, input.c_str(), getStringLength(input.c_str()));
}

//----------------------------------------------------------------------------//
String IconvStringTranscoder::stringFromUTF16(const uint16* input) const
{
#if CEGUI_STRING_CLASS == CEGUI_STRING_CLASS_UNICODE
    IconvHelper ich("UTF-8", UTF16PE);
    return iconvTranscode<String, utf8>(
        ich, reinterpret_cast<const char*>(input),
        getStringLength(input) * sizeof(uint16));
#else
    IconvHelper ich("WCHAR_T", UTF16PE);
    return stringFromStdWString(iconvTranscode<std::wstring, wchar_t>(
        ich, reinterpret_cast<const char*>(input), getStringLength(input)));
#endif
}

//----------------------------------------------------------------------------//
String IconvStringTranscoder::stringFromStdWString(const std::wstring& input) const
{
#if CEGUI_STRING_CLASS == CEGUI_STRING_CLASS_UNICODE
    IconvHelper ich("UTF-8", "WCHAR_T");
    return iconvTranscode<String, utf8>(
        ich, reinterpret_cast<const char*>(input.c_str()),
        input.length() * sizeof(wchar_t));
#else
    /*
     * WTF is this shit - a short story by CrazyEddie
     *
     * One of the following implementations are for use when CEGUI::String is
     * not the unicode capable versions.  Neither of these implementations
     * perform ideally, which is why I left both here - as it is intended
     * that eventually we may come back here and improve this.
     *
     * The codecvt version (1st one) overall does a better job, because when
     * the locale is mbcs compliant, it is able to produce encoded output
     * that will likely match what client code is using elsewhere.  The
     * problem with it is that when using a non-mbcs locale (such as the
     * basic C or maybe an ISO8859-1 locale, a source character that will
     * not fit causes the conversion to fail (and we throw an exception).
     *
     * The ctype version (2nd one) provides us the facility to use a 'default'
     * character for those codes that can not be represented by a single
     * character in the target locale - and the conversion will always
     * succeed, though some data may be lost in translation.  The 'bad'
     * part about using ctype for conversion is that it is a single code
     * unit mapping, which means when the user has a mbcs locale, characters
     * that should be correcly represented (i.e they would be converted by
     * the codecvt version) are instead replaced by the default character.
     *
     * The ideal scenario for us would be able to use the codecvt approach but
     * also have a default character for use when conversion is impossible. This
     * would be the closest match to what is done on the Win32 implementation
     * that uses WideCharToMultiByte passing the CP_ACP value.
     */
    std::locale conv_locale("");
    std::vector<String::value_type> buf(input.length() * 6 + 1);

#if 0
    typedef std::codecvt<wchar_t, char, mbstate_t> Converter;
    const Converter& facet =  std::use_facet<Converter>(conv_locale);

    mbstate_t conv_state;
    const wchar_t* from_next;
    String::value_type* to_next;

    const Converter::result result = 
        facet.out(conv_state, &input[0],  &input[input.length()],  from_next,
                              &buf[0], &buf[buf.size()], to_next);

    if (result == Converter::error || result == Converter::partial)
        CEGUI_THROW(InvalidRequestException("conversion failed."));
#else
    const std::ctype<wchar_t>& facet = 
        std::use_facet<std::ctype<wchar_t> >(conv_locale);

    facet.narrow(&input[0], &input[input.length()], '?', &buf[0]);
#endif
    return String(&buf[0]);

#endif
}

//----------------------------------------------------------------------------//
void IconvStringTranscoder::deleteUTF16Buffer(uint16* input) const
{
    deleteTranscodeBuffer(input);
}

//----------------------------------------------------------------------------//

}

