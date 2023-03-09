/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Commands.cpp
 *  PURPOSE:     Lua function definitions class
 *
 *****************************************************************************/

#include "StdInc.h"

int CLuaFunctionDefs::AddCommandHandler(lua_State* luaVM)
{
    //  bool addCommandHandler ( string commandName, function handlerFunction, [bool caseSensitive = true] )
    SString         strKey;
    CLuaFunctionRef iLuaFunction;
    bool            bCaseSensitive;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadFunction(iLuaFunction);
    argStream.ReadBool(bCaseSensitive, true);
    argStream.ReadFunctionComplete();

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Add them to our list over command handlers
            if (m_pRegisteredCommands->AddCommand(pLuaMain, strKey, iLuaFunction, bCaseSensitive))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::RemoveCommandHandler(lua_State* luaVM)
{
    //  bool removeCommandHandler ( string commandName )
    SString strKey;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Remove it from our list
            if (m_pRegisteredCommands->RemoveCommand(pLuaMain, strKey))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::ExecuteCommandHandler(lua_State* luaVM)
{
    //  bool executeCommandHandler ( string commandName, [ string args ] )
    SString strKey;
    SString strArgs;

    CScriptArgReader argStream(luaVM);
    argStream.ReadString(strKey);
    argStream.ReadString(strArgs, "");

    if (!argStream.HasErrors())
    {
        // Grab our VM
        CLuaMain* pLuaMain = m_pLuaManager->GetVirtualMachine(luaVM);
        if (pLuaMain)
        {
            // Call it
            if (m_pRegisteredCommands->ProcessCommand(strKey, strArgs))
            {
                lua_pushboolean(luaVM, true);
                return 1;
            }
        }
    }
    else
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());

    lua_pushboolean(luaVM, false);
    return 1;
}

int CLuaFunctionDefs::GetCommandHandlers(lua_State* luaVM)
{
    // table getCommandHandlers ( [ resource sourceResource ] );
    CResource* pResource = nullptr;

    CScriptArgReader argStream(luaVM);

    if (!argStream.NextIsNil() && !argStream.NextIsNone())
        argStream.ReadUserData(pResource);

    if (argStream.HasErrors())
    {
        m_pScriptDebugging->LogCustom(luaVM, argStream.GetFullErrorMessage());
        lua_pushnil(luaVM);
        return 1;
    }

    if (pResource)
    {
        CLuaMain* pLuaMain = pResource->GetVM();

        if (pLuaMain)
            m_pRegisteredCommands->GetCommands(luaVM, pLuaMain);
        else
            lua_newtable(luaVM);
    }
    else
    {
        m_pRegisteredCommands->GetCommands(luaVM);
    }

    return 1;
}
