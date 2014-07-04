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

    bool                    Call                            ( const char* szName, const CLuaArguments& Arguments, class CElement* pSource, class CElement* pThis, class CPlayer* pCaller = NULL );

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
