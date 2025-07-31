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

#define WIN32_NO_STATUS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_NO_STATUS
#include <ntstatus.h>
#include <winnt.h>
#include <winternl.h>
#include <delayimp.h>
#include "CCefApp.h"
#include <string>
#include <cef3/cef/include/cef_sandbox_win.h>

// #define CEF_ENABLE_SANDBOX
#ifdef CEF_ENABLE_SANDBOX
    #pragma comment(lib, "cef_sandbox.lib")
#endif

DWORD WINAPI CheckParentProcessAliveness(LPVOID);

int _declspec(dllexport) InitCEF()
{
    // Get absolute CEFLauncher.exe path
    TCHAR buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring currentFileName(buffer);

    // Extract MTA path and set DLL directory (absolute path is required here)
    size_t       pos = currentFileName.find_last_of(L'\\');
    std::wstring mtaPath = currentFileName.substr(0, pos - 3);            // Strip "CEF"
    SetDllDirectory(mtaPath.c_str());

    // Load libcef.dll from the DLL directory
    assert(SUCCEEDED(__HrLoadAllImportsForDll("libcef.dll")));

    // Load CEF
    CefMainArgs        mainArgs(GetModuleHandle(NULL));
    CefRefPtr<CCefApp> app{new CCefApp};

    void* sandboxInfo = nullptr;
#ifdef CEF_ENABLE_SANDBOX
    CefScopedSandboxInfo scopedSandbox;
    sandboxInfo = scopedSandbox.sandbox_info();
#endif

    const HANDLE parentCheckThread = CreateThread(nullptr, 0, CheckParentProcessAliveness, nullptr, 0, nullptr);

    const int exitCode = CefExecuteProcess(mainArgs, app, sandboxInfo);

    if (parentCheckThread != nullptr)
    {
        TerminateThread(parentCheckThread, 0);
        CloseHandle(parentCheckThread);
    }

    return exitCode;
}

static DWORD WINAPI CheckParentProcessAliveness(LPVOID)
{
    NTSTATUS(NTAPI * queryInformation)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG) = nullptr;

    if (HMODULE const ntdll = GetModuleHandleW(L"ntdll.dll"); ntdll != nullptr)
    {
        queryInformation = reinterpret_cast<decltype(queryInformation)>(GetProcAddress(ntdll, "NtQueryInformationProcess"));
    }

    if (queryInformation == nullptr)
        return 1;

    PROCESS_BASIC_INFORMATION info{};

    ULONG    returnLength = 0;
    NTSTATUS status = queryInformation(GetCurrentProcess(), ProcessBasicInformation, &info, sizeof(info), &returnLength);

    if (!NT_SUCCESS(status) || returnLength < sizeof(PROCESS_BASIC_INFORMATION))
        return 2;

    const auto   parentProcessId = static_cast<DWORD>(reinterpret_cast<ULONG_PTR>(info.Reserved3));
    const HANDLE parentProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, parentProcessId);

    if (parentProcess == nullptr)
    {
        if (GetLastError() == ERROR_INVALID_PARAMETER)
            ExitProcess(0);

        return 3;
    }

    while (true)
    {
        DWORD exitCode{};

        if (!GetExitCodeProcess(parentProcess, &exitCode) || exitCode != STILL_ACTIVE)
        {
            CloseHandle(parentProcess);
            ExitProcess(exitCode);
        }

        Sleep(1000);
    }

    return 0;
}
