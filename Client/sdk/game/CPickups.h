/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CPickups.h
 *  PURPOSE:     Pickup entity manager interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "enums/PickupType.h"

class CPickup;
class CVector;

class CPickups
{
public:
    virtual CPickup* CreatePickup(CVector* position, DWORD ModelIndex, PickupType Type = PickupType::PICKUP_ONCE, DWORD dwMonetaryValue = 0, DWORD dwMoneyPerDay = 0,
                                  BYTE bPingOutOfPlayer = 0) = 0;
    virtual void     DisablePickupProcessing(bool bDisabled) = 0;
};
