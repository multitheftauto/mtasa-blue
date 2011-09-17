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

        float fDistance = ( pPlayer->GetLastLightSyncPosition() - pPlayer->GetPosition() ).Length();
        CVehicle* pVehicle = pPlayer->GetOccupiedVehicle ();
        bSyncPosition = ( !pVehicle || pPlayer->GetOccupiedVehicleSeat () == 0 ) && ( fDistance > 0.0001f );

        if ( bSyncPosition == false )
        {
            CVector vecPosition = pPlayer->GetPosition();
            CLogger::LogPrintf ( "LS: Player %s: light sync comparison failed x:%f, y:%f, z:%f, distance:%f\n", pPlayer->GetNick (), vecPosition.fX, vecPosition.fY, vecPosition.fZ, fDistance );
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
            SLowPrecisionPositionSync pos;
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
            pPlayer->SetLastLightSyncPosition ( pPlayer->GetPosition () );
            CLogger::LogPrintf ( "LS: Player %s: light sync position saved as x:%f, y:%f, z:%f\n", pPlayer->GetNick (), pos.data.vecPosition.fX, pos.data.vecPosition.fY, pos.data.vecPosition.fZ );
        }
    }

    return true;
}
