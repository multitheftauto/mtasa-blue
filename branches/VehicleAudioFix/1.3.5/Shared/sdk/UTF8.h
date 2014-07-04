/*
 * Smart Common Input Method
 *
 * Copyright (c) 2002-2005 James Su <suzhe@tsinghua.org.cn>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA  02111-1307  USA
 *
 * $Id: scim_utility.cpp,v 1.48.2.5 2006/11/02 04:11:51 suzhe Exp $
 */

/* Return code if invalid. (xxx_mbtowc, xxx_wctomb) */
#define RET_ILSEQ      0
/* Return code if only a shift sequence of n bytes was read. (xxx_mbtowc) */
#define RET_TOOFEW(n)  (-1-(n))
/* Return code if output buffer is too small. (xxx_wctomb, xxx_reset) */
#define RET_TOOSMALL   -1
/* Replacement character for invalid multibyte sequence or wide character. */
#define BAD_WCHAR ((wchar_t) 0xfffd)
#define BAD_CHAR '?'

int
utf8_mbtowc (wchar_t *pwc, const unsigned char *src, int src_len)
{
    if (!pwc)
        return 0;

    unsigned char c = src [0];

    if (c < 0x80) {
        *pwc = c;
        return 1;
    } else if (c < 0xc2) {
        return RET_ILSEQ;
    } else if (c < 0xe0) {
        if (src_len < 2)
            return RET_TOOFEW(0);
        if (!((src [1] ^ 0x80) < 0x40))
            return RET_ILSEQ;
        *pwc = ((wchar_t) (c & 0x1f) << 6)
                 | (wchar_t) (src [1] ^ 0x80);
        return 2;
    } else if (c < 0xf0) {
        if (src_len < 3)
            return RET_TOOFEW(0);
        if (!((src [1] ^ 0x80) < 0x40 && (src [2] ^ 0x80) < 0x40
                && (c >= 0xe1 || src [1] >= 0xa0)))
            return RET_ILSEQ;
        *pwc = ((wchar_t) (c & 0x0f) << 12)
                 | ((wchar_t) (src [1] ^ 0x80) << 6)
                 | (wchar_t) (src [2] ^ 0x80);
        return 3;
    } else if (c < 0xf8) {
        if (src_len < 4)
            return RET_TOOFEW(0);
        if (!((src [1] ^ 0x80) < 0x40 && (src [2] ^ 0x80) < 0x40
                && (src [3] ^ 0x80) < 0x40
                && (c >= 0xf1 || src [1] >= 0x90)))
            return RET_ILSEQ;
        *pwc = ((wchar_t) (c & 0x07) << 18)
                 | ((wchar_t) (src [1] ^ 0x80) << 12)
                 | ((wchar_t) (src [2] ^ 0x80) << 6)
                 | (wchar_t) (src [3] ^ 0x80);
        return 4;
    } else if (c < 0xfc) {
        if (src_len < 5)
            return RET_TOOFEW(0);
        if (!((src [1] ^ 0x80) < 0x40 && (src [2] ^ 0x80) < 0x40
                && (src [3] ^ 0x80) < 0x40 && (src [4] ^ 0x80) < 0x40
                && (c >= 0xf9 || src [1] >= 0x88)))
            return RET_ILSEQ;
        *pwc = ((wchar_t) (c & 0x03) << 24)
                 | ((wchar_t) (src [1] ^ 0x80) << 18)
                 | ((wchar_t) (src [2] ^ 0x80) << 12)
                 | ((wchar_t) (src [3] ^ 0x80) << 6)
                 | (wchar_t) (src [4] ^ 0x80);
        return 5;
    } else if (c < 0xfe) {
        if (src_len < 6)
            return RET_TOOFEW(0);
        if (!((src [1] ^ 0x80) < 0x40 && (src [2] ^ 0x80) < 0x40
                && (src [3] ^ 0x80) < 0x40 && (src [4] ^ 0x80) < 0x40
                && (src [5] ^ 0x80) < 0x40
                && (c >= 0xfd || src [1] >= 0x84)))
            return RET_ILSEQ;
        *pwc = ((wchar_t) (c & 0x01) << 30)
                 | ((wchar_t) (src [1] ^ 0x80) << 24)
                 | ((wchar_t) (src [2] ^ 0x80) << 18)
                 | ((wchar_t) (src [3] ^ 0x80) << 12)
                 | ((wchar_t) (src [4] ^ 0x80) << 6)
                 | (wchar_t) (src [5] ^ 0x80);
        return 6;
    } else
        return RET_ILSEQ;
}

int
utf8_wctomb (unsigned char *dest, wchar_t wc, int dest_size)
{
    if (!dest)
        return 0;

    int count;
    if (wc < 0x80)
        count = 1;
    else if (wc < 0x800)
        count = 2;
    else if (wc < 0x10000)
        count = 3;
    else if (wc < 0x200000)
        count = 4;
    else if (wc < 0x4000000)
        count = 5;
    else if (wc <= 0x7fffffff)
        count = 6;
    else
        return RET_ILSEQ;
    if (dest_size < count)
        return RET_TOOSMALL;
    switch (count) { /* note: code falls through cases! */
        case 6: dest [5] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x4000000;
        case 5: dest [4] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x200000;
        case 4: dest [3] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x10000;
        case 3: dest [2] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0x800;
        case 2: dest [1] = 0x80 | (wc & 0x3f); wc = wc >> 6; wc |= 0xc0;
        case 1: dest [0] = (unsigned char)wc;
    }
    return count;
}


//////////////////////////////////////////////////
//
// Original - For testing
//

std::wstring
utf8_mbstowcs_orig (const std::string & str)
{
    std::wstring wstr;
    wchar_t wc;
    unsigned int sn = 0;
    int un = 0;

    const unsigned char *s = (const unsigned char *) str.c_str ();

    while (sn < str.length () && *s != 0 &&
            (un=utf8_mbtowc (&wc, s, str.length () - sn)) > 0) {
        wstr.push_back (wc);
        s += un;
        sn += un;
    }
    return wstr;
}

std::string
utf8_wcstombs_orig (const std::wstring & wstr)
{
    std::string str;
    char utf8 [6];
    int un = 0;

    for (unsigned int i = 0; i<wstr.size (); ++i) {
        un = utf8_wctomb ((unsigned char*)utf8, wstr [i], 6);
        if (un > 0)
            str.append (utf8, un);
    }
    return str;
}


//////////////////////////////////////////////////
//
// Optimized - faster for strings smaller than SMALL_STRING_LIMIT
//
#define SMALL_STRING_LIMIT 1000

std::wstring
utf8_mbstowcs (const std::string & str)
{
    const unsigned char *s = (const unsigned char *) str.c_str ();
    const unsigned int length = str.length ();

    if ( length < SMALL_STRING_LIMIT )
    {
        // Faster but limited size
        uint cCharacters = length + 1;
        uint cBytes = ( cCharacters ) * sizeof( wchar_t );

        wchar_t* buffer = (wchar_t*)alloca ( cBytes );
        wchar_t* ptr = buffer;
        wchar_t wc;
        unsigned int sn = 0;
        int un = 0;

        while (sn < length && *s != 0 &&
                (un=utf8_mbtowc (&wc, s, length - sn)) > 0) {
            *ptr++ = wc;
            s += un;
            sn += un;
        }
        unsigned int usedsize = ptr - buffer;
        dassert ( usedsize < cCharacters );
        return std::wstring( buffer, usedsize );
    }
    else
    {
        // Slower but any size
        std::wstring wstr;
        wchar_t wc;
        unsigned int sn = 0;
        int un = 0;

        while (sn < length && *s != 0 &&
                (un=utf8_mbtowc (&wc, s, length - sn)) > 0) {
            wstr.push_back (wc);
            s += un;
            sn += un;
        }
        return wstr;
    }
}


// Optimized
std::string
utf8_wcstombs (const std::wstring & wstr)
{
    const unsigned int size = wstr.length ();

    if ( size < SMALL_STRING_LIMIT )
    {
        // Faster but limited size
        uint cBytes = ( size + 1 ) * 6;
        char* buffer = (char*)alloca ( cBytes );
        char* ptr = buffer;
        for (unsigned int i = 0; i<size ; ++i)
        {
            ptr += utf8_wctomb ((unsigned char*)ptr, wstr [i], 6 );
        }
        unsigned int usedsize = ptr - buffer;
        dassert ( usedsize < cBytes );
        return std::string( buffer, usedsize );
    }
    else
    {
        // Slower but any size
        char utf8 [6];
        std::string str;

        for (unsigned int i = 0; i<size ; ++i) {
            int un = utf8_wctomb ((unsigned char*)utf8, wstr [i], 6);
            if (un > 0)
                str.append (utf8, un);
        }
        return str;
    }
}
