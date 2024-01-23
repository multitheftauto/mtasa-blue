/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CEvents.h
 *  PURPOSE:     Event manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <string>
#include <list>
#include <vector>

struct SEvent
{
    std::vector<class CLuaMain*> pLuaMainVector;
    std::string                  strName;
    std::string                  strArguments;
    bool                         bAllowRemoteTrigger;
};

class CEvents
{
public:
    CEvents();
    ~CEvents() { RemoveAllEvents(); };

    bool AddEvent(const char* szName, const char* szArguments, class CLuaMain* pLuaMain, bool bAllowRemoteTrigger);
    void RemoveEvent(SEvent* pEvent);
    void RemoveEvent(const char* szName);
    void RemoveAllEvents(class CLuaMain* pMain);

    bool    Exists(const char* szName) noexcept { return Get(szName) != nullptr; };
    SEvent* Get(const char* szName) noexcept;

    CFastHashMap<SString, SEvent*>::iterator begin() noexcept { return m_EventHashMap.begin(); };
    CFastHashMap<SString, SEvent*>::iterator end() noexcept { return m_EventHashMap.end(); };

    CFastHashMap<SString, SEvent*>::const_iterator begin() const noexcept { return m_EventHashMap.begin(); };
    CFastHashMap<SString, SEvent*>::const_iterator end() const noexcept { return m_EventHashMap.end(); };

    void PreEventPulse();
    void PostEventPulse();

    void        CancelEvent(bool bCancelled = true);
    void        CancelEvent(bool bCancelled, const char* szReason);
    bool        WasEventCancelled() const noexcept;
    const char* GetLastError() const noexcept;

private:
    void RemoveAllEvents();

    CFastHashMap<SString, SEvent*> m_EventHashMap;

    std::vector<int> m_CancelledList;
    bool             m_bEventCancelled;
    bool             m_bWasEventCancelled;

    SString m_strLastError;
};
