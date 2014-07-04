/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CPerPlayerEntity.cpp
*  PURPOSE:     Per-player entity linking class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CPerPlayerEntity::CPerPlayerEntity ( CElement* pParent, CXMLNode* pNode ) : CElement ( pParent, pNode )
{
    m_bIsSynced = false;
    AddVisibleToReference ( g_pGame->GetMapManager ()->GetRootElement () );
};

CPerPlayerEntity::~CPerPlayerEntity ( void )
{
    // Unsync us from everyone
    //Sync ( false );

    // Unreference us from what we're referencing
    list < CElement* > ::const_iterator iter = m_ElementReferences.begin ();
    for ( ; iter != m_ElementReferences.end (); iter++ )
    {
        if ( !(*iter)->m_ElementReferenced.empty() ) (*iter)->m_ElementReferenced.remove ( this );
    }
}


bool CPerPlayerEntity::Sync ( bool bSync )
{
    // Are we getting synced but not already synced or vice versa?
    if ( bSync != m_bIsSynced )
    {
        // Create it for everyone we're visible if it's synced, otherwise destroy
        if ( bSync )
        {
            m_bIsSynced = true;
            CreateEntity ( NULL );
        }
        else
        {
            DestroyEntity ( NULL );
            m_bIsSynced = false;
        }
    }

    return true;
}


void CPerPlayerEntity::OnReferencedSubtreeAdd ( CElement* pElement )
{
    assert ( pElement );

    // Add all players below that item to our list
    AddPlayersBelow ( pElement, m_PlayersAdded );
}


void CPerPlayerEntity::OnReferencedSubtreeRemove ( CElement* pElement )
{
    assert ( pElement );

    // Remove all players below that item from our list
    RemovePlayersBelow ( pElement, m_PlayersRemoved );
}


void CPerPlayerEntity::UpdatePerPlayer ( void )
{
    // Remove entries that match in both added and removed lists
    RemoveIdenticalEntries ( m_PlayersAdded, m_PlayersRemoved );

    // Delete us for every player in our deleted list
    list < CPlayer* > ::const_iterator iter = m_PlayersRemoved.begin ();
    for ( ; iter != m_PlayersRemoved.end (); iter++ )
    {
        DestroyEntity ( *iter );
    }

    // Add us for every player in our added list
    iter = m_PlayersAdded.begin ();
    for ( ; iter != m_PlayersAdded.end (); iter++ )
    {
        CreateEntity ( *iter );
    }

    // Clear both lists
    m_PlayersAdded.clear ();
    m_PlayersRemoved.clear ();
}


bool CPerPlayerEntity::AddVisibleToReference ( CElement* pElement )
{
    // If he isn't already referencing this element
    if ( !IsVisibleToReferenced ( pElement ) )
    {
        // Add him to our list and add us to his list
        m_ElementReferences.push_back ( pElement );
        pElement->m_ElementReferenced.push_back ( this );

        // Add the reference to it and update the players
        OnReferencedSubtreeAdd ( pElement );
        UpdatePerPlayerEntities ();

        return true;
    }

    return false;
}


bool CPerPlayerEntity::RemoveVisibleToReference ( CElement* pElement )
{
    // We reference this element?
    if ( IsVisibleToReferenced ( pElement ) )
    {
        // Remove him from our list and unreference us from his list
        if ( !m_ElementReferences.empty() ) m_ElementReferences.remove ( pElement );
        if ( !pElement->m_ElementReferenced.empty() ) pElement->m_ElementReferenced.remove ( this );

        // Update the players
        OnReferencedSubtreeRemove ( pElement );
        UpdatePerPlayerEntities ();

        return true;
    }

    return false;
}


void CPerPlayerEntity::ClearVisibleToReferences ( void )
{
    // For each reference in our list
    bool bCleared = false;
    list < CElement* > ::const_iterator iter = m_ElementReferences.begin ();
    for ( ; iter != m_ElementReferences.end (); iter++ )
    {
        // Unreference us from it
        if ( !(*iter)->m_ElementReferenced.empty() ) (*iter)->m_ElementReferenced.remove ( this );

        // Notify our inherits that he was removed
        OnReferencedSubtreeRemove ( *iter );
        bCleared = true;
    }

    // Clear the list and update the players
    if ( bCleared )
    {
        m_ElementReferences.clear ();
        UpdatePerPlayerEntities ();
    }
}


bool CPerPlayerEntity::IsVisibleToReferenced ( CElement* pElement )
{
    list < CElement* > ::const_iterator iter = m_ElementReferences.begin ();
    for ( ; iter != m_ElementReferences.end (); iter++ )
    {
        if ( *iter == pElement )
        {
            return true;
        }
    }

    return false;
}


bool CPerPlayerEntity::IsVisibleToPlayer ( CPlayer& Player )
{
    // Return true if we're visible to the given player
    list < CPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( *iter == &Player )
        {
            return true;
        }
    }

    return false;
}


void CPerPlayerEntity::CreateEntity ( CPlayer* pPlayer )
{
    // Are we visible?
    if ( m_bIsSynced )
    {
        // Create the add entity packet
        CEntityAddPacket Packet;
        Packet.Add ( this );

        // Send it to the player if available, if not everyone
        if ( pPlayer )
        {
            // Only send it to him if we can
            if ( !pPlayer->DoNotSendEntities () )
            {
                //CLogger::DebugPrintf ( "Created %u (%s) for %s\n", GetID (), GetName (), pPlayer->GetNick () );
                pPlayer->Send ( Packet );
            }
        }
        else
        {
            //CLogger::DebugPrintf ( "Created %u (%s) for everyone (%u)\n", GetID (), GetName (), m_Players.size () );
            BroadcastOnlyVisible ( Packet );
        }
    }
}


void CPerPlayerEntity::DestroyEntity ( CPlayer* pPlayer )
{
    // Are we visible?
    if ( m_bIsSynced )
    {
        // Create the remove entity packet
        CEntityRemovePacket Packet;
        Packet.Add ( this );

        // Send it to the player if available, if not everyone
        if ( pPlayer )
        {
            // Only send it to him if we can
            if ( !pPlayer->DoNotSendEntities () )
            {
                pPlayer->Send ( Packet );
                //CLogger::DebugPrintf ( "Destroyed %u (%s) for %s\n", GetID (), GetName (), pPlayer->GetNick () );
            }
        }
        else
        {
            //CLogger::DebugPrintf ( "Destroyed %u (%s) for everyone (%u)\n", GetID (), GetName (), m_Players.size () );
            BroadcastOnlyVisible ( Packet );
        }
    }
}


void CPerPlayerEntity::BroadcastOnlyVisible ( const CPacket& Packet )
{
    // Are we synced? (if not we're not visible to anybody)
    if ( m_bIsSynced )
    {
        // Send it to all players we're visible to
        list < CPlayer* > ::const_iterator iter = m_Players.begin ();
        for ( ; iter != m_Players.end (); iter++ )
        {
            (*iter)->Send ( Packet );
        }
    }
}


void CPerPlayerEntity::RemoveIdenticalEntries ( list < CPlayer* >& List1, list < CPlayer* >& List2 )
{
    // NULL all matching entries
    bool bRemoved = false;
    list < CPlayer* > ::iterator iter1 = List1.begin ();
    list < CPlayer* > ::iterator iter2;
    for ( ; iter1 != List1.end (); iter1++ )
    {
        for ( iter2 = List2.begin (); iter2 != List2.end (); iter2++ )
        {
            if ( *iter1 == *iter2 )
            {
                *iter1 = NULL;
                *iter2 = NULL;
            }
        }
    }

    // If we removed some, remove the NULL entries in both lists
    if ( bRemoved )
    {
        List1.remove ( NULL );
        List2.remove ( NULL );
    }
}


void CPerPlayerEntity::AddPlayersBelow ( CElement* pElement, list < CPlayer* >& Added )
{
    assert ( pElement );

    // Is this a player?
    if ( IS_PLAYER ( pElement ) )
    {
        // Are we not already visible to that player? Add it to the list
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        if ( !IsVisibleToPlayer ( *pPlayer ) )
        {
            Added.push_back ( pPlayer );
        }

        // Add it to our reference list
        AddPlayerReference ( pPlayer );
    }

    // Call ourself on all its children elements
    list < CElement* > ::const_iterator iterChildren = pElement->IterBegin ();
    for ( ; iterChildren != pElement->IterEnd (); iterChildren++ )
    {
        AddPlayersBelow ( *iterChildren, Added );
    }
}


void CPerPlayerEntity::RemovePlayersBelow ( CElement* pElement, list < CPlayer* >& Removed )
{
    assert ( pElement );

    // Is this a player?
    if ( IS_PLAYER ( pElement ) )
    {
        // Remove the reference
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        RemovePlayerReference ( pPlayer );

        // Did we just loose the last reference to that player? Add him to the list over removed players.
        if ( !IsVisibleToPlayer ( *pPlayer ) )
        {
            Removed.push_back ( pPlayer );
        }
    }

    // Call ourself on all our children
    list < CElement* > ::const_iterator iterChildren = pElement->IterBegin ();
    for ( ; iterChildren != pElement->IterEnd (); iterChildren++ )
    {
        RemovePlayersBelow ( *iterChildren, Removed );
    }
}


void CPerPlayerEntity::RemovePlayerReference ( CPlayer* pPlayer )
{
    assert ( pPlayer );

    // Find him and remove only that item
    list < CPlayer* > ::iterator iter = m_Players.begin ();
    while ( iter != m_Players.end () )
    {
        if ( *iter == pPlayer )
        {
            iter = m_Players.erase ( iter );
        }
        else
        {
            iter++;
        }
    }
}
