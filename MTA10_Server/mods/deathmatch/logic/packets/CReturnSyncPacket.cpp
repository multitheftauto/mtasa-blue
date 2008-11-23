/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CReturnSyncPacket.cpp
*  PURPOSE:     Player return sync packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

CReturnSyncPacket::CReturnSyncPacket ( CPlayer * pPlayer )
{
    m_pSourceElement = pPlayer;
}

bool CReturnSyncPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Got a player to write?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Grab eventual vehicle
        CVehicle* pVehicle = pSourcePlayer->GetOccupiedVehicle ();

        // Flags
        unsigned char ucFlags = 0;
        ucFlags |= ( pVehicle != NULL ) ? 1:0;
        
        // Write the flags
        BitStream.Write ( ucFlags );

        // In a vehicle?
        if ( pVehicle )
        {
            // Write its position
            const CVector& vecPosition = pVehicle->GetPosition ();
            BitStream.Write ( vecPosition.fX );
            BitStream.Write ( vecPosition.fY );
            BitStream.Write ( vecPosition.fZ );

            // And rotation
            CVector vecRotationDegrees;
            pVehicle->GetRotationDegrees ( vecRotationDegrees );

            BitStream.Write ( vecRotationDegrees.fX );
            BitStream.Write ( vecRotationDegrees.fY );
            BitStream.Write ( vecRotationDegrees.fZ );
        }
        else
        {
            // Write his position
            CVector vecTemp = pSourcePlayer->GetPosition ();
            BitStream.Write ( vecTemp.fX );
            BitStream.Write ( vecTemp.fY );
            BitStream.Write ( vecTemp.fZ );
        }

        return true;
    }

    return false;
}
