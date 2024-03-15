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

    static bool SetCameraViewMode(std::optional<std::uint8_t> usVehicleViewMode, std::optional<std::uint8_t> usPedViewMode);
    static CLuaMultiReturn<std::uint8_t, std::uint8_t> GetCameraViewMode();

    // Cam get funcs
    static std::variant<CClientCamera*, bool>                                      GetCamera();
    static CLuaMultiReturn<float, float, float, float, float, float, float, float> GetCameraMatrix();
    static CMatrix                                                                 OOP_GetCameraMatrix();
    static std::variant<CClientEntity*, bool>                                      GetCameraTarget();
    static std::uint8_t                                                           GetCameraInterior();
    static std::string                                                             GetCameraGoggleEffect();
    LUA_DECLARE(GetCameraFieldOfView);
    static std::uint8_t GetCameraDrunkLevel();

    // Cam set funcs
    LUA_DECLARE(SetCameraMatrix);
    LUA_DECLARE(SetCameraTarget);
    LUA_DECLARE(SetCameraInterior);
    LUA_DECLARE(SetCameraFieldOfView);
    LUA_DECLARE(FadeCamera);
    LUA_DECLARE(SetCameraClip);
    LUA_DECLARE(GetCameraClip);
    LUA_DECLARE(SetCameraGoggleEffect);
    static bool SetCameraDrunkLevel(short drunkLevel);

    // For OOP only
    LUA_DECLARE(OOP_GetCameraPosition);
    LUA_DECLARE(OOP_SetCameraPosition);
    LUA_DECLARE(OOP_GetCameraRotation);
    LUA_DECLARE(OOP_SetCameraRotation);

    static const SString& GetElementType();
};
