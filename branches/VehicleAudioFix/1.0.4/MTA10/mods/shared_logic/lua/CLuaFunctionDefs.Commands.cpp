/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Commands.cpp
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

int CLuaFunctionDefs::AddCommandHandler ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Got two strings?
        int iArgument1 = lua_type ( luaVM, 1 );
        int iArgument2 = lua_type ( luaVM, 2 );
        if ( iArgument1 == LUA_TSTRING && iArgument2 == LUA_TFUNCTION )
        {
            // Check if the command name is case sensitive
            bool bCaseSensitive = true;
            if ( lua_type ( luaVM, 3 ) == LUA_TBOOLEAN )
                bCaseSensitive = ( ( lua_toboolean ( luaVM, 3 ) == 0 ) ? false : true );

            // Grab the strings. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
            CLuaFunctionRef iLuaFunction = luaM_toref ( luaVM, 2 );
            if ( szKey [0] != 0 && VERIFY_FUNCTION ( iLuaFunction ) )
            {
                // Add them to our list over command handlers
                if ( m_pRegisteredCommands->AddCommand ( pLuaMain, szKey, iLuaFunction, bCaseSensitive ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "Empty key or handler strings sent to addCommandHandler" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "addCommandHandler" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveCommandHandler ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Got a string?
        int iArgument1 = lua_type ( luaVM, 1 );
        if ( iArgument1 == LUA_TSTRING )
        {
            // Grab the string. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
            if ( szKey [0] )
            {
                // Remove it from our list
                if ( m_pRegisteredCommands->RemoveCommand ( pLuaMain, szKey ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "Empty key string sent to removeCommandHandler" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "removeCommandHandler" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ExecuteCommandHandler ( lua_State* luaVM )
{
    // Grab our VM
    CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
    if ( pLuaMain )
    {
        // Got a string?
        if ( lua_type ( luaVM, 1 ) == LUA_TSTRING  )
        {
            // Grab the string. Valid?
            const char* szKey = lua_tostring ( luaVM, 1 );
            if ( szKey [0] )
            {                
                // Grab the args
                const char* szArgs = 0;
                if ( lua_type ( luaVM, 2 ) == LUA_TSTRING )
                    szArgs = lua_tostring ( luaVM, 2 );

                // Call it
                if ( m_pRegisteredCommands->ProcessCommand ( szKey, szArgs ) )
                {
                    lua_pushboolean ( luaVM, true );
                    return 1;
                }
            }
            else
                m_pScriptDebugging->LogWarning ( luaVM, "Empty key string sent to executeCommandHandler" );
        }
        else
            m_pScriptDebugging->LogBadType ( luaVM, "executeCommandHandler" );
    }

    lua_pushboolean ( luaVM, false );
    return 1;
}


