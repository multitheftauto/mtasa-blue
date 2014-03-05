/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        multiplayer_sa/CMultiplayerSA_ClothesSpeedUp.cpp
*  PORPOISE:    Reduce stutter when changing clothes for a CJ ped
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

// TODO - reimplement this once the new streaming system is added.

//
// Toggle hook on/off.
// Enable caching and faster loading of clothes files
//
void CMultiplayerSA::SetFastClothesLoading ( EFastClothesLoading fastClothesLoading )
{
    if ( m_FastClothesLoading == fastClothesLoading )
        return;

    m_FastClothesLoading = fastClothesLoading;

    // Handle auto setting
    if ( fastClothesLoading == FAST_CLOTHES_AUTO )
    {
        // Disable if less than 512MB installed ram
        long long llSystemRamKB = GetWMITotalPhysicalMemory () / 1024LL;
        if ( llSystemRamKB > 0 && llSystemRamKB < 512 * 1024 )
           fastClothesLoading = FAST_CLOTHES_OFF;
    }

    if ( fastClothesLoading != FAST_CLOTHES_OFF )
    {
        pGameInterface->GetStreaming()->CacheIMGFile( "player.img" );
    }
    else
    {
        // Remove cached data - Note: This method ensures the memory is actually freed
        pGameInterface->GetStreaming()->FreeIMGFileCache( "player.img" );
    }
}


//
// Skip loading the directory data from player.img if it has already been loaded.
// Speeds up clothes a bit, but is only part of a solution - The actual files from inside player.img are still loaded each time
//
bool _cdecl IsPlayerImgDirLoaded ( void )
{
    // When player.img dir is loaded, it looks this this:
    // 0x00BC12C0  00bbcdc8 00000226
    DWORD* ptr1 = (DWORD*)0x00BC12C0;
    if ( ptr1[0] == 0x00BBCDC8 && ptr1[1] == 0x0000226 )
    {
        return true;
    }
    return false;
}

// Hook info
#define HOOKPOS_LoadingPlayerImgDir                     0x5A69E3
#define HOOKSIZE_LoadingPlayerImgDir                    5
DWORD RETURN_LoadingPlayerImgDirA =                     0x5A69E8;
DWORD RETURN_LoadingPlayerImgDirB =                     0x5A6A06;
void _declspec(naked) HOOK_LoadingPlayerImgDir()
{
// hook from 005A69E3 5 bytes
    _asm
    {
        pushad
        call    IsPlayerImgDirLoaded
        cmp     al, 0
        jnz     skip
        popad

        // Standard code to load img directory
        push    0BBCDC8h 
        jmp     RETURN_LoadingPlayerImgDirA

        // Skip loading img directory
skip:
        popad
        jmp     RETURN_LoadingPlayerImgDirB
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for ClothesSpeedUp
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_ClothesSpeedUp ( void )
{
    EZHookInstall ( LoadingPlayerImgDir );
}
