/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Blip.cpp
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

int CLuaFunctionDefs::CreateBlip ( lua_State* luaVM )
{
    CVector vecPosition;
    unsigned char ucIcon = 0;
    unsigned char ucSize = 2;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering = 0;
    unsigned short usVisibleDistance = 16383;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( ucIcon, 0 );
    argStream.ReadNumber( ucSize, 2 );
    argStream.ReadNumber( color.R, 255 );
    argStream.ReadNumber( color.G, 0 );
    argStream.ReadNumber( color.B, 0 );
    argStream.ReadNumber( color.A, 255 );
    argStream.ReadNumber( sOrdering, 0 );
    argStream.ReadNumber( usVisibleDistance, 16383 );

    if ( !argStream.HasErrors ( ) )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create the blip
                CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlip ( *pResource, vecPosition, ucIcon, ucSize, color, sOrdering, usVisibleDistance );
                if ( pMarker )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup();
                    if ( pGroup )
                    {
                        pGroup->Add ( pMarker );
                    }

                    lua_pushelement ( luaVM, pMarker );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateBlipAttachedTo ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    // Default colors and size
    unsigned char ucIcon = 0;
    unsigned char ucSize = 2;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering = 0;
    unsigned short usVisibleDistance = 16383;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucIcon, 0 );
    argStream.ReadNumber( ucSize, 2 );
    argStream.ReadNumber( color.R, 255 );
    argStream.ReadNumber( color.G, 0 );
    argStream.ReadNumber( color.B, 0 );
    argStream.ReadNumber( color.A, 255 );
    argStream.ReadNumber( sOrdering, 0 );
    argStream.ReadNumber( usVisibleDistance, 16383 );

    // Element in place?
    if ( !argStream.HasErrors( ) )
    {
        // Grab the element and verify it
        if ( pEntity )
        {
            CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
            if ( pLuaMain )
            {
                CResource* pResource = pLuaMain->GetResource ();
                if ( pResource )
                {
                    // Create the blip
                    CClientRadarMarker* pMarker = CStaticFunctionDefinitions::CreateBlipAttachedTo ( *pResource, *pEntity, ucIcon, ucSize, color, sOrdering, usVisibleDistance );
                    if ( pMarker )
                    {
                        CElementGroup * pGroup = pResource->GetElementGroup();
                        if ( pGroup )
                        {
                            pGroup->Add ( pMarker );
                        }
                        lua_pushelement ( luaVM, pMarker );
                        return 1;
                    }
                }
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipIcon ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            unsigned char ucIcon = static_cast < unsigned char > ( pMarker->GetSprite () );
            lua_pushnumber ( luaVM, ucIcon );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipSize ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            unsigned char ucSize = static_cast < unsigned char > ( pMarker->GetScale () );
            lua_pushnumber ( luaVM, ucSize );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipColor ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            SColor color = pMarker->GetColor ();
            lua_pushnumber ( luaVM, color.R );
            lua_pushnumber ( luaVM, color.G );
            lua_pushnumber ( luaVM, color.B );
            lua_pushnumber ( luaVM, color.A );
            return 4;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipOrdering ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            short sOrdering = pMarker->GetOrdering ();
            lua_pushnumber ( luaVM, sOrdering );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipVisibleDistance ( lua_State* luaVM )
{
    CClientRadarMarker* pMarker = NULL;
    CVector vecPosition;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            unsigned short usVisibleDistance = pMarker->GetVisibleDistance ();
            lua_pushnumber ( luaVM, usVisibleDistance );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "blip", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipIcon ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    unsigned char ucIcon = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucIcon );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipIcon ( *pEntity, ucIcon ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipSize ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    unsigned char ucSize = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( ucSize );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipSize ( *pEntity, ucSize ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipColor ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    SColor color;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( color.R );
    argStream.ReadNumber ( color.G );
    argStream.ReadNumber ( color.B );
    argStream.ReadNumber ( color.A );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipColor ( *pEntity, color ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipOrdering ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    short sOrdering;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( sOrdering );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipOrdering ( *pEntity, sOrdering ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipVisibleDistance ( lua_State* luaVM )
{
    CClientEntity* pEntity = NULL;
    unsigned short usVisibleDistance;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );
    argStream.ReadNumber ( usVisibleDistance );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pEntity )
        {
            if ( CStaticFunctionDefinitions::SetBlipVisibleDistance ( *pEntity, usVisibleDistance ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
        else
            m_pScriptDebugging->LogBadPointer ( luaVM, "element", 1 );
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}


