/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CClientPlayerManager.cpp
*  PURPOSE:     Player entity manager class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <ijsf@gmx.net>
*
*****************************************************************************/

#include "StdInc.h"

#define REMOTE_PLAYER_CONNECTION_TROUBLE_TIME 6000

using std::list;
using std::vector;

CClientPlayerManager::CClientPlayerManager ( CClientManager* pManager )
{
    // Initialize members
    m_pManager = pManager;
    m_bCanRemoveFromList = true;
    m_pLocalPlayer = NULL;
}


CClientPlayerManager::~CClientPlayerManager ( void )
{
    // Destroy all players
    DeleteAll ();
}

void CClientPlayerManager::DoPulse ( void )
{
    unsigned long ulCurrentTime = CClientTime::GetTime ();
    CClientPlayer * pPlayer = NULL;
    vector < CClientPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); ++iter )
    {
        pPlayer = *iter;
        if ( !pPlayer->IsLocalPlayer () )
        {
            // Flag him with connection error if its been too long since last puresync and force his position
            unsigned long ulLastPuresyncTime = pPlayer->GetLastPuresyncTime ();
            bool bHasConnectionTrouble = ( ulLastPuresyncTime != 0 && ulCurrentTime >= ulLastPuresyncTime + REMOTE_PLAYER_CONNECTION_TROUBLE_TIME );
            if ( bHasConnectionTrouble && !g_pClientGame->IsDownloadingBigPacket () && !pPlayer->IsDeadOnNetwork () )
            {
                pPlayer->SetHasConnectionTrouble ( true );

                // Reset his controller so he doesn't get stuck shooting or something
                CControllerState State;
                memset ( &State, 0, sizeof ( CControllerState ) );
                pPlayer->SetControllerState ( State );

                // Grab his vehicle if any and force the position to where he was last sync
                CClientVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
                if ( pVehicle )
                {
                    // Is he driving the vehicle?
                    if ( pPlayer->GetOccupiedVehicleSeat () == 0 )
                    {
                        // Force his position to where he was last sync
                        pVehicle->SetPosition ( pPlayer->GetLastPuresyncPosition () );
                        pVehicle->SetMoveSpeed ( CVector ( 0, 0, 0 ) );
                        pVehicle->SetTurnSpeed ( CVector ( 0, 0, 0 ) );
                        pPlayer->ResetInterpolation ();
                    }
                }
                else
                {
                    // Force his position to where he was last sync
                    pPlayer->SetPosition ( pPlayer->GetLastPuresyncPosition () );
                    pPlayer->ResetInterpolation ();
                    pPlayer->SetMoveSpeed ( CVector ( 0, 0, 0 ) );
                    pPlayer->ResetInterpolation ();
                }
            }
            else
            {
                pPlayer->SetHasConnectionTrouble ( false );
            }
        }
    }
}


void CClientPlayerManager::DeleteAll ( void )
{
    // Delete all the players
    m_bCanRemoveFromList = false;
    vector < CClientPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        delete *iter;
    }

    // Clear the list
    m_Players.clear ();
    m_bCanRemoveFromList = true;
}


CClientPlayer* CClientPlayerManager::Get ( ElementID ID )
{
    // Grab the element with the given id. Check its type.
    CClientEntity* pEntity = CElementIDs::GetElement ( ID );
    if ( pEntity && pEntity->GetType () == CCLIENTPLAYER )
    {
        return static_cast < CClientPlayer* > ( pEntity );
    }

    return NULL;
}


CClientPlayer* CClientPlayerManager::Get ( const char* szNick, bool bCaseSensitive )
{
    assert ( szNick );

    // Find a player with a matching nick in the list
    vector < CClientPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        const char* szPtr = (*iter)->GetNick ();
        if ( szPtr )
        {
            if ( bCaseSensitive && strcmp ( szNick, szPtr ) == 0 || !bCaseSensitive && stricmp ( szNick, szPtr ) == 0 )
            {
                return *iter;
            }
        }
    }

    // If we didn't find it, return NULL
    return NULL;
}


CClientPlayer* CClientPlayerManager::Get ( CPlayerPed* pPlayer, bool bValidatePointer )
{
    if ( !pPlayer ) return NULL;

    if ( bValidatePointer )
    {
        vector < CClientPlayer* > ::const_iterator iter = m_Players.begin ();
        for ( ; iter != m_Players.end () ; iter++ )
        {
            if ( (*iter)->GetGamePlayer () == pPlayer )
            {
                return *iter;
            }
        }
    }
    else
    {
        CClientPed* pPed = reinterpret_cast < CClientPed* > ( pPlayer->GetStoredPointer () );
        if ( pPed->GetType () == CCLIENTPLAYER )
        {
            return static_cast < CClientPlayer * > ( pPed );
        }
    }

    return NULL;
}


bool CClientPlayerManager::Exists ( CClientPlayer* pPlayer )
{
    vector < CClientPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end () ; iter++ )
    {
        if ( *iter == pPlayer )
        {
            return true;
        }
    }

    return false;
}


bool CClientPlayerManager::IsPlayerLimitReached ( void )
{
    return g_pGame->GetPools ()->GetPedCount () >= 110;

    return true;
}


bool CClientPlayerManager::IsValidModel ( unsigned long ulModel )
{
    return ( ulModel == 0 ||
             ulModel == 7 ||
             ulModel >= 9 &&
             ulModel != 208 &&
             ulModel != 149 &&
             ulModel != 119 &&
             ulModel != 86 &&
             ulModel != 74 &&
             ulModel != 65 &&
             ulModel != 42 &&
             ulModel <= 264 ||
             ulModel >= 274 &&
             ulModel <= 288 );
}


void CClientPlayerManager::ResetAll ( void )
{
    vector < CClientPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        CClientPlayer * pPlayer = *iter;
        if ( pPlayer )
        {
            pPlayer->Reset ();
        }
    }
}


vector < CClientPlayer* > ::const_iterator CClientPlayerManager::IterGet ( CClientPlayer* pPlayer )
{
    // Find it in our list
    vector < CClientPlayer* > ::const_iterator iter = m_Players.begin ();
    for ( ; iter != m_Players.end (); iter++ )
    {
        if ( *iter == pPlayer )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_Players.begin ();
}


vector < CClientPlayer* > ::const_reverse_iterator CClientPlayerManager::IterGetReverse ( CClientPlayer* pPlayer )
{
    // Find it in our list
    vector < CClientPlayer* > ::reverse_iterator iter = m_Players.rbegin ();
    for ( ; iter != m_Players.rend (); iter++ )
    {
        if ( *iter == pPlayer )
        {
            return iter;
        }
    }

    // We couldn't find it
    return m_Players.rbegin ();
}


void CClientPlayerManager::RemoveFromList ( CClientPlayer* pPlayer )
{
    if ( m_bCanRemoveFromList )
    {
        ListRemove ( m_Players, pPlayer );
    }
}
