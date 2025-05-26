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
        {"getCameraMatrix", getCameraMatrix},
        {"getCameraTarget", getCameraTarget},
        {"getCameraInterior", getCameraInterior},

        // Set functions
        {"setCameraMatrix", setCameraMatrix},
        {"setCameraTarget", setCameraTarget},
        {"setCameraInterior", setCameraInterior},
        {"fadeCamera", fadeCamera},
    };

    // Add functions
    for (const auto& [name, func] : functions)
        CLuaCFunctions::AddFunction(name, func);
}

int CLuaCameraDefs::getCameraMatrix(lua_State* luaVM)
{
    //  float cameraX, float cameraY, float cameraZ, float targetX, float targetY, float targetZ, float roll, float fov getCameraMatrix ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        CVector vecPosition, vecLookAt;
        float   fRoll, fFOV;
        if (CStaticFunctionDefinitions::GetCameraMatrix(pPlayer, vecPosition, vecLookAt, fRoll, fFOV))
        {
            lua_pushnumber(luaVM, vecPosition.fX);
            lua_pushnumber(luaVM, vecPosition.fY);
            lua_pushnumber(luaVM, vecPosition.fZ);
            lua_pushnumber(luaVM, vecLookAt.fX);
            lua_pushnumber(luaVM, vecLookAt.fY);
            lua_pushnumber(luaVM, vecLookAt.fZ);
            lua_pushnumber(luaVM, fRoll);
            lua_pushnumber(luaVM, fFOV);
            return 8;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
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

int CLuaCameraDefs::getCameraTarget(lua_State* luaVM)
{
    //  element getCameraTarget ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        CElement* pTarget = CStaticFunctionDefinitions::GetCameraTarget(pPlayer);
        if (pTarget)
        {
            lua_pushelement(luaVM, pTarget);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCameraDefs::getCameraInterior(lua_State* luaVM)
{
    //  int getCameraInterior ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (!argStream.HasErrors())
    {
        unsigned char ucInterior;
        if (CStaticFunctionDefinitions::GetCameraInterior(pPlayer, ucInterior))
        {
            lua_pushnumber(luaVM, ucInterior);
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaCameraDefs::setCameraMatrix(lua_State* luaVM)
{
    //  bool setCameraMatrix ( player thePlayer, float positionX, float positionY, float positionZ [, float lookAtX, float lookAtY, float lookAtZ, float roll =
    //  0, float fov = 70 ] )
    CElement* pPlayer;
    CVector   vecPosition;
    CVector   vecLookAt;
    float     fRoll;
    float     fFOV;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);

    if (argStream.NextIsUserDataOfType<CLuaMatrix>())
    {
        CLuaMatrix* pMatrix;
        argStream.ReadUserData(pMatrix);

        vecPosition = pMatrix->GetPosition();
        vecLookAt = pMatrix->GetRotation();
    }
    else
    {
        argStream.ReadVector3D(vecPosition);
        argStream.ReadVector3D(vecLookAt, CVector());
    }

    argStream.ReadNumber(fRoll, 0.0f);
    argStream.ReadNumber(fFOV, 70.0f);

    if (!argStream.HasErrors())
    {
        if (fFOV <= 0.0f || fFOV >= 180.0f)
            fFOV = 70.0f;

        if (CStaticFunctionDefinitions::SetCameraMatrix(pPlayer, vecPosition, &vecLookAt, fRoll, fFOV))
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

int CLuaCameraDefs::setCameraTarget(lua_State* luaVM)
{
    //  bool setCameraTarget ( player thePlayer [, element target = nil ] )
    CElement* pPlayer;
    CElement* pTarget;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadUserData(pTarget, NULL);

    if (pTarget && pTarget->GetType() != CElement::PLAYER)
        MinServerReqCheck(argStream, MIN_SERVER_REQ_SETCAMERATARGET_USE_ANY_ELEMENTS, "target is not a player");

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetCameraTarget(pPlayer, pTarget))
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

int CLuaCameraDefs::setCameraInterior(lua_State* luaVM)
{
    //  bool setCameraInterior ( player thePlayer, int interior )
    CElement*     pElement;
    unsigned char ucInterior;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pElement);
    argStream.ReadNumber(ucInterior);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::SetCameraInterior(pElement, ucInterior))
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

int CLuaCameraDefs::fadeCamera(lua_State* luaVM)
{
    //  bool fadeCamera ( player thePlayer, bool fadeIn, [ float timeToFade = 1.0, int red = 0, int green = 0, int blue = 0 ] )
    CElement*     pPlayer;
    bool          bFadeIn;
    float         fFadeTime;
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;

    CScriptArgReader argStream(luaVM);
    argStream.ReadUserData(pPlayer);
    argStream.ReadBool(bFadeIn);
    argStream.ReadNumber(fFadeTime, 1.0f);
    argStream.ReadNumber(ucRed, 0);
    argStream.ReadNumber(ucGreen, 0);
    argStream.ReadNumber(ucBlue, 0);

    if (!argStream.HasErrors())
    {
        if (CStaticFunctionDefinitions::FadeCamera(pPlayer, bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue))
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
