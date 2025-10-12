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

// Function must be at the start to fix odd compile error (Didn't happen locally but does in build server)
namespace
{
    bool ValidatePath(const SString& path)
    {
        // Placeholder
        return true;
    }
}

//////////////////////////////////////////////////////////
//
// CheckLibVersions
//
// Check lib versions are correct
//
//////////////////////////////////////////////////////////
void CheckLibVersions()
{
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
    for (uint i = 0; i < NUMELMS(moduleList); i++)
    {
        SString strFilename = moduleList[i];
#ifdef MTA_DEBUG
        strFilename = ExtractBeforeExtension(strFilename) + "_d." + ExtractExtension(strFilename);
#endif
        SString fullPath = CalcMTASAPath(strFilename);
        if (!ValidatePath(fullPath))
        {
            DisplayErrorMessageBox(SStringX(_("Invalid module path detected.\n") + SString("\n[%s]\n", *strFilename)),
                                  _E("CL49"), "invalid-module-path");
            ExitProcess(EXIT_ERROR);
        }
        
        SLibVersionInfo fileInfo;
        if (FileExists(fullPath))
        {
            SString strFileVersion = "0.0.0.0";
            if (GetLibVersionInfo(fullPath, &fileInfo))
            {
                // Validate version numbers
                if (fileInfo.dwFileVersionMS > 0 && fileInfo.dwFileVersionMS < MAXDWORD &&
                    fileInfo.dwFileVersionLS > 0 && fileInfo.dwFileVersionLS < MAXDWORD)
                {
                    strFileVersion = SString("%d.%d.%d.%d",
                                            fileInfo.dwFileVersionMS >> 16, fileInfo.dwFileVersionMS & 0xFFFF,
                                            fileInfo.dwFileVersionLS >> 16, fileInfo.dwFileVersionLS & 0xFFFF);
                }
            }

            if (strReqFileVersion.empty())
            {
                strReqFileVersion = strFileVersion;
            }
            else if (strReqFileVersion != strFileVersion)
            {
                DisplayErrorMessageBox(SStringX(_("File version mismatch error. Reinstall MTA:SA if you experience problems.\n") +
                                               SString("\n[%s %s/%s]\n", *strFilename, *strFileVersion, *strReqFileVersion)),
                                      _E("CL40"), "bad-file-version");
                break;
            }
        }
        else
        {
            DisplayErrorMessageBox(SStringX(_("Some files are missing. Reinstall MTA:SA if you experience problems.\n") +
                                           SString("\n[%s]\n", *strFilename)),
                                  _E("CL41"), "missing-file");
            break;
        }
    }
#endif

    // Check for Windows 'Safe Mode'
    if (GetSystemMetrics(SM_CLEANBOOT) != 0)
    {
        DisplayErrorMessageBox(SStringX(_("MTA:SA is not compatible with Windows 'Safe Mode'.\n\nPlease restart your PC.\n")),
                              _E("CL42"), "safe-mode");
        ExitProcess(EXIT_ERROR);
    }
}

// Enum declarations for WSC health
DECLARE_ENUM(WSC_SECURITY_PROVIDER_HEALTH)
IMPLEMENT_ENUM_BEGIN(WSC_SECURITY_PROVIDER_HEALTH)
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_GOOD, "good")
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED, "not_monitored")
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_POOR, "poor")
    ADD_ENUM(WSC_SECURITY_PROVIDER_HEALTH_SNOOZE, "snooze")
IMPLEMENT_ENUM_END("wsc_health")

// Simplified localization dummy class
class CLocalizationDummy final : public CLocalizationInterface
{
public:
    SString Translate(const SString& strMessage) override { return strMessage; }
    SString TranslateWithContext(const SString& strContext, const SString& strMessage) override { return strMessage; }
    SString TranslatePlural(const SString& strSingular, const SString& strPlural, int iNum) override { return strPlural; }
    SString TranslatePluralWithContext(const SString& strContext, const SString& strSingular,
                                       const SString& strPlural, int iNum) override { return strPlural; }
    std::vector<SString> GetAvailableLocales() override { return {}; }
    bool                 IsLocalized() override { return false; }
    SString              GetLanguageDirectory(CLanguage* pLanguage = nullptr) override { return ""; }
    SString              GetLanguageCode() override { return "en_US"; }
    SString              GetLanguageName() override { return "English"; }
};

// Global localization interface
CLocalizationInterface* g_pLocalization = new CLocalizationDummy();

//////////////////////////////////////////////////////////
//
// InitLocalization
//
// Start localization thingmy
//
//////////////////////////////////////////////////////////
void InitLocalization(bool bShowErrors)
{
    static bool bInitialized = false;
    if (bInitialized) return;

    // Check for core.dll
    const SString strCoreDLL = PathJoin(GetLaunchPath(), "mta", MTA_DLL_NAME);
    if (!FileExists(strCoreDLL))
    {
        if (!bShowErrors) return;
        DisplayErrorMessageBox("Load failed. Please ensure that the file " MTA_DLL_NAME
                              " is in the modules directory within the MTA root directory.",
                              _E("CL23"), "core-missing");
        ExitProcess(EXIT_ERROR);
    }

    // Setup DLL search paths
    const SString strMTASAPath = GetMTASAPath();
    SetDllDirectory(PathJoin(strMTASAPath, "mta"));

    // See if xinput is loadable (XInput9_1_0.dll or xinput1_3.dll)
    const DWORD   dwPrevMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    const char*   xinputModules[] = {"XInput9_1_0", "xinput1_3"};

    for (uint i = 0; i < NUMELMS(xinputModules); ++i)
    {
        SString strDllName = SString("%s.dll", xinputModules[i]);

        HMODULE hXInputModule = LoadLibrary(strDllName);
        if (hXInputModule)
        {
            FreeLibrary(hXInputModule);            // Exists already - no need to copy
        }
        else
        {
            // Try to copy from our directory
            const SString strDest = PathJoin(strMTASAPath, "mta", strDllName);
            if (!FileExists(strDest))
            {
                SString strSrc = PathJoin(strMTASAPath, "mta", SString("%s_mta.dll", xinputModules[i]));
                if (!FileExists(strSrc))
                {
                    strSrc = PathJoin(GetLaunchPath(), "mta", SString("%s_mta.dll", xinputModules[i]));
                }
                FileCopy(strSrc, strDest);
            }
        }
    }

    // Load core.dll
    if (bShowErrors) SetErrorMode(dwPrevMode);
    HMODULE hCoreModule = LoadLibraryEx(strCoreDLL, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    SetErrorMode(dwPrevMode);

    if (!hCoreModule)
    {
        if (!bShowErrors) return;
        DisplayErrorMessageBox("Loading core failed. Please ensure that the latest DirectX is correctly installed.",
                              _E("CL24"), "core-not-loadable");
        ExitProcess(EXIT_ERROR);
    }

    LoaderResolveCrashHandlerExports(hCoreModule);

    // Get locale
    SString strLocale = GetApplicationSetting("locale");
    if (strLocale.empty())
    {
        // Get system locale
        char originalLocale[256] = {0};
        if (const char* locale = setlocale(LC_ALL, NULL))
        {
            strncpy(originalLocale, locale, sizeof(originalLocale) - 1);
            originalLocale[sizeof(originalLocale) - 1] = '\0';
        }

        setlocale(LC_ALL, "");
        if (const char* newLocale = setlocale(LC_ALL, NULL))
        {
            strLocale = newLocale;
        }

        // Restore original locale
        if (originalLocale[0])
        {
            setlocale(LC_ALL, originalLocale);
        }
    }

    // Create localization interface
    typedef CLocalizationInterface*(__cdecl* CreateLocalizationFunc)(SString);
    auto pFunc = reinterpret_cast<CreateLocalizationFunc>(static_cast<void*>(GetProcAddress(hCoreModule, "L10n_CreateLocalization")));

    if (!pFunc)
    {
        FreeLibrary(hCoreModule);
        if (bShowErrors)
        {
            DisplayErrorMessageBox("Loading localization failed. Please ensure that MTA San Andreas 1.6\\MTA\\locale is accessible.",
                                  _E("CL26"), "localization-not-loadable");
        }
        return;
    }

    CLocalizationInterface* pLocalization = pFunc(strLocale);
    if (!pLocalization)
    {
        FreeLibrary(hCoreModule);
        if (bShowErrors)
        {
            DisplayErrorMessageBox("Loading localization failed. Please ensure that MTA San Andreas 1.6\\MTA\\locale is accessible.",
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
void HandleTrouble()
{
    if (CheckAndShowFileOpenFailureMessage())
        return;

#if !defined(MTA_DEBUG) && MTASA_VERSION_TYPE != VERSION_TYPE_CUSTOM
    int iResponse = MessageBoxUTF8(NULL, _("Are you having problems running MTA:SA?.\n\nDo you want to revert to an earlier version?"),
                                   "MTA: San Andreas" + _E("CL07"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
    if (iResponse == IDYES)
    {
        BrowseToSolution("crashing-before-gtagame", TERMINATE_PROCESS);
    }
#endif
}

//////////////////////////////////////////////////////////
//
// HandleResetSettings
//
//////////////////////////////////////////////////////////
void HandleResetSettings()
{
    if (CheckAndShowFileOpenFailureMessage())
        return;

    CheckAndShowMissingFileMessage();

    SString strSaveFilePath = PathJoin(GetSystemPersonalPath(), "GTA San Andreas User Files");
    SString strSettingsFilename = PathJoin(strSaveFilePath, "gta_sa.set");
    SString strSettingsFilenameBak = PathJoin(strSaveFilePath, "gta_sa_old.set");

    if (FileExists(strSettingsFilename))
    {
        int iResponse = MessageBoxUTF8(NULL,
                                       _("There seems to be a problem launching MTA:SA.\nResetting GTA settings can sometimes fix this problem.\n\nDo you want "
                                         "to reset GTA settings now?"),
                                       "MTA: San Andreas" + _E("CL08"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        if (iResponse == IDYES)
        {
            FileDelete(strSettingsFilenameBak);
            FileRename(strSettingsFilename, strSettingsFilenameBak);
            FileDelete(strSettingsFilename);

            // Also reset NVidia Optimus "remember option"
            SetApplicationSettingInt("nvhacks", "optimus-remember-option", 0);

            if (!FileExists(strSettingsFilename))
            {
                AddReportLog(4053, "Deleted gta_sa.set");
                MessageBoxUTF8(NULL, _("GTA settings have been reset.\n\nPress OK to continue."), "MTA: San Andreas", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
            }
            else
            {
                AddReportLog(5054, SString("Delete gta_sa.set failed with '%s'", *strSettingsFilename));
                MessageBoxUTF8(NULL, SString(_("File could not be deleted: '%s'"), *strSettingsFilename), "Error" + _E("CL09"),
                               MB_OK | MB_ICONWARNING | MB_TOPMOST);
            }
        }
    }
    else
    {
        // No settings to delete, or can't find them
        int iResponse = MessageBoxUTF8(NULL, _("Are you having problems running MTA:SA?.\n\nDo you want to see some online help?"), "MTA: San Andreas",
                                       MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
        if (iResponse == IDYES)
        {
            BrowseToSolution("crashing-before-gtalaunch", TERMINATE_PROCESS);
        }
    }
}

//////////////////////////////////////////////////////////
//
// HandleNotUsedMainMenu
//
// Changes to window mode if fullscreen and not used the main menu much
//
//////////////////////////////////////////////////////////
void HandleNotUsedMainMenu()
{
    AddReportLog(9310, "Loader - HandleNotUsedMainMenu");

    // Check current display mode in coreconfig.xml
    {
        SString strCoreConfigFilename = CalcMTASAPath(PathJoin("mta", "config", "coreconfig.xml"));
        SString strCoreConfig;
        FileLoad(strCoreConfigFilename, strCoreConfig);
        SString strWindowed = strCoreConfig.SplitRight("<display_windowed>").Left(1);
        SString strFullscreenStyle = strCoreConfig.SplitRight("<display_fullscreen_style>").Left(1);

        if (strFullscreenStyle == "1")
        {
            AddReportLog(9315, "Loader - HandleNotUsedMainMenu - Already Borderless window");
        }
        else if (!strWindowed.empty() && !strFullscreenStyle.empty())
        {
            if (strWindowed == "0" && strFullscreenStyle == "0")
            {
                // Inform user
                SString strMessage = _("Are you having problems running MTA:SA?.\n\nDo you want to change the following setting?");
                strMessage += "\n" + _("Fullscreen mode:") + " -> " + _("Borderless window");
                HideSplash();
                int iResponse = MessageBoxUTF8(NULL, strMessage, "MTA: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
                if (iResponse == IDYES)
                {
                    // Change to borderless window mode
                    strCoreConfig = strCoreConfig.Replace("<display_fullscreen_style>0", "<display_fullscreen_style>1");
                    FileSave(strCoreConfigFilename, strCoreConfig);
                    AddReportLog(9311, "Loader - HandleNotUsedMainMenu - User change to Borderless window");
                }
                else
                {
                    AddReportLog(9313, "Loader - HandleNotUsedMainMenu - User said no");
                }
            }
            else
            {
                AddReportLog(9314, "Loader - HandleNotUsedMainMenu - Mode not fullscreen standard");
            }
        }
        else
        {
            // If no valid settings file yet, do the change without asking
            strCoreConfig = "<mainconfig><settings><display_fullscreen_style>1</display_fullscreen_style></settings></mainconfig>";
            FileSave(strCoreConfigFilename, strCoreConfig);
            AddReportLog(9312, "Loader - HandleNotUsedMainMenu - Set Borderless window");
        }
    }

    // Check for problem processes
    struct
    {
        std::vector<SString> matchTextList;
        const char*          szProductName;
        const char*          szTrouble;
    } procItems[] = {
        {{"\\Evolve"}, "Evolve", "not-used-menu-evolve"},
        {{"\\GbpSv.exe", "Diebold\\Warsaw"}, "GAS Tecnologia - G-Buster Browser Defense", "not-used-menu-gbpsv"}
    };

    bool foundProblemProcess = false;
    const size_t MAX_PROCESS_CHECKS = 100;
    size_t processesChecked = 0;
    
    for (uint i = 0; i < NUMELMS(procItems) && !foundProblemProcess; i++)
    {
        auto processList = MyEnumProcesses(true);
        
        if (processList.size() > 1000)
        {
            WriteDebugEvent("Too many processes to check");
            break;
        }
        
        for (auto processId : processList)
        {
            if (++processesChecked > MAX_PROCESS_CHECKS)
            {
                WriteDebugEvent("Process check limit reached in HandleNotUsedMainMenu");
                break;
            }
            
            SString strProcessFilename = GetProcessPathFilename(processId);
            
            // Validate process path
            if (!strProcessFilename.empty() && strProcessFilename.length() < MAX_PATH)
            {
                for (const auto& strMatchText : procItems[i].matchTextList)
                {
                    if (strProcessFilename.ContainsI(strMatchText))
                    {
                        SString strMessage = _("Are you having problems running MTA:SA?.\n\nTry disabling the following products for GTA and MTA:");
                        strMessage += "\n\n";
                        strMessage += procItems[i].szProductName;
                        DisplayErrorMessageBox(strMessage, _E("CL43"), procItems[i].szTrouble);
                        foundProblemProcess = true;
                        break;
                    }
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
void HandleCustomStartMessage()
{
    SString strStartMessage = GetApplicationSetting("diagnostics", "start-message");
    SString strTrouble = GetApplicationSetting("diagnostics", "start-message-trouble");

    if (strStartMessage.empty())
        return;

    SetApplicationSetting("diagnostics", "start-message", "");
    SetApplicationSetting("diagnostics", "start-message-trouble", "");

    if (strStartMessage.BeginsWith("vdetect"))
    {
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
// Various checks to alert the user to problems which will make MTA not work correctly
// Some checks are cumulative triggers. i.e. they only trigger after a certain number of failures.
//
// Commands are often stored in the registry by the installer and client.
//
// "L0" is opened before the launch sequence and is closed if MTA shutsdown with no error
// "L1" is opened before the launch sequence and is closed if GTA is succesfully started
// "CR1" is a counter which is incremented if GTA was not started and MTA shutsdown with an error
//
// "L2" is opened before the launch sequence and is closed if the GTA loading screen is shown
// "CR2" is a counter which is incremented at startup, if the previous run didn't make it to the loading screen
//
// "L3" is opened before the launch sequence and is closed if the GTA loading screen is shown, or a startup problem is handled elsewhere
//
//////////////////////////////////////////////////////////
void PreLaunchWatchDogs()
{
    // Note: Single instance mutex is properly checked later in the launch sequence
    // Creating it here just ensures we acquire it early, but we shouldn't assert
    // because after a crash the mutex won't exist (OS releases it)
    CreateSingleInstanceMutex();

    // Check for unclean stop on previous run
#ifndef MTA_DEBUG
    if (WatchDogIsSectionOpen("L0"))
    {
        WatchDogSetUncleanStop(true);            // Flag to maybe do things differently if MTA exit code on last run was not 0
        CheckAndShowFileOpenFailureMessage();
    }
    else
#endif
        WatchDogSetUncleanStop(false);

    // Check if crash dump file exists from the game
    SString strCrashFlagFilename = CalcMTASAPath("mta\\core.log.flag");
    if (FileExists(strCrashFlagFilename))
    {
        FileDelete(strCrashFlagFilename);
        WatchDogSetLastRunCrash(true);            // Flag to maybe do things differently if MTA crashed last run
    }
    else
    {
        WatchDogSetLastRunCrash(false);
    }

    // Reset counter if gta game was run last time
    if (!WatchDogIsSectionOpen("L1"))
        WatchDogClearCounter("CR1");

    // If crashed 3 times in a row before starting the game, do something
    if (WatchDogGetCounter("CR1") >= 3)
    {
        WatchDogReset();
        HandleTrouble();
    }

    // Check for possible gta_sa.set problems
    if (WatchDogIsSectionOpen("L2"))
    {
        WatchDogIncCounter("CR2");
        WatchDogCompletedSection("L2");
    }
    else
    {
        WatchDogClearCounter("CR2");
    }

    // If didn't reach loading screen 5 times in a row, do something
    if (WatchDogGetCounter("CR2") >= 5)
    {
        WatchDogClearCounter("CR2");
        HandleResetSettings();
    }

    // Check for possible fullscreen problems
    if (WatchDogIsSectionOpen(WD_SECTION_NOT_USED_MAIN_MENU))
    {
        int iChainLimit;
        if (WatchDogIsSectionOpen(WD_SECTION_POST_INSTALL))
            iChainLimit = 1;            // Low limit if just installed
        else if (GetApplicationSettingInt("times-connected") == 0)
            iChainLimit = 2;            // Medium limit if have never connected in the past
        else
            iChainLimit = 3;            // Normal limit

        WatchDogCompletedSection(WD_SECTION_NOT_USED_MAIN_MENU);
        WatchDogIncCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
        if (WatchDogGetCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU) >= iChainLimit)
        {
            WatchDogClearCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
            HandleNotUsedMainMenu();
        }
    }
    else
    {
        WatchDogClearCounter(WD_COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU);
    }

    // Clear down freeze on quit detection
    WatchDogCompletedSection("Q0");

    WatchDogBeginSection("L0");            // Gets closed if MTA exits with a return code of 0
    WatchDogBeginSection("L1");            // Gets closed when GTA is launched
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
void PostRunWatchDogs(int iReturnCode)
{
    if (iReturnCode == 0)
    {
        WatchDogClearCounter("CR1");
        WatchDogCompletedSection("L0");
    }
}

//////////////////////////////////////////////////////////
//
// HandleIfGTAIsAlreadyRunning
//
// Check for and maybe stop a running GTA process
//
//////////////////////////////////////////////////////////
void HandleIfGTAIsAlreadyRunning()
{
    if (IsGTARunning())
    {
        if (MessageBoxUTF8(0,
                          _("An instance of GTA: San Andreas is already running. It needs to be terminated before MTA:SA can be started. Do you want to do that now?"),
                          _("Information") + _E("CL10"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
        {
            TerminateOtherMTAIfRunning();
            TerminateGTAIfRunning();
            if (IsGTARunning())
            {
                MessageBoxUTF8(0, _("Unable to terminate GTA: San Andreas. If the problem persists, please restart your computer."),
                               _("Information") + _E("CL11"), MB_OK | MB_ICONERROR | MB_TOPMOST);
                ExitProcess(EXIT_ERROR);
            }
        }
        else
        {
            ExitProcess(EXIT_OK);
        }
    }
}

//////////////////////////////////////////////////////////
//
// HandleSpecialLaunchOptions
//
// Check and handle commands (from the installer)
//
//////////////////////////////////////////////////////////
void HandleSpecialLaunchOptions()
{
    // Handle service install request from the installer
    if (CommandLineContains("/kdinstall"))
    {
        UpdateMTAVersionApplicationSetting(true);
        WatchDogReset();
        WatchDogBeginSection(WD_SECTION_POST_INSTALL);
        ExitProcess(CheckService(CHECK_SERVICE_POST_INSTALL) ? EXIT_OK : EXIT_ERROR);
    }

    // Handle service uninstall request from the installer
    if (CommandLineContains("/kduninstall"))
    {
        UpdateMTAVersionApplicationSetting(true);
        ExitProcess(CheckService(CHECK_SERVICE_PRE_UNINSTALL) ? EXIT_OK : EXIT_ERROR);
    }

    // No run 4 sure check
    if (CommandLineContains("/nolaunch"))
    {
        ExitProcess(EXIT_OK);
    }
}

//////////////////////////////////////////////////////////
//
// HandleDuplicateLaunching
//
// Handle duplicate launching, or running from mtasa:// URI
//
//////////////////////////////////////////////////////////
void HandleDuplicateLaunching()
{
    LPSTR lpCmdLine = GetCommandLine();
    if (!lpCmdLine) ExitProcess(EXIT_ERROR);

    //  Validate command line length
    const size_t cmdLineLen = strlen(lpCmdLine);
    if (cmdLineLen >= 32768) ExitProcess(EXIT_ERROR);            // Max Windows command line length

    bool bIsCrashDialog = (cmdLineLen > 0 && strstr(lpCmdLine, "install_stage=crashed") != NULL);

    int recheckTime = 2000;            // 2 seconds recheck time

    // We can only do certain things if MTA is already running
    // Unless this is a crash dialog launch, which needs to run alongside the crashed instance
    //
    // Normal behavior: Loop here if mutex is held, try to pass command line to existing instance
    // Crash dialog: Skip this entirely (bIsCrashDialog=true), proceed directly to showing dialog
    while (!bIsCrashDialog && !CreateSingleInstanceMutex())
    {
        if (cmdLineLen > 0)
        {
            // Command line args present, so pass it on
            HWND hwMTAWindow = FindWindow(NULL, "MTA: San Andreas");
            #ifdef MTA_DEBUG
                if (!hwMTAWindow) hwMTAWindow = FindWindow(NULL, "MTA: San Andreas [DEBUG]");
            #endif

            if (hwMTAWindow)
            {
                // Parse URI from command line
                LPWSTR szCommandLine = GetCommandLineW();
                if (!szCommandLine) continue;

                int     numArgs = 0;
                LPWSTR* aCommandLineArgs = CommandLineToArgvW(szCommandLine, &numArgs);

                if (aCommandLineArgs && numArgs > 0 && numArgs < 1000)
                {
                    for (int i = 1; i < numArgs; ++i)
                    {
                        if (!aCommandLineArgs[i]) continue;

                        WString wideArg = aCommandLineArgs[i];
                        if (wideArg.length() > 8 && wideArg.length() < 2048 &&            // Max MTA connect URI length
                            WStringX(wideArg).BeginsWith(L"mtasa://"))
                        {
                            SString strConnectInfo = ToUTF8(wideArg);
                            // Check for null bytes and validate content
                            if (strConnectInfo.find('\0') != SString::npos)
                                continue;

                            // Additional validation for mtasa:// URI content
                            if (strConnectInfo.Contains("..") || strConnectInfo.Contains("\\\\"))
                                continue;

                            COPYDATASTRUCT cdStruct = {URI_CONNECT, static_cast<DWORD>(strConnectInfo.length() + 1),
                                                       const_cast<char*>(strConnectInfo.c_str())};

                            // Use SendMessageTimeout to prevent hanging
                            DWORD_PTR dwResult = 0;
                            SendMessageTimeout(hwMTAWindow, WM_COPYDATA, NULL, reinterpret_cast<LPARAM>(&cdStruct), SMTO_ABORTIFHUNG | SMTO_BLOCK,
                                             5000, &dwResult);
                            break;
                        }
                    }
                    LocalFree(aCommandLineArgs);
                }
                ExitProcess(EXIT_ERROR);
            }
            else if (recheckTime > 0)
            {
                Sleep(500);
                recheckTime -= 500;
                continue;
            }
            else
            {
                const SString strMessage = _("Trouble restarting MTA:SA\n\n"
                                            "If the problem persists, open Task Manager and\n"
                                            "stop the 'gta_sa.exe' and 'Multi Theft Auto.exe' processes\n\n\n"
                                            "Try to launch MTA:SA again?");

                if (MessageBoxUTF8(0, strMessage, _("Error") + _E("CL04"), MB_ICONWARNING | MB_YESNO | MB_TOPMOST) == IDYES)
                {
                    TerminateGTAIfRunning();
                    TerminateOtherMTAIfRunning();

                    const SString exePath = PathJoin(GetMTASAPath(), MTA_EXE_NAME);
                    if (FileExists(exePath))
                    {
                        ShellExecuteNonBlocking("open", exePath, lpCmdLine);
                    }
                }
                ExitProcess(EXIT_ERROR);
            }
        }
        else
        {
            // No command line args, so just bring to front
            if (!IsGTARunning() && !IsOtherMTARunning())
            {
                MessageBoxUTF8(0, _("Another instance of MTA is already running.\n\n"
                                   "If this problem persists, please restart your computer"),
                              _("Error") + _E("CL05"), MB_ICONERROR | MB_TOPMOST);
            }
            else if (MessageBoxUTF8(0, _("Another instance of MTA is already running.\n\n"
                                          "Do you want to terminate it?"),
                                     _("Error") + _E("CL06"), MB_ICONQUESTION | MB_YESNO | MB_TOPMOST) == IDYES)
            {
                TerminateGTAIfRunning();
                TerminateOtherMTAIfRunning();

                const SString exePath = PathJoin(GetMTASAPath(), MTA_EXE_NAME);
                if (FileExists(exePath))
                {
                    ShellExecuteNonBlocking("open", exePath, lpCmdLine);
                }
            }
            ExitProcess(EXIT_ERROR);
        }
    }

    if (bIsCrashDialog)
    {
        CreateSingleInstanceMutex();
    }
}

//////////////////////////////////////////////////////////
//
// ValidateGTAPath
//
// Ensure we have a valid GTA path
//
//////////////////////////////////////////////////////////
void ValidateGTAPath()
{
    // Get path to GTA
    const ePathResult result = DiscoverGTAPath(true);

    if (result == GAME_PATH_MISSING)
    {
        DisplayErrorMessageBox(_("Registry entries are missing. Please reinstall Multi Theft Auto: San Andreas."),
                              _E("CL12"), "reg-entries-missing");
        ExitProcess(EXIT_ERROR);
    }
    else if (result == GAME_PATH_UNICODE_CHARS)
    {
        DisplayErrorMessageBox(_("The path to your installation of GTA: San Andreas contains unsupported (unicode) characters. "
                                "Please move your Grand Theft Auto: San Andreas installation to a compatible path that contains "
                                "only standard ASCII characters and reinstall Multi Theft Auto: San Andreas."),
                              _E("CL13"));
        ExitProcess(EXIT_ERROR);
    }

    const SString strGTAPath = GetGTAPath();
    const SString strMTASAPath = GetMTASAPath();

    // Check for semicolon in install path (causes problems)
    if (strGTAPath.Contains(";") || strMTASAPath.Contains(";"))
    {
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
// Maybe warn user if no anti-virus running
//
//////////////////////////////////////////////////////////
void CheckAntiVirusStatus()
{
    std::vector<SString> enabledList, disabledList;
    GetWMIAntiVirusStatus(enabledList, disabledList);

    if (enabledList.size() > 100) enabledList.resize(100);
    if (disabledList.size() > 100) disabledList.resize(100);

    WSC_SECURITY_PROVIDER_HEALTH health = static_cast<WSC_SECURITY_PROVIDER_HEALTH>(-1);

    // Get windows defender status
    static const auto WscGetHealth = []() -> decltype(&WscGetSecurityProviderHealth) {
        if (HMODULE wscapi = LoadLibraryW(L"Wscapi.dll"))
        {
            auto function = static_cast<void*>(GetProcAddress(wscapi, "WscGetSecurityProviderHealth"));
            return reinterpret_cast<decltype(&WscGetSecurityProviderHealth)>(function);
        }
        return nullptr;
    }();

    if (WscGetHealth)
    {
        WscGetHealth(WSC_SECURITY_PROVIDER_ANTIVIRUS, &health);
    }

    SString strStatus;
    strStatus.reserve(512);            // Pre-allocate to prevent reallocation
    strStatus += SString("AV health: %s (%d)", *EnumToString(health), static_cast<int>(health));

    const size_t maxStatusItems = 10;
    for (size_t i = 0; i < enabledList.size() && i < maxStatusItems; ++i)
    {
        if (strStatus.length() > 400) break;            // Prevent status string from growing too large
        strStatus += SString(" [Ena%zu:%s]", i, *enabledList[i]);
    }
    for (size_t i = 0; i < disabledList.size() && i < maxStatusItems; ++i)
    {
        if (strStatus.length() > 400) break;            // Prevent status string from growing too large
        strStatus += SString(" [Dis%zu:%s]", i, *disabledList[i]);
    }

    WriteDebugEvent(strStatus);

    if (enabledList.empty() && health != WSC_SECURITY_PROVIDER_HEALTH_GOOD)
    {
        bool showWarning = (health != WSC_SECURITY_PROVIDER_HEALTH_NOTMONITORED);

        if (showWarning)
        {
            // Check for AV in loaded modules
            static const char* avProducts[] = {
                "antivirus", "anti-virus", "Avast", "AVG", "Avira", "NOD32", "ESET",
                "F-Secure", "Faronics", "Kaspersky", "McAfee", "Norton", "Symantec",
                "Panda", "Trend Micro"
            };

            // Check for AV in loaded modules
            std::array<HMODULE, 1024> modules;
            DWORD cbNeeded;

            if (EnumProcessModules(GetCurrentProcess(), modules.data(),
                                  static_cast<DWORD>(sizeof(modules)), &cbNeeded))
            {
                DWORD moduleCount = cbNeeded / sizeof(HMODULE);
                const DWORD maxModules = static_cast<DWORD>(modules.size());
                
                if (cbNeeded > 0 && cbNeeded < MAXDWORD && (cbNeeded % sizeof(HMODULE)) == 0)
                {
                    if (moduleCount > maxModules)
                    {
                        WriteDebugEvent(SString("Warning: Process has %lu modules but array can only hold %lu", moduleCount, maxModules));
                        moduleCount = maxModules;
                    }

                    for (DWORD i = 0; i < moduleCount && showWarning; ++i)
                    {
                        if (!modules[i]) continue;

                        WCHAR modulePath[MAX_PATH * 2] = L"";
                        DWORD pathLen = GetModuleFileNameExW(GetCurrentProcess(), modules[i],
                                                            modulePath, NUMELMS(modulePath) - 1);
                        
                        // Ensure null-termination
                        if (pathLen > 0 && pathLen < NUMELMS(modulePath))
                        {
                            modulePath[pathLen] = L'\0';
                            
                            SLibVersionInfo libInfo;
                            if (GetLibVersionInfo(ToUTF8(modulePath), &libInfo))
                            {
                                for (uint j = 0; j < NUMELMS(avProducts); ++j)
                                {
                                    if (libInfo.strCompanyName.ContainsI(avProducts[j]) ||
                                        libInfo.strProductName.ContainsI(avProducts[j]))
                                    {
                                        showWarning = false;
                                        WriteDebugEvent(SString("AV (module) found: %s", *ToUTF8(modulePath)));
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    WriteDebugEvent("Invalid module enumeration result");
                }
            }

    // Check for running processes for AV
    if (showWarning)
    {
        auto processList = MyEnumProcesses(true);

        const size_t maxProcessesToCheck = 500;
        size_t processesChecked = 0;
        
        for (auto processId : processList)
        {
            if (++processesChecked > maxProcessesToCheck)
            {
                WriteDebugEvent("Process check limit reached");
                break;
            }
            
            SString processPath = GetProcessPathFilename(processId);
            if (!processPath.empty() && ValidatePath(processPath))
            {
                SLibVersionInfo libInfo;
                if (GetLibVersionInfo(processPath, &libInfo))
                {
                    for (uint i = 0; i < NUMELMS(avProducts); ++i)
                    {
                        if (libInfo.strCompanyName.ContainsI(avProducts[i]) ||
                            libInfo.strProductName.ContainsI(avProducts[i]))
                        {
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
// Basic check for some essential files
//
//////////////////////////////////////////////////////////
void CheckDataFiles()
{
    const SString strMTASAPath = GetMTASAPath();
    const SString strGTAPath = GetGTAPath();

    if (!ValidatePath(strMTASAPath) || !ValidatePath(strGTAPath))
    {
        DisplayErrorMessageBox(_("Invalid installation paths detected."), _E("CL45"), "invalid-install-paths");
        ExitProcess(EXIT_ERROR);
    }

    // No-op known incompatible/broken d3d9.dll versions from the launch directory
	// By using file version we account for variants as well. The array is extendable, but primarily for D3D9.dll 6.3.9600.17415 (MTA top 5 crash)
    {
        struct SIncompatibleVersion
        {
            int iMajor;
            int iMinor;
            int iBuild;
            int iRelease;
        };

        static const SIncompatibleVersion incompatibleVersions[] = {
            // The below entry (D3D9.dll 6.3.9600.17415) always crashes the user @ 0x0001F4B3 (CreateSurfaceLH).
            // Furthermore, it's not a graphical mod or functional. Some GTA:SA distributor just placed their own, outdated Win7 DLL in the folder.
            {6, 3, 9600, 17415},
            // The below entry (D3D9.dll 0.3.1.3) is a fully incompatible, modified ENB version ("DirectX 2.0") that crashes the user @ 0002A733
            {0, 3, 1, 3},
        };

        static bool bChecked = false;
        if (!bChecked)
        {
            bChecked = true;

			// Check all 3 game roots
            const std::vector<SString> directoriesToCheck = {
                GetLaunchPath(), // MTA installation folder root
                strGTAPath, // Real GTA:SA installation folder root. As chosen by DiscoverGTAPath()
                PathJoin(GetMTADataPath(), "GTA San Andreas"), // Proxy-mirror that MTA uses for core GTA data files (C:\ProgramData\MTA San Andreas All\<MTA major version>\GTA San Andreas)
            };

            for (const SString& directory : directoriesToCheck)
            {
                if (directory.empty())
                    continue;
                if (!ValidatePath(directory))
                    continue;

                const SString strD3dModuleFilename = PathJoin(directory, "d3d9.dll");
                if (!ValidatePath(strD3dModuleFilename) || !FileExists(strD3dModuleFilename))
                    continue;

                SharedUtil::SLibVersionInfo versionInfo = {};
                if (!SharedUtil::GetLibVersionInfo(strD3dModuleFilename, &versionInfo))
                    continue;

                bool bIsIncompatible = false;
                for (const SIncompatibleVersion& entry : incompatibleVersions)
                {
                    if (versionInfo.GetFileVersionMajor() == entry.iMajor &&
                        versionInfo.GetFileVersionMinor() == entry.iMinor &&
                        versionInfo.GetFileVersionBuild() == entry.iBuild &&
                        versionInfo.GetFileVersionRelease() == entry.iRelease)
                    {
                        bIsIncompatible = true;
                        break;
                    }
                }

                if (!bIsIncompatible)
                    continue;

                const SString strBackupModuleFilename = PathJoin(directory, "d3d9.bak.incompatible");
                const WString wideSourcePath = FromUTF8(strD3dModuleFilename);
                const WString wideBackupPath = FromUTF8(strBackupModuleFilename);

                if (FileExists(strBackupModuleFilename))
                {
                    SetFileAttributesW(wideBackupPath.c_str(), FILE_ATTRIBUTE_NORMAL);
                    DeleteFileW(wideBackupPath.c_str());
                }

                SetFileAttributesW(wideSourcePath.c_str(), FILE_ATTRIBUTE_NORMAL);

                bool bRenamed = MoveFileExW(wideSourcePath.c_str(), wideBackupPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
                if (!bRenamed)
                {
                    if (!CopyFileW(wideSourcePath.c_str(), wideBackupPath.c_str(), FALSE))
                        continue;

                    SetFileAttributesW(wideBackupPath.c_str(), FILE_ATTRIBUTE_NORMAL);

                    if (!DeleteFileW(wideSourcePath.c_str()))
                        continue;

                    bRenamed = true;
                }

                if (bRenamed)
                {
                    SetFileAttributesW(wideBackupPath.c_str(), FILE_ATTRIBUTE_NORMAL);
                }
            }
        }
    }

    // Check for essential MTA files
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

    for (uint i = 0; i < NUMELMS(dataFiles); ++i)
    {
        const SString filePath = PathJoin(strMTASAPath, dataFiles[i]);
        if (!ValidatePath(filePath) || !FileExists(filePath))
        {
            DisplayErrorMessageBox(_("Load failed. Please ensure that the latest data files have been installed correctly."),
                                  _E("CL16"), "mta-datafiles-missing");
            ExitProcess(EXIT_ERROR);
        }
    }

    // Check for client deathmatch module
    if (!FileExists(PathJoin(strMTASAPath, CHECK_DM_CLIENT_NAME)))
    {
        DisplayErrorMessageBox(SString(_("Load failed. Please ensure that %s is installed correctly."), CHECK_DM_CLIENT_NAME),
                              _E("CL18"), "client-missing");
        ExitProcess(EXIT_ERROR);
    }

    // Check for GTA executable
    if (!FileExists(PathJoin(strGTAPath, GTA_EXE_NAME)) &&
        !FileExists(PathJoin(strGTAPath, STEAM_GTA_EXE_NAME)))
    {
        DisplayErrorMessageBox(SString(_("Load failed. Could not find gta_sa.exe in %s."), strGTAPath.c_str()),
                              _E("CL20"), "gta_sa-missing");
        ExitProcess(EXIT_ERROR);
    }

    // Check for conflicting files
    static const char* dllConflicts[] = {
        "xmll.dll", "cgui.dll", "netc.dll", "libcurl.dll", "pthread.dll"
    };

    for (uint i = 0; i < NUMELMS(dllConflicts); ++i)
    {
        if (FileExists(PathJoin(strGTAPath, dllConflicts[i])))
        {
            DisplayErrorMessageBox(SString(_("Load failed. %s exists in the GTA directory. Please delete before continuing."), dllConflicts[i]),
                                  _E("CL21"), "file-clash");
            ExitProcess(EXIT_ERROR);
        }
    }

    // Check main exe has the correct name
    const SString launchFilename = GetLaunchFilename();
    if (!launchFilename.CompareI(MTA_EXE_NAME))
    {
        const SString strMessage(_("Main file has an incorrect name (%s)"), *launchFilename);
        if (MessageBoxUTF8(NULL, strMessage, _("Error") + _E("CL33"), MB_RETRYCANCEL | MB_ICONERROR | MB_TOPMOST) == IDRETRY)
        {
            ReleaseSingleInstanceMutex();
            const SString correctExePath = PathJoin(strMTASAPath, MTA_EXE_NAME);
            if (FileExists(correctExePath))
            {
                ShellExecuteNonBlocking("open", correctExePath);
            }
        }
        ExitProcess(EXIT_ERROR);
    }

    // Check for possible virus activity (simple file hash check)
    struct IntegrityCheck
    {
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

    for (uint i = 0; i < NUMELMS(integrityCheckList); ++i)
    {
        const IntegrityCheck& check = integrityCheckList[i];
        const SString         filePath = PathJoin(strMTASAPath, "mta", check.fileName);
        if (!ValidatePath(filePath) || !FileExists(filePath))
        {
            DisplayErrorMessageBox(SString(_("Data file %s is missing. Possible virus activity."), check.fileName),
                                 _E("CL30"), "maybe-virus2");
            break;
        }

        const SString computed = CMD5Hasher::CalculateHexString(filePath);
        if (!computed.CompareI(check.hash))
        {
            DisplayErrorMessageBox(SString(_("Data file %s is modified. Possible virus activity."), check.fileName),
                                 _E("CL30"), "maybe-virus2");
            break;
        }
    }

    // Check for ASI files
    std::vector<SString> gtaAsiFiles = FindFiles(PathJoin(strGTAPath, "*.asi"), true, false);
    std::vector<SString> mtaAsiFiles = FindFiles(PathJoin(strMTASAPath, "mta", "*.asi"), true, false);

    const size_t MAX_ASI_FILES = 100;
    bool bFoundInGTADir = !gtaAsiFiles.empty() && gtaAsiFiles.size() < MAX_ASI_FILES;
    bool bFoundInMTADir = !mtaAsiFiles.empty() && mtaAsiFiles.size() < MAX_ASI_FILES;

    if (bFoundInGTADir || bFoundInMTADir)
    {
        if (bFoundInGTADir)
        {
            for (size_t i = 0; i < gtaAsiFiles.size() && i < 10; ++i)
            {
                WriteDebugEvent(SString("Warning: ASI file detected in GTA dir: %s", *gtaAsiFiles[i]));
            }
        }
        
        DisplayErrorMessageBox(_(".asi files are in the installation directory.\n\n"
                                "Remove these .asi files if you experience problems."),
                              _E("CL28"), "asi-files");
    }

    // Check for graphics libraries in the GTA/MTA install directory
    {
        const std::pair<const char*, SString> directoriesToCheck[] = {
            {"", strGTAPath},
            {"mta-", PathJoin(strMTASAPath, "mta")}
        };

        std::vector<GraphicsLibrary> offenders;

        for (const std::pair<const char*, SString>& directory : directoriesToCheck)
        {
            if (!ValidatePath(directory.second)) continue;

            for (const char* libraryName : {"d3d9", "dxgi"})
            {
                GraphicsLibrary library(libraryName);
                library.absoluteFilePath = PathJoin(directory.second, library.stem + ".dll");

                if (library.absoluteFilePath.length() > MAX_PATH) continue;
                if (!FileExists(library.absoluteFilePath)) continue;

                library.appLastHash = SString("%s%s-dll-last-hash", directory.first, library.stem.c_str());
                library.appDontRemind = SString("%s%s-dll-not-again", directory.first, library.stem.c_str());

                library.md5Hash = CMD5Hasher::CalculateHexString(library.absoluteFilePath);
                WriteDebugEvent(SString("Detected graphics library %s (md5: %s)",
                                      library.absoluteFilePath.c_str(), library.md5Hash.c_str()));

                bool isProblematic = true;
                if (GetApplicationSetting("diagnostics", library.appLastHash) == library.md5Hash)
                {
                    if (GetApplicationSetting("diagnostics", library.appDontRemind) == "yes")
                    {
                        isProblematic = false;
                    }
                }

                if (isProblematic)
                {
                    offenders.emplace_back(std::move(library));
                }
            }
        }

        if (!offenders.empty() && offenders.size() < 10)
        {
            ShowGraphicsDllDialog(g_hInstance, offenders);
            HideGraphicsDllDialog();
        }
    }

    // Maybe remove some unwanted files
    if (!strGTAPath.CompareI(PathJoin(strMTASAPath, "mta")))
    {
        static const char* logFiles[] = {"CEGUI.log", "logfile.txt", "shutdown.log"};
        for (uint i = 0; i < NUMELMS(logFiles); ++i)
        {
            const SString path = PathJoin(strGTAPath, logFiles[i]);
            if (ValidatePath(path))
            {
                FileDelete(path);
            }
        }
    }
}

//////////////////////////////////////////////////////////
//
// StartGtaProcess
//
// Start GTA as an independent process
//
//////////////////////////////////////////////////////////
BOOL StartGtaProcess(const SString& lpApplicationName, const SString& lpCommandLine, const SString& lpCurrentDirectory,
                     LPPROCESS_INFORMATION lpProcessInformation, DWORD& dwOutError, SString& strOutErrorContext)
{
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    BOOL wasProcessCreated = CreateProcessW(*FromUTF8(lpApplicationName), FromUTF8(lpCommandLine).data(), nullptr, nullptr, FALSE, 0, nullptr,
                                            *FromUTF8(lpCurrentDirectory), &startupInfo, lpProcessInformation);

    if (wasProcessCreated)
        return true;

    std::vector<DWORD> processIdListBefore = GetGTAProcessList();

    if (!ShellExecuteNonBlocking("open", lpApplicationName, lpCommandLine, lpCurrentDirectory))
    {
        dwOutError = GetLastError();
        strOutErrorContext = "ShellExecute";
        return false;
    }

    // Determine pid of new gta process
    for (uint i = 0; i < 10; i++)
    {
        std::vector<DWORD> processIdList = GetGTAProcessList();
        for (DWORD pid : processIdList)
        {
            if (ListContains(processIdListBefore, pid))
            {
                continue;
            }
            lpProcessInformation->dwProcessId = pid;
            lpProcessInformation->hProcess = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION | SYNCHRONIZE, FALSE, pid);
            wasProcessCreated = true;
            break;
        }
        if (lpProcessInformation->dwProcessId)
            break;
        Sleep(500);
    }

    if (lpProcessInformation->dwProcessId == 0)
    {
        // Unable to get pid
        dwOutError = ERROR_INVALID_FUNCTION;
        strOutErrorContext = "FindPID";
        wasProcessCreated = false;
    }
    else if (lpProcessInformation->hProcess == nullptr)
    {
        // Unable to OpenProcess
        dwOutError = ERROR_ELEVATION_REQUIRED;
        strOutErrorContext = "OpenProcess";
        wasProcessCreated = false;
    }

    return wasProcessCreated;
}

//////////////////////////////////////////////////////////
//
// LaunchGame
//
// Create GTA process, hook it and go go go
//
//////////////////////////////////////////////////////////
int LaunchGame(SString strCmdLine)
{
    CheckAndShowModelProblems();
    CheckAndShowUpgradeProblems();
    CheckAndShowImgProblems();

    const SString strGTAPath    = GetGTAPath();
    const SString strMTASAPath  = GetMTASAPath();
    const SString strGTAEXEPath = UTF8FilePath(GetGameExecutablePath());

    if (!ValidatePath(strGTAPath) || !ValidatePath(strMTASAPath) || !ValidatePath(strGTAEXEPath))
    {
        DisplayErrorMessageBox(_("Invalid game paths detected."), _E("CL44"), "invalid-paths");
        return 5;
    }

    SetDllDirectory(PathJoin(strMTASAPath, "mta"));

    if (!CheckService(CHECK_SERVICE_PRE_CREATE) && !IsUserAdmin())
    {
        RelaunchAsAdmin(strCmdLine, _("Fix configuration issue"));
        ExitProcess(EXIT_OK);
    }

    // Do some D3D things
    BeginD3DStuff();
    LogSettings();

    WatchDogBeginSection("L2");                                     // Gets closed when loading screen is shown
    WatchDogBeginSection("L3");                                     // Gets closed when loading screen is shown, or a startup problem is handled elsewhere
    WatchDogBeginSection(WD_SECTION_NOT_USED_MAIN_MENU);            // Gets closed when the main menu is used

    // Extract 'done-admin' flag from command line
    SString    sanitizedCmdLine = strCmdLine;
    const bool bDoneAdmin       = sanitizedCmdLine.Contains("/done-admin");
    sanitizedCmdLine             = sanitizedCmdLine.Replace(" /done-admin", "");

    // Validate command line length
    if (sanitizedCmdLine.length() > 2048)            // Max MTA connect URI length
    {
        sanitizedCmdLine = sanitizedCmdLine.Left(2048);
    }

    // Add server connection after update to command line
    const SString strPostUpdateConnect = GetPostUpdateConnect();
    if (!strPostUpdateConnect.empty() && sanitizedCmdLine.empty() && strPostUpdateConnect.length() < 512)
    {
        sanitizedCmdLine = SString("mtasa://%s", *strPostUpdateConnect);
    }

    // Launch GTA using CreateProcess
    PROCESS_INFORMATION piLoadee = {};
    DWORD dwError = 0;
    SString strErrorContext;
    if (!StartGtaProcess(strGTAEXEPath, sanitizedCmdLine, strGTAPath, &piLoadee, dwError, strErrorContext))
    {
        // Handle process creation failure
        SString strError = SString("Failed to launch GTA: San Andreas. [%s: %d]", *strErrorContext, dwError);
        DisplayErrorMessageBox(strError, _E("CL50"), "gta-launch-failed");
        return 5;
    }

    WriteDebugEvent(SString("Loader - Process created: %s %s", *strGTAEXEPath, *GetApplicationSetting("serial")));
    WriteDebugEvent(SString("Loader - Process ID: %lu, Thread ID: %lu", piLoadee.dwProcessId, piLoadee.dwThreadId));

    // Clear previous on quit commands
    SetOnQuitCommand("");

    // Show splash screen
    ShowSplash(g_hInstance);

    // Wait for game to launch
    DWORD dwExitCode = static_cast<DWORD>(-1);

    if (piLoadee.hProcess && piLoadee.hProcess != INVALID_HANDLE_VALUE)
    {
        BsodDetectionOnGameBegin();

        // Wait for game window
        DWORD status = WAIT_TIMEOUT;
        for (uint i = 0; i < 20 && status == WAIT_TIMEOUT; ++i)            // Max 20 iterations
        {
            status = WaitForSingleObject(piLoadee.hProcess, 1000);            // 1 second timeout

            if (!WatchDogIsSectionOpen("L3"))            // Gets closed when loading screen is shown
            {
                WriteDebugEvent("Loader - L3 closed");
                break;
            }

            // Check for device selection dialog
            if (IsDeviceSelectionDialogOpen(piLoadee.dwProcessId) && i > 0)
            {
                --i;            // Don't count this iteration
                Sleep(100);
            }
        }

        HideSplash();

        // Handle process if stuck at startup
        if (status == WAIT_TIMEOUT)
        {
            CStuckProcessDetector detector(piLoadee.hProcess, 5000);
            while (status == WAIT_TIMEOUT && WatchDogIsSectionOpen("L3"))            // Gets closed when loading screen is shown
            {
                if (detector.UpdateIsStuck())
                {
                    WriteDebugEvent("Detected stuck process at startup");
                    if (MessageBoxUTF8(0, _("GTA: San Andreas may not have launched correctly. Terminate it?"), _("Information") + _E("CL25"),
                                       MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
                    {
                        TerminateProcess(piLoadee.hProcess, 1);
                    }
                    break;
                }
                status = WaitForSingleObject(piLoadee.hProcess, 1000);            // 1 second timeout
            }
        }

        // Wait for game to exit
        WriteDebugEvent("Loader - Wait for game to exit");
        while (status == WAIT_TIMEOUT)
        {
            status = WaitForSingleObject(piLoadee.hProcess, 1500);

            // If core is closing and gta_sa.exe process memory usage is not changing, terminate
            CStuckProcessDetector detector(piLoadee.hProcess, 5000);
            while (status == WAIT_TIMEOUT && WatchDogIsSectionOpen("Q0"))            // Gets closed when quit is detected as frozen
            {
                if (detector.UpdateIsStuck())
                {
                    WriteDebugEvent("Detected stuck process at quit");
                #ifndef MTA_DEBUG
                    TerminateProcess(piLoadee.hProcess, 1);
                    status = WAIT_FAILED;
                    break;
                #endif
                }
                status = WaitForSingleObject(piLoadee.hProcess, 1000);
            }
        }

        BsodDetectionOnGameEnd();

        if (!GetExitCodeProcess(piLoadee.hProcess, &dwExitCode))
        {
            dwExitCode = static_cast<DWORD>(-1);
        }
    }

    AddReportLog(7104, "Loader - Finishing");
    WriteDebugEvent("Loader - Finishing");
    EndD3DStuff();

    // Cleanup
    if (piLoadee.hProcess && piLoadee.hProcess != INVALID_HANDLE_VALUE)
    {
        TerminateProcess(piLoadee.hProcess, 1);
        if (piLoadee.hThread && piLoadee.hThread != INVALID_HANDLE_VALUE)
        {
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
// What to do when GTA exits
//
//////////////////////////////////////////////////////////
void HandleOnQuitCommand()
{
    const SString strMTASAPath = GetMTASAPath();

    if (!ValidatePath(strMTASAPath))
    {
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }

    SetCurrentDirectory(strMTASAPath);
    SetDllDirectory(strMTASAPath);

    SString strOnQuitCommand = GetRegistryValue("", "OnQuitCommand");

    if (strOnQuitCommand.length() > 4096)
    {
        WriteDebugEvent("OnQuitCommand too long, ignoring");
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }

    std::vector<SString> vecParts;
    strOnQuitCommand.Split("\t", vecParts);

    if (vecParts.size() < 5 || vecParts.size() > 100)
    {
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }

    // Validate parts
    for (const auto& part : vecParts)
    {
        if (part.length() > 1024)
        {
            WriteDebugEvent("OnQuitCommand has invalid parts");
            CheckService(CHECK_SERVICE_POST_GAME);
            return;
        }
    }

    SString strOperation  = vecParts[0];
    SString strFile       = vecParts[1];
    SString strParameters = vecParts[2];
    SString strDirectory  = vecParts[3];
    SString strShowCmd    = vecParts[4];

    // Process operation type
    if (strOperation == "restart")
    {
        strOperation = "open";
        strFile      = PathJoin(strMTASAPath, MTA_EXE_NAME);

        if (!FileExists(strFile))
        {
            WriteDebugEvent("MTA executable not found for restart");
            CheckService(CHECK_SERVICE_POST_GAME);
            return;
        }
        CheckService(CHECK_SERVICE_RESTART_GAME);
    }
    else if (strOperation == "open" || strOperation.empty())
    {
        if (strFile.Contains("..") || strFile.Contains("~/") || strFile.Contains("~\\"))
        {
            WriteDebugEvent("Suspicious file path in OnQuitCommand");
            CheckService(CHECK_SERVICE_POST_GAME);
            return;
        }

        // Check for executable extensions
        if (strFile.EndsWithI(".exe") || strFile.EndsWithI(".bat") || strFile.EndsWithI(".cmd") || strFile.EndsWithI(".ps1"))
        {
            if (strFile.Contains(":") || strFile.BeginsWith("\\"))
            {
                if (!FileExists(strFile))
                {
                    WriteDebugEvent("Executable in OnQuitCommand not found");
                    CheckService(CHECK_SERVICE_POST_GAME);
                    return;
                }

                // Don't allow execution from Windows or System directories
                const SString strFileLower = strFile.ToLower();
                if (strFileLower.Contains("\\windows\\") || strFileLower.Contains("\\system32\\") || strFileLower.Contains("\\syswow64\\") ||
                    strFileLower.Contains("\\sysnative\\"))
                {
                    WriteDebugEvent("Refusing to execute from system directory");
                    CheckService(CHECK_SERVICE_POST_GAME);
                    return;
                }
            }
        }
        CheckService(CHECK_SERVICE_POST_GAME);
    }
    else
    {
        WriteDebugEvent(SString("Unknown operation: %s", *strOperation));
        CheckService(CHECK_SERVICE_POST_GAME);
        return;
    }

    // Sanitize parameters
    if (strParameters.Contains("&") || strParameters.Contains("|") || strParameters.Contains(">") || strParameters.Contains("<") ||
        strParameters.Contains("&&") || strParameters.Contains("||"))
    {
        WriteDebugEvent("Suspicious parameters in OnQuitCommand, removing special chars");
        strParameters = strParameters.Replace("&", "").Replace("|", "").Replace(">", "").Replace("<", "");
    }

    // Validate ShowCmd
    INT nShowCmd = SW_SHOWNORMAL;
    if (!strShowCmd.empty())
    {
        int tempShowCmd = atoi(strShowCmd);
        if (tempShowCmd >= 0 && tempShowCmd <= 11)
        {
            nShowCmd = tempShowCmd;
        }
    }

    // Execute command
    if (!strOperation.empty() && !strFile.empty())
    {
        WriteDebugEvent(SString("Executing OnQuitCommand: op=%s, file=%s", *strOperation, *strFile));
        ShellExecuteNonBlocking(strOperation.empty() ? NULL : strOperation.c_str(), strFile.c_str(),
                               strParameters.empty() ? NULL : strParameters.c_str(), strDirectory.empty() ? NULL : strDirectory.c_str(), nShowCmd);
    }
}
