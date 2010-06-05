/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CEvents.cpp
*  PURPOSE:     Events class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CEvents::CEvents ( void )
{
    m_bWasEventCancelled = false;
    m_bEventCancelled = false;
}


CEvents::~CEvents ( void )
{
    RemoveAllEvents ();

    SExtinctEvent * pEvent = NULL;
    list < SExtinctEvent * > ::iterator iter = m_ExtinctEvents.begin ();
    for ( ; iter != m_ExtinctEvents.end () ; iter++ )
    {
         pEvent = *iter;
         delete [] pEvent->szName;
         delete [] pEvent->szNewName;
         delete pEvent;
    }
}


bool CEvents::AddEvent ( const char* szName, const char* szArguments, CLuaMain* pLuaMain, bool bAllowRemoteTrigger )
{
    assert ( szName );
    assert ( szArguments );

    // If it already exists, return
    if ( Get ( szName ) ) return false;

    // Create and add the event
    SEvent* pEvent = new SEvent;
    pEvent->strName = szName;
    pEvent->strArguments = szArguments;
    pEvent->pLuaMain = pLuaMain;
    pEvent->bAllowRemoteTrigger = bAllowRemoteTrigger;
    m_Events.push_back ( pEvent );

    return true;
}


void CEvents::RemoveEvent ( SEvent* pEvent )
{
    assert ( pEvent );

    // Remove it and delete it
    if ( !m_Events.empty() ) m_Events.remove ( pEvent );
    delete pEvent;
}


void CEvents::RemoveEvent ( const char* szName )
{
    assert ( szName );

    // Find it
    SEvent* pEvent = Get ( szName );
    if ( pEvent )
    {
        // Delete it
        if ( !m_Events.empty() ) m_Events.remove ( pEvent );
        delete pEvent;
    }
}


void CEvents::RemoveAllEvents ( class CLuaMain* pMain )
{
    // Delete all items
    list < SEvent* > ::iterator iter = m_Events.begin ();
    while ( iter != m_Events.end () )
    {
        // If they match, delete it null it and set the bool
        if ( (*iter)->pLuaMain == pMain )
        {
			// Delete the object
            delete *iter;

			// Remove from list
			iter = m_Events.erase(iter);
        }
		else
			++iter;
    }
}


SEvent* CEvents::Get ( const char* szName )
{
    assert ( szName );

    // Find a matching name
    list < SEvent* > ::const_iterator iter = m_Events.begin ();
    for ( ; iter != m_Events.end (); iter++ )
    {
        if ( (*iter)->strName == szName )
        {
            return *iter;
        }
    }

    // None matches
    return NULL;
}


void CEvents::RemoveAllEvents ( void )
{
    // Delete all items
    list < SEvent* > ::const_iterator iter = m_Events.begin ();
    for ( ; iter != m_Events.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Events.clear ();
}

void CEvents::PreEventPulse ( void )
{
    m_CancelledList.push_back ( m_bEventCancelled );
    m_bEventCancelled = false;
    m_bWasEventCancelled = false;
}


void CEvents::PostEventPulse ( void )
{
    m_bWasEventCancelled = m_bEventCancelled;
    m_bEventCancelled = m_CancelledList.back () ? true:false;
    m_CancelledList.pop_back ();
}


void CEvents::CancelEvent ( bool bCancelled )
{
    m_bEventCancelled = bCancelled;
}


bool CEvents::WasEventCancelled ( void )
{
    return m_bWasEventCancelled;
}


void CEvents::AddExtinctEvent ( const char * szName, const char * szNewName )
{
    SExtinctEvent * pEvent = new SExtinctEvent;
    pEvent->szName = new char [ strlen ( szName ) + 1 ];
    strcpy ( pEvent->szName, szName );
    pEvent->szNewName = new char [ strlen ( szNewName ) + 1 ];
    strcpy ( pEvent->szNewName, szNewName );
    m_ExtinctEvents.push_back ( pEvent );
}


bool CEvents::IsExtinctEvent ( const char * szName, char * szNewName, unsigned int uiLength )
{
    SExtinctEvent * pEvent = NULL;
    list < SExtinctEvent * > ::iterator iter = m_ExtinctEvents.begin ();
    for ( ; iter != m_ExtinctEvents.end () ; iter++ )
    {
        pEvent = *iter;
        if ( !strcmp ( pEvent->szName, szName ) )
        {
            strncpy ( szNewName, pEvent->szNewName, uiLength );
            return true;
        }
    }
    return false;
}