/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/lua/CLuaFunctionDefs.Commands.cpp
 *  PURPOSE:     Lua command function definitions class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

bool CLuaFunctionDefs::AddCommandHandler(lua_State* luaVM, std::string commandName, CLuaFunctionRef handlerFunction, std::optional<bool> maybeCaseSensitive)
{
    // bool addCommandHandler ( string commandName, function handlerFunction, [bool caseSensitive = true] )
    if (commandName.empty())
        return false;

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    const bool caseSensitive = maybeCaseSensitive.value_or(true);
    return m_pRegisteredCommands->AddCommand(luaMain, commandName.c_str(), handlerFunction, caseSensitive);
}

bool CLuaFunctionDefs::RemoveCommandHandler(lua_State* luaVM, std::string commandName, std::optional<CLuaFunctionRef> maybeHandlerFunction)
{
    // bool removeCommandHandler ( string commandName [, function handler] )
    if (commandName.empty())
        return false;

    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    const CLuaFunctionRef handlerFunction = maybeHandlerFunction.value_or(CLuaFunctionRef());
    return m_pRegisteredCommands->RemoveCommand(luaMain, commandName.c_str(), handlerFunction);
}

bool CLuaFunctionDefs::ExecuteCommandHandler(lua_State* luaVM, std::string commandName, std::optional<std::string> maybeArgs)
{
    // bool executeCommandHandler ( string commandName, [ string args ] )
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    const std::string args = maybeArgs.value_or("");
    return m_pRegisteredCommands->ProcessCommand(commandName.c_str(), args.c_str());
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
