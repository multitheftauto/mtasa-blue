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
    m_uiQueueChecksum = 0;
    m_bQueueLocked = false;
}

CLightsyncManager::~CLightsyncManager ()
{
}

void CLightsyncManager::RegisterPlayer ( CPlayer* pPlayer )
{
    SEntry entry;
    entry.ullTime = GetTickCount64_ ();
    entry.pPlayer = pPlayer;
    entry.eType = SYNC_PLAYER;
    entry.uiContext = 0; // Unused

    bool bDoLock = m_bQueueLocked;
    if ( bDoLock )
        UnlockQueue();
    m_Queue.push_back ( entry );
    if ( bDoLock )
        LockQueue();
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
    UnlockQueue();
    for ( std::list<SEntry>::iterator iter = m_Queue.begin();
          iter != m_Queue.end (); )
    {
        SEntry& entry = *iter;
        if ( entry.pPlayer == pPlayer )
            m_Queue.erase ( iter++ );
        else
            ++iter;
    }
    LockQueue();
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

    const bool bVerifyMemory = g_pGame->GetConfig ()->GetVerifyMemory ();

    // For counting stats
    long iPacketsSent = 0;
    long iBitsSent = 0;

    // For limiting light sync processing
    long iLimitCounter = Max < uint > ( 10, g_pGame->GetPlayerManager ()->Count () / 25 );
    int iLightsyncRate = g_TickRateSettings.iLightSync;
    long long llTickCountNow = GetTickCount64_ ();
    UnlockQueue();
    while ( m_Queue.size() > 0 && m_Queue.front().ullTime + iLightsyncRate <= llTickCountNow && iLimitCounter > 0 )
    {
        SEntry entry = m_Queue.front ();
        m_Queue.pop_front ();

        if ( bVerifyMemory )
            if ( !CheckPlayer( entry.pPlayer ) )
                continue;

        CPlayer* pPlayer = entry.pPlayer;
        CPlayer::SLightweightSyncData& data = pPlayer->GetLightweightSyncData ();

        switch ( entry.eType )
        {
            case SYNC_PLAYER:
            {
                CLightsyncPacket packet;

                // Use this players far list
                const SViewerMapType& farList = pPlayer->GetFarPlayerList ();

                // For each far player
                for ( SViewerMapType ::const_iterator it = farList.begin (); it != farList.end (); ++it )
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
                            iBitsSent += pPlayer->Send ( packet );
                            iPacketsSent++;
                            packet.Reset ();
                        }
                    }
                }

                if ( packet.Count () > 0 )
                {
                    iBitsSent += pPlayer->Send ( packet );
                    iPacketsSent++;
                }

                RegisterPlayer ( pPlayer );
                iLimitCounter--;
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
    LockQueue();

    // Update stats
    g_pStats->lightsync.llLightSyncPacketsSent += iPacketsSent;
    g_pStats->lightsync.llLightSyncBytesSent += iBitsSent / 8;

    // Subtract lightsync usage from skipped accumulators
    g_pStats->lightsync.llSyncPacketsSkipped -= iPacketsSent;
    g_pStats->lightsync.llSyncBytesSkipped -= iBitsSent / 8;
}


///////////////////////////////////////////////////////////////
//
// CLightsyncManager::LockQueue
//
// For checking queue integrity
//
///////////////////////////////////////////////////////////////
void CLightsyncManager::LockQueue( void )
{
    assert( !m_bQueueLocked );

    if ( !g_pGame->GetConfig ()->GetVerifyMemory () )
        return;

    m_uiQueueChecksum = GenerateQueueChecksum();
    m_bQueueLocked = true;
}


///////////////////////////////////////////////////////////////
//
// CLightsyncManager::UnlockQueue
//
//
//
///////////////////////////////////////////////////////////////
void CLightsyncManager::UnlockQueue( void )
{
    if ( !m_bQueueLocked )
        return;

    uint uiQueueChecksum = GenerateQueueChecksum();
    if ( m_uiQueueChecksum != uiQueueChecksum )
    {
        CLogger::LogPrintf( "ERROR: Memory checksum failed for LightsyncManager queue (%08x,%08x)\n", m_uiQueueChecksum, uiQueueChecksum );
    }
    m_bQueueLocked = false;
}


///////////////////////////////////////////////////////////////
//
// CLightsyncManager::GenerateQueueChecksum
//
//
//
///////////////////////////////////////////////////////////////
uint CLightsyncManager::GenerateQueueChecksum( void )
{
    uint uiCksum = 0x12345678;
    for ( std::list<SEntry>::const_iterator iter = m_Queue.begin(); iter != m_Queue.end(); ++iter )
    {
        const SEntry& entry = *iter;
        const uint* pData = (const uint*)&entry;
        for ( uint i = 0 ; i < sizeof ( SEntry ) / sizeof ( uint ) ; i++ )
        {
            uiCksum += pData[i];
            uiCksum += (uiCksum << 10);
            uiCksum ^= (uiCksum >> 6);
        }
    }
    return uiCksum;
}


///////////////////////////////////////////////////////////////
//
// CLightsyncManager::CheckPlayer
//
// Check Player is valid and has valid players in its far list
// Return false if player is not valid
//
///////////////////////////////////////////////////////////////
bool CLightsyncManager::CheckPlayer( CPlayer* pPlayer )
{
    if ( !g_pGame->GetPlayerManager()->Exists( pPlayer ) )
    {
        CLogger::LogPrintf( "ERROR: Unknown player in LightsyncManager queue\n" );
        return false;
    }
    else
    {
        // Check farlist
        const SViewerMapType& farList = pPlayer->GetFarPlayerList();
        for ( SViewerMapType ::const_iterator it = farList.begin(); it != farList.end(); ++it )
        {
            if ( !g_pGame->GetPlayerManager()->Exists( it->first ) )
            {
                CLogger::LogPrintf( "ERROR: Unknown player in farList\n" );
            }
        }
        return true;
    }
}
