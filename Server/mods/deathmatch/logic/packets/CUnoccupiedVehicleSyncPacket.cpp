/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CUnoccupiedVehicleSyncPacket.cpp
 *  PURPOSE:     Unoccupied vehicle synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CUnoccupiedVehicleSyncPacket.h"
#include "CGame.h"
#include "CVehicleManager.h"

CUnoccupiedVehicleSyncPacket::~CUnoccupiedVehicleSyncPacket() noexcept
{
    m_Syncs.clear();
}

bool CUnoccupiedVehicleSyncPacket::Read(NetBitStreamInterface& BitStream) noexcept
{
    auto uiMaxCount = static_cast<std::uint32_t>(g_pGame->GetVehicleManager()->GetVehicleCount()) * 2u + 10u;

    // While we're not out of bytes
    for (auto i = 0; BitStream.GetNumberOfUnreadBits() >= 8; i++)
    {
        if (i > uiMaxCount)
        {
            CLogger::LogPrintf("WARN: Received excess unoccupied vehicle sync data (%d bytes)", BitStream.GetNumberOfUnreadBits() / 8);
            break;
        }

        // Read out the sync data
        SyncData data;
        data.bSend = false;

        SUnoccupiedVehicleSync vehicle;
        if (!BitStream.Read(&vehicle))
            continue;

        data.syncStructure = vehicle;

        // Add it to our list. We no longer check if it's valid here
        // because CUnoccupiedVehicleSync does and it won't write bad ID's
        // back to clients.
        m_Syncs.push_back(data);
    }

    return m_Syncs.size() > 0;
}

bool CUnoccupiedVehicleSyncPacket::Write(NetBitStreamInterface& BitStream) const noexcept
{
    // While we're not out of syncs to write
    bool bSent = false;
    for (const auto& pData : m_Syncs)
    {
        // If we're not supposed to ignore the packet
        const auto data = &pData;

        if (!data->bSend)
            continue;

        BitStream.Write(&(data->syncStructure));

        // We've sent atleast one sync
        bSent = true;
    }

    return bSent;
}
