/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/CSphere.h
*  PURPOSE:     Sphere class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CSphere_H
#define __CSphere_H

#include "CVector.h"

class CSphere
{
public:
    CVector     vecPosition;
    float       fRadius;

    CSphere ()
        : fRadius ( 0.0f )
    {
    }

    CSphere ( const CVector& vecInPosition, float fInRadius )
        : vecPosition ( vecInPosition )
        , fRadius ( fInRadius )
    {
    }

    bool operator== ( const CSphere& other ) const
    {
        return ( vecPosition == other.vecPosition &&
                 fRadius == other.fRadius );
    }

    bool operator!= ( const CSphere& other ) const
    {
        return !operator== ( other );
    }
};

#endif
