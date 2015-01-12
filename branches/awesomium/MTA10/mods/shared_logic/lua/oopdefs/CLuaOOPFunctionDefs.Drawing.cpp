/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Drawing.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*  DEVELOPERS:  qaisjp <qaisjp@gmail.com>
*
*****************************************************************************/

#include "StdInc.h"

int CLuaOOPDefs::DxGetTextWidth ( lua_State* luaVM )
{
    //  float dxGetTextWidth ( string text, [float scale=1, mixed font="default", bool colorCoded=false] )
    SString strText; float fScale; CClientDxFont* pDxFontElement; bool bColorCoded;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pDxFontElement );
    argStream.ReadString ( strText );
    argStream.ReadNumber ( fScale, 1 );
    argStream.ReadBool ( bColorCoded, false );

    if ( !argStream.HasErrors () )
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( FONT_DEFAULT, pDxFontElement );

        // Retrieve the longest line's extent
        std::stringstream ssText ( strText );
        std::string sLineText;
        float fWidth = 0.0f, fLineExtent = 0.0f;

        while ( std::getline ( ssText, sLineText ) )
        {
            fLineExtent = g_pCore->GetGraphics ()->GetDXTextExtent ( sLineText.c_str (), fScale, pD3DXFont, bColorCoded );
            if ( fLineExtent > fWidth )
                fWidth = fLineExtent;
        }

        // Success
        lua_pushnumber ( luaVM, fWidth );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaOOPDefs::DxGetFontHeight ( lua_State* luaVM )
{
    //  int dxGetFontHeight ( [float scale=1, mixed font="default"] )
    float fScale; CClientDxFont* pDxFontElement;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pDxFontElement );
    argStream.ReadNumber ( fScale, 1 );
    
    if ( !argStream.HasErrors () )
    {
        ID3DXFont* pD3DXFont = CStaticFunctionDefinitions::ResolveD3DXFont ( FONT_DEFAULT, pDxFontElement );

        float fHeight = g_pCore->GetGraphics ()->GetDXFontHeight ( fScale, pD3DXFont );
        // Success
        lua_pushnumber ( luaVM, fHeight );
        return 1;
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}