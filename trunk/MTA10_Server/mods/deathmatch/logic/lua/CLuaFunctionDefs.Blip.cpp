/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Blip.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Kent Simon <>
*               Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Ed Lyons <>
*               Oliver Brown <>
*               Jax <>
*               Chris McArthur <>
*               Kevin Whiteside <>
*               lil_Toady <>
*               Alberto Alonso <rydencillo@gmail.com>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::CreateBlip ( lua_State* luaVM )
{
    CVector vecPosition;
    unsigned char ucIcon, ucSize;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering;
    unsigned short usVisibleDistance;
    CElement* pVisibleTo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( ucIcon, 0 );
    argStream.ReadNumber ( ucSize, 2 );
    argStream.ReadNumber ( color.R, color.R );
    argStream.ReadNumber ( color.G, color.G );
    argStream.ReadNumber ( color.B, color.B );
    argStream.ReadNumber ( color.A, color.A );
    argStream.ReadNumber ( sOrdering, 0 );
    argStream.ReadNumber ( usVisibleDistance, 16383 );
    if ( argStream.NextIsBool () || argStream.NextIsNil () )
        pVisibleTo = NULL;
    else
        argStream.ReadUserData ( pVisibleTo, m_pRootElement );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = g_pGame->GetLuaManager ()->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CResource* pResource = pLuaMain->GetResource ();
            if ( pResource )
            {
                // Create the blip
                CBlip* pBlip = CStaticFunctionDefinitions::CreateBlip ( pResource, vecPosition, ucIcon, ucSize, color, sOrdering, usVisibleDistance, pVisibleTo );
                if ( pBlip )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
                    if ( pGroup )
                    {
                        pGroup->Add ( pBlip );
                    }
                    lua_pushelement ( luaVM, pBlip );
                    return 1;
                }
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateBlipAttachedTo ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucIcon, ucSize;
    SColorRGBA color ( 255, 0, 0, 255 );
    short sOrdering;
    unsigned short usVisibleDistance;
    CElement* pVisibleTo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucIcon, 0 );
    argStream.ReadNumber ( ucSize, 2 );
    argStream.ReadNumber ( color.R, color.R );
    argStream.ReadNumber ( color.G, color.G );
    argStream.ReadNumber ( color.B, color.B );
    argStream.ReadNumber ( color.A, color.A );
    argStream.ReadNumber ( sOrdering, 0 );
    argStream.ReadNumber ( usVisibleDistance, 16383 );
    if ( argStream.NextIsBool () || argStream.NextIsNil () )
        pVisibleTo = NULL;
    else
        argStream.ReadUserData ( pVisibleTo, m_pRootElement );

    if ( !argStream.HasErrors () )
    {
        CResource * resource = m_pLuaManager->GetVirtualMachine ( luaVM )->GetResource ();
        if ( resource )
        {
            // Create the blip
            CBlip* pBlip = CStaticFunctionDefinitions::CreateBlipAttachedTo ( resource, pElement, ucIcon, ucSize, color, sOrdering, usVisibleDistance, pVisibleTo );
            if ( pBlip )
            {
                pBlip->SetParentObject ( resource->GetDynamicElementRoot () );
                CElementGroup * group = resource->GetElementGroup ();
                if ( group )
                {
                    group->Add ( pBlip );
                }
                lua_pushelement ( luaVM, pBlip );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipIcon ( lua_State* luaVM )
{
    CBlip* pBlip;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBlip );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucIcon;
        if ( CStaticFunctionDefinitions::GetBlipIcon ( pBlip, ucIcon ) )
        {
            lua_pushnumber ( luaVM, ucIcon );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBlipSize ( lua_State* luaVM )
{
    CBlip* pBlip;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBlip );

    if ( !argStream.HasErrors () )
    {
        unsigned char ucSize;
        if ( CStaticFunctionDefinitions::GetBlipSize ( pBlip, ucSize ) )
        {
            lua_pushnumber ( luaVM, ucSize );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetBlipColor ( lua_State* luaVM )
{
    CBlip* pBlip;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBlip );

    if ( !argStream.HasErrors () )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetBlipColor ( pBlip, color ) )
        {
            lua_pushnumber ( luaVM, color.R );
            lua_pushnumber ( luaVM, color.G );
            lua_pushnumber ( luaVM, color.B );
            lua_pushnumber ( luaVM, color.A );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipOrdering ( lua_State* luaVM )
{
    CBlip* pBlip;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBlip );

    if ( !argStream.HasErrors () )
    {
        short sOrdering;
        if ( CStaticFunctionDefinitions::GetBlipOrdering ( pBlip, sOrdering ) )
        {
            lua_pushnumber ( luaVM, sOrdering );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetBlipVisibleDistance ( lua_State* luaVM )
{
    CBlip* pBlip;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pBlip );

    if ( !argStream.HasErrors () )
    {
        unsigned short usVisibleDistance;
        if ( CStaticFunctionDefinitions::GetBlipVisibleDistance ( pBlip, usVisibleDistance ) )
        {
            lua_pushnumber ( luaVM, usVisibleDistance );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetBlipIcon ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucIcon;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucIcon );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBlipIcon ( pElement, ucIcon ) )
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

int CLuaFunctionDefs::SetBlipSize ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned char ucSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( ucSize );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBlipSize ( pElement, ucSize ) )
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

int CLuaFunctionDefs::SetBlipColor ( lua_State* luaVM )
{
    CElement* pElement;
    SColor color;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( color.R );
    argStream.ReadNumber ( color.G );
    argStream.ReadNumber ( color.B );
    argStream.ReadNumber ( color.A );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBlipColor ( pElement, color ) )
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

int CLuaFunctionDefs::SetBlipOrdering ( lua_State* luaVM )
{
    CElement* pElement;
    short sOrdering;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( sOrdering );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBlipOrdering ( pElement, sOrdering ) )
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

int CLuaFunctionDefs::SetBlipVisibleDistance ( lua_State* luaVM )
{
    CElement* pElement;
    unsigned short usVisibleDistance;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( usVisibleDistance );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetBlipVisibleDistance ( pElement, usVisibleDistance ) )
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

