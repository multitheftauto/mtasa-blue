/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponNames.h
*  PURPOSE:     GTA weapon name definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CWEAPONNAMES_H
#define __CWEAPONNAMES_H

#define NUMBER_OF_WEAPON_NAMES 60

class CWeaponNames
{
public:
    static bool             IsWeaponIDAkimbo    ( unsigned char ucID );
    static bool             IsDriveByWeapon     ( unsigned char ucID );

    static unsigned char    GetWeaponID         ( const char* szName );
    static const char*      GetWeaponName       ( unsigned char ucID );

    static char             GetSlotFromWeapon   ( unsigned char ucID );

    static bool             DoesSlotHaveAmmo    ( unsigned int uiSlot )
    {
        switch ( uiSlot ) {
            case 0: case 1: case 10: case 11: case 12: return false;
            default: return true;
        }
    }
};

#endif
