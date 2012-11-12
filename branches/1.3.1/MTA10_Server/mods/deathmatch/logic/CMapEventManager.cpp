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

extern CGame* g_pGame;


CMapEventManager::CMapEventManager ( void )
{
    m_bIteratingList = false;
    m_bHasEvents = false;
}


CMapEventManager::~CMapEventManager ( void )
{
    // If this happens while we're iterating the list, we're screwed
    assert ( !m_bIteratingList );

    // These should always be empty when m_bIteratingList is false
    assert ( m_TrashCan.empty () && m_PendingAddList.empty () );

    // Delete all eventhandlers
    DeleteAll ();
}


bool CMapEventManager::Add ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod )
{
    // Check for max name length
    if ( strlen ( szName ) <= MAPEVENT_MAX_LENGTH_NAME )
    {
        // Make a new event
        CMapEvent* pEvent = new CMapEvent ( pLuaMain, szName, iLuaFunction, bPropagated, eventPriority, fPriorityMod );

        // Add now, or when finished iterating list
        if ( m_bIteratingList )
            m_PendingAddList.push_back ( pEvent );
        else
            AddInternal ( pEvent );

        m_bHasEvents = true;
        return true;
    }
    return false;
}


bool CMapEventManager::Delete ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction )
{
    // Delete all the events with matching names
    bool bRemovedSomeone = false;

    EventsIter iter = m_EventsMap.begin ();
    while ( iter != m_EventsMap.end () )
    {
        CMapEvent* pMapEvent = iter->second;

        // Matching VM?
        if ( pLuaMain == pMapEvent->GetVM () )
        {
            // If name supplied, check name and function
            if ( !szName || ( ( strcmp ( pMapEvent->GetName (), szName ) == 0 ) && ( pMapEvent->GetLuaFunction () == iLuaFunction ) ) )
            {
                // Not alredy being destroyed?
                if ( !pMapEvent->IsBeingDestroyed () )
                {
                    // Are we in an event handler?
                    if ( m_bIteratingList )
                    {
                        // Put it in the trashcan
                        pMapEvent->SetBeingDestroyed ( true );
                        m_TrashCan.push_back ( pMapEvent );

                        // Remember that we deleted something
                        bRemovedSomeone = true;
                    }
                    else
                    {
                        // Delete the object
                        delete pMapEvent;

                        // Remove from list and remember that we deleted something
                        m_EventsMap.erase ( iter++ );
                        bRemovedSomeone = true;
                        continue;
                    }
                }
            }
        }

        // Increment iterator
        ++iter;
    }

    // Also check pending adds
    std::list < CMapEvent* >::iterator iterPend = m_PendingAddList.begin ();
    while ( iterPend != m_PendingAddList.end () )
    {
        CMapEvent* pMapEvent = *iterPend;

        // Matching VM?
        if ( pLuaMain == pMapEvent->GetVM () )
        {
            // If name supplied, check name and function
            if ( !szName || ( ( strcmp ( pMapEvent->GetName (), szName ) == 0 ) && ( pMapEvent->GetLuaFunction () == iLuaFunction ) ) )
            {
                // Not alredy being destroyed?
                if ( !pMapEvent->IsBeingDestroyed () )
                {
                    pMapEvent->SetBeingDestroyed ( true );

                    // Move into m_TrashCan ( m_bIteratingList is always true if m_PendingAddList has items )
                    iterPend = m_PendingAddList.erase ( iterPend );
                    m_TrashCan.push_back ( pMapEvent );

                     // Remember that we deleted something
                    bRemovedSomeone = true;
                    continue;
                }
            }
        }

        // Increment iterator
        iterPend++;
    }

    m_bHasEvents = !m_EventsMap.empty ();

    // Return whether we actually destroyed someone or not
    return bRemovedSomeone;
}


void CMapEventManager::DeleteAll ( void )
{
    // Delete all the events
    EventsIter iter = m_EventsMap.begin ();
    while ( iter != m_EventsMap.end () )
    {
        CMapEvent* pMapEvent = iter->second;

        // Delete it if it's not already being destroyed
        if ( !pMapEvent->IsBeingDestroyed () )
        {
            delete pMapEvent;
            m_EventsMap.erase ( iter++ );
        }
        else
            ++iter;
    }
    m_bHasEvents = !m_EventsMap.empty ();
}


bool CMapEventManager::Call ( const char* szName, const CLuaArguments& Arguments, class CElement* pSource, class CElement* pThis, CPlayer* pCaller )
{
    // Check if no events
    if ( !m_bHasEvents )
        return false;

    // Check if no events with a name match
    EventsIterPair itPair = m_EventsMap.equal_range ( szName );
    if ( itPair.first == itPair.second )
        return false;

    // Call all the events with matching names
    bool bCalled = false;
    bool bIsAlreadyIterating = m_bIteratingList;
    m_bIteratingList = true;

    for ( EventsIter iter = itPair.first ; iter != itPair.second ; ++iter )
    {
        CMapEvent* pMapEvent = iter->second;

        // If it's not being destroyed
        if ( !pMapEvent->IsBeingDestroyed () )
        {
            // Compare the names
            assert ( strcmp ( pMapEvent->GetName (), szName ) == 0 );
            {
                // Call if propagated?
                if ( pSource == pThis || pMapEvent->IsPropagated () )
                {
                    // Grab the current VM
                    lua_State* pState = pMapEvent->GetVM ()->GetVM ();
                    #if MTA_DEBUG
                        int luaStackPointer = lua_gettop ( pState );
                    #endif

                    TIMEUS startTime = GetTimeUs();

                    // Store the current values of the globals
                    lua_getglobal ( pState, "source" );
                    CLuaArgument OldSource ( pState, -1 );
                    lua_pop( pState, 1 );

                    lua_getglobal ( pState, "this" );
                    CLuaArgument OldThis ( pState, -1 );
                    lua_pop( pState, 1 );

                    lua_getglobal ( pState, "sourceResource" );
                    CLuaArgument OldResource ( pState, -1 );
                    lua_pop( pState, 1 );

                    lua_getglobal ( pState, "sourceResourceRoot" );
                    CLuaArgument OldResourceRoot ( pState, -1 );
                    lua_pop( pState, 1 );

                    lua_getglobal ( pState, "eventName" );
                    CLuaArgument OldEventName ( pState, -1 );
                    lua_pop( pState, 1 );

                    lua_getglobal ( pState, "client" );
                    CLuaArgument OldClient ( pState, -1 );
                    lua_pop( pState, 1 );

                    // Set the "source", "this", "sourceResource" and "sourceResourceRoot" globals on that VM
                    lua_pushelement ( pState, pSource );
                    lua_setglobal ( pState, "source" );

                    lua_pushelement ( pState, pThis );
                    lua_setglobal ( pState, "this" );

                    lua_pushresource ( pState, pMapEvent->GetVM()->GetResource() );     // This is not correct
                    lua_setglobal ( pState, "sourceResource" );

                    lua_pushelement ( pState, pMapEvent->GetVM()->GetResource()->GetResourceRootElement() );     // This is not correct
                    lua_setglobal ( pState, "sourceResourceRoot" );

                    lua_pushstring ( pState, szName );
                    lua_setglobal ( pState, "eventName" );

                    if ( pCaller )
                    {
                        lua_pushelement ( pState, pCaller );
                        lua_setglobal ( pState, "client" );
                    }
                    else
                    {
                        lua_pushnil ( pState );
                        lua_setglobal ( pState, "client" );
                    }

                    // Call it
                    pMapEvent->Call ( Arguments );
                    bCalled = true;

                    // Reset the globals on that VM
                    OldSource.Push ( pState );
                    lua_setglobal ( pState, "source" );

                    OldThis.Push ( pState );
                    lua_setglobal ( pState, "this" );                

                    OldResource.Push ( pState );
                    lua_setglobal ( pState, "sourceResource" );

                    OldResourceRoot.Push ( pState );
                    lua_setglobal ( pState, "sourceResourceRoot" );

                    OldEventName.Push ( pState );
                    lua_setglobal ( pState, "eventName" );

                    OldClient.Push ( pState );
                    lua_setglobal ( pState, "client" );

                    #if MTA_DEBUG
                        assert ( lua_gettop ( pState ) == luaStackPointer );
                    #endif

                    CPerfStatLuaTiming::GetSingleton ()->UpdateLuaTiming ( pMapEvent->GetVM (), szName, GetTimeUs() - startTime );
                }
            }
        }
    }

    // Clean out the trash if we're no longer calling events.
    if ( !bIsAlreadyIterating )
    {
        TakeOutTheTrash ();

        // We're no longer iterating the list
        m_bIteratingList = false;
    }

    // Return whether we called atleast one func or not
    return bCalled;
}


void CMapEventManager::TakeOutTheTrash ( void )
{
    // Loop through our trashcan deleting every item
    list < CMapEvent* > ::const_iterator iterTrash = m_TrashCan.begin ();
    for ( ; iterTrash != m_TrashCan.end (); iterTrash++ )
    {
        CMapEvent* pMapEvent = *iterTrash;

        // Remove from the eventhandler list
        EventsIter iterMap = m_EventsMap.begin ();
        while ( iterMap != m_EventsMap.end () )
        {
            if ( pMapEvent == iterMap->second )
                m_EventsMap.erase ( iterMap++ );
            else
                ++iterMap;
        }

        // Delete it
        delete pMapEvent;
    }

    // Also apply pending adds
    list < CMapEvent* > ::const_iterator iterPend = m_PendingAddList.begin ();
    for ( ; iterPend != m_PendingAddList.end (); iterPend++ )
    {
        AddInternal ( *iterPend );
    }

    m_bHasEvents = !m_EventsMap.empty ();

    // Clear the trashcan
    m_TrashCan.clear ();
    m_PendingAddList.clear ();
}


bool CMapEventManager::HandleExists ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction )
{
    // Return true if we find an event which matches the handle
    for ( EventsConstIter iter = m_EventsMap.begin () ; iter != m_EventsMap.end (); iter++ )
    {
        CMapEvent* pMapEvent = iter->second;

        // Is it not being destroyed?
        if ( !pMapEvent->IsBeingDestroyed () )
        {
            // Same lua main?
            if ( pMapEvent->GetVM () == pLuaMain )
            {
                // Same name?
                if ( strcmp ( pMapEvent->GetName (), szName ) == 0 )
                {
                    // Same lua function?
                    if ( pMapEvent->GetLuaFunction () == iLuaFunction )
                    {
                        // It exists
                        return true;
                    }
                }
            }
        }
    }

    // Also check pending adds
    for ( std::list < CMapEvent* >::const_iterator iterPend = m_PendingAddList.begin () ; iterPend != m_PendingAddList.end (); iterPend++ )
    {
        CMapEvent* pMapEvent = *iterPend;

        // Is it not being destroyed?
        if ( !pMapEvent->IsBeingDestroyed () )
        {
            // Same lua main?
            if ( pMapEvent->GetVM () == pLuaMain )
            {
                // Same name?
                if ( strcmp ( pMapEvent->GetName (), szName ) == 0 )
                {
                    // Same lua function?
                    if ( pMapEvent->GetLuaFunction () == iLuaFunction )
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


void CMapEventManager::AddInternal ( CMapEvent* pEvent )
{
    // Find place to insert
    EventsIterPair itPair = m_EventsMap.equal_range ( pEvent->GetName () );
    EventsIter iter;
    for ( iter = itPair.first ; iter != itPair.second ; ++iter )
    {
        if ( pEvent->IsHigherPriorityThan ( iter->second ) )
            break;
    }
    // Do insert
    m_EventsMap.insert ( iter, std::pair < SString, CMapEvent* > ( pEvent->GetName (), pEvent ) );
}
