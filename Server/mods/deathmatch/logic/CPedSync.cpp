/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CPedSync.cpp
 *  PURPOSE:     Ped entity synchronization class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPedSync.h"
#include "Utils.h"
#include "CElementIDs.h"
#include "CTickRateSettings.h"
#include "packets/CPedStartSyncPacket.h"
#include "packets/CPedStopSyncPacket.h"
#include "CGame.h"
#include "CColManager.h"
#include "CSpatialDatabase.h"
#include "CPlayerCamera.h"

CPedSync::CPedSync(CPlayerManager* pPlayerManager, CPedManager* pPedManager)
{
    m_pPlayerManager = pPlayerManager;
    m_pPedManager = pPedManager;
}

void CPedSync::DoPulse()
{
    // Time to update lists of players near peds?
    if (m_UpdateNearListTimer.Get() > 1000)
    {
        m_UpdateNearListTimer.Reset();
        UpdateNearPlayersList();
    }

    // Time to check for players that should no longer be syncing a ped or peds that should be synced?
    if (m_UpdateSyncerTimer.Get() > 500)
    {
        m_UpdateSyncerTimer.Reset();
        UpdateAllSyncer();
    }
}

bool CPedSync::ProcessPacket(CPacket& Packet)
{
    if (Packet.GetPacketID() == PACKET_ID_PED_SYNC)
    {
        Packet_PedSync(static_cast<CPedSyncPacket&>(Packet));
        return true;
    }

    return false;
}

void CPedSync::OverrideSyncer(CPed* pPed, CPlayer* pPlayer, bool bPersist)
{
    CPlayer* pSyncer = pPed->GetSyncer();
    if (pSyncer)
    {
        if (pSyncer == pPlayer)
        {
            if (bPersist == false)
            {
                SetSyncerAsPersistent(false);
            }

            return;
        }

        StopSync(pPed);
    }

    if (pPlayer && !pPed->IsBeingDeleted())
    {
        SetSyncerAsPersistent(bPersist);
        StartSync(pPlayer, pPed);
    }
}

void CPedSync::UpdateAllSyncer()
{
    auto currentTick = GetTickCount64_();

    // Update all the ped's sync states
    for (auto iter = m_pPedManager->IterBegin(); iter != m_pPedManager->IterEnd(); iter++)
    {
        // Has the duration of the ped's animation already elapsed?
        const SPlayerAnimData& animData = (*iter)->GetAnimationData();
        if (animData.IsAnimating())
        {
            float deltaTime = currentTick - animData.startedTick;
            if (!animData.freezeLastFrame && animData.time > 0 && deltaTime >= animData.time)
                (*iter)->SetAnimationData({});
        }

        // It is a ped, yet not a player
        if (IS_PED(*iter) && !IS_PLAYER(*iter))
            UpdateSyncer(*iter);
    }
}

void CPedSync::UpdateSyncer(CPed* pPed)
{
    CPlayer* pSyncer = pPed->GetSyncer();

    // Handle no syncing
    if (!pPed->IsSyncable())
    {
        // This ped got a syncer?
        if (pSyncer)
        {
            // Tell the syncer to stop syncing
            StopSync(pPed);
        }
        return;
    }

    // This ped got a syncer?
    if (pSyncer)
    {
        // Is he close enough, and in the right dimension?
        if (IsSyncerPersistent() || (pPed->GetDimension() == pSyncer->GetDimension() &&
                                     IsPointNearPoint3D(pSyncer->GetPosition(), pPed->GetPosition(), (float)g_TickRateSettings.iPedSyncerDistance)))
            return;

        // Stop him from syncing it
        StopSync(pPed);
    }

    if (pPed->IsBeingDeleted())
        return;

    // Find a new syncer for it
    FindSyncer(pPed);
}

void CPedSync::FindSyncer(CPed* pPed)
{
    assert(pPed->IsSyncable());

    // Find a player close enough to him
    CPlayer* pPlayer = FindPlayerCloseToPed(pPed, g_TickRateSettings.iPedSyncerDistance - 20.0f);
    if (pPlayer)
    {
        // Tell him to start syncing it
        StartSync(pPlayer, pPed);
    }
}

void CPedSync::StartSync(CPlayer* pPlayer, CPed* pPed)
{
    if (!pPed->IsSyncable())
        return;

    // Tell the player
    pPlayer->Send(CPedStartSyncPacket(pPed));

    // Mark him as the syncing player
    pPed->SetSyncer(pPlayer);

    // Call the onElementStartSync event
    CLuaArguments Arguments;
    Arguments.PushElement(pPlayer);            // New syncer
    pPed->CallEvent("onElementStartSync", Arguments);
}

void CPedSync::StopSync(CPed* pPed)
{
    // Tell the player that used to sync it
    CPlayer* pSyncer = pPed->GetSyncer();
    pSyncer->Send(CPedStopSyncPacket(pPed->GetID()));

    // Unmark him as the syncing player
    pPed->SetSyncer(NULL);

    SetSyncerAsPersistent(false);

    // Call the onElementStopSync event
    CLuaArguments Arguments;
    Arguments.PushElement(pSyncer);            // Old syncer
    pPed->CallEvent("onElementStopSync", Arguments);
}

CPlayer* CPedSync::FindPlayerCloseToPed(CPed* pPed, float fMaxDistance)
{
    // Grab the ped position
    CVector vecPedPosition = pPed->GetPosition();

    // See if any players are close enough
    CPlayer* pLastPlayerSyncing = nullptr;
    CPlayer* pPlayer = nullptr;
    for (auto iter = m_pPlayerManager->IterBegin(); iter != m_pPlayerManager->IterEnd(); iter++)
    {
        pPlayer = *iter;
        // Is he joined?
        if (!pPlayer->IsJoined())
            continue;

        // Same dimension?
        if (pPlayer->GetDimension() != pPed->GetDimension())
            continue;

        // He's near enough?
        if (!IsPointNearPoint3D(vecPedPosition, pPlayer->GetPosition(), fMaxDistance))
            continue;

        // He syncs less peds than the previous player close enough?
        if (!pLastPlayerSyncing || pPlayer->CountSyncingPeds() < pLastPlayerSyncing->CountSyncingPeds())
            pLastPlayerSyncing = pPlayer;
    }

    // Return the player we found that syncs the least number of peds
    return pLastPlayerSyncing;
}

void CPedSync::Packet_PedSync(CPedSyncPacket& Packet)
{
    // Grab the player
    CPlayer* pPlayer = Packet.GetSourcePlayer();
    if (!pPlayer || !pPlayer->IsJoined())
        return;

    // Grab the tick count
    long long llTickCountNow = GetModuleTickCount64();

    // Apply the data for each ped in the packet
    for (auto& Data : Packet.m_Syncs)
    {
        // Grab the ped this packet is for
        CElement* pPedElement = CElementIDs::GetElement(Data.ID);
        if (!pPedElement || !IS_PED(pPedElement))
            continue;

        // Convert to a CPed
        CPed* pPed = static_cast<CPed*>(pPedElement);

        // Is the player syncing this ped?
        // Check if the time context matches.
        if (pPed->GetSyncer() != pPlayer || !pPed->CanUpdateSync(Data.ucSyncTimeContext))
            continue;

        // Apply the data to the ped
        if (Data.ucFlags & 0x01)
        {
            pPed->SetPosition(Data.position.data.vecPosition);
            g_pGame->GetColManager()->DoHitDetection(pPed->GetPosition(), pPed);
        }
        if (Data.ucFlags & 0x02)
            pPed->SetRotation(Data.rotation.data.fRotation);
        if (Data.ucFlags & 0x04)
            pPed->SetVelocity(Data.velocity.data.vecVelocity);

        if (Data.ucFlags & 0x08)
        {
            // Less health than last time?
            float fPreviousHealth = pPed->GetHealth();
            pPed->SetHealth(Data.fHealth);

            if (Data.fHealth < fPreviousHealth)
            {
                // Grab the delta health
                float fDeltaHealth = fPreviousHealth - Data.fHealth;

                if (fDeltaHealth > 0.0f)
                {
                    // Call the onPedDamage event
                    CLuaArguments Arguments;
                    Arguments.PushNumber(fDeltaHealth);
                    pPed->CallEvent("onPedDamage", Arguments);
                }
            }
        }

        if (Data.ucFlags & 0x10)
            pPed->SetArmor(Data.fArmor);

        if (Data.flags2 & 0x01)
            pPed->SetCameraRotation(Data.cameraRotation);

        if (Data.ucFlags & 0x20)
            pPed->SetOnFire(Data.bOnFire);

        if (Data.ucFlags & 0x40)
            pPed->SetInWater(Data.bIsInWater);

        if (Data.ucFlags & 0x60)
        {
            pPed->SetReloadingWeapon(Data.isReloadingWeapon);
        }

        if (Data.ucFlags & 0x80)
            pPed->SetAnimationData({});

        // Is it time to sync to everyone
        bool bDoFarSync = llTickCountNow - pPed->GetLastFarSyncTick() >= g_TickRateSettings.iPedFarSync;

        if (!bDoFarSync && pPed->IsNearPlayersListEmpty())
            continue;

        // Create a new packet, containing only the struct for this ped
        CPedSyncPacket PedPacket(Data);
        if (!&PedPacket)
            continue;

        if (bDoFarSync)
        {
            // Store the tick
            pPed->SetLastFarSyncTick(llTickCountNow);
            // Send to everyone
            m_pPlayerManager->BroadcastOnlyJoined(PedPacket, pPlayer);
            continue;
        }

        // Send to players nearby the ped
        CSendList sendList;
        for (auto iter = pPed->NearPlayersIterBegin(); iter != pPed->NearPlayersIterEnd(); iter++)
        {
            CPlayer* pRemotePlayer = *iter;
            // If the syncer changes between UpdateNearPlayersList() he can be in the list, make sure we don't send to him
            if (pRemotePlayer && pRemotePlayer != pPlayer)
                sendList.push_back(pRemotePlayer);
        }

        if (!sendList.empty())
            m_pPlayerManager->Broadcast(PedPacket, sendList);
    }
}

void CPedSync::UpdateNearPlayersList()
{
    for (auto iter = m_pPedManager->IterBegin(); iter != m_pPedManager->IterEnd(); iter++)
    {
        CPed* pPed = *iter;
        // Clear the list
        pPed->ClearNearPlayersList();
    }

    for (auto iter = m_pPlayerManager->IterBegin(); iter != m_pPlayerManager->IterEnd(); iter++)
    {
        CPlayer* pPlayer = *iter;
        if (!pPlayer->IsJoined() || pPlayer->IsBeingDeleted())
            continue;

        // Grab the camera position
        CVector vecCameraPosition;
        pPlayer->GetCamera()->GetPosition(vecCameraPosition);

        // Do a query in the spatial database
        CElementResult resultNearCamera;
        GetSpatialDatabase()->SphereQuery(resultNearCamera, CSphere(vecCameraPosition, DISTANCE_FOR_NEAR_VIEWER));

        for (CElement* pElement : resultNearCamera)
        {
            // Make sure it's a ped
            if (pElement->GetType() != CElement::PED)
                continue;

            CPed* pPed = static_cast<CPed*>(pElement);

            // Check dimension matches
            if (pPlayer->GetDimension() != pPed->GetDimension())
                continue;

            // If the player is syncing it, don't add the player
            if (pPed->GetSyncer() == pPlayer)
                continue;

            // Check distance accurately because the spatial database is 2D
            if ((vecCameraPosition - pPed->GetPosition()).LengthSquared() < DISTANCE_FOR_NEAR_VIEWER * DISTANCE_FOR_NEAR_VIEWER)
                pPed->AddPlayerToNearList(pPlayer);
        }
    }
}
