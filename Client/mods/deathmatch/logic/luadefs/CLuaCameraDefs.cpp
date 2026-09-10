/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/luadefs/CLuaCameraDefs.cpp
 *  PURPOSE:     Lua camera definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CPlayerInfo.h>
#include <game/CSettings.h>
#include <game/CCam.h>
#include <lua/CLuaFunctionParser.h>

#include <CameraScriptShared.h>
#include <cmath>
#include <numbers>

#define MIN_CLIENT_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS "1.5.8-9.20979"

void CLuaCameraDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Cam get funcs
        {"getCamera", ArgumentParserWarn<false, GetCamera>},
        {"getCameraViewMode", ArgumentParserWarn<false, GetCameraViewMode>},
        {"getCameraMatrix", ArgumentParserWarn<false, GetCameraMatrix>},
        {"getCameraTarget", ArgumentParserWarn<false, GetCameraTarget>},
        {"getCameraInterior", ArgumentParserWarn<false, GetCameraInterior>},
        {"getCameraGoggleEffect", ArgumentParserWarn<false, GetCameraGoggleEffect>},
        {"getCameraFieldOfView", ArgumentParserWarn<false, GetCameraFieldOfView>},
        {"getCameraDrunkLevel", ArgumentParserWarn<false, GetCameraDrunkLevel>},

        // Cam set funcs
        {"setCameraMatrix", ArgumentParserWarn<false, SetCameraMatrix>},
        {"setCameraFieldOfView", ArgumentParserWarn<false, SetCameraFieldOfView>},
        {"setCameraTarget", ArgumentParserWarn<false, SetCameraTarget>},
        {"setCameraInterior", ArgumentParserWarn<false, SetCameraInterior>},
        {"fadeCamera", ArgumentParserWarn<false, FadeCamera>},
        {"setCameraClip", ArgumentParserWarn<false, SetCameraClip>},
        {"getCameraClip", ArgumentParserWarn<false, GetCameraClip>},
        {"setCameraViewMode", ArgumentParserWarn<false, SetCameraViewMode>},
        {"setCameraGoggleEffect", ArgumentParserWarn<false, SetCameraGoggleEffect>},
        {"setCameraDrunkLevel", ArgumentParserWarn<false, SetCameraDrunkLevel>},

        {"shakeCamera", ArgumentParser<ShakeCamera>},
        {"resetShakeCamera", ArgumentParser<ResetShakeCamera>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

void CLuaCameraDefs::AddClass(lua_State* luaVM)
{
    lua_newclass(luaVM);

    lua_classfunction(luaVM, "fade", "fadeCamera");
    lua_classfunction(luaVM, "resetFarClipDistance", "resetFarClipDistance");
    lua_classfunction(luaVM, "resetNearClipDistance", "resetNearClipDistance");

    lua_classfunction(luaVM, "getPosition", ArgumentParserWarn<false, OOP_GetCameraPosition>);
    lua_classfunction(luaVM, "getRotation", ArgumentParserWarn<false, OOP_GetCameraRotation>);
    lua_classfunction(luaVM, "getTarget", "getCameraTarget");
    lua_classfunction(luaVM, "getInterior", "getCameraInterior");
    lua_classfunction(luaVM, "getViewMode", "getCameraViewMode");
    lua_classfunction(luaVM, "getMatrix", ArgumentParserWarn<false, OOP_GetCameraMatrix>);
    lua_classfunction(luaVM, "getFieldOfView", "getCameraFieldOfView");
    lua_classfunction(luaVM, "getGoggleEffect", "getCameraGoggleEffect");
    lua_classfunction(luaVM, "getClip", "getCameraClip");
    lua_classfunction(luaVM, "getFarClipDistance", "getFarClipDistance");
    lua_classfunction(luaVM, "getNearClipDistance", "getNearClipDistance");
    lua_classfunction(luaVM, "getType", ArgumentParser<GetElementType>);

    lua_classfunction(luaVM, "setPosition", ArgumentParserWarn<false, OOP_SetCameraPosition>);
    lua_classfunction(luaVM, "setRotation", ArgumentParserWarn<false, OOP_SetCameraRotation>);
    lua_classfunction(luaVM, "setMatrix", "setCameraMatrix");
    lua_classfunction(luaVM, "setFieldOfView", "setCameraFieldOfView");
    lua_classfunction(luaVM, "setInterior", "setCameraInterior");
    lua_classfunction(luaVM, "setTarget", "setCameraTarget");
    lua_classfunction(luaVM, "setViewMode", "setCameraViewMode");
    lua_classfunction(luaVM, "setGoggleEffect", "setCameraGoggleEffect");
    lua_classfunction(luaVM, "setClip", "setCameraClip");
    lua_classfunction(luaVM, "setFarClipDistance", "setFarClipDistance");
    lua_classfunction(luaVM, "setNearClipDistance", "setNearClipDistance");

    lua_classvariable(luaVM, "interior", "setCameraInterior", "getCameraInterior");
    lua_classvariable(luaVM, "target", "setCameraTarget", "getCameraTarget");
    lua_classvariable(luaVM, "viewMode", "setCameraViewMode", "getCameraViewMode");
    lua_classvariable(luaVM, "goggleEffect", "setCameraGoggleEffect", "getCameraGoggleEffect");
    lua_classvariable(luaVM, "farClipDistance", "setFarClipDistance", "getFarClipDistance");
    lua_classvariable(luaVM, "nearClipDistance", "setNearClipDistance", "getNearClipDistance");

    lua_classvariable(luaVM, "position", ArgumentParserWarn<false, OOP_SetCameraPosition>, ArgumentParserWarn<false, OOP_GetCameraPosition>);
    lua_classvariable(luaVM, "rotation", ArgumentParserWarn<false, OOP_SetCameraRotation>, ArgumentParserWarn<false, OOP_GetCameraRotation>);
    lua_classvariable(luaVM, "matrix", NULL, ArgumentParserWarn<false, OOP_GetCameraMatrix>);
    lua_classvariable(luaVM, "type", nullptr, ArgumentParser<GetElementType>);

    lua_registerstaticclass(luaVM, "Camera");
}

std::variant<CClientCamera*, bool> CLuaCameraDefs::GetCamera()
{
    CClientCamera* camera = g_pClientGame->GetManager()->GetCamera();

    if (camera)
        return camera;

    return false;
}

CLuaMultiReturn<std::uint8_t, std::uint8_t> CLuaCameraDefs::GetCameraViewMode()
{
    CClientCamera* camera = g_pClientGame->GetManager()->GetCamera();

    std::uint8_t vehicleMode = static_cast<std::uint8_t>(camera->GetCameraVehicleViewMode());
    std::uint8_t pedMode = static_cast<std::uint8_t>(camera->GetCameraPedViewMode());

    return {vehicleMode, pedMode};
}

CLuaMultiReturn<float, float, float, float, float, float, float, float> CLuaCameraDefs::GetCameraMatrix()
{
    CClientCamera* camera = m_pManager->GetCamera();

    if (!camera)
        throw std::invalid_argument("Camera not available");

    CVector position, lookAt;
    float   roll = 0.0f;

    camera->GetPosition(position);
    camera->GetFixedTarget(lookAt, &roll);

    float fov = camera->GetAccurateFOV();

    if (roll == 0.0f)
    {
        CMatrix matrix;
        camera->GetMatrix(matrix);

        CVector worldUp(0.0f, 0.0f, 1.0f);
        CVector cameraUp = matrix.vUp;
        CVector cameraRight = matrix.vRight;

        CVector projectedUp = cameraUp - matrix.vFront * cameraUp.DotProduct(&matrix.vFront);

        if (projectedUp.Length() > FLOAT_EPSILON)
        {
            projectedUp.Normalize();

            float cosRoll = worldUp.DotProduct(&projectedUp);
            float sinRoll = cameraRight.DotProduct(&worldUp);

            roll = std::atan2(sinRoll, cosRoll) * (180.0f / std::numbers::pi_v<float>);
        }
    }

    return {position.fX, position.fY, position.fZ, lookAt.fX, lookAt.fY, lookAt.fZ, roll, fov};
}

CMatrix CLuaCameraDefs::OOP_GetCameraMatrix()
{
    CMatrix matrix;
    m_pManager->GetCamera()->GetMatrix(matrix);
    return matrix;
}

std::variant<CClientEntity*, bool> CLuaCameraDefs::GetCameraTarget()
{
    CClientCamera* camera = m_pManager->GetCamera();

    if (!camera)
        return false;

    if (!camera->IsInFixedMode())
    {
        if (CClientEntity* target = camera->GetTargetEntity())
            return target;
    }

    return false;
}

std::uint8_t CLuaCameraDefs::GetCameraInterior()
{
    auto* world = g_pGame->GetWorld();

    if (!world)
        throw std::invalid_argument("World not available");

    return static_cast<std::uint8_t>(world->GetCurrentArea());
}

std::string CLuaCameraDefs::GetCameraGoggleEffect()
{
    const bool isNightVision = g_pMultiplayer->IsNightVisionEnabled();
    const bool isThermalVision = g_pMultiplayer->IsThermalVisionEnabled();

    if (isNightVision)
        return "nightvision";
    else if (isThermalVision)
        return "thermalvision";
    else
        return "normal";
}

std::uint8_t CLuaCameraDefs::GetCameraDrunkLevel()
{
    return g_pGame->GetPlayerInfo()->GetCamDrunkLevel();
}

bool CLuaCameraDefs::SetCameraMatrix(std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> lookAt, std::optional<float> roll,
                                     std::optional<float> fov)
{
    CClientCamera* camera = m_pManager->GetCamera();

    if (!camera)
        return false;

    CVector position;
    CVector lookAtValue;
    bool    lookAtValid = false;

    if (auto* matrix = std::get_if<CLuaMatrix*>(&matrixOrPosition))
    {
        position = (*matrix)->GetPosition();
        lookAtValue = (*matrix)->GetRotation();
        lookAtValid = true;
    }
    else
    {
        position = std::get<CVector>(matrixOrPosition);

        if (lookAt.has_value())
        {
            lookAtValue = lookAt.value();
            lookAtValid = true;
        }
    }

    if (!CameraScriptShared::IsFiniteVector(position))
        throw std::invalid_argument("Invalid matrix/position (values out of range)");

    if (lookAtValid && !CameraScriptShared::IsFiniteVector(lookAtValue))
        throw std::invalid_argument("Invalid lookAt (values out of range)");

    const float rollValue = CameraScriptShared::NormalizeRoll(roll.value_or(0.0f));

    float fovValue = fov.value_or(70.0f);

    if (fovValue <= 0.0f || fovValue >= 180.0f)
        fovValue = 70.0f;

    if (!camera->IsInFixedMode())
        camera->ToggleCameraFixedMode(true);

    camera->SetPosition(position);

    if (lookAtValid)
        camera->SetFixedTarget(lookAtValue, rollValue);
    else
    {
        CVector previousLookAt;
        camera->GetFixedTarget(previousLookAt);
        camera->SetFixedTarget(previousLookAt, rollValue);
    }

    camera->SetFOV(fovValue);

    return true;
}

// Only when onfoot/invehicle
bool CLuaCameraDefs::SetCameraFieldOfView(eFieldOfViewMode mode, float fov, std::optional<bool> instant)
{
    if (fov < 0 || fov > 179)
        throw std::invalid_argument("Invalid FOV range (0-179)");

    const bool isInstant = instant.value_or(false);

    if (mode == FOV_MODE_PLAYER)
        g_pGame->GetSettings()->SetFieldOfViewPlayer(fov, true, isInstant);
    else if (mode == FOV_MODE_VEHICLE)
        g_pGame->GetSettings()->SetFieldOfViewVehicle(fov, true, isInstant);
    else if (mode == FOV_MODE_VEHICLE_MAX)
        g_pGame->GetSettings()->SetFieldOfViewVehicleMax(fov, true, isInstant);
    else
        throw std::invalid_argument(SString("Enum not yet implemented: " + EnumToString(mode)));

    return true;
}

// Only when onfoot/invehicle
std::variant<float, bool> CLuaCameraDefs::GetCameraFieldOfView(eFieldOfViewMode mode)
{
    if (mode == FOV_MODE_PLAYER)
        return g_pGame->GetSettings()->GetFieldOfViewPlayer();
    else if (mode == FOV_MODE_VEHICLE)
        return g_pGame->GetSettings()->GetFieldOfViewVehicle();
    else if (mode == FOV_MODE_VEHICLE_MAX)
        return g_pGame->GetSettings()->GetFieldOfViewVehicleMax();

    throw std::invalid_argument(SString("Enum not yet implemented: " + EnumToString(mode)));
}

bool CLuaCameraDefs::SetCameraTarget(lua_State* luaVM, std::variant<CClientEntity*, CVector> target)
{
    //  bool setCameraTarget ( element target = nil ) or setCameraTarget ( float x, float y, float z )
    if (auto* pTarget = std::get_if<CClientEntity*>(&target))
    {
        if (!*pTarget)
            return false;

        if ((*pTarget)->IsBeingDeleted())
            return false;

        if ((*pTarget)->GetType() != CCLIENTPLAYER)
            MinClientReqCheck(luaVM, MIN_CLIENT_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS, "target is not a player");

        CClientCamera* camera = m_pManager->GetCamera();

        if (!camera)
            return false;

        switch ((*pTarget)->GetType())
        {
            case CCLIENTPLAYER:
            {
                CClientPlayer* player = static_cast<CClientPlayer*>(*pTarget);
                if (player->IsLocalPlayer())
                {
                    camera->SetFocusToLocalPlayer();
                }
                else
                {
                    // TODO: stream in the player here (needs to be done through the streamer)

                    camera->SetFocus(player, MODE_CAM_ON_A_STRING, false);
                }
                break;
            }
            case CCLIENTPED:
            case CCLIENTVEHICLE:
            {
                camera->Reset();
                camera->SetFocus(*pTarget, MODE_CAM_ON_A_STRING, false);
                break;
            }
            default:
                return false;
        }

        return true;
    }

    CClientCamera* camera = m_pManager->GetCamera();
    if (!camera)
        return false;

    camera->SetOrbitTarget(std::get<CVector>(target));
    return true;
}

bool CLuaCameraDefs::SetCameraInterior(std::uint8_t interior)
{
    auto* world = g_pGame->GetWorld();

    if (!world)
        return false;

    world->SetCurrentArea(interior);

    return true;
}

bool CLuaCameraDefs::FadeCamera(bool fadeIn, std::optional<float> fadeTime, std::optional<std::uint8_t> red, std::optional<std::uint8_t> green,
                                std::optional<std::uint8_t> blue)
{
    CClientCamera* camera = m_pManager->GetCamera();

    if (!camera || !g_pClientGame)
        return false;

    g_pClientGame->SetInitiallyFadedOut(false);

    const float fadeTimeValue = fadeTime.value_or(1.0f);

    if (fadeIn)
    {
        camera->FadeIn(fadeTimeValue);
        if (g_pGame && g_pGame->GetHud())
            g_pGame->GetHud()->SetComponentVisible(HUD_AREA_NAME, !g_pClientGame->GetHudAreaNameDisabled());
    }
    else
    {
        camera->FadeOut(fadeTimeValue, red.value_or(0), green.value_or(0), blue.value_or(0));
        if (g_pGame && g_pGame->GetHud())
            g_pGame->GetHud()->SetComponentVisible(HUD_AREA_NAME, false);
    }

    return true;
}

bool CLuaCameraDefs::SetCameraClip(std::optional<bool> objects, std::optional<bool> vehicles)
{
    m_pManager->GetCamera()->SetCameraClip(objects.value_or(true), vehicles.value_or(true));

    return true;
}

CLuaMultiReturn<bool, bool> CLuaCameraDefs::GetCameraClip()
{
    bool isObjects = false;
    bool isVehicles = false;

    m_pManager->GetCamera()->GetCameraClip(isObjects, isVehicles);

    return {isObjects, isVehicles};
}

bool CLuaCameraDefs::SetCameraViewMode(std::optional<std::uint8_t> vehicleViewMode, std::optional<std::uint8_t> pedViewMode)
{
    CClientCamera* camera = g_pClientGame->GetManager()->GetCamera();

    if (vehicleViewMode)
        camera->SetCameraVehicleViewMode(static_cast<eVehicleCamMode>(vehicleViewMode.value()));

    if (pedViewMode)
        camera->SetCameraPedViewMode(static_cast<ePedCamMode>(pedViewMode.value()));

    return true;
}

bool CLuaCameraDefs::SetCameraGoggleEffect(std::string mode, std::optional<bool> noiseEnabled)
{
    const bool isNoiseEnabled = noiseEnabled.value_or(true);

    if (mode.compare("nightvision") == 0)
    {
        g_pMultiplayer->SetNightVisionEnabled(true, isNoiseEnabled);
        g_pMultiplayer->SetThermalVisionEnabled(false, true);
        return true;
    }
    else if (mode.compare("thermalvision") == 0)
    {
        g_pMultiplayer->SetNightVisionEnabled(false, true);
        g_pMultiplayer->SetThermalVisionEnabled(true, isNoiseEnabled);
        return true;
    }
    else if (mode.compare("normal") == 0)
    {
        g_pMultiplayer->SetNightVisionEnabled(false, true);
        g_pMultiplayer->SetThermalVisionEnabled(false, true);
        return true;
    }

    return false;
}

bool CLuaCameraDefs::SetCameraDrunkLevel(std::int16_t level)
{
    if (level < 0 || level > 255)
        throw std::invalid_argument("Invalid range (0-255)");

    CPlayerInfo* playerInfo = g_pGame->GetPlayerInfo();

    playerInfo->SetCamDrunkLevel(static_cast<unsigned char>(level));

    return true;
}

CVector CLuaCameraDefs::OOP_GetCameraPosition() noexcept
{
    CVector position;
    m_pManager->GetCamera()->GetPosition(position);
    return position;
}

bool CLuaCameraDefs::OOP_SetCameraPosition(CVector position)
{
    CClientCamera* camera = m_pManager->GetCamera();

    if (!camera->IsInFixedMode())
        camera->ToggleCameraFixedMode(true);

    camera->SetPosition(position);

    return true;
}

CVector CLuaCameraDefs::OOP_GetCameraRotation() noexcept
{
    CVector rotation;
    m_pManager->GetCamera()->GetRotationDegrees(rotation);
    return rotation;
}

bool CLuaCameraDefs::OOP_SetCameraRotation(CVector rotation)
{
    CClientCamera* camera = m_pManager->GetCamera();

    if (!camera->IsInFixedMode())
        camera->ToggleCameraFixedMode(true);

    camera->SetRotationDegrees(rotation);

    return true;
}

const SString& CLuaCameraDefs::GetElementType()
{
    return m_pManager->GetCamera()->GetTypeName();
}

bool CLuaCameraDefs::ShakeCamera(float radius, std::optional<float> x, std::optional<float> y, std::optional<float> z) noexcept
{
    if (!x || !y || !z)
    {
        const auto* player = m_pPlayerManager->GetLocalPlayer();

        CVector out;

        player->GetPosition(out);

        x = out.fX;
        y = out.fY;
        z = out.fZ;
    }

    m_pManager->GetCamera()->ShakeCamera(radius, *x, *y, *z);

    return true;
}

bool CLuaCameraDefs::ResetShakeCamera() noexcept
{
    m_pManager->GetCamera()->ResetShakeCamera();
    return true;
}
