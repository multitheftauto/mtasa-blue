/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapEventManager.cpp
*  PURPOSE:     Map event manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#include "StdInc.h"

bool g_bAllowAspectRatioAdjustment = false;

CMapEventManager::~CMapEventManager(void)
{
    // If this happens while we're iterating the list, we're screwed
    assert(!m_bIteratingList);

    // This should always be empty when m_bIteratingList is false
    assert(m_TrashCan.empty());
}

bool CMapEventManager::Add(CLuaMain* pLuaMain, const std::string& strName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod)
{
    // Check for max name length
    if (strName.length() > MAPEVENT_MAX_LENGTH_NAME)
        return false;

    // Find a place to insert
    EventsIterPair itPair = m_EventsMap.equal_range(strName);
    EventsIter iter;
    for (iter = itPair.first; iter != itPair.second; ++iter)
    {
        const auto& other = iter->second;

        if (eventPriority < other->GetPriority())
            continue;
        if (eventPriority != other->GetPriority() ||
            fPriorityMod < other->GetPriorityMod())
            continue;
        break;
    }

    // Emplace to the map
    m_EventsMap.emplace_hint(iter, strName, new CMapEvent(pLuaMain, strName, iLuaFunction, bPropagated, eventPriority, fPriorityMod));
    m_bHasEvents = true;
    return true;
}

bool CMapEventManager::Delete(CLuaMain* pLuaMain, const std::string& strName, const CLuaFunctionRef& iLuaFunction)
{
    auto isMatch = [&](const std::unique_ptr<CMapEvent>& pMapEvent) {
        // Matching VM?
        if (pMapEvent->GetVM() != pLuaMain)
            return false;
        // Check name
        if (pMapEvent->GetName() != strName)
            return false;
        // Check function
        if (pMapEvent->GetLuaFunction() != iLuaFunction)
            return false;
        // Don't delete if already being destroyed
        if (pMapEvent->IsBeingDestroyed())
            return false;
        return true;
    };

    return Delete(isMatch);
}

bool CMapEventManager::Delete(CLuaMain* pLuaMain, const std::string& strName)
{
    auto isMatch = [&](const std::unique_ptr<CMapEvent>& pMapEvent) {
        // Matching VM?
        if (pMapEvent->GetVM() != pLuaMain)
            return false;
        // Check name
        if (pMapEvent->GetName() != strName)
            return false;
        // Don't delete if already being destroyed
        if (pMapEvent->IsBeingDestroyed())
            return false;
        return true;
    };

    return Delete(isMatch);
}

bool CMapEventManager::Delete(CLuaMain* pLuaMain)
{
    auto isMatch = [&](const std::unique_ptr<CMapEvent>& pMapEvent) {
        // Matching VM?
        if (pMapEvent->GetVM() != pLuaMain)
            return false;
        // Don't delete if already being destroyed
        if (pMapEvent->IsBeingDestroyed())
            return false;
        return true;
    };

    return Delete(isMatch);
}

void CMapEventManager::DeleteAll(void)
{
    Delete([&](const std::unique_ptr<CMapEvent>&) { return true; });
}

#ifdef MTA_CLIENT
bool CMapEventManager::Call(const std::string& strName, const CLuaArguments& Arguments, class CClientEntity* pSource, class CClientEntity* pThis, CClientPlayer* pCaller)
#else
bool CMapEventManager::Call(const std::string& strName, const CLuaArguments& Arguments, class CElement* pSource, class CElement* pThis, CPlayer* pCaller)
#endif
{
    // Check if no events
    if (!m_bHasEvents)
        return false;

    // Check if no events with a name match
    EventsIterPair itPair = m_EventsMap.equal_range(strName);
    if (itPair.first == itPair.second)
        return false;

#ifdef MTA_CLIENT
    TIMEUS startTimeCall = GetTimeUs();
    SString strStatus;

    // Check for multi-threading slipups
    assert(IsMainThread());
#endif

    // Call all the events with matching names
    bool bCalled = false;
    bool bIsAlreadyIterating = m_bIteratingList;
    m_bIteratingList = true;

    for (EventsIter iter = itPair.first; iter != itPair.second; ++iter)
    {
        auto& pMapEvent = iter->second;

        // If it's not being destroyed
        if (!pMapEvent->IsBeingDestroyed())
        {
            // Compare the names
            dassert(pMapEvent->GetName() == strName);

            // Call if propagated?
            if (pSource == pThis || pMapEvent->IsPropagated())
            {
                // Grab the current VM
                lua_State* pState = pMapEvent->GetVM()->GetVM();

                LUA_CHECKSTACK(pState, 1);  // Ensure some room

#if MTA_DEBUG
                int luaStackPointer = lua_gettop(pState);
#endif
#ifdef MTA_CLIENT
                // Aspect ratio adjustment bodges
                if (pMapEvent->ShouldAllowAspectRatioAdjustment())
                {
                    g_bAllowAspectRatioAdjustment = true;
                    if (pMapEvent->ShouldForceAspectRatioAdjustment())
                        g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(true);
                }

                // Record event for the crash dump writer
                static bool bEnabled = (g_pCore->GetDiagnosticDebug() == EDiagnosticDebug::LUA_TRACE_0000);
                if (bEnabled)
                    g_pCore->LogEvent(0, "Lua Event", pMapEvent->GetVM()->GetScriptName(), strName.c_str());

#endif
                TIMEUS startTime = GetTimeUs();

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
#ifndef MTA_CLIENT
                lua_getglobal(pState, "client");
                CLuaArgument OldClient(pState, -1);
                lua_pop(pState, 1);
#endif
                // Set the "source", "this", "sourceResource" and "sourceResourceRoot" globals on that VM
                lua_pushelement(pState, pSource);
                lua_setglobal(pState, "source");

                lua_pushelement(pState, pThis);
                lua_setglobal(pState, "this");
#ifdef MTA_CLIENT
                CLuaMain* pLuaMain = g_pClientGame->GetScriptDebugging()->GetTopLuaMain();
#else
                CLuaMain* pLuaMain = g_pGame->GetScriptDebugging()->GetTopLuaMain();
#endif
                CResource* pSourceResource = pLuaMain ? pLuaMain->GetResource() : nullptr;
                if (pSourceResource)
                {
                    lua_pushresource(pState, pSourceResource);
                    lua_setglobal(pState, "sourceResource");
#ifdef MTA_CLIENT
                    lua_pushelement(pState, pSourceResource->GetResourceDynamicEntity());
#else
                    lua_pushelement(pState, pSourceResource->GetResourceRootElement());
#endif
                    lua_setglobal(pState, "sourceResourceRoot");
                }
                else
                {
                    lua_pushnil(pState);
                    lua_setglobal(pState, "sourceResource");

                    lua_pushnil(pState);
                    lua_setglobal(pState, "sourceResourceRoot");
                }

                lua_pushlstring(pState, strName.c_str(), strName.length());
                lua_setglobal(pState, "eventName");
#ifndef MTA_CLIENT
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
#endif
                // Call it
                pMapEvent->Call(Arguments);
                bCalled = true;

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
#ifndef MTA_CLIENT
                OldClient.Push(pState);
                lua_setglobal(pState, "client");
#endif
#if MTA_DEBUG
                assert(lua_gettop(pState) == luaStackPointer);
#endif
#ifdef MTA_CLIENT
                // Aspect ratio adjustment bodges
                if (pMapEvent->ShouldAllowAspectRatioAdjustment())
                {
                    g_pCore->GetGraphics()->SetAspectRatioAdjustmentEnabled(false);
                    g_bAllowAspectRatioAdjustment = false;
                }

                TIMEUS deltaTimeUs = GetTimeUs() - startTime;

                if (deltaTimeUs > 3000)
                    if (IS_TIMING_CHECKPOINTS())
                        strStatus += SString(" (%s %d ms)", pMapEvent->GetVM()->GetScriptName(), deltaTimeUs / 1000);

                CClientPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pMapEvent->GetVM(), strName.c_str(), deltaTimeUs);
#else
                CPerfStatLuaTiming::GetSingleton()->UpdateLuaTiming(pMapEvent->GetVM(), strName.c_str(), GetTimeUs() - startTime);
#endif
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

#ifdef MTA_CLIENT
    if (IS_TIMING_CHECKPOINTS())
    {
        TIMEUS deltaTimeUs = GetTimeUs() - startTimeCall;
        if (deltaTimeUs > 5000)
            TIMING_DETAIL(SString("CMapEventManager::Call ( %s, ... ) took %d ms ( %s )", strName.c_str(), deltaTimeUs / 1000, *strStatus));
    }
#endif

    // Return whether we called atleast one func or not
    return bCalled;
}

CMapEventManager::EventsIter CMapEventManager::Erase(EventsIter iter)
{
    if (!m_bIteratingList)
    {
        // When not iterating a list, we can directly remove matching events
        return m_EventsMap.erase(iter);
    }
    else
    {
        // When iterating a list, we need to wait until the next frame to remove
        // Put it in the trashcan
        auto& pMapEvent = iter->second;
        pMapEvent->SetBeingDestroyed(true);
        m_TrashCan.push_back(iter);
        return std::next(iter);
    }
}

void CMapEventManager::TakeOutTheTrash()
{
    // Loop through our trashcan deleting every item
    for (auto iter : m_TrashCan)
    {
        m_EventsMap.erase(iter);
    }

    m_bHasEvents = !m_EventsMap.empty();

    // Clear the trashcan
    m_TrashCan.clear();
}

bool CMapEventManager::HandleExists(CLuaMain* pLuaMain, const std::string& strName, const CLuaFunctionRef& iLuaFunction)
{
    // Return true if we find an event which matches the handle
    EventsIterPair itPair = m_EventsMap.equal_range(strName);
    return std::find_if(itPair.first, itPair.second,
                        [&](const auto& pair) {
                            const auto& pMapEvent = pair.second;
                            // Is it not being destroyed?
                            if (pMapEvent->IsBeingDestroyed())
                                return false;
                            // Same lua main?
                            if (pMapEvent->GetVM() != pLuaMain)
                                return false;
                            // Same name?
                            dassert(pMapEvent->GetName() == strName);
                            // Same lua function?
                            if (pMapEvent->GetLuaFunction() != iLuaFunction)
                                return false;
                            // It exists
                            return true;
                        }) != itPair.second;
}

void CMapEventManager::GetHandles(CLuaMain* pLuaMain, const std::string& strName, lua_State* luaVM)
{
    unsigned int uiIndex = 0;
    EventsIterPair itPair = m_EventsMap.equal_range(strName);
    for (EventsIter iter = itPair.first; iter != itPair.second; ++iter)
    {
        const auto& pMapEvent = iter->second;

        // Is it not being destroyed?
        if (pMapEvent->IsBeingDestroyed())
            continue;

        // Same lua main?
        if (pMapEvent->GetVM() != pLuaMain)
            continue;

        // Same name?
        dassert(pMapEvent->GetName() == strName);
        lua_pushnumber(luaVM, ++uiIndex);
        lua_getref(luaVM, pMapEvent->GetLuaFunction().ToInt());
        lua_settable(luaVM, -3);
    }
}
