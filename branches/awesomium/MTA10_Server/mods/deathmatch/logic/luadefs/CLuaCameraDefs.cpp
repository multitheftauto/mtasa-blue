/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaCameraDefs.cpp
*  PURPOSE:     Lua camera function definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaCameraDefs::LoadFunctions ( void )
{
    // Get functions
    CLuaCFunctions::AddFunction ( "getCameraMatrix", CLuaCameraDefs::getCameraMatrix );
    CLuaCFunctions::AddFunction ( "getCameraTarget", CLuaCameraDefs::getCameraTarget );
    CLuaCFunctions::AddFunction ( "getCameraInterior", CLuaCameraDefs::getCameraInterior );

    // Set functions
    CLuaCFunctions::AddFunction ( "setCameraMatrix", CLuaCameraDefs::setCameraMatrix );
    CLuaCFunctions::AddFunction ( "setCameraTarget", CLuaCameraDefs::setCameraTarget );
    CLuaCFunctions::AddFunction ( "setCameraInterior", CLuaCameraDefs::setCameraInterior );
    CLuaCFunctions::AddFunction ( "fadeCamera", CLuaCameraDefs::fadeCamera );
}


int CLuaCameraDefs::getCameraMatrix ( lua_State* luaVM )
{
//  float cameraX, float cameraY, float cameraZ, float targetX, float targetY, float targetZ, float roll, float fov getCameraMatrix ( player thePlayer )
    CPlayer* pPlayer;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );

    if ( !argStream.HasErrors () )
    {
        CVector vecPosition, vecLookAt;
        float fRoll, fFOV;
        if ( CStaticFunctionDefinitions::GetCameraMatrix ( pPlayer, vecPosition, vecLookAt, fRoll, fFOV ) )
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
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::getCameraTarget ( lua_State* luaVM )
{
//  element getCameraTarget ( player thePlayer )
    CPlayer* pPlayer;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    
    if ( !argStream.HasErrors () )
    {
        CElement* pTarget = CStaticFunctionDefinitions::GetCameraTarget ( pPlayer );
        if ( pTarget )
        {
            lua_pushelement ( luaVM, pTarget );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::getCameraInterior ( lua_State* luaVM )
{
//  int getCameraInterior ( player thePlayer )
    CPlayer* pPlayer;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    
    if ( !argStream.HasErrors () )
    {
        unsigned char ucInterior;
        if ( CStaticFunctionDefinitions::GetCameraInterior ( pPlayer, ucInterior ) )
        {
            lua_pushnumber ( luaVM, ucInterior );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::setCameraMatrix ( lua_State* luaVM )
{
//  bool setCameraMatrix ( player thePlayer, float positionX, float positionY, float positionZ [, float lookAtX, float lookAtY, float lookAtZ, float roll = 0, float fov = 70 ] )
    CElement* pPlayer; CVector vecPosition; CVector vecLookAt; CVector* pvecLookAt = NULL; float fRoll; float fFOV;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadVector3D ( vecPosition );
    if ( argStream.NextIsVector3D ( ) )
    {
        argStream.ReadVector3D(vecLookAt);
        pvecLookAt = &vecLookAt;
    }
    else
        argStream.m_iIndex += 3;

    argStream.ReadNumber ( fRoll, 0.0f );
    argStream.ReadNumber ( fFOV, 70.0f );

    if ( !argStream.HasErrors () )
    {
        if ( fFOV <= 0.0f || fFOV >= 180.0f )
            fFOV = 70.0f;

        if ( CStaticFunctionDefinitions::SetCameraMatrix ( pPlayer, vecPosition, pvecLookAt, fRoll, fFOV ) )
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


int CLuaCameraDefs::setCameraTarget ( lua_State* luaVM )
{
//  bool setCameraTarget ( player thePlayer [, element target = nil ] )
    CElement* pPlayer; CElement* pTarget;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadUserData ( pTarget, NULL );
    
    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetCameraTarget ( pPlayer, pTarget ) )
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


int CLuaCameraDefs::setCameraInterior ( lua_State* luaVM )
{
//  bool setCameraInterior ( player thePlayer, int interior )
    CElement* pElement; unsigned char ucInterior;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucInterior );
    
    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetCameraInterior ( pElement, ucInterior ) )
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


int CLuaCameraDefs::fadeCamera ( lua_State* luaVM )
{
//  bool fadeCamera ( player thePlayer, bool fadeIn, [ float timeToFade = 1.0, int red = 0, int green = 0, int blue = 0 ] )
    CElement* pPlayer; bool bFadeIn; float fFadeTime; unsigned char ucRed; unsigned char ucGreen; unsigned char ucBlue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadBool ( bFadeIn );
    argStream.ReadNumber ( fFadeTime, 1.0f );
    argStream.ReadNumber ( ucRed, 0 );
    argStream.ReadNumber ( ucGreen, 0 );
    argStream.ReadNumber ( ucBlue, 0 );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::FadeCamera ( pPlayer, bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue ) )
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
