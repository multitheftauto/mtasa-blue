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

bool CLightsyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Only the server sends these.
    return false;
}

bool CLightsyncPacket::Write ( NetBitStreamInterface& BitStream ) const 
{
    bool bSyncPosition;

    if ( Count() == 0 )
        return false;

    for ( std::vector<CPlayer *>::const_iterator iter = m_players.begin ();
          iter != m_players.end();
          ++iter )
    {
        CPlayer* pPlayer = *iter;
        CPlayer::SLightweightSyncData& data = pPlayer->GetLightweightSyncData ();

        float fDistance = DistanceBetweenPoints3D ( pPlayer->GetPosition(), pPlayer->GetLastLightSyncPosition() );
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        //bSyncPosition = ( !pVehicle || pPlayer->GetOccupiedVehicleSeat () == 0 ) && ( fDistance > 0.001f );
        bSyncPosition = ( !pVehicle || pPlayer->GetOccupiedVehicleSeat () == 0 );
        if ( pPlayer->GetLastLightSyncPosition ( ) == CVector ( 3000, 3000, 3000 ) )
        {
            // If we don't have a previous lightsync position... create one
            bSyncPosition = true;
        }

        BitStream.Write ( pPlayer->GetID () );
        BitStream.Write ( (unsigned char)pPlayer->GetSyncTimeContext () );

        unsigned short usLatency = pPlayer->GetPing ();
        BitStream.WriteCompressed ( usLatency );

        BitStream.WriteBit ( data.health.bSync );
        if ( data.health.bSync )
        {
            SPlayerHealthSync health;
            health.data.fValue = pPlayer->GetHealth ();
            BitStream.Write ( &health );

            SPlayerArmorSync armor;
            armor.data.fValue = pPlayer->GetArmor ();
            BitStream.Write ( &armor );
        }

        BitStream.WriteBit ( bSyncPosition );
        if ( bSyncPosition )
        {
            SPositionSync pos;
            pos.data.vecPosition = pPlayer->GetPosition ();
            BitStream.Write ( &pos );

            bool bSyncVehicleHealth = data.vehicleHealth.bSync && pVehicle;
            BitStream.WriteBit ( bSyncVehicleHealth );
            if ( bSyncVehicleHealth )
            {
                SLowPrecisionVehicleHealthSync health;
                health.data.fValue = pVehicle->GetHealth ();
                BitStream.Write ( &health );
            }
        }
    }

    return true;
}
