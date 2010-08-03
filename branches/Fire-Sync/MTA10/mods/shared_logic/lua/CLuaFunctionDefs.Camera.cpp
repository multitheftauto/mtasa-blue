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

int CLuaFunctionDefs::GetCameraView ( lua_State* luaVM )
{
    unsigned short ucMode;
    if ( CStaticFunctionDefinitions::GetCameraView ( ucMode ) )
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


int CLuaFunctionDefs::SetCameraMatrix ( lua_State* luaVM )
{
    // Verify the parameter types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the parameters
        CVector vecPosition;
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 1 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );

        CVector vecLookAt;
        CVector * pvecLookAt = NULL;
        float fRoll = 0.0f;
        float fFOV = 70.0f;
        int iArgument4 = lua_type ( luaVM, 4 );
        int iArgument5 = lua_type ( luaVM, 5 );
        int iArgument6 = lua_type ( luaVM, 6 );
        if ( ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
             ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
             ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) )
        {
            vecLookAt.fX = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
            vecLookAt.fY = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
            vecLookAt.fZ = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
            pvecLookAt = &vecLookAt;

            int iArgument7 = lua_type ( luaVM, 7 );
            
            if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
            {
                fRoll = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
                int iArgument8 = lua_type ( luaVM, 8 );
                if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                {
                    fFOV = static_cast < float > ( lua_tonumber ( luaVM, 8 ) );
                    if ( fFOV <= 0.0f || fFOV >= 180.0f )
                        fFOV = 70.0f;
                }
            }
        }

        if ( CStaticFunctionDefinitions::SetCameraMatrix ( vecPosition, pvecLookAt, fRoll, fFOV ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraMatrix" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCameraTarget ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the first argument
        CClientEntity * pTarget = lua_toelement ( luaVM, 1 );
        if ( pTarget )    
        {
            if ( CStaticFunctionDefinitions::SetCameraTarget ( pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setCameraTarget", "target", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCameraInterior ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    if ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING )
    {
        unsigned char ucInterior = static_cast < unsigned char > ( lua_tonumber ( luaVM, 1 ) );

        if ( CStaticFunctionDefinitions::SetCameraInterior ( ucInterior ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraInterior" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::FadeCamera ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 ); // fadeIn
    int iArgument2 = lua_type ( luaVM, 2 ); // fadeTime
    int iArgument3 = lua_type ( luaVM, 3 ); // red
    int iArgument4 = lua_type ( luaVM, 4 ); // green
    int iArgument5 = lua_type ( luaVM, 5 ); // blue

    if ( iArgument1 == LUA_TBOOLEAN )
    {
        bool bFadeIn = lua_toboolean (luaVM, 1 ) ? true:false;
        unsigned char ucRed = 0;
        unsigned char ucGreen = 0;
        unsigned char ucBlue = 0;
        float fFadeTime = 1.0f;

        if ( iArgument2 == LUA_TSTRING || iArgument2 == LUA_TNUMBER )
        {
            fFadeTime = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

            if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
            {
                ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );

                if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
                {
                    ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

                    if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
                    {
                        ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );
                    }
                }
            }
        }

        if ( CStaticFunctionDefinitions::FadeCamera ( bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "fadeCamera" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetCameraClip ( lua_State* luaVM )
{
    bool bObjects   = lua_type ( luaVM, 1 ) != LUA_TBOOLEAN ? true : lua_toboolean ( luaVM, 1 ) ? true : false;
    bool bVehicles  = lua_type ( luaVM, 2 ) != LUA_TBOOLEAN ? true : lua_toboolean ( luaVM, 2 ) ? true : false;

    m_pManager->GetCamera ()->SetCameraClip ( bObjects, bVehicles );

    lua_pushboolean ( luaVM, true );
    return 1;
}

int CLuaFunctionDefs::SetCameraView ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TNUMBER )
    {
        CStaticFunctionDefinitions::SetCameraView ( static_cast <unsigned short> ( lua_tonumber ( luaVM, 1 ) ) );

        lua_pushboolean ( luaVM, true );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}
