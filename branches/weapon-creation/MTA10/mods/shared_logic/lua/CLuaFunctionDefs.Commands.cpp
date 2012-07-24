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
//  bool addCommandHandler ( string commandName, function handlerFunction, [bool caseSensitive = true] )
    SString strKey; CLuaFunctionRef iLuaFunction; bool bCaseSensitive;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadBool ( bCaseSensitive, true );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Add them to our list over command handlers
            if ( m_pRegisteredCommands->AddCommand ( pLuaMain, strKey, iLuaFunction, bCaseSensitive ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::RemoveCommandHandler ( lua_State* luaVM )
{
//  bool removeCommandHandler ( string commandName )
    SString strKey;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );

    if ( !argStream.HasErrors () )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Remove it from our list
            if ( m_pRegisteredCommands->RemoveCommand ( pLuaMain, strKey ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::ExecuteCommandHandler ( lua_State* luaVM )
{
//  bool executeCommandHandler ( string commandName, [ string args ] )
    SString strKey; SString strArgs;

    CScriptArgReader argStream ( luaVM );
    argStream.ReadString ( strKey );
    argStream.ReadString ( strArgs, "" );

    if ( !argStream.HasErrors () )
    {

        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            // Call it
            if ( m_pRegisteredCommands->ProcessCommand ( strKey, strArgs ) )
            {
                lua_pushboolean ( luaVM, true );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, SString ( "Bad argument @ '%s' [%s]", lua_tostring ( luaVM, lua_upvalueindex ( 1 ) ), *argStream.GetErrorMessage () ) );

    lua_pushboolean ( luaVM, false );
    return 1;
}


int CLuaFunctionDefs::toJSON ( lua_State* luaVM )
{
    // Got a string argument?
    if ( lua_type ( luaVM, 1 ) > LUA_TNIL )
    {
        // Read the argument
        CLuaArguments JSON;
        JSON.ReadArgument ( luaVM, 1 );

        // Convert it to a JSON string
        std::string strJSON;
        if ( JSON.WriteToJSONString ( strJSON ) )
        {
            // Return the JSON string
            lua_pushstring ( luaVM, strJSON.c_str () );
            return 1;
        }
    }

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}


int CLuaFunctionDefs::fromJSON ( lua_State* luaVM )
{
    // Got a string argument?
    if ( lua_type ( luaVM, 1 ) == LUA_TSTRING )
    {
        // Grab the JSON string
        const char* szJSONString = lua_tostring ( luaVM, 1 );

        // Read it into lua arguments
        CLuaArguments Converted;
        if ( Converted.ReadFromJSONString ( szJSONString ) )
        {
            // Return it as data
            Converted.PushArguments ( luaVM );
            return Converted.Count ();
        }
    }

    // Failed
    lua_pushnil ( luaVM );
    return 1;
}
