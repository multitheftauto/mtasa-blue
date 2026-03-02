/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_ObjectLODSystem.cpp
 *  PORPOISE:    System for displaying low LOD objects
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

////////////////////////////////////////////////
//
// CRenderer_SetupEntityVisibility helpers
//
////////////////////////////////////////////////
namespace
{
// #define     ARRAY_ModelInfo                 0xA9B0C8
#define LOW_LOD_DRAW_DISTANCE_SCALE 5

    void SetGlobalDrawDistanceScale(float fValue)
    {
        MemPut<float>(0x858FD8, 300.f * fValue);
    }

    float GetDrawDistanceSetting()
    {
        return *(float*)0xB6F118;
    }

    float CalculateLowLodFadeAlpha(CBaseModelInfoSAInterface* pModelInfo, float fDist)
    {
        float fModelDrawDistance = pModelInfo->fLodDistanceUnscaled * LOW_LOD_DRAW_DISTANCE_SCALE + 20.f;
        float fFadeWidth = 20.f + fModelDrawDistance / 50.f;
        float fFadeFar = fModelDrawDistance;
        float fFadeNear = fFadeFar - fFadeWidth;
        return UnlerpClamped(fFadeFar, fDist, fFadeNear);
    }

    struct
    {
        bool                       bValid;
        float                      fLodDistanceUnscaled;
        CBaseModelInfoSAInterface* pModelInfo;
    } saved = {false, 0.f, NULL};

    DWORD preResult = 0;
    DWORD entityCheckValid = 0;
}  // namespace

////////////////////////////////////////////////
//
// CRenderer_SetupEntityVisibility
//
////////////////////////////////////////////////
// Returns -1 to proceed with the original function, or >= 0 to skip it (value used as result)
int OnMY_CRenderer_SetupEntityVisibility_Pre(CEntitySAInterface* pEntity, float& fValue)
{
    // Guard against entities whose model info has been deallocated
    auto* pModelInfo = ((CBaseModelInfoSAInterface**)ARRAY_ModelInfo)[pEntity->m_nModelIndex];
    if (!pModelInfo)
    {
        saved.bValid = false;
        return 0xFF;  // Skip: caller's switch-default ignores the entity
    }

    if (pEntity->IsLowLodEntity())
    {
        SetGlobalDrawDistanceScale(LOW_LOD_DRAW_DISTANCE_SCALE * 2);
        saved.pModelInfo = pModelInfo;
        saved.fLodDistanceUnscaled = pModelInfo->fLodDistanceUnscaled;
        pModelInfo->fLodDistanceUnscaled *= LOW_LOD_DRAW_DISTANCE_SCALE / GetDrawDistanceSetting();
        saved.bValid = true;
    }
    else
        saved.bValid = false;

    return -1;  // Proceed with the original function
}

void OnMY_CRenderer_SetupEntityVisibility_Post(int result, CEntitySAInterface* pEntity, float& fDist)
{
    if (saved.bValid)
    {
        SetGlobalDrawDistanceScale(1);
        saved.pModelInfo->fLodDistanceUnscaled = saved.fLodDistanceUnscaled;
        saved.bValid = false;

        // Doing any distance fading?
        if (result == 0 && CalculateLowLodFadeAlpha(saved.pModelInfo, fDist) < 1.0f)
            pEntity->bDistanceFade = 1;
        else
            pEntity->bDistanceFade = 0;

        pEntity->SetEntityVisibilityResult(result);
    }
    else if (pEntity->IsHighLodEntity())
    {
        pEntity->SetEntityVisibilityResult(result);
    }
}

// Hook info
#define HOOKPOS_CRenderer_SetupEntityVisibility  0x554230
#define HOOKSIZE_CRenderer_SetupEntityVisibility 8
DWORD                         RETURN_CRenderer_SetupEntityVisibility = 0x554238;
static void __declspec(naked) HOOK_CRenderer_SetupEntityVisibility()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
////////////////////
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        call    OnMY_CRenderer_SetupEntityVisibility_Pre
        add     esp, 4*2
        mov     preResult, eax
        popad
        mov     eax, preResult
        cmp     eax, 0FFFFFFFFh
        jne     skip_original       // Pre signaled skip - eax already holds the result

////////////////////
        push    [esp+4*2]
        push    [esp+4*2]
        call    second
        add     esp, 4*2

////////////////////
skip_original:
        pushad
        push    [esp+32+4*2]
        push    [esp+32+4*2]
        push    eax
        call    OnMY_CRenderer_SetupEntityVisibility_Post
        add     esp, 4*2+4
        popad

        retn

second:
        sub     esp,14h
        push    esi
        mov     esi,dword ptr [esp+1Ch]
        jmp     RETURN_CRenderer_SetupEntityVisibility   // 0x554238
    }
    // clang-format on
}

////////////////////////////////////////////////
//
// CWorldScan_ScanWorld
//
////////////////////////////////////////////////
void OnMY_CWorldScan_ScanWorld_Pre(CVector2D* pVec, int iValue, void(__cdecl* func)(int, int))
{
    // Scale scan area
    const CVector2D& vecOrigin = pVec[0];

    for (uint i = 1; i < 5; i++)
    {
        CVector2D vecDiff = pVec[i] - vecOrigin;
        vecDiff *= LOW_LOD_DRAW_DISTANCE_SCALE * 2;
        pVec[i] = vecDiff + vecOrigin;
    }
}

// Hook info
#define HOOKPOS_CWorldScan_ScanWorld  0x55555E
#define HOOKSIZE_CWorldScan_ScanWorld 5
DWORD                         RETURN_CWorldScan_ScanWorlda = 0x555563;
DWORD                         RETURN_CWorldScan_ScanWorldb = 0x72CAE0;
static void __declspec(naked) HOOK_CWorldScan_ScanWorld()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        call    second
        jmp     RETURN_CWorldScan_ScanWorlda   // 555563

second:
        pushad
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        call    OnMY_CWorldScan_ScanWorld_Pre
        add     esp, 4*3
        popad

        jmp     RETURN_CWorldScan_ScanWorldb   // 72CAE0
    }
    // clang-format on
}

////////////////////////////////////////////////
//
// CRenderer_ScanSectorList_EntityCheck
//
// Validates entity pointer before accessing its scan code field.
// The LOD system's 10x expanded scan area can visit sectors containing
// dangling entity pointers (freed but not unlinked from sector lists).
//
////////////////////////////////////////////////
bool IsEntityAccessible(CEntitySAInterface* pEntity)
{
    if (!pEntity)
        return false;

    // GTA SA pool header layout: [B* m_pObjects][BYTE* m_byteMap][int m_nSize]
    // tPoolObjectFlags: bEmpty is bit 7 (MSB) of each byte. Zero MSB = occupied.
    struct RawPool
    {
        BYTE* m_pObjects;
        BYTE* m_byteMap;
        int   m_nSize;
    };

    // CRenderer_ScanSectorList visits sector lists that can contain entities from
    // all five gameplay pools: buildings, dummies, objects, peds, and vehicles.
    // Strides: sizeof(CBuildingSAInterface)=56, sizeof(CEntitySAInterface)=56,
    //          PoolAllocStride<CObjectSAInterface>::value=412,
    //          PoolAllocStride<CPedSAInterface>::value=1988,
    //          PoolAllocStride<CVehicleSAInterface>::value=2584.
    static const struct
    {
        DWORD ppPool;
        DWORD stride;
    } pools[] = {
        {0xb74498, 56},    // *CLASS_CBuildingPool
        {0xb744a0, 56},    // *CLASS_CDummyPool
        {0xb7449c, 412},   // *CLASS_CObjectPool
        {0xb74490, 1988},  // *CLASS_CPedPool
        {0xb74494, 2584},  // *CLASS_CVehiclePool
    };

    const BYTE* ent = reinterpret_cast<const BYTE*>(pEntity);

    for (int i = 0; i < 5; ++i)
    {
        const RawPool* pPool = *reinterpret_cast<RawPool* const*>(pools[i].ppPool);
        if (!pPool || !pPool->m_pObjects || pPool->m_nSize <= 0)
            continue;

        if (ent < pPool->m_pObjects)
            continue;

        const DWORD stride = pools[i].stride;
        const DWORD diff = static_cast<DWORD>(ent - pPool->m_pObjects);
        if (diff % stride != 0)
            continue;

        const DWORD index = diff / stride;
        if (index >= static_cast<DWORD>(pPool->m_nSize))
            continue;

        if ((pPool->m_byteMap[index] & 0x80) == 0)  // bEmpty clear (bit 7 = 0) = slot occupied
            return true;
    }

    return false;
}

// Hook info
#define HOOKPOS_CRenderer_ScanSectorList_EntityCheck  0x554910
#define HOOKSIZE_CRenderer_ScanSectorList_EntityCheck 8
DWORD                        RETURN_CRenderer_ScanSectorList_EntityCheck = 0x554918;
DWORD                        LOOPBOTTOM_CRenderer_ScanSectorList_EntityCheck = 0x554AD5;
static void _declspec(naked) HOOK_CRenderer_ScanSectorList_EntityCheck()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
    check_entity:
        test    edi, edi
        jz      list_done

        mov     esi, [edi]              // entity = node->data

        pushad
        push    esi
        call    IsEntityAccessible
        add     esp, 4
        mov     entityCheckValid, eax
        popad

        cmp     entityCheckValid, 0
        je      skip_entity

        // Valid entity: replicate original 'mov ax, word ptr [CWorld::ms_nCurrentScanCode]'
        mov     ax, word ptr ds:[0xB7CD78]
        jmp     RETURN_CRenderer_ScanSectorList_EntityCheck     // -> 0x554918

    skip_entity:
        mov     edi, [edi+4]            // advance to next node
        jmp     check_entity            // retry with next entity

    list_done:
        jmp     LOOPBOTTOM_CRenderer_ScanSectorList_EntityCheck // -> 0x554AD5 (def_554948: test edi,edi; jnz)
    }
    // clang-format on
}

////////////////////////////////////////////////
//
// CVisibilityPlugins_CalculateFadingAtomicAlpha
//
////////////////////////////////////////////////
int OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre(CBaseModelInfoSAInterface* pModelInfo, CEntitySAInterface* pEntity, float fDist)
{
    if (pEntity->IsLowLodEntity())
    {
        // Apply custom distance fade
        float fAlpha = CalculateLowLodFadeAlpha(pModelInfo, fDist);
        fAlpha *= pModelInfo->ucAlpha;
        return Clamp<int>(0, (int)fAlpha, 255);
    }
    return -1;
}

// Hook info
#define HOOKPOS_CVisibilityPlugins_CalculateFadingAtomicAlpha  0x732500
#define HOOKSIZE_CVisibilityPlugins_CalculateFadingAtomicAlpha 5
DWORD                         RETURN_CVisibilityPlugins_CalculateFadingAtomicAlpha = 0x732505;
static void __declspec(naked) HOOK_CVisibilityPlugins_CalculateFadingAtomicAlpha()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        pushad
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        push    [esp+32+4*3]
        call    OnMY_CVisibilityPlugins_CalculateFadingAtomicAlpha_Pre
        mov     [esp+0],eax
        add     esp, 4*3
        popad
        mov     eax,[esp-32-4*3]

        cmp     eax, 0xffffffff
        jne     usecustom

        // Use standard code
        push    [esp+4*3]
        push    [esp+4*3]
        push    [esp+4*3]
        call    second
        add     esp, 4*3

usecustom:
        retn

second:
        push    ecx
        mov     ecx, [esp+8]
        jmp     RETURN_CVisibilityPlugins_CalculateFadingAtomicAlpha
    }
    // clang-format on
}

////////////////////////////////////////////////
//
// CMultiplayerSA::EnableLODSystem
//
// Enable/disable hooks programmatically
//
////////////////////////////////////////////////
void CMultiplayerSA::SetLODSystemEnabled(bool bEnable)
{
    // Switch
    if (m_bEnabledLODSystem == bEnable)
        return;
    m_bEnabledLODSystem = bEnable;

    // Memory saved here
    static CBuffer savedMem;
    SHookInfo      hookInfoList[] = {MAKE_HOOK_INFO(CRenderer_SetupEntityVisibility), MAKE_HOOK_INFO(CWorldScan_ScanWorld),
                                     MAKE_HOOK_INFO(CRenderer_ScanSectorList_EntityCheck), MAKE_HOOK_INFO(CVisibilityPlugins_CalculateFadingAtomicAlpha)};

    // Enable or not?
    if (bEnable)
    {
        CBufferWriteStream stream(savedMem);
        for (uint i = 0; i < NUMELMS(hookInfoList); i++)
        {
            const SHookInfo& hookInfo = hookInfoList[i];
            // Save memory before we blat it
            stream.WriteBytes((void*)hookInfo.dwAddress, hookInfo.uiSize);
            // Add hook
            HookInstall(hookInfo.dwAddress, hookInfo.dwHook, hookInfo.uiSize);
        }
    }
    else
    {
        // Restore memory
        CBufferReadStream stream(savedMem);
        for (uint i = 0; i < NUMELMS(hookInfoList); i++)
        {
            const SHookInfo& hookInfo = hookInfoList[i];
            BYTE             temp[10];
            assert(sizeof(temp) >= hookInfo.uiSize);
            stream.ReadBytes(temp, hookInfo.uiSize);
            MemCpy((void*)hookInfo.dwAddress, temp, hookInfo.uiSize);
        }
    }
}
