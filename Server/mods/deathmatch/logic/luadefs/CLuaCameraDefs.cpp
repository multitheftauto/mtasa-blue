/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/luadefs/CLuaCameraDefs.cpp
 *  PURPOSE:     Lua camera function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaCameraDefs.h"
#include "CStaticFunctionDefinitions.h"
#include "CScriptArgReader.h"

#define MIN_SERVER_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS "1.5.8-9.20979"

void CLuaCameraDefs::LoadFunctions()
{
    constexpr static const std::pair<const char*, lua_CFunction> functions[]{
        // Get functions
        {"getCameraMatrix", ArgumentParserWarn<false, GetCameraMatrix>},
        {"getCameraTarget", ArgumentParserWarn<false, GetCameraTarget>},
        {"getCameraInterior", ArgumentParserWarn<false, GetCameraInterior>},

        // Set functions
        {"setCameraMatrix", ArgumentParserWarn<false, SetCameraMatrix>},
        {"setCameraTarget", ArgumentParserWarn<false, SetCameraTarget>},
        {"setCameraInterior", ArgumentParserWarn<false, SetCameraInterior>},
        {"fadeCamera", ArgumentParserWarn<false, FadeCamera>},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

std::variant<CLuaMultiReturn<float, float, float, float, float, float, float, float>, bool> CLuaCameraDefs::GetCameraMatrix(CPlayer* pPlayer)
{
    //  float cameraX, float cameraY, float cameraZ, float targetX, float targetY, float targetZ, float roll, float fov getCameraMatrix ( player thePlayer )
    CVector vecPosition, vecLookAt;
    float   fRoll, fFOV;
    if (CStaticFunctionDefinitions::GetCameraMatrix(pPlayer, vecPosition, vecLookAt, fRoll, fFOV))
        return CLuaMultiReturn<float, float, float, float, float, float, float, float>{vecPosition.fX, vecPosition.fY, vecPosition.fZ, vecLookAt.fX,
                                                                                       vecLookAt.fY,   vecLookAt.fZ,   fRoll,          fFOV};

    return false;
}

int CLuaCameraDefs::OOP_getCameraMatrix(lua_State* luaVM)
{
    // Needs further attention before adding
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);

    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        CMatrix matrix;
        // pPlayer->GetCamera ()->GetMatrix ( matrix );

        lua_pushmatrix(luaVM, matrix);
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

std::variant<CElement*, bool> CLuaCameraDefs::GetCameraTarget(CPlayer* pPlayer)
{
    //  element getCameraTarget ( player thePlayer )
    CElement* pTarget = CStaticFunctionDefinitions::GetCameraTarget(pPlayer);
    if (pTarget)
        return pTarget;

    return false;
}

std::variant<unsigned char, bool> CLuaCameraDefs::GetCameraInterior(CPlayer* pPlayer)
{
    //  int getCameraInterior ( player thePlayer )
    unsigned char ucInterior;
    if (CStaticFunctionDefinitions::GetCameraInterior(pPlayer, ucInterior))
        return ucInterior;

    return false;
}

bool CLuaCameraDefs::SetCameraMatrix(CElement* pPlayer, std::variant<CLuaMatrix*, CVector> matrixOrPosition, std::optional<CVector> vecLookAt,
                                     std::optional<float> fRoll, std::optional<float> fFOV)
{
    //  bool setCameraMatrix ( player thePlayer, float positionX, float positionY, float positionZ [, float lookAtX, float lookAtY, float lookAtZ, float roll =
    //  0, float fov = 70 ] )
    CVector vecPosition;
    CVector lookAt;

    if (auto* pMatrix = std::get_if<CLuaMatrix*>(&matrixOrPosition))
    {
        vecPosition = (*pMatrix)->GetPosition();
        lookAt = (*pMatrix)->GetRotation();
    }
    else
    {
        vecPosition = std::get<CVector>(matrixOrPosition);
        lookAt = vecLookAt.value_or(CVector());
    }

    float fFOVValue = fFOV.value_or(70.0f);
    if (fFOVValue <= 0.0f || fFOVValue >= 180.0f)
        fFOVValue = 70.0f;

    return CStaticFunctionDefinitions::SetCameraMatrix(pPlayer, vecPosition, &lookAt, fRoll.value_or(0.0f), fFOVValue);
}

bool CLuaCameraDefs::SetCameraTarget(lua_State* luaVM, CElement* pPlayer, std::optional<CElement*> pTarget)
{
    //  bool setCameraTarget ( player thePlayer [, element target = nil ] )
    if (pTarget.has_value() && pTarget.value() && pTarget.value()->GetType() != CElement::PLAYER)
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager()->GetVirtualMachine(luaVM);
        if (pLuaMain && pLuaMain->GetResource() && pLuaMain->GetResource()->GetMinServerRequirement() < MIN_SERVER_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS)
        {
#if MTASA_VERSION_TYPE >= VERSION_TYPE_UNTESTED
            throw std::invalid_argument(SString("<min_mta_version> section in the meta.xml is incorrect or missing (expected at least server %s because %s)",
                                                MIN_SERVER_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS, "target is not a player"));
#endif
        }
    }

    return CStaticFunctionDefinitions::SetCameraTarget(pPlayer, pTarget.value_or(nullptr));
}

bool CLuaCameraDefs::SetCameraInterior(CElement* pElement, unsigned char ucInterior)
{
    //  bool setCameraInterior ( player thePlayer, int interior )
    return CStaticFunctionDefinitions::SetCameraInterior(pElement, ucInterior);
}

bool CLuaCameraDefs::FadeCamera(CElement* pPlayer, bool bFadeIn, std::optional<float> fFadeTime, std::optional<unsigned char> ucRed,
                                std::optional<unsigned char> ucGreen, std::optional<unsigned char> ucBlue)
{
    //  bool fadeCamera ( player thePlayer, bool fadeIn, [ float timeToFade = 1.0, int red = 0, int green = 0, int blue = 0 ] )
    return CStaticFunctionDefinitions::FadeCamera(pPlayer, bFadeIn, fFadeTime.value_or(1.0f), ucRed.value_or(0), ucGreen.value_or(0), ucBlue.value_or(0));
}
