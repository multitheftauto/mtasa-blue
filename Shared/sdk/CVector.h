/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/CVector.h
*  PURPOSE:		3D vector math implementation
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVector_H
#define __CVector_H

#include <windows.h>
#include <math.h>

/**
 * CVector Structure used to store a 3D vertex.
 */
class CVector
{
public:
	FLOAT fX, fY, fZ;
	
	CVector() 
	{
		this->fX = 0;
		this->fY = 0;
		this->fZ = 0;
	};
	// contructor
	CVector(FLOAT fX, FLOAT fY, FLOAT fZ) 
	{ 
		this->fX = fX;
		this->fY = fY;
		this->fZ = fZ;
	}

    void Normalize( void ) 
    { 
        double t = sqrt(fX*fX + fY*fY + fZ*fZ);
        if ( t > 0 )
        {
            double fX2 = fX / t;
            double fY2 = fY / t;
            double fZ2 = fZ / t;
            fX = (float)fX2;
            fY = (float)fY2;
            fZ = (float)fZ2;
        }
    }

	float Length( void )
	{
		return sqrt((fX*fX) + (fY*fY) + (fZ*fZ));
	}

    void dotproduct( CVector * param ) 
    { 
        double xh = fY * param->fZ - param->fY * fZ;
        double yh = fZ * param->fX - param->fZ * fX;
        double zh = fX * param->fY - param->fX * fY;
        fX = (float)xh;
        fY = (float)yh;
        fZ = (float)zh;
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
        fX /= fRight;
        fY /= fRight;
        fZ /= fRight;
    }

    void operator /= ( const CVector& vecRight )
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
        fZ /= vecRight.fZ;
    }

    bool operator== ( const CVector& param ) const
    {
        return (fX == param.fX && fY == param.fY && fZ == param.fZ);
    }

    bool operator!= ( const CVector& param ) const
    {
        return (fX != param.fX || fY != param.fY || fZ != param.fZ);
    }
};

#endif