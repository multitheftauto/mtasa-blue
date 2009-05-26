/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        Shared/sdk/ieee754.h
*  PURPOSE:     IEEE754 representation for floating point real numbers.
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*               
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

template < typename CType,              // The C type that the current definition refers to
           typename CSignedRawType,     // A C type that can fit every field of the raw binary data (signed)
           typename CUnsignedRawType,   // A C type that can fit every field of the raw binary data (unsigned)
           unsigned int exponentBits,   // Number of bits assigned to the exponent
           unsigned int mantissaBits >  // Number of bits assigned to the mantissa
struct IEEE754
{
    typedef IEEE754 < CType, CSignedRawType, CUnsignedRawType, exponentBits, mantissaBits > thisType;

    union
    {
        struct
        {
            // WARNING: This is made to work for little endian systems.
            // Add support for any endianness here.
            CUnsignedRawType mantissa  : mantissaBits;
            CUnsignedRawType exponent  : exponentBits;
            CUnsignedRawType sign      : 1;
        } fields;

        CUnsignedRawType raw;
        CType value;
    } representation;

    IEEE754 ( ) { representation.raw = 0; }

    explicit IEEE754 ( const CType& value )
    {
        representation.value = value;
    }

    IEEE754 ( const CUnsignedRawType& sign,
              const CUnsignedRawType& exponent,
              const CUnsignedRawType& mantissa )
    {
        representation.fields.sign = sign;
        representation.fields.exponent = exponent;
        representation.fields.mantissa = mantissa;
    }

    thisType operator= ( const CType& value )
    {
        representation.value = value;
        return *this;
    }

    bool isinf ( ) const            { return representation.fields.exponent == ~0 && representation.fields.mantissa == 0; }
    bool isnan ( ) const            { return isinf () || ( representation.fields.exponent == ~0 && representation.fields.mantissa != 0 ); }

    bool ispositiveinf ( ) const    { return isinf () && representation.fields.sign == 0; }
    bool isnegativeinf ( ) const    { return isinf () && representation.fields.sign == 1; }

    bool isnormalised ( ) const     { return !( representation.fields.exponent == ~0 || representation.fields.exponent == 0 ); }

    // Get the exponent bias for the given exponent bitcount
    unsigned int exponentBias ( ) const
    {
        return ( 1 << ( exponentBits - 1 ) ) - 1;
    }

    // Get the type that this instance refers to directly
    operator const CType& ( ) const { return representation.value; }
    const CType& c_value  ( ) const { return representation.value; }

    // Get every component of the real number
    // sign: 0 == positive, 1 == negative.
    unsigned char sign ( ) const
    {
        return ( representation.fields.sign != 0 );
    }

    // exponent: Apply the bias for the given exponent bit count
    CSignedRawType exponent ( ) const
    {
        return representation.fields.exponent - exponentBias ();
    }

    // mantissa: Build a new instance with exponent zero
    CType mantissa ( ) const
    {
        thisType newInstance = *this;
        newInstance.representation.fields.exponent = exponentBias ();
        return newInstance.representation.value;
    }

    // Constants
    static const thisType& positiveInfinite ( )
    {
        static thisType s_positiveInfinite ( 0, ~0, 0 );
        return s_positiveInfinite;
    }

    static const thisType& negativeInfinite ( )
    {
        static thisType s_negativeInfinite ( 1, ~0, 0 );
        return s_negativeInfinite;
    }
};

typedef IEEE754 < float,  int,       unsigned int,        8, 23 > IEEE754_SP;    // Simple precision (32-bit float)
typedef IEEE754 < double, long long, unsigned long long, 11, 52 > IEEE754_DP;    // Double precision (64-bit double)
