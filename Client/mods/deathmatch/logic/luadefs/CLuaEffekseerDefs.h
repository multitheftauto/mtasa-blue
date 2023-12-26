/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/mods/deathmatch/logic/luadefs/CLuaEffekseerDefs.h
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include "CLuaDefs.h"

class CLuaEffekseerDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    static CClientEffekseerEffect*        loadEffekseerFx(lua_State* const luaVM, std::string strPath);
    static CClientEffekseerEffectHandler* playEffekseerFx(CClientEffekseerEffect* pEffect, CVector vecPos);
    static bool                           setEffekseerFxSpeed(CClientEffekseerEffectHandler* handle, float fSpeed);
    static float                          getEffekseerFxSpeed(CClientEffekseerEffectHandler* handle);
    static bool                           setEffekseerFxScale(CClientEffekseerEffectHandler* handle, CVector vecScale);
    static bool                           setEffekseedFxInput(CClientEffekseerEffectHandler* handle, int32_t index, float fValue);
    static float                          getEffekseedFxInput(CClientEffekseerEffectHandler* handle, int32_t index);
};
