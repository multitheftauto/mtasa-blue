/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CRegisteredCommands.cpp
 *  PURPOSE:     Registered commands class
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CClientGame.h"

using std::list;

CRegisteredCommands::CRegisteredCommands()
{
    m_bIteratingList = false;
}

CRegisteredCommands::~CRegisteredCommands()
{
    ClearCommands();
}

bool CRegisteredCommands::AddCommand(CLuaMain* luaMain, const char* commandName, const CLuaFunctionRef& luaFunction, bool caseSensitive)
{
    assert(luaMain);
    assert(commandName);

    if (CommandExists(commandName, nullptr))
    {
        const MultiCommandHandlerPolicy allowMultiHandlers = g_pClientGame->GetAllowMultiCommandHandlers();

        switch (allowMultiHandlers)
        {
            case MultiCommandHandlerPolicy::BLOCK:
                g_pClientGame->GetScriptDebugging()->LogError(
                    luaMain->GetVM(), "addCommandHandler: Duplicate command registration blocked for '%s' (multiple handlers disabled)", commandName);
                return false;

            case MultiCommandHandlerPolicy::WARN:
                g_pClientGame->GetScriptDebugging()->LogWarning(luaMain->GetVM(), "addCommandHandler: Attempt to register duplicate command '%s'", commandName);
                break;

            case MultiCommandHandlerPolicy::ALLOW:
            default:
                break;
        }
    }

    // Check if we already have this key and handler
    SCommand* existingCommand = GetCommand(commandName, luaMain);
    if (existingCommand && existingCommand->luaFunction == luaFunction)
    {
        return false;
    }

    // Create the entry
    auto command = new SCommand;
    command->luaMain = luaMain;
    command->commandName.AssignLeft(commandName, MAX_REGISTERED_COMMAND_LENGTH);
    command->luaFunction = luaFunction;
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
    bool                      found = false;
    list<SCommand*>::iterator iter = m_Commands.begin();

    while (iter != m_Commands.end())
    {
        const int compareResult = (*iter)->caseSensitive ? strcmp((*iter)->commandName, commandName) : stricmp((*iter)->commandName, commandName);

        // Matching VMs and names?
        if ((*iter)->luaMain == luaMain && compareResult == 0)
        {
            if (VERIFY_FUNCTION(luaFunction) && (*iter)->luaFunction != luaFunction)
            {
                ++iter;
                continue;
            }

            found = true;
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
    {
        delete command;
    }

    // Clear the list
    m_Commands.clear();
}

void CRegisteredCommands::CleanUpForVM(CLuaMain* luaMain)
{
    assert(luaMain);

    // Delete every command that matches
    list<SCommand*>::iterator iter = m_Commands.begin();
    while (iter != m_Commands.end())
    {
        // Matching VM?
        if ((*iter)->luaMain == luaMain)
        {
            delete *iter;
            iter = m_Commands.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

bool CRegisteredCommands::CommandExists(const char* commandName, CLuaMain* luaMain)
{
    assert(commandName);

    return GetCommand(commandName, luaMain) != nullptr;
}

bool CRegisteredCommands::ProcessCommand(const char* commandName, const char* arguments)
{
    assert(commandName);

    bool handled = false;
    m_bIteratingList = true;

    for (SCommand* command : m_Commands)
    {
        const int compareResult = command->caseSensitive ? strcmp(command->commandName, commandName) : stricmp(command->commandName, commandName);

        // Matching names?
        if (compareResult == 0)
        {
            CallCommandHandler(command->luaMain, command->luaFunction, command->commandName, arguments);
            handled = true;
        }
    }

    m_bIteratingList = false;
    TakeOutTheTrash();

    return handled;
}

CRegisteredCommands::SCommand* CRegisteredCommands::GetCommand(const char* commandName, CLuaMain* luaMain)
{
    assert(commandName);

    for (SCommand* command : m_Commands)
    {
        const int compareResult = command->caseSensitive ? strcmp(command->commandName, commandName) : stricmp(command->commandName, commandName);

        // Matching name and no given VM or matching VM
        if (compareResult == 0 && (!luaMain || luaMain == command->luaMain))
        {
            return command;
        }
    }

    return nullptr;
}

void CRegisteredCommands::CallCommandHandler(CLuaMain* luaMain, const CLuaFunctionRef& luaFunction, const char* commandName, const char* arguments)
{
    assert(luaMain);
    assert(commandName);

    CLuaArguments luaArguments;
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
