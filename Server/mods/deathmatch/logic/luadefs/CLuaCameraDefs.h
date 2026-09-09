/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaCameraDefs.h
 *  PURPOSE:     Lua camera function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>

class CLuaCameraDefs : public CLuaDefs
{
public:
    static void LoadFunctions();

    // Get functions
    static std::variant<CLuaMultiReturn<float, float, float, float, float, float, float, float>, bool> GetCameraMatrix(CPlayer* pPlayer);
    LUA_DECLARE(OOP_getCameraMatrix);
    static std::variant<CElement*, bool>     GetCameraTarget(CPlayer* pPlayer);
    static std::variant<unsigned char, bool> GetCameraInterior(CPlayer* pPlayer);

    // Set functions
    static bool SetCameraMatrix(CElement* pPlayer, std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> vecLookAt,
                                std::optional<float> fRoll, std::optional<float> fFOV);
    static bool SetCameraTarget(lua_State* luaVM, CElement* pPlayer, std::optional<CElement*> pTarget);
    static bool SetCameraInterior(CElement* pElement, unsigned char ucInterior);
    static bool FadeCamera(CElement* pPlayer, bool bFadeIn, std::optional<float> fFadeTime, std::optional<unsigned char> ucRed,
                           std::optional<unsigned char> ucGreen, std::optional<unsigned char> ucBlue);
};
