/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/deathmatch/logic/lua/CLuaHookGuard.h
 *  PURPOSE:     Integrity guard against inline hooks on the Lua script loader
 *
 *  Third party modules may install inline hooks (MinHook, Detours, ...) on
 *  CLuaMain::LuaLoadBuffer to intercept deobfuscated script buffers and dump
 *  client side scripts. This guard takes a reference snapshot of the machine
 *  code prologue of guarded functions at startup and refuses to pass any
 *  script data to the loader while the code differs from the snapshot.
 *
 *****************************************************************************/

#pragma once

#include <atomic>

class CLuaHookGuard
{
public:
    // Capture reference bytes of all guarded functions.
    // Call once during client mod startup, before any script is loaded.
    static void Initialize();

    // True when every guarded function still matches its startup snapshot.
    static bool IsIntact();

    // IsIntact() plus a one shot diagnostic report to the current server when
    // tampering is detected. Returns false if script loading must be refused.
    static bool VerifyAndReport(uint uiReportId, const SString& strContext);

private:
    struct SGuardedFunction
    {
        const char*   szName;
        void*         pCode;
        unsigned char aReference[16];
        bool          bValid;
    };

    // Detects jump based prologue patches. The jump destination must stay
    // inside our own module, otherwise it is treated as a foreign hook.
    static bool IsPrologueHooked(const SGuardedFunction& func);

    static SGuardedFunction  ms_GuardedFunctions[];
    static std::atomic<bool> ms_bInitialized;
    static std::atomic<bool> ms_bTamperReported;
};
