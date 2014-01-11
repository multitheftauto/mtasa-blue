/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Output.cpp
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

int CLuaFunctionDefs::OutputConsole ( lua_State* luaVM )
{
    // Valid string argument?
    if ( lua_type ( luaVM, 1 ) != LUA_TNONE )
    {
        // Grab it and output
        const char* szText = lua_makestring ( luaVM, 1 );
        if ( CStaticFunctionDefinitions::OutputConsole ( szText ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::OutputChatBox ( lua_State* luaVM )
{
    // Valid string argument?
    if ( lua_type ( luaVM, 1 ) != LUA_TNONE )
    {
        // Grab the output string
        const char* szText = lua_makestring ( luaVM, 1 );

        // Default color setup
        unsigned char ucRed = 235;
        unsigned char ucGreen = 221;
        unsigned char ucBlue = 178;
        bool bColorCoded = false;

        // Verify arguments and grab colors if they are included
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
        {
            ucRed = static_cast < unsigned char > ( lua_tonumber ( luaVM, 2 ) );
            int iArgument3 = lua_type ( luaVM, 3 );
            if ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING )
            {
                ucGreen = static_cast < unsigned char > ( lua_tonumber ( luaVM, 3 ) );
                int iArgument4 = lua_type ( luaVM, 4 );
                if ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING )
                {
                    ucBlue = static_cast < unsigned char > ( lua_tonumber ( luaVM, 4 ) );

                    if ( lua_type ( luaVM, 5 ) == LUA_TBOOLEAN )
                        bColorCoded = ( lua_toboolean ( luaVM, 5 ) ) ? true:false;
                }
            }
        }

        // Output to the chatbox
        if ( CStaticFunctionDefinitions::OutputChatBox ( szText, ucRed, ucGreen, ucBlue, bColorCoded ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::SetClipboard ( lua_State* luaVM )
{
    // Valid string argument?
    if ( lua_istype ( luaVM, 1, LUA_TSTRING ) || lua_istype ( luaVM, 1, LUA_TNUMBER ) )
    {
        // Grab the text
        SString strText = lua_tostring ( luaVM, 1 );

        // set to clip board
        if ( CStaticFunctionDefinitions::SetClipboard ( strText ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::GetClipboard ( lua_State *luaVM )
{
    SString strText;
    if ( CStaticFunctionDefinitions::GetClipboard ( strText ) )
        lua_pushstring ( luaVM, strText.c_str() );
    else
        lua_pushnil ( luaVM );
    return 1;
}

int CLuaFunctionDefs::ShowChat ( lua_State* luaVM )
{
    // Verify arguments
    if ( lua_istype ( luaVM, 1, LUA_TBOOLEAN ) )
    {
        // Grab whether to show the chatbox or not
        bool bShow = ( lua_toboolean ( luaVM, 1 ) ) ? true:false;
        if ( CStaticFunctionDefinitions::ShowChat ( bShow ) )
        {
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    else
        m_pScriptDebugging->LogBadType ( luaVM );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::OutputClientDebugString ( lua_State* luaVM )
{
    SString strText = "";
    unsigned int uiLevel = 3;
    unsigned char ucRed = 255;
    unsigned char ucGreen = 255;
    unsigned char ucBlue = 255;
    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strText );
    argStream.ReadNumber ( uiLevel, 3 );
    argStream.ReadNumber ( ucRed, 255 );
    argStream.ReadNumber ( ucGreen, 255 );
    argStream.ReadNumber ( ucBlue, 255 );

    // Too big level?
    if ( uiLevel > 3 )
    {
        argStream.SetCustomError( "Bad level argument" );
    }

    if ( !argStream.HasErrors ( ) )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( uiLevel == 1 )
            {
                m_pScriptDebugging->LogError ( luaVM, "%s", strText.c_str ( ) );
            }
            else if ( uiLevel == 2 )
            {
                m_pScriptDebugging->LogWarning ( luaVM, "%s", strText.c_str ( ) );
            }
            else if ( uiLevel == 3 )
            {
                m_pScriptDebugging->LogInformation ( luaVM, "%s", strText.c_str ( ) );
            }
            else if ( uiLevel == 0 )
            {
                m_pScriptDebugging->LogCustom ( luaVM, ucRed, ucGreen, ucBlue, "%s", strText.c_str ( ) );
            }

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
    }
    if ( argStream.HasErrors ( ) )
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}



