/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CDummyPool.h
 *  PURPOSE:     Dummy pool interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "Common.h"

class CDummyPool
{
public:
    virtual void RemoveAllBuildingLods() = 0;
    virtual void RestoreAllBuildingsLods() = 0;
    virtual void UpdateBuildingLods(void* oldPool, void* newPool) = 0;
};
