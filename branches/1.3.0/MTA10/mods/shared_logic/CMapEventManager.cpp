/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CMapEventManager.cpp
*  PURPOSE:     Map event manager class
*  DEVELOPERS:  Jax <>
*               Kevin Whiteside <kevuwk@gmail.com>
*               Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CMapEventManager::CMapEventManager ( void )
{
    m_bIteratingList = false;
}


CMapEventManager::~CMapEventManager ( void )
{
    // If this happens while we're iterating the list, we're screwed
    assert ( !m_bIteratingList );

    // Delete all eventhandlers
    DeleteAll ();

    // Take out the trash
    TakeOutTheTrash ();
}


bool CMapEventManager::Add ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction, bool bPropagated, EEventPriorityType eventPriority, float fPriorityMod )
{
    // Check for max name length
    if ( strlen ( szName ) <= MAPEVENT_MAX_LENGTH_NAME )
    {
        // Make a new event
        CMapEvent* pEvent = new CMapEvent ( pLuaMain, szName, iLuaFunction, bPropagated, eventPriority, fPriorityMod );
        // Find place to insert
        for ( std::list < CMapEvent* > ::iterator iter = m_Events.begin () ; iter != m_Events.end (); iter++ )
        {
            if ( pEvent->IsHigherPriorityThan ( *iter ) )
            {
                m_Events.insert ( iter, pEvent );
                return true;
            }
        }
        m_Events.push_back ( pEvent );
        return true;
    }
    return false;
}


bool CMapEventManager::Delete ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction )
{
    // Delete all the events with matching names
    bool bRemovedSomeone = false;
    CMapEvent* pMapEvent = NULL;
    list < CMapEvent* > ::iterator iter = m_Events.begin ();
    while ( iter != m_Events.end () )
    {
        pMapEvent = *iter;

        // Matching VM and name?
        if ( pLuaMain == pMapEvent->GetVM () &&
             strcmp ( pMapEvent->GetName (), szName ) == 0 )
        {
            // Same lua function?
            if ( pMapEvent->GetLuaFunction () == iLuaFunction )
            {
                // Not alredy being destroyed?
                if ( !pMapEvent->IsBeingDestroyed () )
                {
                    // Are we in an event handler?
                    if ( m_bIteratingList )
                    {
                        // Put it in the trashcan
                        pMapEvent->SetBeingDestroyed ( true );
                        m_TrashCan.push_back ( *iter );

                        // Remember that we deleted something
                        bRemovedSomeone = true;
                    }
                    else
                    {
                        // Delete the object
                        delete pMapEvent;

                        // Remove from list and remember that we deleted something
                        iter = m_Events.erase ( iter );
                        bRemovedSomeone = true;
                        continue;
                    }
                }
            }
        }

        // Increment iterator
        iter++;
    }

    // Return whether we actually destroyed someone or not
    return bRemovedSomeone;
}


void CMapEventManager::Delete ( CMapEvent* pEvent )
{
    // If it's not already destroyed
    if ( !pEvent->IsBeingDestroyed () )
    {
        // Are we in an eventhandler or something?
        if ( m_bIteratingList )
        {
            // Add it to the trashcan
            pEvent->SetBeingDestroyed ( true );
            m_TrashCan.push_back ( pEvent );
        }
        else
        {
            // Remove from list and delete
            m_Events.remove ( pEvent );
            delete pEvent;
        }
    }
}


void CMapEventManager::Delete ( CLuaMain* pLuaMain )
{
    // Delete all the events with matching lua VM's
    CMapEvent* pMapEvent = NULL;
    list < CMapEvent* > ::iterator iter = m_Events.begin ();
    while ( iter != m_Events.end () )
    {
        pMapEvent = *iter;

        // Maching VM's?
        if ( pMapEvent->GetVM () == pLuaMain )
        {
            // Not already being destroyed?
            if ( !pMapEvent->IsBeingDestroyed () )
            {
                // Are we iterating the list?
                if ( m_bIteratingList )
                {
                    // Add it to trashcan and mark as being destroyed
                    pMapEvent->SetBeingDestroyed ( true );
                    m_TrashCan.push_back ( pMapEvent );
                }
                else
                {
                    // Delete the event and continue from where we left.
                    // Remember to subtract from our iterator or we'll delete only every other element.
                    delete pMapEvent;
                    iter = m_Events.erase ( iter );
                    continue;
                }
            }
        }

        // Increment iterator
        ++iter;
    }
}


void CMapEventManager::DeleteAll ( void )
{
    // Delete all the events
    list < CMapEvent* > ::iterator iter = m_Events.begin ();
    while ( iter != m_Events.end () )
    {
        // Delete it if it's not already being destroyed
        if ( !(*iter)->IsBeingDestroyed () )
        {
            delete *iter;
            iter = m_Events.erase ( iter );
            continue;
        }

        // Increment iterator
        ++iter;
    }
}


bool CMapEventManager::Exists ( CMapEvent* pEvent )
{
    // Return true if we find it in the list
    list < CMapEvent* > ::const_iterator iter = m_Events.begin ();
    for ( ; iter != m_Events.end (); iter++ )
    {
        // Compare the pointers
        if ( *iter == pEvent )
        {
            // Return true if it's not being destroyed
            return !pEvent->IsBeingDestroyed ();
        }
    }

    // Doesn't exist
    return false;
}


CMapEvent* CMapEventManager::Get ( const char* szName )
{
    // Return it if we find it in the list
    list < CMapEvent* > ::const_iterator iter = m_Events.begin ();
    for ( ; iter != m_Events.end (); iter++ )
    {
        // Compare the names
        if ( strcmp ( (*iter)->GetName (), szName ) == 0 )
        {
            // Return it if it's not being destroyed
            if ( !(*iter)->IsBeingDestroyed () )
            {
                return *iter;
            }
        }
    }

    // Doesn't exist
    return NULL;
}


bool CMapEventManager::Call ( const char* szName, const CLuaArguments& Arguments, class CClientEntity* pSource, class CClientEntity* pThis )
{
    // Check for multi-threading slipups
    assert ( IsMainThread () );

    // Call all the events with matching names
    bool bCalled = false;
    CMapEvent* pMapEvent;
    bool bIsAlreadyIterating = m_bIteratingList;
    m_bIteratingList = true;
    list < CMapEvent* > ::const_iterator iter = m_Events.begin ();
    for ( ; iter != m_Events.end (); iter++ )
    {
        pMapEvent = *iter;

        // If it's not being destroyed
        if ( !pMapEvent->IsBeingDestroyed () )
        {
            // Compare the names
            if ( strcmp ( pMapEvent->GetName (), szName ) == 0 )
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

                    // Record event for the crash dump writer
                    g_pCore->LogEvent ( 405, "Lua Event", pMapEvent->GetVM ()->GetScriptNamePointer (), szName );

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

                    // Set the "source", "this", "sourceResource" and the "sourceResourceRoot" globals on that VM
                    lua_pushelement ( pState, pSource );
                    lua_setglobal ( pState, "source" );

                    lua_pushelement ( pState, pThis );
                    lua_setglobal ( pState, "this" );

                    lua_pushresource ( pState, pMapEvent->GetVM()->GetResource() );
                    lua_setglobal ( pState, "sourceResource" );

                    lua_pushelement ( pState, pMapEvent->GetVM()->GetResource()->GetResourceDynamicEntity() );
                    lua_setglobal ( pState, "sourceResourceRoot" );

                    lua_pushstring ( pState, szName );
                    lua_setglobal ( pState, "eventName" );
                    
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

                    #if MTA_DEBUG
                        assert ( lua_gettop ( pState ) == luaStackPointer );
                    #endif

                    CClientPerfStatLuaTiming::GetSingleton ()->UpdateLuaTiming ( pMapEvent->GetVM (), szName, GetTimeUs() - startTime );
                }
            }
        }
    }

    // Clean out the trash if we're no longer calling events.
    if ( !bIsAlreadyIterating )\
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
    list < CMapEvent* > ::const_iterator iter = m_TrashCan.begin ();
    for ( ; iter != m_TrashCan.end (); iter++ )
    {
        // Delete it
        delete *iter;

        // Remove it from the eventhandler list.
        // NOTE: This is not the same list as we're iterating now. Hence use "remove"
        m_Events.remove ( *iter );
    }

    // Clear the trashcan
    m_TrashCan.clear ();
}


bool CMapEventManager::HandleExists ( CLuaMain* pLuaMain, const char* szName, const CLuaFunctionRef& iLuaFunction )
{
    // Return true if we find an event which matches the handle
    list < CMapEvent* > ::const_iterator iter = m_Events.begin ();
    for ( ; iter != m_Events.end (); iter++ )
    {
        CMapEvent* pMapEvent = *iter;

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
