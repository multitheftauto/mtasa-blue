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

#include <list>
#include <unordered_set>

#define MAX_REGISTERED_COMMAND_LENGTH 64
#define MAX_REGISTERED_COMMANDHANDLER_LENGTH 64

class CRegisteredCommands
{
    struct SCommand
    {
        class CLuaMain* pLuaMain;
        SString         strKey;
        CLuaFunctionRef iLuaFunction;
        bool            bCaseSensitive;
    };

public:
    CRegisteredCommands();
    ~CRegisteredCommands();

    bool AddCommand(class CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction, bool bCaseSensitive);
    bool RemoveCommand(class CLuaMain* pLuaMain, const char* szKey);
    void ClearCommands();
    void CleanUpForVM(class CLuaMain* pLuaMain);

    bool CommandExists(const char* szKey, class CLuaMain* pLuaMain = NULL);

    void GetCommands(lua_State* luaVM);
    void GetCommands(lua_State* luaVM, CLuaMain* pTargetLuaMain);

    bool ProcessCommand(const char* szKey, const char* szArguments);

private:
    SCommand* GetCommand(const char* szKey, class CLuaMain* pLuaMain = NULL);
    void      CallCommandHandler(class CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, const char* szKey, const char* szArguments);

    void TakeOutTheTrash();

    std::list<SCommand*>          m_Commands;
    std::unordered_set<SCommand*> m_TrashCan;
    bool                          m_bIteratingList;
};
