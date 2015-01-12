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
#include "net/SyncStructures.h"

void CWeaponRPCs::LoadFunctions ( void )
{
    AddHandler ( GIVE_WEAPON, GiveWeapon, "GiveWeapon" );
    AddHandler ( TAKE_WEAPON, TakeWeapon, "TakeWeapon" );
    AddHandler ( TAKE_ALL_WEAPONS, TakeAllWeapons, "TakeAllWeapons" );
    AddHandler ( SET_WEAPON_AMMO, SetWeaponAmmo, "SetWeaponAmmo" );
    AddHandler ( SET_WEAPON_SLOT, SetWeaponSlot, "SetWeaponSlot" );
}


void CWeaponRPCs::GiveWeapon ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out weapon id and ammo amount
    SWeaponTypeSync weaponType;

    if ( bitStream.Read ( &weaponType ) )
    {
        SWeaponAmmoSync ammo ( weaponType.data.ucWeaponType, true, false );
        if ( bitStream.Read ( &ammo ) )
        {
            bool bGiveWeapon = bitStream.ReadBit ();
            unsigned char ucWeaponID = weaponType.data.ucWeaponType;
            unsigned short usAmmo = ammo.data.usTotalAmmo;

            CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
            if ( pPed )
            {
                // Don't change remote players weapons (affects sync)
                if ( pPed->GetType () == CCLIENTPED || pPed->GetType () == CCLIENTPLAYER )
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
                            if ( pPlayerWeapon && bGiveWeapon )
                                pPlayerWeapon->SetAsCurrentWeapon ();
                            
                            // Store the ammo so it's not lost if a ped is streamed out
                            if ( pPed->GetType () == CCLIENTPED )
                            {
                                uchar ucSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
                                pPed->m_usWeaponAmmo [ ucSlot ] += usAmmo;
                            }
                        }
                        else
                        {
                            // This could be entered into a hack of the year competition. Its about as hacky as it gets.
                            // For some stupid reason, going from brassknuckles to unarmed causes the knuckles to remain 
                            // on display but unusable. So, what we do is switch to a MELEE weapon (creating one if necessary)
                            // then switch back to unarmed from there, which works fine.
                            CWeapon* oldWeapon = pPed->GetWeapon (WEAPONSLOT_TYPE_UNARMED);
                            if ( oldWeapon )
                            {
                                eWeaponType unarmedWeapon = oldWeapon->GetType();
                                pPed->RemoveWeapon ( unarmedWeapon );
                                if ( bGiveWeapon || pPed->GetCurrentWeaponSlot() == WEAPONSLOT_TYPE_UNARMED )
                                {
                                    oldWeapon = NULL;
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
                            else
                            {
                                // Probably the ped is streamed out
                                pPed->GiveWeapon ( WEAPONTYPE_UNARMED, 1 );
                                if ( bGiveWeapon )
                                    pPed->SetCurrentWeaponSlot ( WEAPONSLOT_TYPE_UNARMED );
                            }
                        }
                    }
                }
            }
        }
    }
}


void CWeaponRPCs::TakeWeapon ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    // Read out weapon id and ammo amount
    SWeaponTypeSync weaponType;

    if ( bitStream.Read ( &weaponType )  )
    {
        unsigned char ucWeaponID = weaponType.data.ucWeaponType;

        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            // Is the weapon id valid? (may not be neccessary, just being safe)
            if ( CClientPickupManager::IsValidWeaponID ( ucWeaponID ) )
            {
                // Have we ammo in packet? If so, we need to take the ammo, not weapon
                SWeaponAmmoSync ammo ( ucWeaponID, true, false );
                if ( bitStream.Read ( &ammo ) )
                {
                    unsigned short usAmmo = ammo.data.usTotalAmmo;

                    // Do we have the weapon?
                    CWeapon* pPlayerWeapon = pPed->GetWeapon ( (eWeaponType) ucWeaponID );

                    // Store the ammo so it's not lost if a ped is streamed out
                    if ( pPed->GetType () == CCLIENTPED )
                    {
                        char cSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
                        if ( cSlot != -1 )
                        {
                            if ( usAmmo > pPed->m_usWeaponAmmo [ cSlot ] )
                                pPed->m_usWeaponAmmo [ cSlot ] = 0;
                            else
                                pPed->m_usWeaponAmmo [ cSlot ] -= usAmmo;
                        }
                    }
                    if ( pPlayerWeapon == NULL ) return;

                    unsigned char ucAmmoInClip = static_cast < unsigned char > ( pPlayerWeapon->GetAmmoInClip () );
                    pPlayerWeapon->SetAmmoInClip ( 0 );

                    unsigned long ulWeaponAmmo = pPlayerWeapon->GetAmmoTotal ();
                    if ( usAmmo > ulWeaponAmmo )
                        ulWeaponAmmo = 0;
                    else
                        ulWeaponAmmo -= usAmmo;

                    // Update the ammo
                    pPlayerWeapon->SetAmmoTotal ( ulWeaponAmmo );

                    if ( pPlayerWeapon->GetAmmoTotal () > ucAmmoInClip )
                        pPlayerWeapon->SetAmmoInClip ( ucAmmoInClip );
                    else if ( pPlayerWeapon->GetAmmoTotal () <= ucAmmoInClip )
                        pPlayerWeapon->SetAmmoInClip ( pPlayerWeapon->GetAmmoTotal () );
                }
                else
                {
                    // Don't change remote players weapons (affects sync)
                    if ( pPed->IsLocalPlayer () )
                    {
                        // Remove the weapon
                        pPed->RemoveWeapon ( static_cast < eWeaponType > ( ucWeaponID ) );
                    }
                }
            }
        }
    }
}


void CWeaponRPCs::TakeAllWeapons ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
    if ( pPed )
    {
        // Remove all our weapons
        pPed->RemoveAllWeapons ();
    }
}


void CWeaponRPCs::SetWeaponSlot ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    SWeaponSlotSync slot;

    if ( bitStream.Read ( &slot ) )
    {
        CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
        if ( pPed )
        {
            pPed->SetCurrentWeaponSlot ( (eWeaponSlot) slot.data.uiSlot );
        }
    }
}


void CWeaponRPCs::SetWeaponAmmo ( CClientEntity* pSource, NetBitStreamInterface& bitStream )
{
    SWeaponTypeSync weaponType;
    if ( bitStream.Read ( &weaponType ) )
    {
        unsigned char ucWeaponID = weaponType.data.ucWeaponType;
        SWeaponAmmoSync ammo ( ucWeaponID, true, true );

        if ( bitStream.Read ( &ammo ) )
        {
            unsigned short usAmmo = ammo.data.usTotalAmmo;
            unsigned short usAmmoInClip = ammo.data.usAmmoInClip;

            CClientPed * pPed = m_pPedManager->Get ( pSource->GetID (), true );
            if ( pPed )
            {
                // Valid weapon id?
                if ( !CClientPickupManager::IsValidWeaponID ( ucWeaponID ) ) return;

                // Do we have it?
                CWeapon* pPlayerWeapon = pPed->GetWeapon ( (eWeaponType) ucWeaponID );

                // Store the ammo so it's not lost if a ped is streamed out
                if ( pPed->GetType () == CCLIENTPED )
                {
                    char cSlot = CWeaponNames::GetSlotFromWeapon ( ucWeaponID );
                    if ( cSlot != -1 )
                    {
                        pPed->m_usWeaponAmmo [ cSlot ] = usAmmo;
                    }
                }
                if ( pPlayerWeapon == NULL ) return;

                unsigned char ucAmmoInClip = static_cast < unsigned char > ( pPlayerWeapon->GetAmmoInClip () );
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
}
