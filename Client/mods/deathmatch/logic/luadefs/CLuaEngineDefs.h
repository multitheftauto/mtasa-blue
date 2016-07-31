/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaEngineDefs.cpp
*  PURPOSE:     Lua definitions class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaEngineDefs : public CLuaDefs
{
public:
    static void LoadFunctions ( void );
    static void AddClass ( lua_State* luaVM );

    LUA_DECLARE ( EngineLoadDFF );
    LUA_DECLARE ( EngineLoadTXD );
    LUA_DECLARE ( EngineLoadCOL );
    LUA_DECLARE ( EngineImportTXD );
    LUA_DECLARE ( EngineReplaceCOL );
    LUA_DECLARE ( EngineRestoreCOL );
    LUA_DECLARE ( EngineReplaceModel );
    LUA_DECLARE ( EngineRestoreModel );
    LUA_DECLARE ( EngineReplaceMatchingAtomics );
    LUA_DECLARE ( EngineReplaceWheelAtomics );
    LUA_DECLARE ( EnginePositionAtomic );
    LUA_DECLARE ( EnginePositionSeats );
    LUA_DECLARE ( EngineAddAllAtomics );
    LUA_DECLARE ( EngineReplaceVehiclePart );
    LUA_DECLARE ( EngineGetModelLODDistance );
    LUA_DECLARE ( EngineSetModelLODDistance );
    LUA_DECLARE ( EngineSetAsynchronousLoading );
    LUA_DECLARE ( EngineApplyShaderToWorldTexture );
    LUA_DECLARE ( EngineRemoveShaderFromWorldTexture );
    LUA_DECLARE ( EngineGetModelNameFromID );
    LUA_DECLARE ( EngineGetModelIDFromName );
    LUA_DECLARE ( EngineGetModelTextureNames );
    LUA_DECLARE ( EngineGetVisibleTextureNames );

private: 
    static void AddEngineColClass ( lua_State* luaVM );
    static void AddEngineTxdClass ( lua_State* luaVM );
    static void AddEngineDffClass ( lua_State* luaVM );
};
