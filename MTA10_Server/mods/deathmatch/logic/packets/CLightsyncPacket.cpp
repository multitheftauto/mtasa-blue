/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CLightsyncPacket.cpp
*  PURPOSE:     Lightweight synchronization packet class
*  DEVELOPERS:  Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CLightsyncPacket::CLightsyncPacket ( CPlayer* pPlayer )
{
    m_pSourceElement = pPlayer;
    CPlayer::SLightweightSyncData& data = pPlayer->GetLightweightSyncData ();
    if ( ( data.uiNumHealthSyncs % 2 ) == 0 )
    {
        if ( fabs(data.fLastHealthSynced - pPlayer->GetHealth()) > LIGHTSYNC_HEALTH_THRESHOLD ||
             fabs(data.fLastArmorSynced - pPlayer->GetArmor()) > LIGHTSYNC_HEALTH_THRESHOLD )
        {
            m_bSyncHealth = true;
            data.fLastHealthSynced = pPlayer->GetHealth ();
            data.fLastArmorSynced = pPlayer->GetArmor ();
            ++data.uiNumHealthSyncs;
        }
        else
            m_bSyncHealth = false;
    }
    else
    {
        m_bSyncHealth = false;
        ++data.uiNumHealthSyncs;
    }

    CVehicle* pOccupiedVehicle = pPlayer->GetOccupiedVehicle ();
    m_bSyncVehicleHealth = false;
    if ( !pOccupiedVehicle )
    {
        m_bSyncPosition = true;
        data.lastSyncedVehicle = 0;
    }
    else
    {
        if ( pPlayer->GetOccupiedVehicleSeat () == 0 )
        {
            m_bSyncPosition = true;
            if ( data.lastSyncedVehicle != pOccupiedVehicle )
            {
                data.lastSyncedVehicle = pOccupiedVehicle;
                data.uiNumHealthSyncs = 1;
                m_bSyncVehicleHealth = true;
                data.fLastVehicleHealthSynced = pOccupiedVehicle->GetHealth ();
            }
            else if ( ( data.uiNumVehicleHealthSyncs % 2 ) == 0 )
            {
                if ( fabs(data.fLastHealthSynced - pOccupiedVehicle->GetHealth()) > LIGHTSYNC_VEHICLE_HEALTH_THRESHOLD )
                {
                    m_bSyncVehicleHealth = true;
                    data.fLastVehicleHealthSynced = pOccupiedVehicle->GetHealth ();
                    data.uiNumVehicleHealthSyncs++;
                }
            }
            else
                data.uiNumVehicleHealthSyncs++;
        }
        else
            m_bSyncPosition = false;
    }
}

bool CLightsyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Only the server sends these.
    return false;
}

bool CLightsyncPacket::Write ( NetBitStreamInterface& BitStream ) const 
{
    CPlayer* pPlayer = static_cast < CPlayer * > ( m_pSourceElement );
    SPlayerIDSync PlayerID;
    PlayerID.data.ID = pPlayer->GetID ();
    BitStream.Write ( &PlayerID );
    BitStream.Write ( (unsigned char)pPlayer->GetSyncTimeContext () );

    unsigned short usLatency = pPlayer->GetPing ();
    BitStream.WriteCompressed ( usLatency );

    BitStream.WriteBit ( m_bSyncHealth );
    if ( m_bSyncHealth )
    {
        SPlayerHealthSync health;
        health.data.fValue = pPlayer->GetHealth ();
        BitStream.Write ( &health );

        SPlayerArmorSync armor;
        armor.data.fValue = pPlayer->GetArmor ();
        BitStream.Write ( &armor );
    }

    BitStream.WriteBit ( m_bSyncPosition );
    if ( m_bSyncPosition )
    {
        SLowPrecisionPositionSync pos;
        pos.data.vecPosition = pPlayer->GetPosition ();
        BitStream.Write ( &pos );
    }

    if ( m_bSyncPosition )
    {
        BitStream.WriteBit ( m_bSyncVehicleHealth );
        if ( m_bSyncVehicleHealth )
        {
            SLowPrecisionVehicleHealthSync health;
            health.data.fValue = pPlayer->GetOccupiedVehicle()->GetHealth ();
            BitStream.Write ( &health );
        }
    }

    return true;
}
