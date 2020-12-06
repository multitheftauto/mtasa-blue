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

class CLuaCameraDefs : public CLuaDefs
{
public:
    static void LoadFunctions();
    static void AddClass(lua_State* luaVM);

    // Cam get funcs
    LUA_DECLARE(GetCamera);
    LUA_DECLARE(GetCameraViewMode);
    LUA_DECLARE_OOP(GetCameraMatrix);
    LUA_DECLARE(GetCameraTarget);
    LUA_DECLARE(GetCameraInterior);
    LUA_DECLARE(GetCameraGoggleEffect);
    LUA_DECLARE(GetCameraFieldOfView);
    static unsigned char GetCameraDrunkLevel();

    // Cam set funcs
    LUA_DECLARE(SetCameraMatrix);
    LUA_DECLARE(SetCameraTarget);
    LUA_DECLARE(SetCameraInterior);
    LUA_DECLARE(SetCameraFieldOfView);
    LUA_DECLARE(FadeCamera);
    LUA_DECLARE(SetCameraClip);
    LUA_DECLARE(GetCameraClip);
    LUA_DECLARE(SetCameraViewMode);
    LUA_DECLARE(SetCameraGoggleEffect);
    static bool SetCameraDrunkLevel(short drunkLevel);

    // For OOP only
    LUA_DECLARE(OOP_GetCameraPosition);
    LUA_DECLARE(OOP_SetCameraPosition);
    LUA_DECLARE(OOP_GetCameraRotation);
    LUA_DECLARE(OOP_SetCameraRotation);
};
