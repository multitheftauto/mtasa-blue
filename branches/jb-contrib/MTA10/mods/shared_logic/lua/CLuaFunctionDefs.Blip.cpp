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
    // Position arguments in place?
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );

    if ( ( iArgument1 == LUA_TNUMBER || iArgument1 == LUA_TSTRING ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) )
    {
        // Grab the vector
        CVector vecPosition = CVector ( static_cast < float > ( lua_tonumber ( luaVM, 1 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 2 ) ),
            static_cast < float > ( lua_tonumber ( luaVM, 3 ) ) );

        // Default colors and size
        unsigned char ucIcon = 0;
        unsigned char ucSize = 2;
        SColorRGBA color ( 255, 0, 0, 255 );
        short sOrdering = 0;
        unsigned short usVisibleDistance = 16383;

        // Read out the optional args
        int iArgument4 = lua_type ( luaVM, 4 );
        if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
        {
            ucIcon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

            int iArgument5 = lua_type ( luaVM, 5 );
            if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
            {
                ucSize = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

                int iArgument6 = lua_type ( luaVM, 6 );
                if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
                {
                    color.R = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );

                    int iArgument7 = lua_type ( luaVM, 7 );
                    if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                    {
                        color.G = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );

                        int iArgument8 = lua_type ( luaVM, 8 );
                        if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                        {
                            color.B = static_cast < unsigned char > ( lua_tonumber ( luaVM, 8 ) );

                            int iArgument9 = lua_type ( luaVM, 9 );
                            if ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING )
                            {
                                color.A = static_cast < unsigned char > ( lua_tonumber ( luaVM, 9 ) );

                                int iArgument10 = lua_type ( luaVM, 10 );
                                if ( iArgument10 == LUA_TNUMBER || iArgument10 == LUA_TSTRING )
                                {
                                    sOrdering = static_cast < short > ( lua_tonumber ( luaVM, 10 ) );
                                
                                    int iArgument11 = lua_type ( luaVM, 11 );
                                    if ( iArgument11 == LUA_TNUMBER || iArgument11 == LUA_TSTRING )
                                    {
                                        usVisibleDistance = static_cast < unsigned short > ( lua_tonumber ( luaVM, 11 ) );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateBlipAttachedTo ( lua_State* luaVM )
{
    // Element in place?
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        // Grab the element and verify it
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            // Default colors and size
            unsigned char ucIcon = 0;
            unsigned char ucSize = 2;
            SColorRGBA color ( 255, 0, 0, 255 );
            short sOrdering = 0;
            unsigned short usVisibleDistance = 16383;

            // Read out the optional args
            int iArgument2 = lua_type ( luaVM, 2 );
            if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
            {
                ucIcon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );

                int iArgument3 = lua_type ( luaVM, 3 );
                if ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING )
                {
                    ucSize = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );

                    int iArgument4 = lua_type ( luaVM, 4 );
                    if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
                    {
                        color.R = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

                        int iArgument5 = lua_type ( luaVM, 5 );
                        if ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING )
                        {
                            color.G = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

                            int iArgument6 = lua_type ( luaVM, 6 );
                            if ( iArgument6 == LUA_TNUMBER || iArgument6 == LUA_TSTRING )
                            {
                                color.B = static_cast < unsigned char > ( lua_tonumber ( luaVM, 6 ) );

                                int iArgument7 = lua_type ( luaVM, 7 );
                                if ( iArgument7 == LUA_TNUMBER || iArgument7 == LUA_TSTRING )
                                {
                                    color.A = static_cast < unsigned char > ( lua_tonumber ( luaVM, 7 ) );

                                    int iArgument8 = lua_type ( luaVM, 8 );
                                    if ( iArgument8 == LUA_TNUMBER || iArgument8 == LUA_TSTRING )
                                    {
                                        sOrdering = static_cast < short > ( lua_tonumber ( luaVM, 8 ) );
                                    
                                        int iArgument9 = lua_type ( luaVM, 9 );
                                        if ( iArgument9 == LUA_TNUMBER || iArgument9 == LUA_TSTRING )
                                        {
                                            usVisibleDistance = static_cast < unsigned short > ( lua_tonumber ( luaVM, 9 ) );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipIcon ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipSize ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipColor ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipOrdering ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipVisibleDistance ( lua_State* luaVM )
{
    if ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA )
    {
        CClientRadarMarker* pMarker = lua_toblip ( luaVM, 1 );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipIcon ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucIcon = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipSize ( lua_State* luaVM )
{
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( lua_type ( luaVM, 1 ) == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned char ucSize = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipColor ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    int iArgument3 = lua_type ( luaVM, 3 );
    int iArgument4 = lua_type ( luaVM, 4 );
    int iArgument5 = lua_type ( luaVM, 5 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) &&
        ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            SColor color;
            color.R = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            color.G = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
            color.B = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );
            color.A = static_cast < unsigned char > ( lua_tonumber ( luaVM, 5 ) );

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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipOrdering ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            short sOrdering = static_cast < short > ( lua_tonumber ( luaVM, 2 ) );

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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipVisibleDistance ( lua_State* luaVM )
{
    int iArgument1 = lua_type ( luaVM, 1 );
    int iArgument2 = lua_type ( luaVM, 2 );
    if ( ( iArgument1 == LUA_TLIGHTUSERDATA ) &&
        ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING ) )
    {
        CClientEntity* pEntity = lua_toelement ( luaVM, 1 );
        if ( pEntity )
        {
            unsigned short usVisibleDistance = static_cast < unsigned short > ( lua_tonumber ( luaVM, 2 ) );

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
        m_pScriptDebugging->LogBadType ( luaVM );

    lua_pushboolean ( luaVM, false );
    return 1;
}


