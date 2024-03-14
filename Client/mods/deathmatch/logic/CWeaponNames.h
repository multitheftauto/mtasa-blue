/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *               (Shared logic for modifications)
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/CWeaponNames.h
 *  PURPOSE:     Weapon names class header
 *
 *****************************************************************************/

#pragma once

class CWeaponNames
{
public:
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
