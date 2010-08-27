/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerPuresyncPacket.cpp
*  PURPOSE:     Player pure synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include "net/SyncStructures.h"

extern CGame* g_pGame;

bool CPlayerPuresyncPacket::Read ( NetBitStreamInterface& BitStream )
{
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Read out the time context
        unsigned char ucTimeContext = 0;
        if ( !BitStream.Read ( ucTimeContext ) )
            return false;

        // Only read this packet if it matches the current time context that
        // player is in.
        if ( !pSourcePlayer->CanUpdateSync ( ucTimeContext ) )
        {
            return false;
        }

        // Read out keys
        CControllerState ControllerState;
        ReadFullKeysync ( ControllerState, BitStream );
        pSourcePlayer->GetPad ()->NewControllerState ( ControllerState );

        // Read the flags
        SPlayerPuresyncFlags flags;
        if ( !BitStream.Read ( &flags ) )
            return false;

        pSourcePlayer->SetInWater ( flags.data.bIsInWater );
        pSourcePlayer->SetOnGround ( flags.data.bIsOnGround );
        pSourcePlayer->SetHasJetPack ( flags.data.bHasJetPack );
        pSourcePlayer->SetDucked ( flags.data.bIsDucked );
        pSourcePlayer->SetWearingGoggles ( flags.data.bWearsGoogles );
        pSourcePlayer->SetChoking ( flags.data.bIsChoking );
        pSourcePlayer->SetAkimboArmUp ( flags.data.bAkimboTargetUp );
        pSourcePlayer->SetOnFire ( flags.data.bIsOnFire );
        pSourcePlayer->SetStealthAiming ( flags.data.bStealthAiming );

        // Contact element
        CElement* pContactElement = NULL;
        if ( flags.data.bHasContact )
        {
            ElementID Temp;
            if ( !BitStream.ReadCompressed ( Temp ) )
                return false;
            pContactElement = CElementIDs::GetElement ( Temp );
        }
        CElement * pPreviousContactElement = pSourcePlayer->GetContactElement ();
        pSourcePlayer->SetContactElement ( pContactElement );

        if ( pPreviousContactElement != pContactElement )
        {
            // Call our onPlayerContact event
            CLuaArguments Arguments;
            if ( pPreviousContactElement )
                Arguments.PushElement ( pPreviousContactElement );
            else
                Arguments.PushNil ();
            if ( pContactElement )
                Arguments.PushElement ( pContactElement );
            else
                Arguments.PushNil ();

            pSourcePlayer->CallEvent ( "onPlayerContact", Arguments );
        }

        // Player position
        SPositionSync position ( false );
        if ( !BitStream.Read ( &position ) )
            return false;

        if ( pContactElement )
        {
            pSourcePlayer->SetContactPosition ( position.data.vecPosition );

            // Get the true position
            CVector vecTempPos = pContactElement->GetPosition ();
            position.data.vecPosition += vecTempPos;
        }
        pSourcePlayer->SetPosition ( position.data.vecPosition );

        // Player rotation
        SPedRotationSync rotation;
        if ( !BitStream.Read ( &rotation ) )
            return false;
        pSourcePlayer->SetRotation ( rotation.data.fRotation );

        // Move speed vector
        if ( flags.data.bSyncingVelocity )
        {
            SVelocitySync velocity;
            if ( !BitStream.Read ( &velocity ) )
                return false;
            pSourcePlayer->SetVelocity ( velocity.data.vecVelocity );
        }

        // Health ( stored with damage )
        SPlayerHealthSync health;
        if ( !BitStream.Read ( &health ) )
            return false;
        float fHealth = health.data.fValue;

        // Armor
        SPlayerArmorSync armor;
        if ( !BitStream.Read ( &armor ) )
            return false;

        float fArmor = armor.data.fValue;
        float fOldArmor = pSourcePlayer->GetArmor ();
        float fArmorLoss = fOldArmor - fArmor;

        pSourcePlayer->SetArmor ( fArmor );

        // Read out and set the camera rotation
        float fCameraRotation;
        if ( !BitStream.Read ( fCameraRotation ) )
            return false;
        pSourcePlayer->SetCameraRotation ( fCameraRotation );

        if ( flags.data.bHasAWeapon )
        {
            // Check client has the weapon we think he has
            unsigned char ucWeaponType;
            if ( !BitStream.Read ( ucWeaponType ) )
                return false;

            if ( pSourcePlayer->GetWeaponType () != ucWeaponType )
                return false;

            // Current weapon slot
            SWeaponSlotSync slot;
            if ( !BitStream.Read ( &slot ) )
                return false;
            unsigned int uiSlot = slot.data.uiSlot;

            pSourcePlayer->SetWeaponSlot ( uiSlot );

            if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
            {
                // Read out the ammo states
                SWeaponAmmoSync ammo ( pSourcePlayer->GetWeaponType (), true, true );
                if ( !BitStream.Read ( &ammo ) )
                    return false;
                pSourcePlayer->SetWeaponAmmoInClip ( ammo.data.usAmmoInClip );
                pSourcePlayer->SetWeaponTotalAmmo ( ammo.data.usTotalAmmo );

                // Read out the aim data
                SWeaponAimSync sync ( pSourcePlayer->GetWeaponRange (), ( ControllerState.RightShoulder1 || ControllerState.ButtonCircle ) );
                if ( !BitStream.Read ( &sync ) )
                    return false;

                // Set the arm directions and whether or not arms are up
                pSourcePlayer->SetAimDirection ( sync.data.fArm );

                // Read the aim data only if he's shooting or aiming
                if ( sync.isFull() )
                {
                    pSourcePlayer->SetSniperSourceVector ( sync.data.vecOrigin );
                    pSourcePlayer->SetTargettingVector ( sync.data.vecTarget );
                }
            }
            else
            {
                pSourcePlayer->SetWeaponAmmoInClip ( 1 );
                pSourcePlayer->SetWeaponTotalAmmo ( 1 );
            }
        }
        else
        {
            pSourcePlayer->SetWeaponSlot ( 0 );
            pSourcePlayer->SetWeaponAmmoInClip ( 1 );
            pSourcePlayer->SetWeaponTotalAmmo ( 1 );
        }

        // Read out damage info if changed
        if ( BitStream.ReadBit () == true )
        {
            ElementID DamagerID;
            if ( !BitStream.ReadCompressed ( DamagerID ) )
                return false;

            SWeaponTypeSync weaponType;
            if ( !BitStream.Read ( &weaponType ) )
                return false;

            SBodypartSync bodyPart;
            if ( !BitStream.Read ( &bodyPart ) )
                return false;

            pSourcePlayer->SetDamageInfo ( DamagerID, weaponType.data.ucWeaponType, bodyPart.data.uiBodypart );
        }

        // If we know the player's dead, make sure the health we send on is 0
        if ( pSourcePlayer->IsDead () )
            fHealth = 0.0f;

        float fOldHealth = pSourcePlayer->GetHealth ();
        float fHealthLoss = fOldHealth - fHealth;
        pSourcePlayer->SetHealth ( fHealth );

        // Less than last packet's frame?
        if ( fHealthLoss > 0 || fArmorLoss > 0 )
        {
            float fDamage = 0.0f;
            if ( fHealthLoss > 0 ) fDamage += fHealthLoss;
            if ( fArmorLoss > 0 ) fDamage += fArmorLoss;

            // Call the onPlayerDamage event
            CLuaArguments Arguments;
            CElement* pKillerElement = CElementIDs::GetElement ( pSourcePlayer->GetPlayerAttacker () );
            if ( pKillerElement ) Arguments.PushElement ( pKillerElement );
            else Arguments.PushNil ();
            Arguments.PushNumber ( pSourcePlayer->GetAttackWeapon () );
            Arguments.PushNumber ( pSourcePlayer->GetAttackBodyPart () );
            Arguments.PushNumber ( fDamage );

            pSourcePlayer->CallEvent ( "onPlayerDamage", Arguments );
        }

        // Success
        return true;
    }

    return false;
}

/*
template < class T >
bool CompareAndSet ( const T& current, T& previous )
{
    bool bChanged = current != previous;
    previous = current;
    return bChanged;
}
*/

bool CPlayerPuresyncPacket::Write ( NetBitStreamInterface& BitStream ) const
{
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        ElementID PlayerID = pSourcePlayer->GetID ();
        unsigned short usLatency = pSourcePlayer->GetPing ();
        const CControllerState& ControllerState = pSourcePlayer->GetPad ()->GetCurrentControllerState ();
        CElement* pContactElement = pSourcePlayer->GetContactElement ();

        // Get current weapon slot
        unsigned char ucWeaponSlot = pSourcePlayer->GetWeaponSlot ();

        // Flags
        SPlayerPuresyncFlags flags;
        flags.data.bIsInWater       = ( pSourcePlayer->IsInWater () == true );
        flags.data.bIsOnGround      = ( pSourcePlayer->IsOnGround () == true );
        flags.data.bHasJetPack      = ( pSourcePlayer->HasJetPack () == true );
        flags.data.bIsDucked        = ( pSourcePlayer->IsDucked () == true );
        flags.data.bWearsGoogles    = ( pSourcePlayer->IsWearingGoggles () == true );
        flags.data.bHasContact      = ( pContactElement != NULL );
        flags.data.bIsChoking       = ( pSourcePlayer->IsChoking () == true );
        flags.data.bAkimboTargetUp  = ( pSourcePlayer->IsAkimboArmUp () == true );
        flags.data.bIsOnFire        = ( pSourcePlayer->IsOnFire () == true );
        flags.data.bHasAWeapon      = ( ucWeaponSlot != 0 );
        flags.data.bSyncingVelocity = ( !flags.data.bIsOnGround || pSourcePlayer->IsSyncingVelocity () );
        flags.data.bStealthAiming   = ( pSourcePlayer->IsStealthAiming () == true );

        CVector vecPosition = pSourcePlayer->GetPosition ();
        if ( pContactElement )
            pSourcePlayer->GetContactPosition ( vecPosition );
        float fCameraRotation = pSourcePlayer->GetCameraRotation ();

        BitStream.WriteCompressed ( PlayerID );

        // Write the time context
        BitStream.Write ( pSourcePlayer->GetSyncTimeContext () );

        BitStream.WriteCompressed ( usLatency );
        WriteFullKeysync ( ControllerState, BitStream );
/*
        // Figure out what to send
        SPlayerPuresyncSentHeader sent;
        sent.bFlags             = CompareAndSet ( usFlags,          pSourcePlayer->lastSent.usFlags );
        sent.bPosition          = CompareAndSet ( vecPosition,      pSourcePlayer->lastSent.vecPosition );
        sent.bRotation          = CompareAndSet ( fRotation,        pSourcePlayer->lastSent.fRotation );
        sent.bVelocity          = CompareAndSet ( vecVelocity,      pSourcePlayer->lastSent.vecVelocity );
        sent.bHealth            = CompareAndSet ( ucHealth,         pSourcePlayer->lastSent.ucHealth );
        sent.bArmor             = CompareAndSet ( ucArmor,          pSourcePlayer->lastSent.ucArmor );
        sent.bCameraRotation    = CompareAndSet ( fCameraRotation,  pSourcePlayer->lastSent.fCameraRotation );
        sent.bWeaponType        = CompareAndSet ( ucWeaponType,     pSourcePlayer->lastSent.ucWeaponType );
        sent.Write ( BitStream );

        if ( sent.bPosition )
        {
            BitStream.Write ( vecPosition.fX );
            BitStream.Write ( vecPosition.fY );
            BitStream.Write ( vecPosition.fZ );
        }

        if ( sent.bRotation )
            BitStream.Write ( fRotation );

        etc... Could also do a 'sent' header in WriteFullKeysync
*/
        BitStream.Write ( &flags );

        if ( pContactElement )
            BitStream.WriteCompressed ( pContactElement->GetID () );

        SPositionSync position ( false );
        position.data.vecPosition = vecPosition;
        BitStream.Write ( &position );

        SPedRotationSync rotation;
        rotation.data.fRotation = pSourcePlayer->GetRotation ();
        BitStream.Write ( &rotation );

        if ( flags.data.bSyncingVelocity )
        {
            SVelocitySync velocity;
            pSourcePlayer->GetVelocity ( velocity.data.vecVelocity );
            BitStream.Write ( &velocity );
        }

        // Player health and armor
        SPlayerHealthSync health;
        health.data.fValue = pSourcePlayer->GetHealth ();
        BitStream.Write ( &health );

        SPlayerArmorSync armor;
        armor.data.fValue = pSourcePlayer->GetArmor ();
        BitStream.Write ( &armor );

        BitStream.Write ( fCameraRotation );

        if ( flags.data.bHasAWeapon )
        {
            unsigned int uiSlot = ucWeaponSlot;
            SWeaponSlotSync slot;
            slot.data.uiSlot = uiSlot;
            BitStream.Write ( &slot );

            if ( CWeaponNames::DoesSlotHaveAmmo ( uiSlot ) )
            {
                unsigned short usWeaponAmmoInClip = pSourcePlayer->GetWeaponAmmoInClip ();
/*
            // Figure out what to send
            SPlayerPuresyncWeaponSentHeader sent;
            sent.bWeaponAmmoInClip      = CompareAndSet ( usWeaponAmmoInClip,   pSourcePlayer->lastSent.usWeaponAmmoInClip );
            sent.bAimDirectionX         = CompareAndSet ( fAimDirectionX,       pSourcePlayer->lastSent.fAimDirectionX );
            sent.bAimDirectionY         = CompareAndSet ( fAimDirectionY,       pSourcePlayer->lastSent.fAimDirectionY );
            sent.bSniperSource          = CompareAndSet ( vecSniperSource,      pSourcePlayer->lastSent.vecSniperSource );
            sent.bTargetting            = CompareAndSet ( vecTargetting,        pSourcePlayer->lastSent.vecTargetting );
            sent.Write ( BitStream );

            if ( sent.bWeaponAmmoInClip )
                BitStream.Write ( usWeaponAmmoInClip );

            if ( sent.bAimDirectionX )
                BitStream.Write ( fAimDirectionX );
            if ( sent.bAimDirectionY )
                BitStream.Write ( fAimDirectionY );

            etc...
*/

                SWeaponAmmoSync ammo ( pSourcePlayer->GetWeaponType (), false, true );
                ammo.data.usAmmoInClip = usWeaponAmmoInClip;
                BitStream.Write ( &ammo );

                SWeaponAimSync aim ( 0.0f, ( ControllerState.RightShoulder1 || ControllerState.ButtonCircle ) );
                aim.data.fArm = pSourcePlayer->GetAimDirection ();

                // Write the aim data only if he's aiming or shooting
                if ( aim.isFull() )
                {
                    aim.data.vecOrigin = pSourcePlayer->GetSniperSourceVector ();
                    pSourcePlayer->GetTargettingVector ( aim.data.vecTarget );
                }
                BitStream.Write ( &aim );
            }
        }

        // Success
        return true;
    }

    return false;
}
