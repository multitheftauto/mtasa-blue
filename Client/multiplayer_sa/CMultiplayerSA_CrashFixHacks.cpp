/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_CrashFixHacks.cpp
 *  PORPOISE:    Home for the poke'n'hope crash fixes
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <game/CAnimManager.h>
#include "../game_sa/CTrainSA.h"
#include "../game_sa/CAnimBlendSequenceSA.h"
#include "../game_sa/CAnimBlendHierarchySA.h"
#include "../game_sa/TaskBasicSA.h"
#include "../game_sa/CFxSystemBPSA.h"
#include "../game_sa/CFxSystemSA.h"

extern CCoreInterface* g_pCore;

//
// Support for crash stats
//
void OnCrashAverted(uint uiId);
void OnEnterCrashZone(uint uiId);

void OnRequestDeferredStreamingMemoryRelief();

static void __declspec(naked) CrashAverted()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
        {
        pushfd
        pushad
        push    [esp+4+32+4*1]
        call    OnCrashAverted
        add     esp, 4
        popad
        popfd
        retn    4
        }
    // clang-format on
}

static bool HasReadAccess(DWORD dwProtect) noexcept
{
    if (dwProtect & PAGE_GUARD)
        return false;

    dwProtect &= 0xFF;

    if (dwProtect == PAGE_NOACCESS)
        return false;

    return dwProtect == PAGE_READONLY || dwProtect == PAGE_READWRITE || dwProtect == PAGE_WRITECOPY || dwProtect == PAGE_EXECUTE ||
           dwProtect == PAGE_EXECUTE_READ || dwProtect == PAGE_EXECUTE_READWRITE || dwProtect == PAGE_EXECUTE_WRITECOPY;
}

static bool HasWriteAccess(DWORD dwProtect) noexcept
{
    if (dwProtect & PAGE_GUARD)
        return false;

    dwProtect &= 0xFF;

    return dwProtect == PAGE_READWRITE || dwProtect == PAGE_WRITECOPY || dwProtect == PAGE_EXECUTE_READWRITE || dwProtect == PAGE_EXECUTE_WRITECOPY;
}

static constexpr std::size_t    REGION_CACHE_SIZE = 8;
static constexpr std::uintptr_t NUM_LOWMEM_THRESHOLD = 0x10000;

#define NUM_LOWMEM_THRESHOLD_ASM 0x10000

struct CachedRegion
{
    std::uintptr_t start{};
    std::uintptr_t end{};
    DWORD          state{};
    DWORD          protect{};
};

static bool QueryRegionCached(std::uintptr_t address, DWORD& outState, DWORD& outProtect, std::uintptr_t& outEnd) noexcept
{
    static thread_local CachedRegion s_cache[REGION_CACHE_SIZE]{};
    static thread_local std::size_t  s_nextSlot{};

    for (const auto& entry : s_cache)
    {
        if (entry.start != 0 && address >= entry.start && address <= entry.end)
        {
            outState = entry.state;
            outProtect = entry.protect;
            outEnd = entry.end;
            return true;
        }
    }

    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(reinterpret_cast<LPCVOID>(address), &mbi, sizeof(mbi)) != sizeof(mbi))
        return false;

    const auto regionSize = static_cast<std::uintptr_t>(mbi.RegionSize);
    if (regionSize == 0)
        return false;

    auto& slot = s_cache[s_nextSlot];
    s_nextSlot = (s_nextSlot + 1) % REGION_CACHE_SIZE;

    slot.start = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress);
    slot.state = mbi.State;
    slot.protect = mbi.Protect;

    const auto endPlusOne = slot.start + regionSize;
    slot.end = (endPlusOne <= slot.start) ? static_cast<std::uintptr_t>(-1) : (endPlusOne - 1);

    outState = slot.state;
    outProtect = slot.protect;
    outEnd = slot.end;
    return true;
}

static bool IsReadablePtr(const void* ptr, std::size_t size) noexcept
{
    if (ptr == nullptr || size == 0)
        return false;

    const auto start = reinterpret_cast<std::uintptr_t>(ptr);
    if (start < NUM_LOWMEM_THRESHOLD)
        return false;

    const auto end = start + size - 1;
    if (end < start)
        return false;

    auto cur = start;
    for (;;)
    {
        DWORD          state{}, protect{};
        std::uintptr_t regionEnd{};
        if (!QueryRegionCached(cur, state, protect, regionEnd))
            return false;

        if (state != MEM_COMMIT || !HasReadAccess(protect))
            return false;

        if (regionEnd >= end)
            return true;

        if (regionEnd < cur || regionEnd == static_cast<std::uintptr_t>(-1))
            return false;

        cur = regionEnd + 1;
    }
}

static bool IsWritablePtr(void* ptr, std::size_t size) noexcept
{
    if (ptr == nullptr || size == 0)
        return false;

    const auto start = reinterpret_cast<std::uintptr_t>(ptr);
    if (start < NUM_LOWMEM_THRESHOLD)
        return false;

    const auto end = start + size - 1;
    if (end < start)
        return false;

    auto cur = start;
    for (;;)
    {
        DWORD          state{}, protect{};
        std::uintptr_t regionEnd{};
        if (!QueryRegionCached(cur, state, protect, regionEnd))
            return false;

        if (state != MEM_COMMIT || !HasWriteAccess(protect))
            return false;

        if (regionEnd >= end)
            return true;

        if (regionEnd < cur || regionEnd == static_cast<std::uintptr_t>(-1))
            return false;

        cur = regionEnd + 1;
    }
}

////////////////////////////////////////////////////////////////////////
// CCustomCarEnvMapPipeline::CustomPipeRenderCB
//
// Null mesh material pointer
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc1  0x5D9A6E
#define HOOKSIZE_CrashFix_Misc1 6
DWORD                         RETURN_CrashFix_Misc1 = 0x5D9A74;
static void __declspec(naked) HOOK_CrashFix_Misc1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax,dword ptr [esp+18h]
        test    eax,eax
        je      cont

        mov     eax,dword ptr ds:[008D12CCh]
        mov     ecx,dword ptr [eax+esi]            // If [eax+esi] (mesh->material) is 0, it causes a crash
        test    ecx,ecx
        jne     cont
        push    1
        call    CrashAverted
        xor    ecx,ecx
    cont:
        jmp     RETURN_CrashFix_Misc1
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAutomobile::ProcessControl
//
// Null CColModel pointer or corrupted m_pColData
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc2  0x6B18B0
#define HOOKSIZE_CrashFix_Misc2 9
DWORD                         RETURN_CrashFix_Misc2 = 0x6B18B9;
DWORD                         RETURN_CrashFix_Misc2B = 0x6B3775;
static void __declspec(naked) HOOK_CrashFix_Misc2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    eax,eax
        je      cont            // Skip much code if eax is zero (vehicle has no colmodel)

        mov     eax,dword ptr [eax+2Ch]

        test    eax,eax
        je      cont            // Skip much code if eax is zero (colmodel has no coldata)

        mov     ebx,dword ptr [eax+10h]

        test    ebx,ebx
        je      cont            // Skip much code if ebx is zero (coldata has no suspension lines)

        mov     cl,byte ptr [esi+429h]
        jmp     RETURN_CrashFix_Misc2
    cont:
        push    2
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc2B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAESoundManager::Service
//
// Division by 0
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc4  0x4F02D2
#define HOOKSIZE_CrashFix_Misc4 5
DWORD                         RETURN_CrashFix_Misc4 = 0x4F02D7;
DWORD                         RETURN_CrashFix_Misc4B = 0x4F0B07;
static void __declspec(naked) HOOK_CrashFix_Misc4()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    ecx,ecx
        je      cont            // Skip much code if ecx is zero (avoid divide by zero in soundmanager::service)

        cdq
        idiv    ecx
        add     edx, ebp
        jmp     RETURN_CrashFix_Misc4
    cont:
        push    4
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc4B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CPed::SetPedPositionInCar
//
// Null pointer m_pVehicleStruct in the CVehicleModelInfo structure
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc5  0x5DF949
#define HOOKSIZE_CrashFix_Misc5 7
DWORD                         RETURN_CrashFix_Misc5 = 0x5DF950;
DWORD                         RETURN_CrashFix_Misc5B = 0x5DFCC4;
static void __declspec(naked) HOOK_CrashFix_Misc5()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm {
        mov edi, dword ptr[ARRAY_ModelInfo]
        mov     edi, dword ptr [ecx*4+edi]
        mov     edi, dword ptr [edi+5Ch]
        test    edi, edi
        je      cont  // Skip much code if edi is zero

        mov edi, dword ptr[ARRAY_ModelInfo]
        mov     edi, dword ptr [ecx*4+edi]
        jmp     RETURN_CrashFix_Misc5
    cont:
        push    5
        call    CrashAverted
        pop edi
        jmp     RETURN_CrashFix_Misc5B
    }
}

////////////////////////////////////////////////////////////////////////
// RpAnimBlend::FrameUpdateCallBackSkinnedWithVelocityExtraction
//
// Mantis #5465 (2/2) (https://web.archive.org/web/20160411120202/https://bugs.mtasa.com/view.php?id=5465)
//
// Null pointer to object in the BlendNodeArrays array of the AnimBlendUpdateData structure
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc6  0x4D1750
#define HOOKSIZE_CrashFix_Misc6 5
DWORD                         RETURN_CrashFix_Misc6 = 0x4D1755;
DWORD                         RETURN_CrashFix_Misc6B = 0x4D1A44;
static void __declspec(naked) HOOK_CrashFix_Misc6()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    ecx, ecx
        je      cont            // Skip much code if ecx is zero (ped has no anim something)

        mov     eax, dword ptr [ecx+10h]
        test    eax, eax
        jmp     RETURN_CrashFix_Misc6
    cont:
        push    6
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc6B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CCollision::ProcessVerticalLine
//
// Mantis #5466 (https://web.archive.org/web/20160401233418/https://bugs.mtasa.com/view.php?id=5466)
//
// Null colModel pointer
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc7  0x417BF8
#define HOOKSIZE_CrashFix_Misc7 5
DWORD                         RETURN_CrashFix_Misc7 = 0x417BFD;
DWORD                         RETURN_CrashFix_Misc7B = 0x417BFF;
static void __declspec(naked) HOOK_CrashFix_Misc7()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    ecx, ecx
        je      cont            // Skip much code if ecx is zero (no colmodel)

        mov     esi, dword ptr [ecx+2Ch]
        test    esi, esi
        jmp     RETURN_CrashFix_Misc7
    cont:
        push    7
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc7B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// Get2DEffectAtomicCallback
//
// Mantis #5468  1/3 (https://web.archive.org/web/20160401233418/https://bugs.mtasa.com/view.php?id=5468)
//
// Null pointer atomic->geometry in the RpAtomic
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc8  0x73485D
#define HOOKSIZE_CrashFix_Misc8 5
DWORD                         RETURN_CrashFix_Misc8 = 0x734862;
DWORD                         RETURN_CrashFix_Misc8B = 0x734871;
static void __declspec(naked) HOOK_CrashFix_Misc8()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    ecx, ecx
        je      cont            // Skip much code if ecx is zero (no 2d effect plugin)

        mov     ecx, dword ptr [edx+ecx]
        test    ecx, ecx
        jmp     RETURN_CrashFix_Misc8
    cont:
        push    8
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc8B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CProjectileInfo::Update
//
// Mantis #5468 2/3 (https://web.archive.org/web/20160401233418/https://bugs.mtasa.com/view.php?id=5468)
//
// Null pointer projectile of type CObject* in the array CProjectileInfo::ms_apProjectile
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc9  0x738B64
#define HOOKSIZE_CrashFix_Misc9 6
DWORD                         RETURN_CrashFix_Misc9 = 0x738B6A;
DWORD                         RETURN_CrashFix_Misc9B = 0x73983A;
static void __declspec(naked) HOOK_CrashFix_Misc9()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    esi, esi
        je      cont            // Skip much code if esi is zero (invalid projectile)

        mov     eax, dword ptr [esi+40h]
        test    ah, 1
        jmp     RETURN_CrashFix_Misc9
    cont:
        push    9
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc9B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CEntity::TransformFromObjectSpace
//
// Mantis #5468 3/3 (https://web.archive.org/web/20160401233418/https://bugs.mtasa.com/view.php?id=5468)
//
// Invalid pointer to a vector
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc10  0x5334FE
#define HOOKSIZE_CrashFix_Misc10 6
DWORD                         RETURN_CrashFix_Misc10 = 0x533504;
DWORD                         RETURN_CrashFix_Misc10B = 0x533539;
static void __declspec(naked) HOOK_CrashFix_Misc10()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     ecx, 0x80
        jb      cont            // Skip much code if ecx is small (invalid vector pointer)

        mov     edx, dword ptr [ecx]
        mov     dword ptr [esp], edx
        jmp     RETURN_CrashFix_Misc10
    cont:
        push    10
        call    CrashAverted
        mov     ecx, dword ptr [esp+1Ch]
        mov     dword ptr [ecx],0
        mov     dword ptr [ecx+4],0
        mov     dword ptr [ecx+8],0
        jmp     RETURN_CrashFix_Misc10B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RpAnimBlend::FrameUpdateCallBackSkinned
//
// Mantis #5576 (https://web.archive.org/web/20160408163600/https://bugs.mtasa.com/view.php?id=5576)
//
// The crash likely occurs due to invalid data passed as the second argument
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc11  0x4D2C62
#define HOOKSIZE_CrashFix_Misc11 5
DWORD                         RETURN_CrashFix_Misc11 = 0x4D2C67;
DWORD                         RETURN_CrashFix_Misc11B = 0x4D2E03;
static void __declspec(naked) HOOK_CrashFix_Misc11()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    ecx, ecx
        je      cont            // Skip much code if ecx is zero (invalid anim somthing)

        mov     eax, dword ptr [ecx+10h]
        test    eax, eax
        jmp     RETURN_CrashFix_Misc11
    cont:
        push    11
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc11B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimManager::UncompressAnimation
//
// Mantis #5530 (https://web.archive.org/web/20160411114105/https://bugs.mtasa.com/view.php?id=5530)
//
// Null pointer of type CAnimBlendHierarchy passed to the function
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc12  0x4D41C5
#define HOOKSIZE_CrashFix_Misc12 5
DWORD                         RETURN_CrashFix_Misc12 = 0x4D41CA;
DWORD                         RETURN_CrashFix_Misc12B = 0x4D4222;
static void __declspec(naked) HOOK_CrashFix_Misc12()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    edi, edi
        je      cont            // Skip much code if edi is zero (invalid anim somthing)

        mov     al, byte ptr [edi+0Bh]
        test    al, al
        jmp     RETURN_CrashFix_Misc12
    cont:
        push    12
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc12B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimManager::BlendAnimation
//
// Invalid animation (Null pointer returned by CAnimBlendAssocGroup::GetAnimation)
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc13  0x4D464E
#define HOOKSIZE_CrashFix_Misc13 6
DWORD                         RETURN_CrashFix_Misc13 = 0x4D4654;
DWORD                         RETURN_CrashFix_Misc13B = 0x4D4764;
static void __declspec(naked) HOOK_CrashFix_Misc13()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     eax, 0x2480
        jb      cont            // Skip much code if eax is less than 0x480 (invalid anim)

        mov     al, byte ptr [eax+0Ah]
        shr     al, 5
        jmp     RETURN_CrashFix_Misc13
    cont:
        push    13
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc13B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAEFrontendAudioEntity::AddAudioEvent
//
// Invalid pointer to the array CAEAudioEntity::m_pAudioEventVolumes
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc14  0x4DD4B5
#define HOOKSIZE_CrashFix_Misc14 6
DWORD                         RETURN_CrashFix_Misc14 = 0x4DD4BB;
static void __declspec(naked) HOOK_CrashFix_Misc14()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
        {
        mov     eax, dword ptr ds:[0BD00F8h]
        cmp     eax, 0
        je      cont            // Skip much code if eax is zero ( Audio event volumes table not initialized )

        sub     esp, 0D4h
        jmp     RETURN_CrashFix_Misc14
    cont:
        push    14
        call    CrashAverted
        add     esp, 12
        retn    12
        }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
void _cdecl DoWait(HANDLE hHandle)
{
    DWORD dwWait = 4000;
    DWORD dwResult = WaitForSingleObject(hHandle, dwWait);
    if (dwResult == WAIT_TIMEOUT)
    {
        AddReportLog(6211, SString("WaitForSingleObject timed out with %08x and %dms", hHandle, dwWait));
        // This thread lock bug in GTA will have to be fixed one day.
        // Until then, a 5 second freeze should be long enough for the loading thread to have finished it's job.
#if 0
        _wassert ( _CRT_WIDE("\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            Press IGNORE  ('I' Key) \n\
            ")
         , _CRT_WIDE(__FILE__), __LINE__);
#endif
        dwResult = WaitForSingleObject(hHandle, 1000);
    }
}

// hook info
#define HOOKPOS_FreezeFix_Misc15  0x156CDAE
#define HOOKSIZE_FreezeFix_Misc15 6
DWORD                         RETURN_FreezeFix_Misc15 = 0x156CDB4;
static void __declspec(naked) HOOK_FreezeFix_Misc15()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pop eax
        pop edx
        pushad

        push eax
        call DoWait
        add esp, 4

        popad
        jmp     RETURN_FreezeFix_Misc15
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CPed::PlayFootSteps
//
// RpAnimBlendClumpGetFirstAssociation returns null pointer
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc16  0x5E5815
#define HOOKSIZE_CrashFix_Misc16 6
DWORD                         RETURN_CrashFix_Misc16 = 0x5E581B;
static void __declspec(naked) HOOK_CrashFix_Misc16()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     eax, 0
        je      cont            // Skip much code if eax is zero ( RpAnimBlendClumpGetFirstAssociation returns NULL )

         // continue standard path
        movsx   ecx, word ptr [eax+2Ch]
        xor     edi, edi
        jmp     RETURN_CrashFix_Misc16

    cont:
        push    16
        call    CrashAverted
        add     esp, 96
        retn
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RwFrameForAllObjects
//
// Null pointer for the objectList (0x90) field of the RwFrame structure
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc17  0x7F120E
#define HOOKSIZE_CrashFix_Misc17 6
DWORD                         RETURN_CrashFix_Misc17 = 0x7F1214;
DWORD                         RETURN_CrashFix_Misc17B = 0x7F1236;
static void __declspec(naked) HOOK_CrashFix_Misc17()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     eax, 0
        je      cont            // Skip much code if eax is zero

         // continue standard path
        mov     eax, [eax+90h]
        jmp     RETURN_CrashFix_Misc17

    cont:
        push    17
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc17B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CVehicleModelInfo::GetWheelPosn
//
// Null pointer frame returned by CClumpModelInfo::GetFrameFromId
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc18  0x4C7DAD
#define HOOKSIZE_CrashFix_Misc18 7
DWORD                         RETURN_CrashFix_Misc18 = 0x4C7DB4;
static void __declspec(naked) HOOK_CrashFix_Misc18()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
        {
        cmp     ebp, 0
        je      cont            // Skip much code if ebp is zero

             // continue standard path
        mov         edx,dword ptr [ebp+40h]
        mov         eax,dword ptr [esp+10h]
        jmp     RETURN_CrashFix_Misc18

    cont:
        push    18
        call    CrashAverted
        mov         edx,0
        mov         eax,dword ptr [esp+10h]
        mov         dword ptr [eax],edx
        mov         dword ptr [eax+4],edx
        pop         esi
        mov         dword ptr [eax+8],edx
        pop         ebp
        ret         0Ch
        }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RwFrameAddChild
//
// The pointer passed as the first argument of type RwFrame to the function is null
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc19  0x7F0BF7
#define HOOKSIZE_CrashFix_Misc19 6
DWORD                         RETURN_CrashFix_Misc19 = 0x7F0BFD;
DWORD                         RETURN_CrashFix_Misc19B = 0x7F0C20;
static void __declspec(naked) HOOK_CrashFix_Misc19()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     esi, 0
        je      cont            // Skip much code if esi is zero

         // continue standard path
        mov     eax, [esi+98h]
        jmp     RETURN_CrashFix_Misc19

    cont:
        push    19
        call    CrashAverted
        mov     edx,dword ptr [ecx+98h]
        test    edx,edx
        jmp     RETURN_CrashFix_Misc19B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CPlaceable::RemoveMatrix
//
// "this" is invalid
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc20  0x54F3B0
#define HOOKSIZE_CrashFix_Misc20 6
DWORD                         RETURN_CrashFix_Misc20 = 0x54F3B6;
static void __declspec(naked) HOOK_CrashFix_Misc20()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     ecx, 0
        je      cont            // Skip much code if ecx is zero

         // continue standard path
        sub     esp, 10h
        mov     eax, [ecx+14h]
        jmp     RETURN_CrashFix_Misc20

    cont:
        push    20
        call    CrashAverted
        retn
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CTaskSimpleCarFallOut::FinishAnimFallOutCB
//
// Handle CTaskSimpleCarFallOut::FinishAnimFallOutCB having wrong data
////////////////////////////////////////////////////////////////////////
bool IsTaskSimpleCarFallOutValid(CAnimBlendAssociationSAInterface* pAnimBlendAssociation, CTaskSimpleCarFallOutSAInterface* pTask)
{
    if (pTask->VTBL != (TaskVTBL*)VTBL_CTaskSimpleCarFallOut)
    {
        AddReportLog(8530, SString("IsTaskSimpleCarFallOutValid fail - pTask->VTBL: %08x", pTask->VTBL), 5);
        return false;
    }

    if (pTask->pVehicle)
    {
        SClientEntity<CVehicleSA>* pVehicleClientEntity = pGameInterface->GetPools()->GetVehicle((DWORD*)pTask->pVehicle);
        CVehicle*                  pVehicle = pVehicleClientEntity ? pVehicleClientEntity->pEntity : nullptr;
        if (!pVehicle)
        {
            // Task looks valid, but vehicle is not recognised by MTA
            AddReportLog(8531, SString("IsTaskSimpleCarFallOutValid invalid vehicle ptr - pTask->pVehicle: %08x", pTask->pVehicle), 5);
            pTask->pVehicle = NULL;
            return true;
        }
    }

    return true;
}

#define HOOKPOS_CrashFix_Misc21  0x648EE0
#define HOOKSIZE_CrashFix_Misc21 7
DWORD                         RETURN_CrashFix_Misc21 = 0x648EE7;
static void __declspec(naked) HOOK_CrashFix_Misc21()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    IsTaskSimpleCarFallOutValid
        add     esp, 4*2
        cmp     al,0
        popad
        je      cont            // Skip much code if CTaskSimpleCarFallOut is not valid

         // continue standard path
        mov     eax, [esp+8]
        mov     ecx, [eax+10h]
        jmp     RETURN_CrashFix_Misc21

    cont:
        push    21
        call    CrashAverted
        retn
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimBlendAssociation::Init
//
// this->m_clumpAssoc.m_pNodeArray[v5] is invalid
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc22  0x4CEF08
#define HOOKSIZE_CrashFix_Misc22 6
DWORD                         RETURN_CrashFix_Misc22 = 0x4CEF25;
static void __declspec(naked) HOOK_CrashFix_Misc22()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov         edx,dword ptr [edi+0Ch]

        cmp     edx, 0x480
        jb      altcode            // Fill output with zeros if edx is low

         // do standard code
    lp1:
        mov         edx,dword ptr [edi+0Ch]
        mov         edx,dword ptr [edx+eax*4]
        mov         ebx,dword ptr [esi+10h]
        mov         dword ptr [ebx+ecx+10h],edx
        mov         edx,dword ptr [esi+10h]
        mov         dword ptr [edx+ecx+14h],esi
        movsx       edx,word ptr [esi+0Ch]
        inc         eax
        add         ecx,18h
        cmp         eax,edx
        jl          lp1
        jmp     RETURN_CrashFix_Misc22

          // do alternate code
    altcode:
        push    22
        call    CrashAverted
        mov     edx,0
        mov         ebx,dword ptr [esi+10h]
        mov         dword ptr [ebx+ecx+10h],edx
        mov         dword ptr [ebx+ecx+14h],edx
        movsx       edx,word ptr [esi+0Ch]
        inc         eax
        add         ecx,18h
        cmp         eax,edx
        jl          altcode
        jmp     RETURN_CrashFix_Misc22
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CVehicleAnimGroup::ComputeAnimDoorOffsets
//
// Door index is out of range
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc23  0x6E3D10
#define HOOKSIZE_CrashFix_Misc23 7
DWORD                         RETURN_CrashFix_Misc23 = 0x6E3D17;
static void __declspec(naked) HOOK_CrashFix_Misc23()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Ensure door index is reasonable
        mov     edx, [esp+8]
        cmp     edx,16
        jb      ok

            // zero if out of range
        mov     edx,0
        mov     [esp+8], edx
        push    23
        call    CrashAverted

    ok:
        // continue standard path
        mov     edx, [esp+8]
        lea     eax, [edx+edx*2]
        jmp     RETURN_CrashFix_Misc23
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RwFrameForAllChildren
//
// The first argument of type RwFrame received is invalid
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc24  0x7F0DC8
#define HOOKSIZE_CrashFix_Misc24 6
DWORD                         RETURN_CrashFix_Misc24 = 0x7F0DCE;
static void __declspec(naked) HOOK_CrashFix_Misc24()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     ebp, 0x480
        jb      cont            // Skip code if ebp is low

         // continue standard path
        mov     eax, [ebp+98h]
        jmp     RETURN_CrashFix_Misc24

    cont:
        push    24
        call    CrashAverted
        mov     ebp, 0
        mov     eax, 0
        jmp     RETURN_CrashFix_Misc24
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CTaskSimpleCarOpenDoorFromOutside::FinishAnimCarOpenDoorFromOutsideCB
//
// Null vehicle pointer
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc25  0x646026
#define HOOKSIZE_CrashFix_Misc25 5
DWORD                         RETURN_CrashFix_Misc25 = 0x64602B;
static void __declspec(naked) HOOK_CrashFix_Misc25()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Check for zero pointer to vehicle
        mov     eax, [esi+0x10]
        cmp     eax, 0
        jz      fix

            // Continue standard path
        lea     eax,[esp+10h]
        push    eax
        jmp     RETURN_CrashFix_Misc25

    fix:
        push    25
        call    CrashAverted
            // Do special thing
        pop     esi
        pop     ecx
        retn
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CShotInfo::Update
//
// _creator->m_pIntelligence is invalid
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc26  0x739FA0
#define HOOKSIZE_CrashFix_Misc26 6
DWORD                         RETURN_CrashFix_Misc26 = 0x739FA6;
static void __declspec(naked) HOOK_CrashFix_Misc26()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Check for incorrect pointer
        cmp     ebx, 130h
        jz      fix

            // Continue standard path
        mov     edi,dword ptr [ebx+ebp*4]
        dec     ebp
        test    edi,edi
        jmp     RETURN_CrashFix_Misc26

    fix:
        push    26
        call    CrashAverted
             // Do special thing
        mov     edi, 0
        dec     ebp
        test    edi,edi
        jmp     RETURN_CrashFix_Misc26
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CTaskComplexDieInCar::ControlSubTask
//
// ped or ped->m_pVehicle is null pointer
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc27  0x6377FB
#define HOOKSIZE_CrashFix_Misc27 7
DWORD                         RETURN_CrashFix_Misc27 = 0x637802;
static void __declspec(naked) HOOK_CrashFix_Misc27()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Execute replaced code
        cmp     byte ptr [edi+484h], 2
        je      cont

            // Check if veh pointer is zero
        mov     ecx, [edi+58Ch]
        test    ecx, ecx
        jne     cont
        push    27
        call    CrashAverted

cont:
        // Continue standard path
        jmp     RETURN_CrashFix_Misc27
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CObject::ProcessGarageDoorBehaviour
//
// Null this->m_pDummyObject pointer
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc28  0x44A4FD
#define HOOKSIZE_CrashFix_Misc28 6
DWORD                         RETURN_CrashFix_Misc28 = 0x44A503;
DWORD                         RETURN_CrashFix_Misc28B = 0x44A650;
static void __declspec(naked) HOOK_CrashFix_Misc28()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Execute replaced code
        mov     eax, [esi+170h]

        // Check if dummy pointer is zero
        test    eax, eax
        jne     cont

        push    28
        call    CrashAverted
                // Skip much code
        jmp     RETURN_CrashFix_Misc28B

cont:
        // Continue standard path
        jmp     RETURN_CrashFix_Misc28
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAEMP3BankLoader::IsSoundBankLoaded
//
// The value of the argument BankSlotId is invalid
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc29  0x4E022C
#define HOOKSIZE_CrashFix_Misc29 5
DWORD                         RETURN_CrashFix_Misc29 = 0x4E0231;
DWORD                         RETURN_CrashFix_Misc29B = 0x4E0227;
static void __declspec(naked) HOOK_CrashFix_Misc29()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Execute replaced code
        movsx   eax,word ptr [esp+8]

        // Check word being -1
        cmp     al, 0xffff
        jz      cont

            // Continue standard path
        jmp     RETURN_CrashFix_Misc29

cont:
        push    29
        call    CrashAverted
            // Skip much code
        jmp     RETURN_CrashFix_Misc29B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimBlendAssociation::SetFinishCallback
//
// "this" is invalid
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc30   0x4CEBE8
#define HOOKSIZE_CrashFix_Misc30  7
#define HOOKCHECK_CrashFix_Misc30 0xC7
DWORD                         RETURN_CrashFix_Misc30 = 0x4CEBEF;
DWORD                         RETURN_CrashFix_Misc30B = 0x4CEBF5;
static void __declspec(naked) HOOK_CrashFix_Misc30()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Check for incorrect pointer
        cmp     ecx, 0
        jz      cont

            // Execute replaced code
        mov     dword ptr [ecx+30h], 1
        // Continue standard path
        jmp     RETURN_CrashFix_Misc30

cont:
        push    30
        call    CrashAverted
            // Skip much code
        jmp     RETURN_CrashFix_Misc30B
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimBlendAssociation::SetCurrentTime
//
// "this" is invalid
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc32  0x4CEA80
#define HOOKSIZE_CrashFix_Misc32 8
DWORD RETURN_CrashFix_Misc32 = 0x4CEA88;

static void __declspec(naked) HOOK_CrashFix_Misc32()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
        {
        test    ecx, ecx
        jz      cont

             // Check hierarchy pointer (offset 0x14)
             // We can use eax as scratch because it gets overwritten by the first replaced instruction anyway
        mov     eax, [ecx+14h]
        test    eax, eax
        jz      cont

             // Execute replaced code
        mov     eax, [esp+4]
        fld     dword ptr [esp+4]
        jmp     RETURN_CrashFix_Misc32

    cont:
        push    32
        call    CrashAverted
        retn    4
        }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CVehicleModelInfo::FindTextureCB (0x4C7510)
// SA's inlined strcpy to 32-byte buffer overruns if name >= 31 chars > 0xC0000409
// Reimplement it using safe string handling
//
// Hook interaction: This calls RwTexDictionaryFindNamedTexture (0x7F39F0) which has
// Misc33 hook. Direct calls causes re-entry and stack corruption.
// Solution: CallOriginalRwTexDictionaryFindNamedTexture replicates Misc33's overwritten
// bytes (mov eax,[esp+4]; push ebx) and jumps to 0x7F39F5 to work around the hook.
////////////////////////////////////////////////////////////////////////
typedef RwTexture*(__cdecl* RwTexDictionaryFindNamedTexture_t)(RwTexDictionary* dict, const char* name);
typedef RwTexDictionary*(__cdecl* RwTexDictionaryGetCurrent_t)();

static RwTexDictionaryGetCurrent_t pfnRwTexDictionaryGetCurrentForMisc39 = (RwTexDictionaryGetCurrent_t)0x7F3A90;

// Trampoline to call the ORIGINAL RwTexDictionaryFindNamedTexture at 0x7F39F0,
// thereby working around HOOK_CrashFix_Misc33.
// Replicates the 5 overwritten bytes (mov eax,[esp+4]; push ebx) then jumps to 0x7F39F5.
static constexpr DWORD       AddrFindNamedTexture_Continue = 0x7F39F5;
static void _declspec(naked) TrampolineRwTexDictionaryFindNamedTexture()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        mov     eax, [esp+4]
        push    ebx
        jmp     AddrFindNamedTexture_Continue
    }
    // clang-format on
}

static constexpr std::size_t TextureNameSize = 32;
static constexpr char        RemapPrefix[] = "remap";

static RwTexture* __cdecl OnMY_FindTextureCB(const char* name)
{
    if (name == nullptr)
        return nullptr;

    const auto RwTexDictionaryFindNamedTexture =
        reinterpret_cast<RwTexDictionaryFindNamedTexture_t>(reinterpret_cast<void*>(TrampolineRwTexDictionaryFindNamedTexture));
    const auto RwTexDictionaryGetCurrent = pfnRwTexDictionaryGetCurrentForMisc39;

    RwTexDictionary* vehicleTxd = *reinterpret_cast<RwTexDictionary**>(0x00B4E688);
    if (vehicleTxd != nullptr)
    {
        RwTexture* tex = RwTexDictionaryFindNamedTexture(vehicleTxd, name);
        if (tex != nullptr)
        {
            return tex;
        }
    }

    RwTexDictionary* currentTxd = RwTexDictionaryGetCurrent();
    if (currentTxd == nullptr)
        return nullptr;

    RwTexture* tex = RwTexDictionaryFindNamedTexture(currentTxd, name);

    if (std::strncmp(name, RemapPrefix, sizeof(RemapPrefix) - 1) == 0)
    {
        if (tex == nullptr)
        {
            const std::size_t nameLen = strnlen(name, TextureNameSize);
            if (nameLen >= TextureNameSize)
            {
                OnCrashAverted(39);
                return nullptr;
            }
            char tmp[TextureNameSize];
            std::memcpy(tmp, name, nameLen + 1);
            tmp[0] = '#';
            return RwTexDictionaryFindNamedTexture(currentTxd, tmp);
        }
        else
        {
            if (IsWritablePtr(tex, sizeof(RwTexture)))
            {
                tex->name[0] = '#';
            }
        }
    }

    return tex;
}

#define HOOKPOS_CrashFix_Misc39  0x4C7510
#define HOOKSIZE_CrashFix_Misc39 5
static void __declspec(naked) HOOK_CrashFix_Misc39()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        push    [esp+4]
        call    OnMY_FindTextureCB
        add     esp, 4
        ret
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RwTexDictionaryFindNamedTexture (0x7F39F0)
//
// Crash occur when dict pointer is NULL or invalid (not a valid RwTexDictionary).
// This can happen with corrupted texture dictionary chains or during streaming issues.
//
// Validates the dict pointer before calling the SA function
//
// Overwrites the first 5 bytes at 0x7F39F0:
//   Original: 8B 44 24 04 53  (mov eax,[esp+4]; push ebx)
//   Replaced: E9 xx xx xx xx  (jmp HOOK_CrashFix_Misc33)
//
// HOOK_CrashFix_Misc39 (FindTextureCB replacement) needs to call this function
// but has to dodge this hook (risk of re-entry and stack corruption)
// See CallOriginalRwTexDictionaryFindNamedTexture in the Misc39 code for the
// trampoline that replicates the overwritten bytes and jumps to 0x7F39F5.
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc33  0x7F39F0
#define HOOKSIZE_CrashFix_Misc33 5
DWORD RETURN_CrashFix_Misc33 = 0x7F39F5;

typedef RwTexDictionary*(__cdecl* PFN_RwTexDictionaryGetCurrent)();
PFN_RwTexDictionaryGetCurrent pfnRwTexDictionaryGetCurrent = (PFN_RwTexDictionaryGetCurrent)0x7F3A90;

// Helper to execute the original func's overwritten prologue and continue.
// Used by HOOK_CrashFix_Misc33 when validation passes.
static void __declspec(naked) CallOriginalFindNamedTexture()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        // Replicate overwritten bytes at 0x7F39F0
        mov     eax, [esp+4]        // Original: mov eax, [esp+dict]
        push    ebx                  // Original: push ebx
        jmp     RETURN_CrashFix_Misc33  // Continue at 0x7F39F5: add eax, 8
    }
    // clang-format on
}

static void __declspec(naked) HOOK_CrashFix_Misc33()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        // Get first argument (dict)
        mov     ecx, [esp+4]

        // Grab name parameter for later reuse
        mov     edx, [esp+8]

        // Name must be valid
        test    edx, edx
        jz      invalid_texture

            // Check for NULL dict
        test    ecx, ecx
        jz      invalid_texture

            // Check for valid pointer
        cmp     ecx, 0x10000
        jb      invalid_texture

        // Check if it's a dictionary (RwObject type 6 at offset 0)
        cmp     byte ptr [ecx], 6
        jne     use_current_dict

            // Validate dict->texturesInDict.next (offset 8)
        mov     eax, [ecx+8]
        test    eax, eax
        jz      invalid_texture

        // All validation passed - execute original function
        jmp     CallOriginalFindNamedTexture

    use_current_dict:
        // Dict exists but is not type 6, try to recover using current dictionary
        // Save name in edx across the call
        push    edx
        call    pfnRwTexDictionaryGetCurrent
        pop     edx
        test    eax, eax
        jz      invalid_texture

        // Replace the invalid dict argument on stack with current one
        mov     [esp+4], eax
        // Continue with corrected dict
        jmp     CallOriginalFindNamedTexture

    invalid_texture:
        push    33
        call    CrashAverted
        xor     eax, eax            // Return null (texture not found)
        retn                        // cdecl
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CCustomCarEnvMapPipeline::CustomPipeRenderCB - EnvWave path
//
// NULL env map plugin data pointer (EBP) causes crash in CalculateEnvMap
// Crash when rendering a car environment map (Accessing [esi+2])
// This occurs when a material has MF_HAS_SHINE_WAVE flag set
// but the env map plugin data slot is NULL.
//
// Hook at 0x5D9CB2 replaces: jz loc_5D9E0D (original flag check)
//
// Original code flow at loc_5D9CAC:
//  0x5D9CAC: mov al, [esp+4Ch+var_39]  ; load EnvWave flag
//  0x5D9CB0: test al, al
//  0x5D9CB2: jz loc_5D9E0D             ; skip if flag not set <-- HOOKED
//  0x5D9CB8: mov eax, ds:_RwEngineInstance (EnvWave processing begins)
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc34  0x5D9CB2
#define HOOKSIZE_CrashFix_Misc34 6
DWORD                         RETURN_CrashFix_Misc34 = 0x5D9CB8;
DWORD                         RETURN_CrashFix_Misc34_Skip = 0x5D9E0D;  // Skip to end of EnvWave block
static void __declspec(naked) HOOK_CrashFix_Misc34()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        // Replicate original flag check: ZF was set by previous "test al, al"
        // If ZF is set (al == 0, flag not set), skip EnvWave (original behavior)
        // Note: JMP does not modify flags, so ZF from "test al, al" is preserved
        jz      skip_envwave_normal

            // Flag is set - check if EBP (env map plugin data) is valid
            // EBP was loaded at loc_5D9A00: mov ebp, [ecx+esi] (material plugin slot)
            // Check if EBP points to a valid address (catches NULL and low invalid addresses)
        cmp     ebp, 0x10000
        jb      skip_envwave_crash

            // EBP is valid, continue with EnvWave processing
            // Return to 0x5D9CB8: mov eax, ds:_RwEngineInstance
        jmp     RETURN_CrashFix_Misc34

    skip_envwave_normal:
        // Normal skip - flag was not set (original behavior)
        jmp     RETURN_CrashFix_Misc34_Skip

    skip_envwave_crash:
        // Crash averted - flag was set but EBP was NULL or invalid
        push    34
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc34_Skip
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RwTexDictionaryForAllTextures
//
// Invalid list entry pointer (crash at 0x7F374A: mov esi, [eax])
// (https://pastebin.com/hFduf1JB)
// WARNING: No pushad/popad with C++ calls (corrupts /GS cookie > 0xC0000409)
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc35   0x7F374A
#define HOOKSIZE_CrashFix_Misc35  5
#define HOOKCHECK_CrashFix_Misc35 0x8B
DWORD RETURN_CrashFix_Misc35 = 0x7F374F;
DWORD RETURN_CrashFix_Misc35_Abort = 0x7F3760;

static void __declspec(naked) HOOK_CrashFix_Misc35()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        cmp     eax, NUM_LOWMEM_THRESHOLD_ASM
        jb      abort_35

        push    eax
        push    ecx
        push    edx
        push    4
        push    eax
        call    IsReadablePtr
        add     esp, 8
        test    al, al
        pop     edx
        pop     ecx
        pop     eax
        jz      abort_35

        mov     esi, [eax]
        add     eax, 0FFFFFFF8h
        jmp     RETURN_CrashFix_Misc35

    abort_35:
        push    35
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc35_Abort
    }
    // clang-format on
}

///////////////////////////////////////////////////////////////////////////////
// RwTexDictionaryFindNamedTexture - Invalid/corrupted texture name pointer
// Crash: 0x7F3A17 (mov cl,[esi]), see https://pastebin.com/buBbyWRx and https://pastebin.com/1wTeTwu2
// Flow: validate ebx > compute ecx=ebx+8 > range-check > IsReadablePtr(ecx)
// Bad ptrs: 0xFF1B1B1B (high), 0x4E505444 (unmapped ASCII)
// WARNING: No pushad/popad with C++ calls (corrupts /GS cookie > 0xC0000409)
//          Use push eax/ecx/edx + pushfd instead (16 bytes vs 32)
///////////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc36   0x7F3A09
#define HOOKSIZE_CrashFix_Misc36  6
#define HOOKCHECK_CrashFix_Misc36 0x8D
DWORD RETURN_CrashFix_Misc36 = 0x7F3A0F;
DWORD RETURN_CrashFix_Misc36_Abort = 0x7F3A5C;

static void __declspec(naked) HOOK_CrashFix_Misc36()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        cmp     ebx, NUM_LOWMEM_THRESHOLD_ASM
        jb      abort_36

        // Replicate original code
        lea     eax, [ebx-8]
        lea     ecx, [eax+10h]

        cmp     ecx, NUM_LOWMEM_THRESHOLD_ASM
        jb      abort_36

        push    eax
        push    ecx
        push    edx
        push    1
        push    ecx
        call    IsReadablePtr
        add     esp, 8
        test    al, al
        pop     edx
        pop     ecx
        pop     eax
        jz      abort_36

        // Continue to original code at test ecx, ecx
        jmp     RETURN_CrashFix_Misc36

    abort_36:
        push    36
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc36_Abort
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RwTexDictionaryAddTexture
//
// Invalid list head pointer (crash at 0x7F39B3: mov [esi+4], edx)
// (https://pastebin.com/pkWwsSih)
// WARNING: No pushad/popad with C++ calls (corrupts /GS cookie > 0xC0000409)
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc37   0x7F39B3
#define HOOKSIZE_CrashFix_Misc37  5
#define HOOKCHECK_CrashFix_Misc37 0x89
DWORD RETURN_CrashFix_Misc37 = 0x7F39B8;

static void __declspec(naked) HOOK_CrashFix_Misc37()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;
    // clang-format off
    __asm
    {
        cmp     esi, NUM_LOWMEM_THRESHOLD_ASM
        jb      bad

        push    eax
        push    ecx
        push    edx
        push    8
        push    esi
        call    IsWritablePtr
        add     esp, 8
        test    al, al
        pop     edx
        pop     ecx
        pop     eax
        jz      bad

        cmp     esi, ecx
        jz      ok

        cmp     dword ptr [esi+4], ecx
        jz      ok

    bad:
        push    37
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc37

    ok:
        mov     [edx], esi
        mov     [esi+4], edx
        mov     [ecx], edx
        jmp     RETURN_CrashFix_Misc37
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// __rpD3D9SkinGeometryReinstance
//
// NULL pointer (out of video mem crash at 0x003C91CC: mov ebx, [esi])
// Hook at loc_7C91C0 validates [eax]
// On NULL, skip to loc_7C91DA (after the call block) to continue the loop.
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_Misc38   0x7C91C0
#define HOOKSIZE_CrashFix_Misc38  6
#define HOOKCHECK_CrashFix_Misc38 0x8B
DWORD RETURN_CrashFix_Misc38 = 0x7C91C6;
DWORD RETURN_CrashFix_Misc38_Skip = 0x7C91DA;  // loc_7C91DA: after call block, safe loop continuation

constexpr std::uint32_t NUM_VRAM_RELIEF_THROTTLE_MS = 500;

static void OnVideoMemoryExhausted()
{
    static DWORD s_dwLastReliefTick = 0;
    const DWORD  dwNow = GetTickCount32();

    if (dwNow - s_dwLastReliefTick < NUM_VRAM_RELIEF_THROTTLE_MS)
        return;

    s_dwLastReliefTick = dwNow;
    OnRequestDeferredStreamingMemoryRelief();
}

// No pushad/popad with C++ calls (corrupts /GS cookie > 0xC0000409)
static void __declspec(naked) HOOK_CrashFix_Misc38()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     esi, [eax]
        test    esi, esi
        jnz     ok

        // Save only volatile registers before call
        push    eax
        push    ecx
        push    edx
        call    OnVideoMemoryExhausted
        pop     edx
        pop     ecx
        pop     eax

        push    38
        call    CrashAverted
        jmp     RETURN_CrashFix_Misc38_Skip

    ok:
        lea     ecx, [esp+ecx*4+18h]
        jmp     RETURN_CrashFix_Misc38
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// __rpD3D9VertexDeclarationInstV3d
//
// Same class of issue as CrashFix_Misc38 (skin geometry path), usually due to out of video mem.
// This condition indicates out of video mem, but we'll alleviate it by calling OnVideoMemoryExhausted following the avert.
// NULL locked vertex buffer crash (VB Lock failure unchecked by RW).
// RW's D3D9AtomicDefaultInstanceCallback passes a NULL lockedVertexBuffer
// to this function when IDirect3DVertexBuffer9::Lock() fails and the
// HRESULT is discarded. The function writes vertex data to the NULL
// pointer, causing an crash at 0x00352BA7 (0x752BA7) (case 2, first store).
//
// __cdecl: [esp+4]=type, [esp+8]=mem, [esp+C]=src, [esp+10]=numVerts, [esp+14]=stride
// Returns D3D9VertexTypeSize[type] in EAX on all paths.
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_VBInstV3dNull   0x752AD0
#define HOOKSIZE_CrashFix_VBInstV3dNull  7
#define HOOKCHECK_CrashFix_VBInstV3dNull 0x51
DWORD RETURN_CrashFix_VBInstV3dNull = 0x752AD7;
DWORD ARRAY_D3D9VertexTypeSize = 0x874EF8;

static void _declspec(naked) HOOK_CrashFix_VBInstV3dNull()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp+8]                         // eax = mem (locked VB pointer)
        test    eax, eax
        jz      bail_out

        // Replicate overwritten prologue (7 bytes: push ecx; push ebx; push ebp; mov ebp,[esp+0Ch])
        push    ecx
        push    ebx
        push    ebp
        mov     ebp, [esp+10h]                       // type (was [esp+0C] before 3 pushes, +4 each = +0Ch+4 = 10h)
        jmp     RETURN_CrashFix_VBInstV3dNull

    bail_out:
        push    eax
        push    ecx
        push    edx
        call    OnVideoMemoryExhausted
        pop     edx
        pop     ecx
        pop     eax

        push    40
        call    CrashAverted

        // Return D3D9VertexTypeSize[type] to satisfy caller's contract
        mov     ecx, [esp+4]                         // ecx = type
        mov     eax, ARRAY_D3D9VertexTypeSize        // eax = array base address
        mov     eax, [eax + ecx*4]                   // eax = D3D9VertexTypeSize[type]
        ret
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// __rpD3D9VertexDeclarationInstV3dMorph
//
// Same NULL locked VB crash as above __rpD3D9VertexDeclarationInstV3d, but in the morph interpolation path.
// Called from D3D9AtomicDefaultInstanceCallback when geometry has morph
// targets. mem (2nd arg) receives lockedVertexBuffer[] which can be NULL.
// This condition indicates out of video mem, but we'll alleviate it by calling OnVideoMemoryExhausted following the avert.
//
// __cdecl: [esp+4]=type, [esp+8]=mem, [esp+C]=src1, [esp+10]=src2,
//          [esp+14]=scale(float), [esp+18]=numVerts, [esp+1C]=stride
// Returns D3D9VertexTypeSize[type] in EAX.
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CrashFix_VBInstV3dMorphNull   0x753B60
#define HOOKSIZE_CrashFix_VBInstV3dMorphNull  5
#define HOOKCHECK_CrashFix_VBInstV3dMorphNull 0x51
DWORD RETURN_CrashFix_VBInstV3dMorphNull = 0x753B65;

static void _declspec(naked) HOOK_CrashFix_VBInstV3dMorphNull()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp+8]                         // eax = mem (locked VB pointer)
        test    eax, eax
        jz      bail_out_morph

        // Replicate overwritten prologue (5 bytes: push ecx; mov eax,[esp+4+arg_0])
        push    ecx
        mov     eax, [esp+8]                         // type (was [esp+4+4] = [esp+8] after push ecx)
        jmp     RETURN_CrashFix_VBInstV3dMorphNull

    bail_out_morph:
        push    eax
        push    ecx
        push    edx
        call    OnVideoMemoryExhausted
        pop     edx
        pop     ecx
        pop     eax

        push    41
        call    CrashAverted

        // Return D3D9VertexTypeSize[type]
        mov     ecx, [esp+4]                         // ecx = type
        mov     eax, ARRAY_D3D9VertexTypeSize        // eax = array base address
        mov     eax, [eax + ecx*4]                   // eax = D3D9VertexTypeSize[type]
        ret
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CClumpModelInfo::GetFrameFromId
//
// Invalid frame
////////////////////////////////////////////////////////////////////////
RwFrame* OnMY_CClumpModelInfo_GetFrameFromId_Post(RwFrame* pFrameResult, DWORD _ebx, DWORD _esi, DWORD _edi, DWORD calledFrom, RpClump* pClump, int id)
{
    if (pFrameResult)
        return pFrameResult;

    // Don't check frame if call can legitimately return NULL
    if (calledFrom == 0x6D308F      // CVehicle::SetWindowOpenFlag
        || calledFrom == 0x6D30BF   // CVehicle::ClearWindowOpenFlag
        || calledFrom == 0x4C7DDE   // CVehicleModelInfo::GetOriginalCompPosition
        || calledFrom == 0x4C96BD)  // CVehicleModelInfo::CreateInstance
        return NULL;

    // Ignore external calls
    if (calledFrom < 0x401000 || calledFrom > 0x801000)
        return NULL;

    // Now we have a NULL frame which will cause a crash
    // (Probably due to a custom vehicle with missing frames)

    // See if we can get the model id for reporting
    int   iModelId = 0;
    DWORD pVehicle = NULL;

    if (calledFrom == 0x6D3847)  // CVehicle::AddReplacementUpgrade
        pVehicle = _ebx;
    else if (calledFrom == 0x6DFA61      // CVehicle::AddUpgrade
             || calledFrom == 0x6D3A62)  // CVehicle::GetReplacementUpgrade
        pVehicle = _edi;
    else if (calledFrom == 0x06AC740     // CAutomobile::PreRender (Forklift)
             || calledFrom == 0x6D39F3   // CVehicle::RemoveReplacementUpgrade
             || calledFrom == 0x6D3A32)  // CVehicle::RemoveReplacementUpgrade2
        pVehicle = _esi;

    if (pVehicle > 0x1000)
        iModelId = ((CVehicleSAInterface*)pVehicle)->m_nModelIndex;

    // Need a valid frame to replace the missing one - Find nearest other id
    for (uint i = 2; i < 40; i++)
    {
        RwFrame* pNewFrameResult = NULL;
        uint     uiNewId = id + (i / 2) * ((i & 1) ? -1 : 1);
        DWORD    dwFunc = 0x4C53C0;  // CClumpModelInfo::GetFrameFromId
        // clang-format off
        __asm
        {
            push    uiNewId
            push    pClump
            call    dwFunc
            add     esp, 8
            mov     pNewFrameResult,eax
        }
        // clang-format on

        if (pNewFrameResult)
        {
            SString strMsg("No frame for vehicle:%d  frameId:%d  (replaced with:%d  calledfrom:%08x)", iModelId, id, uiNewId, calledFrom);
            AddReportLog(5412, SString("GetFrameFromId - %s", *strMsg));
            LogEvent(5412, "Model frame warning", "GetFrameFromId", strMsg);
            return pNewFrameResult;
        }
    }

    // Couldn't find a replacement frame id
    SString strMsg("No frame for vehicle:%d  frameId:%d  (calledfrom:%08x)", iModelId, id, calledFrom);
    AddReportLog(5413, SString("GetFrameFromId - %s", *strMsg));
    LogEvent(5413, "Model frame error", "GetFrameFromId", strMsg);

    return NULL;
}

// Hook info
#define HOOKPOS_CClumpModelInfo_GetFrameFromId  0x4C53C0
#define HOOKSIZE_CClumpModelInfo_GetFrameFromId 7
DWORD                         RETURN_CClumpModelInfo_GetFrameFromId = 0x4C53C7;
static void __declspec(naked) HOOK_CClumpModelInfo_GetFrameFromId()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    [esp+4*2]
        push    [esp+4*2]
        call    inner
        add     esp, 4*2

        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    edi
        push    esi
        push    ebx
        push    eax
        call    OnMY_CClumpModelInfo_GetFrameFromId_Post
        mov     [esp+0],eax
        add     esp, 4*7
        popad
        mov     eax,[esp-32-4*7]
        retn

inner:
        sub     esp,8
        mov     eax,dword ptr [esp+10h]
        jmp     RETURN_CClumpModelInfo_GetFrameFromId
    }
    // clang-format on
}

CStreamingInfo* GetStreamingInfo(uint id)
{
    CStreamingInfo* pItemInfo = (CStreamingInfo*)(CStreaming__ms_aInfoForModel);
    return pItemInfo + id;
}

////////////////////////////////////////////////////////////////////////
// CEntity::GetBoundRect
//
// modelInfo->m_pColModel is a null pointer
////////////////////////////////////////////////////////////////////////
void OnMY_CEntity_GetBoundRect(CEntitySAInterface* pEntity)
{
    uint32                     usModelId = pEntity->m_nModelIndex;
    CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[usModelId];
    if (!pModelInfo)
    {
        // Crash will occur at offset 00134131
        LogEvent(814, "Model info missing", "CEntity_GetBoundRect", SString("No info for model:%d", usModelId), 5414);
        CArgMap argMap;
        argMap.Set("id", usModelId);
        argMap.Set("reason", "info");
        SetApplicationSetting("diagnostics", "gta-model-fail", argMap.ToString());
    }
    else
    {
        CColModelSAInterface* pColModel = pModelInfo->pColModel;
        if (!pColModel)
        {
            // Crash will occur at offset 00134134
            CStreamingInfo* pStreamingInfo = pGameInterface->GetStreaming()->GetStreamingInfo(usModelId);
            if (pStreamingInfo)
            {
                SString strDetails("refs:%d txd:%d RwObj:%08x bOwn:%d flg:%d off:%d size:%d loadState:%d", pModelInfo->usNumberOfRefs,
                                   pModelInfo->usTextureDictionary, pModelInfo->pRwObject, pModelInfo->bDoWeOwnTheColModel, pStreamingInfo->flg,
                                   pStreamingInfo->offsetInBlocks, pStreamingInfo->sizeInBlocks, pStreamingInfo->loadState);
                LogEvent(815, "Model collision missing", "CEntity_GetBoundRect", SString("No collision for model:%d %s", usModelId, *strDetails), 5415);
            }
            else
            {
                LogEvent(815, "Model collision missing", "CEntity_GetBoundRect", SString("No collision for model:%d (invalid streaming info)", usModelId),
                         5415);
            }
            CArgMap argMap;
            argMap.Set("id", usModelId);
            argMap.Set("reason", "collision");
            SetApplicationSetting("diagnostics", "gta-model-fail", argMap.ToString());
        }
    }
}

// Hook info
#define HOOKPOS_CEntity_GetBoundRect   0x534131
#define HOOKSIZE_CEntity_GetBoundRect  5
#define HOOKCHECK_CEntity_GetBoundRect 0x8B
DWORD                         RETURN_CEntity_GetBoundRect = 0x534136;
static void __declspec(naked) HOOK_CEntity_GetBoundRect()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    esi
        call    OnMY_CEntity_GetBoundRect
        add     esp, 4*1
        popad

             // Continue replaced code
        mov     eax, [ecx+14h]
        mov     edx, [eax]
        jmp     RETURN_CEntity_GetBoundRect
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////
// CVehicle_AddUpgrade
//
// Record some variables in case crash occurs
////////////////////////////////////////////////////////////////////////
void OnMY_CVehicle_AddUpgrade_Pre(CVehicleSAInterface* pVehicle, int iUpgradeId, int iFrame)
{
    CArgMap argMap;
    argMap.Set("vehid", pVehicle->m_nModelIndex);
    argMap.Set("upgid", iUpgradeId);
    argMap.Set("frame", iFrame);
    SetApplicationSetting("diagnostics", "gta-upgrade-fail", argMap.ToString());
}

void OnMY_CVehicle_AddUpgrade_Post()
{
    SetApplicationSetting("diagnostics", "gta-upgrade-fail", "");
}

// Hook info
#define HOOKPOS_CVehicle_AddUpgrade   0x6DFA20
#define HOOKSIZE_CVehicle_AddUpgrade  6
#define HOOKCHECK_CVehicle_AddUpgrade 0x51
DWORD                         RETURN_CVehicle_AddUpgrade = 0x6DFA26;
static void __declspec(naked) HOOK_CVehicle_AddUpgrade()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    ecx
        call    OnMY_CVehicle_AddUpgrade_Pre
        add     esp, 4*3
        popad

        push    [esp+4*2]
        push    [esp+4*2]
        call inner

        pushad
        call    OnMY_CVehicle_AddUpgrade_Post
        popad
        retn    8
inner:
        push    ecx
        push    ebx
        mov     ebx, [esp+16]
        jmp     RETURN_CVehicle_AddUpgrade
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CObject::~CObject, CObject::ProcessTrainCrossingBehavior
//
// Train crossings: Detach barrier from post (to be able to create objects 1373 and 1374 separately)
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CObject_Destructor_TrainCrossing_Check      0x59F7A8
#define HOOKPOS_CObject_ProcessTrainCrossingBehavior1       0x5A0C34
#define HOOKPOS_CObject_ProcessTrainCrossingBehavior2       0x5A0C54
#define RETURN_CObject_Destructor_TrainCrossing_Check       0x59F7AD
#define RETURN_CObject_Destructor_TrainCrossing_Invalid     0x59F811
#define RETURN_CObject_ProcessTrainCrossingBehavior1_Check  0x5A0C39
#define RETURN_CObject_ProcessTrainCrossingBehavior2_Check  0x5A0C59
#define RETURN_CObject_ProcessTrainCrossingBehavior_Invalid 0x5A0CBD

DWORD TrainCrossingFix_ReturnAddress, TrainCrossingFix_InvalidReturnAddress;
template <DWORD ReturnAddress, DWORD InvalidReturnAddress>
static void __declspec(naked) HOOK_TrainCrossingBarrierCrashFix()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // We cannot use template parameters directly in inline assembly; the following instructions don't modify registers
    TrainCrossingFix_ReturnAddress = ReturnAddress;
    TrainCrossingFix_InvalidReturnAddress = InvalidReturnAddress;

    // clang-format off
    __asm
    {
        test eax, eax            // Check if pLinkedBarrierPost exists
        jz jmp_invalid            // Skip the barrier stuff
        mov ecx, [eax+14h]            // Execute replaced code
        test ecx, ecx
        jmp TrainCrossingFix_ReturnAddress

jmp_invalid:
        jmp TrainCrossingFix_InvalidReturnAddress
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// Interior_c::Init
//
// GTA doesn't reset the furniture object counter, so do it manually everytime before GTA furnishes an interior (Interior_c::Init)
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_ResetFurnitureObjectCounter  0x593BF0
#define HOOKSIZE_ResetFurnitureObjectCounter 6
DWORD                         RETURN_ResetFurnitureObjectCounter = 0x593BF6;
static void __declspec(naked) HOOK_ResetFurnitureObjectCounter()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    *(int*)0xBB3A18 = 0;  // InteriorManager_c::ms_objectCounter

    // clang-format off
    __asm
    {
        // original instruction
        mov eax, fs:[0]
        jmp RETURN_ResetFurnitureObjectCounter
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CVolumetricShadowMgr_Render
//
// Custom models can cause problems for volumetric shadows.
// Record when volumetric shadows are being rendered so we can disable them if a crash occurs.
////////////////////////////////////////////////////////////////////////
void OnMY_CVolumetricShadowMgr_Render_Pre()
{
    OnEnterCrashZone(1);
}

void OnMY_CVolumetricShadowMgr_Render_Post()
{
    OnEnterCrashZone(0);
}

// Hook info
#define HOOKPOS_CVolumetricShadowMgr_Render   0x7113B0
#define HOOKSIZE_CVolumetricShadowMgr_Render  8
#define HOOKCHECK_CVolumetricShadowMgr_Render 0x83
DWORD                         RETURN_CVolumetricShadowMgr_Render = 0x7113B8;
static void __declspec(naked) HOOK_CVolumetricShadowMgr_Render()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_CVolumetricShadowMgr_Render_Pre
        popad

        call inner

        pushad
        call    OnMY_CVolumetricShadowMgr_Render_Post
        popad
        retn

inner:
     // Replaced code
        sub     esp, 18h
        mov     ecx, 0A9AE00h
        jmp     RETURN_CVolumetricShadowMgr_Render
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CVolumetricShadowMgr_Update
//
// Custom models can cause problems for volumetric shadows.
// Record when volumetric shadows are being updated so we can disable them if a crash occurs.
////////////////////////////////////////////////////////////////////////
void OnMY_CVolumetricShadowMgr_Update_Pre()
{
    OnEnterCrashZone(2);
}

void OnMY_CVolumetricShadowMgr_Update_Post()
{
    OnEnterCrashZone(0);
}

// Hook info
#define HOOKPOS_CVolumetricShadowMgr_Update   0x711D90
#define HOOKSIZE_CVolumetricShadowMgr_Update  5
#define HOOKCHECK_CVolumetricShadowMgr_Update 0xB9
DWORD                         RETURN_CVolumetricShadowMgr_Update = 0x711D95;
static void __declspec(naked) HOOK_CVolumetricShadowMgr_Update()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        call    OnMY_CVolumetricShadowMgr_Update_Pre
        popad

        push    [esp+4*1]
        call inner
        add     esp, 4*1

        pushad
        call    OnMY_CVolumetricShadowMgr_Update_Post
        popad
        retn

inner:
     // Replaced code
        mov     ecx, 0A9AE00h
        jmp     RETURN_CVolumetricShadowMgr_Update
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimManager::CreateAnimAssocGroups
//
// CModelInfo::ms_modelInfoPtrs at the given index is a null pointer
////////////////////////////////////////////////////////////////////////
void OnMY_CAnimManager_CreateAnimAssocGroups(uint uiModelId)
{
    CModelInfo*                pModelInfo = pGameInterface->GetModelInfo(uiModelId);
    CBaseModelInfoSAInterface* pInterface = pModelInfo ? pModelInfo->GetInterface() : nullptr;
    if (!pInterface || pInterface->pRwObject == nullptr)
    {
        // Crash will occur at offset 00349b7b
        LogEvent(816, "Model not loaded", "CAnimManager_CreateAnimAssocGroups", SString("No RwObject for model:%d", uiModelId), 5416);
        CArgMap argMap;
        argMap.Set("id", uiModelId);
        argMap.Set("reason", "createanim");
        SetApplicationSetting("diagnostics", "gta-model-fail", argMap.ToString());
    }
}

// Hook info
#define HOOKPOS_CAnimManager_CreateAnimAssocGroups   0x4D3D52
#define HOOKSIZE_CAnimManager_CreateAnimAssocGroups  5
#define HOOKCHECK_CAnimManager_CreateAnimAssocGroups 0x8B
DWORD                         RETURN_CAnimManager_CreateAnimAssocGroups = 0x4D3D59;
static void __declspec(naked) HOOK_CAnimManager_CreateAnimAssocGroups()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    eax
        call    OnMY_CAnimManager_CreateAnimAssocGroups
        add     esp, 4*1
        popad

             // Replaced code
        push    ecx
        mov     ecx, dword ptr[ARRAY_ModelInfo]
        mov     eax, dword ptr[ecx + eax*4]
        pop     ecx

        jmp     RETURN_CAnimManager_CreateAnimAssocGroups
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CTaskComplexCarSlowBeDraggedOut::CreateFirstSubTask
//
// Accessing a temporally not existing vehicle
// (seems to happen when the driver is slower being thrown out than the jacker enters the vehicle)
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask  0x6485AC
#define HOOKSIZE_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask 6
DWORD RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask = 0x6485B2;
DWORD RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask_Invalid = 0x6485E1;
void  OnMY_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask()
{
    LogEvent(817, "CTaskComplexCarSlowBeDraggedOut", "", "CTaskComplexCarSlowBeDraggedOut race condition");
}

static void __declspec(naked) HOOK_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test eax, eax
        jz invalid_vehicle

        mov ecx, [eax+460h]
        jmp RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask

    invalid_vehicle:
        pushad
        call OnMY_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask
        popad
        jmp RETURN_CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask_Invalid
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// OnMY_printf
//
// GTA outputs stuff via printf which we can use to help diagnose problems
////////////////////////////////////////////////////////////////////////
void _cdecl OnMY_printf(DWORD dwCalledFrom, const char* szMessage)
{
    SString strMessage = SStringX(szMessage).Replace("\n", "");

    // Ignore unimportant messages
    if (strMessage == "Initialised SoundManager" || strMessage == "%f")
    {
        return;
    }

    SString strContext("GTALOG Called from 0x%08x", dwCalledFrom);
    LogEvent(6311, "printf", strContext, strMessage, 6311);

    // Check for known issues
    if (strMessage == "Error subrastering")
    {
        // Couldn't create render target for CPostEffects
        BrowseToSolution("error-subrastering", EXIT_GAME_FIRST | ASK_GO_ONLINE, _("Problem with graphics driver"));
    }
    else if (strMessage == "Too many objects without modelinfo structures")
    {
        // An img file contains errors
        if (GetApplicationSetting("diagnostics", "img-file-corrupt").empty())
        {
            SetApplicationSetting("diagnostics", "img-file-corrupt", "gta3.img");
        }
    }
}

// hook info
#define HOOKPOS_printf   0x821982
#define HOOKSIZE_printf  7
#define HOOKCHECK_printf 0x6A
DWORD                         RETURN_printf = 0x821989;
static void __declspec(naked) HOOK_printf()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*1]
        push    [esp+32+4*1]
        call    OnMY_printf
        add     esp, 4*2
        popad

             // Replaced code
        push    10h
        push    887DC0h
        jmp     RETURN_printf
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RwMatrixMultiply
//
// The third received argument of type RwMatrixTag* is a null pointer
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_RwMatrixMultiply   0x7F18B0
#define HOOKSIZE_RwMatrixMultiply  6
#define HOOKCHECK_RwMatrixMultiply 0x8B
DWORD                         RETURN_RwMatrixMultiply = 0x7F18B6;
static void __declspec(naked) HOOK_RwMatrixMultiply()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp+0Ch]
        cmp     eax, 0x480
        jb      cont            // Skip code if eax is low

        mov     ecx, dword ptr ds:[0C979BCh]
        jmp     RETURN_RwMatrixMultiply

cont:
        push    31
        call    CrashAverted
        retn
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimBlendNode::GetCurrentTranslation
//
// Invalid endKeyFrameIndex
////////////////////////////////////////////////////////////////////////
void OnMY_CAnimBlendNode_GetCurrentTranslation(CAnimBlendNodeSAInterface* pInterface)
{
    if (!pInterface)
        return;

    // Crash will occur at offset 0x000CFCD6
    OnCrashAverted(32);
    CAnimBlendAssociationSAInterface* pAnimAssoc = pInterface->pAnimBlendAssociation;
    if (!pAnimAssoc)
        return;

    CAnimBlendSequenceSAInterface* pAnimSequence = pInterface->pAnimSequence;
    if (!pAnimSequence)
        return;

    CAnimBlendHierarchySAInterface* pAnimHierarchy = pAnimAssoc->pAnimHierarchy;
    if (!pAnimHierarchy)
        return;

    bool bSequenceExistsInHierarchy = false;

    if (pAnimHierarchy->pSequences && pAnimHierarchy->usNumSequences > 0 && pAnimHierarchy->usNumSequences < 1000)
    {
        CAnimBlendSequenceSAInterface* pAnimHierSequence = pAnimHierarchy->pSequences;
        for (int i = 0; i < pAnimHierarchy->usNumSequences; i++)
        {
            if (pAnimHierSequence == pAnimSequence)
            {
                bSequenceExistsInHierarchy = true;
                break;
            }
            pAnimHierSequence++;
        }
    }

    LogEvent(588, "GetCurrentTranslation", "Incorrect endKeyFrameIndex",
             SString("m_endKeyFrameId = %d | pAnimAssoc = %p | GroupID = %d | AnimID = %d | \
                pAnimSeq = %p | BoneID = %d | BoneHash = %u | \
                pAnimHier = %p | HierHash = %u | SequenceExistsInHierarchy: %s",
                     pInterface->m_endKeyFrameId, pAnimAssoc, pAnimAssoc->sAnimGroup, pAnimAssoc->sAnimID, pAnimSequence, pAnimSequence->m_boneId,
                     pAnimSequence->m_hash, pAnimHierarchy, pAnimHierarchy->uiHashKey, bSequenceExistsInHierarchy ? "Yes" : "No"),
             588);
}

// Hook info
#define HOOKPOS_CAnimBlendNode_GetCurrentTranslation  0x4CFCB5
#define HOOKSIZE_CAnimBlendNode_GetCurrentTranslation 6
DWORD                         RETURN_CAnimBlendNode_GetCurrentTranslation = 0x4CFCBB;
static void __declspec(naked) HOOK_CAnimBlendNode_GetCurrentTranslation()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    __asm
        {// if end key frame index is greater than 10,000 then return
        cmp     eax, 0x2710
        jg      altcode

             // Normal path - execute original code
        push    ebx
        mov     bl, [edx + 4]
        shr     bl, 1
        jmp     RETURN_CAnimBlendNode_GetCurrentTranslation

             // Crash prevention path
        altcode:
         // Save registers before logging
        pushad
        push    ebp  // Pass 'this' pointer
        call    OnMY_CAnimBlendNode_GetCurrentTranslation
        add     esp, 4
        popad

             // Return safely without executing original buggy code
             // The function expects 8 bytes of parameters
        retn    8
        }
}

////////////////////////////////////////////////////////////////////////
// CStreaming::AreAnimsUsedByRequestedModels
//
// GTA streamer will use this function to decide if IFP blocks should be unloaded or not.
// We will return true to disable unloading.
////////////////////////////////////////////////////////////////////////
bool __cdecl OnMY_CStreaming_AreAnimsUsedByRequestedModels(int modelID)
{
    // GTA SA possibly cannot have more than 200 IFP blocks since that's the limit
    const int maximumIFPBlocks = 200;
    if (modelID < 0 || modelID > maximumIFPBlocks)
    {
        return false;
    }

    std::unique_ptr<CAnimBlock> pInternalBlock = g_pCore->GetGame()->GetAnimManager()->GetAnimationBlock(modelID);
    if (!pInternalBlock->IsLoaded())
    {
        return false;
    }
    return true;
}

// Hook info
#define HOOKPOS_CStreaming_AreAnimsUsedByRequestedModels  0x407AD5
#define HOOKSIZE_CStreaming_AreAnimsUsedByRequestedModels 7
static void __declspec(naked) HOOK_CStreaming_AreAnimsUsedByRequestedModels()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        push    [esp + 4]
        call    OnMY_CStreaming_AreAnimsUsedByRequestedModels
        add     esp, 0x4
        retn
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CTrain::ProcessControl
//
// This hook overwrites the logic to wrap the train's rail distance, because in the
// original game code this could cause an infinite loop
//
//     0x6F8F83 | 88 9E CA 05 00 00 | mov     [esi + 5CAh], bl
// >>> 0x6F8F89 | D9 86 A8 05 00 00 | fld     dword ptr [esi + 5A8h]
//     0x6F8F8F | D8 1D 50 8B 85 00 | fcomp   ds: __real @00000000
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTrain__ProcessControl  0x6F8F89
#define HOOKSIZE_CTrain__ProcessControl 6
static DWORD CONTINUE_CTrain__ProcessControl = 0x6F8FE5;

// 0xC37FEC; float RailTrackLength[NUM_TRACKS]
static float* RailTrackLength = reinterpret_cast<float*>(0xC37FEC);

static void _cdecl WrapTrainRailDistance(CTrainSAInterface* train)
{
    // Check if the train is driving on a valid rail track (id < NUM_TRACKS)
    if (train->m_ucRailTrackID >= 4)
    {
        train->m_fTrainRailDistance = 0.0f;
        return;
    }

    // Check if the current rail track has a valid length (>= 1.0f)
    const float railTrackLength = RailTrackLength[train->m_ucRailTrackID];

    if (railTrackLength < 1.0f)
    {
        train->m_fTrainRailDistance = 0.0f;
        return;
    }

    // Check if the current rail distance is in the interval [0, railTrackLength)
    float railDistance = train->m_fTrainRailDistance;

    if (railDistance >= 0.0f && railDistance < railTrackLength)
        return;

    // Wrap the current rail distance
    if (railDistance > 0.0f)
    {
        railDistance = std::fmodf(railDistance, railTrackLength);
    }
    else
    {
        railDistance = railTrackLength - std::fmodf(std::fabsf(railDistance), railTrackLength);
    }

    train->m_fTrainRailDistance = railDistance;
}

static void __declspec(naked) HOOK_CTrain__ProcessControl()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    esi            // CVehicleSAInterface*
        call    WrapTrainRailDistance
        add     esp, 4
        popad
        jmp     CONTINUE_CTrain__ProcessControl
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask
//
// This hook adds a null-pointer check for eax, which stores the ped's current vehicle.
// Returning a null-pointer from this function will prevent the animation from being played.
//
//     0x648AAC | C2 04 00             | retn   4
// >>> 0x648AAF | 8B 80 84 03 00 00    | mov    eax, [eax + 384h]
//     0x648AB5 | 0F B6 80 DE 00 00 00 | movzx  eax, byte ptr [eax + 0DEh]
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask  0x648AAF
#define HOOKSIZE_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask 6
static DWORD CONTINUE_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask = 0x648AB5;

static void _cdecl LOG_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask()
{
    LogEvent(819, "CTaskComplexCarSlowBeDraggedOutAndStandUp::CreateFirstSubTask", "eax is null", "");
}

static void __declspec(naked) HOOK_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
        {
        test    eax, eax
        jz      returnZeroTaskLocation
        mov     eax, [eax + 384h]
        jmp     CONTINUE_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask

        returnZeroTaskLocation:
        pushad
        call    LOG_CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask
        popad
        pop     edi
        pop     esi
        retn    4
        }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CVehicleModelInfo::LoadVehicleColours
//
// A modified data/carcols.dat can have entries with invalid model names and these cause
// CModelInfo::GetModelInfo to return a null pointer, but the original code doesn't verify
// the return value and tries to use the null pointer. This hook adds a null pointer check
// and then skips the line if in the null case. There are two locations to hook.
////////////////////////////////////////////////////////////////////////
static void _cdecl LOG_CVehicleModelInfo__LoadVehicleColours(int location, const char* modelName)
{
    LogEvent(820 + location, "CVehicleModelInfo::LoadVehicleColours", "Could not find model by name:", modelName);
}

//     0x5B6B1B | E8 20 EE F0 FF | call  CModelInfo::GetModelInfo
// >>> 0x5B6B20 | 8B F0          | mov   esi, eax
//     0x5B6B22 | 8D 47 FF       | lea   eax, [edi - 1]
//     0x5B6B25 | 99             | cdq
#define HOOKPOS_CVehicleModelInfo__LoadVehicleColours_1  0x5B6B20
#define HOOKSIZE_CVehicleModelInfo__LoadVehicleColours_1 5
static DWORD CONTINUE_CVehicleModelInfo__LoadVehicleColours_1 = 0x5B6B25;
static DWORD SKIP_CVehicleModelInfo__LoadVehicleColours_1 = 0x5B6D04;

static void __declspec(naked) HOOK_CVehicleModelInfo__LoadVehicleColours_1()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    eax, eax
        jnz     continueLoadingColorLineLocation

        pushad
        lea     ecx, [esp + 55Ch - 440h]
        push    ecx
        push    0
        call    LOG_CVehicleModelInfo__LoadVehicleColours
        add     esp, 8
        popad

        add     esp, 54h
        jmp     SKIP_CVehicleModelInfo__LoadVehicleColours_1

        continueLoadingColorLineLocation:
        mov     esi, eax
        lea     eax, [edi - 1]
        jmp     CONTINUE_CVehicleModelInfo__LoadVehicleColours_1
    }
    // clang-format on
}

//     0x5B6CA5 | E8 96 EC F0 FF | call  CModelInfo::GetModelInfo
// >>> 0x5B6CAA | 8B F0          | mov   esi, eax
//     0x5B6CAC | 8D 47 FF       | lea   eax, [edi - 1]
//     0x5B6CAF | 99             | cdq
#define HOOKPOS_CVehicleModelInfo__LoadVehicleColours_2  0x5B6CAA
#define HOOKSIZE_CVehicleModelInfo__LoadVehicleColours_2 5
static DWORD CONTINUE_CVehicleModelInfo__LoadVehicleColours_2 = 0x5B6CAF;
static DWORD SKIP_CVehicleModelInfo__LoadVehicleColours_2 = 0x5B6D04;

static void __declspec(naked) HOOK_CVehicleModelInfo__LoadVehicleColours_2()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test    eax, eax
        jnz     continueLoadingColorLineLocation

        pushad
        lea     ecx, [esp + 59Ch - 440h]
        push    ecx
        push    1
        call    LOG_CVehicleModelInfo__LoadVehicleColours
        add     esp, 8
        popad

        add     esp, 94h
        jmp     SKIP_CVehicleModelInfo__LoadVehicleColours_2

        continueLoadingColorLineLocation:
        mov     esi, eax
        lea     eax, [edi - 1]
        jmp     CONTINUE_CVehicleModelInfo__LoadVehicleColours_2
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CPlaceName::Process
//
// Prevent the original game code from accessing the ped's vehicle, when it's a null pointer
// and the ped flag bInVehicle is set by setting the ped flag to zero.
//
//     0x571F37 | 8B F1             | mov   esi, ecx
// >>> 0x571F39 | 8B 88 6C 04 00 00 | mov   ecx, [eax + 46Ch]
//     0x571F3F | F6 C5 01          | test  ch, 1
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CPlaceName__Process  0x571F39
#define HOOKSIZE_CPlaceName__Process 6
static DWORD CONTINUE_CPlaceName__Process = 0x571F3F;

static void __declspec(naked) HOOK_CPlaceName__Process()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        mov     ecx, [eax + 46Ch]
        test    ch, 1            // if (ped->pedFlags.bInVehicle
        jz      continueAfterFixLocation
        mov     ebx, [eax + 58Ch]            //     && !ped->m_pVehicle)
        test    ebx, ebx
        jnz     continueAfterFixLocation
        and     ch, 0FEh
        mov     dword ptr [eax + 46Ch], ecx            // ped->pedFlags.bInVehicle = 0

        continueAfterFixLocation:
        popad
        mov     ecx, [eax + 46Ch]
        jmp     CONTINUE_CPlaceName__Process
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CWorld::FindObjectsKindaCollidingSectorList
//
// A null pointer at the given index in CModelInfo::ms_modelInfoPtrs or a null pointer in the m_pColModel field
////////////////////////////////////////////////////////////////////////
static void LOG_CWorld__FindObjectsKindaCollidingSectorList(unsigned int modelId)
{
    CBaseModelInfoSAInterface* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[modelId];
    if (!pModelInfo)
    {
        LogEvent(840, "Model info missing", "CWorld__FindObjectsKindaCollidingSectorList", SString("Corrupt model: %d", modelId), 5601);
        return;
    }

    if (!pModelInfo->pColModel)
    {
        LogEvent(840, "Col model missing", "CWorld__FindObjectsKindaCollidingSectorList", SString("Corrupt col model: %d", modelId), 5601);
    }
}

#define HOOKPOS_CWorld__FindObjectsKindaCollidingSectorList  0x56508C
#define HOOKSIZE_CWorld__FindObjectsKindaCollidingSectorList 0xA
static const unsigned int     RETURN_CWorld__FindObjectsKindaCollidingSectorList = 0x565096;
static const unsigned int     RETURN_CWorld__FindObjectsKindaCollidingSectorList_SKIP = 0x5650C3;
static void __declspec(naked) HOOK_CWorld__FindObjectsKindaCollidingSectorList()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov eax, [edx*4+0xA9B0C8]            // CModelInfo::ms_modelInfoPtrs
        test eax, eax
        jz skip

        mov ecx, [eax+0x14]            // m_pColModel
        test ecx, ecx
        jz skip

        jmp RETURN_CWorld__FindObjectsKindaCollidingSectorList

    skip:
        pushad
        push edx
        call LOG_CWorld__FindObjectsKindaCollidingSectorList
        add esp, 4
        popad

        jmp RETURN_CWorld__FindObjectsKindaCollidingSectorList_SKIP
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RpClumpForAllAtomics
//
// Adds a nullptr check for the clump object pointer.
//
// >>> 0x749B70 | 8B 44 24 04 | mov  eax, [esp+arg_0]
// >>> 0x749B74 | 53          | push ebx
// >>> 0x749B75 | 55          | push ebp
//     0x749B76 | 56          | push esi
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_RpClumpForAllAtomics  0x749B70
#define HOOKSIZE_RpClumpForAllAtomics 6
static DWORD CONTINUE_RpClumpForAllAtomics = 0x749B76;

static void __declspec(naked) HOOK_RpClumpForAllAtomics()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp+4]            // RpClump* clump
        test    eax, eax
        jnz     continueAfterFixLocation
        retn

        continueAfterFixLocation:
        push    ebx
        push    ebp
        jmp     CONTINUE_RpClumpForAllAtomics
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// RpAnimBlendClumpGetFirstAssociation
//
// Adds a nullptr check for the clump object pointer.
//
// >>> 0x4D6A70 | 8B 0D 78 F8 B5 00 | mov ecx, ds:_ClumpOffset
//     0x4D6A76 | 8B 44 24 04       | mov eax, [esp+4]
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_RpAnimBlendClumpGetFirstAssociation  0x4D6A70
#define HOOKSIZE_RpAnimBlendClumpGetFirstAssociation 6
static DWORD CONTINUE_RpAnimBlendClumpGetFirstAssociation = 0x4D6A76;

static void __declspec(naked) HOOK_RpAnimBlendClumpGetFirstAssociation()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp+4]            // RpClump* clump
        test    eax, eax
        jnz     continueAfterFixLocation
        retn

        continueAfterFixLocation:
        mov     ecx, ds:[0xB5F878]
        jmp     CONTINUE_RpAnimBlendClumpGetFirstAssociation
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CAnimManager::BlendAnimation
//
// Adds a nullptr check for the clump object pointer.
//
// >>> 0x4D4610 | 83 EC 14          | sub esp, 14h
// >>> 0x4D4613 | 8B 4C 24 18       | mov ecx, [esp+18h]
//     0x4D4617 | 8B 15 34 EA B4 00 | mov edx, CAnimManager::ms_aAnimAssocGroups
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CAnimManager__BlendAnimation  0x4D4610
#define HOOKSIZE_CAnimManager__BlendAnimation 7
static DWORD CONTINUE_CAnimManager__BlendAnimation = 0x4D4617;

static void __declspec(naked) HOOK_CAnimManager__BlendAnimation()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        mov     eax, [esp+4]            // RpClump* clump
        test    eax, eax
        jnz     continueAfterFixLocation
        retn

        continueAfterFixLocation:
        sub     esp, 14h
        mov     ecx, [esp+18h]
        jmp     CONTINUE_CAnimManager__BlendAnimation
    }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// FxSystemBP_c::Load
//
// Remove every FxEmitter without a main texture because the game logic expects AT LEAST
// one texture at index 0 ("main texture").
//
//     0x5C0A14 | 5E                   | pop  esi
// >>> 0x5C0A15 | 5D                   | pop  ebp
// >>> 0x5C0A16 | 32 C0                | xor  al, al
// >>> 0x5C0A18 | 5B                   | pop  ebx
// >>> 0x5C0A19 | 64 89 0D 00 00 00 00 | mov  large fs:0, ecx
// >>> 0x5C0A20 | 81 C4 E8 05 00 00    | add  esp, 5E8h
// >>> 0x5C0A26 | C2 0C 00             | retn 0Ch
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_FxSystemBP_c__Load  0x5C0A15
#define HOOKSIZE_FxSystemBP_c__Load 19

static void _cdecl POST_PROCESS_FxSystemBP_c__Load(CFxSystemBPSAInterface* blueprint)
{
    if (!blueprint->cNumOfPrims)
        return;

    char count = blueprint->cNumOfPrims;
    char last = count - 1;

    for (char i = last; i >= 0; i--)
    {
        if (blueprint->pPrims[i]->m_apTextures[0])
            continue;

        blueprint->pPrims[i] = nullptr;
        --count;

        if (i != last)
        {
            std::swap(blueprint->pPrims[i], blueprint->pPrims[last]);
            --last;
        }
    }

    if (blueprint->cNumOfPrims != count)
    {
        OnCrashAverted(33);
        blueprint->cNumOfPrims = count;
    }
}

static void __declspec(naked) HOOK_FxSystemBP_c__Load()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
        {
        pushad
        push    ebp
        call    POST_PROCESS_FxSystemBP_c__Load
        add     esp, 4
        popad

        pop     ebp
        xor     al, al
        pop     ebx
             //  mov     large fs:0, ecx
        add     esp, 5E8h
        retn    0Ch
        }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// FxPrim_c::Enable
//
// Add a null-pointer check for the ecx pointer. This hook is a side-effect of the hook for
// FxSystemBP_c::Load above.
//
// >>> 0x4A9F50 | 8A 44 24 04 | mov  al, [esp+4]
// >>> 0x4A9F54 | 88 41 0C    | mov  [ecx+0xC], al
// >>> 0x4A9F57 | C2 04 00    | retn 4
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_FxPrim_c__Enable  0x4A9F50
#define HOOKSIZE_FxPrim_c__Enable 10

static void __declspec(naked) HOOK_FxPrim_c__Enable()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
        {
        test    ecx, ecx
        jz      returnFromFunction
        mov     al, [esp+4]
        mov     [ecx+0xC], al

        returnFromFunction:
        retn    4
        }
    // clang-format on
}

////////////////////////////////////////////////////////////////////////
// CFire::ProcessFire
//
// GitHub #1757 (https://github.com/multitheftauto/mtasa-blue/issues/1757)
//
// Null pointer to the attachedTo field in the CFire structure
////////////////////////////////////////////////////////////////////////
#define HOOKPOS_CFire_ProcessFire  0x53A6FC
#define HOOKSIZE_CFire_ProcessFire 9
static constexpr DWORD        CONTINUE_CFire_ProcessFire = 0x53A705;
static constexpr DWORD        SKIP_CFire_ProcessFire = 0x53A69C;
static void __declspec(naked) HOOK_CFire_ProcessFire()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        test byte ptr [esi], 1            // If the "active" flag has been set to 0, we skip processing attached entities
        jz skip

        mov ecx, [esi+10h]
        mov eax, [ecx+590h]
        jmp CONTINUE_CFire_ProcessFire

        skip:
        mov ecx, esi
        jmp SKIP_CFire_ProcessFire
    }
    // clang-format on
}

//////////////////////////////////////////////////////////////////////////////////////////
//
// Setup hooks for CrashFixHacks
//
////////////////////////////////////////////////////////////////////////
void CMultiplayerSA::InitHooks_CrashFixHacks()
{
    EZHookInstall(CrashFix_Misc1);
    EZHookInstall(CrashFix_Misc2);
    EZHookInstall(CrashFix_Misc4);
    EZHookInstall(CrashFix_Misc5);
    EZHookInstall(CrashFix_Misc6);
    EZHookInstall(CrashFix_Misc7);
    EZHookInstall(CrashFix_Misc8);
    EZHookInstall(CrashFix_Misc9);
    EZHookInstall(CrashFix_Misc10);
    EZHookInstall(CrashFix_Misc11);
    // EZHookInstall ( CrashFix_Misc12 );
    EZHookInstall(CrashFix_Misc13);
    EZHookInstall(CrashFix_Misc14);
    EZHookInstall(FreezeFix_Misc15);
    EZHookInstall(CrashFix_Misc16);
    // EZHookInstall ( CrashFix_Misc17 );
    EZHookInstall(CrashFix_Misc18);
    // EZHookInstall ( CrashFix_Misc19 );
    EZHookInstall(CrashFix_Misc20);
    EZHookInstall(CrashFix_Misc21);
    EZHookInstall(CrashFix_Misc22);
    EZHookInstall(CrashFix_Misc23);
    EZHookInstall(CrashFix_Misc24);
    EZHookInstall(CrashFix_Misc25);
    EZHookInstall(CrashFix_Misc26);
    EZHookInstall(CrashFix_Misc27);
    EZHookInstall(CrashFix_Misc28);
    EZHookInstall(CrashFix_Misc29);
    EZHookInstallChecked(CrashFix_Misc30);
    EZHookInstall(CrashFix_Misc32);
    EZHookInstall(CrashFix_Misc33);
    EZHookInstall(CrashFix_Misc34);
    EZHookInstallChecked(CrashFix_Misc35);
    EZHookInstallChecked(CrashFix_Misc36);
    EZHookInstallChecked(CrashFix_Misc37);
    EZHookInstallChecked(CrashFix_Misc38);
    EZHookInstallChecked(CrashFix_VBInstV3dNull);
    EZHookInstallChecked(CrashFix_VBInstV3dMorphNull);
    EZHookInstall(CrashFix_Misc39);
    EZHookInstall(CClumpModelInfo_GetFrameFromId);
    EZHookInstallChecked(CEntity_GetBoundRect);
    EZHookInstallChecked(CVehicle_AddUpgrade);
    EZHookInstall(ResetFurnitureObjectCounter);
    EZHookInstallChecked(CVolumetricShadowMgr_Render);
    EZHookInstallChecked(CVolumetricShadowMgr_Update);
    EZHookInstallChecked(CAnimManager_CreateAnimAssocGroups);
    EZHookInstall(CAnimBlendNode_GetCurrentTranslation);
    EZHookInstall(CStreaming_AreAnimsUsedByRequestedModels);
    EZHookInstall(CTaskComplexCarSlowBeDraggedOut_CreateFirstSubTask);
    EZHookInstallChecked(printf);
    EZHookInstallChecked(RwMatrixMultiply);
    EZHookInstall(CTrain__ProcessControl);
    EZHookInstall(CTaskComplexCarSlowBeDraggedOutAndStandUp__CreateFirstSubTask);
    EZHookInstall(CVehicleModelInfo__LoadVehicleColours_1);
    EZHookInstall(CVehicleModelInfo__LoadVehicleColours_2);
    EZHookInstall(CPlaceName__Process);
    EZHookInstall(CWorld__FindObjectsKindaCollidingSectorList);
    EZHookInstall(RpClumpForAllAtomics);
    EZHookInstall(RpAnimBlendClumpGetFirstAssociation);
    EZHookInstall(CAnimManager__BlendAnimation);
    EZHookInstall(FxSystemBP_c__Load);
    EZHookInstall(FxPrim_c__Enable);
    EZHookInstall(CFire_ProcessFire);

    // Install train crossing crashfix (the temporary variable is required for the template logic)
    void (*temp)() = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_Destructor_TrainCrossing_Check, RETURN_CObject_Destructor_TrainCrossing_Invalid>;
    HookInstall(HOOKPOS_CObject_Destructor_TrainCrossing_Check, (DWORD)temp, 5);
    temp = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_ProcessTrainCrossingBehavior1_Check, RETURN_CObject_ProcessTrainCrossingBehavior_Invalid>;
    HookInstall(HOOKPOS_CObject_ProcessTrainCrossingBehavior1, (DWORD)temp, 5);
    temp = HOOK_TrainCrossingBarrierCrashFix<RETURN_CObject_ProcessTrainCrossingBehavior2_Check, RETURN_CObject_ProcessTrainCrossingBehavior_Invalid>;
    HookInstall(HOOKPOS_CObject_ProcessTrainCrossingBehavior2, (DWORD)temp, 5);
}
