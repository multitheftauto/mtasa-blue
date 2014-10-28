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
#include "../game_sa/CWeaponInfoSA.h"
extern EDamageReasonType g_GenerateDamageEventReason;
static CElapsedTime ms_LastFxTimer;

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
// CShotInfo_Update
//
// Reset shotinfo array when game is not running
//
//////////////////////////////////////////////////////////////////////////////////////////

// Hook info
#define HOOKPOS_CShotInfo_Update                         0x739E60
#define HOOKSIZE_CShotInfo_Update                        6
DWORD RETURN_CShotInfo_Update =                          0x739E66;

// Clear all shotinfos
void ResetShotInfoArray( void )
{
    CFlameShotInfo* pInfo = (CFlameShotInfo*)ARRAY_CFlameShotInfo;
    memset ( pInfo, 0, sizeof ( CFlameShotInfo ) );
    pInfo->weaponType = WEAPONTYPE_PISTOL;
    pInfo->fRadius = 1;
    for ( uint i = 1 ; i < MAX_FLAME_SHOT_INFOS ; i++ )
        memcpy ( pInfo + i, pInfo, sizeof ( CFlameShotInfo ) );
}

#pragma warning( push )
#pragma warning( disable : 4731 )   // warning C4731: 'Call_CShotInfo_Update' : frame pointer register 'ebp' modified by inline assembly code

void Call_CShotInfo_Update( void )
{
    _asm
    {
        call inner
        jmp  done
    inner:
        push    ebp  
        mov     ebp, esp
        and     esp, 0FFFFFFF8h
        jmp     RETURN_CShotInfo_Update
    done:
    }
}

#pragma warning( pop )

// Our code for when CShotInfo_Update is called
void OnMY_CShotInfo_Update( void )
{
    if ( !pMultiplayer->IsConnected () )
    {
        // Reset shotinfo array when game is not running
        ResetShotInfoArray ();
    }

    __try
    {
        // Call original CShotInfo::Update with hacky protection against bad pointers
        Call_CShotInfo_Update ();
    }
    __except( GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION )
    {
        // Reset shotinfo array when it has problems
        ResetShotInfoArray ();
    }
}

// The hook goes here
void _declspec(naked) HOOK_CShotInfo_Update()
{
    _asm
    {
        pushad
        call    OnMY_CShotInfo_Update
        popad
        retn
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Fx_AddBulletImpact
//
// Modify bullet impact effect type
//
// 1 = sparks
// 2 = sand
// 3 = wood
// 4 = dust
//
//////////////////////////////////////////////////////////////////////////////////////////
int OnMY_Fx_AddBulletImpact( int iType )
{
    // Limit sand or dust effect due to performance issues
    if ( iType == 2 || iType == 4 )
    {
        if ( ms_LastFxTimer.Get() > 500 )
            ms_LastFxTimer.Reset();     // Allow once every 500ms
        else
            iType = 1;                  // Otherwise replace with spark
    }
    return iType;
}

// Hook info
#define HOOKPOS_Fx_AddBulletImpact                         0x049F3E8
#define HOOKSIZE_Fx_AddBulletImpact                        5
DWORD RETURN_Fx_AddBulletImpact =                          0x049F3ED;

void _declspec(naked) HOOK_Fx_AddBulletImpact( void )
{
    _asm
    {
        pushad
        push    eax
        call    OnMY_Fx_AddBulletImpact
        mov     [esp+0], eax         // Put result temp
        add     esp, 4*1
        popad

        mov     esi, [esp-32-4*1]    // Get result temp
        mov     eax, ds:0x0B6F03C
        jmp     RETURN_Fx_AddBulletImpact
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
    EZHookInstall ( CShotInfo_Update );
    EZHookInstall ( Fx_AddBulletImpact );
}
