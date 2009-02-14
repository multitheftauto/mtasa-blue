/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/rpc/CWeaponRPCs.cpp
*  PURPOSE:     Weapon remote procedure calls
*  DEVELOPERS:  Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include <StdInc.h>
#include "CWeaponRPCs.h"

void CWeaponRPCs::LoadFunctions ( void )
{
    AddHandler ( GIVE_WEAPON, GiveWeapon, "GiveWeapon" );
    AddHandler ( TAKE_WEAPON, TakeWeapon, "TakeWeapon" );
    AddHandler ( TAKE_ALL_WEAPONS, TakeAllWeapons, "TakeAllWeapons" );
	AddHandler ( GIVE_WEAPON_AMMO, GiveWeaponAmmo, "GiveWeaponAmmo" );
    AddHandler ( TAKE_WEAPON_AMMO, TakeWeaponAmmo, "TakeWeaponAmmo" );
    AddHandler ( SET_WEAPON_AMMO, SetWeaponAmmo, "SetWeaponAmmo" );
	AddHandler ( SET_WEAPON_SLOT, SetWeaponSlot, "SetWeaponSlot" );
}


void CWeaponRPCs::GiveWeapon ( NetBitStreamInterface& bitStream )
{
    // Read out weapon id and ammo amount
    ElementID ID;
    unsigned char ucWeaponID;
    unsigned short usAmmo;
    unsigned char ucGiveWeapon;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucWeaponID ) &&
         bitStream.Read ( usAmmo ) &&
         bitStream.Read ( ucGiveWeapon ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            // Don't change remote players weapons (affects sync)
            if ( pPed->GetType () == CCLIENTPED || ( CClientPlayer * ) pPed == m_pPlayerManager->GetLocalPlayer () )
            {
                // Valid weapon id?
                if ( ucWeaponID == 0 || CClientPickupManager::IsValidWeaponID ( ucWeaponID ) )
                {
                    // Adjust the ammo to 9999 if it's above
                    if ( usAmmo > 9999 ) usAmmo = 9999;

                    // Give the local player the weapon
                    CWeapon* pPlayerWeapon = NULL;
                    if ( ucWeaponID != 0 )
                    {
                        pPlayerWeapon = pPed->GiveWeapon ( static_cast < eWeaponType > ( ucWeaponID ), usAmmo );
                        if ( pPlayerWeapon && ucGiveWeapon == 1 )
                            pPlayerWeapon->SetAsCurrentWeapon ();
                    }
                    else
                    {
                        // This could be entered into a hack of the year competition. Its about as hacky as it gets.
                        // For some stupid reason, going from brassknuckles to unarmed causes the knuckles to remain 
                        // on display but unusable. So, what we do is switch to a MELEE weapon (creating one if necessary)
                        // then switch back to unarmed from there, which works fine.
                        eWeaponType unarmedWeapon = pPed->GetWeapon (WEAPONSLOT_TYPE_UNARMED)->GetType();
                        pPed->RemoveWeapon ( unarmedWeapon );
                        if ( ucGiveWeapon == 1 || pPed->GetCurrentWeaponSlot() == WEAPONSLOT_TYPE_UNARMED )
                        {
                            CWeapon * oldWeapon = NULL;
                            if ( unarmedWeapon == WEAPONTYPE_BRASSKNUCKLE )
                            {
                                oldWeapon = pPed->GetWeapon(WEAPONSLOT_TYPE_MELEE);
                                if ( oldWeapon && oldWeapon->GetType() == WEAPONTYPE_UNARMED )
                                {
                                    oldWeapon = pPed->GiveWeapon(WEAPONTYPE_GOLFCLUB, 100);
                                }
                                else
                                {
                                    oldWeapon = NULL;
                                }
                                pPed->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_MELEE );
                            }

                            // switch to the unarmed slot
                            pPed->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );

                            // if we created a special MELEE weapon just for this, remove it now
                            if ( oldWeapon )
                            {
                                oldWeapon->Remove();
                            }
                        }
                    }
                }
            }
        }
    }
}


void CWeaponRPCs::TakeWeapon ( NetBitStreamInterface& bitStream )
{
    // Read out weapon id and ammo amount
    ElementID ID;
    unsigned char ucWeaponID;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucWeaponID )  )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            // Don't change remote players weapons (affects sync)
            if ( pPed->GetType () == CCLIENTPED || ( CClientPlayer * ) pPed == m_pPlayerManager->GetLocalPlayer () )
            {
                // Is the weapon id valid? (may not be neccessary, just being safe)
                if ( CClientPickupManager::IsValidWeaponID ( ucWeaponID ) )
                {
                    // Remove the weapon
                    pPed->RemoveWeapon ( static_cast < eWeaponType > ( ucWeaponID ) );
                }
            }
        }
    }
}


void CWeaponRPCs::TakeAllWeapons ( NetBitStreamInterface& bitStream )
{
    ElementID ID;
    if ( bitStream.Read ( ID ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            // Remove all our weapons
            pPed->RemoveAllWeapons ();
        }
    }
}


void CWeaponRPCs::GiveWeaponAmmo ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
    unsigned char ucWeaponID;
    unsigned short usAmmo;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucWeaponID ) &&
         bitStream.Read ( usAmmo ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            // Valid weapon id?
            if ( !CClientPickupManager::IsValidWeaponID ( ucWeaponID ) ) return;

		    // Do we have it?
		    CWeapon* pPlayerWeapon = pPed->GetWeapon ( (eWeaponType) ucWeaponID );
		    if ( pPlayerWeapon == NULL ) return;

		    unsigned long ulWeaponAmmo = pPlayerWeapon->GetAmmoTotal ();
		    ulWeaponAmmo += usAmmo;
            // Adjust the ammo to 9999 if it's above
            if ( ulWeaponAmmo > 9999 )
            {
                ulWeaponAmmo = 9999;
            }

            // Add the weapon ammo
		    pPlayerWeapon->SetAmmoTotal ( ulWeaponAmmo );
        }
    }
}


void CWeaponRPCs::TakeWeaponAmmo ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
    unsigned char ucWeaponID;
    unsigned short usAmmo;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucWeaponID ) &&
         bitStream.Read ( usAmmo ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            // Valid weapon id?
            if ( !CClientPickupManager::IsValidWeaponID ( ucWeaponID ) ) return;

	        // Do we have it?
	        CWeapon* pPlayerWeapon = pPed->GetWeapon ( (eWeaponType) ucWeaponID );
	        if ( pPlayerWeapon == NULL ) return;

            unsigned char ucAmmoInClip = pPlayerWeapon->GetAmmoInClip ();
            pPlayerWeapon->SetAmmoInClip ( 0 );

	        unsigned long ulWeaponAmmo = pPlayerWeapon->GetAmmoTotal ();
            if ( ulWeaponAmmo - usAmmo < 0 )
                ulWeaponAmmo = 0;
            else
                ulWeaponAmmo -= usAmmo;

            // Remove the weapon ammo
	        pPlayerWeapon->SetAmmoTotal ( ulWeaponAmmo );

            if ( pPlayerWeapon->GetAmmoTotal () > ucAmmoInClip )
                pPlayerWeapon->SetAmmoInClip ( ucAmmoInClip );
            else if ( pPlayerWeapon->GetAmmoTotal () <= ucAmmoInClip )
                pPlayerWeapon->SetAmmoInClip ( pPlayerWeapon->GetAmmoTotal () );
        }
    }
}


void CWeaponRPCs::SetWeaponSlot ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
    unsigned char ucSlot;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucSlot ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
		    pPed->SetCurrentWeaponSlot ( (eWeaponSlot) ucSlot );
        }
    }
}


void CWeaponRPCs::SetWeaponAmmo ( NetBitStreamInterface& bitStream )
{
	ElementID ID;
    unsigned char ucWeaponID;
    unsigned short usAmmo;
    unsigned short usAmmoInClip;
    if ( bitStream.Read ( ID ) &&
         bitStream.Read ( ucWeaponID ) &&
         bitStream.Read ( usAmmo )  &&
         bitStream.Read ( usAmmoInClip ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( ID, true );
        if ( pPed )
        {
            // Valid weapon id?
            if ( !CClientPickupManager::IsValidWeaponID ( ucWeaponID ) ) return;

		    // Do we have it?
		    CWeapon* pPlayerWeapon = pPed->GetWeapon ( (eWeaponType) ucWeaponID );
		    if ( pPlayerWeapon == NULL ) return;

            unsigned char ucAmmoInClip = pPlayerWeapon->GetAmmoInClip ();
            pPlayerWeapon->SetAmmoInClip ( usAmmoInClip );
            pPlayerWeapon->SetAmmoTotal ( usAmmo );

            if ( !usAmmoInClip )
            {
                if ( usAmmo > ucAmmoInClip )
                    pPlayerWeapon->SetAmmoInClip ( ucAmmoInClip );
                else if ( usAmmo <= ucAmmoInClip )
                    pPlayerWeapon->SetAmmoInClip ( usAmmo );
            }
        }
    }
}