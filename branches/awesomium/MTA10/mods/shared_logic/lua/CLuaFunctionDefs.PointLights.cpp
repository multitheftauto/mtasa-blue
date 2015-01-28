/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.PointLights.cpp
*  PURPOSE:     Lua function definitions class
*  DEVELOPERS:  Alexander Romanov <lexr128@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"


int CLuaFunctionDefs::CreateLight ( lua_State* luaVM )
{
//  light createLight ( int lightType, float posX, float posY, float posX, [ float radius = 3, int r = 255, int g = 0, int b = 0, float dirX = 0, float dirY = 0, float dirZ = 0, bool createsShadow = false ] )
    int iMode; CVector vecPosition; float fRadius; SColor color; CVector vecDirection; bool bCreatesShadow;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadNumber ( iMode );
    argStream.ReadNumber ( vecPosition.fX );
    argStream.ReadNumber ( vecPosition.fY );
    argStream.ReadNumber ( vecPosition.fZ );
    argStream.ReadNumber ( fRadius, 3.0f );
    argStream.ReadNumber ( color.R, 255 );
    argStream.ReadNumber ( color.G, 0 );
    argStream.ReadNumber ( color.B, 0 );
    argStream.ReadNumber ( vecDirection.fX, 0 );
    argStream.ReadNumber ( vecDirection.fY, 0 );
    argStream.ReadNumber ( vecDirection.fZ, 0 );
    argStream.ReadBool ( bCreatesShadow, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        CResource* pResource = pLuaMain ? pLuaMain->GetResource () : NULL;
        if ( pResource )
        {
            // Create it
            CClientPointLights* pLight = CStaticFunctionDefinitions::CreateLight ( *pResource, iMode, vecPosition, fRadius, color, vecDirection );
            if ( pLight )
            {
                CElementGroup * pGroup = pResource->GetElementGroup();
                if ( pGroup )
                {
                    pGroup->Add ( ( CClientEntity* ) pLight );
                }
                lua_pushelement ( luaVM, pLight );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetLightType ( lua_State* luaVM )
{
//  int getLightType ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLight );

    if ( !argStream.HasErrors () )
    {
        int iMode;
        if ( CStaticFunctionDefinitions::GetLightType ( pLight, iMode ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( iMode ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetLightRadius ( lua_State* luaVM )
{
//  float getLightRadius ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLight );

    if ( !argStream.HasErrors () )
    {
        float fRadius;
        if ( CStaticFunctionDefinitions::GetLightRadius ( pLight, fRadius ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( fRadius ) );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );


    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetLightColor ( lua_State* luaVM )
{
//  int, int, int getLightColor ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLight );

    if ( !argStream.HasErrors () )
    {
        SColor color;
        if ( CStaticFunctionDefinitions::GetLightColor ( pLight, color ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.R ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.G ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( color.B ) );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetLightDirection ( lua_State* luaVM )
{
//  float, float, float getLightDirection ( light theLight )
    CClientPointLights* pLight;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLight );

    if ( !argStream.HasErrors () )
    {
        CVector vecDirection;
        if ( CStaticFunctionDefinitions::GetLightDirection ( pLight, vecDirection ) )
        {
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( vecDirection.fX ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( vecDirection.fY ) );
            lua_pushnumber ( luaVM, static_cast < lua_Number > ( vecDirection.fZ ) );
            return 3;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetLightRadius ( lua_State* luaVM )
{
//  bool setLightRadius ( Light theLight, float radius )
    CClientPointLights* pLight; float fRadius;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLight );
    argStream.ReadNumber ( fRadius );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetLightRadius ( pLight, fRadius ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetLightColor ( lua_State* luaVM )
{
//  bool setLightColor ( light theLight, float r, float g, float b )
    CClientPointLights* pLight; float fRed; float fGreen; float fBlue;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLight );
    argStream.ReadNumber ( fRed );
    argStream.ReadNumber ( fGreen );
    argStream.ReadNumber ( fBlue );

    if ( !argStream.HasErrors () )
    {
        SColorRGBA color ( fRed, fGreen, fBlue, 0 );
        if ( CStaticFunctionDefinitions::SetLightColor ( pLight, color ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetLightDirection ( lua_State* luaVM )
{
//  bool setLightDirection ( light theLight, float x, float y, float z )
    CClientPointLights* pLight; CVector vecDirection;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pLight );
    argStream.ReadNumber ( vecDirection.fX );
    argStream.ReadNumber ( vecDirection.fY );
    argStream.ReadNumber ( vecDirection.fZ );

    if ( !argStream.HasErrors () )
    {
        if ( CStaticFunctionDefinitions::SetLightDirection ( pLight, vecDirection ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}