/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CAutomobile.h
 *  PURPOSE:     Automobile vehicle entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"
#include "CVehicle.h"

class CPhysical;
class CDoor;

class CAutomobile : public virtual CVehicle
{
public:
    virtual ~CAutomobile() {};

    virtual bool IsAnyWheelTouchingGround() const = 0;
};
