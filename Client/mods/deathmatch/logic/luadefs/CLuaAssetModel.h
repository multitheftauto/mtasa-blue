/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaAssetModelDefs.cpp
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaAssetModelDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    LUA_DECLARE(LoadAssetModel);
    LUA_DECLARE(GetAssetProperties);
    LUA_DECLARE(AssetGetChilldrenNodes);
    LUA_DECLARE(AssetGetNodeMeshes);
    LUA_DECLARE(AssetGetTextures);
    LUA_DECLARE(AssetRender);
    LUA_DECLARE(AssetSetTexture);
    LUA_DECLARE(AssetGetMaterialProperties);
    LUA_DECLARE(AssetGetMetaData);
    LUA_DECLARE(AssetGetRenderGroupProperties);
    LUA_DECLARE(AssetSetRenderGroupProperties);
};
