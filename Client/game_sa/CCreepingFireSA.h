/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        sdk/game/CCreepingFireSA.h
 *  PURPOSE:     Creeping fire reimplementation
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CVector.h"
#include <cstdint>
#include <array>

class CCreepingFireSA
{
public:
    static bool TryToStartFireAtCoors(CVector position, std::uint8_t numGenerationsAllowed, float zDistance);
    static void Update();

    static void StaticSetHooks();
};
