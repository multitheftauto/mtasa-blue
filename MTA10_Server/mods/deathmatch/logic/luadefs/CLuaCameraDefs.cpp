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
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            CVector vecPosition, vecLookAt;
            if ( CStaticFunctionDefinitions::GetCameraMatrix ( pPlayer, vecPosition, vecLookAt ) )
            {
                lua_pushnumber ( luaVM, vecPosition.fX );
                lua_pushnumber ( luaVM, vecPosition.fY );
                lua_pushnumber ( luaVM, vecPosition.fZ );
                lua_pushnumber ( luaVM, vecLookAt.fX );
                lua_pushnumber ( luaVM, vecLookAt.fY );
                lua_pushnumber ( luaVM, vecLookAt.fZ );
                return 6;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getCameraMatrix", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getCameraMatrix" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::getCameraTarget ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            CElement* pTarget = CStaticFunctionDefinitions::GetCameraTarget ( pPlayer );
            if ( pTarget )
            {
                lua_pushelement ( luaVM, pTarget );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getCameraTarget", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getCameraTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::getCameraInterior ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CPlayer* pPlayer = lua_toplayer ( luaVM, 1 );
        if ( pPlayer )
        {
            unsigned char ucInterior;
            if ( CStaticFunctionDefinitions::GetCameraInterior ( pPlayer, ucInterior ) )
            {
                lua_pushnumber ( luaVM, ucInterior );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "getCameraInterior", "player", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "getCameraInterior" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::setCameraMatrix ( lua_State* luaVM )
{
    // Verify the parameter types
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );

    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
         ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
         ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) )
    {
        // Grab the parameters
        CVector vecPosition;
        CElement* pElement = lua_toelement ( luaVM, 1 );
        vecPosition.fX = static_cast < float > ( lua_tonumber ( luaVM, 2 ) );
        vecPosition.fY = static_cast < float > ( lua_tonumber ( luaVM, 3 ) );
        vecPosition.fZ = static_cast < float > ( lua_tonumber ( luaVM, 4 ) );
        CVector vecLookAt;
        int iArgument5 = lua_type ( luaVM, 5 );
        int iArgument6 = lua_type ( luaVM, 6 );
        int iArgument7 = lua_type ( luaVM, 7 );
        if ( ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) &&
             ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING ) &&
             ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING ) )
        {
            vecLookAt.fX = static_cast < float > ( lua_tonumber ( luaVM, 5 ) );
            vecLookAt.fY = static_cast < float > ( lua_tonumber ( luaVM, 6 ) );
            vecLookAt.fZ = static_cast < float > ( lua_tonumber ( luaVM, 7 ) );
        }


        // Verify the element
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetCameraMatrix ( pElement, vecPosition, vecLookAt ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraMatrix" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::setCameraTarget ( lua_State* luaVM )
{
    // Verify the argument 1 type
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( iArgument1 == LUA_TLIGHTUSERDATA )
    {
        // Grab the first argument
        CElement* pElement = lua_toelement ( luaVM, 1 );
        CElement* pTarget = NULL;

        // Eventually grab the target element from argument 2 and make sure it has valid data
        if ( iArgument2 == LUA_TLIGHTUSERDATA )
        {
            pTarget = lua_toelement ( luaVM, 2 );
            if ( !pTarget )
                m_pScriptDebugging->LogBadPointer ( luaVM, "setCameraTarget", "target", 2 );
        }

        // Verify the elements
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetCameraTarget ( pElement, pTarget ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "setCameraTarget", "element", 1 );
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraTarget" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::setCameraInterior ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA &&
         ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CElement* pElement = lua_toelement ( luaVM, 1 );
        unsigned char ucInterior = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

        // Verify the element
        if ( pElement )
        {
            if ( CStaticFunctionDefinitions::SetCameraInterior ( pElement, ucInterior ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM, "setCameraInterior" );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaCameraDefs::fadeCamera ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 ); // element
    int iArgument2 = lua_type ( luaVM, 2 ); // fadeIn
    int iArgument3 = lua_type ( luaVM, 3 ); // fadeTime
    int iArgument4 = lua_type ( luaVM, 4 ); // red
    int iArgument5 = lua_type ( luaVM, 5 ); // green
    int iArgument6 = lua_type ( luaVM, 6 ); // blue

    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) && ( iArgument2 == LUA_TBOOLEAN ) )
    {
        CElement * pElement = lua_toelement ( luaVM, 1 );
        bool bFadeIn = lua_toboolean (luaVM, 2 ) ? true:false;

        if ( pElement )
        {
            unsigned char ucRed = 0;
            unsigned char ucGreen = 0;
            unsigned char ucBlue = 0;
            float fFadeTime = 1.0f;

            if ( iArgument3 == LUA_TSTRING || iArgument3 == LUA_TNUMBER )
            {
                fFadeTime = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );

                if ( iArgument4 == LUA_TSTRING || iArgument4 == LUA_TNUMBER )
                {
                    ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

                    if ( iArgument5 == LUA_TSTRING || iArgument5 == LUA_TNUMBER )
                    {
                        ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

                        if ( iArgument6 == LUA_TSTRING || iArgument6 == LUA_TNUMBER )
                        {
                            ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );
                        }
                    }
                }
            }

            if ( CStaticFunctionDefinitions::FadeCamera ( pElement, bFadeIn, fFadeTime, ucRed, ucGreen, ucBlue ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
        {
            m_pScriptDebugging->LogBadPointer ( luaVM, "fadeCamera", "player", 1 );
        }
    }
    else
    {
        m_pScriptDebugging->LogBadType ( luaVM, "fadeCamera" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}