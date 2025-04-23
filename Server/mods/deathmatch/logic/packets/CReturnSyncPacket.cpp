/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CReturnSyncPacket.cpp
 *  PURPOSE:     Player return sync packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CReturnSyncPacket.h"
#include "CPlayer.h"
#include "net/SyncStructures.h"

CReturnSyncPacket::CReturnSyncPacket(CPlayer* pPlayer)
{
    m_pSourceElement = pPlayer;
}

bool CReturnSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    // Got a player to write?
    if (m_pSourceElement)
    {
        CPlayer* pSourcePlayer = static_cast<CPlayer*>(m_pSourceElement);

        // Grab eventual vehicle
        CVehicle* pVehicle = pSourcePlayer->GetOccupiedVehicle();

        SPositionSync position(false);

        // Flags
        BitStream.WriteBit(pVehicle != NULL);

        // In a vehicle?
        if (pVehicle)
        {
            // Write its position
            position.data.vecPosition = pVehicle->GetPosition();
            BitStream.Write(&position);

            // And rotation
            SRotationDegreesSync rotation(false);
            pVehicle->GetRotationDegrees(rotation.data.vecRotation);
            BitStream.Write(&rotation);
        }
        else
        {
            // Write his position
            position.data.vecPosition = pSourcePlayer->GetPosition();
            BitStream.Write(&position);
        }

        return true;
    }

    return false;
}
