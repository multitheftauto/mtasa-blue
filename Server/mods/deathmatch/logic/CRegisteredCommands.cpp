/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRegisteredCommands.cpp
 *  PURPOSE:     Registered (lua) command manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CRegisteredCommands.h"
#include "lua/LuaCommon.h"
#include "lua/CLuaArguments.h"
#include "lua/CLuaMain.h"
#include "CAccessControlListManager.h"
#include "CClient.h"
#include "CConsoleClient.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CScriptDebugging.h"
#include "CMainConfig.h"

CRegisteredCommands::CRegisteredCommands(CAccessControlListManager* aclManager)
{
    m_pACLManager = aclManager;
    m_bIteratingList = false;
}

CRegisteredCommands::~CRegisteredCommands()
{
    ClearCommands();
}

bool CRegisteredCommands::AddCommand(CLuaMain* luaMain, const char* commandName, const CLuaFunctionRef& luaFunction, bool restricted, bool caseSensitive)
{
    assert(luaMain);
    assert(commandName);

    if (CommandExists(commandName, nullptr))
    {
        const auto policy = static_cast<MultiCommandHandlerPolicy>(g_pGame->GetConfig()->GetAllowMultiCommandHandlers());

        switch (policy)
        {
            case MultiCommandHandlerPolicy::BLOCK:
                g_pGame->GetScriptDebugging()->LogError(
                    luaMain->GetVM(), "addCommandHandler: Duplicate command registration blocked for '%s' (multiple handlers disabled)", commandName);
                return false;

            case MultiCommandHandlerPolicy::WARN:
                g_pGame->GetScriptDebugging()->LogWarning(luaMain->GetVM(), "Attempt to register duplicate command '%s'", commandName);
                break;

            case MultiCommandHandlerPolicy::ALLOW:
            default:
                break;
        }
    }

    // Check if we already have this key and handler
    SCommand* existingCommand = GetCommand(commandName, luaMain);

    if (existingCommand && luaFunction == existingCommand->luaFunction)
        return false;

    // Create the entry
    auto command = new SCommand;
    command->luaMain = luaMain;
    command->commandName.AssignLeft(commandName, MAX_REGISTERED_COMMAND_LENGTH);
    command->luaFunction = luaFunction;
    command->restricted = restricted;
    command->caseSensitive = caseSensitive;

    // Add it to our list
    m_Commands.push_back(command);

    return true;
}

bool CRegisteredCommands::RemoveCommand(CLuaMain* luaMain, const char* commandName, const CLuaFunctionRef& luaFunction)
{
    assert(luaMain);
    assert(commandName);

    // Call the handler for every virtual machine that matches the given key
    bool                           found = false;
    std::list<SCommand*>::iterator iter = m_Commands.begin();

    while (iter != m_Commands.end())
    {
        const int compareResult =
            (*iter)->caseSensitive ? strcmp((*iter)->commandName.c_str(), commandName) : stricmp((*iter)->commandName.c_str(), commandName);

        // Matching VM's and names?
        if ((*iter)->luaMain == luaMain && compareResult == 0)
        {
            if (VERIFY_FUNCTION(luaFunction) && (*iter)->luaFunction != luaFunction)
            {
                ++iter;
                continue;
            }

            // Delete it and remove it from our list
            if (m_bIteratingList)
            {
                m_TrashCan.emplace(*iter);
                ++iter;
            }
            else
            {
                delete *iter;
                iter = m_Commands.erase(iter);
            }

            found = true;
        }
        else
            ++iter;
    }

    return found;
}

void CRegisteredCommands::ClearCommands()
{
    // Delete all the commands
    for (SCommand* command : m_Commands)
        delete command;

    // Clear the list
    m_Commands.clear();
}

void CRegisteredCommands::CleanUpForVM(CLuaMain* luaMain)
{
    assert(luaMain);

    // Delete every command that matches
    std::list<SCommand*>::iterator iter = m_Commands.begin();

    while (iter != m_Commands.end())
    {
        // Matching VM's?
        if ((*iter)->luaMain == luaMain)
        {
            // Delete the entry and remove it from the list
            delete *iter;
            iter = m_Commands.erase(iter);
        }
        else
            ++iter;
    }
}

bool CRegisteredCommands::CommandExists(const char* commandName, CLuaMain* luaMain)
{
    assert(commandName);

    return GetCommand(commandName, luaMain) != nullptr;
}

bool CRegisteredCommands::ProcessCommand(const char* commandName, const char* arguments, CClient* client)
{
    assert(commandName);

    // Call the handler for every virtual machine that matches the given key
    bool                                 handled = false;
    std::list<SCommand*>::const_iterator iter = m_Commands.begin();

    m_bIteratingList = true;

    for (; iter != m_Commands.end(); ++iter)
    {
        const int compareResult =
            (*iter)->caseSensitive ? strcmp((*iter)->commandName.c_str(), commandName) : stricmp((*iter)->commandName.c_str(), commandName);

        // Matching names?
        if (compareResult == 0)
        {
            if (m_pACLManager->CanObjectUseRight(
                    client->GetAccount()->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER, (*iter)->commandName,
                    CAccessControlListRight::RIGHT_TYPE_COMMAND,
                    !(*iter)->restricted))  // If this command is restricted, the default access should be false unless granted specially
            {
                // Call it
                CallCommandHandler((*iter)->luaMain, (*iter)->luaFunction, (*iter)->commandName, arguments, client);
                handled = true;
            }
        }
    }

    m_bIteratingList = false;
    TakeOutTheTrash();

    // Return whether some handler was called or not
    return handled;
}

CRegisteredCommands::SCommand* CRegisteredCommands::GetCommand(const char* commandName, class CLuaMain* luaMain)
{
    assert(commandName);

    // Try to find an entry with a matching name in our list
    for (SCommand* command : m_Commands)
    {
        const int compareResult =
            command->caseSensitive ? strcmp(command->commandName.c_str(), commandName) : stricmp(command->commandName.c_str(), commandName);

        // Matching name and no given VM or matching VM
        if (compareResult == 0 && (!luaMain || luaMain == command->luaMain))
            return command;
    }

    // Doesn't exist
    return nullptr;
}

void CRegisteredCommands::CallCommandHandler(CLuaMain* luaMain, const CLuaFunctionRef& luaFunction, const char* commandName, const char* arguments,
                                             CClient* client)
{
    assert(luaMain);
    assert(commandName);

    CLuaArguments luaArguments;

    // First, try to call a handler with the same number of arguments
    if (client)
    {
        switch (client->GetClientType())
        {
            case CClient::CLIENT_PLAYER:
            {
                luaArguments.PushElement(static_cast<CPlayer*>(client));
                break;
            }
            case CClient::CLIENT_CONSOLE:
            {
                luaArguments.PushElement(static_cast<CConsoleClient*>(client));
                break;
            }
            default:
            {
                luaArguments.PushBoolean(false);
                break;
            }
        }
    }
    else
        luaArguments.PushBoolean(false);

    luaArguments.PushString(commandName);

    if (arguments && *arguments)
    {
        // Avoid dynamic heap allocations for typical command arguments (< 512 bytes)
        char                    stackBuffer[512];
        char*                   argumentBuffer = stackBuffer;
        const size_t            argumentLength = strlen(arguments);
        std::unique_ptr<char[]> heapBuffer;

        if (argumentLength >= sizeof(stackBuffer))
        {
            heapBuffer = std::make_unique<char[]>(argumentLength + 1);
            argumentBuffer = heapBuffer.get();
        }

        memcpy(argumentBuffer, arguments, argumentLength + 1);
        char* arg = strtok(argumentBuffer, " ");

        while (arg)
        {
            luaArguments.PushString(arg);
            arg = strtok(nullptr, " ");
        }
    }

    // Call the handler with the arguments we pushed
    luaArguments.Call(luaMain, luaFunction);
}

void CRegisteredCommands::GetCommands(lua_State* luaVM)
{
    unsigned int index = 0;

    lua_newtable(luaVM);

    for (SCommand* command : m_Commands)
    {
        // Create an entry table: {'command', resource}
        lua_pushinteger(luaVM, ++index);
        lua_createtable(luaVM, 0, 2);
        {
            lua_pushstring(luaVM, command->commandName.c_str());
            lua_rawseti(luaVM, -2, 1);

            lua_pushresource(luaVM, command->luaMain->GetResource());
            lua_rawseti(luaVM, -2, 2);
        }
        lua_settable(luaVM, -3);
    }
}

void CRegisteredCommands::GetCommands(lua_State* luaVM, CLuaMain* targetLuaMain)
{
    unsigned int index = 0;

    lua_newtable(luaVM);

    for (SCommand* command : m_Commands)
    {
        if (command->luaMain == targetLuaMain)
        {
            lua_pushinteger(luaVM, ++index);
            lua_pushstring(luaVM, command->commandName.c_str());
            lua_settable(luaVM, -3);
        }
    }
}

void CRegisteredCommands::TakeOutTheTrash()
{
    for (SCommand* command : m_TrashCan)
    {
        m_Commands.remove(command);
        delete command;
    }

    m_TrashCan.clear();
}
