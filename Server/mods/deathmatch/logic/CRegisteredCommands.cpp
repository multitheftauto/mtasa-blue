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

CRegisteredCommands::CRegisteredCommands(CAccessControlListManager* pACLManager)
{
    m_pACLManager = pACLManager;
    m_bIteratingList = false;
}

CRegisteredCommands::~CRegisteredCommands()
{
    ClearCommands();
}

bool CRegisteredCommands::AddCommand(CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction, bool bRestricted, bool bCaseSensitive)
{
    assert(pLuaMain);
    assert(szKey);

    // Check if we already have this key and handler
    SCommand* pCommand = GetCommand(szKey, pLuaMain);

    if (pCommand && iLuaFunction == pCommand->iLuaFunction)
        return false;

    // Create the entry
    pCommand = new SCommand;
    pCommand->pLuaMain = pLuaMain;
    pCommand->strKey.AssignLeft(szKey, MAX_REGISTERED_COMMAND_LENGTH);
    pCommand->iLuaFunction = iLuaFunction;
    pCommand->bRestricted = bRestricted;
    pCommand->bCaseSensitive = bCaseSensitive;

    // Add it to our list
    m_Commands.push_back(pCommand);

    return true;
}

bool CRegisteredCommands::RemoveCommand(CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction)
{
    assert(pLuaMain);
    assert(szKey);

    // Call the handler for every virtual machine that matches the given key
    int                            iCompareResult;
    bool                           bFound = false;
    std::list<SCommand*>::iterator iter = m_Commands.begin();

    while (iter != m_Commands.end())
    {
        if ((*iter)->bCaseSensitive)
            iCompareResult = strcmp((*iter)->strKey.c_str(), szKey);
        else
            iCompareResult = stricmp((*iter)->strKey.c_str(), szKey);

        // Matching VM's and names?
        if ((*iter)->pLuaMain == pLuaMain && iCompareResult == 0)
        {
            if (VERIFY_FUNCTION(iLuaFunction) && (*iter)->iLuaFunction != iLuaFunction)
            {
                iter++;
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

            bFound = true;
        }
        else
            ++iter;
    }

    return bFound;
}

void CRegisteredCommands::ClearCommands()
{
    // Delete all the commands
    std::list<SCommand*>::const_iterator iter = m_Commands.begin();

    for (; iter != m_Commands.end(); iter++)
        delete *iter;

    // Clear the list
    m_Commands.clear();
}

void CRegisteredCommands::CleanUpForVM(CLuaMain* pLuaMain)
{
    assert(pLuaMain);

    // Delete every command that matches
    std::list<SCommand*>::iterator iter = m_Commands.begin();

    while (iter != m_Commands.end())
    {
        // Matching VM's?
        if ((*iter)->pLuaMain == pLuaMain)
        {
            // Delete the entry and remove it from the list
            delete *iter;
            iter = m_Commands.erase(iter);
        }
        else
            ++iter;
    }
}

bool CRegisteredCommands::CommandExists(const char* szKey, CLuaMain* pLuaMain)
{
    assert(szKey);

    return GetCommand(szKey, pLuaMain) != NULL;
}

bool CRegisteredCommands::ProcessCommand(const char* szKey, const char* szArguments, CClient* pClient)
{
    assert(szKey);

    // Call the handler for every virtual machine that matches the given key
    bool                                 bHandled = false;
    int                                  iCompareResult;
    std::list<SCommand*>::const_iterator iter = m_Commands.begin();

    m_bIteratingList = true;

    for (; iter != m_Commands.end(); iter++)
    {
        if ((*iter)->bCaseSensitive)
            iCompareResult = strcmp((*iter)->strKey.c_str(), szKey);
        else
            iCompareResult = stricmp((*iter)->strKey.c_str(), szKey);

        // Matching names?
        if (iCompareResult == 0)
        {
            if (m_pACLManager->CanObjectUseRight(
                    pClient->GetAccount()->GetName().c_str(), CAccessControlListGroupObject::OBJECT_TYPE_USER, (*iter)->strKey,
                    CAccessControlListRight::RIGHT_TYPE_COMMAND,
                    !(*iter)->bRestricted))            // If this command is restricted, the default access should be false unless granted specially
            {
                // Call it
                CallCommandHandler((*iter)->pLuaMain, (*iter)->iLuaFunction, (*iter)->strKey, szArguments, pClient);
                bHandled = true;
            }
        }
    }

    m_bIteratingList = false;
    TakeOutTheTrash();

    // Return whether some handler was called or not
    return bHandled;
}

CRegisteredCommands::SCommand* CRegisteredCommands::GetCommand(const char* szKey, class CLuaMain* pLuaMain)
{
    assert(szKey);

    // Try to find an entry with a matching name in our list
    int                                  iCompareResult;
    std::list<SCommand*>::const_iterator iter = m_Commands.begin();

    for (; iter != m_Commands.end(); iter++)
    {
        if ((*iter)->bCaseSensitive)
            iCompareResult = strcmp((*iter)->strKey.c_str(), szKey);
        else
            iCompareResult = stricmp((*iter)->strKey.c_str(), szKey);

        // Matching name and no given VM or matching VM
        if (iCompareResult == 0 && (!pLuaMain || pLuaMain == (*iter)->pLuaMain))
            return *iter;
    }

    // Doesn't exist
    return NULL;
}

void CRegisteredCommands::CallCommandHandler(CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, const char* szKey, const char* szArguments,
                                             CClient* pClient)
{
    assert(pLuaMain);
    assert(szKey);

    CLuaArguments Arguments;

    // First, try to call a handler with the same number of arguments
    if (pClient)
    {
        switch (pClient->GetClientType())
        {
            case CClient::CLIENT_PLAYER:
            {
                Arguments.PushElement(static_cast<CPlayer*>(pClient));
                break;
            }
            case CClient::CLIENT_CONSOLE:
            {
                Arguments.PushElement(static_cast<CConsoleClient*>(pClient));
                break;
            }
            default:
            {
                Arguments.PushBoolean(false);
                break;
            }
        }
    }
    else
        Arguments.PushBoolean(false);

    Arguments.PushString(szKey);

    if (szArguments)
    {
        // Create a copy and strtok modifies the string
        char* szTempArguments = new char[strlen(szArguments) + 1];
        strcpy(szTempArguments, szArguments);

        char* arg;
        arg = strtok(szTempArguments, " ");

        while (arg)
        {
            Arguments.PushString(arg);
            arg = strtok(NULL, " ");
        }

        delete[] szTempArguments;
    }

    // Call the handler with the arguments we pushed
    Arguments.Call(pLuaMain, iLuaFunction);
}

void CRegisteredCommands::GetCommands(lua_State* luaVM)
{
    unsigned int uiIndex = 0;

    lua_newtable(luaVM);

    for (SCommand* pCommand : m_Commands)
    {
        // Create an entry table: {'command', resource}
        lua_pushinteger(luaVM, ++uiIndex);
        lua_createtable(luaVM, 0, 2);
        {
            lua_pushstring(luaVM, pCommand->strKey.c_str());
            lua_rawseti(luaVM, -2, 1);

            lua_pushresource(luaVM, pCommand->pLuaMain->GetResource());
            lua_rawseti(luaVM, -2, 2);
        }
        lua_settable(luaVM, -3);
    }
}

void CRegisteredCommands::GetCommands(lua_State* luaVM, CLuaMain* pTargetLuaMain)
{
    unsigned int uiIndex = 0;

    lua_newtable(luaVM);

    for (SCommand* pCommand : m_Commands)
    {
        if (pCommand->pLuaMain == pTargetLuaMain)
        {
            lua_pushinteger(luaVM, ++uiIndex);
            lua_pushstring(luaVM, pCommand->strKey.c_str());
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
