/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Camera.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Chris McArthur <>
*               Derek Abdine <>
*               Christian Myhre Lundheim <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"


int CLuaFunctionDefs::GetCamera ( lua_State* luaVM )
{
    CClientCamera* pCamera = g_pClientGame->GetManager ()->GetCamera ();
    if ( pCamera )
    {
        lua_pushelement ( luaVM, pCamera );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetCameraViewMode ( lua_State* luaVM )
{
    unsigned short ucMode;
    if ( CStaticFunctionDefinitions::GetCameraViewMode ( ucMode ) )
    {
        lua_pushnumber ( luaVM, ucMode );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetCameraMatrix ( lua_State* luaVM )
{
    CVector vecPosition, vecLookAt;
    float fRoll, fFOV;
    if ( CStaticFunctionDefinitions::GetCameraMatrix ( vecPosition, vecLookAt, fRoll, fFOV ) )
    {
        lua_pushnumber ( luaVM, vecPosition.fX );
        lua_pushnumber ( luaVM, vecPosition.fY );
        lua_pushnumber ( luaVM, vecPosition.fZ );
        lua_pushnumber ( luaVM, vecLookAt.fX );
        lua_pushnumber ( luaVM, vecLookAt.fY );
        lua_pushnumber ( luaVM, vecLookAt.fZ );
        lua_pushnumber ( luaVM, fRoll );
        lua_pushnumber ( luaVM, fFOV );
        return 8;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetCameraTarget ( lua_State* luaVM )
{
    CClientEntity* pTarget = CStaticFunctionDefinitions::GetCameraTarget ();
    if ( pTarget )
    {
        lua_pushelement ( luaVM, pTarget );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetCameraInterior ( lua_State* luaVM )
{
    unsigned char ucInterior;
    if ( CStaticFunctionDefinitions::GetCameraInterior ( ucInterior ) )
    {
        lua_pushnumber ( luaVM, ucInterior );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetCameraGoggleEffect ( lua_State *luaVM )
{
    bool bNightVision   = g_pMultiplayer->IsNightVisionEnabled   ( );
    bool bThermalVision = g_pMultiplayer->IsThermalVisionEnabled ( );

    if ( bNightVision )
        lua_pushstring ( luaVM, "nightvision" );
    else if ( bThermalVision )
        lua_pushstring ( luaVM, "thermalvision" );
    else
        lua_pushstring ( luaVM, "normal" );

    return 1;
}


int CLuaFunctionDefs::SetCameraMatrix ( lua_State* luaVM )
{
    CVector vecPosition;
    CVector vecLookAt;
    float fRoll = 0.0f;
    float fFOV = 70.0f;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadVector3D ( vecLookAt, CVector() );
    argStream.ReadNumber ( fRoll, 0.0f );
    argStream.ReadNumber ( fFOV, 70.0f );
    if ( fFOV <= 0.0f || fFOV >= 180.0f )
        fFOV = 70.0f;

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetCameraMatrix ( vecPosition, vecLookAt, fRoll, fFOV ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCameraTarget ( lua_State* luaVM )
{
//  bool setCameraTarget ( element target = nil ) or setCameraTarget ( float x, float y, float z )

    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsUserData () )
    {
        CClientEntity* pTarget;
        argStream.ReadUserData ( pTarget );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetCameraTarget ( pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
    {
        CVector vecTarget;
        argStream.ReadVector3D ( vecTarget );

        if ( !argStream.HasErrors () )
        {
            if ( CStaticFunctionDefinitions::SetCameraTarget ( vecTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }

    if ( argStream.HasErrors () )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCameraInterior ( lua_State* luaVM )
{
    unsigned char ucInterior = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( ucInterior );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::SetCameraInterior ( ucInterior ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::FadeCamera ( lua_State* luaVM )
{
    bool bFadeIn = false;
    unsigned char ucRed = 0;
    unsigned char ucGreen = 0;
    unsigned char ucBlue = 0;
    float fFadeTime = 1.0f;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bFadeIn );
    argStream.ReadNumber ( fFadeTime, 1.0f );
    argStream.ReadNumber ( ucRed, 0 );
    argStream.ReadNumber ( ucGreen, 0 );
    argStream.ReadNumber ( ucBlue, 0 );

    if ( !argStream.HasErrors ( ) )
    {
        if ( CStaticFunctionDefinitions::FadeCamera ( bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCameraClip ( lua_State* luaVM )
{
    bool bObjects = true;
    bool bVehicles = true;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadBool ( bObjects, true );
    argStream.ReadBool ( bVehicles, true );

    m_pManager->GetCamera ()->SetCameraClip ( bObjects, bVehicles );

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::SetCameraViewMode ( lua_State* luaVM )
{
    unsigned short usViewMode = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( usViewMode );

    if ( !argStream.HasErrors ( ) )
    {
        CStaticFunctionDefinitions::SetCameraViewMode ( usViewMode );

        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SetCameraGoggleEffect ( lua_State *luaVM )
{
    SString strMode = "";
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strMode );

    if ( !argStream.HasErrors ( ) )
    {
        bool bSuccess = false;

        if ( strMode.compare ( "nightvision" ) == 0 )
        {
            g_pMultiplayer->SetNightVisionEnabled   ( true );
            g_pMultiplayer->SetThermalVisionEnabled ( false );
            
            bSuccess = true;
        }
        else if ( strMode.compare ( "thermalvision" ) == 0 )
        {
            g_pMultiplayer->SetNightVisionEnabled   ( false );
            g_pMultiplayer->SetThermalVisionEnabled ( true );

            bSuccess = true;
        }
        else if ( strMode.compare ( "normal" ) == 0 )
        {
            g_pMultiplayer->SetNightVisionEnabled   ( false );
            g_pMultiplayer->SetThermalVisionEnabled ( false );

            bSuccess = true;
        }

        if ( bSuccess )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
