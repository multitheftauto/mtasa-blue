/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        sdk/game/CDoor.h
*  PURPOSE:     Door entity interface
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CGAME_DOOR
#define __CGAME_DOOR

#include <windows.h>

#include "Common.h"

class CDoor
{
public:
    virtual FLOAT           GetAngleOpenRatio ( )=0;
    virtual BOOL            IsClosed (  )=0;
    virtual BOOL            IsFullyOpen (  )=0;
    virtual VOID            Open ( float fUnknown )=0;
    virtual eDoorState      GetDoorState()=0;
};

#endif
