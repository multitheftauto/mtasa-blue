/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.x
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/luadefs/CLuaMarkerDefs.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

void CLuaMarkerDefs::LoadFunctions ( void )
{
    // Marker functions
    CLuaCFunctions::AddFunction ( "createMarker", CreateMarker );

    // Marker get functions
    CLuaCFunctions::AddFunction ( "getMarkerCount", GetMarkerCount );
    CLuaCFunctions::AddFunction ( "getMarkerType", GetMarkerType );
    CLuaCFunctions::AddFunction ( "getMarkerSize", GetMarkerSize );
    CLuaCFunctions::AddFunction ( "getMarkerColor", GetMarkerColor );
    CLuaCFunctions::AddFunction ( "getMarkerTarget", GetMarkerTarget );
    CLuaCFunctions::AddFunction ( "getMarkerIcon", GetMarkerIcon );

    // Marker set functions
    CLuaCFunctions::AddFunction ( "setMarkerType", SetMarkerType );
    CLuaCFunctions::AddFunction ( "setMarkerSize", SetMarkerSize );
    CLuaCFunctions::AddFunction ( "setMarkerColor", SetMarkerColor );
    CLuaCFunctions::AddFunction ( "setMarkerTarget", SetMarkerTarget );
    CLuaCFunctions::AddFunction ( "setMarkerIcon", SetMarkerIcon );
}


void CLuaMarkerDefs::AddClass ( lua_State* luaVM )
{
    lua_newclass ( luaVM );

    lua_classfunction ( luaVM, "create", "createMarker" );
    lua_classfunction ( luaVM, "getCount", "getMarkerCount" );

    lua_classfunction ( luaVM, "getMarkerType", "getMarkerType" );
    lua_classfunction ( luaVM, "getIcon", "getMarkerIcon" );
    lua_classfunction ( luaVM, "getSize", "getMarkerSize" );
    lua_classfunction ( luaVM, "getTarget", "getMarkerTarget", OOP_GetMarkerTarget );
    lua_classfunction ( luaVM, "getColor", "getMarkerColor" ); // color

    lua_classfunction ( luaVM, "setType", "setMarkerType" );
    lua_classfunction ( luaVM, "setIcon", "setMarkerIcon" );
    lua_classfunction ( luaVM, "setSize", "setMarkerSize" );
    lua_classfunction ( luaVM, "setTarget", "setMarkerTarget" );
    lua_classfunction ( luaVM, "setColor", "setMarkerColor" ); // color

    lua_classvariable ( luaVM, "markerType", "setMarkerType", "getMarkerType" );
    lua_classvariable ( luaVM, "icon", "setMarkerIcon", "getMarkerIcon" );
    lua_classvariable ( luaVM, "size", "setMarkerSize", "getMarkerSize" );

    lua_classvariable ( luaVM, "target", "setMarkerTarget", "getMarkerTarget", SetMarkerTarget, OOP_GetMarkerTarget );
    //lua_classvariable ( luaVM, "color", CLuaOOPDefs::SetMarkerColor, CLuaOOPDefs::GetMarkerColor ); color

    lua_registerclass ( luaVM, "Marker", "Element" );
}


int CLuaMarkerDefs::CreateMarker ( lua_State* luaVM )
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

int CLuaMarkerDefs::GetMarkerCount ( lua_State* luaVM )
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

int CLuaMarkerDefs::GetMarkerType ( lua_State* luaVM )
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

int CLuaMarkerDefs::GetMarkerSize ( lua_State* luaVM )
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

int CLuaMarkerDefs::GetMarkerColor ( lua_State* luaVM )
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


int CLuaMarkerDefs::GetMarkerTarget ( lua_State* luaVM )
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

int CLuaMarkerDefs::OOP_GetMarkerTarget ( lua_State* luaVM )
{
    CMarker* pMarker = NULL;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pMarker );

    if ( !argStream.HasErrors () )
    {
        CVector vecPosition;
        if ( CStaticFunctionDefinitions::GetMarkerTarget ( pMarker, vecPosition ) )
        {
            lua_pushvector ( luaVM, vecPosition );
        }
        else
            lua_pushboolean ( luaVM, false );

        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}

int CLuaMarkerDefs::GetMarkerIcon ( lua_State* luaVM )
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


int CLuaMarkerDefs::SetMarkerType ( lua_State* luaVM )
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


int CLuaMarkerDefs::SetMarkerSize ( lua_State* luaVM )
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

int CLuaMarkerDefs::SetMarkerColor ( lua_State* luaVM )
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

int CLuaMarkerDefs::SetMarkerTarget ( lua_State* luaVM )
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

int CLuaMarkerDefs::SetMarkerIcon ( lua_State* luaVM )
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