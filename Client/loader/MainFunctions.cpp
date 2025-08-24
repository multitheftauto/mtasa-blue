/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/MainFunctions.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "MainFunctions.h"
#include "Main.h"
#include "Utils.h"
#include "Dialogs.h"
#include "D3DStuff.h"
#include <version.h>
#include <array>
#include <locale.h>
#include <memory>

// Constants for better maintainability
namespace Constants {
    constexpr DWORD PROCESS_WAIT_TIMEOUT = 1000;
    constexpr DWORD MAX_WAIT_TIME = 3600000;
    constexpr int RECHECK_TIME_LIMIT = 2000;
    constexpr int MAX_RETRIES = 10;
    constexpr int RETRY_DELAY_MS = 500;
    constexpr uint MAX_WAIT_ITERATIONS = 20;
    constexpr size_t MAX_COMMAND_LINE = 32768;
    constexpr size_t MAX_URI_LENGTH = 2048;
}

//////////////////////////////////////////////////////////
//
// CheckLibVersions
//
//////////////////////////////////////////////////////////
void CheckLibVersions() {
#if MTASA_VERSION_TYPE >= VERSION_TYPE_UNTESTED
    const char* moduleList[] = {
        "MTA\\loader.dll",
        "MTA\\cgui.dll",
        "MTA\\core.dll",
        "MTA\\game_sa.dll",
        "MTA\\multiplayer_sa.dll",
        "MTA\\netc.dll",
        "MTA\\xmll.dll",
        "MTA\\game_sa.dll",
        "MTA\\" LOADER_PROXY_DLL_NAME,
        "mods\\deathmatch\\client.dll",
        "mods\\deathmatch\\pcre3.dll"
    };
    
    SString strReqFileVersion;
    for (uint i = 0; i < NUMELMS(moduleList); i++) {
        SString strFilename = moduleList[i];
#ifdef MTA_DEBUG
        strFilename = ExtractBeforeExtension(strFilename) + "_d." + ExtractExtension(strFilename);
#endif
        SLibVersionInfo fileInfo;
        if (FileExists(CalcMTASAPath(strFilename))) {
            SString strFileVersion = "0.0.0.0";
            if (GetLibVersionInfo(CalcMTASAPath(strFilename), &fileInfo)) {
                strFileVersion = SString("%d.%d.%d.%d", 
                    fileInfo.dwFileVersionMS >> 16, fileInfo.dwFileVersionMS & 0xFFFF, 
                    fileInfo.dwFileVersionLS >> 16, fileInfo.dwFileVersionLS & 0xFFFF);
            }
            
            if (strReqFileVersion.empty()) {
                strReqFileVersion = strFileVersion;
            } else if (strReqFileVersion != strFileVersion) {
                DisplayErrorMessageBox(SStringX(_("File version mismatch error. Reinstall MTA:SA if you experience problems.\n") +
                                               SString("\n[%s %s/%s]\n", *strFilename, *strFileVersion, *strReqFileVersion)),
                                      _E("CL40"), "bad-file-version");
                break;
            }
        } else {
            DisplayErrorMessageBox(SStringX(_("Some files are missing. Reinstall MTA:SA if you experience problems.\n") +
                                           SString("\n[%s]\n", *strFilename)),
                                  _E("CL41"), "missing-file");
            break;
        }
    }
#endif

    if (GetSystemMetrics(SM_CLEANBOOT) != 0) {
        DisplayErrorMessageBox(SStringX(_("MTA:SA is not compatible with Windows 'Safe Mode'.\n\nPlease restart your PC.\n")),
                              _E("CL42"), "safe-mode");
        ExitProcess(EXIT_ERROR);
    }
}

// Enum declarations
DECLARE_ENUM(WSC_SECURITY_PROVIDER_HEALTH)
IMPLEMENT_ENUM_BEGIN(WSC_SECURITY_PROVIDER_HEALTH)
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_GOOD, "good")
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED, "not_monitored")
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_POOR, "poor")
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_SNOOZE, "snooze")
IMPLEMENT_ENUM_END("wsc_health")

// Simplified localization dummy class
class CLocalizationDummy final : public CLocalizationInterface {
public:
    SString Translate(const SString& strMessage) override { return strMessage; }
    SString TranslateWithContext(const SString& strContext, const SString& strMessage) override { return strMessage; }
    SString TranslatePlural(const SString& strSingular, const SString& strPlural, int iNum) override { return strPlural; }
    SString TranslatePluralWithContext(const SString& strContext, const SString& strSingular, 
                                       const SString& strPlural, int iNum) override { return strPlural; }
    std::vector<SString> GetAvailableLocales() override { return {}; }
    bool IsLocalized() override { return false; }
    SString GetLanguageDirectory(CLanguage* pLanguage = nullptr) override { return ""; }
    SString GetLanguageCode() override { return "en_US"; }
    SString GetLanguageName() override { return "English"; }
};

// Global localization interface
CLocalizationInterface* g_pLocalization = new CLocalizationDummy();

// Helper functions
namespace {
    bool ValidatePath(const SString& path) {
        return !path.empty() && path.length() <= MAX_PATH && !path.Contains("..");
    }
    
    void DisplayError(const SString& message, const SString& code, const SString& extra = "") {
        DisplayErrorMessageBox(message, code, extra);
        ExitProcess(EXIT_ERROR);
    }
    
    template<typename Func>
    void SafeFileOperation(const SString& path, Func&& operation) {
        if (ValidatePath(path)) {
            operation(path);
        }
    }
}

//////////////////////////////////////////////////////////
//
// InitLocalization
//
//////////////////////////////////////////////////////////
void InitLocalization(bool bShowErrors) {
    static bool bInitialized = false;
    if (bInitialized) return;
    
    // Load core.dll for localization
    const SString strCoreDLL = PathJoin(GetLaunchPath(), "mta", MTA_DLL_NAME);
    if (!FileExists(strCoreDLL)) {
        if (!bShowErrors) return;
        DisplayError(_("Load failed. Please ensure that the file " MTA_DLL_NAME 
                      " is in the modules directory within the MTA root directory."),
                    _E("CL23"), "core-missing");
    }
    
    const SString strMTASAPath = GetMTASAPath();
    SetDllDirectory(PathJoin(strMTASAPath, "mta"));
    
    // Handle XInput dependencies
    const DWORD dwPrevMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    const char* xinputModules[] = {"XInput9_1_0", "xinput1_3"};
    
    for (uint i = 0; i < NUMELMS(xinputModules); ++i) {
        SString strDllName = SString("%s.dll", xinputModules[i]);
        
        HMODULE hXInputModule = LoadLibrary(strDllName);
        if (hXInputModule) {
            FreeLibrary(hXInputModule);  // Just checking if it exists
        } else {
            const SString strDest = PathJoin(strMTASAPath, "mta", strDllName);
            if (!FileExists(strDest)) {
                SString strSrc = PathJoin(strMTASAPath, "mta", SString("%s_mta.dll", xinputModules[i]));
                if (!FileExists(strSrc)) {
                    strSrc = PathJoin(GetLaunchPath(), "mta", SString("%s_mta.dll", xinputModules[i]));
                }
                FileCopy(strSrc, strDest);
            }
        }
    }
    
    // Load core module
    if (bShowErrors) SetErrorMode(dwPrevMode);
    HMODULE hCoreModule = LoadLibraryEx(strCoreDLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    SetErrorMode(dwPrevMode);
    
    if (!hCoreModule) {
        if (!bShowErrors) return;
        DisplayError(_("Loading core failed. Please ensure that the latest DirectX is correctly installed."),
                    _E("CL24"), "core-not-loadable");
    }
    
    // Get locale
    SString strLocale = GetApplicationSetting("locale");
    if (strLocale.empty()) {
        char originalLocale[256] = {0};
        if (const char* locale = setlocale(LC_ALL, NULL)) {
            strncpy(originalLocale, locale, sizeof(originalLocale) - 1);
            originalLocale[sizeof(originalLocale) - 1] = '\0';
        }
        
        setlocale(LC_ALL, "");
        if (const char* newLocale = setlocale(LC_ALL, NULL)) {
            strLocale = newLocale;
        }
        
        if (originalLocale[0]) {
            setlocale(LC_ALL, originalLocale);
        }
    }
    
    // Create localization
    typedef CLocalizationInterface*(__cdecl* CreateLocalizationFunc)(SString);
    auto pFunc = reinterpret_cast<CreateLocalizationFunc>(static_cast<void*>(GetProcAddress(hCoreModule, "L10n_CreateLocalization")));
    
    if (!pFunc) {
        FreeLibrary(hCoreModule);
        if (bShowErrors) {
            DisplayError(_("Loading localization failed. Please ensure that MTA San Andreas 1.6\\MTA\\locale is accessible."),
                        _E("CL26"), "localization-not-loadable");
        }
        return;
    }
    
    CLocalizationInterface* pLocalization = pFunc(strLocale);
    if (!pLocalization) {
        FreeLibrary(hCoreModule);
        if (bShowErrors) {
            DisplayError(_("Loading localization failed. Please ensure that MTA San Andreas 1.6\\MTA\\locale is accessible."),
                        _E("CL26"), "localization-not-loadable");
        }
        return;
    }
    
    // Replace the dummy localization with the real one
    SAFE_DELETE(g_pLocalization);
    g_pLocalization = pLocalization;
    // NOTE: hCoreModule is intentionally kept loaded as it contains the localization implementation
    bInitialized = true;
    
    #ifdef MTA_DEBUG
        TestDialogs();
    #endif
}

//////////////////////////////////////////////////////////
//
// HandleTrouble
//
//////////////////////////////////////////////////////////
void HandleTrouble() {
    if (CheckAndShowFileOpenFailureMessage())
        return;

#if !defined(MTA_DEBUG) && MTASA_VERSION_TYPE != VERSION_TYPE_CUSTOM
    int iResponse = MessageBoxUTF8(NULL, _("Are you having problems running MTA:SA?.\n\nDo you want to revert to an earlier version?"),
                                   "MTA: San Andreas" + _E("CL07"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
    if (iResponse == IDYES) {
        BrowseToSolution("crashing-before-gtagame", TERMINATE_PROCESS);
    }
#endif
}

//////////////////////////////////////////////////////////
//
// HandleResetSettings
//
//////////////////////////////////////////////////////////
void HandleResetSettings() {
    if (CheckAndShowFileOpenFailureMessage())
        return;

    CheckAndShowMissingFileMessage();

    SString strSaveFilePath = PathJoin(GetSystemPersonalPath(), "GTA San Andreas User Files");
    SString strSettingsFilename = PathJoin(strSaveFilePath, "gta_sa.set");
    SString strSettingsFilenameBak = PathJoin(strSaveFilePath, "gta_sa_old.set");

    if (FileExists(strSettingsFilename)) {
        int iResponse = MessageBoxUTF8(NULL,
                                       _("There seems to be a problem launching MTA:SA.\nResetting GTA settings can sometimes fix this problem.\n\nDo you want "
                                         "to reset GTA settings now?"),
                                       "MTA: San Andreas" + _E("CL08"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        if (iResponse == IDYES) {
            FileDelete(strSettingsFilenameBak);
            FileRename(strSettingsFilename, strSettingsFilenameBak);
            FileDelete(strSettingsFilename);

            // Also reset NVidia Optimus "remember option"
            SetApplicationSettingInt("nvhacks", "optimus-remember-option", 0);

            if (!FileExists(strSettingsFilename)) {
                AddReportLog(4053, "Deleted gta_sa.set");
                MessageBoxUTF8(NULL, _("GTA settings have been reset.\n\nPress OK to continue."), "MTA: San Andreas", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            } else {
                AddReportLog(5054, SString("Delete gta_sa.set failed with '%s'", *strSettingsFilename));
                MessageBoxUTF8(NULL, SString(_("File could not be deleted: '%s'"), *strSettingsFilename), "Error" + _E("CL09"),
                               MB_OK | MB_ICONWARNING | MB_TOPMOST);
            }
        }
    } else {
        // No settings to delete, or can't find them
        int iResponse = MessageBoxUTF8(NULL, _("Are you having problems running MTA:SA?.\n\nDo you want to see some online help?"), "MTA: San Andreas",
                                       MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        if (iResponse == IDYES) {
            BrowseToSolution("crashing-before-gtalaunch", TERMINATE_PROCESS);
        }
    }
}

//////////////////////////////////////////////////////////
//
// HandleNotUsedMainMenu
//
//////////////////////////////////////////////////////////
void HandleNotUsedMainMenu() {
    AddReportLog(9310, "Loader - HandleNotUsedMainMenu");
    {
        // Check in-game settings
        SString strCoreConfigFilename = CalcMTASAPath(PathJoin("mta", "config", "coreconfig.xml"));
        SString strCoreConfig;
        FileLoad(strCoreConfigFilename, strCoreConfig);
        SString strWindowed = strCoreConfig.SplitRight("<display_windowed>").Left(1);
        SString strFullscreenStyle = strCoreConfig.SplitRight("<display_fullscreen_style>").Left(1);
        
        if (strFullscreenStyle == "1") {
            AddReportLog(9315, "Loader - HandleNotUsedMainMenu - Already Borderless window");
        } else if (!strWindowed.empty() && !strFullscreenStyle.empty()) {
            if (strWindowed == "0" && strFullscreenStyle == "0") {
                // Inform user
                SString strMessage = _("Are you having problems running MTA:SA?.\n\nDo you want to change the following setting?");
                strMessage += "\n" + _("Fullscreen mode:") + " -> " + _("Borderless window");
                HideSplash();
                int iResponse = MessageBoxUTF8(NULL, strMessage, "MTA: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
                if (iResponse == IDYES) {
                    // Change in-game settings
                    strCoreConfig = strCoreConfig.Replace("<display_fullscreen_style>0", "<display_fullscreen_style>1");
                    FileSave(strCoreConfigFilename, strCoreConfig);
                    AddReportLog(9311, "Loader - HandleNotUsedMainMenu - User change to Borderless window");
                } else {
                    AddReportLog(9313, "Loader - HandleNotUsedMainMenu - User said no");
                }
            } else {
                AddReportLog(9314, "Loader - HandleNotUsedMainMenu - Mode not fullscreen standard");
            }
        } else {
            // If no valid settings file yet, do the change without asking
            strCoreConfig = "<mainconfig><settings><display_fullscreen_style>1</display_fullscreen_style></settings></mainconfig>";
            FileSave(strCoreConfigFilename, strCoreConfig);
            AddReportLog(9312, "Loader - HandleNotUsedMainMenu - Set Borderless window");
        }
    }

    // Check if problem processes are active
    struct {
        std::vector<SString> matchTextList;
        const char* szProductName;
        const char* szTrouble;
    } procItems[] = {
        {{"\\Evolve"}, "Evolve", "not-used-menu-evolve"},
        {{"\\GbpSv.exe", "Diebold\\Warsaw"}, "GAS Tecnologia - G-Buster Browser Defense", "not-used-menu-gbpsv"}
    };
    
    bool foundProblemProcess = false;
    for (uint i = 0; i < NUMELMS(procItems) && !foundProblemProcess; i++) {
        for (auto processId : MyEnumProcesses(true)) {
            SString strProcessFilename = GetProcessPathFilename(processId);
            for (const auto& strMatchText : procItems[i].matchTextList) {
                if (strProcessFilename.ContainsI(strMatchText)) {
                    SString strMessage = _("Are you having problems running MTA:SA?.\n\nTry disabling the following products for GTA and MTA:");
                    strMessage += "\n\n";
                    strMessage += procItems[i].szProductName;
                    DisplayErrorMessageBox(strMessage, _E("CL43"), procItems[i].szTrouble);
                    foundProblemProcess = true;
                    break;
                }
            }
            if (foundProblemProcess) break;
        }
    }
}

//////////////////////////////////////////////////////////
//
// HandleCustomStartMessage
//
//////////////////////////////////////////////////////////
void HandleCustomStartMessage() {
    SString strStartMessage = GetApplicationSetting("diagnostics", "start-message");
    SString strTrouble = GetApplicationSetting("diagnostics", "start-message-trouble");

    if (strStartMessage.empty())
        return;

    SetApplicationSetting("diagnostics", "start-message", "");
    SetApplicationSetting("diagnostics", "start-message-trouble", "");

    if (strStartMessage.BeginsWith("vdetect")) {
        SString strFilename = strStartMessage.SplitRight("name=");
        strStartMessage = _("WARNING\n\n"
                           "MTA:SA has detected unusual activity.\n"
                           "Please run a virus scan to ensure your system is secure.\n\n");
        strStartMessage += SString(_("The detected file was:  %s\n"), *strFilename);
    }

    DisplayErrorMessageBox(strStartMessage, _E("CL37"), strTrouble);
}

//////////////////////////////////////////////////////////
//
// PreLaunchWatchDogs
//
//////////////////////////////////////////////////////////
void PreLaunchWatchDogs() {
    assert(!CreateSingleInstanceMutex());

    // Check for unclean stop on previous run
#ifndef MTA_DEBUG
    if (WatchDogIsSectionOpen("L0")) {
        WatchDogSetUncleanStop(true);
        CheckAndShowFileOpenFailureMessage();
    } else
#endif
        WatchDogSetUncleanStop(false);

    SString strCrashFlagFilename = CalcMTASAPath("mta\\core.log.flag");
    if (FileExists(strCrashFlagFilename)) {
        FileDelete(strCrashFlagFilename);
        WatchDogSetLastRunCrash(true);
    } else {
        WatchDogSetLastRunCrash(false);
    }

    // Reset counter if gta game was run last time
    if (!WatchDogIsSectionOpen("L1"))
        WatchDogClearCounter("CR1");

    // If crashed 3 times in a row before starting the game, do something
    if (WatchDogGetCounter("CR1") >= 3) {
        WatchDogReset();
        HandleTrouble();
    }

    // Check for possible gta_sa.set problems
    if (WatchDogIsSectionOpen("L2")) {
        WatchDogIncCounter("CR2");
        WatchDogCompletedSection("L2");
    } else {
        WatchDogClearCounter("CR2");
    }

    // If didn't reach loading screen 5 times in a row, do something
    if (WatchDogGetCounter("CR2") >= 5) {
        WatchDogClearCounter("CR2");
        HandleResetSettings();
    }

    // Check for possible fullscreen problems
    if (WatchDogIsSectionOpen(WD_SECTION_NOT_USED_MAIN_MENU)) {
        int iChainLimit;
        if (WatchDogIsSectionOpen(WD_SECTION_POST_INSTALL))
            iChainLimit = 1;
        else if (GetApplicationSettingInt("times-connected") == 0)
            iChainLimit = 2;
        else
            iChainLimit = 3;
        
        WatchDogCompletedSection(WD_SECTION_NOT_USED_MAIN_MENU);
        WatchDogIncCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
        if (WatchDogGetCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU) >= iChainLimit) {
            WatchDogClearCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
            HandleNotUsedMainMenu();
        }
    } else {
        WatchDogClearCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
    }

    // Clear down freeze on quit detection
    WatchDogCompletedSection("Q0");

    WatchDogBeginSection("L0");
    WatchDogBeginSection("L1");
    SetApplicationSetting("diagnostics", "gta-fopen-fail", "");
    SetApplicationSetting("diagnostics", "last-crash-reason", "");
    SetApplicationSetting("diagnostics", "last-crash-module", "");
    SetApplicationSetting("diagnostics", "last-crash-code", "");
    SetApplicationSetting("diagnostics", "gta-fopen-last", "");
}

//////////////////////////////////////////////////////////
//
// PostRunWatchDogs
//
//////////////////////////////////////////////////////////
void PostRunWatchDogs(int iReturnCode) {
    if (iReturnCode == 0) {
        WatchDogClearCounter("CR1");
        WatchDogCompletedSection("L0");
    }
}

//////////////////////////////////////////////////////////
//
// HandleIfGTAIsAlreadyRunning
//
//////////////////////////////////////////////////////////
void HandleIfGTAIsAlreadyRunning() {
    if (IsGTARunning()) {
        if (MessageBoxUTF8(0, 
            _("An instance of GTA: San Andreas is already running. It needs to be terminated before MTA:SA can be started. Do you want to do that now?"),
            _("Information") + _E("CL10"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES) {
            
            TerminateOtherMTAIfRunning();
            TerminateGTAIfRunning();
            if (IsGTARunning()) {
                MessageBoxUTF8(0, _("Unable to terminate GTA: San Andreas. If the problem persists, please restart your computer."),
                               _("Information") + _E("CL11"), MB_OK | MB_ICONERROR | MB_TOPMOST);
                ExitProcess(EXIT_ERROR);
            }
        } else {
            ExitProcess(EXIT_OK);
        }
    }
}

//////////////////////////////////////////////////////////
//
// HandleSpecialLaunchOptions
//
//////////////////////////////////////////////////////////
void HandleSpecialLaunchOptions() {
    if (CommandLineContains("/kdinstall")) {
        UpdateMTAVersionApplicationSetting(true);
        WatchDogReset();
        WatchDogBeginSection(WD_SECTION_POST_INSTALL);
        ExitProcess(CheckService(CHECK_SERVICE_POST_INSTALL) ? EXIT_OK : EXIT_ERROR);
    }
    
    if (CommandLineContains("/kduninstall")) {
        UpdateMTAVersionApplicationSetting(true);
        ExitProcess(CheckService(CHECK_SERVICE_PRE_UNINSTALL) ? EXIT_OK : EXIT_ERROR);
    }
    
    if (CommandLineContains("/nolaunch")) {
        ExitProcess(EXIT_OK);
    }
}

//////////////////////////////////////////////////////////
//
// HandleDuplicateLaunching
//
//////////////////////////////////////////////////////////
void HandleDuplicateLaunching() {
    LPSTR lpCmdLine = GetCommandLine();
    if (!lpCmdLine) ExitProcess(EXIT_ERROR);
    
    const size_t cmdLineLen = strlen(lpCmdLine);
    if (cmdLineLen > Constants::MAX_COMMAND_LINE) ExitProcess(EXIT_ERROR);
    
    int recheckTime = Constants::RECHECK_TIME_LIMIT;
    
    while (!CreateSingleInstanceMutex()) {
        if (cmdLineLen > 0) {
            HWND hwMTAWindow = FindWindow(NULL, "MTA: San Andreas");
            #ifdef MTA_DEBUG
                if (!hwMTAWindow) hwMTAWindow = FindWindow(NULL, "MTA: San Andreas [DEBUG]");
            #endif
            
            if (hwMTAWindow) {
                LPWSTR szCommandLine = GetCommandLineW();
                if (!szCommandLine) continue;
                
                int numArgs = 0;
                LPWSTR* aCommandLineArgs = CommandLineToArgvW(szCommandLine, &numArgs);
                
                if (aCommandLineArgs && numArgs > 0 && numArgs < 1000) {
                    for (int i = 1; i < numArgs; ++i) {
                        if (!aCommandLineArgs[i]) continue;
                        
                        WString wideArg = aCommandLineArgs[i];
                        if (wideArg.length() > 8 && wideArg.length() < Constants::MAX_URI_LENGTH &&
                            WStringX(wideArg).BeginsWith(L"mtasa://")) {
                            
                            SString strConnectInfo = ToUTF8(wideArg);
                            if (strConnectInfo.find('\0') == SString::npos) {
                                COPYDATASTRUCT cdStruct = {
                                    URI_CONNECT,
                                    static_cast<DWORD>(strConnectInfo.length() + 1),
                                    const_cast<char*>(strConnectInfo.c_str())
                                };
                                SendMessage(hwMTAWindow, WM_COPYDATA, NULL, reinterpret_cast<LPARAM>(&cdStruct));
                            }
                            break;
                        }
                    }
                    LocalFree(aCommandLineArgs);
                }
                ExitProcess(EXIT_ERROR);
            } else if (recheckTime > 0) {
                Sleep(500);
                recheckTime -= 500;
                continue;
            } else {
                const SString strMessage = _("Trouble restarting MTA:SA\n\n"
                                            "If the problem persists, open Task Manager and\n"
                                            "stop the 'gta_sa.exe' and 'Multi Theft Auto.exe' processes\n\n\n"
                                            "Try to launch MTA:SA again?");
                
                if (MessageBoxUTF8(0, strMessage, _("Error") + _E("CL04"), MB_ICONWARNING | MB_YESNO | MB_TOPMOST) == IDYES) {
                    TerminateGTAIfRunning();
                    TerminateOtherMTAIfRunning();
                    
                    const SString exePath = PathJoin(GetMTASAPath(), MTA_EXE_NAME);
                    if (FileExists(exePath)) {
                        ShellExecuteNonBlocking("open", exePath, lpCmdLine);
                    }
                }
                ExitProcess(EXIT_ERROR);
            }
        } else {
            if (!IsGTARunning() && !IsOtherMTARunning()) {
                MessageBoxUTF8(0, _("Another instance of MTA is already running.\n\n"
                                   "If this problem persists, please restart your computer"),
                              _("Error") + _E("CL05"), MB_ICONERROR | MB_TOPMOST);
            } else if (MessageBoxUTF8(0, _("Another instance of MTA is already running.\n\n"
                                          "Do you want to terminate it?"),
                                     _("Error") + _E("CL06"), MB_ICONQUESTION | MB_YESNO | MB_TOPMOST) == IDYES) {
                TerminateGTAIfRunning();
                TerminateOtherMTAIfRunning();
                
                const SString exePath = PathJoin(GetMTASAPath(), MTA_EXE_NAME);
                if (FileExists(exePath)) {
                    ShellExecuteNonBlocking("open", exePath, lpCmdLine);
                }
            }
            ExitProcess(EXIT_ERROR);
        }
    }
}

//////////////////////////////////////////////////////////
//
// ValidateGTAPath
//
//////////////////////////////////////////////////////////
void ValidateGTAPath() {
    const ePathResult result = DiscoverGTAPath(true);
    
    if (result == GAME_PATH_MISSING) {
        DisplayError(_("Registry entries are missing. Please reinstall Multi Theft Auto: San Andreas."), 
                    _E("CL12"), "reg-entries-missing");
    } else if (result == GAME_PATH_UNICODE_CHARS) {
        DisplayError(_("The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. "
                      "Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains "
                      "only standard ASCII characters and reinstall Multi Theft Auto: San Andreas."),
                    _E("CL13"));
    }
    
    const SString strGTAPath = GetGTAPath();
    const SString strMTASAPath = GetMTASAPath();
    
    if (strGTAPath.Contains(";") || strMTASAPath.Contains(";")) {
        DisplayErrorMessageBox(_("The path to your installation of 'MTA:SA' or 'GTA: San Andreas'\n"
                                "contains a ';' (semicolon).\n\n"
                                "If you experience problems when running MTA:SA,\n"
                                "move your installation(s) to a path that does not contain a semicolon."),
                              _E("CL15"), "path-semicolon");
    }
}

//////////////////////////////////////////////////////////
//
// CheckAntiVirusStatus
//
//////////////////////////////////////////////////////////
void CheckAntiVirusStatus() {
    std::vector<SString> enabledList, disabledList;
    GetWMIAntiVirusStatus(enabledList, disabledList);
    
    WSC_SECURITY_PROVIDER_HEALTH health = static_cast<WSC_SECURITY_PROVIDER_HEALTH>(-1);
    
    // Load WSC function dynamically
    static const auto WscGetHealth = []() -> decltype(&WscGetSecurityProviderHealth) {
        if (HMODULE wscapi = LoadLibraryW(L"Wscapi.dll")) {
            return reinterpret_cast<decltype(&WscGetSecurityProviderHealth)>(
                GetProcAddress(wscapi, "WscGetSecurityProviderHealth"));
        }
        return nullptr;
    }();
    
    if (WscGetHealth) {
        WscGetHealth(WSC_SECURITY_PROVIDER_ANTIVIRUS, &health);
    }
    
    // Build status string efficiently
    SString strStatus;
    strStatus.reserve(256);
    strStatus += SString("AV health: %s (%d)", *EnumToString(health), static_cast<int>(health));
    
    for (size_t i = 0; i < enabledList.size(); ++i) {
        strStatus += SString(" [Ena%zu:%s]", i, *enabledList[i]);
    }
    for (size_t i = 0; i < disabledList.size(); ++i) {
        strStatus += SString(" [Dis%zu:%s]", i, *disabledList[i]);
    }
    
    WriteDebugEvent(strStatus);
    
    // Check if AV warning needed
    if (enabledList.empty() && health != WSC_SECURITY_PROVIDER_HEALTH_GOOD) {
        bool showWarning = (health != WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED);
        
        if (showWarning) {
            static const char* avProducts[] = {
                "antivirus", "anti-virus", "Avast", "AVG", "Avira", "NOD32", "ESET",
                "F-Secure", "Faronics", "Kaspersky", "McAfee", "Norton", "Symantec",
                "Panda", "Trend Micro"
            };
            
            // Check loaded modules for AV
            std::array<HMODULE, 1024> modules;
            DWORD cbNeeded;
            
            if (EnumProcessModules(GetCurrentProcess(), modules.data(), 
                                  sizeof(modules), &cbNeeded)) {
                DWORD moduleCount = cbNeeded / sizeof(HMODULE);
                const DWORD maxModules = static_cast<DWORD>(modules.size());
                if (moduleCount > maxModules) {
                    WriteDebugEvent(SString("Warning: Process has %lu modules but array can only hold %lu", moduleCount, maxModules));
                    moduleCount = maxModules;
                }
                
                for (DWORD i = 0; i < moduleCount && showWarning; ++i) {
                    if (!modules[i]) continue;
                    
                    WCHAR modulePath[MAX_PATH * 2] = L"";
                    if (GetModuleFileNameExW(GetCurrentProcess(), modules[i], 
                                            modulePath, NUMELMS(modulePath))) {
                        SLibVersionInfo libInfo;
                        if (GetLibVersionInfo(ToUTF8(modulePath), &libInfo)) {
                            for (uint j = 0; j < NUMELMS(avProducts); ++j) {
                                if (libInfo.strCompanyName.ContainsI(avProducts[j]) ||
                                    libInfo.strProductName.ContainsI(avProducts[j])) {
                                    showWarning = false;
                                    WriteDebugEvent(SString("AV (module) found: %s", *ToUTF8(modulePath)));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            
            // Check running processes for AV
            if (showWarning) {
                for (auto processId : MyEnumProcesses(true)) {
                    SString processPath = GetProcessPathFilename(processId);
                    if (!processPath.empty()) {
                        SLibVersionInfo libInfo;
                        if (GetLibVersionInfo(processPath, &libInfo)) {
                            for (uint i = 0; i < NUMELMS(avProducts); ++i) {
                                if (libInfo.strCompanyName.ContainsI(avProducts[i]) ||
                                    libInfo.strProductName.ContainsI(avProducts[i])) {
                                    showWarning = false;
                                    WriteDebugEvent(SString("AV (process) found: %s", *processPath));
                                    break;
                                }
                            }
                        }
                    }
                    if (!showWarning) break;
                }
            }
        }
        
        ShowNoAvDialog(g_hInstance, showWarning);
        HideNoAvDialog();
    }
}

//////////////////////////////////////////////////////////
//
// CheckDataFiles
//
//////////////////////////////////////////////////////////
void CheckDataFiles() {
    const SString strMTASAPath = GetMTASAPath();
    const SString strGTAPath = GetGTAPath();
    
    if (!ValidatePath(strMTASAPath) || !ValidatePath(strGTAPath)) {
        DisplayError(_("Invalid installation paths detected."), _E("CL45"), "invalid-install-paths");
    }
    
    // Essential data files to check
    static const char* dataFiles[] = {
        "MTA\\cgui\\images\\background_logo.png",
        "MTA\\cgui\\images\\radarset\\up.png",
        "MTA\\cgui\\images\\busy_spinner.png",
        "MTA\\data\\gta_sa_diff.dat",
        "MTA\\D3DX9_42.dll",
        "MTA\\D3DCompiler_42.dll",
        "MTA\\d3dcompiler_43.dll",
        "MTA\\d3dcompiler_47.dll",
        "MTA\\bass.dll",
        "MTA\\bass_ac3.dll",
        "MTA\\bassflac.dll",
        "MTA\\bassmix.dll",
        "MTA\\basswebm.dll",
        "MTA\\bass_aac.dll",
        "MTA\\bass_fx.dll",
        "MTA\\bassmidi.dll",
        "MTA\\bassopus.dll",
        "MTA\\basswma.dll",
        "MTA\\tags.dll",
        "MTA\\sa.dat",
        "MTA\\xinput1_3_mta.dll",
        "MTA\\XInput9_1_0_mta.dll"
    };
    
    for (uint i = 0; i < NUMELMS(dataFiles); ++i) {
        const SString filePath = PathJoin(strMTASAPath, dataFiles[i]);
        if (!ValidatePath(filePath) || !FileExists(filePath)) {
            DisplayError(_("Load failed. Please ensure that the latest data files have been installed correctly."),
                        _E("CL16"), "mta-datafiles-missing");
        }
    }
    
    // Check for client file
    if (!FileExists(PathJoin(strMTASAPath, CHECK_DM_CLIENT_NAME))) {
        DisplayError(SString(_("Load failed. Please ensure that %s is installed correctly."), CHECK_DM_CLIENT_NAME),
                    _E("CL18"), "client-missing");
    }
    
    // Check GTA executable
    if (!FileExists(PathJoin(strGTAPath, GTA_EXE_NAME)) && 
        !FileExists(PathJoin(strGTAPath, STEAM_GTA_EXE_NAME))) {
        DisplayError(SString(_("Load failed. Could not find gta_sa.exe in %s."), strGTAPath.c_str()),
                    _E("CL20"), "gta_sa-missing");
    }
    
    // Check for conflicting DLLs
    static const char* dllConflicts[] = {
        "xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll", "pthread.dll"
    };
    
    for (uint i = 0; i < NUMELMS(dllConflicts); ++i) {
        if (FileExists(PathJoin(strGTAPath, dllConflicts[i]))) {
            DisplayError(SString(_("Load failed. %s exists in the GTA directory. Please delete before continuing."), dllConflicts[i]),
                        _E("CL21"), "file-clash");
        }
    }
    
    // Verify main executable name
    const SString launchFilename = GetLaunchFilename();
    if (!launchFilename.CompareI(MTA_EXE_NAME)) {
        const SString strMessage(_("Main file has an incorrect name (%s)"), *launchFilename);
        if (MessageBoxUTF8(NULL, strMessage, _("Error") + _E("CL33"), MB_RETRYCANCEL | MB_ICONERROR | MB_TOPMOST) == IDRETRY) {
            ReleaseSingleInstanceMutex();
            const SString correctExePath = PathJoin(strMTASAPath, MTA_EXE_NAME);
            if (FileExists(correctExePath)) {
                ShellExecuteNonBlocking("open", correctExePath);
            }
        }
        ExitProcess(EXIT_ERROR);
    }
    
    // Integrity checks
    struct IntegrityCheck {
        const char* hash;
        const char* fileName;
    };
    
    static const IntegrityCheck integrityCheckList[] = {
        {"DE5C08577EAA65309974F9860E303F53", "bass.dll"},
        {"1D5A1AEF041255DEA49CD4780CAE4CCC", "bass_aac.dll"},
        {"8A1AC2AAD7F1691943635CA42F7F2940", "bass_ac3.dll"},
        {"61C38C1FD091375F2A30EC631DF337E6", "bass_fx.dll"},
        {"F47DCE69DAFAA06A55A4BC1F07F80C8A", "bassflac.dll"},
        {"49A603ED114982787FC0A301C0E93FDB", "bassmidi.dll"},
        {"064398B1A74B4EF35902F0C218142133", "bassmix.dll"},
        {"9CFA31A873FF89C2CC491B9974FC5C65", "bassopus.dll"},
        {"B35714019BBFF0D0CEE0AFA2637A77A7", "basswebm.dll"},
        {"1507C60C02E159B5FB247FEC6B209B09", "basswma.dll"},
        {"C6A44FC3CF2F5801561804272217B14D", "D3DX9_42.dll"},
        {"D439E8EDD8C93D7ADE9C04BCFE9197C6", "sa.dat"},
        {"B33B21DB610116262D906305CE65C354", "D3DCompiler_42.dll"},
        {"4B3932359373F11CBC542CC96D9A9285", "tags.dll"},
        {"0B3DD892007FB366D1F52F2247C046F5", "d3dcompiler_43.dll"},
        {"D5D8C8561C6DDA7EF0D7D6ABB0D772F4", "xinput1_3_mta.dll"},
        {"2C0C596EE071B93CE15130BD5EE9CD31", "d3dcompiler_47.dll"},
        {"F1CA5A1E77965777AC26A81EAF345A7A", "XInput9_1_0_mta.dll"}
    };
    
    for (uint i = 0; i < NUMELMS(integrityCheckList); ++i) {
        const IntegrityCheck& check = integrityCheckList[i];
        const SString filePath = PathJoin(strMTASAPath, "mta", check.fileName);
        if (!ValidatePath(filePath) || !FileExists(filePath)) {
            DisplayErrorMessageBox(SString(_("Data file %s is missing. Possible virus activity."), check.fileName),
                                 _E("CL30"), "maybe-virus2");
            break;
        }
        
        const SString computed = CMD5Hasher::CalculateHexString(filePath);
        if (!computed.CompareI(check.hash)) {
            DisplayErrorMessageBox(SString(_("Data file %s is modified. Possible virus activity."), check.fileName),
                                 _E("CL30"), "maybe-virus2");
            break;
        }
    }
    
    // Check for ASI files
    std::vector<SString> gtaAsiFiles = FindFiles(PathJoin(strGTAPath, "*.asi"), true, false);
    std::vector<SString> mtaAsiFiles = FindFiles(PathJoin(strMTASAPath, "mta", "*.asi"), true, false);
    
    bool bFoundInGTADir = !gtaAsiFiles.empty() && gtaAsiFiles.size() < 1000;
    bool bFoundInMTADir = !mtaAsiFiles.empty() && mtaAsiFiles.size() < 1000;
    
    if (bFoundInGTADir || bFoundInMTADir) {
        DisplayErrorMessageBox(_(".asi files are in the installation directory.\n\n"
                                "Remove these .asi files if you experience problems."),
                              _E("CL28"), "asi-files");
    }
    
    // Check for graphics libraries in the GTA/MTA install directory
    {
        struct DirectoryCheck {
            const char* prefix;
            SString path;
        };
        
        const DirectoryCheck directoriesToCheck[] = {
            {"", strGTAPath},
            {"mta-", PathJoin(strMTASAPath, "mta")}
        };
        
        std::vector<GraphicsLibrary> offenders;
        
        for (uint i = 0; i < NUMELMS(directoriesToCheck); ++i) {
            const DirectoryCheck& directory = directoriesToCheck[i];
            if (!ValidatePath(directory.path)) continue;
            
            const char* libraryNames[] = {"d3d9", "dxgi"};
            for (uint j = 0; j < NUMELMS(libraryNames); ++j) {
                GraphicsLibrary library(libraryNames[j]);  // Use the constructor
                library.absoluteFilePath = PathJoin(directory.path, library.stem + ".dll");
                
                if (library.absoluteFilePath.length() > MAX_PATH) continue;
                if (!FileExists(library.absoluteFilePath)) continue;
                
                library.appLastHash = SString("%s%s-dll-last-hash", directory.prefix, library.stem.c_str());
                library.appDontRemind = SString("%s%s-dll-not-again", directory.prefix, library.stem.c_str());
                
                library.md5Hash = CMD5Hasher::CalculateHexString(library.absoluteFilePath);
                WriteDebugEvent(SString("Detected graphics library %s (md5: %s)", 
                                      library.absoluteFilePath.c_str(), library.md5Hash.c_str()));
                
                bool isProblematic = true;
                if (GetApplicationSetting("diagnostics", library.appLastHash) == library.md5Hash) {
                    if (GetApplicationSetting("diagnostics", library.appDontRemind) == "yes") {
                        isProblematic = false;
                    }
                }
                
                if (isProblematic) {
                    offenders.push_back(library);  // Changed from emplace_back(std::move(library))
                }
            }
        }
        
        if (!offenders.empty() && offenders.size() < 10) {
            ShowGraphicsDllDialog(g_hInstance, offenders);
            HideGraphicsDllDialog();
        }
    }
    
    // Clean up old log files
    if (!strGTAPath.CompareI(PathJoin(strMTASAPath, "mta"))) {
        static const char* logFiles[] = {"CEGUI.log", "logfile.txt", "shutdown.log"};
        for (uint i = 0; i < NUMELMS(logFiles); ++i) {
            SafeFileOperation(PathJoin(strGTAPath, logFiles[i]), 
                            [](const SString& path) { FileDelete(path); });
        }
    }
}

//////////////////////////////////////////////////////////
//
// StartGtaProcess
//
//////////////////////////////////////////////////////////
BOOL StartGtaProcess(const SString& lpApplicationName, const SString& lpCommandLine,
                    const SString& lpCurrentDirectory, LPPROCESS_INFORMATION lpProcessInformation,
                    DWORD& dwOutError, SString& strOutErrorContext) {
    
    if (!lpProcessInformation) {
        dwOutError = ERROR_INVALID_PARAMETER;
        strOutErrorContext = "InvalidParameter:NullProcessInfo";
        return FALSE;
    }
    
    ZeroMemory(lpProcessInformation, sizeof(PROCESS_INFORMATION));
    
    // Validate inputs
    if (!ValidatePath(lpApplicationName)) {
        dwOutError = ERROR_INVALID_PARAMETER;
        strOutErrorContext = "InvalidParameter:ApplicationName";
        return FALSE;
    }
    
    if (lpCommandLine.length() > Constants::MAX_COMMAND_LINE) {
        dwOutError = ERROR_INVALID_PARAMETER;
        strOutErrorContext = "InvalidParameter:CommandLineTooLong";
        return FALSE;
    }
    
    // Verify the application exists before trying to execute
    if (!FileExists(lpApplicationName)) {
        dwOutError = ERROR_FILE_NOT_FOUND;
        strOutErrorContext = "FileNotFound";
        return FALSE;
    }
    
    // Try CreateProcess first
    STARTUPINFOW startupInfo = {sizeof(STARTUPINFOW)};
    std::wstring cmdLineCopy = FromUTF8(lpCommandLine);
    std::wstring appNameW = FromUTF8(lpApplicationName);
    std::wstring currentDirW = FromUTF8(lpCurrentDirectory);
    
    if (CreateProcessW(appNameW.c_str(),
                      cmdLineCopy.empty() ? nullptr : &cmdLineCopy[0],  // Must be mutable buffer
                      nullptr, nullptr, FALSE, 0, nullptr,
                      currentDirW.c_str(),
                      &startupInfo, lpProcessInformation)) {
        
        if (lpProcessInformation->hProcess && 
            lpProcessInformation->hProcess != INVALID_HANDLE_VALUE) {
            return TRUE;
        }
        
        dwOutError = ERROR_INVALID_HANDLE;
        strOutErrorContext = "InvalidProcessHandle";
        return FALSE;
    }
    
    // Fallback to ShellExecute
    std::vector<DWORD> processIdsBefore = GetGTAProcessList();
    
    if (!ShellExecuteNonBlocking("open", lpApplicationName, lpCommandLine, lpCurrentDirectory)) {
        dwOutError = GetLastError();
        strOutErrorContext = "ShellExecute";
        return FALSE;
    }
    
    // Find new process
    for (int retry = 0; retry < Constants::MAX_RETRIES; ++retry) {
        const auto currentProcesses = GetGTAProcessList();
        
        for (DWORD pid : currentProcesses) {
            if (pid <= 4 || ListContains(processIdsBefore, pid)) continue;
            
            lpProcessInformation->dwProcessId = pid;
            lpProcessInformation->hProcess = OpenProcess(
                PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE,
                FALSE, pid);
            
            if (lpProcessInformation->hProcess && 
                lpProcessInformation->hProcess != INVALID_HANDLE_VALUE) {
                return TRUE;
            }
        }
        
        Sleep(Constants::RETRY_DELAY_MS);
    }
    
    dwOutError = ERROR_INVALID_FUNCTION;
    strOutErrorContext = "FindPID:NotFound";
    return FALSE;
}

//////////////////////////////////////////////////////////
//
// LaunchGame
//
//////////////////////////////////////////////////////////
int LaunchGame(SString strCmdLine) {
    CheckAndShowModelProblems();
    CheckAndShowUpgradeProblems();
    CheckAndShowImgProblems();
    
    const SString strGTAPath = GetGTAPath();
    const SString strMTASAPath = GetMTASAPath();
    const SString strGTAEXEPath = GetGameExecutablePath().u8string();
    
    if (!ValidatePath(strGTAPath) || !ValidatePath(strMTASAPath) || !ValidatePath(strGTAEXEPath)) {
        DisplayErrorMessageBox(_("Invalid game paths detected."), _E("CL44"), "invalid-paths");
        return 5;
    }
    
    SetDllDirectory(PathJoin(strMTASAPath, "mta"));
    
    if (!CheckService(CHECK_SERVICE_PRE_CREATE) && !IsUserAdmin()) {
        RelaunchAsAdmin(strCmdLine, _("Fix configuration issue"));
        ExitProcess(EXIT_OK);
    }
    
    BeginD3DStuff();
    LogSettings();
    
    WatchDogBeginSection("L2");
    WatchDogBeginSection("L3");
    WatchDogBeginSection(WD_SECTION_NOT_USED_MAIN_MENU);
    
    // Sanitize command line
    SString sanitizedCmdLine = strCmdLine;
    const bool bDoneAdmin = sanitizedCmdLine.Contains("/done-admin");
    sanitizedCmdLine = sanitizedCmdLine.Replace(" /done-admin", "");
    
    if (sanitizedCmdLine.length() > Constants::MAX_URI_LENGTH) {
        sanitizedCmdLine = sanitizedCmdLine.Left(Constants::MAX_URI_LENGTH);
    }
    
    // Add post-update connection
    const SString strPostUpdateConnect = GetPostUpdateConnect();
    if (!strPostUpdateConnect.empty() && sanitizedCmdLine.empty() && 
        strPostUpdateConnect.length() < 512) {
        sanitizedCmdLine = SString("mtasa://%s", *strPostUpdateConnect);
    }
    
    // Launch GTA
    PROCESS_INFORMATION piLoadee = {};
    DWORD dwError;
    SString strErrorContext;
    
    if (!StartGtaProcess(strGTAEXEPath, sanitizedCmdLine, strGTAPath, 
                        &piLoadee, dwError, strErrorContext)) {
        WriteDebugEvent(SString("Loader - Process not created[%d (%s)]: %s", 
                              dwError, *strErrorContext, *strGTAEXEPath));
        
        if (dwError == ERROR_ELEVATION_REQUIRED && !bDoneAdmin) {
            RelaunchAsAdmin(sanitizedCmdLine + " /done-admin", _("Fix elevation required error"));
            ExitProcess(EXIT_OK);
        }
        
        const SString strError = GetSystemErrorMessage(dwError) + " (" + strErrorContext + ")";
        DisplayErrorMessageBox(SString(_("Could not start Grand Theft Auto: San Andreas. [%s]"), *strError),
                              _E("CL22"), "createprocess-fail&err=" + strError);
        EndD3DStuff();
        return 5;
    }
    
    WriteDebugEvent(SString("Loader - Process created: %s", *strGTAEXEPath));
    WriteDebugEvent(SString("Loader - Process ID: %lu", piLoadee.dwProcessId));
    
    SetOnQuitCommand("");
    ShowSplash(g_hInstance);
    
    DWORD dwExitCode = static_cast<DWORD>(-1);
    
    if (piLoadee.hProcess && piLoadee.hProcess != INVALID_HANDLE_VALUE) {
        BsodDetectionOnGameBegin();
        
        // Wait for game window
        DWORD status = WAIT_TIMEOUT;
        for (uint i = 0; i < Constants::MAX_WAIT_ITERATIONS && status == WAIT_TIMEOUT; ++i) {
            status = WaitForSingleObject(piLoadee.hProcess, Constants::PROCESS_WAIT_TIMEOUT);
            
            if (!WatchDogIsSectionOpen("L3")) break;
            
            if (IsDeviceSelectionDialogOpen(piLoadee.dwProcessId) && i > 0) {
                --i;  // Don't count this iteration
                Sleep(100);
            }
        }
        
        HideSplash();
        
        // Handle stuck process
        if (status == WAIT_TIMEOUT) {
            CStuckProcessDetector detector(piLoadee.hProcess, 5000);
            while (status == WAIT_TIMEOUT && WatchDogIsSectionOpen("L3")) {
                if (detector.UpdateIsStuck()) {
                    WriteDebugEvent("Detected stuck process at startup");
                    if (MessageBoxUTF8(0, _("GTA: San Andreas may not have launched correctly. Terminate it?"),
                                      _("Information") + _E("CL25"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES) {
                        TerminateProcess(piLoadee.hProcess, 1);
                    }
                    break;
                }
                status = WaitForSingleObject(piLoadee.hProcess, Constants::PROCESS_WAIT_TIMEOUT);
            }
        }
        
        // Wait for game exit
        WriteDebugEvent("Loader - Wait for game to exit");
        DWORD totalWaitTime = 0;
        
        while (status == WAIT_TIMEOUT && totalWaitTime < Constants::MAX_WAIT_TIME) {
            status = WaitForSingleObject(piLoadee.hProcess, 1500);
            totalWaitTime += 1500;
            
            CStuckProcessDetector detector(piLoadee.hProcess, 5000);
            while (status == WAIT_TIMEOUT && WatchDogIsSectionOpen("Q0")) {
                if (detector.UpdateIsStuck()) {
                    WriteDebugEvent("Detected stuck process at quit");
                    #ifndef MTA_DEBUG
                        TerminateProcess(piLoadee.hProcess, 1);
                        status = WAIT_FAILED;
                        break;
                    #endif
                }
                status = WaitForSingleObject(piLoadee.hProcess, Constants::PROCESS_WAIT_TIMEOUT);
                totalWaitTime += Constants::PROCESS_WAIT_TIMEOUT;
                
                if (totalWaitTime >= Constants::MAX_WAIT_TIME) {
                    WriteDebugEvent("Maximum wait time exceeded");
                    TerminateProcess(piLoadee.hProcess, 1);
                    status = WAIT_FAILED;
                    break;
                }
            }
        }
        
        BsodDetectionOnGameEnd();
        
        if (!GetExitCodeProcess(piLoadee.hProcess, &dwExitCode)) {
            dwExitCode = static_cast<DWORD>(-1);
        }
    }
    
    AddReportLog(7104, "Loader - Finishing");
    WriteDebugEvent("Loader - Finishing");
    EndD3DStuff();
    
    // Cleanup
    if (piLoadee.hProcess && piLoadee.hProcess != INVALID_HANDLE_VALUE) {
        TerminateProcess(piLoadee.hProcess, 1);
        if (piLoadee.hThread && piLoadee.hThread != INVALID_HANDLE_VALUE) {
            CloseHandle(piLoadee.hThread);
        }
        CloseHandle(piLoadee.hProcess);
    }
    
    ReleaseSingleInstanceMutex();
    return dwExitCode;
}

//////////////////////////////////////////////////////////
//
// HandleOnQuitCommand
//
//////////////////////////////////////////////////////////
void HandleOnQuitCommand() {
    const SString strMTASAPath = GetMTASAPath();
    
    if (!ValidatePath(strMTASAPath)) {
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }
    
    SetCurrentDirectory(strMTASAPath);
    SetDllDirectory(strMTASAPath);
    
    SString strOnQuitCommand = GetRegistryValue("", "OnQuitCommand");
    if (strOnQuitCommand.length() > 4096) {
        WriteDebugEvent("OnQuitCommand too long, ignoring");
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }
    
    std::vector<SString> vecParts;
    strOnQuitCommand.Split("\t", vecParts);
    
    if (vecParts.size() < 5 || vecParts.size() > 100) {
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }
    
    // Validate parts
    for (const auto& part : vecParts) {
        if (part.length() > 1024) {
            WriteDebugEvent("OnQuitCommand has invalid parts");
            CheckService(CHECK_SERVICE_POST_GAME);
            return;
        }
    }
    
    SString strOperation = vecParts[0];
    SString strFile = vecParts[1];
    SString strParameters = vecParts[2];
    SString strDirectory = vecParts[3];
    SString strShowCmd = vecParts[4];
    
    // Validate operation
    if (strOperation == "restart") {
        strOperation = "open";
        strFile = PathJoin(strMTASAPath, MTA_EXE_NAME);
        
        if (!FileExists(strFile)) {
            WriteDebugEvent("MTA executable not found for restart");
            CheckService(CHECK_SERVICE_POST_GAME);
            return;
        }
        CheckService(CHECK_SERVICE_RESTART_GAME);
    } else if (strOperation == "open" || strOperation.empty()) {
        // Security checks
        if (strFile.Contains("..") || strFile.Contains("~/") || strFile.Contains("~\\")) {
            WriteDebugEvent("Suspicious file path in OnQuitCommand");
            CheckService(CHECK_SERVICE_POST_GAME);
            return;
        }
        
        // Check for executable extensions
        if (strFile.EndsWithI(".exe") || strFile.EndsWithI(".bat") || 
            strFile.EndsWithI(".cmd") || strFile.EndsWithI(".ps1")) {
            
            if (strFile.Contains(":") || strFile.BeginsWith("\\")) {
                if (!FileExists(strFile)) {
                    WriteDebugEvent("Executable in OnQuitCommand not found");
                    CheckService(CHECK_SERVICE_POST_GAME);
                    return;
                }
                
                // Don't allow execution from Windows or System directories
                const SString strFileLower = strFile.ToLower();
                if (strFileLower.Contains("\\windows\\") || 
                    strFileLower.Contains("\\system32\\") ||
                    strFileLower.Contains("\\syswow64\\")) {
                    WriteDebugEvent("Refusing to execute from system directory");
                    CheckService(CHECK_SERVICE_POST_GAME);
                    return;
                }
            }
        }
        CheckService(CHECK_SERVICE_POST_GAME);
    } else {
        WriteDebugEvent(SString("Unknown operation: %s", *strOperation));
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }
    
    // Sanitize parameters
    if (strParameters.Contains("&") || strParameters.Contains("|") || 
        strParameters.Contains(">") || strParameters.Contains("<") ||
        strParameters.Contains("&&") || strParameters.Contains("||")) {
        WriteDebugEvent("Suspicious parameters in OnQuitCommand, removing special chars");
        strParameters = strParameters.Replace("&", "")
                                     .Replace("|", "")
                                     .Replace(">", "")
                                     .Replace("<", "");
    }
    
    // Validate ShowCmd
    INT nShowCmd = SW_SHOWNORMAL;
    if (!strShowCmd.empty()) {
        int tempShowCmd = atoi(strShowCmd);
        if (tempShowCmd >= 0 && tempShowCmd <= 11) {
            nShowCmd = tempShowCmd;
        }
    }
    
    // Execute command
    if (!strOperation.empty() && !strFile.empty()) {
        WriteDebugEvent(SString("Executing OnQuitCommand: op=%s, file=%s", *strOperation, *strFile));
        ShellExecuteNonBlocking(
            strOperation.empty() ? NULL : strOperation.c_str(),
            strFile.c_str(),
            strParameters.empty() ? NULL : strParameters.c_str(),
            strDirectory.empty() ? NULL : strDirectory.c_str(),
            nShowCmd
        );
    }
}
