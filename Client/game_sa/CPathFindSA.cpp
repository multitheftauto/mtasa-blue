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

// Test-only: deliberately reproduces the crash above by forcing every m_pPathNodes[area]
// allocation to look like it failed, the moment after it's stored. Off unless explicitly
// compiled in, so it can never affect a normal build. Used to confirm the crash happens without
// the fix above (undo HOOK_CPathFind_LoadPathNodeCount_Mid's install to test that) and is gone
// with it.
// #define MTA_DEBUG_REPRO_PATHFIND_NULL_CRASH

#ifdef MTA_DEBUG_REPRO_PATHFIND_NULL_CRASH
    #define HOOKPOS_CPathFind_ReproForceNull_Mid  0x0156F7F6
    #define HOOKSIZE_CPathFind_ReproForceNull_Mid 7
DWORD                         RETURN_CPathFind_ReproForceNull_Mid = 0x0156F7FD;
static void __declspec(naked) HOOK_CPathFind_ReproForceNull_Mid()
{
    MTA_VERIFY_HOOK_LOCAL_SIZE;

    // clang-format off
    __asm
    {
        // Replicate the overwritten instruction (unrelated field, must still run)
        mov     eax, dword ptr [esi + edi*4 + 0x1304]

        // Force the just-stored m_pPathNodes[area] to null, as if malloc() had failed
        mov     dword ptr [esi + edi*4 + 0x804], 0

        jmp     RETURN_CPathFind_ReproForceNull_Mid
    }
    // clang-format on
}
#endif

// Both hooks above live past 0x8A4000, past the range IsSlowMem normally covers, so they
// install through the Fast path instead of EZHookInstall (see SetInitialVirtualProtect).
void CPathFindSA::StaticSetHooks()
{
    BYTE jumpBytes[MAX_JUMPCODE_SIZE];
    MemSetFast(jumpBytes, 0x90, MAX_JUMPCODE_SIZE);
    CreateJump(HOOKPOS_CPathFind_LoadPathNodeCount_Mid, (DWORD)HOOK_CPathFind_LoadPathNodeCount_Mid, jumpBytes);
    MemCpyFast((PVOID)HOOKPOS_CPathFind_LoadPathNodeCount_Mid, jumpBytes, HOOKSIZE_CPathFind_LoadPathNodeCount_Mid);

#ifdef MTA_DEBUG_REPRO_PATHFIND_NULL_CRASH
    MemSetFast(jumpBytes, 0x90, MAX_JUMPCODE_SIZE);
    CreateJump(HOOKPOS_CPathFind_ReproForceNull_Mid, (DWORD)HOOK_CPathFind_ReproForceNull_Mid, jumpBytes);
    MemCpyFast((PVOID)HOOKPOS_CPathFind_ReproForceNull_Mid, jumpBytes, HOOKSIZE_CPathFind_ReproForceNull_Mid);
#endif
}
