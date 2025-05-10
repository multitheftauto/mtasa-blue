/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CDoor.h
 *  PURPOSE:     Door entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "enums/DoorState.h"

class CDoor
{
public:
    virtual float      GetAngleOpenRatio() = 0;
    virtual bool       IsClosed() = 0;
    virtual bool       IsFullyOpen() = 0;
    virtual void       Open(float fRatio) = 0;
    virtual DoorState GetDoorState() = 0;
};
