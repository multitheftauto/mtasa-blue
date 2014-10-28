/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/oopdefs/CLuaOOPFunctionDefs.Player.cpp
*  PURPOSE:     Lua OOP specific function definitions class
*
*****************************************************************************/
#include "StdInc.h"

int CLuaOOPDefs::OutputChat ( lua_State * luaVM )
{
    // bool Player:outputChat ( string text [, int r=231, int g=217, int b=176, bool colorCoded=false ] )
    CPlayer* pPlayer; SString strText; uchar ucRed = 231; uchar ucGreen = 217; uchar ucBlue = 176; bool bColorCoded;
    
    CScriptArgReader argStream ( luaVM );
    argStream.ReadUserData ( pPlayer );
    argStream.ReadString ( strText );

    if ( argStream.NextIsNumber ( 0 ) && argStream.NextIsNumber ( 1 ) && argStream.NextIsNumber ( 2 ) )
    {
        argStream.ReadNumber ( ucRed );
        argStream.ReadNumber ( ucGreen );
        argStream.ReadNumber ( ucBlue );
    }
    else
        argStream.Skip ( 3 );

    argStream.ReadBool ( bColorCoded, false );

    if ( !argStream.HasErrors () )
    {
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            CStaticFunctionDefinitions::OutputChatBox ( strText, pPlayer, ucRed, ucGreen, ucBlue, bColorCoded, pLuaMain );
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage () );

    lua_pushboolean ( luaVM, false );
    return 1;
}
