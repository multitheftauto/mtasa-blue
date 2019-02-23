/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CEvents.h
 *  PURPOSE:     Events class header
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <list>
#include <vector>

struct SEvent
{
    class CLuaMain* pLuaMain;
    std::string     strName;
    std::string     strArguments;
    bool            bAllowRemoteTrigger;
};

class CEvents
{
public:
    CEvents();
    ~CEvents();

    bool AddEvent(const char* szName, const char* szArguments, class CLuaMain* pLuaMain, bool bAllowRemoteTrigger);
    void RemoveEvent(SEvent* pEvent);
    void RemoveEvent(const char* szName);
    void RemoveAllEvents(class CLuaMain* pMain);

    bool    Exists(const char* szName) { return Get(szName) != NULL; };
    SEvent* Get(const char* szName);

    CFastHashMap<SString, SEvent*>::const_iterator IterBegin() { return m_EventHashMap.begin(); };
    CFastHashMap<SString, SEvent*>::const_iterator IterEnd() { return m_EventHashMap.end(); };

    void PreEventPulse();
    void PostEventPulse();

    void CancelEvent(bool bCancelled = true);
    bool WasEventCancelled();

private:
    void RemoveAllEvents();

    CFastHashMap<SString, SEvent*> m_EventHashMap;
    std::vector<int>               m_CancelledList;
    bool                           m_bEventCancelled;
    bool                           m_bWasEventCancelled;
};
