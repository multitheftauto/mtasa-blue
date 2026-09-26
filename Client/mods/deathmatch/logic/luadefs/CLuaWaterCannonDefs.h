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

    static std::variant<CClientWaterCannon*, bool> CreateWaterCannon(lua_State* luaVM, CVector vecPosition, std::optional<SColor> color,
                                                                     std::optional<bool> bKnockdownEnabled);

    static bool    SetWaterCannonDirection(CClientWaterCannon* pCannon, CVector vecDirection);
    static CVector GetWaterCannonDirection(CClientWaterCannon* pCannon);

    static bool  SetWaterCannonForce(CClientWaterCannon* pCannon, float fForce);
    static float GetWaterCannonForce(CClientWaterCannon* pCannon);

    static bool SetWaterCannonEnabled(CClientWaterCannon* pCannon, bool bEnabled);
    static bool IsWaterCannonEnabled(CClientWaterCannon* pCannon);

    static bool SetWaterCannonKnockdownEnabled(CClientWaterCannon* pCannon, bool bEnabled);
    static bool IsWaterCannonKnockdownEnabled(CClientWaterCannon* pCannon);

    static bool SetWaterCannonColor(CClientWaterCannon* pCannon, unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue,
                                    std::optional<unsigned char> ucAlpha);
    static CLuaMultiReturn<uchar, uchar, uchar, uchar> GetWaterCannonColor(CClientWaterCannon* pCannon);
    static bool                                        ResetWaterCannonColor(CClientWaterCannon* pCannon);
};
