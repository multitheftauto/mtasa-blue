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
        double dFactor = pow( 10.0, 8 - ceil( log10( fabs( dValue ) ) ) );
        return floor( dValue * dFactor + 0.5 ) / dFactor;
    }

    // Determine if value would be better as an int or float.
    // piNumber is set if result is true
    inline bool ShouldUseInt( double dValue, int* piNumber )
    {
        if ( dValue > -0x1000000 && dValue < 0x1000000 )
        {
            // float more accurate with this range, but check if we can use int as it is better for compressing
            *piNumber = static_cast < int > ( dValue );
            return ( dValue == *piNumber );
        }
        else
        if ( dValue >= -0x7FFFFFFF && dValue <= 0x7FFFFFFF )
        {
            // int more accurate with this range
            *piNumber = Round( dValue );
            return true;
        }
        else
        {
            // Value too large for int
            return false;
        }
    }

    inline float DegreesToRadians( float fValue )
    {
        return fValue * 0.017453292f;
    }
}
