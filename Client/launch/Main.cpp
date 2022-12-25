/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        launch/Main.cpp
 *  PURPOSE:     Unchanging .exe that doesn't change
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <ShellScalingApi.h>

/*
    IMPORTANT

    If this project changes, a new release build should be copied into
    the Shared\data\launchers directory.

    The .exe in Shared\data\launchers will be used by the installer and updater.

    (set flag.new_client_exe on the build server to generate new exe)
*/

/**
 * @brief Applies the highest available form of DPI awareness for this process.
 */
void ApplyDpiAwareness()
{
    // Minimum version: Windows 10, version 1607
    using SetProcessDpiAwarenessContext_t = BOOL(WINAPI*)(DPI_AWARENESS_CONTEXT value);

    static SetProcessDpiAwarenessContext_t Win32SetProcessDpiAwarenessContext = ([] {
        HMODULE user32 = LoadLibrary("user32");
        return user32 ? reinterpret_cast<SetProcessDpiAwarenessContext_t>(static_cast<void*>(GetProcAddress(user32, "SetProcessDpiAwarenessContext"))) : nullptr;
    })();

    if (Win32SetProcessDpiAwarenessContext)
    {
        Win32SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        return;
    }

    // Minimum version: Windows 8.1
    using SetProcessDpiAwareness_t = HRESULT(WINAPI*)(PROCESS_DPI_AWARENESS value);

    static SetProcessDpiAwareness_t Win32SetProcessDpiAwareness = ([] {
        HMODULE shcore = LoadLibrary("shcore");
        return shcore ? reinterpret_cast<SetProcessDpiAwareness_t>(static_cast<void*>(GetProcAddress(shcore, "SetProcessDpiAwareness"))) : nullptr;
    })();

    if (Win32SetProcessDpiAwareness)
    {
        Win32SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        return;
    }

    // Minimum version: Windows Vista
    SetProcessDPIAware();
}

///////////////////////////////////////////////////////////////
//
// WinMain
//
//
//
///////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (!IsWindowsXPSP3OrGreater())
    {
        BrowseToSolution("launch-xpsp3-check", ASK_GO_ONLINE, "This version of MTA requires Windows XP SP3 or later");
        return 1;
    }

    ApplyDpiAwareness();

    // Group our processes and windows under a single taskbar button
    SetCurrentProcessExplicitAppUserModelID(L"Multi Theft Auto");

    // Load the loader.dll and continue the load
#ifdef MTA_DEBUG
    SString strLoaderDllFilename = "loader_d.dll";
#else
    SString strLoaderDllFilename = "loader.dll";
#endif

    SString strMTASAPath = PathJoin(GetLaunchPath(), "mta");
    SString strLoaderDllPathFilename = PathJoin(strMTASAPath, strLoaderDllFilename);

    // No Windows error box during first load attempt
    DWORD   dwPrevMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    HMODULE hModule = LoadLibraryW(FromUTF8(strLoaderDllPathFilename));
    DWORD   dwLoadLibraryError = GetLastError();
    SetErrorMode(dwPrevMode);

    if (!hModule)
    {
        // Retry using MTA current directory
        SetCurrentDirectoryW(FromUTF8(strMTASAPath));
        hModule = LoadLibraryW(FromUTF8(strLoaderDllPathFilename));
        dwLoadLibraryError = GetLastError();
        if (hModule)
        {
            AddReportLog(5712, SString("LoadLibrary '%s' succeeded on change to directory '%s'", *strLoaderDllFilename, *strMTASAPath));
        }
    }

    int iReturnCode = 0;
    if (hModule)
    {
        // Find and call DoWinMain
        typedef int (*PFNDOWINMAIN)(HINSTANCE, HINSTANCE, LPSTR, int);
        PFNDOWINMAIN pfnDoWinMain = static_cast<PFNDOWINMAIN>(static_cast<PVOID>(GetProcAddress(hModule, "DoWinMain")));
        if (pfnDoWinMain)
            iReturnCode = pfnDoWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

        FreeLibrary(hModule);
    }
    else
    {
        iReturnCode = 1;
        SString strError = GetSystemErrorMessage(dwLoadLibraryError);
        SString strMessage("Failed to load: '%s'\n\n%s", *strLoaderDllPathFilename, *strError);
        AddReportLog(5711, strMessage);

        // Error could be due to missing VC Redist.
        // Online help page will have VC Redist download link.
        BrowseToSolution("loader-dll-not-loadable", ASK_GO_ONLINE, strMessage);
    }

    return iReturnCode;
}
