/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaFBXDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaFBXDefs : public CLuaDefs
{
public:
    static void LoadFunctions(void);
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(FBXLoadFile);
    LUA_DECLARE(FBXGetAllMeshes);
    LUA_DECLARE(FBXGetMeshProperties);
    LUA_DECLARE(FBXGetMeshRawData);
    LUA_DECLARE(FBXDrawPreview);
};
