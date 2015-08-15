/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaPickupDefs.h
*  PURPOSE:     Lua pickup definitions class
*  DEVELOPERS:  Jax <>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaPickupDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );
    static void     AddClass ( lua_State* luaVM );

    // Create/destroy
    static int      createPickup                        ( lua_State* luaVM );

    // Get
    static int      getPickupType                       ( lua_State* luaVM );
    static int      getPickupWeapon                     ( lua_State* luaVM );
    static int      getPickupAmount                     ( lua_State* luaVM );
    static int      getPickupAmmo                       ( lua_State* luaVM );
    static int      getPickupRespawnInterval            ( lua_State* luaVM );
    static int      isPickupSpawned                     ( lua_State* luaVM );

    // Set
    static int      setPickupType                       ( lua_State* luaVM );
    static int      setPickupRespawnInterval            ( lua_State* luaVM );
    static int      usePickup                           ( lua_State* luaVM );
};
