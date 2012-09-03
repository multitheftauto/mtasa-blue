/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_Weapons.cpp
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
extern EDamageReasonType g_GenerateDamageEventReason;


//////////////////////////////////////////////////////////////////////////////////////////
//
// CWeapon::GenerateDamageEvent
//
// Try to detect pistol whippings
//
//////////////////////////////////////////////////////////////////////////////////////////
void OnMY_CWeapon_GenerateDamageEvent( DWORD calledFrom, CPedSAInterface* pPed, CEntitySAInterface* pEntity, eWeaponType weaponType, uint uiFlags1, ePedPieceTypes pedPieceType, uint uiFlags2 )
{
// uiFlags1 appears to be:
//          4 - punch
//          7 - punch 2
//         12 - punch 3
//         20 - ground kick
//          8 - pistol whip
//        140 - deagle fire
//         40 - silenced pistol fire
//         25 - pistol fire
//         25 - mp5 fire
//         20 - tec fire
//         20 - uzi fire
//         30 - ak47 fire
//         30 - m4 fire
//         75 - sniper fire
//         14 - shovel hit
//         10 - rocket launcher explode
//         10 - dead
//          and lots more probably

    if ( uiFlags1 == 8 )
        g_GenerateDamageEventReason = EDamageReason::PISTOL_WHIP;
    else
        g_GenerateDamageEventReason = EDamageReason::OTHER;
}


// Hook info
#define HOOKPOS_CWeapon_GenerateDamageEvent                         0x73A530
#define HOOKSIZE_CWeapon_GenerateDamageEvent                        7
DWORD RETURN_CWeapon_GenerateDamageEvent =                          0x73A537;
void _declspec(naked) HOOK_CWeapon_GenerateDamageEvent()
{
    _asm
    {
        pushad
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        push    [esp+32+4*6]
        call    OnMY_CWeapon_GenerateDamageEvent
        add     esp, 4*6+4
        popad

        push    0FFFFFFFFh 
        push    848E10h 
        jmp     RETURN_CWeapon_GenerateDamageEvent
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// CMultiplayerSA::InitHooks_Weapons
//
// Setup hooks
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_Weapons ( void )
{
    EZHookInstall ( CWeapon_GenerateDamageEvent );
}
