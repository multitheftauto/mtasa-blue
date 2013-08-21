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
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Default level and color
        unsigned int uiLevel = 3;
        int iRed = 255;
        int iGreen = 255;
        int iBlue = 255;

        // Grab the level and color stuff
        int iArgument2 = lua_type ( luaVM, 2 );
        int iArgument3 = lua_type ( luaVM, 3 );
        int iArgument4 = lua_type ( luaVM, 4 );
        int iArgument5 = lua_type ( luaVM, 5 );
        if ( iArgument2 == LUA_TNUMBER || iArgument2 == LUA_TSTRING )
        {
            // Level 0? Own color can be specified
            uiLevel = static_cast < unsigned int > ( lua_tonumber ( luaVM, 2 ) );
            if ( uiLevel == 0 )
            {
                // level 0 can specify its own color, check if they have
                if ( iArgument3 != LUA_TNONE )
                {
                    // if they have, check that they've specified 3 potential numbers
                    if ( ( iArgument3 == LUA_TNUMBER || iArgument3 == LUA_TSTRING ) &&
                        ( iArgument4 == LUA_TNUMBER || iArgument4 == LUA_TSTRING ) &&
                        ( iArgument5 == LUA_TNUMBER || iArgument5 == LUA_TSTRING ) )
                    {
                        // read out the numbers
                        iRed = static_cast < unsigned int> ( lua_tonumber ( luaVM, 3 ) );
                        iGreen = static_cast < unsigned int> ( lua_tonumber ( luaVM, 4 ) );
                        iBlue = static_cast < unsigned int> ( lua_tonumber ( luaVM, 5 ) );

                        // check they're in range
                        if ( iRed > 255 || iRed < 0 ) 
                            m_pScriptDebugging->LogWarning ( luaVM, "Specify a red value between 0 and 255" );
                        else if ( iGreen > 255 || iGreen < 0 )
                            m_pScriptDebugging->LogWarning ( luaVM, "Specify a green value between 0 and 255" );
                        else if ( iBlue >  255 || iBlue <  0 )
                            m_pScriptDebugging->LogWarning ( luaVM, "Specify a blue value between 0 and 255" );
                    }
                    else
                    {
                        // specified something as the 3rd argument, but it can't be a number
                        m_pScriptDebugging->LogBadType ( luaVM );
                    }
                } // didn't spec a color
            } // wasn't level 0

            // Too big level?
            if ( uiLevel > 3 )
            {
                m_pScriptDebugging->LogWarning ( luaVM, "Bad level argument sent to %s (0-3)", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ) );

                lua_pushboolean ( luaVM, false );
                return 1;
            }
        }

        // Valid string?
        if ( lua_type ( luaVM, 1 ) != LUA_TNONE )
        {
            // Output it
            const char* szString = lua_makestring ( luaVM, 1 );
            if ( uiLevel == 1 )
            {
                m_pScriptDebugging->LogError ( luaVM, "%s", szString );
            }
            else if ( uiLevel == 2 )
            {
                m_pScriptDebugging->LogWarning ( luaVM, "%s", szString );
            }
            else if ( uiLevel == 3 )
            {
                m_pScriptDebugging->LogInformation ( luaVM, "%s", szString );
            }
            else if ( uiLevel == 0 )
            {
                m_pScriptDebugging->LogCustom ( luaVM, iRed, iGreen, iBlue, "%s", szString );
            }

            // Success
            lua_pushboolean ( luaVM, true );
            return 1;
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM );
    }

    // Failed
    lua_pushboolean ( luaVM, false );
    return 1;
}



