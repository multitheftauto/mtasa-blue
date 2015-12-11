/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/luadefs/CLuaSearchLightDefs.cpp
*  PURPOSE:     Lua searchlight class functions
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaSearchLightDefs::LoadFunctions ( void )
{
    CLuaCFunctions::AddFunction ( "createSearchLight", CreateSearchLight );

    CLuaCFunctions::AddFunction ( "getSearchLightStartPosition", GetSearchLightStartPosition );
    CLuaCFunctions::AddFunction ( "getSearchLightEndPosition", GetSearchLightEndPosition );
    CLuaCFunctions::AddFunction ( "getSearchLightStartRadius", GetSearchLightStartRadius );
    CLuaCFunctions::AddFunction ( "getSearchLightEndRadius", GetSearchLightEndRadius );

    CLuaCFunctions::AddFunction ( "setSearchLightStartPosition", SetSearchLightStartPosition );
    CLuaCFunctions::AddFunction ( "setSearchLightEndPosition", SetSearchLightEndPosition );
    CLuaCFunctions::AddFunction ( "setSearchLightStartRadius", SetSearchLightStartRadius );
    CLuaCFunctions::AddFunction ( "setSearchLightEndRadius", SetSearchLightEndRadius );
}

void CLuaSearchLightDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createSearchLight" );
    lua_classfunction ( luaVM, "getStartPosition", OOP_GetSearchLightStartPosition );
    lua_classfunction ( luaVM, "getEndPosition", OOP_GetSearchLightEndPosition );
    lua_classfunction ( luaVM, "getStartRadius", "getSearchLightStartRadius" );
    lua_classfunction ( luaVM, "getEndRadius", "getSearchLightEndRadius" );
    lua_classfunction ( luaVM, "setStartPosition", "setSearchLightStartPosition" );
    lua_classfunction ( luaVM, "setEndPosition", "setSearchLightEndPosition" );
    lua_classfunction ( luaVM, "setStartRadius", "setSearchLightStartRadius" );
    lua_classfunction ( luaVM, "setEndRadius", "setSearchLightEndRadius" );

    lua_classvariable ( luaVM, "startPosition", SetSearchLightStartPosition, OOP_GetSearchLightStartPosition );
    lua_classvariable ( luaVM, "endPosition", SetSearchLightEndPosition, OOP_GetSearchLightEndPosition );
    lua_classvariable ( luaVM, "startRadius", "setSearchLightStartRadius", "getSearchLightStartRadius" );
    lua_classvariable ( luaVM, "endRadius", "setSearchLightEndRadius", "getSearchLightEndRadius" );

    lua_registerclass ( luaVM, "SearchLight", "Element" );
}

int CLuaSearchLightDefs::CreateSearchLight ( lua_State* luaVM )
{
    //  searchlight createSearchLight ( float startX, float startY, float startZ, float endX, float endY, float endZ, float startRadius, float endRadius [, bool renderSpot = true ] )
    CVector vecStart, vecEnd; float startRadius, endRadius; bool renderSpot;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadVector3D ( vecStart );
    argStream.ReadVector3D ( vecEnd );
    argStream.ReadNumber ( startRadius );
    argStream.ReadNumber ( endRadius );
    argStream.ReadBool ( renderSpot, true );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource* pResource = pLuaMain ? pLuaMain->GetResource () : nullptr;

        if ( pResource )
        {
            auto pLight = CStaticFunctionDefinitions::CreateSearchLight ( *pResource, vecStart, vecEnd, startRadius, endRadius, renderSpot );
            if ( pLight )
            {
                CElementGroup* pGroup = pResource->GetElementGroup ();
                if ( pGroup )
                    pGroup->Add ( pLight );

                lua_pushelement ( luaVM, pLight );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::GetSearchLightStartPosition ( lua_State* luaVM )
{
//  bool getSearchLightStartPosition ( searchlight light )
    CClientSearchLight* pSearchLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );

    if ( !argStream.HasErrors () )
    {
        auto pos = pSearchLight->GetStartPosition ();
        lua_pushnumber ( luaVM, pos.fX );
        lua_pushnumber ( luaVM, pos.fY );
        lua_pushnumber ( luaVM, pos.fZ );
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::GetSearchLightEndPosition ( lua_State* luaVM )
{
//  bool getSearchLightEndPosition ( searchlight light )
    CClientSearchLight* pSearchLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );

    if ( !argStream.HasErrors () )
    {
        auto pos = pSearchLight->GetEndPosition ();
        lua_pushnumber ( luaVM, pos.fX );
        lua_pushnumber ( luaVM, pos.fY );
        lua_pushnumber ( luaVM, pos.fZ );
        return 3;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::GetSearchLightStartRadius ( lua_State* luaVM )
{
//  float getSearchLightStartRadius ( searchlight light )
    CClientSearchLight* pSearchLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pSearchLight->GetStartRadius () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::GetSearchLightEndRadius ( lua_State* luaVM )
{
//  float getSearchLightEndRadius ( searchlight light )
    CClientSearchLight* pSearchLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );

    if ( !argStream.HasErrors () )
    {
        lua_pushnumber ( luaVM, pSearchLight->GetEndRadius () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::SetSearchLightStartPosition ( lua_State* luaVM )
{
//  bool setSearchLightStartPosition ( searchlight light, Vector3 position )
    CClientSearchLight* pSearchLight; CVector position;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );
    argStream.ReadVector3D ( position );

    if ( !argStream.HasErrors () )
    {
        pSearchLight->SetStartPosition ( position );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::SetSearchLightEndPosition ( lua_State* luaVM )
{
//  bool setSearchLightEndPosition ( searchlight light, Vector3 position )
    CClientSearchLight* pSearchLight; CVector position;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );
    argStream.ReadVector3D ( position );

    if ( !argStream.HasErrors () )
    {
        pSearchLight->SetEndPosition ( position );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::SetSearchLightStartRadius ( lua_State* luaVM )
{
//  bool setSearchLightStartRadius ( searchlight light, float radius )
    CClientSearchLight* pSearchLight; float radius;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );
    argStream.ReadNumber ( radius );

    if ( !argStream.HasErrors () )
    {
        pSearchLight->SetStartRadius ( radius );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::SetSearchLightEndRadius ( lua_State* luaVM )
{
//  bool setSearchLightEndRadius ( searchlight light, float radius )
    CClientSearchLight* pSearchLight; float radius;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );
    argStream.ReadNumber ( radius );

    if ( !argStream.HasErrors () )
    {
        pSearchLight->SetEndRadius ( radius );
        lua_pushboolean ( luaVM, true );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::OOP_GetSearchLightStartPosition ( lua_State* luaVM )
{
//  bool getSearchLightStartPosition ( searchlight light )
    CClientSearchLight* pSearchLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, pSearchLight->GetStartPosition () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaSearchLightDefs::OOP_GetSearchLightEndPosition ( lua_State* luaVM )
{
//  bool getSearchLightEndPosition ( searchlight light )
    CClientSearchLight* pSearchLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pSearchLight );

    if ( !argStream.HasErrors () )
    {
        lua_pushvector ( luaVM, pSearchLight->GetEndPosition () );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
