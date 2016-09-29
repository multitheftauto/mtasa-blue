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
    for ( auto& pPlayer : m_Players )
    {
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
    for ( auto& pPlayer : m_Players )
    {
        delete pPlayer;
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
    for ( auto& pPlayer : m_Players )
    {
        const char* szPtr = pPlayer->GetNick ();
        if ( szPtr )
        {
            if ( bCaseSensitive && strcmp ( szNick, szPtr ) == 0 || !bCaseSensitive && stricmp ( szNick, szPtr ) == 0 )
            {
                return pPlayer;
            }
        }
    }

    // If we didn't find it, return NULL
    return NULL;
}


CClientPlayer* CClientPlayerManager::Get ( CPlayerPed* pGamePlayer, bool bValidatePointer )
{
    if ( !pGamePlayer ) return NULL;

    if ( bValidatePointer )
    {
        
        for ( auto& pPlayer : m_Players )
        {
            if ( pPlayer->GetGamePlayer () == pGamePlayer )
            {
                return pPlayer;
            }
        }
    }
    else
    {
        CClientPed* pPed = reinterpret_cast < CClientPed* > ( pGamePlayer->GetStoredPointer () );
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


bool CClientPlayerManager::IsValidModel(unsigned long ulModel)
{
    return (ulModel == 0 ||
        ulModel == 1 ||
        ulModel == 2 ||
        ulModel == 7 ||
        ulModel >= 9 &&
        ulModel != 208 &&
        ulModel != 149 &&
        ulModel != 119 &&
        ulModel != 86 &&
        ulModel != 74 &&
        ulModel != 65 &&
        ulModel != 42 &&
        ulModel <= 272 ||
        ulModel >= 274 &&
        ulModel <= 288 ||
        ulModel >= 290 &&
        ulModel <= 312);
}


void CClientPlayerManager::ResetAll ( void )
{
    for ( auto& pPlayer : m_Players )
    {
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
