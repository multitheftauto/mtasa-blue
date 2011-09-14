/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        mods/deathmatch/logic/CWeaponNames.cpp
*  PURPOSE:     GTA weapon name definitions class
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Jax <>
*               Derek Abdine <>
*               Kevin Whiteside <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

char szWeaponNameEmpty [] = "";

struct SWeaponName
{
    char szName [32];
};

struct SWeaponSlot
{
    char cSlot;
};

SWeaponName WeaponNames [] =
{ {"Fist"}, {"Brassknuckle"}, {"Golfclub"}, {"Nightstick"}, {"Knife"},
{"Bat"}, {"Shovel"}, {"Poolstick"}, {"Katana"}, {"Chainsaw"}, 
{"Dildo"}, {"Dildo"}, {"Vibrator"}, {"Vibrator"}, {"Flower"},
{"Cane"}, {"Grenade"}, {"Teargas"}, {"Molotov"}, {"Rocket"},
{"Rocket"}, {"Freefall Bomb"}, {"Colt 45"}, {"Silenced"}, {"Deagle"},
{"Shotgun"}, {"Sawed-off"}, {"Combat Shotgun"}, {"Uzi"}, {"MP5"},
{"AK-47"}, {"M4"}, {"Tec-9"}, {"Rifle"}, {"Sniper"},
{"Rocket Launcher"}, {"Rocket Launcher HS"}, {"Flamethrower"}, {"Minigun"}, {"Satchel"},
{"Bomb"}, {"Spraycan"}, {"Fire Extinguisher"}, {"Camera"}, {"Nightvision"}, {"Infrared"},
{"Parachute"}, {"Last Weapon"}, {"Armour"}, {"Rammed"}, {"Ranover"}, {"Explosion"}, {"Driveby"}, 
{"Drowned"}, {"Fall"}, {"Unknown"}, {"Melee"}, {"Weapon"}, {"Flare"}, { "Tank Grenade" } };

SWeaponSlot WeaponIDs [] =
{{0},{0},{1},{1},{1},{1},{1},{1},{1},{1},{10},{10},{10},{-1},{10},{10},{8},{8},
{8},{-1},{-1},{-1},{2},{2},{2},{3},{3},{3},{4},{4},{5},{5},{4},{6},{6},{7},
{7},{7},{7},{8},{12},{9},{9},{9},{11},{11},{11},{-1}};


bool CWeaponNames::IsWeaponIDAkimbo ( unsigned char ucID )
{
    return ( ucID == 22 ||
             ucID == 26 ||
             ucID == 28 ||
             ucID == 32 );
}


bool CWeaponNames::IsDriveByWeapon ( unsigned char ucID )
{
    return ( ( ucID >= 22 && ucID <= 33 ) || ucID == 38 );
}


unsigned char CWeaponNames::GetWeaponID ( const char* szName )
{
    // If the specified string was empty, return 0
    if ( szName [0] == 0 ) return 0xFF;

    // Look for it in our table
    for ( unsigned int i = 0; i < NUMELMS ( WeaponNames ); i++ )
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
    if ( ucID < NUMELMS ( WeaponNames ) )
    {
        return WeaponNames [ucID].szName;
    }

    return szWeaponNameEmpty;
}


float CWeaponNames::GetWeaponRange ( unsigned char ucWeaponType )
{
    static const float s_fWeaponRanges [ 60 ] = {
        1.6f,  1.6f,  1.6f,  1.6f,   1.6f,   1.6f,   1.6f,  1.6f,  1.6f,  1.6f,
        1.6f,  1.6f,  1.6f,  0.0f,   1.6f,   1.6f,   40.0f, 40.0f, 40.0f, 0.0f,
        0.0f,  0.0f,  35.0f, 35.0f,  35.0f,  40.0f,  35.0f, 40.0f, 35.0f, 45.0f,
        70.0f, 90.0f, 35.0f, 100.0f, 300.0f, 55.0f,  55.0f, 5.1f,  75.0f, 40.0f,
        25.0f, 6.1f,  10.1f, 100.0f, 100.0f, 100.0f, 1.6f,  0.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  0.0f,   0.0f,   0.0f,   0.0f,  0.0f,  0.0f,  0.0f
    };

    if ( ucWeaponType < 60 )
        return s_fWeaponRanges [ ucWeaponType ];

    return 0.0f;
}


char CWeaponNames::GetSlotFromWeapon ( unsigned char ucID )
{
    if ( ucID < NUMELMS ( WeaponIDs ) )
    {
        return WeaponIDs [ucID].cSlot;
    }

    return -1;
}
