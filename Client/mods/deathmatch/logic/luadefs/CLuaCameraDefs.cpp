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
        {"getCameraFieldOfView", GetCameraFieldOfView},
        {"getCameraDrunkLevel", ArgumentParserWarn<false, GetCameraDrunkLevel>},

        // Cam set funcs
        {"setCameraMatrix", SetCameraMatrix},
        {"setCameraFieldOfView", SetCameraFieldOfView},
        {"setCameraTarget", SetCameraTarget},
        {"setCameraInterior", SetCameraInterior},
        {"fadeCamera", FadeCamera},
        {"setCameraClip", SetCameraClip},
        {"getCameraClip", GetCameraClip},
        {"setCameraViewMode", ArgumentParserWarn<false, SetCameraViewMode>},
        {"setCameraGoggleEffect", SetCameraGoggleEffect},
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

int CLuaCameraDefs::SetCameraMatrix(lua_State* luaVM)
{
    CVector          vecPosition;
    CVector          vecLookAt;
    float            fRoll = 0.0f;
    float            fFOV = 70.0f;
    CScriptArgReader argStream(luaVM);
    bool             bLookAtValid;

    if (argStream.NextIsUserDataOfType<CLuaMatrix>())
    {
        CLuaMatrix* pMatrix;
        argStream.ReadUserData(pMatrix);

        vecPosition = pMatrix->GetPosition();
        vecLookAt = pMatrix->GetRotation();
        bLookAtValid = true;
    }
    else
    {
        argStream.ReadVector3D(vecPosition);
        bLookAtValid = argStream.NextIsVector3D();
        argStream.ReadVector3D(vecLookAt, CVector());
    }

    argStream.ReadNumber(fRoll, 0.0f);
    argStream.ReadNumber(fFOV, 70.0f);
    if (fFOV <= 0.0f || fFOV >= 180.0f)
        fFOV = 70.0f;

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetCameraMatrix(vecPosition, bLookAtValid ? &vecLookAt : nullptr, fRoll, fFOV))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

// Only when onfoot/invehicle
int CLuaCameraDefs::SetCameraFieldOfView(lua_State* luaVM)
{
    float            fFOV;
    eFieldOfViewMode eMode;
    CScriptArgReader argStream(luaVM);

    argStream.ReadEnumString(eMode);
    argStream.ReadNumber(fFOV);

    if (!argStream.HasErrors())
    {
        while (true)
        {
            if (fFOV < 0 || fFOV > 179)
            {
                argStream.SetCustomError("Invalid FOV range (0-179)");
                break;
            }

            if (eMode == FOV_MODE_PLAYER)
                g_pGame->GetSettings()->SetFieldOfViewPlayer(fFOV, true);
            else if (eMode == FOV_MODE_VEHICLE)
                g_pGame->GetSettings()->SetFieldOfViewVehicle(fFOV, true);
            else if (eMode == FOV_MODE_VEHICLE_MAX)
                g_pGame->GetSettings()->SetFieldOfViewVehicleMax(fFOV, true);
            else
            {
                argStream.m_iIndex = 1;
                argStream.SetCustomError(SString("Enum not yet implemented: " + EnumToString(eMode)));
                break;
            }

            lua_pushboolean(luaVM, true);
            return 1;
        }
    }

    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    lua_pushboolean(luaVM, false);
    return 1;
}

// Only when onfoot/invehicle
int CLuaCameraDefs::GetCameraFieldOfView(lua_State* luaVM)
{
    eFieldOfViewMode eMode;
    CScriptArgReader argStream(luaVM);

    argStream.ReadEnumString(eMode);

    if (!argStream.HasErrors())
    {
        float fFOV;
        if (eMode == FOV_MODE_PLAYER)
            fFOV = g_pGame->GetSettings()->GetFieldOfViewPlayer();
        else if (eMode == FOV_MODE_VEHICLE)
            fFOV = g_pGame->GetSettings()->GetFieldOfViewVehicle();
        else if (eMode == FOV_MODE_VEHICLE_MAX)
            fFOV = g_pGame->GetSettings()->GetFieldOfViewVehicleMax();
        else
        {
            argStream.m_iIndex = 1;
            m_pScriptDebugging->LogCustom(luaVM, SString("Enum not yet implemented: " + EnumToString(eMode)));
            lua_pushboolean(luaVM, false);
            return 1;
        }

        lua_pushnumber(luaVM, fFOV);
        return 1;
    }

    m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCameraDefs::SetCameraTarget(lua_State* luaVM)
{
    //  bool setCameraTarget ( element target = nil ) or setCameraTarget ( float x, float y, float z )

    CScriptArgReader argStream(luaVM);
    if (argStream.NextIsUserDataOfType<CClientEntity>())
    {
        CClientEntity* pTarget;
        argStream.ReadUserData(pTarget);

        if (pTarget->GetType() != CCLIENTPLAYER)
            MinClientReqCheck(argStream, MIN_CLIENT_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS, "target is not a player");

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetCameraTarget(pTarget))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
    {
        CVector vecTarget;
        argStream.ReadVector3D(vecTarget);

        if (!argStream.HasErrors())
        {
            if (CStaticFunctionDefinitions::SetCameraTarget(vecTarget))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }

    if (argStream.HasErrors())
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCameraDefs::SetCameraInterior(lua_State* luaVM)
{
    unsigned char    ucInterior = 0;
    CScriptArgReader argStream(luaVM);
    argStream.ReadNumber(ucInterior);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetCameraInterior(ucInterior))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCameraDefs::FadeCamera(lua_State* luaVM)
{
    bool          bFadeIn = false;
    unsigned char ucRed = 0;
    unsigned char ucGreen = 0;
    unsigned char ucBlue = 0;
    float         fFadeTime = 1.0f;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bFadeIn);
    argStream.ReadNumber(fFadeTime, 1.0f);
    argStream.ReadNumber(ucRed, 0);
    argStream.ReadNumber(ucGreen, 0);
    argStream.ReadNumber(ucBlue, 0);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FadeCamera(bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue))
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCameraDefs::SetCameraClip(lua_State* luaVM)
{
    bool bObjects = true;
    bool bVehicles = true;

    CScriptArgReader argStream(luaVM);
    argStream.ReadBool(bObjects, true);
    argStream.ReadBool(bVehicles, true);

    m_pManager->GetCamera()->SetCameraClip(bObjects, bVehicles);

    lua_pushboolean(luaVM, true);
    return 1;
}

int CLuaCameraDefs::GetCameraClip(lua_State* luaVM)
{
    bool bObjects, bVehicles;
    m_pManager->GetCamera()->GetCameraClip(bObjects, bVehicles);

    lua_pushboolean(luaVM, bObjects);
    lua_pushboolean(luaVM, bVehicles);
    return 2;
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

int CLuaCameraDefs::SetCameraGoggleEffect(lua_State* luaVM)
{
    SString          strMode;
    bool             bNoiseEnabled;
    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strMode);
    argStream.ReadBool(bNoiseEnabled, true);

    if (!argStream.HasErrors())
    {
        bool bSuccess = false;

        if (strMode.compare("nightvision") == 0)
        {
            g_pMultiplayer->SetNightVisionEnabled(true, bNoiseEnabled);
            g_pMultiplayer->SetThermalVisionEnabled(false, true);

            bSuccess = true;
        }
        else if (strMode.compare("thermalvision") == 0)
        {
            g_pMultiplayer->SetNightVisionEnabled(false, true);
            g_pMultiplayer->SetThermalVisionEnabled(true, bNoiseEnabled);

            bSuccess = true;
        }
        else if (strMode.compare("normal") == 0)
        {
            g_pMultiplayer->SetNightVisionEnabled(false, true);
            g_pMultiplayer->SetThermalVisionEnabled(false, true);

            bSuccess = true;
        }

        if (bSuccess)
        {
            lua_pushboolean(luaVM, true);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

bool CLuaCameraDefs::SetCameraDrunkLevel(short drunkLevel)
{
    if (drunkLevel < 0 || drunkLevel > 255)
        throw std::invalid_argument("Invalid range (0-255)");

    CPlayerInfo* pPlayerInfo = g_pGame->GetPlayerInfo();
    pPlayerInfo->SetCamDrunkLevel(drunkLevel);

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