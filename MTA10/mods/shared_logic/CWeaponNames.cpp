/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*               (Shared logic for modifications)
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/shared_logic/CWeaponNames.cpp
*  PURPOSE:     Weapon names class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*
*****************************************************************************/

#include <StdInc.h>

using namespace std;

char szWeaponNameEmpty [] = "";

struct SWeaponName
{
    char szName [32];
};

SWeaponName WeaponNames [44] =
{ {"Melee"}, {"Brassknuckle"}, {"Golfclub"}, {"Nitestick"}, {"Knife"},
{"Bat"}, {"Shovel"}, {"Poolstick"}, {"Katana"}, {"Chainsaw"}, 
{"Dildo"}, {"Dildo"}, {"Vibrator"}, {"Vibrator"}, {"Flower"},
{"Cane"}, {"Grenade"}, {"Teargas"}, {"Molotov"}, {""},
{""}, {""}, {"Colt 45"}, {"Silenced"}, {"Deagle"},
{"Shotgun"}, {"Sawed-off"}, {"Combat Shotgun"}, {"Uzi"}, {"MP5"},
{"AK-47"}, {"M4"}, {"Tec-9"}, {"Rifle"}, {"Sniper"},
{"Rocket Launcher"}, {"Rocket Launcher HS"}, {"Flamethrower"}, {"Minigun"}, {"Satchel"},
{"Bomb"}, {"Spraycan"}, {"Fire Extinguisher"}, {"Camera"} };

unsigned char CWeaponNames::GetWeaponID ( const char* szName )
{
    // If the specified string was empty, return 0
    if ( szName [0] == 0 ) return 0xFF;

    // Look for it in our table
    for ( unsigned int i = 0; i <= 43; i++ )
    {
        if ( stricmp ( szName, WeaponNames [i].szName ) == 0 )
        {
            return i;
        }
    }

    return 0xFF;
}


const char* CWeaponNames::GetWeaponName ( unsigned char ucID )
{
    if ( ucID <= 43 )
    {
        return WeaponNames [ucID].szName;
    }

    return szWeaponNameEmpty;
}
