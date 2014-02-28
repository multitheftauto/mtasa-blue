/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/CVector.h
*  PURPOSE:     3D vector math implementation
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVector_H
#define __CVector_H

#define _USE_MATH_DEFINES
#include <math.h>

#define FLOAT_EPSILON 0.0001f

/**
 * CVector Structure used to store a 3D vertex.
 */
class CVector
{
public:
    float fX, fY, fZ;
    
    CVector ( void )
    {
        fX = 0;
        fY = 0;
        fZ = 0;
    };
    
    CVector ( float x, float y, float z) 
    { 
        fX = x;
        fY = y;
        fZ = z;
    }

    inline void Reset ( void )
    {
        fX = 0;
        fY = 0;
        fZ = 0;
    }

    inline float Normalize ( void ) 
    { 
        double length = Length();

        if ( length < FLOAT_EPSILON )
            return 0;

        Divide( (float)length );
        return (float)length;
    }

    inline float Length ( void ) const
    {
        return sqrt ( (fX*fX) + (fY*fY) + (fZ*fZ) );
    }

    inline float LengthSquared ( void ) const
    {
        return (fX*fX) + (fY*fY) + (fZ*fZ);
    }

    inline float DotProduct( const CVector& param ) const
    {
        return fX*param.fX + fY*param.fY + fZ*param.fZ;
    }

    inline float DotProduct ( const CVector *param ) const
    {
        return DotProduct( *param );
    }

    inline void CrossProduct( const CVector& param )
    {
        float _fX = fX, _fY = fY, _fZ = fZ;
        fX = _fY * param.fZ - param.fY * _fZ;
        fY = _fZ * param.fX - param.fZ * _fX;
        fZ = _fX * param.fY - param.fX * _fY;
    }

    inline void CrossProduct( const CVector *param ) 
    { 
        CrossProduct( *param );
    }

    inline void Divide( float right )
    {
        fX /= right;
        fY /= right;
        fZ /= right;
    }

    // Convert (direction) to rotation
    CVector ToRotation ( void ) const
    {
        CVector vecRotation;
        vecRotation.fZ = atan2 ( fY, fX );
        CVector vecTemp ( sqrt ( fX * fX + fY * fY ), fZ, 0 );
        vecTemp.Normalize ();
        vecRotation.fY = atan2 ( vecTemp.fX, vecTemp.fY ) - (float)M_PI / 2;
        return vecRotation;
    }

    // Return a perpendicular direction
    CVector GetOtherAxis ( void ) const
    {
        CVector vecResult;
        if ( abs( fX ) > abs( fY ) )
	        vecResult = CVector( fZ, 0, -fX );
        else
	        vecResult = CVector( 0, -fZ, fY );
        vecResult.Normalize();
        return vecResult;
    }

    CVector operator + ( const CVector& vecRight ) const
    {
        return CVector ( fX + vecRight.fX, fY + vecRight.fY, fZ + vecRight.fZ );
    }

    CVector operator - ( const CVector& vecRight ) const
    {
        return CVector ( fX - vecRight.fX, fY - vecRight.fY, fZ - vecRight.fZ );
    }

    CVector operator * ( const CVector& vecRight ) const
    {
        return CVector ( fX * vecRight.fX, fY * vecRight.fY, fZ * vecRight.fZ );
    }

    CVector operator * ( float fRight ) const
    {
        return CVector ( fX * fRight, fY * fRight, fZ * fRight );
    }

    CVector operator / ( const CVector& vecRight ) const
    {
        return CVector ( fX / vecRight.fX, fY / vecRight.fY, fZ / vecRight.fZ );
    }

    CVector operator / ( float fRight ) const
    {
        float fRcpValue = 1 / fRight;
        return CVector ( fX * fRcpValue, fY * fRcpValue, fZ * fRcpValue );
    }

    CVector operator - ( void ) const
    {
        return CVector ( -fX, -fY, -fZ );
    }

    void operator += ( float fRight )
    {
        fX += fRight;
        fY += fRight;
        fZ += fRight;
    }

    void operator += ( const CVector& vecRight )
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
        fZ += vecRight.fZ;
    }

    void operator -= ( float fRight )
    {
        fX -= fRight;
        fY -= fRight;
        fZ -= fRight;
    }

    void operator -= ( const CVector& vecRight )
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
        fZ -= vecRight.fZ;
    }

    void operator *= ( float fRight )
    {
        fX *= fRight;
        fY *= fRight;
        fZ *= fRight;
    }

    void operator *= ( const CVector& vecRight )
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
        fZ *= vecRight.fZ;
    }

    void operator /= ( float fRight )
    {
        Divide( fRight );
    }

    void operator /= ( const CVector& vecRight )
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
    }

    bool operator== ( const CVector& param ) const
    {
        return ( ( fabs ( fX - param.fX ) < FLOAT_EPSILON ) &&
                 ( fabs ( fY - param.fY ) < FLOAT_EPSILON ) &&
                 ( fabs ( fZ - param.fZ ) < FLOAT_EPSILON ) );
    }

    bool operator!= ( const CVector& param ) const
    {
        return ( ( fabs ( fX - param.fX ) >= FLOAT_EPSILON ) ||
                 ( fabs ( fY - param.fY ) >= FLOAT_EPSILON ) ||
                 ( fabs ( fZ - param.fZ ) >= FLOAT_EPSILON ) );
    }

    inline float operator [] ( unsigned int idx ) const
    {
        return ((float*)this)[idx];
    }

    inline float& operator [] ( unsigned int idx )
    {
        return ((float*)this)[idx];
    }
};

#endif
