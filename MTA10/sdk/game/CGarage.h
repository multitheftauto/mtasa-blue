/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CGarage.h
*  PURPOSE:     Garage interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_GARAGE
#define __CGAME_GARAGE

#include "Common.h"
#include <CVector.h>

class CGarageInterface
{
public:
};

class CGarage
{
public:
    virtual void                    SetType         ( BYTE type ) = 0;
    virtual BYTE                    GetType         ( ) const = 0;

    virtual void                    GetPosition         ( CVector& vecPosition ) const = 0;
    virtual void                    GetDirection        ( FLOAT& x1, FLOAT& y1, FLOAT& x2, FLOAT& y2 ) const = 0;
    virtual void                    GetSize             ( FLOAT& height, FLOAT& width, FLOAT& depth ) const = 0;
    virtual void                    GetBoundingBox      ( FLOAT& left, FLOAT& right, FLOAT& front, FLOAT& back ) const = 0;

    virtual void                    SetOpen             ( bool bOpen ) = 0;
    virtual bool                    IsOpen              ( ) const = 0;

    virtual CGarageInterface*       GetInterface    ( ) = 0;
};

#endif
