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

    lua_classfunction(luaVM, "getPosition", OOP_GetCameraPosition);
    lua_classfunction(luaVM, "getRotation", OOP_GetCameraRotation);
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

    lua_classfunction(luaVM, "setPosition", OOP_SetCameraPosition);
    lua_classfunction(luaVM, "setRotation", OOP_SetCameraRotation);
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

    lua_classvariable(luaVM, "position", OOP_SetCameraPosition, OOP_GetCameraPosition);
    lua_classvariable(luaVM, "rotation", OOP_SetCameraRotation, OOP_GetCameraRotation);
    lua_classvariable(luaVM, "matrix", NULL, ArgumentParserWarn<false, OOP_GetCameraMatrix>);
    lua_classvariable(luaVM, "type", nullptr, ArgumentParser<GetElementType>);

    lua_registerstaticclass(luaVM, "Camera");
}

std::variant<CClientCamera*, bool> CLuaCameraDefs::GetCamera()
{
    CClientCamera* pCamera = g_pClientGame->GetManager()->GetCamera();
    if (pCamera)
        return pCamera;
    return false;
}

CLuaMultiReturn<unsigned char, unsigned char> CLuaCameraDefs::GetCameraViewMode()
{
    CClientCamera* pCamera = g_pClientGame->GetManager()->GetCamera();

    unsigned char ucVehicleMode = (unsigned char)pCamera->GetCameraVehicleViewMode();
    unsigned char ucPedMode = (unsigned char)pCamera->GetCameraPedViewMode();

    return {ucVehicleMode, ucPedMode};
}

CLuaMultiReturn<float, float, float, float, float, float, float, float> CLuaCameraDefs::GetCameraMatrix()
{
    CVector vecPosition, vecLookAt;
    float   fRoll, fFOV;
    CStaticFunctionDefinitions::GetCameraMatrix(vecPosition, vecLookAt, fRoll, fFOV);
    return {vecPosition.fX, vecPosition.fY, vecPosition.fZ, vecLookAt.fX, vecLookAt.fY, vecLookAt.fZ, fRoll, fFOV};
}

CMatrix CLuaCameraDefs::OOP_GetCameraMatrix()
{
    CMatrix matrix;
    m_pManager->GetCamera()->GetMatrix(matrix);
    return matrix;
}

std::variant<CClientEntity*, bool> CLuaCameraDefs::GetCameraTarget()
{
    CClientEntity* pTarget = CStaticFunctionDefinitions::GetCameraTarget();
    if (pTarget)
        return pTarget;
    return false;
}

unsigned char CLuaCameraDefs::GetCameraInterior()
{
    unsigned char ucInterior;
    CStaticFunctionDefinitions::GetCameraInterior(ucInterior);
    return ucInterior;
}

std::string CLuaCameraDefs::GetCameraGoggleEffect()
{
    bool bNightVision = g_pMultiplayer->IsNightVisionEnabled();
    bool bThermalVision = g_pMultiplayer->IsThermalVisionEnabled();

    if (bNightVision)
        return "nightvision";
    else if (bThermalVision)
        return "thermalvision";
    else
        return "normal";
}

unsigned char CLuaCameraDefs::GetCameraDrunkLevel()
{
    return g_pGame->GetPlayerInfo()->GetCamDrunkLevel();
}

bool CLuaCameraDefs::SetCameraMatrix(std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> vecLookAt, std::optional<float> fRoll,
                                     std::optional<float> fFOV)
{
    CVector vecPosition;
    CVector lookAt;
    bool    bLookAtValid = false;

    if (auto* pMatrix = std::get_if<CLuaMatrix*>(&matrixOrPosition))
    {
        vecPosition = (*pMatrix)->GetPosition();
        lookAt = (*pMatrix)->GetRotation();
        bLookAtValid = true;
    }
    else
    {
        vecPosition = std::get<CVector>(matrixOrPosition);
        if (vecLookAt.has_value())
        {
            lookAt = vecLookAt.value();
            bLookAtValid = true;
        }
    }

    float fFOVValue = fFOV.value_or(70.0f);
    if (fFOVValue <= 0.0f || fFOVValue >= 180.0f)
        fFOVValue = 70.0f;

    return CStaticFunctionDefinitions::SetCameraMatrix(vecPosition, bLookAtValid ? &lookAt : nullptr, fRoll.value_or(0.0f), fFOVValue);
}

// Only when onfoot/invehicle
bool CLuaCameraDefs::SetCameraFieldOfView(eFieldOfViewMode eMode, float fFOV, std::optional<bool> instant)
{
    if (fFOV < 0 || fFOV > 179)
        throw std::invalid_argument("Invalid FOV range (0-179)");

    bool bInstant = instant.value_or(false);
    if (eMode == FOV_MODE_PLAYER)
        g_pGame->GetSettings()->SetFieldOfViewPlayer(fFOV, true, bInstant);
    else if (eMode == FOV_MODE_VEHICLE)
        g_pGame->GetSettings()->SetFieldOfViewVehicle(fFOV, true, bInstant);
    else if (eMode == FOV_MODE_VEHICLE_MAX)
        g_pGame->GetSettings()->SetFieldOfViewVehicleMax(fFOV, true, bInstant);
    else
        throw std::invalid_argument(SString("Enum not yet implemented: " + EnumToString(eMode)));

    return true;
}

// Only when onfoot/invehicle
std::variant<float, bool> CLuaCameraDefs::GetCameraFieldOfView(eFieldOfViewMode eMode)
{
    if (eMode == FOV_MODE_PLAYER)
        return g_pGame->GetSettings()->GetFieldOfViewPlayer();
    else if (eMode == FOV_MODE_VEHICLE)
        return g_pGame->GetSettings()->GetFieldOfViewVehicle();
    else if (eMode == FOV_MODE_VEHICLE_MAX)
        return g_pGame->GetSettings()->GetFieldOfViewVehicleMax();

    throw std::invalid_argument(SString("Enum not yet implemented: " + EnumToString(eMode)));
}

bool CLuaCameraDefs::SetCameraTarget(lua_State* luaVM, std::variant<CClientEntity*, CVector> target)
{
    //  bool setCameraTarget ( element target = nil ) or setCameraTarget ( float x, float y, float z )
    if (auto* pTarget = std::get_if<CClientEntity*>(&target))
    {
        if (*pTarget && (*pTarget)->GetType() != CCLIENTPLAYER)
            MinClientReqCheck(luaVM, MIN_CLIENT_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS, "target is not a player");

        return CStaticFunctionDefinitions::SetCameraTarget(*pTarget);
    }

    return CStaticFunctionDefinitions::SetCameraTarget(std::get<CVector>(target));
}

bool CLuaCameraDefs::SetCameraInterior(unsigned char ucInterior)
{
    return CStaticFunctionDefinitions::SetCameraInterior(ucInterior);
}

bool CLuaCameraDefs::FadeCamera(bool bFadeIn, std::optional<float> fFadeTime, std::optional<unsigned char> ucRed, std::optional<unsigned char> ucGreen,
                                std::optional<unsigned char> ucBlue)
{
    return CStaticFunctionDefinitions::FadeCamera(bFadeIn, fFadeTime.value_or(1.0f), ucRed.value_or(0), ucGreen.value_or(0), ucBlue.value_or(0));
}

bool CLuaCameraDefs::SetCameraClip(std::optional<bool> bObjects, std::optional<bool> bVehicles)
{
    m_pManager->GetCamera()->SetCameraClip(bObjects.value_or(true), bVehicles.value_or(true));

    return true;
}

CLuaMultiReturn<bool, bool> CLuaCameraDefs::GetCameraClip()
{
    bool bObjects, bVehicles;
    m_pManager->GetCamera()->GetCameraClip(bObjects, bVehicles);

    return {bObjects, bVehicles};
}

bool CLuaCameraDefs::SetCameraViewMode(std::optional<unsigned char> ucVehicleViewMode, std::optional<unsigned char> ucPedViewMode)
{
    CClientCamera* pCamera = g_pClientGame->GetManager()->GetCamera();

    if (ucVehicleViewMode)
        pCamera->SetCameraVehicleViewMode((eVehicleCamMode)ucVehicleViewMode.value());

    if (ucPedViewMode)
        pCamera->SetCameraPedViewMode((ePedCamMode)ucPedViewMode.value());

    return true;
}

bool CLuaCameraDefs::SetCameraGoggleEffect(std::string strMode, std::optional<bool> bNoiseEnabled)
{
    bool bNoise = bNoiseEnabled.value_or(true);
    if (strMode.compare("nightvision") == 0)
    {
        g_pMultiplayer->SetNightVisionEnabled(true, bNoise);
        g_pMultiplayer->SetThermalVisionEnabled(false, true);
        return true;
    }
    else if (strMode.compare("thermalvision") == 0)
    {
        g_pMultiplayer->SetNightVisionEnabled(false, true);
        g_pMultiplayer->SetThermalVisionEnabled(true, bNoise);
        return true;
    }
    else if (strMode.compare("normal") == 0)
    {
        g_pMultiplayer->SetNightVisionEnabled(false, true);
        g_pMultiplayer->SetThermalVisionEnabled(false, true);
        return true;
    }

    return false;
}

bool CLuaCameraDefs::SetCameraDrunkLevel(short drunkLevel)
{
    if (drunkLevel < 0 || drunkLevel > 255)
        throw std::invalid_argument("Invalid range (0-255)");

    CPlayerInfo* pPlayerInfo = g_pGame->GetPlayerInfo();
    pPlayerInfo->SetCamDrunkLevel(static_cast<unsigned char>(drunkLevel));

    return true;
}

int CLuaCameraDefs::OOP_GetCameraPosition(lua_State* luaVM)
{
    CVector vecPosition;
    m_pManager->GetCamera()->GetPosition(vecPosition);

    lua_pushvector(luaVM, vecPosition);
    return 1;
}

int CLuaCameraDefs::OOP_SetCameraPosition(lua_State* luaVM)
{
    CVector          vecPosition;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecPosition);

    if (!argStream.HasErrors())
    {
        CClientCamera* pCamera = m_pManager->GetCamera();
        if (!pCamera->IsInFixedMode())
        {
            pCamera->ToggleCameraFixedMode(true);
        }

        pCamera->SetPosition(vecPosition);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCameraDefs::OOP_GetCameraRotation(lua_State* luaVM)
{
    CVector vecPosition;
    m_pManager->GetCamera()->GetRotationDegrees(vecPosition);

    lua_pushvector(luaVM, vecPosition);
    return 1;
}

int CLuaCameraDefs::OOP_SetCameraRotation(lua_State* luaVM)
{
    CVector          vecRotation;
    CScriptArgReader argStream(luaVM);
    argStream.ReadVector3D(vecRotation);

    if (!argStream.HasErrors())
    {
        CClientCamera* pCamera = m_pManager->GetCamera();
        if (!pCamera->IsInFixedMode())
        {
            pCamera->ToggleCameraFixedMode(true);
        }

        pCamera->SetRotationDegrees(vecRotation);

        lua_pushboolean(luaVM, true);
        return 1;
    }
    lua_pushboolean(luaVM, false);
    return 1;
}

const SString& CLuaCameraDefs::GetElementType()
{
    return m_pManager->GetCamera()->GetTypeName();
}

bool CLuaCameraDefs::ShakeCamera(float radius, std::optional<float> x, std::optional<float> y, std::optional<float> z) noexcept
{
    if (!x || !y || !z)
    {
        const auto* player = CStaticFunctionDefinitions::GetLocalPlayer();
        CVector     out;
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
