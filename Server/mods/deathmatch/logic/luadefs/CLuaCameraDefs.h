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
    static std::variant<CLuaMultiReturn<float, float, float, float, float, float, float, float>, bool> GetCameraMatrix(CPlayer* player);
    static std::variant<CElement*, bool>                                                               GetCameraTarget(CPlayer* player);
    static std::variant<std::uint8_t, bool>                                                            GetCameraInterior(CPlayer* player);

    // Set functions
    static bool SetCameraMatrix(CElement* element, std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> lookAt,
                                std::optional<float> roll, std::optional<float> fov);
    static bool SetCameraTarget(lua_State* luaVM, CElement* element, std::optional<CElement*> target);
    static bool SetCameraInterior(CElement* element, std::uint8_t interior);
    static bool FadeCamera(CElement* element, bool fadeIn, std::optional<float> fadeTime, std::optional<std::uint8_t> red, std::optional<std::uint8_t> green,
                           std::optional<std::uint8_t> blue);
};
