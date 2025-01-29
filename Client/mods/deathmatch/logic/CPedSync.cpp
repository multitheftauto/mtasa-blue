/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPedSync.cpp
 *  PURPOSE:     Ped synchronization handler
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using std::list;

extern CClientGame* g_pClientGame;

#define PED_SYNC_RATE   ( g_TickRateSettings.iPedSync )

CPedSync::CPedSync(CClientPedManager* pPedManager)
{
    m_pPedManager = pPedManager;
    m_ulLastSyncTime = 0;
}

CPedSync::~CPedSync()
{
}

bool CPedSync::ProcessPacket(unsigned char ucPacketID, NetBitStreamInterface& BitStream)
{
    switch (ucPacketID)
    {
        case PACKET_ID_PED_STARTSYNC:
        {
            Packet_PedStartSync(BitStream);
            return true;
        }

        case PACKET_ID_PED_STOPSYNC:
        {
            Packet_PedStopSync(BitStream);
            return true;
        }

        case PACKET_ID_PED_SYNC:
        {
            Packet_PedSync(BitStream);
            return true;
        }
    }

    return false;
}

void CPedSync::DoPulse()
{
    // Got any items?
    if (m_List.size() > 0)
    {
        for (auto iter = m_List.begin(); iter != m_List.end(); ++iter)
        {
            CClientPed* pPed = *iter;
            // Update enter/exit sequence
            pPed->UpdateVehicleInOut();
        }
    }

    // Has it been long enough since our last state's sync?
    unsigned long ulCurrentTime = CClientTime::GetTime();
    if (ulCurrentTime >= m_ulLastSyncTime + PED_SYNC_RATE)
    {
        Update();
        m_ulLastSyncTime = ulCurrentTime;
    }
}

void CPedSync::AddPed(CClientPed* pPed)
{
    m_List.push_front(pPed);
    pPed->SetSyncing(true);
}

void CPedSync::RemovePed(CClientPed* pPed)
{
    if (!m_List.empty())
        m_List.remove(pPed);

    pPed->SetSyncing(false);
}

bool CPedSync::Exists(CClientPed* pPed)
{
    return m_List.Contains(pPed);
}

void CPedSync::Packet_PedStartSync(NetBitStreamInterface& BitStream)
{
    // Read out the element id
    ElementID ID;
    if (BitStream.Read(ID))
    {
        // Grab the ped
        CClientPed* pPed = static_cast<CClientPed*>(m_pPedManager->Get(ID));
        if (pPed)
        {
            // Read out the position
            CVector vecPosition;
            BitStream.Read(vecPosition.fX);
            BitStream.Read(vecPosition.fY);
            BitStream.Read(vecPosition.fZ);

            // And rotation
            float fRotation;
            BitStream.Read(fRotation);

            // And the velocity
            CVector vecVelocity;
            BitStream.Read(vecVelocity.fX);
            BitStream.Read(vecVelocity.fY);
            BitStream.Read(vecVelocity.fZ);

            // And health/armor
            float fHealth, fArmor;
            BitStream.Read(fHealth);
            BitStream.Read(fArmor);
            
            if (BitStream.Can(eBitStreamVersion::PedSync_CameraRotation))
            {
                float cameraRotation{};
                BitStream.Read(cameraRotation);

                pPed->SetCameraRotation(cameraRotation);
            }

            // Set data
            pPed->SetPosition(vecPosition);
            pPed->SetCurrentRotation(fRotation);
            pPed->SetMoveSpeed(vecVelocity);

            // Unlock health and armour for the syncer
            pPed->UnlockHealth();
            pPed->UnlockArmor();

            // Set the new health
            pPed->SetHealth(fHealth);
            pPed->SetArmor(fArmor);

            AddPed(pPed);
        }
    }
}

void CPedSync::Packet_PedStopSync(NetBitStreamInterface& BitStream)
{
    // Read out the ped id
    ElementID ID;
    if (BitStream.Read(ID))
    {
        // Grab the ped
        CClientPed* pPed = static_cast<CClientPed*>(m_pPedManager->Get(ID));
        if (pPed)
        {
            // Lock health and armour
            pPed->LockHealth(pPed->GetHealth());
            pPed->LockArmor(pPed->GetArmor());

            // Stop syncing it
            RemovePed(pPed);
        }
    }
}

void CPedSync::Packet_PedSync(NetBitStreamInterface& BitStream)
{
    // While we're not out of peds
    while (BitStream.GetNumberOfUnreadBits() > 32)
    {
        // Read out the ped id
        ElementID ID;
        if (BitStream.Read(ID))
        {
            // Read out the sync time context. See CClientEntity for documentation on that.
            unsigned char ucSyncTimeContext = 0;
            BitStream.Read(ucSyncTimeContext);

            unsigned char ucFlags = 0;
            BitStream.Read(ucFlags);

            std::uint8_t flags2{};
            if (BitStream.Can(eBitStreamVersion::PedSync_CameraRotation))
                BitStream.Read(flags2);

            CVector vecPosition{ CVector::NoInit{} }, vecMoveSpeed{ CVector::NoInit{} };
            float   fRotation, fHealth, fArmor;
            bool    bOnFire;
            bool    bIsInWater;
            float   cameraRotation;

            if (BitStream.Can(eBitStreamVersion::PedSync_Revision))
            {
                // Read out the position
                SPositionSync position(false);
                if (ucFlags & 0x01)
                    BitStream.Read(&position);

                // And rotation
                SPedRotationSync rotation;
                if (ucFlags & 0x02)
                    BitStream.Read(&rotation);

                // And the move speed
                SVelocitySync velocity;
                if (ucFlags & 0x04)
                    BitStream.Read(&velocity);

                vecPosition = position.data.vecPosition;
                fRotation = rotation.data.fRotation;
                vecMoveSpeed = velocity.data.vecVelocity;
            }
            else
            {
                if (ucFlags & 0x01)
                {
                    BitStream.Read(vecPosition.fX);
                    BitStream.Read(vecPosition.fY);
                    BitStream.Read(vecPosition.fZ);
                }

                if (ucFlags & 0x02)
                    BitStream.Read(fRotation);

                if (ucFlags & 0x04)
                {
                    BitStream.Read(vecMoveSpeed.fX);
                    BitStream.Read(vecMoveSpeed.fY);
                    BitStream.Read(vecMoveSpeed.fZ);
                }
            }

            // And health with armour
            if (ucFlags & 0x08)
                BitStream.Read(fHealth);
            if (ucFlags & 0x10)
                BitStream.Read(fArmor);

            if (flags2 & 0x01)
            {
                SCameraRotationSync camRotation;
                BitStream.Read(&camRotation);
                cameraRotation = camRotation.data.fRotation;
            }

            // And the burning state
            if (BitStream.Version() >= 0x04E && ucFlags & 0x20)
                BitStream.ReadBit(bOnFire);

            // And the in water state
            if (BitStream.Version() >= 0x55 && ucFlags & 0x40)
                BitStream.ReadBit(bIsInWater);

            // Grab the ped. Only update the sync if this packet is from the same context.
            CClientPed* pPed = m_pPedManager->Get(ID);
            if (pPed && pPed->CanUpdateSync(ucSyncTimeContext))
            {
                if (ucFlags & 0x01)
                    pPed->SetTargetPosition(vecPosition, PED_SYNC_RATE);
                if (ucFlags & 0x02)
                    pPed->SetTargetRotation(PED_SYNC_RATE, fRotation, std::nullopt);
                if (ucFlags & 0x04)
                    pPed->SetMoveSpeed(vecMoveSpeed);
                if (ucFlags & 0x08)
                    pPed->LockHealth(fHealth);
                if (ucFlags & 0x10)
                    pPed->LockArmor(fArmor);
                if (flags2 & 0x01)
                    pPed->SetTargetRotation(PED_SYNC_RATE, std::nullopt, cameraRotation);
                if (BitStream.Version() >= 0x04E && ucFlags & 0x20)
                    pPed->SetOnFire(bOnFire);
                if (BitStream.Version() >= 0x55 && ucFlags & 0x40)
                    pPed->SetInWater(bIsInWater);
            }
        }
    }
}

void CPedSync::Update()
{
    // Got any items?
    if (m_List.size() > 0)
    {
        // Create a packet
        NetBitStreamInterface* pBitStream = g_pNet->AllocateNetBitStream();
        if (pBitStream)
        {
            // Write each ped to it
            list<CClientPed*>::const_iterator iter = m_List.begin();
            for (; iter != m_List.end(); ++iter)
            {
                WritePedInformation(pBitStream, *iter);
            }

            // Send and destroy the packet
            g_pNet->SendPacket(PACKET_ID_PED_SYNC, pBitStream, PACKET_PRIORITY_MEDIUM, PACKET_RELIABILITY_UNRELIABLE_SEQUENCED);
            g_pNet->DeallocateNetBitStream(pBitStream);
        }
    }
}

void CPedSync::WritePedInformation(NetBitStreamInterface* pBitStream, CClientPed* pPed)
{
    CVector vecPosition;
    pPed->GetPosition(vecPosition);
    CVector vecVelocity;
    pPed->GetMoveSpeed(vecVelocity);

    unsigned char ucFlags = 0;
    if (vecPosition != pPed->m_LastSyncedData->vPosition)
        ucFlags |= 0x01;
    if (pPed->GetCurrentRotation() != pPed->m_LastSyncedData->fRotation)
        ucFlags |= 0x02;
    if (vecVelocity != pPed->m_LastSyncedData->vVelocity)
        ucFlags |= 0x04;
    if (pPed->GetHealth() != pPed->m_LastSyncedData->fHealth)
        ucFlags |= 0x08;
    if (pPed->GetArmor() != pPed->m_LastSyncedData->fArmour)
        ucFlags |= 0x10;
    if (pPed->IsOnFire() != pPed->m_LastSyncedData->bOnFire)
        ucFlags |= 0x20;
    if (pPed->IsInWater() != pPed->m_LastSyncedData->bIsInWater)
        ucFlags |= 0x40;
    if (pPed->IsReloadingWeapon() != pPed->m_LastSyncedData->isReloadingWeapon && pBitStream->Can(eBitStreamVersion::IsPedReloadingWeapon))
        ucFlags |= 0x60;
    if (pPed->HasSyncedAnim() && (!pPed->IsRunningAnimation() || pPed->m_animationOverridedByClient))
        ucFlags |= 0x80;

    std::uint8_t flags2{};
    if (!IsNearlyEqual(pPed->GetCameraRotation(), pPed->m_LastSyncedData->cameraRotation) && pBitStream->Can(eBitStreamVersion::PedSync_CameraRotation))
        flags2 |= 0x01;

    // Do we really have to sync this ped?
    if (ucFlags == 0 && flags2 == 0)
        return;

    // Write the ped id
    pBitStream->Write(pPed->GetID());

    // Write the sync time context
    pBitStream->Write(pPed->GetSyncTimeContext());

    // Write flags
    pBitStream->Write(ucFlags);

    // Write flags 2
    if (pBitStream->Can(eBitStreamVersion::PedSync_CameraRotation))
        pBitStream->Write(flags2);

    // Write position if needed
    if (ucFlags & 0x01)
    {
        if (pBitStream->Can(eBitStreamVersion::PedSync_Revision))
        {
            SPositionSync position(false);
            position.data.vecPosition = vecPosition;
            pBitStream->Write(&position);
        }
        else
        {
            pBitStream->Write(vecPosition.fX);
            pBitStream->Write(vecPosition.fY);
            pBitStream->Write(vecPosition.fZ);
        }

        pPed->m_LastSyncedData->vPosition = vecPosition;
    }

    if (ucFlags & 0x02)
    {
        if (pBitStream->Can(eBitStreamVersion::PedSync_Revision))
        {
            SPedRotationSync rotation;
            rotation.data.fRotation = pPed->GetCurrentRotation();
            pBitStream->Write(&rotation);
        }
        else
            pBitStream->Write(pPed->GetCurrentRotation());

        pPed->m_LastSyncedData->fRotation = pPed->GetCurrentRotation();
    }

    // Write velocity
    if (ucFlags & 0x04)
    {
        if (pBitStream->Can(eBitStreamVersion::PedSync_Revision))
        {
            SVelocitySync velocity;
            pBitStream->Write(&velocity);
        }
        else
        {
            pBitStream->Write(vecVelocity.fX);
            pBitStream->Write(vecVelocity.fY);
            pBitStream->Write(vecVelocity.fZ);
        }

        pPed->m_LastSyncedData->vVelocity = vecVelocity;
    }

    // And health
    if (ucFlags & 0x08)
    {
        pBitStream->Write(pPed->GetHealth());
        pPed->m_LastSyncedData->fHealth = pPed->GetHealth();
    }

    if (ucFlags & 0x10)
    {
        pBitStream->Write(pPed->GetArmor());
        pPed->m_LastSyncedData->fArmour = pPed->GetArmor();
    }

    if (flags2 & 0x01)
    {
        SCameraRotationSync camRotation;
        camRotation.data.fRotation = pPed->GetCameraRotation();
        pBitStream->Write(&camRotation);
        pPed->m_LastSyncedData->cameraRotation = camRotation.data.fRotation;
    }

    if (ucFlags & 0x20 && pBitStream->Version() >= 0x04E)
    {
        pBitStream->WriteBit(pPed->IsOnFire());
        pPed->m_LastSyncedData->bOnFire = pPed->IsOnFire();
    }

    if (ucFlags & 0x40 && pBitStream->Version() >= 0x55)
    {
        pBitStream->WriteBit(pPed->IsInWater());
        pPed->m_LastSyncedData->bIsInWater = pPed->IsInWater();
    }

    if (ucFlags & 0x60 && pBitStream->Can(eBitStreamVersion::IsPedReloadingWeapon))
    {
        bool isReloadingWeapon = pPed->IsReloadingWeapon();

        pBitStream->WriteBit(isReloadingWeapon);
        pPed->m_LastSyncedData->isReloadingWeapon = isReloadingWeapon;
    }

    // The animation has been overwritten or interrupted by the client
    if (ucFlags & 0x80 && pBitStream->Can(eBitStreamVersion::AnimationsSync))
    {
        pPed->SetHasSyncedAnim(false);
        pPed->m_animationOverridedByClient = false;
    }
}
