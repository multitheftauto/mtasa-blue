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
    for ( auto& pElement : m_Elements )
    {
        delete pElement;
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
    return Get(pCGUIElement) != nullptr;
}


CClientGUIElement* CClientGUIManager::Get ( CGUIElement* pCGUIElement )
{
    if ( pCGUIElement ) {
        // Find the object in the list
        for (auto& pElement : m_Elements)
        {
            if ( pElement->GetCGUIElement () == pCGUIElement )
            {
                return pElement;
            }
        }
    }

    // Doesn't exist
    return nullptr;
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
    for ( auto& iter : m_QueuedGridListUpdates)
    {
        CClientEntity* pEntity = CElementIDs::GetElement ( iter.first );
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
