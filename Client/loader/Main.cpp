/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Main.cpp
 *  PURPOSE:     MTA loader
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "Main.h"
#include "CInstallManager.h"
#include "MainFunctions.h"
#include "Dialogs.h"
#include "Utils.h"
#include "SharedUtil.Win32Utf8FileHooks.hpp"

#if defined(MTA_DEBUG)
    #include "SharedUtil.Tests.hpp"
#endif

#include <version.h>
#include <memory>
#include <algorithm>

namespace {
    // Constants
    constexpr size_t MAX_CMD_LINE_LENGTH = 4096;
    constexpr int ERROR_NULL_INSTANCE = -1;
    constexpr int ERROR_NULL_INSTALL_MANAGER = -2;
    constexpr int ERROR_LAUNCH_EXCEPTION = -3;
    constexpr int ERROR_INSTALL_CONTINUE = -4;
    
    // Report log IDs
    constexpr int LOG_ID_END = 1044;
    constexpr int LOG_ID_CONTINUE_EXCEPTION = 1045;
    constexpr int LOG_ID_LAUNCH_EXCEPTION = 1046;

    class Utf8FileHooksGuard {
    public:
        Utf8FileHooksGuard() { AddUtf8FileHooks(); }
        ~Utf8FileHooksGuard() { RemoveUtf8FileHooks(); }
        
        // Disable copy and move
        Utf8FileHooksGuard(const Utf8FileHooksGuard&) = delete;
        Utf8FileHooksGuard& operator=(const Utf8FileHooksGuard&) = delete;
        Utf8FileHooksGuard(Utf8FileHooksGuard&&) = delete;
        Utf8FileHooksGuard& operator=(Utf8FileHooksGuard&&) = delete;
    };

    inline void SafeCopyCommandLine(LPSTR lpCmdLine, char* safeCmdLine, size_t bufferSize) {
        if (!lpCmdLine || !safeCmdLine || bufferSize == 0) {
            return;
        }
        
        const size_t maxCopyLen = bufferSize - 1;
        const size_t cmdLineLen = strnlen(lpCmdLine, maxCopyLen);
        
        memcpy(safeCmdLine, lpCmdLine, cmdLineLen);
        safeCmdLine[cmdLineLen] = '\0';
    }


    inline DWORD GetSafeProcessId() noexcept {
        try {
            return GetCurrentProcessId();
        }
        catch (...) {
            return 0;
        }
    }

    bool PerformInitialization(const char* safeCmdLine) {
        auto* pInstallManager = GetInstallManager();
        if (!pInstallManager) {
            return false;
        }

        // Configure install manager
        pInstallManager->SetMTASAPathSource(safeCmdLine);
        
        // Initialize logging
        BeginEventLog();
        
        // Start localization (non-critical, continue on failure)
        InitLocalization(false);
        
        // Handle installer commands
        HandleSpecialLaunchOptions();
        
        // Ensure single instance
        HandleDuplicateLaunching();
        
        // Clear any pending operations
        ClearPendingBrowseToSolution();
        
        // Validate GTA installation
        ValidateGTAPath();
        
        return true;
    }

    void PerformPreLaunchSetup(HINSTANCE hInstance) {
        // Ensure localization is fully initialized
        InitLocalization(true);
        
        // Initialize monitoring systems
        PreLaunchWatchDogs();
        
        // Handle custom configurations
        HandleCustomStartMessage();
        
        #if !defined(MTA_DEBUG) && MTASA_VERSION_TYPE != VERSION_TYPE_CUSTOM
        ForbodenProgramsMessage();
        #endif
        
        // Maintenance operations
        CycleEventLog();
        BsodDetectionPreLaunch();
        MaybeShowCopySettingsDialog();
        
        // Check for conflicts
        HandleIfGTAIsAlreadyRunning();
		
		// Maybe warn user if no anti-virus running
        CheckAntiVirusStatus();
        
        // Show splash screen
        ShowSplash(hInstance);
        
        // Verify integrity
        CheckDataFiles();
        CheckLibVersions();
    }

    SString ContinueUpdateProcedure(CInstallManager* pInstallManager) {
        if (!pInstallManager) {
            return SString();
        }
        
        try {
            return pInstallManager->Continue();
        }
        catch (...) {
            AddReportLog(LOG_ID_CONTINUE_EXCEPTION, "Exception in InstallManager::Continue()");
            return SString();
        }
    }

    int LaunchGameSafely(const SString& strCmdLine) {
        try {
            return LaunchGame(strCmdLine);
        }
        catch (...) {
            AddReportLog(LOG_ID_LAUNCH_EXCEPTION, "Exception in LaunchGame()");
            return ERROR_LAUNCH_EXCEPTION;
        }
    }
}

///////////////////////////////////////////////////////////////
//
// DoWinMain
//
// 'MTA San Andreas.exe' is launched as a subprocess under the following circumstances:
//      1. During install with /kdinstall command (as admin)
//      2. During uninstall with /kduninstall command (as admin)
//      3. By 'MTA San Andreas.exe' when temporary elevated privileges are required (as admin)
//      4. By 'MTA San Andreas.exe' during auto-update (in a temporary directory somewhere)
//         (Which may then call it again as admin)
//
///////////////////////////////////////////////////////////////
MTAEXPORT int DoWinMain(HINSTANCE hLauncherInstance, HINSTANCE hPrevInstance, 
                        LPSTR lpCmdLine, int nCmdShow)
{
    // Validate critical parameters
    if (!hLauncherInstance) {
        return ERROR_NULL_INSTANCE;
    }

    // RAII guard for UTF8 file hooks
    Utf8FileHooksGuard utf8Guard;

    // Run debug tests if in debug mode
    #if defined(MTA_DEBUG)
    SharedUtil_Tests();
    #endif

    // Prepare safe command line buffer
    char safeCmdLine[MAX_CMD_LINE_LENGTH] = {0};
    SafeCopyCommandLine(lpCmdLine, safeCmdLine, sizeof(safeCmdLine));

    //
    // Initialization Phase
    //
    if (!PerformInitialization(safeCmdLine)) {
        return ERROR_NULL_INSTALL_MANAGER;
    }

    // Show initial splash screen
    ShowSplash(hLauncherInstance);

    //
    // Update Phase
    //
    auto* pInstallManager = GetInstallManager();
    const SString strCmdLine = ContinueUpdateProcedure(pInstallManager);

    //
    // Pre-Launch Phase
    //
    PerformPreLaunchSetup(hLauncherInstance);

    //
    // Launch Phase
    //
    const int iReturnCode = LaunchGameSafely(strCmdLine);
    
    // Post-launch monitoring
    PostRunWatchDogs(iReturnCode);

    //
    // Cleanup Phase
    //
    HandleOnQuitCommand();
    ProcessPendingBrowseToSolution();

    // Log termination details
    const DWORD currentPid = GetSafeProcessId();
    AddReportLog(LOG_ID_END, SString("* End (0x%X)* pid:%d", iReturnCode, currentPid));

    return iReturnCode;
}