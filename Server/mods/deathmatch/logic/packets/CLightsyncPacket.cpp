/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CLightsyncPacket.cpp
 *  PURPOSE:     Lightweight synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLightsyncPacket.h"
#include "net/SyncStructures.h"
#include "CTickRateSettings.h"
#include "CPlayer.h"

bool CLightsyncPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    // Only the server sends these.
    return false;
}

bool CLightsyncPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    bool bSyncPosition;

    if (!Count())
        return false;

    for (const auto& pPlayer : m_players)
    {
        CPlayer::SLightweightSyncData& data = pPlayer->GetLightweightSyncData();
        CVehicle*                      pVehicle = pPlayer->GetOccupiedVehicle();

        // Find the difference between now and the time the position last changed for the player
        std::int64_t llTicksDifference = GetTickCount64_() - pPlayer->GetPositionLastChanged();

        // Right we need to sync the position if there is no vehicle or he's in a vehicle and the difference between setPosition is less than or equal to the
        // slow sync rate i.e. make sure his position has been updated more than 0.001f in the last 1500ms plus a small margin for error (probably not needed).
        // This will ensure we only send positions when the position has changed.
        bSyncPosition = (!pVehicle || pPlayer->GetOccupiedVehicleSeat() == 0) && llTicksDifference <= g_TickRateSettings.iLightSync + 100;

        BitStream.Write(pPlayer->GetID());
        BitStream.Write((std::uint8_t)pPlayer->GetSyncTimeContext());

        unsigned short usLatency = pPlayer->GetPing();
        BitStream.WriteCompressed(usLatency);

        BitStream.WriteBit(data.health.bSync);
        if (data.health.bSync)
        {
            SPlayerHealthSync health;
            health.data.fValue = pPlayer->GetHealth();
            BitStream.Write(&health);

            SPlayerArmorSync armor;
            armor.data.fValue = pPlayer->GetArmor();
            BitStream.Write(&armor);
        }

        BitStream.WriteBit(bSyncPosition);
        if (bSyncPosition)
        {
            SLowPrecisionPositionSync pos;
            pos.data.vecPosition = pPlayer->GetPosition();
            BitStream.Write(&pos);

            bool bSyncVehicleHealth = data.vehicleHealth.bSync && pVehicle;
            BitStream.WriteBit(bSyncVehicleHealth);
            if (bSyncVehicleHealth)
            {
                SLowPrecisionVehicleHealthSync health;
                health.data.fValue = pVehicle->GetHealth();
                BitStream.Write(&health);
            }
        }
    }

    return true;
}
