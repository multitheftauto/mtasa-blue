/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CEvents.cpp
 *  PURPOSE:     Event manager class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CEvents.h"

CEvents::CEvents()
{
    m_bWasEventCancelled = false;
    m_bEventCancelled = false;
}

bool CEvents::AddEvent(const char* szName, const char* szArguments, CLuaMain* pLuaMain, bool bAllowRemoteTrigger)
{
    assert(szName);
    assert(szArguments);

    // Get the event if it already exists
    SEvent* pEvent = Get(szName);

    if (pEvent)
    {
        // If bAllowRemoteTrigger has been altered, return
        if (pEvent->bAllowRemoteTrigger != bAllowRemoteTrigger)
            return false;

        // Add pLuaMain to the vector, in case it's not already there
        if (!ListContains(pEvent->pLuaMainVector, pLuaMain))
            pEvent->pLuaMainVector.push_back(pLuaMain);
    }
    else
    {
        // Create and add the event
        pEvent = new SEvent;
        pEvent->strName = szName;
        pEvent->strArguments = szArguments;
        pEvent->pLuaMainVector.push_back(pLuaMain);
        pEvent->bAllowRemoteTrigger = bAllowRemoteTrigger;
    }

    m_EventHashMap[szName] = pEvent;

    return true;
}

void CEvents::RemoveEvent(SEvent* pEvent)
{
    assert(pEvent);

    // Remove it and delete it
    MapRemove(m_EventHashMap, pEvent->strName);
    delete pEvent;
}

void CEvents::RemoveEvent(const char* szName)
{
    assert(szName);

    // Find it
    SEvent* pEvent = Get(szName);
    if (pEvent)
    {
        // Delete it
        MapRemove(m_EventHashMap, pEvent->strName);
        delete pEvent;
    }
}

void CEvents::RemoveAllEvents(class CLuaMain* pMain)
{
    // Delete all items
    CFastHashMap<SString, SEvent*>::iterator iter = m_EventHashMap.begin();
    while (iter != m_EventHashMap.end())
    {
        SEvent* pEvent = (*iter).second;
        ListRemoveFirst(pEvent->pLuaMainVector, pMain);

        // If no pMain is left, delete it null it and set the bool
        if (pEvent->pLuaMainVector.empty())
        {
            // Delete the object
            delete pEvent;

            // Remove from list
            m_EventHashMap.erase(iter);
        }

        ++iter;
    }
}

SEvent* CEvents::Get(const char* szName)
{
    assert(szName);

    SEvent** pEvent = MapFind(m_EventHashMap, szName);
    if (pEvent != NULL)
    {
        return *pEvent;
    }
    return NULL;
}

void CEvents::RemoveAllEvents()
{
    // Delete all items
    CFastHashMap<SString, SEvent*>::const_iterator iter = m_EventHashMap.begin();
    for (; iter != m_EventHashMap.end(); iter++)
    {
        SEvent* pEvent = (*iter).second;
        delete pEvent;
    }

    // Clear the list
    m_EventHashMap.clear();
}

void CEvents::PreEventPulse()
{
    m_CancelledList.push_back(m_bEventCancelled);
    m_bEventCancelled = false;
    m_bWasEventCancelled = false;
    m_strLastError = "";
}

void CEvents::PostEventPulse()
{
    m_bWasEventCancelled = m_bEventCancelled;
    m_bEventCancelled = m_CancelledList.back() ? true : false;
    m_CancelledList.pop_back();
}

void CEvents::CancelEvent(bool bCancelled)
{
    m_bEventCancelled = bCancelled;
}

void CEvents::CancelEvent(bool bCancelled, const char* szReason)
{
    m_bEventCancelled = bCancelled;
    m_strLastError = SStringX(szReason);
}

bool CEvents::WasEventCancelled()
{
    return m_bWasEventCancelled;
}

const char* CEvents::GetLastError()
{
    return m_strLastError;
}
