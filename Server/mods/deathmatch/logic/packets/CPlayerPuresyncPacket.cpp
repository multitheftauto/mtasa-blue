/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/packets/CPlayerPuresyncPacket.cpp
 *  PURPOSE:     Player pure synchronization packet class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPlayerPuresyncPacket.h"
#include "CElementIDs.h"
#include "CWeaponNames.h"
#include "Utils.h"
#include "CTickRateSettings.h"
#include <net/SyncStructures.h>

extern CGame* g_pGame;

CPlayerPuresyncPacket::CPlayerPuresyncPacket(CPlayer* pPlayer)
{
    m_pSourceElement = pPlayer;
}

bool CPlayerPuresyncPacket::Read(NetBitStreamInterface& BitStream)
{
    if (m_pSourceElement)
    {
        CPlayer* pSourcePlayer = static_cast<CPlayer*>(m_pSourceElement);

        // Read out the time context
        unsigned char ucTimeContext = 0;
        if (!BitStream.Read(ucTimeContext))
            return false;

        // Only read this packet if it matches the current time context that
        // player is in.
        if (!pSourcePlayer->CanUpdateSync(ucTimeContext))
        {
            return false;
        }

        // Read out keys
        CControllerState ControllerState;
        ReadFullKeysync(ControllerState, BitStream);
        pSourcePlayer->GetPad()->NewControllerState(ControllerState);

        // Read the flags
        SPlayerPuresyncFlags flags;
        if (!BitStream.Read(&flags))
            return false;

        pSourcePlayer->SetInWater(flags.data.bIsInWater);
        pSourcePlayer->SetOnGround(flags.data.bIsOnGround);
        pSourcePlayer->SetHasJetPack(flags.data.bHasJetPack);
        pSourcePlayer->SetDucked(flags.data.bIsDucked);
        pSourcePlayer->SetWearingGoggles(flags.data.bWearsGoogles);
        pSourcePlayer->SetChoking(flags.data.bIsChoking);
        pSourcePlayer->SetAkimboArmUp(flags.data.bAkimboTargetUp);
        pSourcePlayer->SetOnFire(flags.data.bIsOnFire);
        pSourcePlayer->SetStealthAiming(flags.data.bStealthAiming);

        if (BitStream.Can(eBitStreamVersion::IsPedReloadingWeapon))
            pSourcePlayer->SetReloadingWeapon(flags.data2.isReloadingWeapon);

        // Contact element
        CElement* pContactElement = NULL;
        if (flags.data.bHasContact)
        {
            ElementID Temp;
            if (!BitStream.Read(Temp))
                return false;
            pContactElement = CElementIDs::GetElement(Temp);
        }

        // Player position
        SPositionSync position(false);
        bool          positionRead = BitStream.Read(&position);

        if (positionRead && pContactElement != nullptr)
        {
            int32_t radius = -1;

            switch (pContactElement->GetType())
            {
                case CElement::VEHICLE:
                    if (((CVehicle*)pContactElement)->GetSyncer() != pSourcePlayer)
                        radius = g_TickRateSettings.iVehicleContactSyncRadius;
                    break;
            }

            if (radius > -1 && 
                (!IsPointNearPoint3D(pSourcePlayer->GetPosition(), pContactElement->GetPosition(), radius) ||
                    pSourcePlayer->GetDimension() != pContactElement->GetDimension()))
            {
                pContactElement = nullptr;
                // Use current player position. They are not reporting their absolute position so we have to disregard it.
                position.data.vecPosition = pSourcePlayer->GetPosition();
            }
        }

        // If the client reported contact but the element doesn't exist anymore,
        // the coordinates become invalid as they are relative to that element.
        if (positionRead && pContactElement == nullptr && flags.data.bHasContact)
        {
            position.data.vecPosition = pSourcePlayer->GetPosition();
        }
        
        CElement* pPreviousContactElement = pSourcePlayer->GetContactElement();
        pSourcePlayer->SetContactElement(pContactElement);

        if (pPreviousContactElement != pContactElement)
        {
            // Call our onPlayerContact event
            CLuaArguments Arguments;
            if (pPreviousContactElement)
                Arguments.PushElement(pPreviousContactElement);
            else
                Arguments.PushNil();
            if (pContactElement)
                Arguments.PushElement(pContactElement);
            else
                Arguments.PushNil();

            pSourcePlayer->CallEvent("onPlayerContact", Arguments);
        }

        if (!positionRead)
            return false;

        if (pContactElement)
        {
            pSourcePlayer->SetContactPosition(position.data.vecPosition);

            // Get the true position
            CVector vecTempPos = pContactElement->GetPosition();
            position.data.vecPosition += vecTempPos;
        }

        // if (position.data.vecPosition.fX != 0.0f || position.data.vecPosition.fY != 0.0f || position.data.vecPosition.fZ != 0.0f)
        {
            CVector playerPosition = pSourcePlayer->GetPosition();
            float playerDistancePosition = DistanceBetweenPoints3D(playerPosition, position.data.vecPosition);
            if (playerDistancePosition >= g_TickRateSettings.playerTeleportAlert)
            {
                if (!pSourcePlayer->GetTeleported())
                {
                    CLuaArguments arguments;
                    arguments.PushNumber(playerPosition.fX);
                    arguments.PushNumber(playerPosition.fY);
                    arguments.PushNumber(playerPosition.fZ);
                    arguments.PushNumber(position.data.vecPosition.fX);
                    arguments.PushNumber(position.data.vecPosition.fY);
                    arguments.PushNumber(position.data.vecPosition.fZ);
                    pSourcePlayer->CallEvent("onPlayerTeleport", arguments, nullptr);
                }

                pSourcePlayer->SetTeleported(false);
            }
        }

        pSourcePlayer->SetPosition(position.data.vecPosition);

        // Player rotation
        SPedRotationSync rotation;
        if (!BitStream.Read(&rotation))
            return false;
        pSourcePlayer->SetRotation(rotation.data.fRotation);

        // Move speed vector
        if (flags.data.bSyncingVelocity)
        {
            SVelocitySync velocity;
            if (!BitStream.Read(&velocity))
                return false;
            pSourcePlayer->SetVelocity(velocity.data.vecVelocity);
        }

        // Health ( stored with damage )
        SPlayerHealthSync health;
        if (!BitStream.Read(&health))
            return false;
        float fHealth = health.data.fValue;

        // Armor
        SPlayerArmorSync armor;
        if (!BitStream.Read(&armor))
            return false;

        float fArmor = armor.data.fValue;
        float fOldArmor = pSourcePlayer->GetArmor();
        float fArmorLoss = fOldArmor - fArmor;

        pSourcePlayer->SetArmor(fArmor);

        // Read out and set the camera rotation
        SCameraRotationSync camRotation;
        BitStream.Read(&camRotation);
        pSourcePlayer->SetCameraRotation(camRotation.data.fRotation);

        // Read the camera orientation
        CVector vecCamPosition, vecCamFwd;
        ReadCameraOrientation(position.data.vecPosition, BitStream, vecCamPosition, vecCamFwd);
        pSourcePlayer->SetCameraOrientation(vecCamPosition, vecCamFwd);

        if (flags.data.bHasAWeapon)
        {
            // Read client weapon data, but only apply it if the weapon matches with the server
            uchar ucUseWeaponType = pSourcePlayer->GetWeaponType();
            bool  bWeaponCorrect = true;

            // Check client has the weapon we think he has
            unsigned char ucClientWeaponType;
            if (!BitStream.Read(ucClientWeaponType))
                return false;

            if (pSourcePlayer->GetWeaponType() != ucClientWeaponType)
            {
                bWeaponCorrect = false;                          // Possibly old weapon data.
                ucUseWeaponType = ucClientWeaponType;            // Use the packet supplied weapon type to skip over the correct amount of data
            }

            // Update check counts
            pSourcePlayer->SetWeaponCorrect(bWeaponCorrect);

            // Current weapon slot
            SWeaponSlotSync slot;
            if (!BitStream.Read(&slot))
                return false;
            unsigned int uiSlot = slot.data.uiSlot;

            // Set weapon slot
            if (bWeaponCorrect)
                pSourcePlayer->SetWeaponSlot(uiSlot);
            else
            {
                // remove invalid weapon data to prevent this from being relayed to other players
                ucClientWeaponType = 0;
                slot.data.uiSlot = 0;
            }

            if (CWeaponNames::DoesSlotHaveAmmo(uiSlot))
            {
                // Read out the ammo states
                SWeaponAmmoSync ammo(ucUseWeaponType, true, true);
                if (!BitStream.Read(&ammo))
                    return false;

                float fWeaponRange = pSourcePlayer->GetWeaponRangeFromSlot(uiSlot);

                // Read out the aim data
                SWeaponAimSync sync(fWeaponRange, (ControllerState.RightShoulder1 || ControllerState.ButtonCircle));
                if (!BitStream.Read(&sync))
                    return false;

                if (bWeaponCorrect)
                {
                    // Set the ammo states
                    pSourcePlayer->SetWeaponAmmoInClip(ammo.data.usAmmoInClip);
                    pSourcePlayer->SetWeaponTotalAmmo(ammo.data.usTotalAmmo);

                    // Set the arm directions and whether or not arms are up
                    pSourcePlayer->SetAimDirection(sync.data.fArm);

                    // Read the aim data only if he's shooting or aiming
                    if (sync.isFull())
                    {
                        pSourcePlayer->SetSniperSourceVector(sync.data.vecOrigin);
                        pSourcePlayer->SetTargettingVector(sync.data.vecTarget);
                    }
                }
            }
            else
            {
                if (bWeaponCorrect)
                {
                    pSourcePlayer->SetWeaponAmmoInClip(1);
                    pSourcePlayer->SetWeaponTotalAmmo(1);
                }
            }
        }
        else
        {
            pSourcePlayer->SetWeaponSlot(0);
            pSourcePlayer->SetWeaponAmmoInClip(1);
            pSourcePlayer->SetWeaponTotalAmmo(1);
        }

        // Read out damage info if changed
        if (BitStream.ReadBit() == true)
        {
            ElementID DamagerID;
            if (!BitStream.Read(DamagerID))
                return false;

            SWeaponTypeSync weaponType;
            if (!BitStream.Read(&weaponType))
                return false;

            SBodypartSync bodyPart;
            if (!BitStream.Read(&bodyPart))
                return false;

            pSourcePlayer->SetDamageInfo(DamagerID, weaponType.data.ucWeaponType, bodyPart.data.uiBodypart);
        }

        // If we know the player's dead, make sure the health we send on is 0
        if (pSourcePlayer->IsDead())
            fHealth = 0.0f;

        float fOldHealth = pSourcePlayer->GetHealth();
        float fHealthLoss = fOldHealth - fHealth;
        pSourcePlayer->SetHealth(fHealth);

        // Less than last packet's frame?
        if (fHealthLoss > 0 || fArmorLoss > 0)
        {
            float fDamage = 0.0f;
            if (fHealthLoss > 0)
                fDamage += fHealthLoss;
            if (fArmorLoss > 0)
                fDamage += fArmorLoss;

            // Call the onPlayerDamage event
            CLuaArguments Arguments;
            CElement*     pKillerElement = CElementIDs::GetElement(pSourcePlayer->GetPlayerAttacker());
            if (pKillerElement)
                Arguments.PushElement(pKillerElement);
            else
                Arguments.PushNil();
            Arguments.PushNumber(pSourcePlayer->GetAttackWeapon());
            Arguments.PushNumber(pSourcePlayer->GetAttackBodyPart());
            Arguments.PushNumber(fDamage);

            pSourcePlayer->CallEvent("onPlayerDamage", Arguments);
        }

        // Success
        return true;
    }

    return false;
}

bool CPlayerPuresyncPacket::Write(NetBitStreamInterface& BitStream) const
{
    if (m_pSourceElement)
    {
        CPlayer* pSourcePlayer = static_cast<CPlayer*>(m_pSourceElement);

        ElementID               PlayerID = pSourcePlayer->GetID();
        unsigned short          usLatency = pSourcePlayer->GetPing();
        const CControllerState& ControllerState = pSourcePlayer->GetPad()->GetCurrentControllerState();
        CElement*               pContactElement = pSourcePlayer->GetContactElement();

        // Get current weapon slot
        unsigned char ucWeaponSlot = pSourcePlayer->GetWeaponSlot();

        // Flags
        SPlayerPuresyncFlags flags;
        flags.data.bIsInWater = (pSourcePlayer->IsInWater() == true);
        flags.data.bIsOnGround = (pSourcePlayer->IsOnGround() == true);
        flags.data.bHasJetPack = (pSourcePlayer->HasJetPack() == true);
        flags.data.bIsDucked = (pSourcePlayer->IsDucked() == true);
        flags.data.bWearsGoogles = (pSourcePlayer->IsWearingGoggles() == true);
        flags.data.bHasContact = (pContactElement != NULL);
        flags.data.bIsChoking = (pSourcePlayer->IsChoking() == true);
        flags.data.bAkimboTargetUp = (pSourcePlayer->IsAkimboArmUp() == true);
        flags.data.bIsOnFire = (pSourcePlayer->IsOnFire() == true);
        flags.data.bHasAWeapon = (ucWeaponSlot != 0);
        flags.data.bSyncingVelocity = (!flags.data.bIsOnGround || pSourcePlayer->IsSyncingVelocity());
        flags.data.bStealthAiming = (pSourcePlayer->IsStealthAiming() == true);

        if (pSourcePlayer->CanBitStream(eBitStreamVersion::IsPedReloadingWeapon))
            flags.data2.isReloadingWeapon = pSourcePlayer->IsReloadingWeapon();

        CVector vecPosition = pSourcePlayer->GetPosition();
        if (pContactElement)
            pSourcePlayer->GetContactPosition(vecPosition);

        BitStream.Write(PlayerID);

        // Write the time context
        BitStream.Write(pSourcePlayer->GetSyncTimeContext());

        BitStream.WriteCompressed(usLatency);
        WriteFullKeysync(ControllerState, BitStream);

        BitStream.Write(&flags);

        if (pContactElement)
            BitStream.Write(pContactElement->GetID());

        SPositionSync position(false);
        position.data.vecPosition = vecPosition;
        BitStream.Write(&position);

        SPedRotationSync rotation;
        rotation.data.fRotation = pSourcePlayer->GetRotation();
        BitStream.Write(&rotation);

        if (flags.data.bSyncingVelocity)
        {
            SVelocitySync velocity;
            pSourcePlayer->GetVelocity(velocity.data.vecVelocity);
            BitStream.Write(&velocity);
        }

        // Player health and armor
        SPlayerHealthSync health;
        health.data.fValue = pSourcePlayer->GetHealth();
        BitStream.Write(&health);

        SPlayerArmorSync armor;
        armor.data.fValue = pSourcePlayer->GetArmor();
        BitStream.Write(&armor);

        SCameraRotationSync camRotation;
        camRotation.data.fRotation = pSourcePlayer->GetCameraRotation();
        BitStream.Write(&camRotation);

        if (flags.data.bHasAWeapon)
        {
            unsigned int    uiSlot = ucWeaponSlot;
            SWeaponSlotSync slot;
            slot.data.uiSlot = uiSlot;
            BitStream.Write(&slot);

            if (CWeaponNames::DoesSlotHaveAmmo(uiSlot))
            {
                unsigned short usWeaponAmmoInClip = pSourcePlayer->GetWeaponAmmoInClip();

                SWeaponAmmoSync ammo(pSourcePlayer->GetWeaponType(), false, true);
                ammo.data.usAmmoInClip = usWeaponAmmoInClip;
                BitStream.Write(&ammo);

                SWeaponAimSync aim(0.0f, (ControllerState.RightShoulder1 || ControllerState.ButtonCircle));
                aim.data.fArm = pSourcePlayer->GetAimDirection();

                // Write the aim data only if he's aiming or shooting
                if (aim.isFull())
                {
                    aim.data.vecOrigin = pSourcePlayer->GetSniperSourceVector();
                    pSourcePlayer->GetTargettingVector(aim.data.vecTarget);
                }
                BitStream.Write(&aim);
            }
        }

        // Success
        return true;
    }

    return false;
}
