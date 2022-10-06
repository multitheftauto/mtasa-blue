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

#pragma once

#include "Common.h"

class CDoor
{
public:
    virtual float      GetAngleOpenRatio() = 0;
    virtual bool       IsClosed() = 0;
    virtual bool       IsFullyOpen() = 0;
    virtual void       Open(float fRatio) = 0;
    virtual eDoorState GetDoorState() = 0;
};
