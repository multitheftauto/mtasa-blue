/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CLightsyncManager.h
*  PURPOSE:     Lightweight synchronization manager class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CLightsyncManager::CLightsyncManager ()
{
}

CLightsyncManager::~CLightsyncManager ()
{
}

void CLightsyncManager::RegisterPlayer ( CPlayer* pPlayer )
{
    SEntry entry;
    entry.ullTime = GetTickCount64_ () + SLOW_SYNCRATE;
    entry.pPlayer = pPlayer;
    entry.eType = SYNC_PLAYER;
    entry.uiContext = 0; // Unused
    m_Queue.push_back ( entry );
}

CPlayer* CLightsyncManager::FindPlayer ( const char* szArguments )
{    
    for ( std::list<SEntry>::iterator iter = m_Queue.begin();
          iter != m_Queue.end (); )
    {
        SEntry& entry = *iter;
        if ( strcmp ( entry.pPlayer->GetNick ( ), szArguments ) == 0 )
        {
            return entry.pPlayer;
        }

         ++iter;
    }
    return NULL;
}

void CLightsyncManager::UnregisterPlayer ( CPlayer* pPlayer )
{
    for ( std::list<SEntry>::iterator iter = m_Queue.begin();
          iter != m_Queue.end (); )
    {
        SEntry& entry = *iter;
        if ( entry.pPlayer == pPlayer )
            m_Queue.erase ( iter++ );
        else
            ++iter;
    }
}

void CLightsyncManager::DoPulse ()
{
    // The point of this method is to get all players that should receive right now the lightweight
    // sync from all the other players. To make this efficient, we are using a queue in what the
    // players at the front have are more inminent to receive this sync. As all players have the same
    // far sync rate, just taking the players from the front, processing them  and pushing them to the
    // back will keep the order of priority to be processed.
    //
    // We also want to perform delta sync on stuff like the health, but it would require to know the last
    // value synced from every player to every player, because we are getting players from the front and
    // sending them all the other players data. This would be crazily inefficient and would require a lot
    // of mainteinance, so we will leave special entries in the queue to mark in what moment a player
    // health changed. This way, we will force the health sync for that player until the queue cycle
    // reaches that special entry again. As there might be multiple changes in a complete queue cycle, we
    // are also storing the delta context in what it happened, so we only consider the health unchanged
    // when we find the marker with the same context value.

    if ( g_pBandwidthSettings->bLightSyncEnabled == false )
        return;

    CPlayerManager* pManager = g_pGame->GetPlayerManager();
    while ( m_Queue.size() > 0 && m_Queue.front().ullTime <= GetTickCount64_ () )
    {
        SEntry entry = m_Queue.front ();
        CPlayer* pPlayer = entry.pPlayer;
        CPlayer::SLightweightSyncData& data = pPlayer->GetLightweightSyncData ();

        m_Queue.pop_front ();

        switch ( entry.eType )
        {
            case SYNC_PLAYER:
            {
                CLightsyncPacket packet;

                // Use this players far list
                const std::map < CPlayer*, SNearInfo >& farList = pPlayer->GetFarPlayerList ();

                // For each far player
                for ( std::map < CPlayer*, SNearInfo > ::const_iterator it = farList.begin (); it != farList.end (); ++it )
                {
                    CPlayer* pCurrent = it->first;
                    dassert ( pPlayer != pCurrent );

                    // Only send if he isn't network troubled.
                    if ( pCurrent->UhOhNetworkTrouble ( ) == false )
                    {
                        CPlayer::SLightweightSyncData& currentData = pCurrent->GetLightweightSyncData ();
                        packet.AddPlayer ( pCurrent );

                        // Calculate the delta sync
                        if ( fabs(currentData.health.fLastHealth - pCurrent->GetHealth()) > LIGHTSYNC_HEALTH_THRESHOLD ||
                             fabs(currentData.health.fLastArmor - pCurrent->GetArmor()) > LIGHTSYNC_HEALTH_THRESHOLD )
                        {
                            currentData.health.fLastHealth = pCurrent->GetHealth ();
                            currentData.health.fLastArmor = pCurrent->GetArmor ();
                            currentData.health.bSync = true;
                            currentData.health.uiContext++;

                            // Generate the health marker
                            SEntry marker;
                            marker.ullTime = 0;
                            marker.pPlayer = pCurrent;
                            marker.eType = DELTA_MARKER_HEALTH;
                            marker.uiContext = currentData.health.uiContext;
                            m_Queue.push_back ( marker );
                        }

                        CVehicle* pVehicle = pCurrent->GetOccupiedVehicle ();
                        if ( pVehicle && pCurrent->GetOccupiedVehicleSeat() == 0 )
                        {
                            if ( currentData.vehicleHealth.lastVehicle != pVehicle ||
                                 fabs(currentData.vehicleHealth.fLastHealth - pVehicle->GetHealth ()) > LIGHTSYNC_VEHICLE_HEALTH_THRESHOLD )
                            {
                                currentData.vehicleHealth.fLastHealth = pVehicle->GetHealth ();
                                currentData.vehicleHealth.lastVehicle = pVehicle;
                                currentData.vehicleHealth.bSync = true;
                                currentData.vehicleHealth.uiContext++;

                                // Generate the vehicle health marker
                                SEntry marker;
                                marker.ullTime = 0;
                                marker.pPlayer = pCurrent;
                                marker.eType = DELTA_MARKER_VEHICLE_HEALTH;
                                marker.uiContext = currentData.vehicleHealth.uiContext;
                                m_Queue.push_back ( marker );
                            }
                        }

                        if ( packet.Count () == LIGHTSYNC_MAX_PLAYERS )
                        {
                            pPlayer->Send ( packet );
                            packet.Reset ();
                        }
                    }
                }

                if ( packet.Count () > 0 )
                    pPlayer->Send ( packet );

                RegisterPlayer ( pPlayer );
                break;
            }

            case DELTA_MARKER_HEALTH:
            {
                if ( data.health.uiContext == entry.uiContext )
                    data.health.bSync = false;

                break;
            }

            case DELTA_MARKER_VEHICLE_HEALTH:
            {
                if ( data.vehicleHealth.uiContext == entry.uiContext )
                    data.vehicleHealth.bSync = false;

                break;
            }
        }
    }
}
