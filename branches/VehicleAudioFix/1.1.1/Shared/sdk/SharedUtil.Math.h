/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        SharedUtil.Math.h
*  PURPOSE:
*  DEVELOPERS:
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

namespace SharedUtil
{
    // Bitwise constant operations
    template < unsigned int N >
    struct NumberOfSignificantBits
    {
        enum { COUNT = 1 + NumberOfSignificantBits<(N >> 1)>::COUNT };
    };
    template <> struct NumberOfSignificantBits<0> { enum { COUNT = 0 }; };
}