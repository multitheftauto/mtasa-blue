/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/lua/CLuaFunctionDefs.Crypt.cpp
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


int CLuaFunctionDefs::DisabledFunction ( lua_State* luaVM )
{
    m_pScriptDebugging->LogError ( luaVM, "Unsafe function was called." );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::CreateExplosion ( lua_State* luaVM )
{
    CVector vecPosition;
    unsigned char ucType;
    CElement* pCreator;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( ucType );
    argStream.ReadUserData ( pCreator, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::CreateExplosion ( vecPosition, ucType, pCreator ) )
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


int CLuaFunctionDefs::CreateFire ( lua_State* luaVM )
{
    CVector vecPosition;
    float fSize;
    CElement* pCreator;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecPosition );
    argStream.ReadNumber ( fSize, 1.8f );
    argStream.ReadUserData ( pCreator, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::CreateFire ( vecPosition, fSize, pCreator ) )
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


int CLuaFunctionDefs::GetRootElement ( lua_State* luaVM )
{
    CElement* pRoot = CStaticFunctionDefinitions::GetRootElement ();
    if ( pRoot )
    {
        lua_pushelement ( luaVM, pRoot );
        return 1;
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::LoadMapData ( lua_State* luaVM )
{
    CXMLNode* pXML;
    CElement* pParent;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pXML );
    argStream.ReadUserData ( pParent );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CElement* pLoadedRoot = CStaticFunctionDefinitions::LoadMapData ( pLuaMain, pParent, pXML );
            if ( pLoadedRoot )
            {
                lua_pushelement ( luaVM, pLoadedRoot );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaFunctionDefs::SaveMapData ( lua_State* luaVM )
{
    CXMLNode* pXML;
    CElement* pParent;
    bool bChildren;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pXML );
    argStream.ReadUserData ( pParent );
    argStream.ReadBool ( bChildren, false );

    if ( !argStream.HasErrors () )
    {
        CXMLNode* pSavedNode = CStaticFunctionDefinitions::SaveMapData ( pParent, pXML, bChildren );
        if ( pSavedNode )
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

int CLuaFunctionDefs::ResetMapInfo ( lua_State* luaVM )
{
    CElement* pElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pElement, NULL );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::ResetMapInfo ( pElement ) )
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