/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPathManager.cpp
*  PURPOSE:     Path node entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*****************************************************************************/

#include "StdInc.h"

using std::list;

CClientPathManager::CClientPathManager ( CClientManager* pManager )
{
    // Initialize members
    m_pManager = pManager;

    // Create the path node list
    m_bRemoveFromList = true;
}


CClientPathManager::~CClientPathManager ( void )
{
    // Destroy all path nodes
    DeleteAll ();
}


void CClientPathManager::DeleteAll ( void )
{
    // Delete all nodes in the list
    m_bRemoveFromList = false;

    for ( auto& pPath : m_PathNodeList )
    {
        delete pPath;
    }

    // Clear the list
    m_PathNodeList.clear ();
    m_bRemoveFromList = true;
}


void CClientPathManager::DoPulse ( void )
{
    for (auto& pPath : m_PathNodeList)
    {
        pPath->DoPulse ();
    }
}


void CClientPathManager::LinkNodes ( void )
{
    // Link our nodes with the IDs
    for (auto& pPath : m_PathNodeList) 
    {
        // Grab the ID of the next node
        ElementID NextNodeID = pPath->m_NextNodeID;

        // If it was set
        if ( NextNodeID != INVALID_ELEMENT_ID )
        {
            // Try to find and assign it from the rest
            for (auto& pPath2 : m_PathNodeList) 
            {
                if ( pPath2 != pPath && pPath2->GetID () == NextNodeID )
                {
                    pPath2->SetPreviousNode ( pPath );
                    pPath->SetNextNode ( pPath2 );
                }
            }
        }
    }
}


void CClientPathManager::ReverseNodes ( CClientPathNode* pPathNode )
{
    // Get the first in this chain, or use this one if its a loop
    CClientPathNode *pFirstNode = NULL, *pTempNode = pPathNode;
    do
    {
        pFirstNode = pPathNode;
        pPathNode = pPathNode->GetPreviousNode ();
    } while ( pPathNode && pPathNode != pTempNode );

    // Go through the chain untill we've reached an end or looped, and reverse
    pTempNode = pFirstNode;
    do
    {
        pTempNode = pFirstNode->GetNextNode ();
        pFirstNode->SetNextNode ( pFirstNode->GetPreviousNode () );
        pFirstNode->SetPreviousNode ( pTempNode );
        pFirstNode = pTempNode;
    } while ( pTempNode && pTempNode != pFirstNode );
}


bool CClientPathManager::DetachEntity ( CClientEntity* pEntity )
{
    // Loop through the nodes
    for (auto& pPath : m_PathNodeList)
    {
        // Loop through each node's attached entities
        for (auto& pAttachedEntity : pPath->GetAttachedEntities() )
        {
            // If we found it, it should only be on one, so detach and return
            if ( pAttachedEntity == pEntity )
            { 
                pPath->DetachEntity ( pEntity );
                return true;
            }
        }
    }

    return false;
}


CClientPathNode* CClientPathManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTPATHNODE )
    {
        return static_cast < CClientPathNode* > ( pEntity );
    }

    return NULL;
}


void CClientPathManager::RemoveFromList ( CClientPathNode* pPathNode )
{
    if ( m_bRemoveFromList )
    {
        if ( !m_PathNodeList.empty() ) 
            m_PathNodeList.remove ( pPathNode );
    }
}
