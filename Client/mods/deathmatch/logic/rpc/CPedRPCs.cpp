/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/rpc/CPedRPCs.cpp
 *  PURPOSE:     Ped remote procedure calls
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CAnimManager.h>
#include "CPedRPCs.h"
#include "net/SyncStructures.h"

class CLuaArguments;

void CPedRPCs::LoadFunctions()
{
    AddHandler(SET_PED_ARMOR, SetPedArmor, "SetPedArmor");
    AddHandler(SET_PED_ROTATION, SetPedRotation, "SetPedRotation");
    AddHandler(GIVE_PED_JETPACK, GivePedJetPack, "GivePedJetPack");
    AddHandler(REMOVE_PED_JETPACK, RemovePedJetPack, "RemovePedJetPack");
    AddHandler(REMOVE_PED_CLOTHES, RemovePedClothes, "RemovePedClothes");
    AddHandler(SET_PED_GRAVITY, SetPedGravity, "SetPedGravity");
    AddHandler(SET_PED_CHOKING, SetPedChoking, "SetPedChoking");
    AddHandler(SET_PED_FIGHTING_STYLE, SetPedFightingStyle, "SetPedFightingStyle");
    AddHandler(SET_PED_MOVE_ANIM, SetPedMoveAnim, "SetPedMoveAnim");
    AddHandler(WARP_PED_INTO_VEHICLE, WarpPedIntoVehicle, "WarpPedIntoVehicle");
    AddHandler(REMOVE_PED_FROM_VEHICLE, RemovePedFromVehicle, "RemovePedFromVehicle");
    AddHandler(SET_PED_DOING_GANG_DRIVEBY, SetPedDoingGangDriveby, "SetPedDoingGangDriveby");
    AddHandler(SET_PED_ANIMATION, SetPedAnimation, "SetPedAnimation");
    AddHandler(SET_PED_ANIMATION_PROGRESS, SetPedAnimationProgress, "SetPedAnimationProgress");
    AddHandler(SET_PED_ANIMATION_SPEED, SetPedAnimationSpeed, "SetPedAnimationSpeed");
    AddHandler(SET_PED_ON_FIRE, SetPedOnFire, "SetPedOnFire");
    AddHandler(SET_PED_HEADLESS, SetPedHeadless, "SetPedHeadless");
    AddHandler(SET_PED_FROZEN, SetPedFrozen, "SetPedFrozen");
    AddHandler(RELOAD_PED_WEAPON, ReloadPedWeapon, "ReloadPedWeapon");
}

void CPedRPCs::SetPedArmor(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned char ucArmor;
    unsigned char ucTimeContext;
    if (bitStream.Read(ucArmor) && bitStream.Read(ucTimeContext))
    {
        float fArmor = static_cast<float>(ucArmor) / 1.25f;

        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetSyncTimeContext(ucTimeContext);
            if (pPed->IsArmorLocked())
                pPed->LockArmor(fArmor);
            else
                pPed->SetArmor(fArmor);
        }
    }
}

void CPedRPCs::SetPedRotation(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    SPedRotationSync rotation;
    unsigned char    ucTimeContext;
    if (bitStream.Read(&rotation) && bitStream.Read(ucTimeContext))
    {
        uchar ucNewWay = 0;
        if (bitStream.GetNumberOfBytesUsed() > 0)
            bitStream.Read(ucNewWay);

        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            if (ucNewWay == 1)
                pPed->SetCurrentRotationNew(rotation.data.fRotation);
            else
                pPed->SetCurrentRotation(rotation.data.fRotation);

            if (!IS_PLAYER(pPed))
                pPed->SetCameraRotation(rotation.data.fRotation);
            pPed->SetSyncTimeContext(ucTimeContext);
        }
    }
}

void CPedRPCs::GivePedJetPack(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
    if (pPed)
    {
        pPed->SetHasJetPack(true);
    }
}

void CPedRPCs::RemovePedJetPack(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
    if (pPed)
    {
        pPed->SetHasJetPack(false);
    }
}

void CPedRPCs::RemovePedClothes(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned char ucType;
    if (bitStream.Read(ucType))
    {
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            CClientPlayerClothes* pClothes = pPed->GetClothes();
            if (pClothes->RemoveClothes(ucType, false))
            {
                pPed->RebuildModel(true);
            }
        }
    }
}

void CPedRPCs::SetPedGravity(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    float fGravity;
    if (bitStream.Read(fGravity))
    {
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            if (pPed->IsLocalPlayer())
            {
                g_pCore->GetMultiplayer()->SetLocalPlayerGravity(fGravity);
            }
            else
            {
                CRemoteDataStorage* pData = pPed->GetRemoteData();
                if (pData)
                {
                    pData->SetGravity(fGravity);
                }
            }
        }
    }
}

void CPedRPCs::SetPedChoking(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bChoking;
    if (bitStream.ReadBit(bChoking))
    {
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetChoking(bChoking);
        }
    }
}

void CPedRPCs::SetPedFightingStyle(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned char ucStyle;
    if (bitStream.Read(ucStyle))
    {
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetFightingStyle((eFightingStyle)ucStyle);
        }
    }
}

void CPedRPCs::SetPedMoveAnim(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    unsigned int uiMoveAnim;
    if (bitStream.ReadCompressed(uiMoveAnim))
    {
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetMoveAnim((eMoveAnim)uiMoveAnim);
        }
    }
}

void CPedRPCs::WarpPedIntoVehicle(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the player and vehicle id
    ElementID     VehicleID;
    unsigned char ucSeat;
    unsigned char ucTimeContext;
    if (bitStream.Read(VehicleID) && bitStream.Read(ucSeat) && bitStream.Read(ucTimeContext))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetSyncTimeContext(ucTimeContext);

            // Grab the vehicle
            CClientVehicle* pVehicle = m_pVehicleManager->Get(VehicleID);
            if (pVehicle)
            {
                CStaticFunctionDefinitions::WarpPedIntoVehicle(pPed, pVehicle, ucSeat);
            }
        }
    }
}

void CPedRPCs::RemovePedFromVehicle(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the player and vehicle id
    unsigned char ucTimeContext;
    if (bitStream.Read(ucTimeContext))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            if (CStaticFunctionDefinitions::RemovePedFromVehicle(pPed))
                pPed->SetSyncTimeContext(ucTimeContext);
        }
    }
}

void CPedRPCs::SetPedDoingGangDriveby(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the player and vehicle id
    bool          bDriveby;
    unsigned char ucTimeContext;
    if (bitStream.ReadBit(bDriveby) && bitStream.Read(ucTimeContext))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetSyncTimeContext(ucTimeContext);

            pPed->SetDoingGangDriveby(bDriveby);
        }
    }
}

void CPedRPCs::SetPedAnimation(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::string blockName;

    if (bitStream.ReadString<unsigned char>(blockName))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            if (!blockName.empty())
            {
                std::string animName;
                int         iTime;
                int         iBlend = 250;
                bool        bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame, bTaskToBeRestoredOnAnimEnd;

                if (bitStream.ReadString<unsigned char>(animName) && bitStream.Read(iTime) && bitStream.ReadBit(bLoop) && bitStream.ReadBit(bUpdatePosition) &&
                    bitStream.ReadBit(bInterruptable) && bitStream.ReadBit(bFreezeLastFrame))
                {
                    if (bitStream.Can(eBitStreamVersion::SetPedAnimation_Blend))
                    {
                        bitStream.Read(iBlend);
                        bitStream.ReadBit(bTaskToBeRestoredOnAnimEnd);
                        if (!pPed->IsDucked())
                        {
                            bTaskToBeRestoredOnAnimEnd = false;
                        }
                    }

                    std::unique_ptr<CAnimBlock> pBlock = g_pGame->GetAnimManager()->GetAnimationBlock(blockName.c_str());
                    if (pBlock)
                    {
                        pPed->RunNamedAnimation(pBlock, animName.c_str(), iTime, iBlend, bLoop, bUpdatePosition, bInterruptable, bFreezeLastFrame);
                        pPed->SetTaskToBeRestoredOnAnimEnd(bTaskToBeRestoredOnAnimEnd);
                        pPed->SetTaskTypeToBeRestoredOnAnimEnd((eTaskType)TASK_SIMPLE_DUCK);
                    }
                }
            }
            else
            {
                pPed->KillAnimation();
            }
        }
    }
}

void CPedRPCs::SetPedAnimationProgress(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    // Read out the player and vehicle id
    std::string animName;
    float       fProgress;

    if (bitStream.ReadString<unsigned char>(animName))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            if (!animName.empty())
            {
                if (bitStream.Read(fProgress))
                {
                    auto pAnimAssociation = g_pGame->GetAnimManager()->RpAnimBlendClumpGetAssociation(pPed->GetClump(), animName.c_str());
                    if (pAnimAssociation)
                    {
                        pAnimAssociation->SetCurrentProgress(fProgress);
                    }
                }
            }
            else
            {
                pPed->KillAnimation();
            }
        }
    }
}

void CPedRPCs::SetPedAnimationSpeed(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    std::string animName;
    float       fSpeed;

    if (bitStream.ReadString<unsigned char>(animName))
    {
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed && !animName.empty())
        {
            if (bitStream.Read(fSpeed))
            {
                auto pAnimAssociation = g_pGame->GetAnimManager()->RpAnimBlendClumpGetAssociation(pPed->GetClump(), animName.c_str());
                if (pAnimAssociation)
                {
                    pAnimAssociation->SetCurrentSpeed(fSpeed);
                }
            }
        }
    }
}

void CPedRPCs::SetPedOnFire(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bIsOnFire;

    if (bitStream.ReadBit(bIsOnFire))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetOnFire(bIsOnFire);
        }
    }
}

void CPedRPCs::SetPedHeadless(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bIsHeadless;

    if (bitStream.ReadBit(bIsHeadless))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetHeadless(bIsHeadless);
        }
    }
}

void CPedRPCs::SetPedFrozen(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    bool bIsFrozen = 0;

    if (bitStream.ReadBit(bIsFrozen))
    {
        // Grab the ped
        CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
        if (pPed)
        {
            pPed->SetFrozen(bIsFrozen);
        }
    }
}

void CPedRPCs::ReloadPedWeapon(CClientEntity* pSource, NetBitStreamInterface& bitStream)
{
    CClientPed* pPed = m_pPedManager->Get(pSource->GetID(), true);
    if (pPed)
    {
        pPed->ReloadWeapon();
    }
}
