/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaWaterCannonDefs.h
 *  PURPOSE:     Lua definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaFunctionParser.h>

class CLuaWaterCannonDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static std::variant<CClientWaterCannon*, bool> CreateWaterCannon(lua_State* luaVM, CVector vecPosition);

    static bool    SetWaterCannonDirection(CClientWaterCannon* pCannon, CVector vecDirection);
    static CVector GetWaterCannonDirection(CClientWaterCannon* pCannon);

    static bool  SetWaterCannonForce(CClientWaterCannon* pCannon, float fForce);
    static float GetWaterCannonForce(CClientWaterCannon* pCannon);

    static bool SetWaterCannonEnabled(CClientWaterCannon* pCannon, bool bEnabled);
    static bool IsWaterCannonEnabled(CClientWaterCannon* pCannon);
};
