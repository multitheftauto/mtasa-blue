/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaCameraDefs.h
 *  PURPOSE:     Lua camera definitions class header
 *
 *****************************************************************************/

#pragma once
#include "CLuaDefs.h"
#include <lua/CLuaMultiReturn.h>
#include <optional>

class CLuaCameraDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    static bool SetCameraViewMode(std::optional<unsigned char> usVehicleViewMode, std::optional<unsigned char> usPedViewMode);
    static CLuaMultiReturn<unsigned char, unsigned char> GetCameraViewMode();

    // Cam get funcs
    LUA_DECLARE(GetCamera);
    LUA_DECLARE_OOP(GetCameraMatrix);
    LUA_DECLARE(GetCameraTarget);
    LUA_DECLARE(GetCameraInterior);
    LUA_DECLARE(GetCameraGoggleEffect);
    LUA_DECLARE(GetCameraShakeLevel);
    LUA_DECLARE(GetCameraFieldOfView);

    // Cam set funcs
    LUA_DECLARE(SetCameraMatrix);
    LUA_DECLARE(SetCameraTarget);
    LUA_DECLARE(SetCameraInterior);
    LUA_DECLARE(SetCameraFieldOfView);
    LUA_DECLARE(FadeCamera);
    LUA_DECLARE(SetCameraClip);
    LUA_DECLARE(GetCameraClip);
    LUA_DECLARE(SetCameraGoggleEffect);
    LUA_DECLARE(SetCameraShakeLevel);

    // For OOP only
    LUA_DECLARE(OOP_GetCameraPosition);
    LUA_DECLARE(OOP_SetCameraPosition);
    LUA_DECLARE(OOP_GetCameraRotation);
    LUA_DECLARE(OOP_SetCameraRotation);
};
