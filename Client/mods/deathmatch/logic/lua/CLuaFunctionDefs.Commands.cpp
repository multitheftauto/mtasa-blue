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
//  bool addCommandHandler ( string/table commandName, function handlerFunction, [bool caseSensitive = true] )
    SString strKey;
    std::vector < SString > vecKey;
    bool bType;
    CLuaFunctionRef iLuaFunction; bool bCaseSensitive;
    
    CScriptArgReader argStream ( luaVM );
    if ( argStream.NextIsTable() )
    {
        argStream.ReadStringTable( vecKey );
        bType = false;
    }
    else
    {
        argStream.ReadString( strKey );
        bType = true;
    }
    argStream.ReadFunction ( iLuaFunction );
    argStream.ReadBool ( bCaseSensitive, true );
    argStream.ReadFunctionComplete ();

    if ( !argStream.HasErrors () )
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine ( luaVM );
        if ( pLuaMain )
        {
            if ( bType )
            {
                // Add them to our list over command handlers
                if ( strKey == "" || m_pRegisteredCommands->AddCommand(pLuaMain, strKey, iLuaFunction, bCaseSensitive) )
                {
                    lua_pushboolean( luaVM, true );
                    return 1;
                }
            }
            else
            {
                // Add them to our list over command handlers from table {"command1","command2",...}
                bool bCommandError = false;
                for (uint i = 0; i < vecKey.size(); i++)
                {
                    if ( vecKey[i] == "" || !m_pRegisteredCommands->AddCommand(pLuaMain, vecKey[i], iLuaFunction, bCaseSensitive) )
                    {
                        bCommandError = true;
                        break;
                    }
                }
                lua_pushboolean( luaVM, !bCommandError );
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

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
        m_pScriptDebugging->LogCustom ( luaVM, argStream.GetFullErrorMessage() );

    lua_pushboolean ( luaVM, false );
    return 1;
}
