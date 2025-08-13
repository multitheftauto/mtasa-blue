/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_ClothesSpeedUp.cpp
 *  PORPOISE:    Reduce stutter when changing clothes for a CJ ped
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "../Client/game_sa/CDirectorySA.h"

DWORD FUNC_CStreamingInfoAddToList = 0x407480;
DWORD FUNC_CStreamingConvertBufferToObject = 0x40C6B0;

//
// Toggle hook on/off.
// Enable caching and faster loading of clothes files
//
void CMultiplayerSA::SetFastClothesLoading(EFastClothesLoading fastClothesLoading)
{
    if (m_FastClothesLoading == fastClothesLoading)
        return;

    m_FastClothesLoading = fastClothesLoading;

    // Handle auto setting
    if (fastClothesLoading == FAST_CLOTHES_AUTO)
    {
        // Disable if less than 512MB installed ram
        long long llSystemRamKB = GetWMITotalPhysicalMemory() / 1024LL;
        if (llSystemRamKB > 0 && llSystemRamKB < 512 * 1024)
            fastClothesLoading = FAST_CLOTHES_OFF;
    }

    if (fastClothesLoading != FAST_CLOTHES_OFF)
    {
        // Load and cache player.img
        SString strGTASAPath = GetCommonRegistryValue("", "GTA:SA Path");
        SString strFilename = PathJoin(strGTASAPath, "models", "player.img");
        FileLoad(strFilename, m_PlayerImgCache);
    }
    else
    {
        // Remove cached data - Note: This method ensures the memory is actually freed
        std::vector<char>().swap(m_PlayerImgCache);
    }

    // Update the cache pointer
    if (!m_PlayerImgCache.empty())
        ms_PlayerImgCachePtr = &m_PlayerImgCache[0];
    else
        ms_PlayerImgCachePtr = NULL;
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
        ushort usNext;
        ushort usPrev;

        ushort uiUnknown1;            // Parent ?
        uchar  uiUnknown2;            // 0x12 when loading, 0x02 when finished loading
        uchar  ucImgId;

        int  iBlockOffset;
        int  iBlockCount;
        uint uiLoadflag;            // 0-not loaded  2-requested  3-loaded  1-processed
    };

    int   iReturnFileId;
    char* pReturnBuffer;
}            // namespace

////////////////////////////////////////////////////////////////////////////////////////////////
//
// If request is for a file inside player.img (imgId 5) and uiLoadflag is 0 or 1
// then force use of our cached player.img data
//
bool _cdecl OnCallCStreamingInfoAddToList(int flags, SImgGTAItemInfo* pImgGTAInfo)
{
    if (!CMultiplayerSA::ms_PlayerImgCachePtr)
        return false;

    if (pImgGTAInfo->ucImgId == 5)
    {
        // If bLoadingBigModel is set, try to get it unset
        #define VAR_CStreaming_bLoadingBigModel     0x08E4A58
        BYTE& bLoadingBigModel = *(BYTE*)VAR_CStreaming_bLoadingBigModel;
        if (bLoadingBigModel)
        {
            pGameInterface->GetStreaming()->LoadAllRequestedModels(true);
            if (bLoadingBigModel)
                pGameInterface->GetStreaming()->LoadAllRequestedModels(false);
            assert(!bLoadingBigModel);
        }

        int iFileId = ((char*)pImgGTAInfo - (char*)CStreaming__ms_aInfoForModel) / 20;

        iReturnFileId = iFileId;
        pReturnBuffer = CMultiplayerSA::ms_PlayerImgCachePtr + pImgGTAInfo->iBlockOffset * 2048;

        // Update flags
        pImgGTAInfo->uiLoadflag = 3;

        // Remove priorty flag, as not counted in ms_numPriorityRequests
        pImgGTAInfo->uiUnknown2 &= ~0x10;

        return true;
    }

    return false;
}

// Hook info
#define HOOKPOS_CallCStreamingInfoAddToList             0x408962
#define HOOKSIZE_CallCStreamingInfoAddToList            5
DWORD RETURN_CallCStreamingInfoAddToListA = 0x408967;
DWORD RETURN_CallCStreamingInfoAddToListB = 0x408990;
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
bool _cdecl ShouldSkipLoadRequestedModels(DWORD calledFrom)
{
    if (!CMultiplayerSA::ms_PlayerImgCachePtr)
        return false;

    // Skip LoadRequestedModels if called from:
    //      CClothesBuilder::ConstructGeometryArray      5A55A0 - 5A56B6
    //      CClothesBuilder::LoadAndPutOnClothes         5A5F70 - 5A6039
    //      CClothesBuilder::ConstructTextures           5A6040 - 5A6520
    if (calledFrom > 0x5A55A0 && calledFrom < 0x5A6520)
        return true;

    return false;
}

// Hook info
#define HOOKPOS_CStreamingLoadRequestedModels        0x15670A0
#define HOOKSIZE_CStreamingLoadRequestedModels       5
DWORD RETURN_CStreamingLoadRequestedModels = 0x15670A5;
DWORD RETURN_CStreamingLoadRequestedModelsB = 0x156711B;
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
        jmp     RETURN_CStreamingLoadRequestedModels

        // Skip LoadRequestedModels
skip:
        popad
        jmp     RETURN_CStreamingLoadRequestedModelsB
    }
}

//
// Skip loading the directory data from player.img if it has already been loaded.
// Speeds up clothes a bit, but is only part of a solution - The actual files from inside player.img are still loaded each time
//

static std::uint32_t g_playerImgEntries = 0xBBCDC8;
static std::uint16_t g_playerImgSize = 0x226;

bool IsPlayerImgDirLoaded()
{
    // When player.img dir is loaded, it looks this this:
    // 0x00BC12C0  00bbcdc8 00000226
    DWORD* ptr1 = (DWORD*)0xBC12C0;

    return ptr1[0] == g_playerImgEntries && ptr1[1] == g_playerImgSize;
}

// Hook info
#define HOOKSIZE_LoadingPlayerImgDir 5
#define HOOKPOS_LoadingPlayerImgDir  0x5A69E3                               // 005A69D6 -> CClothesBuilder::CreateSkinnedClump -> playerImgEntries
static constexpr std::uintptr_t RETURN_LoadingPlayerImgDirA = 0x5A69E8;     // push 00000226 { 550 }
static constexpr std::uintptr_t RETURN_LoadingPlayerImgDirB = 0x5A6A06;     // return of CreateSkinnedClump function

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
        mov     eax, g_playerImgEntries
        push    eax
        jmp     RETURN_LoadingPlayerImgDirA

         // Skip loading img directory
skip:
        popad
        jmp     RETURN_LoadingPlayerImgDirB
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup clothing directory size
//
//////////////////////////////////////////////////////////////////////////////////////////
bool SetClothingDirectorySize(int directorySize)
{
    DirectoryInfoSA* clothesDirectory = new DirectoryInfoSA[directorySize];

    if (!clothesDirectory)
        return false;

    // CClothesBuilder::LoadCdDirectory(void)
    MemPut<std::uint32_t>(0x5A4190 + 1, reinterpret_cast<uint32_t>(clothesDirectory));      // push    offset _playerImgEntries; headers
    MemPut<std::uint16_t>(0x5A4195 + 1, directorySize);                                     // push    550             ; count
    MemPut<std::uint16_t>(0x5A69E8 + 1, directorySize);                                     // push    550             ; count

    g_playerImgEntries = reinterpret_cast<uint32_t>(clothesDirectory);
    g_playerImgSize = directorySize;

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for ClothesSpeedUp
//
//////////////////////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_ClothesSpeedUp()
{
    SetClothingDirectorySize(2050);

    EZHookInstall(CStreamingLoadRequestedModels);
    EZHookInstall(LoadingPlayerImgDir);
    EZHookInstall(CallCStreamingInfoAddToList);
}
