/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaFxDefs.h
*  PURPOSE:     Lua fx definitions class header
*  DEVELOPERS:  Jax <>
*
*****************************************************************************/

#ifndef __CLuaFxDefs_H
#define __CLuaFxDefs_H

#include "CLuaDefs.h"
#include "lua/LuaCommon.h"

#define LUA_DECLARE(x) static int x ( lua_State * luaVM );

class CLuaFxDefs: public CLuaDefs
{
public:
    static void     LoadFunctions                       ( void );

    LUA_DECLARE ( fxAddBlood );
    LUA_DECLARE ( fxAddWood );
    LUA_DECLARE ( fxAddSparks );
    LUA_DECLARE ( fxAddTyreBurst );
    LUA_DECLARE ( fxAddBulletImpact );
    LUA_DECLARE ( fxAddPunchImpact );
    LUA_DECLARE ( fxAddDebris );
    LUA_DECLARE ( fxAddGlass );
    LUA_DECLARE ( fxAddWaterHydrant );
    LUA_DECLARE ( fxAddGunshot );
    LUA_DECLARE ( fxAddTankFire );
    LUA_DECLARE ( fxAddWaterSplash );
    LUA_DECLARE ( fxAddBulletSplash );
    LUA_DECLARE ( fxAddFootSplash );
    LUA_DECLARE ( CreateEffect );
    LUA_DECLARE ( SetEffectSpeed );
    LUA_DECLARE ( GetEffectSpeed );
    LUA_DECLARE ( SetEffectDensity );
    LUA_DECLARE ( GetEffectDensity );
};

#endif
