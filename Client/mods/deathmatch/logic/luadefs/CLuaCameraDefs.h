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

    static bool                                        SetCameraViewMode(std::optional<std::uint8_t> vehicleViewMode, std::optional<std::uint8_t> pedViewMode);
    static CLuaMultiReturn<std::uint8_t, std::uint8_t> GetCameraViewMode();

    // Cam get funcs
    static std::variant<CClientCamera*, bool>                                      GetCamera();
    static CLuaMultiReturn<float, float, float, float, float, float, float, float> GetCameraMatrix();
    static CMatrix                                                                 OOP_GetCameraMatrix();
    static std::variant<CClientEntity*, bool>                                      GetCameraTarget();
    static std::uint8_t                                                            GetCameraInterior();
    static std::string                                                             GetCameraGoggleEffect();
    static std::variant<float, bool>                                               GetCameraFieldOfView(eFieldOfViewMode mode);
    static std::uint8_t                                                            GetCameraDrunkLevel();

    // Cam set funcs
    static bool SetCameraMatrix(std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> lookAt, std::optional<float> roll,
                                std::optional<float> fov);
    static bool SetCameraTarget(lua_State* luaVM, std::variant<CClientEntity*, CVector> target);
    static bool SetCameraInterior(std::uint8_t interior);
    static bool SetCameraFieldOfView(eFieldOfViewMode mode, float fov, std::optional<bool> instant);
    static bool FadeCamera(bool fadeIn, std::optional<float> fadeTime, std::optional<std::uint8_t> red, std::optional<std::uint8_t> green,
                           std::optional<std::uint8_t> blue);
    static bool SetCameraClip(std::optional<bool> objects, std::optional<bool> vehicles);
    static CLuaMultiReturn<bool, bool> GetCameraClip();
    static bool                        SetCameraGoggleEffect(std::string mode, std::optional<bool> noiseEnabled);
    static bool                        SetCameraDrunkLevel(std::int16_t level);

    // Cam do funcs
    static bool ShakeCamera(float radius, std::optional<float> x, std::optional<float> y, std::optional<float> z) noexcept;
    static bool ResetShakeCamera() noexcept;

    // For OOP only
    static CVector OOP_GetCameraPosition() noexcept;
    static bool    OOP_SetCameraPosition(CVector position);
    static CVector OOP_GetCameraRotation() noexcept;
    static bool    OOP_SetCameraRotation(CVector rotation);

    static const SString& GetElementType();
};
