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

bool CRegisteredCommands::AddCommand(CLuaMain* pLuaMain, const char* szKey, const CLuaFunctionRef& iLuaFunction, bool bCaseSensitive)
{
    assert(pLuaMain);
    assert(szKey);

    if (CommandExists(szKey, nullptr))
    {
        const MultiCommandHandlerPolicy allowMultiHandlers = g_pClientGame->GetAllowMultiCommandHandlers();
        
        switch (allowMultiHandlers)
        {
            case MultiCommandHandlerPolicy::BLOCK:
                g_pClientGame->GetScriptDebugging()->LogError(pLuaMain->GetVM(), "addCommandHandler: Duplicate command registration blocked for '%s' (multiple handlers disabled)", szKey);
                return false;
            
            case MultiCommandHandlerPolicy::WARN:
                g_pClientGame->GetScriptDebugging()->LogWarning(pLuaMain->GetVM(), "addCommandHandler: Attempt to register duplicate command '%s'", szKey);
                break;
            
            case MultiCommandHandlerPolicy::ALLOW:
            default:
                break;
        }
    }

    // Check if we already have this key and handler
    SCommand* pCommand = GetCommand(szKey, pLuaMain);
    if (pCommand)
    {
        if (pCommand->iLuaFunction == iLuaFunction)
            return false;
    }

    // Create the entry
    pCommand = new SCommand;
    pCommand->pLuaMain = pLuaMain;
    pCommand->strKey.AssignLeft(szKey, MAX_REGISTERED_COMMAND_LENGTH);
    pCommand->iLuaFunction = iLuaFunction;
    pCommand->bCaseSensitive = bCaseSensitive;

    // Add it to our list
    m_Commands.push_back(pCommand);
    return true;
}

bool CRegisteredCommands::RemoveCommand(CLuaMain* pLuaMain, const char* szKey)
{
    assert(pLuaMain);
    assert(szKey);

    // Call the handler for every virtual machine that matches the given key
    bool                      bFound = false;
    list<SCommand*>::iterator iter = m_Commands.begin();
    int                       iCompareResult;

    while (iter != m_Commands.end())
    {
        if ((*iter)->bCaseSensitive)
            iCompareResult = strcmp((*iter)->strKey, szKey);
        else
            iCompareResult = stricmp((*iter)->strKey, szKey);

        // Matching vm's and names?
        if ((*iter)->pLuaMain == pLuaMain && iCompareResult == 0)
        {
            bFound = true;
            // Delete it and remove it from our list
            if (m_bIteratingList)
            {
                m_TrashCan.emplace(*iter);
            }
            else
            {
                delete *iter;
                m_Commands.erase(iter);
                iter = m_Commands.begin();
                continue;
            }
        }
        iter++;
    }

    return bFound;
}

void CRegisteredCommands::ClearCommands()
{
    // Delete all the commands
    list<SCommand*>::const_iterator iter = m_Commands.begin();
    for (; iter != m_Commands.end(); iter++)
    {
        delete *iter;
    }

    // Clear the list
    m_Commands.clear();
}

void CRegisteredCommands::CleanUpForVM(CLuaMain* pLuaMain)
{
    assert(pLuaMain);

    // Delete every command that matches
    list<SCommand*>::iterator iter = m_Commands.begin();
    for (; iter != m_Commands.end();)
    {
        // Matching VM's?
        if ((*iter)->pLuaMain == pLuaMain)
        {
            // Delete the entry and remove it from the list
            delete *iter;
            iter = m_Commands.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

bool CRegisteredCommands::CommandExists(const char* szKey, CLuaMain* pLuaMain)
{
    assert(szKey);

    return GetCommand(szKey, pLuaMain) != nullptr;
}

bool CRegisteredCommands::ProcessCommand(const char* szKey, const char* szArguments)
{
    assert(szKey);

    // Call the handler for every virtual machine that matches the given key
    int  iCompareResult;
    bool bHandled = false;
    m_bIteratingList = true;
    list<SCommand*>::const_iterator iter = m_Commands.begin();
    for (; iter != m_Commands.end(); iter++)
    {
        if ((*iter)->bCaseSensitive)
            iCompareResult = strcmp((*iter)->strKey, szKey);
        else
            iCompareResult = stricmp((*iter)->strKey, szKey);

        // Matching names?
        if (iCompareResult == 0)
        {
            // Call it
            CallCommandHandler((*iter)->pLuaMain, (*iter)->iLuaFunction, (*iter)->strKey, szArguments);
            bHandled = true;
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
    list<SCommand*>::const_iterator iter = m_Commands.begin();
    int                             iCompareResult;

    for (; iter != m_Commands.end(); iter++)
    {
        if ((*iter)->bCaseSensitive)
            iCompareResult = strcmp((*iter)->strKey, szKey);
        else
            iCompareResult = stricmp((*iter)->strKey, szKey);

        // Matching name and no given VM or matching VM
        if (iCompareResult == 0 && (!pLuaMain || pLuaMain == (*iter)->pLuaMain))
        {
            return *iter;
        }
    }

    // Doesn't exist
    return nullptr;
}

void CRegisteredCommands::CallCommandHandler(CLuaMain* pLuaMain, const CLuaFunctionRef& iLuaFunction, const char* szKey, const char* szArguments)
{
    assert(pLuaMain);
    assert(szKey);

    CLuaArguments Arguments;
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
            arg = strtok(nullptr, " ");
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
