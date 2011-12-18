/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientGUIManager.cpp
*  PURPOSE:     GUI entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Cecill Etheredge <ijsf@gmx.net>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;
using std::map;

CClientGUIManager::CClientGUIManager ( void )
{
    m_bCanRemoveFromList = true;
}


CClientGUIManager::~CClientGUIManager ( void )
{
    // Destroy all objects
    DeleteAll ();
}


void CClientGUIManager::DeleteAll ( void )
{
    // We must make sure the gui elements don't interrupt our list
    m_bCanRemoveFromList = false;

    // Delete all the gui elements
    list < CClientGUIElement* > ::const_iterator iter = m_Elements.begin ();
    for ( ; iter != m_Elements.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Elements.clear ();
    m_bCanRemoveFromList = true;
}


bool CClientGUIManager::Exists ( CClientGUIElement* pGUIElement )
{
    return m_Elements.Contains ( pGUIElement );
}


bool CClientGUIManager::Exists ( CGUIElement* pCGUIElement )
{
    if ( pCGUIElement ) {
        // Find the object in the list
        list < CClientGUIElement* > ::const_iterator iter = m_Elements.begin ();
        for ( ; iter != m_Elements.end (); iter++ )
        {
            if ( (*iter)->GetCGUIElement () == pCGUIElement )
            {
                return true;
            }
        }
    }

    // Doesn't exist
    return false;
}


CClientGUIElement* CClientGUIManager::Get ( CGUIElement* pCGUIElement )
{
    if ( pCGUIElement ) {
        // Find the object in the list
        list < CClientGUIElement* > ::const_iterator iter = m_Elements.begin ();
        for ( ; iter != m_Elements.end (); iter++ )
        {
            if ( (*iter)->GetCGUIElement () == pCGUIElement )
            {
                return *iter;
            }
        }
    }

    // Doesn't exist
    return NULL;
}


void CClientGUIManager::Add ( CClientGUIElement* pElement )
{
    m_Elements.push_back ( pElement );
}


void CClientGUIManager::Remove ( CClientGUIElement* pGUIElement )
{
    if ( m_bCanRemoveFromList )
    {
        m_Elements.remove ( pGUIElement );
    }
}

void CClientGUIManager::DoPulse ( void )
{
    FlushQueuedUpdates ();
}

void CClientGUIManager::QueueGridListUpdate ( CClientGUIElement *pGUIElement )
{
    ElementID ID = pGUIElement->GetID ();
    if ( m_QueuedGridListUpdates.find ( ID ) == m_QueuedGridListUpdates.end () )
        m_QueuedGridListUpdates[ ID ] = true;
}

void CClientGUIManager::FlushQueuedUpdates ()
{
    map < ElementID, bool >::iterator iter = m_QueuedGridListUpdates.begin ();
    for ( ; iter != m_QueuedGridListUpdates.end () ; ++iter )
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( iter->first );
        if ( pEntity && !pEntity->IsBeingDeleted () && pEntity->GetType () == CCLIENTGUI )
        {
            CClientGUIElement* pGUIElement = static_cast < CClientGUIElement* > ( pEntity );
            if ( pGUIElement && IS_CGUIELEMENT_GRIDLIST ( pGUIElement ) )
            {
                CGUIGridList* pGUIGridList = static_cast < CGUIGridList* > ( pGUIElement->GetCGUIElement () );
                pGUIGridList->ForceUpdate ();
            }
        }
    }
    m_QueuedGridListUpdates.clear ();
}
