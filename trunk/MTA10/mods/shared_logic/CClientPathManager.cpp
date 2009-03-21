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

    list < CClientPathNode* > ::const_iterator iter = m_PathNodeList.begin ();
    for ( ; iter != m_PathNodeList.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_PathNodeList.clear ();
    m_bRemoveFromList = true;
}


void CClientPathManager::DoPulse ( void )
{
    list < CClientPathNode* > ::const_iterator iter = m_PathNodeList.begin ();
    for ( ; iter != m_PathNodeList.end () ; iter++ )
    {
        (*iter)->DoPulse ();
    }
}


void CClientPathManager::LinkNodes ( void )
{
    // Link our nodes with the IDs
    list < CClientPathNode* > ::const_iterator iterList = m_PathNodeList.begin ();
    for ( ; iterList != m_PathNodeList.end () ; iterList++ )
    {
        // Grab the ID of the next node
        ElementID NextNodeID = (*iterList)->m_NextNodeID;

        // If it was set
        if ( NextNodeID != INVALID_ELEMENT_ID )
        {
            // Try to find and assign it from the rest
            list < CClientPathNode* > ::const_iterator iter = m_PathNodeList.begin ();
            for ( ; iter != m_PathNodeList.end () ; iter++ )
            {
                if ( (*iter) != (*iterList) && (*iter)->GetID () == NextNodeID )
                {
                    (*iter)->SetPreviousNode ( *iterList );
                    (*iterList)->SetNextNode ( *iter );
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
    list < CClientPathNode* > ::const_iterator iterList = m_PathNodeList.begin ();
    for ( ; iterList != m_PathNodeList.end (); iterList++ )
    {
        // Loop through each node's attached entities
        list < CClientEntity* > ::const_iterator iter = (*iterList)->AttachedIterBegin ();
        for ( ; iter != (*iterList)->AttachedIterEnd (); iter++ )
        {
            // If we found it, it should only be on one, so detach and return
            if ( *iter == pEntity )
            { 
                (*iterList)->DetachEntity ( pEntity );
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
        if ( !m_PathNodeList.empty() ) m_PathNodeList.remove ( pPathNode );
    }
}
