/*
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/sdk/core/CrashHandlerExports.h
 *  PURPOSE:     Crash handler exports
 */

#pragma once

#include <windows.h>

#if !defined(BUGSUTIL_DLLINTERFACE)
    #if defined(BUGSUTIL_EXPORTS)
        #define BUGSUTIL_DLLINTERFACE __declspec(dllexport)
    #else
        #define BUGSUTIL_DLLINTERFACE
    #endif
#endif

#ifdef __cplusplus
    #define MTA_CH_NOEXCEPT noexcept
    #define MTA_CH_NODISCARD [[nodiscard]]
    extern "C"
    {
#else
    #define MTA_CH_NOEXCEPT
    #define MTA_CH_NODISCARD
#endif

MTA_CH_NODISCARD BOOL BUGSUTIL_DLLINTERFACE __stdcall EnableAllHandlersAfterInitialization(void) MTA_CH_NOEXCEPT;

#ifdef __cplusplus
MTA_CH_NODISCARD BOOL BUGSUTIL_DLLINTERFACE __stdcall StartWatchdogThread(DWORD mainThreadId, DWORD timeoutSeconds = 20) MTA_CH_NOEXCEPT;
#else
MTA_CH_NODISCARD BOOL BUGSUTIL_DLLINTERFACE __stdcall StartWatchdogThread(DWORD mainThreadId, DWORD timeoutSeconds) MTA_CH_NOEXCEPT;
#endif

void BUGSUTIL_DLLINTERFACE __stdcall StopWatchdogThread(void) MTA_CH_NOEXCEPT;
void BUGSUTIL_DLLINTERFACE __stdcall UpdateWatchdogHeartbeat(void) MTA_CH_NOEXCEPT;

#ifdef __cplusplus
    }
#endif

#undef MTA_CH_NOEXCEPT
#undef MTA_CH_NODISCARD
