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

DWORD FUNC_CStreamingInfoAddToList              = 0x407480;
DWORD FUNC_CStreamingConvertBufferToObject      = 0x40C6B0;

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
        // Load and cache player.img
        SString strGTASAPath = GetCommonRegistryValue ( "", "GTA:SA Path" );
        SString strFilename = PathJoin ( strGTASAPath, "models", "player.img" );
        FileLoad ( strFilename, m_PlayerImgCache );
    }
    else
    {
        // Remove cached data - Note: This method ensures the memory is actually freed
        std::vector < char > ().swap ( m_PlayerImgCache );
    }

    // Update the cache pointer
    if ( !m_PlayerImgCache.empty () )
        ms_PlayerImgCachePtr = &m_PlayerImgCache[0];
    else
        ms_PlayerImgCachePtr = NULL;
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


////////////////////////////////////////////////
//
// Hook CStreaming::RequestFile
//
//
//
////////////////////////////////////////////////
namespace
{
    struct SImgGTAItemInfo
    {
        ushort    usNext;
        ushort    usPrev;

        ushort  uiUnknown1;         // Parent ?
        uchar   uiUnknown2;         // 0x12 when loading, 0x02 when finished loading
        uchar   ucImgId;

        int     iBlockOffset;
        int     iBlockCount;
        uint    uiLoadflag;         // 0-not loaded  2-requested  3-loaded  1-processed
    };

    int     iReturnFileId;
    char*   pReturnBuffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// If request is for a file inside player.img (imgId 5) and uiLoadflag is 0 or 1
// then force use of our cached player.img data
//
bool _cdecl OnCallCStreamingInfoAddToList ( int flags, SImgGTAItemInfo* pImgGTAInfo )
{
    if ( !CMultiplayerSA::ms_PlayerImgCachePtr )
        return false;

    if ( pImgGTAInfo->ucImgId == 5 )
    {
        // If bLoadingBigModel is set, try to get it unset
        #define VAR_CStreaming_bLoadingBigModel     0x08E4A58
        BYTE& bLoadingBigModel = *(BYTE*)VAR_CStreaming_bLoadingBigModel;
        if ( bLoadingBigModel )
        {
            pGameInterface->GetStreaming()->LoadAllRequestedModels ( true );
            if ( bLoadingBigModel )
                pGameInterface->GetStreaming()->LoadAllRequestedModels ( false );
            assert ( !bLoadingBigModel );
        }

        int iFileId = ((int)pImgGTAInfo - 0x08E4CC0) / 20;

        iReturnFileId = iFileId;
        pReturnBuffer = CMultiplayerSA::ms_PlayerImgCachePtr + pImgGTAInfo->iBlockOffset * 2048;

        // Update flags
        pImgGTAInfo->uiLoadflag = 3;

        // Remove priorty flag, as not counted in ms_numPriorityRequests
        pImgGTAInfo->uiUnknown2 &= ~ 0x10;

        return true;
    }

    return false;
}

// Hook info
#define HOOKPOS_CallCStreamingInfoAddToList             0x408962
#define HOOKSIZE_CallCStreamingInfoAddToList            5
DWORD RETURN_CallCStreamingInfoAddToListA               = 0x408967;
DWORD RETURN_CallCStreamingInfoAddToListB               = 0x408990;
void _declspec(naked) HOOK_CallCStreamingInfoAddToList()
{
    _asm
    {
        pushad
        push    ecx
        push    ebx
        call    OnCallCStreamingInfoAddToList
        add     esp, 4*2
        cmp     al, 0
        jnz     skip

        // Continue with standard code
        popad
        call    FUNC_CStreamingInfoAddToList
        jmp     RETURN_CallCStreamingInfoAddToListA


        // Handle load here
skip:
        popad
        pushad

        mov     eax, 0
        push    eax
        mov     eax, iReturnFileId
        push    eax
        mov     eax, pReturnBuffer
        push    eax
        call    FUNC_CStreamingConvertBufferToObject
        add     esp, 4*3

        popad
        add     esp, 4*1
        jmp     RETURN_CallCStreamingInfoAddToListB
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Return true to skip
bool _cdecl ShouldSkipLoadRequestedModels ( DWORD calledFrom )
{
    if ( !CMultiplayerSA::ms_PlayerImgCachePtr )
        return false;

    // Skip LoadRequestedModels if called from:
    //      CClothesBuilder::ConstructGeometryArray      5A55A0 - 5A56B6
    //      CClothesBuilder::LoadAndPutOnClothes         5A5F70 - 5A6039
    //      CClothesBuilder::ConstructTextures           5A6040 - 5A6520
    if ( calledFrom > 0x5A55A0 && calledFrom < 0x5A6520 )
        return true;

    return false;
}

// Hook info
#define HOOKPOS_CStreamingLoadRequestedModels_US        0x15670A0
#define HOOKPOS_CStreamingLoadRequestedModels_EU        0x1567090
#define HOOKSIZE_CStreamingLoadRequestedModels_US       5
#define HOOKSIZE_CStreamingLoadRequestedModels_EU       5
DWORD RETURN_CStreamingLoadRequestedModels_US =         0x15670A5;
DWORD RETURN_CStreamingLoadRequestedModels_EU =         0x1567095;
DWORD RETURN_CStreamingLoadRequestedModels_BOTH =       0;
DWORD RETURN_CStreamingLoadRequestedModelsB_US =        0x156711B;
DWORD RETURN_CStreamingLoadRequestedModelsB_EU =        0x156710B;
DWORD RETURN_CStreamingLoadRequestedModelsB_BOTH =      0;
void _declspec(naked) HOOK_CStreamingLoadRequestedModels()
{
    _asm
    {
        pushad
        push    [esp+32+4*0]
        call    ShouldSkipLoadRequestedModels
        add     esp, 4*1
        cmp     al, 0
        jnz     skip

        // Continue with standard code
        popad
        mov     al,byte ptr ds:[008E4A58h]
        jmp     RETURN_CStreamingLoadRequestedModels_BOTH

        // Skip LoadRequestedModels
skip:
        popad
        jmp     RETURN_CStreamingLoadRequestedModelsB_BOTH
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for ClothesSpeedUp
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_ClothesSpeedUp ( void )
{
    EZHookInstall ( CStreamingLoadRequestedModels );
    EZHookInstall ( LoadingPlayerImgDir );
    EZHookInstall ( CallCStreamingInfoAddToList );
}
