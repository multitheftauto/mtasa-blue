/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
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
    virtual void RemoveAllWithBackup() = 0;
    virtual void RestoreBackup() = 0;
    virtual void UpdateBuildingLods(const std::uint32_t offset) = 0;
};
