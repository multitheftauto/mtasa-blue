/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CRegisteredCommands.h
 *  PURPOSE:     Registered commands class header
 *
 *****************************************************************************/

#pragma once

#include "lua/CLuaFunctionRef.h"
#include <cstdint>
#include <list>
#include <unordered_set>

#define MAX_REGISTERED_COMMAND_LENGTH        64
#define MAX_REGISTERED_COMMANDHANDLER_LENGTH 64

enum class MultiCommandHandlerPolicy : std::uint8_t
{
    BLOCK = 0,
    WARN = 1,
    ALLOW = 2
};

class CRegisteredCommands
{
    struct SCommand
    {
        class CLuaMain* luaMain;
        SString         commandName;
        CLuaFunctionRef luaFunction;
        bool            caseSensitive;
    };

public:
    CRegisteredCommands();
    ~CRegisteredCommands();

    bool AddCommand(class CLuaMain* luaMain, const char* commandName, const CLuaFunctionRef& luaFunction, bool caseSensitive);
    bool RemoveCommand(class CLuaMain* luaMain, const char* commandName, const CLuaFunctionRef& luaFunction = CLuaFunctionRef());
    void ClearCommands();
    void CleanUpForVM(class CLuaMain* luaMain);

    bool CommandExists(const char* commandName, class CLuaMain* luaMain = nullptr);

    void GetCommands(lua_State* luaVM);
    void GetCommands(lua_State* luaVM, CLuaMain* targetLuaMain);

    bool ProcessCommand(const char* commandName, const char* arguments);

private:
    SCommand* GetCommand(const char* commandName, class CLuaMain* luaMain = nullptr);
    void      CallCommandHandler(class CLuaMain* luaMain, const CLuaFunctionRef& luaFunction, const char* commandName, const char* arguments);

    void TakeOutTheTrash();

    std::list<SCommand*>          m_Commands;
    std::unordered_set<SCommand*> m_TrashCan;
    bool                          m_bIteratingList;
};
