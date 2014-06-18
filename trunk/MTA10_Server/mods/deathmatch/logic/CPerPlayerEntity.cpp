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

std::set < CPerPlayerEntity* > CPerPlayerEntity::ms_AllPerPlayerEntityMap;


CPerPlayerEntity::CPerPlayerEntity ( CElement* pParent, CXMLNode* pNode ) : CElement ( pParent, pNode )
{
    MapInsert ( ms_AllPerPlayerEntityMap, this );
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
        (*iter)->m_ElementReferenced.remove ( this );
    }
    MapRemove ( ms_AllPerPlayerEntityMap, this );
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
    if ( m_PlayersAdded.empty () && m_PlayersRemoved.empty () )    // This check reduces cpu usage when loading large maps (due to recursion)
        return;

    // Remove entries that match in both added and removed lists
    RemoveIdenticalEntries ( m_PlayersAdded, m_PlayersRemoved );

    // Delete us for every player in our deleted list
    std::set < CPlayer* > ::const_iterator iter = m_PlayersRemoved.begin ();
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
        m_ElementReferences.remove ( pElement );
        pElement->m_ElementReferenced.remove ( this );

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
        (*iter)->m_ElementReferenced.remove ( this );

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
    return MapContains( m_Players, &Player );
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
        CPlayerManager* pPlayerManager = g_pGame->GetPlayerManager();
        for ( std::set < CPlayer* >::iterator iter = m_Players.begin() ; iter != m_Players.end() ; )
        {
            if ( !pPlayerManager->Exists( *iter ) )
            {
                // Why does this happen?
                // CLogger::ErrorPrintf( "CPerPlayerEntity removed invalid player from list: %08x", *iter );
                m_Players.erase( iter++ );
            }
            else
                ++iter;
        }

        // Send it to all players we're visible to
        CPlayerManager::Broadcast ( Packet, m_Players );
    }
}


void CPerPlayerEntity::RemoveIdenticalEntries ( std::set < CPlayer* >& List1, std::set < CPlayer* >& List2 )
{
    std::vector < CPlayer* > dupList;

    // Make list of dups
    for ( std::set < CPlayer* > ::iterator it = List1.begin (); it != List1.end (); it++ )
        if ( MapContains( List2, *it ) )
            dupList.push_back( *it );

    // Remove dups from both lists
    for ( std::vector < CPlayer* > ::iterator it = dupList.begin (); it != dupList.end (); it++ )
    {
        MapRemove( List1, *it );
        MapRemove( List2, *it );
    }
}


void CPerPlayerEntity::AddPlayersBelow ( CElement* pElement, std::set < CPlayer* >& Added )
{
    assert ( pElement );

    // Is this a player?
    if ( IS_PLAYER ( pElement ) )
    {
        // Are we not already visible to that player? Add it to the list
        CPlayer* pPlayer = static_cast < CPlayer* > ( pElement );
        if ( !IsVisibleToPlayer ( *pPlayer ) )
        {
            MapInsert( Added, pPlayer );
        }

        // Add it to our reference list
        AddPlayerReference ( pPlayer );
    }

    // Call ourself on all its children elements
    CChildListType ::const_iterator iterChildren = pElement->IterBegin ();
    for ( ; iterChildren != pElement->IterEnd (); iterChildren++ )
    {
        CElement* pElement = *iterChildren;
        if ( pElement->CountChildren () || IS_PLAYER ( pElement ) )    // This check reduces cpu usage when loading large maps (due to recursion)
            AddPlayersBelow ( pElement, Added );
    }
}


void CPerPlayerEntity::RemovePlayersBelow ( CElement* pElement, std::set < CPlayer* >& Removed )
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
            MapInsert( Removed, pPlayer );
        }
    }

    // Call ourself on all our children
    CChildListType ::const_iterator iterChildren = pElement->IterBegin ();
    for ( ; iterChildren != pElement->IterEnd (); iterChildren++ )
    {
        CElement* pElement = *iterChildren;
        if ( pElement->CountChildren () || IS_PLAYER ( pElement ) )    // This check reduces cpu usage when unloading large maps (due to recursion)
            RemovePlayersBelow ( pElement, Removed );
    }
}


void CPerPlayerEntity::AddPlayerReference ( CPlayer* pPlayer )
{
    if ( g_pGame->GetPlayerManager()->Exists( pPlayer ) )
        MapInsert( m_Players, pPlayer );
    else
        CLogger::ErrorPrintf( "CPerPlayerEntity tried to add reference for non existing player: %08x\n", pPlayer );
}


void CPerPlayerEntity::RemovePlayerReference ( CPlayer* pPlayer )
{
    MapRemove( m_Players, pPlayer );
}


//
// Hacks to stop crash
//
void CPerPlayerEntity::StaticOnPlayerDelete ( CPlayer* pPlayer )
{
    for ( std::set < CPerPlayerEntity* >::iterator iter = ms_AllPerPlayerEntityMap.begin (); iter != ms_AllPerPlayerEntityMap.end () ; ++iter )
    {
        (*iter)->OnPlayerDelete ( pPlayer );
    }
}


void CPerPlayerEntity::OnPlayerDelete ( CPlayer* pPlayer )
{
    /* Caz - Debug code disabled because it is being fixed by removing them from the map and the error is misleading users*/
    //SString strStatus;
    if ( MapContains( m_Players, pPlayer ) )
    {
        //strStatus += "m_Players ";
        MapRemove( m_Players, pPlayer );
    }

    if ( MapContains( m_PlayersAdded, pPlayer ) )
    {
        //strStatus += "m_PlayersAdded ";
        MapRemove( m_PlayersAdded, pPlayer );
    }

    if ( MapContains( m_PlayersRemoved, pPlayer ) )
    {
        //strStatus += "m_PlayersRemoved ";
        MapRemove( m_PlayersRemoved, pPlayer );
    }

    /*if ( !strStatus.empty() )
    {
        CLogger::ErrorPrintf( "CPerPlayerEntity problem: %s\n", *strStatus );
    }*/
}
