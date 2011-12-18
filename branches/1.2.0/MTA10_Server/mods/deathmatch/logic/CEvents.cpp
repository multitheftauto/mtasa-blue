/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CEvents.cpp
*  PURPOSE:     Event manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <>
*               Jax <>
*               lil_Toady <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CEvents::CEvents ( void )
{
    m_bWasEventCancelled = false;
    m_bEventCancelled = false;
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
            iter = m_Events.erase ( iter );
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
    m_strLastError = "";
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

void CEvents::CancelEvent ( bool bCancelled, const char* szReason )
{
    m_bEventCancelled = bCancelled;
    m_strLastError = SStringX ( szReason );
}

bool CEvents::WasEventCancelled ( void )
{
    return m_bWasEventCancelled;
}

const char* CEvents::GetLastError ( void )
{
    return m_strLastError;
}

