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
#include <cassert>

#if __cplusplus >= 201703L
    #define MAYBE_UNUSED [[maybe_unused]]
#else
    #define MAYBE_UNUSED
#endif

namespace {
    // Error codes enum for better maintainability
    enum ErrorCode : int {
        ERROR_NULL_INSTANCE = -1,
        ERROR_NULL_INSTALL_MANAGER = -2,
        ERROR_LAUNCH_EXCEPTION = -3,
        ERROR_INSTALL_CONTINUE = -4
    };
    
    // Command line constants
    constexpr size_t MAX_CMD_LINE_LENGTH = 4096;
    
    // Report log IDs
    constexpr int LOG_ID_END = 1044;
    constexpr int LOG_ID_CONTINUE_EXCEPTION = 1045;
    constexpr int LOG_ID_LAUNCH_EXCEPTION = 1046;
    
    // Compile-time checks  
    static_assert(MAX_CMD_LINE_LENGTH > 0, "Command line buffer size must be positive");
    static_assert(MAX_CMD_LINE_LENGTH <= 65536, "Command line buffer size seems unreasonably large");
    static_assert(sizeof(DWORD) >= sizeof(int), "DWORD must be at least as large as int");

    class Utf8FileHooksGuard {
    private:
        bool m_released = false;
        
    public:
        Utf8FileHooksGuard() { 
            AddUtf8FileHooks(); 
        }
        
        ~Utf8FileHooksGuard() noexcept { 
            if (!m_released) {
                RemoveUtf8FileHooks();
            }
        }
        
        // Called when we want to keep hooks active (early return with error)
        void release() noexcept { 
            m_released = true; 
        }
        
        // Called when we want to remove hooks early (on GetInstallManager failure)
        void removeNow() noexcept {
            if (!m_released) {
                RemoveUtf8FileHooks();
                m_released = true;
            }
        }
        
        // Disable copy and move
        Utf8FileHooksGuard(const Utf8FileHooksGuard&) = delete;
        Utf8FileHooksGuard& operator=(const Utf8FileHooksGuard&) = delete;
        Utf8FileHooksGuard(Utf8FileHooksGuard&&) = delete;
        Utf8FileHooksGuard& operator=(Utf8FileHooksGuard&&) = delete;
    };

    inline void SafeCopyCommandLine(LPSTR lpCmdLine, char* safeCmdLine, size_t bufferSize) noexcept {
        // Preconditions (only in debug builds)
        assert(safeCmdLine != nullptr && "Destination buffer must not be null");
        assert(bufferSize > 0 && "Buffer size must be positive");
        
        if (!safeCmdLine || bufferSize == 0) {
            return;
        }
        
        // If source is null, destination remains zero-initialized
        if (!lpCmdLine) {
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

    CInstallManager* PerformEarlyInitialization(const char* safeCmdLine) {
        auto* pInstallManager = GetInstallManager();
        if (!pInstallManager) {
            return nullptr;
        }

        // Let install manager figure out what MTASA path to use
        pInstallManager->SetMTASAPathSource(safeCmdLine);
        
        // Start logging.....now
        BeginEventLog();
        
        // Start localization if possible
        InitLocalization(false);
        
        // Handle commands from the installer
        HandleSpecialLaunchOptions();
        
        // Check MTA is launched only once
        HandleDuplicateLaunching();
        
        return pInstallManager;
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

	// Launch the game with exception handling
    int LaunchGameSafely(const SString& strCmdLine) {
        try {
            return LaunchGame(strCmdLine);
        }
        catch (...) {
            AddReportLog(LOG_ID_LAUNCH_EXCEPTION, "Exception in LaunchGame()");
            return static_cast<int>(ERROR_LAUNCH_EXCEPTION);
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
MTAEXPORT int DoWinMain(HINSTANCE hLauncherInstance, MAYBE_UNUSED HINSTANCE hPrevInstance, 
                        LPSTR lpCmdLine, MAYBE_UNUSED int nCmdShow)
{
    // Silence unused parameter warnings for older compilers
    #if __cplusplus < 201703L
        (void)hPrevInstance;
        (void)nCmdShow;
    #endif

    // Check for null parameters before use
    if (!hLauncherInstance) {
        return static_cast<int>(ERROR_NULL_INSTANCE);
    }

    char safeCmdLine[MAX_CMD_LINE_LENGTH] = {0};
    SafeCopyCommandLine(lpCmdLine, safeCmdLine, sizeof(safeCmdLine));

    // Log the command line we're receiving
    {
        char debugBuf[512];
        _snprintf_s(debugBuf, sizeof(debugBuf), _TRUNCATE, 
                   "========================================\nMain.cpp - Command line received: '%s'\n========================================\n", 
                   safeCmdLine);
        OutputDebugStringA(debugBuf);
    }

    // RAII guard for UTF8 file hooks
    Utf8FileHooksGuard utf8Guard;

    #if defined(MTA_DEBUG)
    SharedUtil_Tests();
    #endif

    //
    // Init
    //

    auto* pInstallManager = PerformEarlyInitialization(safeCmdLine);
    if (!pInstallManager) {
        // Remove hooks when install manager fails
        utf8Guard.removeNow();
        return static_cast<int>(ERROR_NULL_INSTALL_MANAGER);
    }

    HINSTANCE hInstanceToUse = hLauncherInstance;
    
    // Show logo
    ShowSplash(hInstanceToUse);

    // Other init stuff
    ClearPendingBrowseToSolution();

    // Find GTA path to use
    ValidateGTAPath();


    // Continue any update procedure
    SString strCmdLine = ContinueUpdateProcedure(pInstallManager);

    // Ensure localization is started
    InitLocalization(true);

    // Setup/test various counters and flags for monitoring problems
    PreLaunchWatchDogs();

    // Stuff
    HandleCustomStartMessage();

    #if !defined(MTA_DEBUG) && MTASA_VERSION_TYPE != VERSION_TYPE_CUSTOM
    ForbodenProgramsMessage();
    #endif

    CycleEventLog();
    BsodDetectionPreLaunch();
    MaybeShowCopySettingsDialog();

    // Make sure GTA is not running
    HandleIfGTAIsAlreadyRunning();

    // Maybe warn user if no anti-virus running
    CheckAntiVirusStatus();

    // Ensure logo is showing
    ShowSplash(hInstanceToUse);

    // Check MTA files look good
    CheckDataFiles();
    CheckLibVersions();

    // Go for launch
    // Initialize return code with safe default
    int iReturnCode = 0;
    iReturnCode = LaunchGameSafely(strCmdLine);

    PostRunWatchDogs(iReturnCode);

    //
    // Quit
    //

    HandleOnQuitCommand();

    // Maybe show help if trouble was encountered
    ProcessPendingBrowseToSolution();

    // Get current process ID for logging
    DWORD currentPid = GetSafeProcessId();

    const DWORD exitCodeForLog = static_cast<DWORD>(static_cast<unsigned int>(iReturnCode));
    AddReportLog(LOG_ID_END, SString("* End (0x%08X)* pid:%lu", exitCodeForLog, static_cast<unsigned long>(currentPid)));

    return iReturnCode;
}
