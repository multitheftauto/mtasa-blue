/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CPathFindSA.cpp
 *  PURPOSE:     Path find (CPathFind) hooks
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CPathFindSA.h"

// CPathFind's per map area path node loader calls malloc() for m_pPathNodes[area] (offset
// 0x804) without checking the result, then loops over it using m_dwNumNodes[area] (offset
// 0xfa4) as the count. If the allocation fails, that loop dereferences a null pointer and
// crashes.
//
// Right where the node count gets loaded for the loop's bound check, this hook also checks
// whether m_pPathNodes[area] is null, and if so forces the count to 0 so the loop is skipped.
#define HOOKPOS_CPathFind_LoadPathNodeCount_Mid  0x0156F966
#define HOOKSIZE_CPathFind_LoadPathNodeCount_Mid 7
DWORD                         RETURN_CPathFind_LoadPathNodeCount_Mid = 0x0156F96D;
static void __declspec(naked) HOOK_CPathFind_LoadPathNodeCount_Mid()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Replicate the overwritten instruction: EAX = m_dwNumNodes[area]
        mov     eax, dword ptr [esi + edi*4 + 0x0fa4]

        // If m_pPathNodes[area] failed to allocate, force the node count to 0 for this loop
        cmp     dword ptr [esi + edi*4 + 0x804], 0
        jne     nodesOk
        xor     eax, eax
        nodesOk:
        jmp     RETURN_CPathFind_LoadPathNodeCount_Mid
    }
    // clang-format on
}

// This address is further into the executable than IsSlowMem recognizes, so the normal
// EZHookInstall path would assert in Debug builds. Unprotected up front in
// SetInitialVirtualProtect and installed via the Fast path instead, like the other hot addresses.
void CPathFindSA::StaticSetHooks()
{
    BYTE jumpBytes[MAX_JUMPCODE_SIZE];
    MemSetFast(jumpBytes, 0x90, MAX_JUMPCODE_SIZE);
    CreateJump(HOOKPOS_CPathFind_LoadPathNodeCount_Mid, (DWORD)HOOK_CPathFind_LoadPathNodeCount_Mid, jumpBytes);
    MemCpyFast((PVOID)HOOKPOS_CPathFind_LoadPathNodeCount_Mid, jumpBytes, HOOKSIZE_CPathFind_LoadPathNodeCount_Mid);
}
