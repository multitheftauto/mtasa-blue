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


    template < class T >
    T Square ( const T& value )
    {
        return value * value;
    }


    // Remove possible crap after casting a float to a double
    inline double RoundFromFloatSource( double dValue )
    {
        double dFactor = pow( 10.0, 7 - ceil( log10( fabs( dValue ) ) ) );
        return floor( dValue * dFactor + 0.5 ) / dFactor;
    }

    inline float DegreesToRadians( float fValue )
    {
        return fValue * 0.017453292f;
    }
}
