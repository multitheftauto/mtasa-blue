/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEventManager.h
*  PURPOSE:     Map event manager class header
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CMAPEVENTMANAGER_H
#define __CMAPEVENTMANAGER_H

#include "lua/CLuaArguments.h"
#include "CMapEvent.h"
#include <list>

class CMapEventManager
{
public:
                            CMapEventManager                ( void );
                            ~CMapEventManager               ( void );

    bool                    Add                             ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod );
    bool                    Delete                          ( CLuaMain* pLuaMain, const char* szName = NULL, const CLuaFunctionRef& iLuaFunction = CLuaFunctionRef () );
    void                    DeleteAll                       ( void );
    bool                    HandleExists                    ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction );
    bool                    HasEvents                       ( void ) const          { return m_bHasEvents; }

    bool                    Call                            ( const char* szName, const CLuaArguments& Arguments, class CClientEntity* pSource, class CClientEntity* pThis );

private:
    void                    TakeOutTheTrash                 ( void );
    void                    AddInternal                     ( CMapEvent* pEvent );

    bool                                    m_bHasEvents;
    bool                                    m_bIteratingList;
    std::multimap < SString, CMapEvent* >   m_EventsMap;
    std::list < CMapEvent* >                m_TrashCan;

    // Types for m_EventsMap access
    typedef std::multimap < SString, CMapEvent* > ::const_iterator  EventsConstIter;
    typedef std::multimap < SString, CMapEvent* > ::iterator        EventsIter;
    typedef std::pair < EventsIter, EventsIter >                    EventsIterPair;
};

#endif
