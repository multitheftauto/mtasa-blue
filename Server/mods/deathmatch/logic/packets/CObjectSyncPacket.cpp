/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CObjectSyncPacket.cpp
 *  PURPOSE:     Header for object sync packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CObjectSyncPacket.h"
#include <net/SyncStructures.h>

CObjectSyncPacket::~CObjectSyncPacket()
{
    std::vector<SyncData*>::const_iterator iter = m_Syncs.begin();
    for (; iter != m_Syncs.end(); ++iter)
    {
        delete *iter;
    }
    m_Syncs.clear();
}

bool CObjectSyncPacket::Read(NetBitStreamInterface& BitStream)
{
    // While we're not out of bytes
    while (BitStream.GetNumberOfUnreadBits() > 8)
    {
        // Read out the sync data
        SyncData* pData = new SyncData;
        pData->bSend = false;

        // Read out the ID
        if (!BitStream.Read(pData->ID))
            return false;

        // Read the sync time context
        if (!BitStream.Read(pData->ucSyncTimeContext))
            return false;

        // Read out the flags
        //SIntegerSync<unsigned char, 12> flags;
        unsigned int flags;
        if (!BitStream.Read(flags))
            return false;
        pData->ucFlags = flags;

        // Read out the position
        if (flags & 0x1)
        {
            SPositionSync position;
            if (!BitStream.Read(&position))
                return false;
            pData->vecPosition = position.data.vecPosition;
        }

        // Read out the rotation
        if (flags & 0x2)
        {
            SRotationRadiansSync rotation;
            if (!BitStream.Read(&rotation))
                return false;
            pData->vecRotation = rotation.data.vecRotation;
        }

        // Read out the velocity
        if (flags & 0x4)
        {
            SVelocitySync velocity;
            if (!BitStream.Read(&velocity))
                return false;

            pData->vecVelocity = velocity.data.vecVelocity;
        }

        // Read out the angular velocity
        if (flags & 0x8)
        {
            SVelocitySync angularVelocity;
            if (!BitStream.Read(&angularVelocity))
                return false;

            pData->vecTurnVelocity = angularVelocity.data.vecVelocity;
        }

        // Read out the health & attacker
        if (flags & 0x10)
        {
            SObjectHealthSync health;
            if (!BitStream.Read(&health) || !BitStream.Read(pData->attackerID))
                return false;
            pData->fHealth = health.data.fValue;
        }

        // Read out inWater state
        if (flags & 0x20)
            pData->bIsInWater = BitStream.ReadBit();

        // Add it to our list
        m_Syncs.push_back(pData);
    }

    return m_Syncs.size() > 0;
}

bool CObjectSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    bool                                   bSent = false;
    std::vector<SyncData*>::const_iterator iter = m_Syncs.begin();
    // Write syncs
    for (; iter != m_Syncs.end(); ++iter)
    {
        SyncData* pData = *iter;
        // If we're not supposed to ignore the packet
        if (pData->bSend)
        {
            // Write the ID
            BitStream.Write(pData->ID);

            // Write the sync time context
            BitStream.Write(pData->ucSyncTimeContext);

            // Write the flags
            //SIntegerSync<unsigned char, 12> flags(pData->ucFlags);
            unsigned int flags = pData->ucFlags;
            BitStream.Write(flags);

            // Write the position
            if (flags & 0x1)
            {
                SPositionSync position;
                position.data.vecPosition = pData->vecPosition;
                BitStream.Write(&position);
            }

            // Write the rotation
            if (flags & 0x2)
            {
                SRotationRadiansSync rotation;
                rotation.data.vecRotation = pData->vecRotation;
                BitStream.Write(&rotation);
            }

            // Write the velocity
            if (flags & 0x4)
            {
                SVelocitySync velocity;
                velocity.data.vecVelocity = pData->vecVelocity;
                BitStream.Write(&velocity);
            }

            // Write the angular velocity
            if (flags & 0x8)
            {
                SVelocitySync angularVelocity;
                angularVelocity.data.vecVelocity = pData->vecTurnVelocity;
                BitStream.Write(&angularVelocity);
            }

            // Write the health & attacker
            if (flags & 0x10)
            {
                SObjectHealthSync health;
                health.data.fValue = pData->fHealth;
                BitStream.Write(&health);
                BitStream.Write(pData->attackerID);
            }

            // Write the inWater state
            if (flags & 0x20)
                BitStream.WriteBit(pData->bIsInWater);

            // We've sent atleast one sync
            bSent = true;
        }
    }

    return bSent;
}
