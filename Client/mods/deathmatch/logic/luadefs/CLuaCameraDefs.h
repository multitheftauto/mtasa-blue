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
    static std::variant<CClientCamera*, bool>                                      GetCamera();
    static CLuaMultiReturn<float, float, float, float, float, float, float, float> GetCameraMatrix();
    static CMatrix                                                                 OOP_GetCameraMatrix();
    static std::variant<CClientEntity*, bool>                                      GetCameraTarget();
    static unsigned char                                                           GetCameraInterior();
    static std::string                                                             GetCameraGoggleEffect();
    static std::variant<float, bool>                                               GetCameraFieldOfView(eFieldOfViewMode eMode);
    static unsigned char                                                           GetCameraDrunkLevel();

    // Cam set funcs
    static bool SetCameraMatrix(std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> vecLookAt, std::optional<float> fRoll,
                                std::optional<float> fFOV);
    static bool SetCameraTarget(lua_State* luaVM, std::variant<CClientEntity*, CVector> target);
    static bool SetCameraInterior(unsigned char ucInterior);
    static bool SetCameraFieldOfView(eFieldOfViewMode eMode, float fFOV, std::optional<bool> instant);
    static bool FadeCamera(bool bFadeIn, std::optional<float> fFadeTime, std::optional<unsigned char> ucRed, std::optional<unsigned char> ucGreen,
                           std::optional<unsigned char> ucBlue);
    static bool SetCameraClip(std::optional<bool> bObjects, std::optional<bool> bVehicles);
    static CLuaMultiReturn<bool, bool> GetCameraClip();
    static bool                        SetCameraGoggleEffect(std::string strMode, std::optional<bool> bNoiseEnabled);
    static bool                        SetCameraDrunkLevel(short drunkLevel);

    // Cam do funcs
    static bool ShakeCamera(float radius, std::optional<float> x, std::optional<float> y, std::optional<float> z) noexcept;
    static bool ResetShakeCamera() noexcept;

    // For OOP only
    LUA_DECLARE(OOP_GetCameraPosition);
    LUA_DECLARE(OOP_SetCameraPosition);
    LUA_DECLARE(OOP_GetCameraRotation);
    LUA_DECLARE(OOP_SetCameraRotation);

    static const SString& GetElementType();
};
