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

#ifdef WIN32
#include <windows.h>
#endif

#include <math.h>

#define FLOAT_EPSILON 0.0001f
#include "CVector4D.h"

/**
 * CVector Structure used to store a 3D vertex.
 */
class CVector
{
public:
    float fX, fY, fZ;
    
    CVector ()
    {
        this->fX = 0;
        this->fY = 0;
        this->fZ = 0;
    };
    
    CVector ( float fX, float fY, float fZ) 
    { 
        this->fX = fX;
        this->fY = fY;
        this->fZ = fZ;
    }

    float Normalize ( void ) 
    { 
        float t = sqrt(fX*fX + fY*fY + fZ*fZ);
        if ( t > FLOAT_EPSILON )
        {
            float fRcpt = 1 / t;
            fX *= fRcpt;
            fY *= fRcpt;
            fZ *= fRcpt;
        }
        else
            t = 0;
        return t;
    }

    float Length ( void ) const
    {
        return sqrt ( (fX*fX) + (fY*fY) + (fZ*fZ) );
    }

    float LengthSquared ( void ) const
    {
        return (fX*fX) + (fY*fY) + (fZ*fZ);
    }

    float DotProduct ( const CVector * param ) const
    {
        return fX*param->fX + fY*param->fY + fZ*param->fZ;
    }

    void CrossProduct ( const CVector * param ) 
    { 
        float _fX = fX, _fY = fY, _fZ = fZ;
        fX = _fY * param->fZ - param->fY * _fZ;
        fY = _fZ * param->fX - param->fZ * _fX;
        fZ = _fX * param->fY - param->fX * _fY;
    }

    // Convert (direction) to rotation
    CVector ToRotation ( void ) const
    {
        CVector vecRotation;
        vecRotation.fZ = atan2 ( fY, fX );
        CVector vecTemp ( sqrt ( fX * fX + fY * fY ), fZ, 0 );
        vecTemp.Normalize ();
        vecRotation.fY = atan2 ( vecTemp.fX, vecTemp.fY ) - PI / 2;
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

    CVector operator - () const
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
        float fRcpValue = 1 / fRight;
        fX *= fRcpValue;
        fY *= fRcpValue;
        fZ *= fRcpValue;
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

    CVector& operator = ( const CVector4D& vec )
    {
        fX = vec.fX;
        fY = vec.fY;
        fZ = vec.fZ;
        return *this;
    }
};

#endif
