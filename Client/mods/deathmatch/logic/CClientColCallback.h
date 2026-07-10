/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CClientColCallback.h
 *  PURPOSE:     Collision entity events class
 *
 *****************************************************************************/

#pragma once

#include "CClientColShape.h"

class CClientColCallback
{
public:
    // Whether a geometric hit should be stored in collision state. Callbacks can defer
    // tracking until interior/dimension checks pass, so stale hits are cleared. #5018
    virtual bool ShouldTrackCollision(CClientColShape& Shape, CClientEntity& Entity) = 0;
    virtual void Callback_OnCollision(CClientColShape& Shape, CClientEntity& Entity) = 0;
    virtual void Callback_OnLeave(CClientColShape& Shape, CClientEntity& Entity) = 0;
};
