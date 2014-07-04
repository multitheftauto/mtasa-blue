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
    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );

    if ( !argStream.HasErrors ( ) )
    {
        // Default colors and size
        unsigned char ucIcon = 0;
        unsigned char ucSize = 2;
        SColorRGBA color ( 255, 0, 0, 255 );
        short sOrdering = 0;
        unsigned short usVisibleDistance = 16383;

        // Read out the optional args
        if ( argStream.NextIsNumber ( ) )
        {
            argStream.ReadNumber( ucIcon );

            if ( argStream.NextIsNumber ( ) )
            {
                argStream.ReadNumber( ucSize );

                if ( argStream.NextIsNumber ( ) )
                {
                    argStream.ReadNumber( color.R );

                    if ( argStream.NextIsNumber ( ) )
                    {
                        argStream.ReadNumber( color.G );

                        if ( argStream.NextIsNumber ( ) )
                        {
                            argStream.ReadNumber( color.B );

                            if ( argStream.NextIsNumber ( ) )
                            {
                                argStream.ReadNumber( color.A );

                                if ( argStream.NextIsNumber ( ) )
                                {
                                    argStream.ReadNumber( sOrdering );
                                
                                    if ( argStream.NextIsNumber ( ))
                                    {
                                        argStream.ReadNumber( usVisibleDistance );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

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
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pEntity );

    // Element in place?
    if ( !argStream.HasErrors( ) )
    {
        // Grab the element and verify it
        if ( pEntity )
        {
            // Default colors and size
            unsigned char ucIcon = 0;
            unsigned char ucSize = 2;
            SColorRGBA color ( 255, 0, 0, 255 );
            short sOrdering = 0;
            unsigned short usVisibleDistance = 16383;

            // Read out the optional args
            if ( argStream.NextIsNumber ( ) )
            {
                argStream.ReadNumber( ucIcon );

                if ( argStream.NextIsNumber ( ) )
                {
                    argStream.ReadNumber( ucSize );

                    if ( argStream.NextIsNumber ( ) )
                    {
                        argStream.ReadNumber( color.R );

                        if ( argStream.NextIsNumber ( ) )
                        {
                            argStream.ReadNumber( color.G );

                            if ( argStream.NextIsNumber ( ) )
                            {
                                argStream.ReadNumber( color.B );

                                if ( argStream.NextIsNumber ( ) )
                                {
                                    argStream.ReadNumber( color.A );

                                    if ( argStream.NextIsNumber ( ) )
                                    {
                                        argStream.ReadNumber( sOrdering );

                                        if ( argStream.NextIsNumber ( ))
                                        {
                                            argStream.ReadNumber( usVisibleDistance );
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

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
    CClientRadarMarker* pMarker = NULL;
    unsigned char ucIcon = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );
    argStream.ReadNumber ( ucIcon );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            if ( CStaticFunctionDefinitions::SetBlipIcon ( *pMarker, ucIcon ) )
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
    CClientRadarMarker* pMarker = NULL;
    unsigned char ucSize = 0;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );
    argStream.ReadNumber ( ucSize );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            if ( CStaticFunctionDefinitions::SetBlipSize ( *pMarker, ucSize ) )
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
    CClientRadarMarker* pMarker = NULL;
    SColor color;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );
    argStream.ReadNumber ( color.R );
    argStream.ReadNumber ( color.G );
    argStream.ReadNumber ( color.B );
    argStream.ReadNumber ( color.A );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            if ( CStaticFunctionDefinitions::SetBlipColor ( *pMarker, color ) )
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
    CClientRadarMarker* pMarker = NULL;
    short sOrdering;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );
    argStream.ReadNumber ( sOrdering );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            if ( CStaticFunctionDefinitions::SetBlipOrdering ( *pMarker, sOrdering ) )
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
    CClientRadarMarker* pMarker = NULL;
    unsigned short usVisibleDistance;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );
    argStream.ReadNumber ( usVisibleDistance );

    if ( !argStream.HasErrors ( ) )
    {
        if ( pMarker )
        {
            if ( CStaticFunctionDefinitions::SetBlipVisibleDistance ( *pMarker, usVisibleDistance ) )
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


