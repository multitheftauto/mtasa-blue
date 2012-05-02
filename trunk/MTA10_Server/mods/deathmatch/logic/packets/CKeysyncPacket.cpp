/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CKeysyncPacket.cpp
*  PURPOSE:     Key controls synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Chris McArthur <>
*               Jax <>
*               Cecill Etheredge <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

CKeysyncPacket::CKeysyncPacket ( CPlayer * pPlayer )
{
    m_pSourceElement = pPlayer;
}


bool CKeysyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Got a player to write?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Read out the controller states
        CControllerState ControllerState;
        if ( !ReadSmallKeysync ( ControllerState, BitStream ) )
            return false;

        // Read the rotations
        float fPlayerCurrentRotation;
        float fCameraRotation;
        if ( BitStream.Version () >= 0x2C )
        {
            SKeysyncRotation rotation;
            BitStream.Read ( &rotation );
            fPlayerCurrentRotation = rotation.data.fPlayerRotation;
            fCameraRotation = rotation.data.fCameraRotation;
        }
        else
        {
            fPlayerCurrentRotation = pSourcePlayer->GetRotation ();
            fCameraRotation = pSourcePlayer->GetCameraRotation ();
        }

        // Flags
        SKeysyncFlags flags;
        if ( !BitStream.Read ( &flags ) )
            return false;

        // Set the ducked and choking state
        pSourcePlayer->SetDucked ( flags.data.bIsDucked );
        pSourcePlayer->SetChoking ( flags.data.bIsChoking );

        // If he's shooting or aiming
        if ( ControllerState.ButtonCircle || ControllerState.RightShoulder1 )
        {
            bool bHasWeapon = BitStream.ReadBit ();

            if ( bHasWeapon )
            {
                // Read client weapon data, but only apply it if the weapon matches with the server
                uchar ucUseWeaponType = pSourcePlayer->GetWeaponType ();
                bool bWeaponCorrect = true;
 
               // Check client has the weapon we think he has
                unsigned char ucClientWeaponType;
                if ( !BitStream.Read ( ucClientWeaponType ) )
                    return false;

                if ( pSourcePlayer->GetWeaponType () != ucClientWeaponType )
                {
                    bWeaponCorrect = false;                 // Possibly old weapon data.
                    ucUseWeaponType = ucClientWeaponType;   // Use the packet supplied weapon type to skip over the correct amount of data
                }

                // Read out the current weapon slot and set it
                SWeaponSlotSync slot;
                if ( !BitStream.Read ( &slot ) )
                    return false;
                unsigned int uiSlot = slot.data.uiSlot;

                if ( bWeaponCorrect )
                    pSourcePlayer->SetWeaponSlot ( uiSlot );

                // Did he have a weapon?
                if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
                {
                    // And ammo in clip
                    SWeaponAmmoSync ammo ( ucUseWeaponType, false, true );
                    if ( !BitStream.Read ( &ammo ) )
                        return false;

                    eWeaponType eWeapon = static_cast < eWeaponType > ( pSourcePlayer->GetWeaponType ( uiSlot ) );
                    float fSkill = pSourcePlayer->GetPlayerStat ( CWeaponStatManager::GetSkillStatIndex ( eWeapon ) );
                    float fWeaponRange = g_pGame->GetWeaponStatManager ( )->GetWeaponRangeFromSkillLevel ( eWeapon, fSkill );

                    // Read the aim data
                    SWeaponAimSync aim ( fWeaponRange );
                    if ( !BitStream.Read ( &aim ) )
                        return false;
                    pSourcePlayer->SetSniperSourceVector ( aim.data.vecOrigin );
                    pSourcePlayer->SetTargettingVector ( aim.data.vecTarget );

                    if ( bWeaponCorrect )
                    {
                        pSourcePlayer->SetWeaponAmmoInClip ( ammo.data.usAmmoInClip );

                        pSourcePlayer->SetSniperSourceVector ( aim.data.vecOrigin );
                        pSourcePlayer->SetTargettingVector ( aim.data.vecTarget );

                        // Set the arm directions and whether or not arms are up
                        pSourcePlayer->SetAimDirection ( aim.data.fArm );
                        pSourcePlayer->SetAkimboArmUp ( flags.data.bAkimboTargetUp );
                    }

                    // Read out the driveby direction
                    unsigned char ucDriveByDirection;
                    if ( !BitStream.Read ( ucDriveByDirection ) )
                        return false;
                    pSourcePlayer->SetDriveByDirection ( ucDriveByDirection );
                }
            }
            else
            {
                pSourcePlayer->SetWeaponSlot ( 0 );
            }
        }

        // If he's in a vehicle, read out the small vehicle specific data
        CVehicle* pVehicle = pSourcePlayer->GetOccupiedVehicle ();
        if ( pVehicle && flags.data.bSyncingVehicle )
        {
            ReadVehicleSpecific ( pVehicle, BitStream );

            if ( pVehicle->GetUpgrades ()->HasUpgrade ( 1087 ) ) // Hydraulics?
            {
                short sRightStickX, sRightStickY;
                if ( !BitStream.Read ( sRightStickX ) ||
                     !BitStream.Read ( sRightStickY ) )
                     return false;

                ControllerState.RightStickX = sRightStickX;
                ControllerState.RightStickY = sRightStickY;
            }

            if ( pVehicle->GetVehicleType () == VEHICLE_PLANE ||
                 pVehicle->GetVehicleType () == VEHICLE_HELI )
            {
                bool bState1, bState2;
                if ( ! BitStream.ReadBit ( bState1 ) ||
                     ! BitStream.ReadBit ( bState2 ) )
                     return false;
                ControllerState.LeftShoulder2 = bState1 * 255;
                ControllerState.RightShoulder2 = bState2 * 255;
            }
        }

        // Set the controller states
        pSourcePlayer->GetPad ()->NewControllerState ( ControllerState );
        if ( BitStream.Version () >= 0x2C )
        {
            pSourcePlayer->SetRotation ( fPlayerCurrentRotation );
            pSourcePlayer->SetCameraRotation ( fCameraRotation );
        }
        return true;
    }
    
    return false;
}


bool CKeysyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Got a player to write?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );
        CVehicle* pVehicle = pSourcePlayer->GetOccupiedVehicle ();

        // Write the source player id
        ElementID PlayerID = pSourcePlayer->GetID ();
        BitStream.Write ( PlayerID );

        // Write the keysync data
        const CControllerState& ControllerState = pSourcePlayer->GetPad ()->GetCurrentControllerState ();
        WriteSmallKeysync ( ControllerState, BitStream );

        // Write the rotations
        if ( BitStream.Version () >= 0x2C )
        {
            SKeysyncRotation rotation;
            rotation.data.fPlayerRotation = pSourcePlayer->GetRotation ();
            rotation.data.fCameraRotation = pSourcePlayer->GetCameraRotation ();
            BitStream.Write ( &rotation );
        }

        // Flags
        SKeysyncFlags flags;
        flags.data.bIsDucked = ( pSourcePlayer->IsDucked () == true );
        flags.data.bIsChoking = ( pSourcePlayer->IsChoking () == true );
        flags.data.bAkimboTargetUp = ( pSourcePlayer->IsAkimboArmUp () == true );
        flags.data.bSyncingVehicle = ( pVehicle != NULL && pSourcePlayer->GetOccupiedVehicleSeat () == 0 );

        // Write the flags
        BitStream.Write ( &flags );

        // If he's shooting or aiming
        if ( ControllerState.ButtonCircle || ControllerState.RightShoulder1 )
        {
            // Write his current weapon slot
            unsigned int uiSlot = pSourcePlayer->GetWeaponSlot ();
            SWeaponSlotSync slot;
            slot.data.uiSlot = uiSlot;
            BitStream.Write ( &slot );

            if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
            {
                // Write his ammo in clip
                SWeaponAmmoSync ammo ( pSourcePlayer->GetWeaponType (), false, true );
                ammo.data.usAmmoInClip = pSourcePlayer->GetWeaponAmmoInClip ();
                BitStream.Write ( &ammo );

                // Write the weapon aim data
                SWeaponAimSync aim ( 0.0f );
                aim.data.vecOrigin = pSourcePlayer->GetSniperSourceVector ();
                pSourcePlayer->GetTargettingVector ( aim.data.vecTarget );
                aim.data.fArm = pSourcePlayer->GetAimDirection ();
                BitStream.Write ( &aim );

                // Write the driveby aim directoin
                BitStream.Write ( pSourcePlayer->GetDriveByDirection () );
            }
            else
            {
                pSourcePlayer->SetWeaponAmmoInClip ( 1 );
                pSourcePlayer->SetWeaponTotalAmmo ( 1 );
            }
        }

        // If he's in a vehicle, read out the small vehicle specific data
        if ( flags.data.bSyncingVehicle )
        {
            WriteVehicleSpecific ( pVehicle, BitStream );

            if ( pVehicle->GetUpgrades ()->HasUpgrade ( 1087 ) ) // Hydraulics?
            {
                BitStream.Write ( ControllerState.RightStickX );
                BitStream.Write ( ControllerState.RightStickY );
            }

            if ( pVehicle->GetVehicleType () == VEHICLE_PLANE ||
                 pVehicle->GetVehicleType () == VEHICLE_HELI )
            {
                BitStream.WriteBit ( ControllerState.LeftShoulder2 != 0);
                BitStream.WriteBit ( ControllerState.RightShoulder2 != 0);
            }
        }

        return true;
    }

    return false;
}


void CKeysyncPacket::ReadVehicleSpecific ( CVehicle* pVehicle, NetBitStreamInterface& BitStream )
{
    // Turret states
    unsigned short usModel = pVehicle->GetModel ();
    if ( CVehicleManager::HasTurret ( usModel ) )
    {
        // Read out the turret position
        SVehicleTurretSync vehicle;
        BitStream.Read ( &vehicle );

        // Set the data
        pVehicle->SetTurretPosition ( vehicle.data.fTurretX, vehicle.data.fTurretY );
    }
}


void CKeysyncPacket::WriteVehicleSpecific ( CVehicle* pVehicle, NetBitStreamInterface& BitStream ) const
{
    // Turret states
    unsigned short usModel = pVehicle->GetModel ();
    if ( CVehicleManager::HasTurret ( usModel ) )
    {
        // Grab the turret position
        SVehicleTurretSync vehicle;
        pVehicle->GetTurretPosition ( vehicle.data.fTurretX, vehicle.data.fTurretY );

        BitStream.Write ( &vehicle );
    }
}
