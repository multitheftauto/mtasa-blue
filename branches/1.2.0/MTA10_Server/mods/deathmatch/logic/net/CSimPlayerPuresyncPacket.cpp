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


CSimPlayerPuresyncPacket::CSimPlayerPuresyncPacket ( ElementID PlayerID,
                                                     ushort PlayerLatency,
                                                     uchar PlayerSyncTimeContext,
                                                     uchar PlayerGotWeaponType,
                                                     float WeaponRange )
    : m_PlayerID ( PlayerID )
    , m_PlayerLatency ( PlayerLatency )
    , m_PlayerSyncTimeContext ( PlayerSyncTimeContext )
    , m_PlayerGotWeaponType ( PlayerGotWeaponType )
    , m_WeaponRange ( WeaponRange )
{
}


//
// Should do the same this as what CPlayerPuresyncPacket::Read() does
//
bool CSimPlayerPuresyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    // Read out the time context
    if ( !BitStream.Read ( m_Cache.ucTimeContext ) )
        return false;

    // If incoming time context is zero, use the one here
    if ( m_Cache.ucTimeContext == 0 )
        m_Cache.ucTimeContext = m_PlayerSyncTimeContext;

    // Only read this packet if it matches the current time context that
    // player is in.
    if ( !CanUpdateSync ( m_Cache.ucTimeContext ) )
    {
        return false;
    }

    // Read out keys
    ReadFullKeysync ( m_Cache.ControllerState, BitStream );

    // Read the flags
    if ( !BitStream.Read ( &m_Cache.flags ) )
        return false;

    // Contact element
    if ( m_Cache.flags.data.bHasContact )
    {
        if ( !BitStream.Read ( m_Cache.ContactElementID ) )
            return false;
    }

    // Player position
    SPositionSync position ( false );
    if ( !BitStream.Read ( &position ) )
        return false;
    m_Cache.Position = position.data.vecPosition;

    // Player rotation
    SPedRotationSync rotation;
    if ( !BitStream.Read ( &rotation ) )
        return false;
    m_Cache.fRotation = rotation.data.fRotation;

    // Move speed vector
    if ( m_Cache.flags.data.bSyncingVelocity )
    {
        SVelocitySync velocity;
        if ( !BitStream.Read ( &velocity ) )
            return false;
        m_Cache.Velocity = velocity.data.vecVelocity;
    }

    // Health ( stored with damage )
    SPlayerHealthSync health;
    if ( !BitStream.Read ( &health ) )
        return false;
    m_Cache.fHealth = health.data.fValue;

    // Armor
    SPlayerArmorSync armor;
    if ( !BitStream.Read ( &armor ) )
        return false;

    m_Cache.fArmor = armor.data.fValue;

    // Read out the camera rotation
    if ( !BitStream.Read ( m_Cache.fCameraRotation ) )
        return false;

    // Read the camera orientation
    ReadCameraOrientation ( position.data.vecPosition, BitStream, m_Cache.vecCamPosition, m_Cache.vecCamFwd );

    if ( m_Cache.flags.data.bHasAWeapon )
    {
        // Read client weapon data, but only apply it if the weapon matches with the server
        uchar ucUseWeaponType = m_PlayerGotWeaponType;
        bool bWeaponCorrect = true;
        m_Cache.bIsAimFull = false;

       // Check client has the weapon we think he has
        unsigned char ucClientWeaponType;
        if ( !BitStream.Read ( ucClientWeaponType ) )
            return false;

        if ( m_PlayerGotWeaponType != ucClientWeaponType )
        {
            bWeaponCorrect = false;                 // Possibly old weapon data.
            ucUseWeaponType = ucClientWeaponType;   // Use the packet supplied weapon type to skip over the correct amount of data
        }

        // Update check counts
        m_Cache.bWeaponCorrect = bWeaponCorrect;

        // Current weapon slot
        SWeaponSlotSync slot;
        if ( !BitStream.Read ( &slot ) )
            return false;
        unsigned int uiSlot = slot.data.uiSlot;

        // Set weapon slot
        if ( bWeaponCorrect )
            m_Cache.ucWeaponSlot = uiSlot;

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            // Read out the ammo states
            SWeaponAmmoSync ammo ( ucUseWeaponType, true, true );
            if ( !BitStream.Read ( &ammo ) )
                return false;

            // Read out the aim data
            SWeaponAimSync sync ( m_WeaponRange, ( m_Cache.ControllerState.RightShoulder1 || m_Cache.ControllerState.ButtonCircle ) );
            if ( !BitStream.Read ( &sync ) )
                return false;

            // Set the arm directions and whether or not arms are up
            // Save this here incase weapon is not correct
            m_Cache.fAimDirection = sync.data.fArm;

            if ( bWeaponCorrect )
            {
                // Set the ammo states
                m_Cache.usAmmoInClip = ammo.data.usAmmoInClip;
                m_Cache.usTotalAmmo = ammo.data.usTotalAmmo;

                // Read the aim data only if he's shooting or aiming
                if ( sync.isFull() )
                {
                    m_Cache.vecSniperSource = sync.data.vecOrigin;
                    m_Cache.vecTargetting = sync.data.vecTarget;
                    m_Cache.bIsAimFull = true;
                }
            }
        }
        else
        {
            if ( bWeaponCorrect )
            {
                m_Cache.usAmmoInClip = 1;
                m_Cache.usTotalAmmo = 1;
            }
        }
    }
    else
    {
        m_Cache.ucWeaponSlot = 0;
        m_Cache.usAmmoInClip = 1;
        m_Cache.usTotalAmmo = 1;
    }

    // Success
    return true;
}


//
// Should do the same this as what CPlayerPuresyncPacket::Write() does
//
bool CSimPlayerPuresyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    BitStream.Write ( m_PlayerID );

    // Write the time context
    BitStream.Write ( m_Cache.ucTimeContext );

    BitStream.WriteCompressed ( m_PlayerLatency );
    WriteFullKeysync ( m_Cache.ControllerState, BitStream );

    BitStream.Write ( &m_Cache.flags );

    if ( m_Cache.flags.data.bHasContact )
        BitStream.Write ( m_Cache.ContactElementID );

    SPositionSync position ( false );
    position.data.vecPosition = m_Cache.Position;
    BitStream.Write ( &position );

    SPedRotationSync rotation;
    rotation.data.fRotation = m_Cache.fRotation;
    BitStream.Write ( &rotation );

    if ( m_Cache.flags.data.bSyncingVelocity )
    {
        SVelocitySync velocity;
        velocity.data.vecVelocity = m_Cache.Velocity;
        BitStream.Write ( &velocity );
    }

    // Player health and armor
    SPlayerHealthSync health;
    health.data.fValue = m_Cache.fHealth;
    BitStream.Write ( &health );

    SPlayerArmorSync armor;
    armor.data.fValue = m_Cache.fArmor;
    BitStream.Write ( &armor );

    BitStream.Write ( m_Cache.fCameraRotation );

    if ( m_Cache.flags.data.bHasAWeapon )
    {
        unsigned int uiSlot = m_Cache.ucWeaponSlot;   // check m_Cache.bWeaponCorrect !
        SWeaponSlotSync slot;
        slot.data.uiSlot = uiSlot;
        BitStream.Write ( &slot );

        if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
        {
            unsigned short usWeaponAmmoInClip = m_Cache.usAmmoInClip;

            SWeaponAmmoSync ammo ( m_PlayerGotWeaponType, false, true );
            ammo.data.usAmmoInClip = usWeaponAmmoInClip;
            BitStream.Write ( &ammo );

            SWeaponAimSync aim ( 0.0f, m_Cache.bIsAimFull );
            aim.data.fArm = m_Cache.fAimDirection;

            // Write the aim data only if he's aiming or shooting
            if ( aim.isFull() )
            {
                aim.data.vecOrigin = m_Cache.vecSniperSource;
                aim.data.vecTarget = m_Cache.vecTargetting;
            }
            BitStream.Write ( &aim );
        }
    }

    // Success
    return true;

}
