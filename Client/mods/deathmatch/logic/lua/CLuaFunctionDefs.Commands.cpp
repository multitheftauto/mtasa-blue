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

bool CLuaFunctionDefs::AddCommandHandler(lua_State* luaVM, std::variant<std::string, std::vector<std::string>> commandNames, CLuaFunctionRef handlerFunction,
                                         std::optional<bool> maybeCaseSensitive)
{
    // bool addCommandHandler ( string / table commandNames, function handlerFunction, [bool caseSensitive = true] )
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    const bool caseSensitive = maybeCaseSensitive.value_or(true);

    if (std::holds_alternative<std::string>(commandNames))
    {
        const std::string& commandName = std::get<std::string>(commandNames);
        if (commandName.empty())
            return false;

        return m_pRegisteredCommands->AddCommand(luaMain, commandName.c_str(), handlerFunction, caseSensitive);
    }

    const auto& nameList = std::get<std::vector<std::string>>(commandNames);
    if (nameList.empty())
        return false;

    std::unordered_set<std::string> uniqueCommands;
    bool                            success = false;

    for (const std::string& commandName : nameList)
    {
        if (commandName.empty())
            continue;

        // Skip duplicates within the same array registration
        if (!uniqueCommands.insert(commandName).second)
            continue;

        if (m_pRegisteredCommands->AddCommand(luaMain, commandName.c_str(), handlerFunction, caseSensitive))
            success = true;
    }

    return success;
}

bool CLuaFunctionDefs::RemoveCommandHandler(lua_State* luaVM, std::variant<std::string, std::vector<std::string>> commandNames,
                                            std::optional<CLuaFunctionRef> maybeHandlerFunction)
{
    // bool removeCommandHandler ( string / table commandNames [, function handler] )
    CLuaMain* luaMain = m_pLuaManager->GetVirtualMachine(luaVM);
    if (!luaMain)
        return false;

    const CLuaFunctionRef handlerFunction = maybeHandlerFunction.value_or(CLuaFunctionRef());

    if (std::holds_alternative<std::string>(commandNames))
    {
        const std::string& commandName = std::get<std::string>(commandNames);
        if (commandName.empty())
            return false;

        return m_pRegisteredCommands->RemoveCommand(luaMain, commandName.c_str(), handlerFunction);
    }

    const auto& nameList = std::get<std::vector<std::string>>(commandNames);
    if (nameList.empty())
        return false;

    bool success = false;
    for (const std::string& commandName : nameList)
    {
        if (commandName.empty())
            continue;

        if (m_pRegisteredCommands->RemoveCommand(luaMain, commandName.c_str(), handlerFunction))
            success = true;
    }

    return success;
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
