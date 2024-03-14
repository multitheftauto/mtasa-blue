/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/CWeaponNames.h
 *  PURPOSE:     GTA weapon name definitions class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#define NUMBER_OF_WEAPON_NAMES 60

class CWeaponNames
{
public:
    static bool IsWeaponIDAkimbo(std::uint8_t ucID);
    static bool IsDriveByWeapon(std::uint8_t ucID);

    static std::uint8_t GetWeaponID(const char* szName);
    static const char*   GetWeaponName(std::uint8_t ucID);
    static char          GetSlotFromWeapon(std::uint8_t ucID);

    static bool DoesSlotHaveAmmo(std::uint32_t uiSlot)
    {
        switch (uiSlot)
        {
            case 0:
            case 1:
            case 10:
            case 11:
            case 12:
                return false;
            default:
                return true;
        }
    }
};
