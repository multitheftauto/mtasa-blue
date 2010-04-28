/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientDFFManager.cpp
*  PURPOSE:     .dff model manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"

CClientDFFManager::CClientDFFManager ( CClientManager* pManager )
{
    // Init
    m_pObjectManager = pManager->GetObjectManager ();
    m_pVehicleManager = pManager->GetVehicleManager ();
    m_bRemoveFromList = true;
}


CClientDFFManager::~CClientDFFManager ( void )
{
    // Delete all our DFF's
    RemoveAll ();
}


void CClientDFFManager::RemoveAll ( void )
{
    // Make sure they don't remove themselves from our list
    m_bRemoveFromList = false;

    // Run through our list deleting the DFF's
    std::list < CClientDFF* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Allow list removal again
    m_bRemoveFromList = true;
}


bool CClientDFFManager::Exists ( CClientDFF* pDFF )
{
    // Matches given DFF?
    std::list < CClientDFF* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Match?
        if ( pDFF == *iter )
        {
            // It exists
            return true;
        }
    }

    // It doesn't
    return false;
}


CClientDFF* CClientDFFManager::GetElementThatReplaced  ( unsigned short usModel, CClientDFF* pDontSearch )
{
    // Matches given DFF?
    std::list < CClientDFF* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Match?
        if ( *iter != pDontSearch &&
             (*iter)->HasReplaced ( usModel ) )
        {
            // It exists
            return *iter;
        }
    }

    // None
    return NULL;
}


bool CClientDFFManager::IsReplacableModel ( unsigned short usModel )
{
    // Either a vehicle model or an object model
    return CClientObjectManager::IsValidModel ( usModel ) ||
           CClientVehicleManager::IsValidModel ( usModel );
}


bool CClientDFFManager::RestoreModel ( unsigned short usModel )
{
    // Get the DFF file that replaced it
    CClientDFF* pDFF = GetElementThatReplaced ( usModel );
    if ( pDFF )
    {
        // Restore it
        pDFF->RestoreModel ( usModel );

        // Success
        return true;
    }

    // Nothing to restore
    return false;
}


void CClientDFFManager::RemoveFromList ( CClientDFF* pDFF )
{
    // Can we remove anything from the list?
    if ( m_bRemoveFromList )
    {
        m_List.remove ( pDFF );
    }
}
