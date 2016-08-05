/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientColModelManager.cpp
*  PURPOSE:     Model collision entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include "StdInc.h"
#include "CClientColModelManager.h"
#include "CClientObjectManager.h"

CClientColModelManager::CClientColModelManager ( class CClientManager* pManager )
{
    // Init
    m_bRemoveFromList = true;
}


CClientColModelManager::~CClientColModelManager ( void )
{
    // Remove all items
    RemoveAll ();
}


void CClientColModelManager::RemoveAll ( void )
{
    // Make sure they don't remove themselves from the list
    m_bRemoveFromList = false;

    // Delete all the items
    std::list < CClientColModel* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_List.clear ();

    // They can now remove themselves again
    m_bRemoveFromList = true;
}


bool CClientColModelManager::Exists ( CClientColModel* pCol )
{
    // Item exists in list?
    std::list < CClientColModel* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Same element?
        if ( pCol == *iter )
        {
            return true;
        }
    }

    // Doesn't exist
    return false;
}


CClientColModel* CClientColModelManager::GetElementThatReplaced ( unsigned short usModel, CClientColModel* pDontSearch )
{
    // Item exists in list?
    std::list < CClientColModel* > ::iterator iter = m_List.begin ();
    for ( ; iter != m_List.end (); iter++ )
    {
        // Has this element replaced the given model?
        if ( *iter != pDontSearch &&
             (*iter)->HasReplaced ( usModel ) )
        {
            // Return the item
            return *iter;
        }
    }

    // Nobody has replaced it
    return NULL;
}


bool CClientColModelManager::IsReplacableModel ( unsigned short usModel )
{
    // We can only replace object collisions atm
    return CClientObjectManager::IsValidModel ( usModel );
}


bool CClientColModelManager::RestoreModel ( unsigned short usModel )
{
    // Grab the item that replaced that model
    CClientColModel* pCol = GetElementThatReplaced ( usModel );
    if ( pCol )
    {
        // Restore it
        pCol->Restore ( usModel );

        // Success
        return true;
    }

    // Failed
    return false;
}


void CClientColModelManager::RemoveFromList ( CClientColModel* pCol )
{
    // Remove it from the list if we can
    if ( m_bRemoveFromList )
    {
        m_List.remove ( pCol );
    }
}
