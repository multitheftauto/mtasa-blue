/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/CVector2D.h
*  PURPOSE:		2D vector class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CVector2D_H
#define __CVector2D_H

#include <cmath>

/**
 * CVector2D Structure used to store a 2D vertex.
 */
class CVector2D
{
public:
    CVector2D ( void )
    {
        fX = 0;
        fY = 0;
    }

    CVector2D ( float _fX, float _fY )
    {
        fX = _fX;
        fY = _fY;
    }

    float DotProduct ( CVector2D& other )
    {
        return fX*other.fX + fY*other.fY;
    }

    float Length ()
    {
        return sqrt ( fX * fX + fY * fY );
    }

    void Normalize ( void ) 
    { 
        float fLength = Length ();
        if ( fLength > 0.0f )
        {
            fX /= fLength;
            fY /= fLength;
        }
    }

    CVector2D operator + ( const CVector2D& vecRight ) const
    {
        return CVector2D ( fX + vecRight.fX, fY + vecRight.fY );
    }

    CVector2D operator - ( const CVector2D& vecRight ) const
    {
        return CVector2D ( fX - vecRight.fX, fY - vecRight.fY );
    }

    CVector2D operator * ( const CVector2D& vecRight ) const
    {
        return CVector2D ( fX * vecRight.fX, fY * vecRight.fY );
    }

    CVector2D operator / ( const CVector2D& vecRight ) const
    {
        return CVector2D ( fX / vecRight.fX, fY / vecRight.fY );
    }

    void operator += ( float fRight )
    {
        fX += fRight;
        fY += fRight;
    }

    void operator += ( const CVector2D& vecRight )
    {
        fX += vecRight.fX;
        fY += vecRight.fY;
    }

    void operator -= ( float fRight )
    {
        fX -= fRight;
        fY -= fRight;
    }

    void operator -= ( const CVector2D& vecRight )
    {
        fX -= vecRight.fX;
        fY -= vecRight.fY;
    }

    void operator *= ( float fRight )
    {
        fX *= fRight;
        fY *= fRight;
    }

    void operator *= ( const CVector2D& vecRight )
    {
        fX *= vecRight.fX;
        fY *= vecRight.fY;
    }

    void operator /= ( float fRight )
    {
        fX /= fRight;
        fY /= fRight;
    }

    void operator /= ( const CVector2D& vecRight )
    {
        fX /= vecRight.fX;
        fY /= vecRight.fY;
    }

    bool operator == ( const CVector2D& vecRight ) const
    {
        return fX == vecRight.fX && fY == vecRight.fY;
    }

    bool operator != ( const CVector2D& vecRight ) const
    {
        return !(*this == vecRight);
    }

    float fX;
    float fY;
};


#endif