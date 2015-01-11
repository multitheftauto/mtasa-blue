/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Marker.cpp
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

int CLuaFunctionDefs::CreateMarker ( lua_State* luaVM )
{
    CVector vecPosition;
    float fSize;
    SColorRGBA color ( 0, 0, 255, 255 );
    SString strType;
    CElement* pVisibleTo;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadString ( strType, "default" );
    argStream.ReadNumber ( fSize, 4.0f );
    argStream.ReadNumber ( color.R, color.R );
    argStream.ReadNumber ( color.G, color.G );
    argStream.ReadNumber ( color.B, color.B );
    argStream.ReadNumber ( color.A, color.A );

    if ( argStream.NextIsBool () || argStream.NextIsNil () )
    {
        pVisibleTo = NULL;
    }
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
                // Create it
                CMarker* pMarker = CStaticFunctionDefinitions::CreateMarker ( pResource, vecPosition, strType, fSize, color, pVisibleTo );
                if ( pMarker )
                {
                    CElementGroup * pGroup = pResource->GetElementGroup ();
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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetMarkerCount ( lua_State* luaVM )
{
    unsigned int uiCount = 0;
    if ( CStaticFunctionDefinitions::GetMarkerCount ( uiCount ) )
    {
        lua_pushnumber ( luaVM, static_cast <lua_Number> ( uiCount ) );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetMarkerType ( lua_State* luaVM )
{
    CMarker* pMarker;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        char szMarkerType[64];
        if ( CStaticFunctionDefinitions::GetMarkerType ( pMarker, szMarkerType ) )
        {
            lua_pushstring ( luaVM, szMarkerType );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetMarkerSize ( lua_State* luaVM )
{
    CMarker* pMarker;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        float fSize;
        if ( CStaticFunctionDefinitions::GetMarkerSize ( pMarker, fSize ) )
        {
            lua_pushnumber ( luaVM, static_cast <lua_Number> ( fSize ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetMarkerColor ( lua_State* luaVM )
{
    CMarker* pMarker;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetMarkerColor ( pMarker, color ) )
        {
            lua_pushnumber ( luaVM, static_cast <lua_Number> ( color.R ) );
            lua_pushnumber ( luaVM, static_cast <lua_Number> ( color.G ) );
            lua_pushnumber ( luaVM, static_cast <lua_Number> ( color.B ) );
            lua_pushnumber ( luaVM, static_cast <lua_Number> ( color.A ) );
            return 4;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetMarkerTarget ( lua_State* luaVM )
{
    CMarker* pMarker;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        CVector vecTarget;
        if ( CStaticFunctionDefinitions::GetMarkerTarget ( pMarker, vecTarget ) )
        {
            lua_pushnumber ( luaVM, vecTarget.fX );
            lua_pushnumber ( luaVM, vecTarget.fY );
            lua_pushnumber ( luaVM, vecTarget.fZ );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::GetMarkerIcon ( lua_State* luaVM )
{
    CMarker* pMarker;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        char szMarkerIcon[64];
        if ( CStaticFunctionDefinitions::GetMarkerType ( pMarker, szMarkerIcon ) )
        {
            lua_pushstring ( luaVM, szMarkerIcon );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetMarkerType ( lua_State* luaVM )
{
    CElement* pElement;
    SString strType;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strType );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetMarkerType ( pElement, strType ) )
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


int CLuaFunctionDefs::SetMarkerSize ( lua_State* luaVM )
{
    CElement* pElement;
    float fSize;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadNumber ( fSize );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetMarkerSize ( pElement, fSize ) )
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

int CLuaFunctionDefs::SetMarkerColor ( lua_State* luaVM )
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
        if ( CStaticFunctionDefinitions::SetMarkerColor ( pElement, color ) )
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

int CLuaFunctionDefs::SetMarkerTarget ( lua_State* luaVM )
{
    CElement* pElement;
    CVector vecTarget;
    CVector *pvecTarget = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );

    if ( argStream.NextIsVector3D () )
    {
        argStream.ReadVector3D ( vecTarget );
        pvecTarget = &vecTarget;
    }


    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetMarkerTarget ( pElement, pvecTarget ) )
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

int CLuaFunctionDefs::SetMarkerIcon ( lua_State* luaVM )
{
    CElement* pElement;
    SString strIcon;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement );
    argStream.ReadString ( strIcon );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetMarkerIcon ( pElement, strIcon ) )
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
