/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CMapEventManager.h
*  PURPOSE:     Map event manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/
#pragma once

#include "lua/CLuaArguments.h"
#include "CMapEvent.h"
#include <list>

class CMapEventManager
{
public:
    ~CMapEventManager();

    bool Add(CLuaMain* pLuaMain, const std::string& strName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod);
    bool Delete(CLuaMain* pLuaMain, const std::string& strName, const CLuaFunctionRef& iLuaFunction);
    bool Delete(CLuaMain* pLuaMain, const std::string& strName);
    bool Delete(CLuaMain* pLuaMain);
    void DeleteAll();
    bool HandleExists(CLuaMain* pLuaMain, const std::string& strName, const CLuaFunctionRef& iLuaFunction);
    bool HasEvents() const { return m_bHasEvents; }
    void GetHandles(CLuaMain* pLuaMain, const std::string& strName, lua_State* luaVM);
#ifdef MTA_CLIENT
    bool Call(const std::string& strName, const CLuaArguments& Arguments, class CClientEntity* pSource, class CClientEntity* pThis, class CClientPlayer* pCaller = NULL);
#else
    bool Call(const std::string& strName, const CLuaArguments& Arguments, class CElement* pSource, class CElement* pThis, class CPlayer* pCaller = NULL);
#endif
private:
    // Types for m_EventsMap access
    using EventsConstIter = std::multimap<std::string, std::unique_ptr<CMapEvent>>::const_iterator;
    using EventsIter = std::multimap<std::string, std::unique_ptr<CMapEvent>>::iterator;
    using EventsIterPair = std::pair<EventsIter, EventsIter>;

    template <typename Predicate>
    bool Delete(Predicate& pred)
    {
        bool bRemovedSomeone = false;
        auto iter = m_EventsMap.begin();
        while (iter != m_EventsMap.end())
        {
            if (!pred(iter->second))
            {
                ++iter;
                continue;
            }
            bRemovedSomeone = true;
            iter = Erase(iter);
        }
        m_bHasEvents = !m_EventsMap.empty();

        // Return whether we actually destroyed someone or not
        return bRemovedSomeone;
    }

    void TakeOutTheTrash();
    EventsIter Erase(EventsIter iter);

    bool m_bHasEvents = false;
    bool m_bIteratingList = false;
    std::multimap<std::string, std::unique_ptr<CMapEvent>> m_EventsMap;
    std::list<EventsIter> m_TrashCan;
};
