/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/CRect2D.h
*  PURPOSE:     2D rectangle interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CRECT2D_H
#define __CRECT2D_H

/**
 * CRect2D Structure used to store a 2D rectangle
 */
class CRect2D
{
public:
    float fX1, fY1, fX2, fY2;

    CRect2D () 
    {
        this->fX1 = 0;
        this->fY1 = 0;
        this->fX2 = 0;
        this->fY2 = 0;
    };

    CRect2D ( float fX1, float fY1, float fX2, float fY2 )
    {
        this->fX1 = fX1;
        this->fY1 = fY1;
        this->fX2 = fX2;
        this->fY2 = fY2;
    };

    bool        operator==      ( const CRect2D& rhs ) const
    {
        return ( ( fX1 == rhs.fX1 ) && ( fX2 == rhs.fX2 ) && ( fY1 == rhs.fY1 ) && ( fY2 == rhs.fY2 ) );
    }

    bool        operator!=      ( const CRect2D& rhs ) const
    {
        return !( operator== ( rhs ) );
    }

    CRect2D&   operator=       ( const CRect2D& rhs );

};

#endif
