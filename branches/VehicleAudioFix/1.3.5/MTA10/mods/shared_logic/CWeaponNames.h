/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CWeaponNames.h
*  PURPOSE:     Weapon names class header
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#ifndef __CWEAPONNAMES_H
#define __CWEAPONNAMES_H

class CWeaponNames
{
public:
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
