/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPedSyncPacket.cpp
 *  PURPOSE:     Ped synchronization packet class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPedSyncPacket.h"

CPedSyncPacket::CPedSyncPacket(SyncData& ReadData)
{
    // Copy the struct
    m_Syncs.push_back(ReadData);
}

bool CPedSyncPacket::Read(NetBitStreamInterface& BitStream)
{
    // While we're not out of bytes
    while (BitStream.GetNumberOfUnreadBits() > 32)
    {
        // Read out the sync data
        SyncData Data;

        if (!BitStream.Read(Data.ID))
            return false;

        // Read the sync time context
        if (!BitStream.Read(Data.ucSyncTimeContext))
            return false;

        unsigned char ucFlags = 0;
        if (!BitStream.Read(ucFlags))
            return false;
        Data.ucFlags = ucFlags;

        if (BitStream.Can(eBitStreamVersion::PedSync_CameraRotation))
        {
            if (!BitStream.Read(Data.flags2))
                return false;
        }
        else
            Data.flags2 = 0;

        // Did we recieve position?
        if (ucFlags & 0x01)
        {    
            if (!(BitStream.Can(eBitStreamVersion::PedSync_Revision) ? Data.ReadSpatialData(BitStream) : Data.ReadSpatialDataBC(BitStream)))
                return false;
        }

        // Health and armour
        if (ucFlags & 0x08)
        {
            if (!BitStream.Read(Data.fHealth))
                return false;
        }
        if (ucFlags & 0x10)
        {
            if (!BitStream.Read(Data.fArmor))
                return false;
        }

        if (Data.flags2 & 0x01)
        {
            SCameraRotationSync camRotation;
            if (!BitStream.Read(&camRotation))
                return false;
            Data.cameraRotation = camRotation.data.fRotation;
        }

        // On Fire
        if (ucFlags & 0x20)
        {
            if (!BitStream.ReadBit(Data.bOnFire))
                return false;
        }

        if (ucFlags & 0x60 && BitStream.Can(eBitStreamVersion::IsPedReloadingWeapon))
        {
            if (!BitStream.ReadBit(Data.isReloadingWeapon))
                return false;
        }

        // In Water
        if (ucFlags & 0x40)
        {
            if (!BitStream.ReadBit(Data.bIsInWater))
                return false;
        }

        // Add it to our list. We no longer check if it's valid here
        // because CPedSync does and it won't write bad ID's
        // back to clients.
        m_Syncs.push_back(Data);
    }

    return m_Syncs.size() > 0;
}

bool CPedSyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    const SyncData& Data = m_Syncs.front();
    if (!&Data)
        return false;

    // Write vehicle ID
    BitStream.Write(Data.ID);

    // Write the sync time context
    BitStream.Write(Data.ucSyncTimeContext);

    BitStream.Write(Data.ucFlags);

    if (BitStream.Can(eBitStreamVersion::PedSync_CameraRotation))
        BitStream.Write(Data.flags2);

    if (BitStream.Can(eBitStreamVersion::PedSync_Revision))
    {
        // Position and rotation
        if (Data.ucFlags & 0x01)
            BitStream.Write(&Data.position);

        if (Data.ucFlags & 0x02)
            BitStream.Write(&Data.rotation);

        // Velocity
        if (Data.ucFlags & 0x04)
            BitStream.Write(&Data.velocity);
    }
    else
    {
        // Position and rotation
        if (Data.ucFlags & 0x01)
        {
            BitStream.Write(Data.position.data.vecPosition.fX);
            BitStream.Write(Data.position.data.vecPosition.fY);
            BitStream.Write(Data.position.data.vecPosition.fZ);
        }

        if (Data.ucFlags & 0x02)
            BitStream.Write(Data.rotation.data.fRotation);

        // Velocity
        if (Data.ucFlags & 0x04)
        {
            BitStream.Write(Data.velocity.data.vecVelocity.fX);
            BitStream.Write(Data.velocity.data.vecVelocity.fY);
            BitStream.Write(Data.velocity.data.vecVelocity.fZ);
        }
    }

    // Health, armour, on fire and is in water
    if (Data.ucFlags & 0x08)
        BitStream.Write(Data.fHealth);
    if (Data.ucFlags & 0x10)
        BitStream.Write(Data.fArmor);

    if (Data.flags2 & 0x01)
    {
        SCameraRotationSync camRotation;
        camRotation.data.fRotation = Data.cameraRotation;
        BitStream.Write(&camRotation);
    }

    if (Data.ucFlags & 0x20)
        BitStream.WriteBit(Data.bOnFire);
    if (Data.ucFlags & 0x60 && BitStream.Can(eBitStreamVersion::IsPedReloadingWeapon))
        BitStream.Write(Data.isReloadingWeapon);
    if (Data.ucFlags & 0x40)
        BitStream.Write(Data.bIsInWater);

    return true;
}

bool CPedSyncPacket::SyncData::ReadSpatialData(NetBitStreamInterface& BitStream)
{
    // Did we recieve position?
    if (ucFlags & 0x01)
    {            
        if (!BitStream.Read(&position))
            return false;
    }

    // Rotation
    if (ucFlags & 0x02)
    {            
        if (!BitStream.Read(&rotation))
            return false;
    }

    // Velocity
    if (ucFlags & 0x04)
    {           
        if (!BitStream.Read(&velocity))
            return false;
    }

    return true;
}

bool CPedSyncPacket::SyncData::ReadSpatialDataBC(NetBitStreamInterface& BitStream)
{
    // Did we recieve position?
    if (ucFlags & 0x01)
    {
        if (!BitStream.Read(position.data.vecPosition.fX) || !BitStream.Read(position.data.vecPosition.fY) || !BitStream.Read(position.data.vecPosition.fZ))
            return false;
    }

    // Rotation
    if (ucFlags & 0x02)
    {
        if (!BitStream.Read(rotation.data.fRotation))
            return false;
    }

    // Velocity
    if (ucFlags & 0x04)
    {
        if (!BitStream.Read(velocity.data.vecVelocity.fX) || !BitStream.Read(velocity.data.vecVelocity.fY) || !BitStream.Read(velocity.data.vecVelocity.fZ))
            return false;
    }

    return true;
}
