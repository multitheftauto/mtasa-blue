/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "SimHeaders.h"


CSimKeysyncPacket::CSimKeysyncPacket (    ElementID PlayerID,
                                          bool bPlayerHasOccupiedVehicle,
                                          ushort usVehicleGotModel,
                                          uchar ucPlayerGotWeaponType,
                                          float fPlayerGotWeaponRange,
                                          bool bVehicleHasHydraulics,
                                          bool bVehicleIsPlaneOrHeli,
                                          CControllerState& sharedControllerState )
    : m_PlayerID ( PlayerID )
    , m_bPlayerHasOccupiedVehicle ( bPlayerHasOccupiedVehicle )
    , m_usVehicleGotModel ( usVehicleGotModel )
    , m_ucPlayerGotWeaponType ( ucPlayerGotWeaponType )
    , m_fPlayerGotWeaponRange ( fPlayerGotWeaponRange )
    , m_bVehicleHasHydraulics ( bVehicleHasHydraulics )
    , m_bVehicleIsPlaneOrHeli ( bVehicleIsPlaneOrHeli )
    , m_sharedControllerState ( sharedControllerState )
{
}


//
// Should do the same this as what CKeysyncPacket::Read() does
//
bool CSimKeysyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Read out the controller states
    if ( !ReadSmallKeysync ( m_sharedControllerState, CControllerState (), BitStream ) )
        return false;

    // Flags
    if ( !BitStream.Read ( &m_Cache.flags ) )
        return false;

    // If he's shooting or aiming
    if ( m_sharedControllerState.ButtonCircle || m_sharedControllerState.RightShoulder1 )
    {
        bool bHasWeapon = BitStream.ReadBit ();

        if ( bHasWeapon )
        {
            // Read client weapon data, but only apply it if the weapon matches with the server
            uchar ucUseWeaponType = m_ucPlayerGotWeaponType;
            bool bWeaponCorrect = true;

           // Check client has the weapon we think he has
            unsigned char ucClientWeaponType;
            if ( !BitStream.Read ( ucClientWeaponType ) )
                return false;

            if ( m_ucPlayerGotWeaponType != ucClientWeaponType )
            {
                bWeaponCorrect = false;                 // Possibly old weapon data.
                ucUseWeaponType = ucClientWeaponType;   // Use the packet supplied weapon type to skip over the correct amount of data
            }

            m_Cache.bWeaponCorrect = bWeaponCorrect;    // Copied from PlayerPuresyncPacket

            // Read out the current weapon slot and set it
            SWeaponSlotSync slot;
            if ( !BitStream.Read ( &slot ) )
                return false;
            unsigned int uiSlot = slot.data.uiSlot;

            if ( bWeaponCorrect )
                m_Cache.ucWeaponSlot = uiSlot;

            // Did he have a weapon?
            if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
            {
                // And ammo in clip
                SWeaponAmmoSync ammo ( ucUseWeaponType, false, true );
                if ( !BitStream.Read ( &ammo ) )
                    return false;

                // Read the aim data
                SWeaponAimSync aim ( m_fPlayerGotWeaponRange );
                if ( !BitStream.Read ( &aim ) )
                    return false;

                // Set the arm directions and whether or not arms are up
                // Save this here incase weapon is not correct
                m_Cache.fAimDirection = aim.data.fArm;

                if ( bWeaponCorrect )
                {
                    m_Cache.usAmmoInClip = ammo.data.usAmmoInClip;

                    m_Cache.vecSniperSource = aim.data.vecOrigin;
                    m_Cache.vecTargetting = aim.data.vecTarget;
                }

                // Read out the driveby direction
                if ( !BitStream.Read ( m_Cache.ucDriveByDirection ) )
                    return false;
            }
        }
        else
        {
            m_Cache.ucWeaponSlot = 0;
        }
    }

    // If he's in a vehicle, read out the small vehicle specific data
    if ( m_bPlayerHasOccupiedVehicle && m_Cache.flags.data.bSyncingVehicle )
    {
        if ( CVehicleManager::HasTurret ( m_usVehicleGotModel ) )
            BitStream.Read ( &m_Cache.turretSync );

        if ( m_bVehicleHasHydraulics )
        {
            short sRightStickX, sRightStickY;
            if ( !BitStream.Read ( sRightStickX ) ||
                 !BitStream.Read ( sRightStickY ) )
                 return false;

            m_sharedControllerState.RightStickX = sRightStickX;
            m_sharedControllerState.RightStickY = sRightStickY;
        }

        if ( m_bVehicleIsPlaneOrHeli )
        {
            bool bState1, bState2;
            if ( ! BitStream.ReadBit ( bState1 ) ||
                 ! BitStream.ReadBit ( bState2 ) )
                 return false;
            m_sharedControllerState.LeftShoulder2 = bState1 * 255;
            m_sharedControllerState.RightShoulder2 = bState2 * 255;
        }
    }

    return true;
}


//
// Should do the same this as what CKeysyncPacket::Write() does
//
bool CSimKeysyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    // Write the source player id
    BitStream.Write ( m_PlayerID );

    // Write the keysync data
    WriteSmallKeysync ( m_sharedControllerState, CControllerState (), BitStream );

    // Write the flags
    BitStream.Write ( &m_Cache.flags );

    // If he's shooting or aiming
    if ( m_sharedControllerState.ButtonCircle || m_sharedControllerState.RightShoulder1 )
    {
        // Write his current weapon slot
        unsigned int uiSlot = m_Cache.ucWeaponSlot;   // check m_Cache.bWeaponCorrect ! 
        SWeaponSlotSync slot;
        slot.data.uiSlot = uiSlot;
        BitStream.Write ( &slot );

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            // Write his ammo in clip
            SWeaponAmmoSync ammo ( m_ucPlayerGotWeaponType, false, true );
            ammo.data.usAmmoInClip = m_Cache.usAmmoInClip;
            BitStream.Write ( &ammo );

            // Write the weapon aim data
            SWeaponAimSync aim ( 0.0f );
            aim.data.vecOrigin = m_Cache.vecSniperSource;
            aim.data.vecTarget = m_Cache.vecTargetting;
            aim.data.fArm = m_Cache.fAimDirection;
            BitStream.Write ( &aim );

            // Write the driveby aim directoin
            BitStream.Write ( m_Cache.ucDriveByDirection );
        }
    }

    // If he's in a vehicle, read out the small vehicle specific data
    if ( m_bPlayerHasOccupiedVehicle && m_Cache.flags.data.bSyncingVehicle )
    {
        if ( CVehicleManager::HasTurret ( m_usVehicleGotModel ) )
            BitStream.Write ( &m_Cache.turretSync );

        if ( m_bVehicleHasHydraulics )
        {
            BitStream.Write ( m_sharedControllerState.RightStickX );
            BitStream.Write ( m_sharedControllerState.RightStickY );
        }

        if ( m_bVehicleIsPlaneOrHeli )
        {
            BitStream.WriteBit ( m_sharedControllerState.LeftShoulder2 != 0);
            BitStream.WriteBit ( m_sharedControllerState.RightShoulder2 != 0);
        }
    }

    return true;
}
