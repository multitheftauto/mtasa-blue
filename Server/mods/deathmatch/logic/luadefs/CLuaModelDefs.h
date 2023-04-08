/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaModelDefs.h
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "StdInc.h"
#include "luadefs/CLuaDefs.h"

class CLuaModelDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static bool AllocateModelFromParent(lua_State* const luaVM, uint32_t uiModelID, uint32_t uiParentModelID);
    static bool UnloadModel(lua_State* const luaVM, uint32_t uiModelID);

    static std::list<uint32_t> GetAllocatedModels(std::optional<eModelInfoType> eType);
    static std::list<uint32_t> GetFreeModels();
};
