/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CMapEventManager.cpp
 *  PURPOSE:     Map event manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CMapEventManager.h"
#include "CMapEvent.h"
#include "lua/CLuaMain.h"
#include "CGame.h"
#include "CDebugHookManager.h"
#include "CScriptDebugging.h"
#include "CDummy.h"
#include "CPerfStatManager.h"

CMapEventManager::CMapEventManager()
{
    m_bIteratingList = false;
    m_bHasEvents = false;
}

CMapEventManager::~CMapEventManager()
{
    // If this happens while we're iterating the list, we're screwed
    assert(!m_bIteratingList);

    // This should always be empty when m_bIteratingList is false
    assert(m_TrashCan.empty());

    // Delete all eventhandlers
    DeleteAll();
}

bool CMapEventManager::Add(CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority,
                           float fPriorityMod)
{
    // Check for max name length
    if (strlen(szName) <= MAPEVENT_MAX_LENGTH_NAME)
    {
        // Make a new event
        CMapEvent* pEvent = new CMapEvent(pLuaMain, szName, iLuaFunction, bPropagated, eventPriority, fPriorityMod);

        // Add now
        AddInternal(pEvent);

        m_bHasEvents = true;
        return true;
    }
    return false;
}

bool CMapEventManager::Delete(CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction)
{
    // Delete all the events with matching names
    bool bRemovedSomeone = false;

    EventsIter iter = m_EventsMap.begin();
    while (iter != m_EventsMap.end())
    {
        CMapEvent* pMapEvent = iter->second;

        // Matching VM?
        if (pLuaMain == pMapEvent->GetVM())
        {
            // If name supplied, check name and function
            if (!szName || ((strcmp(pMapEvent->GetName(), szName) == 0) && (pMapEvent->GetLuaFunction() == iLuaFunction)))
            {
                // Not alredy being destroyed?
                if (!pMapEvent->IsBeingDestroyed())
                {
                    // Are we in an event handler?
                    if (m_bIteratingList)
                    {
                        // Put it in the trashcan
                        pMapEvent->SetBeingDestroyed(true);
                        m_TrashCan.push_back(pMapEvent);

                        // Remember that we deleted something
                        bRemovedSomeone = true;
                    }
                    else
                    {
                        // Delete the object
                        delete pMapEvent;

                        // Remove from list and remember that we deleted something
                        m_EventsMap.erase(iter++);
                        bRemovedSomeone = true;
                        continue;
                    }
                }
            }
        }

        // Increment iterator
        ++iter;
    }

    m_bHasEvents = !m_EventsMap.empty();

    // Return whether we actually destroyed someone or not
    return bRemovedSomeone;
}

void CMapEventManager::DeleteAll()
{
    // Delete all the events
    EventsIter iter = m_EventsMap.begin();
    while (iter != m_EventsMap.end())
    {
        CMapEvent* pMapEvent = iter->second;

        // Delete it if it's not already being destroyed
        if (!pMapEvent->IsBeingDestroyed())
        {
            delete pMapEvent;
            m_EventsMap.erase(iter++);
        }
        else
            ++iter;
    }
    m_bHasEvents = !m_EventsMap.empty();
}

bool CMapEventManager::Call(const char* szName, const CLuaArguments& Arguments, class CElement* pSource, class CElement* pThis, CPlayer* pCaller)
{
    // Check if no events
    if (!m_bHasEvents)
        return false;

    // Check if no events with a name match
    EventsIterPair itPair = m_EventsMap.equal_range(szName);
    if (itPair.first == itPair.second)
        return false;

    // Call all the events with matching names
    bool bCalled = false;
    bool bIsAlreadyIterating = m_bIteratingList;
    m_bIteratingList = true;

    // Copy the results into a array in case m_EventsMap is modified during the call
    std::vector<CMapEvent*> matchingEvents;
    for (EventsIter iter = itPair.first; iter != itPair.second; ++iter)
        matchingEvents.push_back(iter->second);

    for (std::vector<CMapEvent*>::iterator iter = matchingEvents.begin(); iter != matchingEvents.end(); ++iter)
    {
        CMapEvent* pMapEvent = *iter;

        // If it's not being destroyed
        if (!pMapEvent->IsBeingDestroyed())
        {
            // Compare the names
            dassert(strcmp(pMapEvent->GetName(), szName) == 0);
            {
                // Call if propagated?
                if (pSource == pThis || pMapEvent->IsPropagated())
                {
                    // Grab the current VM
                    lua_State* pState = pMapEvent->GetVM()->GetVM();

                    LUA_CHECKSTACK(pState, 1);            // Ensure some room

                    #if MTA_DEBUG
                    int luaStackPointer = lua_gettop(pState);
                    #endif

                    TIMEUS startTime = GetTimeUs();

                    if (!g_pGame->GetDebugHookManager()->OnPreEventFunction(szName, Arguments, pSource, pCaller, pMapEvent))
                        continue;

                    // Store the current values of the globals
                    lua_getglobal(pState, "source");
                    CLuaArgument OldSource(pState, -1);
                    lua_pop(pState, 1);

                    lua_getglobal(pState, "this");
                    CLuaArgument OldThis(pState, -1);
                    lua_pop(pState, 1);

                    lua_getglobal(pState, "sourceResource");
                    CLuaArgument OldResource(pState, -1);
                    lua_pop(pState, 1);

                    lua_getglobal(pState, "sourceResourceRoot");
                    CLuaArgument OldResourceRoot(pState, -1);
                    lua_pop(pState, 1);

                    lua_getglobal(pState, "eventName");
                    CLuaArgument OldEventName(pState, -1);
                    lua_pop(pState, 1);

                    lua_getglobal(pState, "client");
                    CLuaArgument OldClient(pState, -1);
                    lua_pop(pState, 1);

                    // Set the "source", "this", "sourceResource" and "sourceResourceRoot" globals on that VM
                    lua_pushelement(pState, pSource);
                    lua_setglobal(pState, "source");

                    lua_pushelement(pState, pThis);
                    lua_setglobal(pState, "this");

                    CLuaMain*  pLuaMain = g_pGame->GetScriptDebugging()->GetTopLuaMain();
                    CResource* pSourceResource = pLuaMain ? pLuaMain->GetResource() : NULL;
                    if (pSourceResource)
                    {
                        lua_pushresource(pState, pSourceResource);
                        lua_setglobal(pState, "sourceResource");

                        lua_pushelement(pState, pSourceResource->GetResourceRootElement());
                        lua_setglobal(pState, "sourceResourceRoot");
                    }
                    else
                    {
                        lua_pushnil(pState);
                        lua_setglobal(pState, "sourceResource");

                        lua_pushnil(pState);
                        lua_setglobal(pState, "sourceResourceRoot");
                    }

                    lua_pushstring(pState, szName);
                    lua_setglobal(pState, "eventName");

                    if (pCaller)
                    {
                        lua_pushelement(pState, pCaller);
                        lua_setglobal(pState, "client");
                    }
                    else
                    {
                        lua_pushnil(pState);
                        lua_setglobal(pState, "client");
                    }

                    // Call it
                    pMapEvent->Call(Arguments);
                    bCalled = true;

                    g_pGame->GetDebugHookManager()->OnPostEventFunction(szName, Arguments, pSource, pCaller, pMapEvent);

                    // Reset the globals on that VM
                    OldSource.Push(pState);
                    lua_setglobal(pState, "source");

                    OldThis.Push(pState);
                    lua_setglobal(pState, "this");

                    OldResource.Push(pState);
                    lua_setglobal(pState, "sourceResource");

                    OldResourceRoot.Push(pState);
                    lua_setglobal(pState, "sourceResourceRoot");

                    OldEventName.Push(pState);
                    lua_setglobal(pState, "eventName");

                    OldClient.Push(pState);
                    lua_setglobal(pState, "client");

                    #if MTA_DEBUG
                    assert(lua_gettop(pState) == luaStackPointer);
                    #endif

                    CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pMapEvent->GetVM(), szName, GetTimeUs() - startTime);
                }
            }
        }
    }

    // Clean out the trash if we're no longer calling events.
    if (!bIsAlreadyIterating)
    {
        TakeOutTheTrash();

        // We're no longer iterating the list
        m_bIteratingList = false;
    }

    // Return whether we called atleast one func or not
    return bCalled;
}

void CMapEventManager::TakeOutTheTrash()
{
    // Loop through our trashcan deleting every item
    std::list<CMapEvent*>::const_iterator iterTrash = m_TrashCan.begin();
    for (; iterTrash != m_TrashCan.end(); iterTrash++)
    {
        CMapEvent* pMapEvent = *iterTrash;

        // Remove from the eventhandler list
        EventsIter iterMap = m_EventsMap.begin();
        while (iterMap != m_EventsMap.end())
        {
            if (pMapEvent == iterMap->second)
                m_EventsMap.erase(iterMap++);
            else
                ++iterMap;
        }

        // Delete it
        delete pMapEvent;
    }

    m_bHasEvents = !m_EventsMap.empty();

    // Clear the trashcan
    m_TrashCan.clear();
}

bool CMapEventManager::HandleExists(CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction)
{
    // Return true if we find an event which matches the handle
    EventsIterPair itPair = m_EventsMap.equal_range(szName);
    for (EventsIter iter = itPair.first; iter != itPair.second; ++iter)
    {
        CMapEvent* pMapEvent = iter->second;

        // Is it not being destroyed?
        if (!pMapEvent->IsBeingDestroyed())
        {
            // Same lua main?
            if (pMapEvent->GetVM() == pLuaMain)
            {
                // Same name?
                dassert(strcmp(pMapEvent->GetName(), szName) == 0);
                {
                    // Same lua function?
                    if (pMapEvent->GetLuaFunction() == iLuaFunction)
                    {
                        // It exists
                        return true;
                    }
                }
            }
        }
    }

    // Doesn't exist
    return false;
}

void CMapEventManager::AddInternal(CMapEvent* pEvent)
{
    // Find place to insert
    EventsIterPair itPair = m_EventsMap.equal_range(pEvent->GetName());
    EventsIter     iter;
    for (iter = itPair.first; iter != itPair.second; ++iter)
    {
        if (pEvent->IsHigherPriorityThan(iter->second))
            break;
    }
    // Do insert
    m_EventsMap.insert(iter, std::pair<SString, CMapEvent*>(pEvent->GetName(), pEvent));
}

void CMapEventManager::GetHandles(CLuaMain* pLuaMain, const char* szName, lua_State* luaVM)
{
    unsigned int   uiIndex = 0;
    EventsIterPair itPair = m_EventsMap.equal_range(szName);
    for (EventsIter iter = itPair.first; iter != itPair.second; ++iter)
    {
        CMapEvent* pMapEvent = iter->second;

        // Is it not being destroyed?
        if (!pMapEvent->IsBeingDestroyed())
        {
            // Same lua main?
            if (pMapEvent->GetVM() == pLuaMain)
            {
                // Same name?
                dassert(strcmp(pMapEvent->GetName(), szName) == 0);
                {
                    lua_pushnumber(luaVM, ++uiIndex);
                    lua_getref(luaVM, pMapEvent->GetLuaFunction().ToInt());
                    lua_settable(luaVM, -3);
                }
            }
        }
    }
}
