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
            // Pulse voice data if voice is enabled
            if ( g_pClientGame->GetVoiceRecorder()->IsEnabled() && pPlayer->GetVoice() )
                pPlayer->GetVoice()->DoPulse();

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
    return m_Players.Contains ( pPlayer );
}


bool CClientPlayerManager::IsPlayerLimitReached ( void )
{
    return g_pGame->GetPools ()->GetPedCount () >= 110;

    return true;
}


bool CClientPlayerManager::IsValidModel ( unsigned long ulModel )
{
    return ( ( ulModel >= 0 && ulModel <= 312 ) || ulModel == 329 || ulModel == 340 || ulModel == 382 ||
         ulModel == 383 || ulModel == 398 || ulModel == 399 || ( ulModel >= 612 &&  ulModel <= 614 ) ||
         ulModel == 662 || ulModel == 663 || ( ulModel >= 665 && ulModel <= 699 ) ||
         ( ulModel >= 793 && ulModel <= 799 ) || ( ulModel >= 907 && ulModel <= 909 ) || 
         ulModel == 965 || ulModel == 999 || ( ulModel >= 1194 && ulModel <= 1206 ) ||
         ulModel == 1326 || ulModel == 1573 || ulModel == 1699 || ulModel == 2883 || ulModel == 2884 ||
         ( ulModel >= 3176 && ulModel <= 3197 ) || ( ulModel >= 3215 && ulModel <= 3220 ) ||
         ulModel == 3245 || ulModel == 3247 || ulModel == 3248 || ulModel == 3251 || ulModel == 3254 ||
         ulModel == 3266 || ulModel == 3348 || ulModel == 3349 || ulModel == 3416 || ulModel == 3429 ||
         ulModel == 3610 || ulModel == 3611 || ulModel == 3784 || ulModel == 3870 || ulModel == 3871 ||
         ulModel == 3883 || ulModel == 3889 || ulModel == 3974 || ( ulModel >= 4542 && ulModel <= 4549 ) ||
         ulModel == 5090 || ulModel == 5104 || ( ulModel >= 3136 && ulModel <= 3166 ) ||
         ( ulModel >= 3194 && ulModel <= 3213 ) || ( ulModel >= 3222 && ulModel <= 3240 ) ||
         ( ulModel >= 4763 && ulModel <= 4805 ) || ( ulModel >= 5376 && ulModel <= 5389 )
        );
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


void CClientPlayerManager::RemoveFromList ( CClientPlayer* pPlayer )
{
    if ( m_bCanRemoveFromList )
    {
        m_Players.remove ( pPlayer );
    }
}
