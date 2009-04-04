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

CKeysyncPacket::CKeysyncPacket ( CPlayer * pPlayer )
{
    m_pSourceElement = pPlayer;
}

bool CKeysyncPacket::Read ( NetServerBitStreamInterface& BitStream )
{
    // Got a player to write?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Read out the controller states
        CControllerState ControllerState;
        CControllerState LastControllerState = pSourcePlayer->GetPad ()->GetLastControllerState ();
        ReadSmallKeysync ( ControllerState, LastControllerState, BitStream );        

        // Flags
        unsigned char ucFlags;
        BitStream.Read ( ucFlags );

        // Decode the flags
        bool bDucked = ( ucFlags & 0x01 ) ? true:false;
        bool bChoking = ( ucFlags & 0x02 ) ? true:false;
        bool bArmUp = ( ucFlags & 0x04 ) ? true:false;

        // Set the ducked and choking state
        pSourcePlayer->SetDucked ( bDucked );
        pSourcePlayer->SetChoking ( bChoking );

        // If he's shooting
        if ( ControllerState.ButtonCircle )
        {
            // Read out the current weapon slot and set it
            unsigned char ucCurrentWeaponSlot;
            unsigned char ucCurrentWeaponType;

            BitStream.Read ( ucCurrentWeaponSlot );
            BitStream.Read ( ucCurrentWeaponType );

            pSourcePlayer->SetWeaponSlot ( ucCurrentWeaponSlot );
            pSourcePlayer->SetWeaponType ( ucCurrentWeaponType );

            // Did he have a weapon?
            if ( ucCurrentWeaponType != 0 )
            {
                unsigned char ucTemp;

                // And ammo in clip
                unsigned short usAmmoInClip;
                BitStream.Read ( usAmmoInClip );
                pSourcePlayer->SetWeaponAmmoInClip ( usAmmoInClip );

				// Read out the aim directions
				float fArmX, fArmY;
				BitStream.Read ( fArmX );
				BitStream.Read ( fArmY );

				// Set the arm directions and whether or not arms are up
				pSourcePlayer->SetAimDirections ( fArmX, fArmY );
				pSourcePlayer->SetAkimboArmUp ( bArmUp );

                // Source vector
                CVector vecTemp;
                BitStream.Read ( vecTemp.fX );
                BitStream.Read ( vecTemp.fY );
                BitStream.Read ( vecTemp.fZ );
                pSourcePlayer->SetSniperSourceVector ( vecTemp );

                // Read out the weapon target vector
                CVector vecTarget;
                BitStream.Read ( vecTarget.fX );
                BitStream.Read ( vecTarget.fY );
                BitStream.Read ( vecTarget.fZ );
                pSourcePlayer->SetTargettingVector ( vecTarget );

                // Read out the driveby direction
                unsigned char ucDriveByDirection;
                BitStream.Read ( ucDriveByDirection );
                pSourcePlayer->SetDriveByDirection ( ucDriveByDirection );
            }
        }

        // If he's in a vehicle, read out the small vehicle specific data
        CVehicle* pVehicle = pSourcePlayer->GetOccupiedVehicle ();
        if ( pVehicle && pSourcePlayer->GetOccupiedVehicleSeat () == 0 )
        {
            ReadVehicleSpecific ( pVehicle, BitStream );

            if ( pVehicle->GetUpgrades ()->HasUpgrade ( 1087 ) ) // Hydraulics?
            {
                short sRightStickX, sRightStickY;
                BitStream.Read ( sRightStickX );
                BitStream.Read ( sRightStickY );

                ControllerState.RightStickX = sRightStickX;
                ControllerState.RightStickY = sRightStickY;
            }
        }

        // Set the controller states
        pSourcePlayer->GetPad ()->NewControllerState ( ControllerState );

        return true;
    }
    
    return false;
}


bool CKeysyncPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    // Got a player to write?
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Write the source player id
        ElementID PlayerID = pSourcePlayer->GetID ();
        BitStream.Write ( PlayerID );

        // Write his ping divided with 2 plus a small number so the client can find out when this packet was sent
        unsigned short usLatency = pSourcePlayer->GetPing ();
        BitStream.Write ( usLatency );

        // Write the keysync data
        const CControllerState& ControllerState = pSourcePlayer->GetPad ()->GetCurrentControllerState ();
        const CControllerState& LastControllerState = pSourcePlayer->GetPad ()->GetLastControllerState ();
        WriteSmallKeysync ( ControllerState, LastControllerState, BitStream );

        // Flags
        unsigned char ucFlags = 0;
        ucFlags |= pSourcePlayer->IsDucked () ? 1:0;
        ucFlags |= pSourcePlayer->IsChoking () << 1;
        ucFlags |= pSourcePlayer->IsAkimboArmUp () << 2;

        // Write the flags
        BitStream.Write ( ucFlags );

        // If he's shooting
        if ( ControllerState.ButtonCircle )
        {
            // Write his current weapon
            unsigned char ucCurrentWeaponSlot = pSourcePlayer->GetWeaponSlot ();
            unsigned char ucCurrentWeaponType = pSourcePlayer->GetWeaponType ();

            BitStream.Write ( ucCurrentWeaponSlot );
            BitStream.Write ( ucCurrentWeaponType );

            if ( ucCurrentWeaponType != 0 )
            {
                // Write his ammo in clip and aim directions
                BitStream.Write ( pSourcePlayer->GetWeaponAmmoInClip () );
				BitStream.Write ( pSourcePlayer->GetAimDirectionX () );
				BitStream.Write ( pSourcePlayer->GetAimDirectionY () );

                // Source vector if sniper
                CVector vecTemp = pSourcePlayer->GetSniperSourceVector ();
                BitStream.Write ( vecTemp.fX );
                BitStream.Write ( vecTemp.fY );
                BitStream.Write ( vecTemp.fZ );

                // Write his current target vector
                pSourcePlayer->GetTargettingVector ( vecTemp );
                BitStream.Write ( vecTemp.fX );
                BitStream.Write ( vecTemp.fY );
                BitStream.Write ( vecTemp.fZ );

                // Write the driveby aim directoin
                BitStream.Write ( pSourcePlayer->GetDriveByDirection () );
            }
        }

        // If he's in a vehicle, read out the small vehicle specific data
        CVehicle* pVehicle = pSourcePlayer->GetOccupiedVehicle ();
        if ( pVehicle && pSourcePlayer->GetOccupiedVehicleSeat () == 0 )
        {
            WriteVehicleSpecific ( pVehicle, BitStream );

            if ( pVehicle->GetUpgrades ()->HasUpgrade ( 1087 ) ) // Hydraulics?
            {
                BitStream.Write ( ControllerState.RightStickX );
                BitStream.Write ( ControllerState.RightStickY );
            }
        }

        return true;
    }

    return false;
}


void CKeysyncPacket::ReadVehicleSpecific ( CVehicle* pVehicle, NetServerBitStreamInterface& BitStream )
{
    // Turret states
    unsigned short usModel = pVehicle->GetModel ();
    if ( CVehicleManager::HasTurret ( usModel ) )
    {
        // Read out the turret position
        float fHorizontal;
        float fVertical;
        BitStream.Read ( fHorizontal );
        BitStream.Read ( fVertical );
        
        // Set it
        pVehicle->SetTurretPosition ( fHorizontal, fVertical );
    }
}


void CKeysyncPacket::WriteVehicleSpecific ( CVehicle* pVehicle, NetServerBitStreamInterface& BitStream ) const
{
    // Turret states
    unsigned short usModel = pVehicle->GetModel ();
    if ( CVehicleManager::HasTurret ( usModel ) )
    {
        // Grab the turret position
        float fHorizontal;
        float fVertical;
        pVehicle->GetTurretPosition ( fHorizontal, fVertical );

        // Write it
        BitStream.Write ( fHorizontal );
        BitStream.Write ( fVertical );
    }
}
