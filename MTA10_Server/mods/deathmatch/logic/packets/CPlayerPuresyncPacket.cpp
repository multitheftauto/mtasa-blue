/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/packets/CPlayerPuresyncPacket.cpp
*  PURPOSE:     Player pure synchronization packet class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Cecill Etheredge <>
*               Alberto Alonso <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CGame* g_pGame;

bool CPlayerPuresyncPacket::Read ( NetServerBitStreamInterface& BitStream )
{
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        // Read out the time context
        unsigned char ucTimeContext = 0;
        BitStream.Read ( ucTimeContext );

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

        unsigned short usFlags;
        BitStream.Read ( (unsigned short) usFlags );
        pSourcePlayer->SetInWater ( ( usFlags & 0x01 ) ? true:false );
        pSourcePlayer->SetOnGround ( ( usFlags & 0x02 ) ? true:false );
        pSourcePlayer->SetHasJetPack ( ( usFlags & 0x04 ) ? true:false );
        pSourcePlayer->SetDucked ( ( usFlags & 0x08 ) ? true:false );
        pSourcePlayer->SetWearingGoggles ( ( usFlags & 0x10 ) ? true:false );
        bool bInContact = ( usFlags & 0x20 ) ? true:false;
        pSourcePlayer->SetChoking ( ( usFlags & 0x40 ) ? true:false );
        pSourcePlayer->SetAkimboArmUp ( ( usFlags & 0x80 ) ? true:false );
        pSourcePlayer->SetOnFire ( ( usFlags & 0x100 ) ? true:false );

        // Contact element
        CElement* pContactElement = NULL;
        if ( bInContact )
        {            
            ElementID Temp;
            BitStream.Read ( Temp );
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
        CVector vecTemp;
        BitStream.Read ( vecTemp.fX );
        BitStream.Read ( vecTemp.fY );
        BitStream.Read ( vecTemp.fZ );
        if ( pContactElement )
        {
            pSourcePlayer->SetContactPosition ( vecTemp );
            
            // Get the true position
            CVector vecTempPos = pContactElement->GetPosition ();
            vecTemp += vecTempPos;
        }
        pSourcePlayer->SetPosition ( vecTemp );

        // Player rotation
        float fRotation = 0.0f;
        BitStream.Read ( fRotation );
        pSourcePlayer->SetRotation ( fRotation );

        // Move speed vector
        BitStream.Read ( vecTemp.fX );
        BitStream.Read ( vecTemp.fY );
        BitStream.Read ( vecTemp.fZ );
        pSourcePlayer->SetVelocity ( vecTemp );

        // Health ( stored with damage )
        unsigned char ucHealth;
	    BitStream.Read ( ucHealth );

        // Armor
        unsigned char ucArmor;
        BitStream.Read ( ucArmor );

        float fArmor = static_cast < float > ( ucArmor ) / 1.25f;
        float fOldArmor = pSourcePlayer->GetArmor ();
		float fArmorLoss = fOldArmor - fArmor;
        
        pSourcePlayer->SetArmor ( fArmor );

        // Read out and set the camera rotation
        float fCameraRotation;
        BitStream.Read ( fCameraRotation );
        pSourcePlayer->SetCameraRotation ( fCameraRotation );        

        // Current weapon slot
        unsigned char ucPreviousWeaponType = pSourcePlayer->GetWeaponType ();
        unsigned char ucCurrentWeaponSlot, ucCurrentWeaponType;
        BitStream.Read ( ucCurrentWeaponSlot );
		BitStream.Read ( ucCurrentWeaponType );

		if ( ucCurrentWeaponType != ucPreviousWeaponType )
        {
            // Call our weapon switch command
            CLuaArguments Arguments;
            Arguments.PushNumber ( ucPreviousWeaponType );
            Arguments.PushNumber ( ucCurrentWeaponType );
            bool bContinue = pSourcePlayer->CallEvent ( "onPlayerWeaponSwitch", Arguments );

			// Set the weapon type and slot
			pSourcePlayer->SetWeaponType ( ucCurrentWeaponType );
			pSourcePlayer->SetWeaponSlot ( ucCurrentWeaponSlot );
        }

        if ( ucCurrentWeaponType != 0 )
        {
            // Read out the ammo states
            unsigned short usAmmoInClip;
            BitStream.Read ( usAmmoInClip );
            pSourcePlayer->SetWeaponAmmoInClip ( usAmmoInClip );
            
            unsigned short usTotalAmmo;
            BitStream.Read ( usTotalAmmo );
            pSourcePlayer->SetWeaponTotalAmmo ( usTotalAmmo );

            // Read out the aim directions
            float fArmX, fArmY;
			BitStream.Read ( fArmX );
			BitStream.Read ( fArmY );

			// Set the arm directions and whether or not arms are up
			pSourcePlayer->SetAimDirections ( fArmX, fArmY );

            // Source vector
            BitStream.Read ( vecTemp.fX );
            BitStream.Read ( vecTemp.fY );
            BitStream.Read ( vecTemp.fZ );
            pSourcePlayer->SetSniperSourceVector ( vecTemp );

            // Target vector
            BitStream.Read ( vecTemp.fX );
            BitStream.Read ( vecTemp.fY );
            BitStream.Read ( vecTemp.fZ );
            pSourcePlayer->SetTargettingVector ( vecTemp );
        }

        // Read out damage info if changed
        ElementID DamagerID;
        BitStream.Read ( DamagerID );
        
        // If it's different, carry on reading
        if ( DamagerID != RESERVED_ELEMENT_ID )
        {            
            CElement* pElement = CElementIDs::GetElement ( DamagerID );

            unsigned char ucWeapon, ucBodyPart;
            BitStream.Read ( ucWeapon );
            BitStream.Read ( ucBodyPart );

            pSourcePlayer->SetDamageInfo ( pElement, ucWeapon, ucBodyPart );
        }

        float fHealth = static_cast < float > ( ucHealth ) / 1.25f;

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
            CElement* pKillerElement = pSourcePlayer->GetPlayerAttacker ();
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


bool CPlayerPuresyncPacket::Write ( NetServerBitStreamInterface& BitStream ) const
{
    if ( m_pSourceElement )
    {
        CPlayer * pSourcePlayer = static_cast < CPlayer * > ( m_pSourceElement );

        ElementID PlayerID = pSourcePlayer->GetID ();
        unsigned short usLatency = pSourcePlayer->GetPing ();
        const CControllerState& ControllerState = pSourcePlayer->GetPad ()->GetCurrentControllerState ();
        CElement* pContactElement = pSourcePlayer->GetContactElement ();

        // Flags
        unsigned short usFlags = 0;
        if ( pSourcePlayer->IsInWater () ) usFlags |= 0x01;
        if ( pSourcePlayer->IsOnGround () ) usFlags |= 0x02;
        if ( pSourcePlayer->HasJetPack () ) usFlags |= 0x04;
        if ( pSourcePlayer->IsDucked () ) usFlags |= 0x08;
        if ( pSourcePlayer->IsWearingGoggles () ) usFlags |= 0x10;
        if ( pContactElement ) usFlags |= 0x20;
        if ( pSourcePlayer->IsChoking () ) usFlags |= 0x40;
        if ( pSourcePlayer->IsAkimboArmUp () ) usFlags |= 0x80;
        if ( pSourcePlayer->IsOnFire () ) usFlags |= 0x100;

        CVector vecPosition = pSourcePlayer->GetPosition ();
        if ( pContactElement )
            pSourcePlayer->GetContactPosition ( vecPosition );
        float fRotation = pSourcePlayer->GetRotation ();
        CVector vecVelocity;
        pSourcePlayer->GetVelocity ( vecVelocity );
        unsigned char ucHealth = static_cast < unsigned char > ( pSourcePlayer->GetHealth () * 1.25f );
        unsigned char ucArmor = static_cast < unsigned char > ( pSourcePlayer->GetArmor () * 1.25f );
        float fCameraRotation = pSourcePlayer->GetCameraRotation ();
        unsigned char ucWeaponType = pSourcePlayer->GetWeaponType ();

        BitStream.Write ( PlayerID );

        // Write the time context
        BitStream.Write ( pSourcePlayer->GetSyncTimeContext () );

        BitStream.Write ( usLatency );
        WriteFullKeysync ( ControllerState, BitStream );   
        BitStream.Write ( usFlags );
        
        if ( pContactElement )
            BitStream.Write ( pContactElement->GetID () );            

        BitStream.Write ( vecPosition.fX );
        BitStream.Write ( vecPosition.fY );
        BitStream.Write ( vecPosition.fZ ); 

        BitStream.Write ( fRotation );       
        BitStream.Write ( vecVelocity.fX );
        BitStream.Write ( vecVelocity.fY );
        BitStream.Write ( vecVelocity.fZ );            
	    BitStream.Write ( ucHealth );
        BitStream.Write ( ucArmor );
        BitStream.Write ( fCameraRotation );

        BitStream.Write ( ucWeaponType );
        if ( ucWeaponType != 0 )
        {
            unsigned short usWeaponAmmoInClip = pSourcePlayer->GetWeaponAmmoInClip ();
            float fAimDirectionX = pSourcePlayer->GetAimDirectionX ();
            float fAimDirectionY = pSourcePlayer->GetAimDirectionY ();
			bool bArmUp = pSourcePlayer->IsAkimboArmUp ();
            CVector vecSniperSource = pSourcePlayer->GetSniperSourceVector ();
            CVector vecTargetting;
            pSourcePlayer->GetTargettingVector ( vecTargetting );

            BitStream.Write ( usWeaponAmmoInClip );

			BitStream.Write ( fAimDirectionX );
			BitStream.Write ( fAimDirectionY );

            BitStream.Write ( vecSniperSource.fX );
            BitStream.Write ( vecSniperSource.fY );
            BitStream.Write ( vecSniperSource.fZ );
            BitStream.Write ( vecTargetting.fX );
            BitStream.Write ( vecTargetting.fY );
            BitStream.Write ( vecTargetting.fZ );
        }        

        // Success
        return true;
    }

    return false;
}
