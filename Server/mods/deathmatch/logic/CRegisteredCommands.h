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
        bool            bRestricted;
        bool            bCaseSensitive;
    };

public:
    CRegisteredCommands(class CAccessControlListManager* pACLManager);
    ~CRegisteredCommands();

    bool AddCommand(class CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction, bool bRestricted, bool bCaseSensitive);
    bool RemoveCommand(class CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef());
    void ClearCommands();
    void CleanUpForVM(class CLuaMain* pLuaMain);

    bool CommandExists(const char* szKey, class CLuaMain* pLuaMain = NULL);

    void GetCommands(lua_State* luaVM);
    void GetCommands(lua_State* luaVM, CLuaMain* pTargetLuaMain);

    bool ProcessCommand(const char* szKey, const char* szArguments, class CClient* pClient);

private:
    SCommand* GetCommand(const char* szKey, class CLuaMain* pLuaMain = NULL);
    void CallCommandHandler(class CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, const char* szKey, const char* szArguments, class CClient* pClient);

    void TakeOutTheTrash();

    std::list<SCommand*>          m_Commands;
    std::unordered_set<SCommand*> m_TrashCan;
    bool                          m_bIteratingList;

    class CAccessControlListManager* m_pACLManager;
};
