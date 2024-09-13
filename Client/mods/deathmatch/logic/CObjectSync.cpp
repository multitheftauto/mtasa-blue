/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CObjectSync.cpp
 *  PURPOSE:     Object sync class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;
extern CClientGame* g_pClientGame;

#define OBJECT_SYNC_RATE (g_TickRateSettings.iObjectSync)

CObjectSync::CObjectSync(CClientObjectManager* pObjectManager) : m_pObjectManager(pObjectManager), m_ulLastSyncTime(0)
{
}

bool CObjectSync::ProcessPacket(unsigned char ucPacketID, NetBitStreamInterface& bitStream)
{
    switch (ucPacketID)
    {
        case PACKET_ID_OBJECT_STARTSYNC:
        {
            Packet_ObjectStartSync(bitStream);
            return true;
        }

        case PACKET_ID_OBJECT_STOPSYNC:
        {
            Packet_ObjectStopSync(bitStream);
            return true;
        }

        case PACKET_ID_OBJECT_SYNC:
        {
            Packet_ObjectSync(bitStream);
            return true;
        }
    }

    return false;
}

void CObjectSync::DoPulse()
{
    unsigned long ulCurrentTime = CClientTime::GetTime();
    if (ulCurrentTime >= m_ulLastSyncTime + OBJECT_SYNC_RATE)
    {
        Update();
        m_ulLastSyncTime = ulCurrentTime;
    }
}

void CObjectSync::AddObject(CDeathmatchObject* pObject)
{
    m_List.push_front(pObject);
}

void CObjectSync::RemoveObject(CDeathmatchObject* pObject)
{
    if (!m_List.empty())
        m_List.remove(pObject);
}

bool CObjectSync::Exists(CDeathmatchObject* pObject)
{
    return m_List.Contains(pObject);
}

void CObjectSync::Packet_ObjectStartSync(NetBitStreamInterface& bitStream)
{
    // Read out the element ID
    ElementID ID;
    if (!bitStream.Read(ID))
        return;

    // Grab the object
    auto pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(ID));
    if (!pObject)
        return;

    // Read out the data
    SPositionSync        position;
    SRotationRadiansSync rotation;
    SVelocitySync        velocity;
    SVelocitySync        angularVelocity;
    SObjectHealthSync    health;

    if (!bitStream.Read(&position) || !bitStream.Read(&rotation) || !bitStream.Read(&velocity) || !bitStream.Read(&angularVelocity) || !bitStream.Read(&health))
        return;

    // Set data from the server
    pObject->SetOrientation(position.data.vecPosition, rotation.data.vecRotation);
    pObject->SetHealth(health.data.fValue);
    pObject->SetMoveSpeed(velocity.data.vecVelocity);
    pObject->SetTurnSpeed(angularVelocity.data.vecVelocity);

    // Add object to the sync list
    AddObject(pObject);
}

void CObjectSync::Packet_ObjectStopSync(NetBitStreamInterface& bitStream)
{
    // Read out the element ID
    ElementID ID;
    if (!bitStream.Read(ID))
        return;

    // Grab the object
    auto* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(ID));
    if (!pObject)
        return;

    // Remove object from the sync list
    RemoveObject(pObject);
}

void CObjectSync::Packet_ObjectSync(NetBitStreamInterface& bitStream)
{
    // While we're not out of bytes
    while (bitStream.GetNumberOfUnreadBits() > 8)
    {
        // Read out the element ID
        ElementID ID;
        if (!bitStream.Read(ID))
            return;

        // Read out the sync time context
        unsigned char ucSyncTimeContext;
        if (!bitStream.Read(ucSyncTimeContext))
            return;

        // Read out the flags
        SIntegerSync<unsigned char, 5> ucFlags(0);
        if (!bitStream.Read(&ucFlags))
            return;

        // Read out the position
        SPositionSync position;
        if (ucFlags & 0x1)
        {
            if (!bitStream.Read(&position))
                return;
        }

        // Read out the rotation
        SRotationRadiansSync rotation;
        if (ucFlags & 0x2)
        {
            if (!bitStream.Read(&rotation))
                return;
        }

        // Read out the velocity
        SVelocitySync velocity;
        if (ucFlags & 0x4)
        {
            if (!bitStream.Read(&velocity))
                return;
        }

        // Read out the angular velocity
        SVelocitySync angularVelocity;
        if (ucFlags & 0x8)
        {
            if (!bitStream.Read(&angularVelocity))
                return;
        }

        // Read out the health
        SObjectHealthSync health;
        if (ucFlags & 0x10)
        {
            if (!bitStream.Read(&health))
                return;
        }

        // Grab the object.
        auto* pObject = static_cast<CDeathmatchObject*>(m_pObjectManager->Get(ID));

        // Only update the sync if this packet is from the same context
        if (!pObject || !pObject->CanUpdateSync(ucSyncTimeContext))
            return;

        // Set data
        if (ucFlags & 0x1)
            pObject->SetPosition(position.data.vecPosition);
        if (ucFlags & 0x2)
            pObject->SetRotationRadians(rotation.data.vecRotation);
        if (ucFlags & 0x4)
            pObject->SetMoveSpeed(velocity.data.vecVelocity);
        if (ucFlags & 0x8)
            pObject->SetTurnSpeed(angularVelocity.data.vecVelocity);
        if (ucFlags & 0x10)
            pObject->SetHealth(health.data.fValue);
    }
}

void CObjectSync::Update()
{
    // Got any objects to sync?
    if (m_List.size() > 0)
    {
        // Create packet
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (!pBitStream)
            return;

        // Write object information
        list<CDeathmatchObject*>::const_iterator iter = m_List.begin();
        for (; iter != m_List.end(); iter++)
        {
            WriteObjectInformation(pBitStream, *iter);
        }

        // Send and destroy the packet
        g_pNet->SendPacket(PACKET_ID_OBJECT_SYNC, pBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED);
        g_pNet->DeallocateNetBitStream(pBitStream);
    }
}

void CObjectSync::WriteObjectInformation(NetBitStreamInterface* pBitStream, CDeathmatchObject* pObject)
{
    CVector vecPosition, vecRotation, vecVelocity, vecAngularVelocity;
    pObject->GetPosition(vecPosition);
    pObject->GetRotationRadians(vecRotation);
    pObject->GetMoveSpeed(vecVelocity);
    pObject->GetTurnSpeed(vecAngularVelocity);

    unsigned int ucFlags = 0;

    if (vecPosition != pObject->m_LastSyncedData.vecPosition)
        ucFlags |= 0x1;
    if (vecRotation != pObject->m_LastSyncedData.vecRotation)
        ucFlags |= 0x2;
    if (vecVelocity != pObject->m_LastSyncedData.vecVelocity)
        ucFlags |= 0x4;
    if (vecAngularVelocity != pObject->m_LastSyncedData.vecTurnVelocity)
        ucFlags |= 0x8;
    if (pObject->GetHealth() != pObject->m_LastSyncedData.fHealth)
        ucFlags |= 0x10;
    if (pObject->IsInWater() != pObject->m_LastSyncedData.bIsInWater)
        ucFlags |= 0x20;

    // Don't sync if nothing changed
    if (ucFlags == 0)
        return;

    // Write the object ID
    pBitStream->Write(pObject->GetID());

    // Write the sync time context
    pBitStream->Write(pObject->GetSyncTimeContext());

    // Write flags
    //SIntegerSync<unsigned char, 12> flags(ucFlags);
    pBitStream->Write(ucFlags);

    // Write position
    if (ucFlags & 0x1)
    {
        SPositionSync position(false);
        pObject->GetPosition(position.data.vecPosition);
        pBitStream->Write(&position);

        pObject->m_LastSyncedData.vecPosition = position.data.vecPosition;
    }

    // Write rotation
    if (ucFlags & 0x2)
    {
        SRotationRadiansSync rotation;
        pObject->GetRotationRadians(rotation.data.vecRotation);
        pBitStream->Write(&rotation);

        pObject->m_LastSyncedData.vecRotation = rotation.data.vecRotation;
    }

    // Write velocity
    if (ucFlags & 0x4)
    {
        SVelocitySync velocity;
        pObject->GetMoveSpeed(velocity.data.vecVelocity);
        pBitStream->Write(&velocity);

        pObject->m_LastSyncedData.vecVelocity = velocity.data.vecVelocity;
    }

    // Write angular velocity
    if (ucFlags & 0x8)
    {
        SVelocitySync angularVelocity;
        pObject->GetTurnSpeed(angularVelocity.data.vecVelocity);
        pBitStream->Write(&angularVelocity);

        pObject->m_LastSyncedData.vecTurnVelocity = angularVelocity.data.vecVelocity;
    }

    // Write health & attacker
    if (ucFlags & 0x10)
    {
        SObjectHealthSync health;
        health.data.fValue = pObject->GetHealth();
        pBitStream->Write(&health);
        pBitStream->Write(pObject->GetAttackerID());

        pObject->m_LastSyncedData.fHealth = health.data.fValue;
    }

    // Write inWater state
    if (ucFlags & 0x20)
    {
        bool bIsInWater = pObject->IsInWater();
        pBitStream->WriteBit(bIsInWater);

        pObject->m_LastSyncedData.bIsInWater = bIsInWater;
    }
}
