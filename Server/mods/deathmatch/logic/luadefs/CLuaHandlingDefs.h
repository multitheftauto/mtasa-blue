/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaHandlingDefs.h
 *  PURPOSE:     Lua vehicle handling definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaHandlingDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    // Set
    LUA_DECLARE(SetVehicleHandling);
    LUA_DECLARE(SetModelHandling);

    // Get
    LUA_DECLARE(GetVehicleHandling);
    LUA_DECLARE(GetModelHandling);
    LUA_DECLARE(GetOriginalHandling);
};