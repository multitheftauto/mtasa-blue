/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CEvents.cpp
 *  PURPOSE:     Events class
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

CEvents::CEvents()
{
    m_bWasEventCancelled = false;
    m_bEventCancelled = false;
}

CEvents::~CEvents()
{
    RemoveAllEvents();
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
    if (!m_EventHashMap.empty())
    {
        MapRemove(m_EventHashMap, pEvent->strName);
    }
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
        if (!m_EventHashMap.empty())
        {
            MapRemove(m_EventHashMap, pEvent->strName);
        }
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

    m_EventHashMap.clear();
}

void CEvents::PreEventPulse()
{
    m_CancelledList.push_back(m_bEventCancelled);
    m_bEventCancelled = false;
    m_bWasEventCancelled = false;
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

bool CEvents::WasEventCancelled()
{
    return m_bWasEventCancelled;
}
