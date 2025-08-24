/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CBike.h
 *  PURPOSE:     Bike vehicle entity interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CVehicle.h"

class CBikeHandlingEntry;

class CBike : public virtual CVehicle
{
public:
    virtual ~CBike(){};

    virtual CBikeHandlingEntry* GetBikeHandlingData() = 0;
    virtual void                SetBikeHandlingData(CBikeHandlingEntry* pHandling) = 0;
    virtual void                RecalculateBikeHandling() = 0;
};
