/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        ceflauncher/Main.cpp
 *  PURPOSE:     CEF launcher entry point
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

// #define CEF_ENABLE_SANDBOX

#include <atomic>

#define WIN32_NO_STATUS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winternl.h>
#include <delayimp.h>

#include "CCefApp.h"
#include "SharedUtil.h"

#ifdef CEF_ENABLE_SANDBOX
#include <cef3/cef/include/cef_sandbox_win.h>
#pragma comment(lib, "cef_sandbox.lib")
#endif

// Return codes
inline constexpr int CEF_INIT_SUCCESS                = 0;
inline constexpr int CEF_INIT_ERROR_NO_BASE_DIR      = -1;
inline constexpr int CEF_INIT_ERROR_DLL_LOAD_FAILED  = -2;

inline constexpr DWORD       CEF_PARENT_CHECK_INTERVAL       = 1000;
inline constexpr const char* CEF_DLL_NAME                    = "libcef.dll";
inline constexpr const char* CEF_MTA_SUBDIR                  = "MTA";

inline constexpr DWORD PARENT_CHECK_ERROR_NO_QUERY_FUNC    = 1;
inline constexpr DWORD PARENT_CHECK_ERROR_QUERY_FAILED     = 2;
inline constexpr DWORD PARENT_CHECK_ERROR_OPEN_FAILED      = 3;

using NtQueryInformationProcessFunc = NTSTATUS(NTAPI*)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

// Safe parent monitor thread shutdown
std::atomic<bool> g_bShouldTerminateMonitor{false};
std::atomic<HANDLE> g_hMonitorThread{nullptr};

namespace
{
    [[nodiscard]] auto GetNtQueryInformationProcess() noexcept -> NtQueryInformationProcessFunc
    {
        const auto ntdll = GetModuleHandleW(L"ntdll.dll");
        if (!ntdll)
            return nullptr;
            
        const auto procAddr = GetProcAddress(ntdll, "NtQueryInformationProcess");
        if (!procAddr)
            return nullptr;
            
        return reinterpret_cast<NtQueryInformationProcessFunc>(procAddr);
    }

    [[nodiscard]] auto GetParentProcessId(NtQueryInformationProcessFunc queryFunc) noexcept -> DWORD
    {
        PROCESS_BASIC_INFORMATION info{};
        ULONG returnLength = 0;
        
        if (const auto status = queryFunc(GetCurrentProcess(), ProcessBasicInformation, &info, sizeof(info), &returnLength);
            !NT_SUCCESS(status) || returnLength < sizeof(PROCESS_BASIC_INFORMATION))
        {
            return 0;
        }

        return static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(info.Reserved3));
    }

    void MonitorParentProcess(HANDLE parentProcess) noexcept
    {
        while (!g_bShouldTerminateMonitor.load(std::memory_order_acquire))
        {
            const DWORD result = WaitForSingleObject(parentProcess, CEF_PARENT_CHECK_INTERVAL);
            
            if (result == WAIT_OBJECT_0)
            {
                DWORD exitCode = 0;
                if (GetExitCodeProcess(parentProcess, &exitCode))
                    ExitProcess(exitCode);
                else
                    ExitProcess(0);
            }
            else if (result == WAIT_FAILED)
            {
                // Wine/Proton compatibility: Exit thread instead of terminating process
                // Wine's handle implementation dont support all wait operations reliably
                break;
            }
        }
    }
}            // namespace

DWORD WINAPI CheckParentProcessAliveness(LPVOID) noexcept;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, [[maybe_unused]] LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        g_bShouldTerminateMonitor.store(false, std::memory_order_relaxed);
        g_hMonitorThread.store(nullptr, std::memory_order_relaxed);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
        g_bShouldTerminateMonitor.store(true, std::memory_order_release);
    }
    
    return TRUE;
}

extern "C" [[nodiscard]] __declspec(dllexport) auto InitCEF() noexcept -> int
{
    const auto baseDir = SharedUtil::GetMTAProcessBaseDir();
    if (baseDir.empty())
        return CEF_INIT_ERROR_NO_BASE_DIR;
    
    const auto mtaDir = SharedUtil::PathJoin(baseDir, CEF_MTA_SUBDIR);
    SetDllDirectoryW(SharedUtil::FromUTF8(mtaDir));

    if (FAILED(__HrLoadAllImportsForDll(CEF_DLL_NAME)))
        return CEF_INIT_ERROR_DLL_LOAD_FAILED;

    const CefMainArgs mainArgs(GetModuleHandleW(nullptr));
    const CefRefPtr<CCefApp> app{new CCefApp};

    void* sandboxInfo = nullptr;
#ifdef CEF_ENABLE_SANDBOX
    const CefScopedSandboxInfo scopedSandbox;
    sandboxInfo = scopedSandbox.sandbox_info();
#endif

    const auto hThread = CreateThread(nullptr, 0, CheckParentProcessAliveness, nullptr, 0, nullptr);
    if (hThread)
        g_hMonitorThread.store(hThread, std::memory_order_release);

    return CefExecuteProcess(mainArgs, app, sandboxInfo);
}

static auto WINAPI CheckParentProcessAliveness([[maybe_unused]] LPVOID) noexcept -> DWORD
{
    const auto queryFunc = GetNtQueryInformationProcess();
    if (!queryFunc)
        return PARENT_CHECK_ERROR_NO_QUERY_FUNC;

    const auto parentProcessId = GetParentProcessId(queryFunc);
    if (parentProcessId == 0)
        return PARENT_CHECK_ERROR_QUERY_FAILED;

    const auto parentProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, false, parentProcessId);
    if (!parentProcess)
    {
        // Wine/Proton fallback: PROCESS_QUERY_LIMITED_INFORMATION may not be implemented
        const auto parentProcessFallback = OpenProcess(SYNCHRONIZE, false, parentProcessId);
        if (parentProcessFallback)
        {
            MonitorParentProcess(parentProcessFallback);
            CloseHandle(parentProcessFallback);
            return CEF_INIT_SUCCESS;
        }
        return PARENT_CHECK_ERROR_OPEN_FAILED;
    }

    MonitorParentProcess(parentProcess);
    CloseHandle(parentProcess);
    return CEF_INIT_SUCCESS;
}
