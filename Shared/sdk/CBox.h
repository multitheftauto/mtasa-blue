/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/CBox.h
*  PURPOSE:     Box class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CBox_H
#define __CBox_H

#include "CVector.h"

class CBox
{
public:
    CVector     vecMin;
    CVector     vecMax;

    CBox ()
    {
    }

    CBox ( const CVector& vecInMin, const CVector& vecInMax )
        : vecMin ( vecInMin )
        , vecMax ( vecInMax )
    {
    }

    // Construct from a sphere
    CBox ( const CVector& vecPosition, float fRadius )
    {
        vecMin.fX = vecPosition.fX - fRadius;
        vecMin.fY = vecPosition.fY - fRadius;
        vecMin.fZ = vecPosition.fZ - fRadius;
        vecMax.fX = vecPosition.fX + fRadius;
        vecMax.fY = vecPosition.fY + fRadius;
        vecMax.fZ = vecPosition.fZ + fRadius;
    }

    bool operator== ( const CBox& other ) const
    {
        return ( vecMin == other.vecMin &&
                 vecMax == other.vecMax );
    }

    bool operator!= ( const CBox& other ) const
    {
        return !operator== ( other );
    }
};

#endif