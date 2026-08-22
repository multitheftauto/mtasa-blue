/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaHookGuard.cpp
 *  PURPOSE:     Integrity guard against inline hooks on the Lua script loader
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CLuaHookGuard.h"
#include "CLuaMain.h"

extern CClientGame* g_pClientGame;

#define LUA_HOOKGUARD_REPORT_ID 1006

CLuaHookGuard::SGuardedFunction CLuaHookGuard::ms_GuardedFunctions[] = {
    {"CLuaMain::LuaLoadBuffer", reinterpret_cast<void*>(&CLuaMain::LuaLoadBuffer), {}, false},
};
std::atomic<bool> CLuaHookGuard::ms_bInitialized{false};
std::atomic<bool> CLuaHookGuard::ms_bTamperReported{false};

///////////////////////////////////////////////////////////////
//
// CLuaHookGuard::IsPrologueHooked
//
// Heuristic for hook engine prologues (relative/near jump or
// push+ret). A legitimate incremental link thunk also starts
// with a jmp, but it targets a location inside our own module,
// while foreign hooks transfer control to memory allocated
// outside of our image. Anything jumping outside is hostile.
//
///////////////////////////////////////////////////////////////
bool CLuaHookGuard::IsPrologueHooked(const SGuardedFunction& func)
{
    const unsigned char* pBytes = reinterpret_cast<const unsigned char*>(func.pCode);
    const size_t         uiSize = sizeof(func.aReference);

    // Resolve the module range that contains the guarded function
    HMODULE hModule = NULL;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCSTR>(func.pCode),
                            &hModule))
        return true;  // Cannot verify, assume the worst

    const IMAGE_DOS_HEADER* pDosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(hModule);
    if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return true;

    const IMAGE_NT_HEADERS* pNtHeader = reinterpret_cast<const IMAGE_NT_HEADERS*>(reinterpret_cast<const BYTE*>(hModule) + pDosHeader->e_lfanew);
    const uintptr_t         uiImageStart = reinterpret_cast<const uintptr_t>(hModule);
    const uintptr_t         uiImageEnd = uiImageStart + pNtHeader->OptionalHeader.SizeOfImage;

    auto IsInsideOwnModule = [&](const void* pTarget)
    {
        const uintptr_t uiTarget = reinterpret_cast<uintptr_t>(pTarget);
        return uiTarget >= uiImageStart && uiTarget < uiImageEnd;
    };

    if (uiSize >= 5 && pBytes[0] == 0xE9)
    {
        // jmp rel32
        const void* pTarget = pBytes + 5 + *reinterpret_cast<const int32_t*>(pBytes + 1);
        return !IsInsideOwnModule(pTarget);
    }

    if (uiSize >= 6 && pBytes[0] == 0x68 && pBytes[5] == 0xC3)
    {
        // push imm32; ret
        const void* pTarget = *reinterpret_cast<void* const*>(pBytes + 1);
        return !IsInsideOwnModule(pTarget);
    }

    if (uiSize >= 6 && pBytes[0] == 0xFF && pBytes[1] == 0x25)
    {
        // jmp [disp32]; hook engines emit it with zero displacement so the
        // absolute target address is stored right after the instruction
        const void* pTarget = *reinterpret_cast<void* const*>(pBytes + 6);
        return !IsInsideOwnModule(pTarget);
    }

    if (uiSize >= 2 && pBytes[0] == 0xEB)
    {
        // jmp rel8 into unowned territory is suspicious even for short jumps
        const void* pTarget = pBytes + 2 + static_cast<signed char>(pBytes[1]);
        return !IsInsideOwnModule(pTarget);
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CLuaHookGuard::Initialize
//
// Snapshot the prologue bytes of every guarded function.
//
///////////////////////////////////////////////////////////////
void CLuaHookGuard::Initialize()
{
    for (SGuardedFunction& func : ms_GuardedFunctions)
    {
        if (!func.pCode)
            continue;

        memcpy(func.aReference, func.pCode, sizeof(func.aReference));
        func.bValid = !IsPrologueHooked(func);

        if (!func.bValid)
            AddReportLog(3400 + LUA_HOOKGUARD_REPORT_ID, SString("Hook guard detected patched prologue of %s during startup", func.szName), 10);
    }
    ms_bInitialized = true;
}

///////////////////////////////////////////////////////////////
//
// CLuaHookGuard::IsIntact
//
///////////////////////////////////////////////////////////////
bool CLuaHookGuard::IsIntact()
{
    if (!ms_bInitialized.load(std::memory_order_acquire))
        return true;

    for (const SGuardedFunction& func : ms_GuardedFunctions)
    {
        if (!func.pCode || !func.bValid)
            continue;

        if (memcmp(func.aReference, func.pCode, sizeof(func.aReference)) != 0)
            return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////
//
// CLuaHookGuard::VerifyAndReport
//
// Returns false when script data must not be handed to the
// loader. Reports tampering exactly once per session so the
// server can act on it without being spammed.
//
///////////////////////////////////////////////////////////////
bool CLuaHookGuard::VerifyAndReport(uint uiReportId, const SString& strContext)
{
    if (IsIntact())
        return true;

    if (!ms_bTamperReported.exchange(true))
    {
        SString strMessage("CLIENT TAMPERING: Lua loader integrity check failed (%s). Script loading is disabled.", *strContext);
        if (g_pClientGame)
            g_pClientGame->TellServerSomethingImportant(uiReportId, strMessage);
        else
            AddReportLog(3400 + uiReportId, strMessage, 10);
    }
    return false;
}
