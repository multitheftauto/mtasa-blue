/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.x
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaBuildingDefs.h
 *  PURPOSE:     Lua building definitions class header
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"

class CLuaBuildingDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Buiding create funcs
    static CClientBuilding* CreateBuilding(lua_State* const luaVM, uint16_t modelId, CVector pos, CVector rot, std::optional<uint8_t> interior);
    static void             RemoveAllGameBuildings();
    static void             RestoreGameBuildings();
};
