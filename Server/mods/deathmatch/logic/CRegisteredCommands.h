/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CRegisteredCommands.h
 *  PURPOSE:     Registered (lua) command manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
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
        bool            restricted;
        bool            caseSensitive;
    };

public:
    CRegisteredCommands(class CAccessControlListManager* aclManager);
    ~CRegisteredCommands();

    bool AddCommand(class CLuaMain* luaMain, const char* commandName, const CLuaFunctionRef& luaFunction, bool restricted, bool caseSensitive);
    bool RemoveCommand(class CLuaMain* luaMain, const char* commandName, const CLuaFunctionRef& luaFunction = CLuaFunctionRef());
    void ClearCommands();
    void CleanUpForVM(class CLuaMain* luaMain);

    bool CommandExists(const char* commandName, class CLuaMain* luaMain = nullptr);

    void GetCommands(lua_State* luaVM);
    void GetCommands(lua_State* luaVM, CLuaMain* targetLuaMain);

    bool ProcessCommand(const char* commandName, const char* arguments, class CClient* client);

private:
    SCommand* GetCommand(const char* commandName, class CLuaMain* luaMain = nullptr);
    void CallCommandHandler(class CLuaMain* luaMain, const CLuaFunctionRef& luaFunction, const char* commandName, const char* arguments, class CClient* client);

    void TakeOutTheTrash();

    std::list<SCommand*>          m_Commands;
    std::unordered_set<SCommand*> m_TrashCan;
    bool                          m_bIteratingList;

    class CAccessControlListManager* m_pACLManager;
};
