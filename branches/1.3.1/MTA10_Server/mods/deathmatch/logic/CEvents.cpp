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

    m_EventHashMap [ szName ] = pEvent;

    return true;
}


void CEvents::RemoveEvent ( SEvent* pEvent )
{
    assert ( pEvent );

    // Remove it and delete it
    MapRemove ( m_EventHashMap, pEvent->strName );
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
        MapRemove ( m_EventHashMap, pEvent->strName );
        delete pEvent;
    }
}


void CEvents::RemoveAllEvents ( class CLuaMain* pMain )
{
    // Delete all items
    CFastHashMap < SString, SEvent* > ::iterator iter = m_EventHashMap.begin ();
    while ( iter != m_EventHashMap.end () )
    {
        SEvent * pEvent = (*iter).second;
        // If they match, delete it null it and set the bool
        if ( pEvent->pLuaMain == pMain )
        {
            // Delete the object
            delete pEvent;

            // Remove from list
            m_EventHashMap.erase ( iter++ );
        }
        else
            ++iter;
    }
}


SEvent* CEvents::Get ( const char* szName )
{
    assert ( szName );

    SEvent ** pEvent = MapFind ( m_EventHashMap, szName );
    if ( pEvent != NULL )
    {
        return *pEvent;
    }
    return NULL;
}


void CEvents::RemoveAllEvents ( void )
{
    // Delete all items
    CFastHashMap < SString, SEvent* > ::const_iterator iter = m_EventHashMap.begin ();
    for ( ; iter != m_EventHashMap.end (); iter++ )
    {
        SEvent * pEvent = (*iter).second;
        delete pEvent;
    }

    // Clear the list
    m_EventHashMap.clear ();
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

