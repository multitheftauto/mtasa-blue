/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/CInstallManager.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "CInstallManager.h"
#include "Utils.h"
#include "Main.h"
#include "Dialogs.h"
#include "Install.h"
#include "GameExecutablePatcher.h"
#include "FileGenerator.h"
#include "FileSystem.h"

namespace fs = std::filesystem;

namespace
{
    //////////////////////////////////////////////////////////
    //
    // Helper functions for this file
    //
    //
    //
    //////////////////////////////////////////////////////////

    // Ensure report log stuff has correct tags
    void UpdateSettingsForReportLog()
    {
        UpdateMTAVersionApplicationSetting();
        SetApplicationSetting("os-version", SString("%d.%d", GetOSVersion().dwMajor, GetOSVersion().dwMinor));
        SetApplicationSetting("real-os-version", SString("%d.%d", GetRealOSVersion().dwMajor, GetRealOSVersion().dwMinor));
        SetApplicationSetting("is-admin", IsUserAdmin() ? "1" : "0");
        SetApplicationSettingInt("last-server-ip", 0);
        SetApplicationSetting("real-os-build", SString("%d", GetRealOSVersion().dwBuild));
    }

    // Comms between 'Admin' and 'User' processes
    void SendStringToUserProcess(const SString& strText) { SetApplicationSetting("admin2user_comms", strText); }

    SString ReceiveStringFromAdminProcess() { return GetApplicationSetting("admin2user_comms"); }

    bool IsBlockingUserProcess() { return GetApplicationSetting("admin2user_comms") == "user_waiting"; }

    void SetIsBlockingUserProcess() { SetApplicationSetting("admin2user_comms", "user_waiting"); }

    void ClearIsBlockingUserProcess()
    {
        if (IsBlockingUserProcess())
            SetApplicationSetting("admin2user_comms", "");
    }
}            // namespace

//////////////////////////////////////////////////////////
//
// CInstallManager global object
//
//
//
//////////////////////////////////////////////////////////
CInstallManager* g_pInstallManager = NULL;

CInstallManager* GetInstallManager()
{
    if (!g_pInstallManager)
        g_pInstallManager = new CInstallManager();
    return g_pInstallManager;
}

//////////////////////////////////////////////////////////
//
// CInstallManager::InitSequencer
//
//
//
//////////////////////////////////////////////////////////
void CInstallManager::InitSequencer()
{
    #define CR "\n"
    SString strSource = CR "initial: "                                             // *** Starts here  by default
        CR "            CALL CheckOnRestartCommand "                               ////// Start of 'update game' //////
        CR "            IF LastResult != ok GOTO update_end: "                     //
        CR " "                                                                     //
        CR "            CALL MaybeSwitchToTempExe "                                // If update files comes with an .exe, switch to that for the install
        CR " "                                                                     //
        CR "copy_files: "                                                          //
        CR "            CALL InstallFiles "                                        // Try to install update files
        CR "            IF LastResult == ok GOTO update_end: "                     //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If install failed, try as admin
        CR " "                                                                     //
        CR "copy_files_admin: "                                                    //
        CR "            CALL InstallFiles "                                        // Try to install update files
        CR "            IF LastResult == ok GOTO update_end_admin: "               //
        CR " "                                                                     //
        CR "            CALL ShowCopyFailDialog "                                  // If install failed as admin, show message box
        CR "            IF LastResult == retry GOTO copy_files_admin: "            //
        CR " "                                                                     //
        CR "update_end_admin: "                                                    //
        CR "            CALL ChangeFromAdmin "                                     //
        CR " "                                                                     //
        CR "update_end: "                                                          ////// End of 'update game' //////
        CR "            CALL SwitchBackFromTempExe "                               //
        CR " "                                                                     //
        CR "newlayout_check:"                                                      ////// Start of 'new layout check' //////
        CR "            CALL ProcessLayoutChecks "                                 //
        CR "            IF LastResult == ok GOTO newlayout_end: "                  //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO newlayout_check: "                //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "newlayout_end: "                                                       ////// End of 'new layout check' //////
        CR " "                                                                     //
        CR "langfile_check: "                                                      ////// Start of 'Lang file fix' //////
        CR "            CALL ProcessLangFileChecks "                               // Make changes to comply with requirements
        CR "            IF LastResult == ok GOTO langfile_end: "                   //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO langfile_check: "                 //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "langfile_end: "                                                        ////// End of 'Lang file fix' //////
        CR " "                                                                     //
        CR "prepare_launch_location:"                                              ////// Start of 'prepare launch location' //////
        CR "            CALL PrepareLaunchLocation "                               //
        CR "            IF LastResult == ok GOTO prepare_launch_location_end: "    //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO prepare_launch_location: "        //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "prepare_launch_location_end:"                                          ////// End of 'prepare launch location' //////
        CR " "                                                                     //
        CR "gta_patch_check:"                                                      ////// Start of 'gta patch check' //////
        CR "            CALL ProcessGtaPatchCheck "                                //
        CR "            IF LastResult != ok GOTO do_quit: "                        // Switching to admin has zero impact
        CR " "                                                                     //
        CR "gta_patch_check_end:"                                                  ////// End of 'gta patch check' ////// 
        CR " "                                                                     //
        CR "gta_dll_check:"                                                        ////// Start of 'gta dll check' //////
        CR "            CALL ProcessGtaDllCheck "                                  //
        CR "            IF LastResult == ok GOTO gta_dll_end: "                    //
        CR "            IF LastResult == quit GOTO do_quit: "                      //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO gta_dll_check: "                  //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "gta_dll_end: "                                                         ////// End of 'gta dll check' //////
        CR " "                                                                     //
        CR "gta_version_check:"                                                    ////// Start of 'gta version check' //////
        CR "            CALL ProcessGtaVersionCheck "                              //
        CR "            IF LastResult == ok GOTO gta_version_end: "                //
        CR "            IF LastResult == quit GOTO do_quit: "                      //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO gta_version_check: "              //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "gta_version_end: "                                                     ////// End of 'gta version check' //////
        CR " "                                                                     //
        CR "service_check: "                                                       ////// Start of 'Service checks' //////
        CR "            CALL ProcessServiceChecks "                                // Make changes to comply with service requirements
        CR "            IF LastResult == ok GOTO service_end: "                    //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO service_check: "                  //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "service_end: "                                                         ////// End of 'Service checks' //////
        CR " "                                                                     //
        CR "appcompat_check: "                                                     ////// Start of 'AppCompat checks' //////
        CR "            CALL ProcessAppCompatChecks "                              // Make changes to comply with appcompat requirements
        CR "            IF LastResult == ok GOTO appcompat_end: "                  //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO appcompat_check: "                //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "appcompat_end: "                                                       ////// End of 'AppCompat checks' //////
        CR " "                                                                     //
        CR "            CALL ChangeFromAdmin "                                     //
        CR "            CALL InstallNewsItems "                                    // Install pending news
        CR "            GOTO launch: "                                             //
        CR " "                                                                     //
        CR "do_quit: "                                                             // Quit ensuring termination of both user & admin instance
        CR "            CALL ChangeFromAdmin "                                     //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "crashed: "                                                             // *** Starts here when restarting after crash
        CR "            CALL ShowCrashFailDialog "                                 //
        CR "            IF LastResult == ok GOTO initial: "                        //
        CR "            CALL Quit "                                                //
        CR " "                                                                     //
        CR "launch: ";

    m_pSequencer = new CSequencerType();
    m_pSequencer->SetSource(this, strSource);
    m_pSequencer->AddFunction("ShowCrashFailDialog", &CInstallManager::_ShowCrashFailDialog);
    m_pSequencer->AddFunction("CheckOnRestartCommand", &CInstallManager::_CheckOnRestartCommand);
    m_pSequencer->AddFunction("MaybeSwitchToTempExe", &CInstallManager::_MaybeSwitchToTempExe);
    m_pSequencer->AddFunction("SwitchBackFromTempExe", &CInstallManager::_SwitchBackFromTempExe);
    m_pSequencer->AddFunction("InstallFiles", &CInstallManager::_InstallFiles);
    m_pSequencer->AddFunction("ChangeToAdmin", &CInstallManager::_ChangeToAdmin);
    m_pSequencer->AddFunction("ShowCopyFailDialog", &CInstallManager::_ShowCopyFailDialog);
    m_pSequencer->AddFunction("PrepareLaunchLocation", &CInstallManager::_PrepareLaunchLocation);
    m_pSequencer->AddFunction("ProcessGtaPatchCheck", &CInstallManager::_ProcessGtaPatchCheck);
    m_pSequencer->AddFunction("ProcessGtaDllCheck", &CInstallManager::_ProcessGtaDllCheck);
    m_pSequencer->AddFunction("ProcessGtaVersionCheck", &CInstallManager::_ProcessGtaVersionCheck);
    m_pSequencer->AddFunction("ProcessLayoutChecks", &CInstallManager::_ProcessLayoutChecks);
    m_pSequencer->AddFunction("ProcessLangFileChecks", &CInstallManager::_ProcessLangFileChecks);
    m_pSequencer->AddFunction("ProcessServiceChecks", &CInstallManager::_ProcessServiceChecks);
    m_pSequencer->AddFunction("ProcessAppCompatChecks", &CInstallManager::_ProcessAppCompatChecks);
    m_pSequencer->AddFunction("ChangeFromAdmin", &CInstallManager::_ChangeFromAdmin);
    m_pSequencer->AddFunction("InstallNewsItems", &CInstallManager::_InstallNewsItems);
    m_pSequencer->AddFunction("Quit", &CInstallManager::_Quit);
}

//////////////////////////////////////////////////////////
//
// CInstallManager::SetMTASAPathSource
//
// Figure out and set the path that GetMTASAPath() should use
//
//////////////////////////////////////////////////////////
void CInstallManager::SetMTASAPathSource(const SString& strCommandLineIn)
{
    // Update some settings which are used by ReportLog
    UpdateSettingsForReportLog();

    InitSequencer();
    RestoreSequencerFromSnapshot(strCommandLineIn);

    // If command line says we're not running from the launch directory, get the launch directory location from the registry
    if (m_pSequencer->GetVariable(INSTALL_LOCATION) == "far")
        ::SetMTASAPathSource(true);
    else
        ::SetMTASAPathSource(false);
}

//////////////////////////////////////////////////////////
//
// CInstallManager::Continue
//
// Process next step
//
//////////////////////////////////////////////////////////
SString CInstallManager::Continue()
{
    // Initial report line
    DWORD   dwProcessId = GetCurrentProcessId();
    SString GotPathFrom = (m_pSequencer->GetVariable(INSTALL_LOCATION) == "far") ? "registry" : "module location";
    AddReportLog(1041, SString("* Launch * pid:%d '%s' MTASAPath set from %s '%s'", dwProcessId, GetLaunchPathFilename().c_str(), GotPathFrom.c_str(),
                               GetMTASAPath().c_str()));

    // Run sequencer
    for (int i = 0; !m_pSequencer->AtEnd() && i < 1000; i++)
        m_pSequencer->ProcessNextLine();

    // Remove unwanted files
    CleanDownloadCache();

    // Extract command line launch args
    SString strCommandLineOut;
    for (int i = 0; i < m_pSequencer->GetVariableInt("_argc"); i++)
        strCommandLineOut += m_pSequencer->GetVariable(SString("_arg_%d", i)) + " ";

    AddReportLog(1060, SString("CInstallManager::Continue - return %s", *strCommandLineOut));
    return *strCommandLineOut.TrimEnd(" ");
}

//////////////////////////////////////////////////////////
//
// CInstallManager::RestoreSequencerFromSnapshot
//
// Set current sequencer position from a string
//
//////////////////////////////////////////////////////////
void CInstallManager::RestoreSequencerFromSnapshot(const SString& strText)
{
    AddReportLog(1061, SString("CInstallManager::RestoreSequencerState %s", *strText));
    std::vector<SString> parts;
    strText.Split(" ", parts);

    int iFirstArg = 0;
    if (parts.size() > 0 && parts[0].Contains("="))
    {
        // Upgrade variables
        if (parts[0].Contains(INSTALL_STAGE) && !parts[0].Contains("_pc_label"))
            parts[0] = parts[0].Replace(INSTALL_STAGE, "_pc_label");

        m_pSequencer->RestoreStateFromString(parts[0]);
        iFirstArg++;
    }

    // Add any extra command line args
    if (m_pSequencer->GetVariableInt("_argc") == 0)
    {
        m_pSequencer->SetVariable("_argc", parts.size() - iFirstArg);
        for (uint i = iFirstArg; i < parts.size(); i++)
        {
            m_pSequencer->SetVariable(SString("_arg_%d", i - iFirstArg), parts[i]);
        }
    }

    // Ignore LastResult
    m_pSequencer->SetVariable("LastResult", "ok");
}

//////////////////////////////////////////////////////////
//
// CInstallManager::GetSequencerSnapshot
//
// Save current sequencer position to a string
//
//////////////////////////////////////////////////////////
SString CInstallManager::GetSequencerSnapshot()
{
    m_pSequencer->SetVariable("LastResult", "");
    return m_pSequencer->SaveStateToString();
}

//////////////////////////////////////////////////////////
//
// CInstallManager::GetLauncherPathFilename
//
// Get path to launch exe
//
//////////////////////////////////////////////////////////
SString CInstallManager::GetLauncherPathFilename()
{
    SString strLocation = m_pSequencer->GetVariable(INSTALL_LOCATION);
    SString strResult = PathJoin(strLocation == "far" ? GetSystemCurrentDirectory() : GetMTASAPath(), MTA_EXE_NAME);
    AddReportLog(1062, SString("GetLauncherPathFilename %s", *strResult));
    return strResult;
}

//
// Functions called from the sequencer
//

//////////////////////////////////////////////////////////
//
// CInstallManager::_ChangeToAdmin
//
//
// Save the state of the sequencer and launch process as admin
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ChangeToAdmin()
{
    if (!IsUserAdmin())
    {
        MessageBoxUTF8(
            NULL, SString(_("MTA:SA needs Administrator access for the following task:\n\n  '%s'\n\nPlease confirm in the next window."), *m_strAdminReason),
            "Multi Theft Auto: San Andreas", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
        SetIsBlockingUserProcess();
        ReleaseSingleInstanceMutex();
        if (ShellExecuteBlocking("runas", GetLauncherPathFilename(), GetSequencerSnapshot()))
        {
            // Will return here once admin process has finished
            CreateSingleInstanceMutex();
            UpdateSettingsForReportLog();
            RestoreSequencerFromSnapshot(ReceiveStringFromAdminProcess());
            ClearIsBlockingUserProcess();
            return "ok";            // This will appear as the result for _ChangeFromAdmin
        }
        CreateSingleInstanceMutex();
        ClearIsBlockingUserProcess();
        MessageBoxUTF8(NULL, SString(_("MTA:SA could not complete the following task:\n\n  '%s'\n"), *m_strAdminReason),
                       "Multi Theft Auto: San Andreas" + _E("CL01"), MB_OK | MB_ICONWARNING | MB_TOPMOST);
    }
    return "fail";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ChangeFromAdmin
//
// Save the state of the sequencer and exit back to the user process
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ChangeFromAdmin()
{
    if (IsUserAdmin() && IsBlockingUserProcess())
    {
        SendStringToUserProcess(GetSequencerSnapshot());
        AddReportLog(1003, SString("CInstallManager::_ChangeToAdmin - exit(0) %s", ""));
        ClearIsBlockingUserProcess();
        ExitProcess(0);
    }
    return "fail";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ShowCrashFailDialog
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ShowCrashFailDialog()
{
    // Crashed before gta game started ?
    if (WatchDogIsSectionOpen("L1"))
        WatchDogIncCounter("CR1");

    HideSplash();

    // Transfer gta-fopen-last info
    if (GetApplicationSetting("diagnostics", "gta-fopen-fail").empty())
        SetApplicationSetting("diagnostics", "gta-fopen-fail", GetApplicationSetting("diagnostics", "gta-fopen-last"));
    SetApplicationSetting("diagnostics", "gta-fopen-last", "");

    SString strMessage = GetApplicationSetting("diagnostics", "last-crash-info");
    SString strReason = GetApplicationSetting("diagnostics", "last-crash-reason");
    SetApplicationSetting("diagnostics", "last-crash-reason", "");
    if (strReason == "direct3ddevice-reset")
    {
        strMessage += _("** The crash was caused by a graphics driver error **\n\n** Please update your graphics drivers **");
    }
    else
    {
        strMessage += strReason;
    }

    strMessage = strMessage.Replace("\r", "").Replace("\n", "\r\n");
    SString strResult = ShowCrashedDialog(g_hInstance, strMessage);
    HideCrashedDialog();

    CheckAndShowFileOpenFailureMessage();

    return strResult;
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_CheckOnRestartCommand
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_CheckOnRestartCommand()
{
    // Check for pending update
    const SString strResult = CheckOnRestartCommand();

    if (strResult.Contains("install"))
    {
        // New settings for install
        m_pSequencer->SetVariable(INSTALL_LOCATION, strResult.Contains("far") ? "far" : "near");
        m_pSequencer->SetVariable(HIDE_PROGRESS, strResult.Contains("hideprogress") ? "yes" : "no");
        return "ok";
    }
    else if (!strResult.Contains("no update"))
    {
        AddReportLog(4047, SString("_CheckOnRestartCommand: CheckOnRestartCommand returned %s", strResult.c_str()));
    }

    return "no_action";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_MaybeSwitchToTempExe
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_MaybeSwitchToTempExe()
{
    // If a new "Multi Theft Auto.exe" exists, let that complete the install
    if (m_pSequencer->GetVariable(INSTALL_LOCATION) == "far")
    {
        ReleaseSingleInstanceMutex();
        if (ShellExecuteNonBlocking("open", GetLauncherPathFilename(), GetSequencerSnapshot()))
            ExitProcess(0);            // All done here
        CreateSingleInstanceMutex();
        return "fail";
    }
    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_SwitchBackFromTempExe
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_SwitchBackFromTempExe()
{
    // If currently running temp install exe, switch back
    if (m_pSequencer->GetVariable(INSTALL_LOCATION) == "far")
    {
        m_pSequencer->SetVariable(INSTALL_LOCATION, "near");

        ReleaseSingleInstanceMutex();
        if (ShellExecuteNonBlocking("open", GetLauncherPathFilename(), GetSequencerSnapshot()))
            ExitProcess(0);            // All done here
        CreateSingleInstanceMutex();
        return "fail";
    }
    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_InstallFiles
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_InstallFiles()
{
    WatchDogReset();

    // Install new files
    if (!InstallFiles(m_pSequencer->GetVariable(HIDE_PROGRESS) != "yes"))
    {
        if (!IsUserAdmin())
            AddReportLog(3048, SString("_InstallFiles: Install - trying as admin %s", ""));
        else
            AddReportLog(5049, SString("_InstallFiles: Couldn't install files %s", ""));

        m_strAdminReason = _("Install updated MTA:SA files");
        return "fail";
    }
    else
    {
        UpdateMTAVersionApplicationSetting();
        AddReportLog(2050, SString("_InstallFiles: ok %s", ""));
        return "ok";
    }
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ShowCopyFailDialog
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ShowCopyFailDialog()
{
    int iResponse = MessageBoxUTF8(NULL, _("Could not update due to file conflicts. Please close other applications and retry"), _("Error") + _E("CL02"),
                                   MB_RETRYCANCEL | MB_ICONERROR | MB_TOPMOST);
    if (iResponse == IDRETRY)
        return "retry";
    return "ok";
}

void ShowLayoutError(const SString& strExtraInfo)
{
    MessageBoxUTF8(0, SString(_("Multi Theft Auto has not been installed properly, please reinstall. %s"), *strExtraInfo), _("Error") + _E("CL03"),
                   MB_OK | MB_ICONERROR | MB_TOPMOST);
    TerminateProcess(GetCurrentProcess(), 9);
}

// Move file to new location
void MigrateFile(const SString& strFilenameOld, const SString& strFilenameNew)
{
    if (FileExists(strFilenameOld))
    {
        if (!FileExists(strFilenameNew))
            FileRename(strFilenameOld, strFilenameNew);
        FileDelete(strFilenameOld);
    }
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_PrepareLaunchLocation
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_PrepareLaunchLocation()
{
    const bool isAdmin = IsUserAdmin();

    // Ensure GTA exe is not running
    TerminateGTAIfRunning();

    const fs::path gtaDir = GetGameBaseDirectory();
    const fs::path mtaDir = GetMTARootDirectory() / "MTA";
    const fs::path launchDir = GetGameLaunchDirectory();

#if 0
    // NOTE(botder): We are not using this solution because creating directory junctions requires administrator privileges.
    // Create GTA subdirectory junctions to our launch directory.
    for (const char* directoryName : {"anim", "audio", "data", "models", "text"})
    {
        // Delete shortcuts that may be confusing to the eye.
        const fs::path shortcutPath = (launchDir / directoryName).replace_extension(".lnk");

        if (std::error_code ec; fs::exists(shortcutPath, ec))
        {
            if (!fs::remove(shortcutPath, ec))
            {
                OutputDebugLine(*SString("Failed to remove shortcut for %s (%d, %s)", directoryName, ec.value(), ec.message().c_str()));
            }
        }

        if (std::error_code ec; !SetDirectoryJunction(gtaDir / directoryName, launchDir / directoryName, ec))
        {
            OutputDebugLine(*SString("Failed to create junction for %s (%d, %s)", directoryName, ec.value(), ec.message().c_str()));
            m_strAdminReason = _("Create GTA:SA junctions");
            return "fail";
        }
    }
#endif

    // Copy GTA dependencies to our launch directory.
    for (const char* fileName : {"eax.dll", "ogg.dll", "vorbis.dll", "vorbisFile.dll"})
    {
        const fs::path sourcePath = gtaDir / fileName;
        const fs::path targetPath = launchDir / fileName;

        if (std::error_code ec; !SafeCopyFile(sourcePath, targetPath, ec))
        {
            if (IsErrorCodeLoggable(ec))
            {
                const uintmax_t sourceSize = GetFileSize(sourcePath);
                const uintmax_t targetSize = GetFileSize(targetPath);
                AddReportLog(3052, SString("_PrepareLaunchLocation: Copying '%s' failed (err: %d, size: %ju <=> %ju, admin: %d)", fileName, ec.value(),
                                           sourceSize, targetSize, isAdmin));
            }
        }
    }

    // Copy MTA dependencies to our launch directory.
    for (const char* fileName : {LOADER_PROXY_DLL_NAME})
    {
        const fs::path sourcePath = mtaDir / fileName;
        const fs::path targetPath = launchDir / fileName;

        if (std::error_code ec; !SafeCopyFile(sourcePath, targetPath, ec))
        {
            if (IsErrorCodeLoggable(ec))
            {
                const uintmax_t sourceSize = GetFileSize(sourcePath);
                const uintmax_t targetSize = GetFileSize(targetPath);
                AddReportLog(3052, SString("_PrepareLaunchLocation: Copying '%s' failed (err: %d, size: %ju <=> %ju, admin: %d)", fileName, ec.value(),
                                           sourceSize, targetSize, isAdmin));
            }

            if (isAdmin)
            {
                if (fs::is_regular_file(sourcePath, ec))
                {
                    SString strMessage(_("MTA:SA cannot launch because copying a file failed:"));
                    strMessage += "\n\n" + targetPath.u8string();
                    BrowseToSolution("copy-files", ASK_GO_ONLINE, strMessage);
                }
                else
                {
                    SString strMessage(_("MTA:SA cannot launch because an MTA:SA file is incorrect or missing:"));
                    strMessage += "\n\n" + sourcePath.u8string();
                    BrowseToSolution("mta-datafiles-missing", ASK_GO_ONLINE, strMessage);
                }
                
                return "quit";
            }
            else
            {
                m_strAdminReason = _("Copy MTA:SA files");
                return "fail";
            }
        }
    }

    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessGtaPatchCheck
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessGtaPatchCheck()
{
    const fs::path patchBasePath = FileGenerator::GetPatchBasePath();
    const fs::path patchDiffPath = FileGenerator::GetPatchDiffPath();

    if (!FileGenerator::IsPatchBase(patchBasePath))
    {
        SString strMessage(_("MTA:SA cannot launch because a GTA:SA file is incorrect or missing:"));
        strMessage += "\n\n" + patchBasePath.u8string();
        BrowseToSolution("gengta_pakfiles", ASK_GO_ONLINE, strMessage);
        return "quit";
    }

    if (!FileGenerator::IsPatchDiff(patchDiffPath))
    {
        SString strMessage(_("MTA:SA cannot launch because an MTA:SA file is incorrect or missing:"));
        strMessage += "\n\n" + patchDiffPath.u8string();
        BrowseToSolution("mta-datafiles-missing", ASK_GO_ONLINE, strMessage);
        return "quit";
    }

    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessGtaDllCheck
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessGtaDllCheck()
{
    // Ensure GTA exe is not running
    TerminateGTAIfRunning();

    struct DependencyHash
    {
        const char* fileName;
        FileHash    fileHash;
    };

    // clang-format off
    constexpr std::array<DependencyHash, 4> dependencies{{
        {"eax.dll",         FileHash{0xb2, 0xda, 0x4f, 0x1e, 0x47, 0xef, 0x80, 0x54, 0xc8, 0x39, 0x0e, 0xad, 0x0b, 0x97, 0xd1, 0xfb,
                                     0xb0, 0xc5, 0x47, 0x24, 0x5b, 0x79, 0xb8, 0x86, 0x1c, 0xfa, 0x92, 0xce, 0x9e, 0xf1, 0x53, 0xfb}},
        {"ogg.dll",         FileHash{0x4a, 0x4f, 0x65, 0x42, 0x7e, 0x01, 0x6b, 0x3c, 0x5a, 0xe0, 0xd2, 0x51, 0x7a, 0x69, 0xdb, 0x5f,
                                     0x1c, 0xdc, 0x7a, 0x43, 0xd2, 0xc0, 0xa7, 0x95, 0x7e, 0x8d, 0xa5, 0xd6, 0xf3, 0x78, 0xf0, 0x63}},
        {"vorbis.dll",      FileHash{0xfe, 0xfd, 0xa8, 0x50, 0xb6, 0x9e, 0x00, 0x7f, 0xce, 0xba, 0x64, 0x44, 0x83, 0xc7, 0x61, 0x6b,
                                     0xc0, 0x7e, 0x9f, 0x17, 0x7f, 0xc6, 0x34, 0xfb, 0x74, 0xe1, 0x14, 0xf0, 0xd1, 0x5b, 0x0d, 0xb0}},
        {"vorbisFile.dll",  FileHash{0xa0, 0x89, 0x23, 0x47, 0x90, 0x00, 0xce, 0xc3, 0x66, 0x96, 0x7f, 0xb8, 0x25, 0x9e, 0x09, 0x20,
                                     0xb7, 0xaa, 0x18, 0x85, 0x97, 0x22, 0xc7, 0xdd, 0xa1, 0x41, 0x57, 0x26, 0xbe, 0xd4, 0x77, 0x4f}},
    }};
    // clang-format on

    const fs::path launchDir = GetGameLaunchDirectory();
    const bool     isAdmin = IsUserAdmin();
    
    FileGenerator gtasa{};

    for (size_t i = 0; i < dependencies.size(); ++i)
    {
        const DependencyHash& dependency = dependencies[i];
        std::filesystem::path dependecyPath = launchDir / dependency.fileName;

        std::error_code ec1{};
        FileHash        hash{};

        if (GetFileHash(dependecyPath, hash, ec1) && hash == dependency.fileHash)
            continue;

        std::error_code ec2{};

        if (gtasa.GenerateFile(std::to_string(i), dependecyPath, ec2))
            continue;

        if (IsErrorCodeLoggable(ec1) || IsErrorCodeLoggable(ec2))
        {
            const uintmax_t fileSize = GetFileSize(dependecyPath);
            AddReportLog(3052, SString("_ProcessGtaDllCheck: GenerateFile failed (err-1: %d, err-2: %d, size: %ju, admin: %d)", ec1.value(), ec2.value(),
                                       fileSize, isAdmin));
        }

        if (isAdmin)
        {
            SString strMessage(_("MTA:SA cannot launch because a GTA:SA file is incorrect or missing:"));
            strMessage += "\n\n" + dependecyPath.u8string();
            BrowseToSolution(SString("gendep_error&name=%s", dependency.fileName), ASK_GO_ONLINE, strMessage);
            return "quit";
        }
        else
        {
            m_strAdminReason = _("Patch GTA:SA dependency");
            return "fail";
        }
    }

    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessGtaVersionCheck
//
// Make sure gta exe is the correct version
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessGtaVersionCheck()
{
    const fs::path gtaExePath = GetGameExecutablePath();
    const bool     isAdmin = IsUserAdmin();

    GameExecutablePatcher patcher{gtaExePath};
    FileGenerator         gtasa{};

    std::error_code ec{};

    if (!patcher.Load(ec))
    {
        if (ec)
        {
            if (IsErrorCodeLoggable(ec))
            {
                const uintmax_t fileSize = GetFileSize(gtaExePath);
                AddReportLog(3052, SString("_ProcessGtaVersionCheck: Loading #1 failed (err: %d, size: %ju, admin: %d)", ec.value(), fileSize, isAdmin));
            }

            ec.clear();
        }

        if (!gtasa.GenerateFile("data", gtaExePath, ec))
        {
            if (IsErrorCodeLoggable(ec))
            {
                const uintmax_t fileSize = GetFileSize(gtaExePath);
                AddReportLog(3052, SString("_ProcessGtaVersionCheck: GenerateFile failed (err: %d, size: %ju, admin: %d)", ec.value(), fileSize, isAdmin));
            }

            if (isAdmin)
            {
                SString strMessage(_("MTA:SA cannot launch because the GTA:SA executable is incorrect or missing:"));
                strMessage += "\n\n" + gtaExePath.u8string();
                strMessage +=
                    "\n\n" +
                    _("Please check your anti-virus for a false-positive detection, try to add an exception for the GTA:SA executable and restart MTA:SA.");
                BrowseToSolution(SString("gengta_error&code=%d", ec.value()), ASK_GO_ONLINE, strMessage);
                return "quit";
            }
            else
            {
                m_strAdminReason = _("Generate GTA:SA");
                return "fail";
            }
        }

        if (!patcher.Load(ec))
        {
            if (IsErrorCodeLoggable(ec))
            {
                const uintmax_t fileSize = GetFileSize(gtaExePath);
                AddReportLog(3052, SString("_ProcessGtaVersionCheck: Loading #2 failed (err: %d, size: %ju, admin: %d)", ec.value(), fileSize, isAdmin));
            }

            if (isAdmin)
            {
                SString strMessage(_("MTA:SA cannot launch because the GTA:SA executable is not loadable:"));
                strMessage += "\n\n" + gtaExePath.u8string();
                BrowseToSolution(SString("gengta_error&code=%d", ec.value()), ASK_GO_ONLINE, strMessage);
                return "quit";
            }
            else
            {
                m_strAdminReason = _("Patch GTA:SA");
                return "fail";
            }
        }
    }

    if (!patcher.ApplyPatches(ec))
    {
        if (IsErrorCodeLoggable(ec))
        {
            const uintmax_t fileSize = GetFileSize(gtaExePath);
            AddReportLog(3052, SString("_ProcessGtaVersionCheck: ApplyPatches failed (err: %d, size: %ju, admin: %d)", ec.value(), fileSize, isAdmin));
        }

        if (isAdmin)
        {
            SString strMessage(_("MTA:SA cannot launch because patching GTA:SA has failed:"));
            strMessage += "\n\n" + gtaExePath.u8string();
            BrowseToSolution(SString("patchgta_error&code=%d", ec.value()), ASK_GO_ONLINE, strMessage);
            return "quit";
        }
        else
        {
            m_strAdminReason = _("Patch GTA:SA");
            return "fail";
        }
    }

    SetApplicationSetting("gta-exe-md5", patcher.GenerateMD5());
    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::UpdateOptimusSymbolExport
//
// Try to restore or apply 'HighPerformanceGraphicsPatch'.
// Return false if admin required
//
//////////////////////////////////////////////////////////
bool CInstallManager::UpdateOptimusSymbolExport()
{
    return _ProcessGtaVersionCheck() == "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessLayoutChecks
//
// Make sure new reg/dir structure is ok
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessLayoutChecks()
{
    //
    // Validation
    //

    // Check data dirs exists and writable
    for (uint i = 0; i < 2; i++)
    {
        SString strMTADataPath = i ? GetMTADataPathCommon() : GetMTADataPath();

        if (!DirectoryExists(strMTADataPath))
            ShowLayoutError("[Data directory not present]");            // Can't find directory

        SString strTestFilePath = PathJoin(strMTADataPath, "testdir", "testfile.txt");

        FileDelete(strTestFilePath);
        RemoveDirectory(ExtractPath(strTestFilePath));

        SString strContent = "test";
        if (!FileSave(strTestFilePath, strContent))
            ShowLayoutError("[Data directory not writable]");            // Can't save file

        FileDelete(strTestFilePath);
        RemoveDirectory(ExtractPath(strTestFilePath));
    }

#if MTASA_VERSION_TYPE != VERSION_TYPE_CUSTOM
    // Check reg key exists
    {
        if (GetRegistryValue("", "Last Install Location").empty())
            ShowLayoutError("[Registry key not present]");            // Can't find reg key
    }
#endif

    // Check reg key writable
    {
        RemoveRegistryKey("testkeypath");

        SString strValue = GetTimeString(true, true);
        SetRegistryValue("testkeypath", "testname", strValue);
        SString strValueCheck = GetRegistryValue("testkeypath", "testname");
        if (strValueCheck != strValue)
            ShowLayoutError("[Registry key not writable]");            // Can't write reg key

        RemoveRegistryKey("testkeypath");
    }

    // Check install dir writable
    {
        SString strTestFilePath = CalcMTASAPath(PathJoin("mta", "writetest.txt"));

        FileDelete(strTestFilePath);

        SString strContent = "test";
        if (!FileSave(strTestFilePath, strContent))
            ShowLayoutError("[Install directory not writable]");            // Can't save file

        FileDelete(strTestFilePath);
    }

    //
    // Migration
    //

    // If news/temp/upcache folder doesn't exist in new, but does in old place, move it
    {
        const char* folders[] = {"news", "temp", "upcache"};
        for (uint i = 0; i < NUMELMS(folders); i++)
        {
            SString strSrc = PathJoin(GetSystemLocalAppDataPath(), "MTA San Andreas " + GetMajorVersionString(), folders[i]);
            SString strDest = PathJoin(GetMTADataPath(), folders[i]);
            if (!DirectoryExists(strDest) && DirectoryExists(strSrc))
                MoveFile(strSrc, strDest);
        }
    }

    // Set aero option if reg entry doesn't exist
    {
        if (GetApplicationSetting("aero-enabled").empty())
        {
            SetApplicationSettingInt("aero-enabled", 1);
        }
    }

    // New log/config file layout
    {
        SString strOldDir = CalcMTASAPath("MTA");
        SString strNewConfigDir = CalcMTASAPath(PathJoin("MTA", "config"));
        SString strNewLogDir = CalcMTASAPath(PathJoin("MTA", "logs"));

        MakeSureDirExists(strNewConfigDir + "/");
        MakeSureDirExists(strNewLogDir + "/");

        // Move usable files if they exist
        const char* migrateConfigs[] = {"coreconfig.xml", "chatboxpresets.xml", "servercache.xml"};
        for (uint i = 0; i < NUMELMS(migrateConfigs); i++)
            MigrateFile(PathJoin(strOldDir, migrateConfigs[i]), PathJoin(strNewConfigDir, migrateConfigs[i]));

        const char* migrateLogs[] = {"CEGUI.log", "console-input.log", "clientscript.log", "logfile.txt"};
        for (uint i = 0; i < NUMELMS(migrateLogs); i++)
            MigrateFile(PathJoin(strOldDir, migrateLogs[i]), PathJoin(strNewLogDir, migrateLogs[i]));

        // Delete unusable files
        const char* deleteLogs[] = {"clientscript.log.bak", "logfile_old.txt"};
        for (uint i = 0; i < NUMELMS(deleteLogs); i++)
            FileDelete(PathJoin(strOldDir, deleteLogs[i]));

        // Delete old console log files
        std::vector<SString> fileList = FindFiles(PathJoin(strOldDir, "logs", "console_logfile-*"), true, false);
        for (uint i = 0; i < fileList.size(); i++)
            FileDelete(PathJoin(strOldDir, "logs", fileList[i]));
    }

    // Remove superfluous files from MTA subdirectory.
    {
        const SString mtaDir = CalcMTASAPath("mta");

        for (const char* fileName : {"winmm.dll", "vea.dll", "vog.dll", "vvo.dll", "vvof.dll"})
        {
            SString filePath = PathJoin(mtaDir, fileName);

            if (FileExists(filePath))
            {
                FileDelete(filePath);
            }
        }
    }

    // Remove proxy_sa.exe from GTA directory.
    {
        SString filePath = PathJoin(GetGTAPath(), "proxy_sa.exe");

        if (FileExists(filePath))
        {
            FileDelete(filePath);
        }
    }

    //
    // Disk space check
    //
    SString strDriveWithNoSpace = GetDriveNameWithNotEnoughSpace();
    if (!strDriveWithNoSpace.empty())
    {
        SString strMessage(_("MTA:SA cannot continue because drive %s does not have enough space."), *strDriveWithNoSpace.Left(1));
        BrowseToSolution(SString("low-disk-space&drive=%s", *strDriveWithNoSpace), ASK_GO_ONLINE | TERMINATE_PROCESS, strMessage);
    }

    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessLangFileChecks
//
// Make sure required language files exist
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessLangFileChecks()
{
    if (!HasGTAPath())
        return "ok";

    SString     strTextFilePath = PathJoin(GetGTAPath(), "text");
    const char* langFileNames[] = {"american.gxt", "french.gxt", "german.gxt", "italian.gxt", "spanish.gxt"};

    // Get language that will be used
    SString strSettingsFilename = PathJoin(GetSystemPersonalPath(), "GTA San Andreas User Files", "gta_sa.set");
    FILE*   fh = File::Fopen(strSettingsFilename, "rb");
    if (fh)
    {
        fseek(fh, 0xB3B, SEEK_SET);
        char cLang = -1;
        fread(&cLang, 1, 1, fh);
        fclose(fh);
        if (cLang >= 0 && cLang < NUMELMS(langFileNames))
        {
            // Check the selected language file is present
            SString strPathFilename = PathJoin(strTextFilePath, langFileNames[cLang]);
            if (FileSize(strPathFilename) > 10000)
                return "ok";
        }
    }

    // Apply fix if the selected language file is not present, or there is no gta_sa.set file

    // Find file to copy from
    SString strSrcFile;
    for (uint i = 0; i < NUMELMS(langFileNames); i++)
    {
        SString strPathFilename = PathJoin(strTextFilePath, langFileNames[i]);
        if (FileSize(strPathFilename) > 10000)
        {
            strSrcFile = strPathFilename;
            break;
        }
    }

    if (strSrcFile.empty())
    {
        SString strMessage = _("Missing file:");
        strMessage += "\n\n";
        strMessage += PathJoin(strTextFilePath, langFileNames[0]);
        strMessage += "\n\n";
        strMessage += _("If MTA fails to load, please re-install GTA:SA");
        MessageBoxUTF8(NULL, strMessage, "Multi Theft Auto: San Andreas", MB_OK | MB_ICONWARNING | MB_TOPMOST);
        return "ok";
    }

    // Fix missing files by copying
    for (uint i = 0; i < NUMELMS(langFileNames); i++)
    {
        SString strPathFilename = PathJoin(strTextFilePath, langFileNames[i]);
        if (FileSize(strPathFilename) < 10000)
        {
            if (!FileCopy(strSrcFile, strPathFilename))
            {
                m_strAdminReason = "Update language files";
                return "fail";
            }
        }
    }

    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessServiceChecks
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessServiceChecks()
{
    if (!CheckService(CHECK_SERVICE_PRE_GAME))
    {
        if (!IsUserAdmin())
        {
            m_strAdminReason = _("Update install settings");
            return "fail";
        }
    }
    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessAppCompatChecks
//
// Remove/add required options from AppCompatFlags/Layers
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessAppCompatChecks()
{
    BOOL bIsWOW64 = false;            // 64bit OS
    IsWow64Process(GetCurrentProcess(), &bIsWOW64);
    uint    uiHKLMFlags = bIsWOW64 ? KEY_WOW64_64KEY : 0;
    WString strGTAExePathFilename = GetGameExecutablePath().wstring();
    WString strMTAExePathFilename = FromUTF8(GetLaunchPathFilename());
    WString strCompatModeRegKey = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers";
    int     bWin816BitColorOption = GetApplicationSettingInt("Win8Color16");
    int     bWin8MouseOption = GetApplicationSettingInt("Win8MouseFix");

    // Lists of things to add/remove from AppCompatFlags
    std::vector<WString> addList;
    std::vector<WString> removeList;

    // Remove user voodoo
    removeList.push_back(L"WIN95");
    removeList.push_back(L"WIN98");
    removeList.push_back(L"NT4SP5");
    removeList.push_back(L"WIN2000");
    removeList.push_back(L"WINXPSP2");
    removeList.push_back(L"WINXPSP3");
    removeList.push_back(L"WINSRV03SP1");
    removeList.push_back(L"WINSRV08SP1");
    removeList.push_back(L"VISTARTM");
    removeList.push_back(L"VISTASP1");
    removeList.push_back(L"VISTASP2");
    removeList.push_back(L"WIN7RTM");
    removeList.push_back(L"256COLOR");
    removeList.push_back(L"16BITCOLOR");
    removeList.push_back(L"640X480");
    removeList.push_back(L"DISABLETHEMES");
    removeList.push_back(L"DISABLEDWM");
    removeList.push_back(L"HIGHDPIAWARE");

    // Fix for GitHub issue #983 "crash on join server"
#ifdef DEBUG
    removeList.push_back(L"IgnoreFreeLibrary<client_d.dll>");
#else
    removeList.push_back(L"IgnoreFreeLibrary<client.dll>");
#endif

    // Remove potential performance hit
    removeList.push_back(L"FaultTolerantHeap");

    // Handle Windows 8 options
    if (bWin816BitColorOption)
        addList.push_back(L"DWM8And16BitMitigation");
    else
        removeList.push_back(L"DWM8And16BitMitigation");

    if (bWin8MouseOption)
        addList.push_back(L"NoDTToDITMouseBatch");
    else
        removeList.push_back(L"NoDTToDITMouseBatch");

    // Disable hybrid execution mode (x86 apps only) - ARM emulation settings.
    if (IsNativeArm64Host())
        addList.push_back(L"ARM64CHPEDISABLED");
    else
        removeList.push_back(L"ARM64CHPEDISABLED");

    // Details of reg keys to fiddle with
    struct
    {
        WString strProgName;
        HKEY    hKeyRoot;
        uint    uiFlags;
    } items[] = {{strGTAExePathFilename, HKEY_CURRENT_USER, 0},
                 {strGTAExePathFilename, HKEY_LOCAL_MACHINE, uiHKLMFlags},
                 {strMTAExePathFilename, HKEY_CURRENT_USER, 0},
                 {strMTAExePathFilename, HKEY_LOCAL_MACHINE, uiHKLMFlags}};

    bool bTryAdmin = false;
    for (uint i = 0; i < NUMELMS(items); i++)
    {
        // Get current setting
        WString strValue = ReadCompatibilityEntries(items[i].strProgName, strCompatModeRegKey, items[i].hKeyRoot, items[i].uiFlags);

        // Break into words
        std::vector<WString> entryList;
        strValue.Split(" ", entryList);
        ListRemove(entryList, WString());

        // Apply removals
        for (uint a = 0; a < removeList.size(); a++)
            ListRemove(entryList, removeList[a]);

        // Apply adds
        for (uint a = 0; a < addList.size(); a++)
            ListAddUnique(entryList, addList[a]);

        // Clear list if only flags remain
        if (entryList.size() == 1 && entryList[0].size() < 3)
            entryList.clear();

        // Join to one value
        WString strNewValue = WString::Join(L" ", entryList);

        // Save setting
        if (strNewValue != strValue)
            if (!WriteCompatibilityEntries(items[i].strProgName, strCompatModeRegKey, items[i].hKeyRoot, items[i].uiFlags, strNewValue))
                bTryAdmin = true;
    }

    // Windows 7: Fix invalid GameUX URL (which causes rundll32.exe to use excessive CPU)
    WString strUrlKey = L"SOFTWARE\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\GameUX\\ServiceLocation";
    WString strUrlItem = L"Games";
    WString strUrlValue = ReadCompatibilityEntries(strUrlItem, strUrlKey, HKEY_CURRENT_USER, 0);
    if (!strUrlValue.empty())
    {
        WriteDebugEvent(SString("GameUX ServiceLocation was '%s'", *ToUTF8(strUrlValue)));
        if (strUrlValue.ContainsI(L":"))
        {
            strUrlValue = L"disabled";            // Can be anything not containing `:`
            if (!WriteCompatibilityEntries(strUrlItem, strUrlKey, HKEY_CURRENT_USER, 0, strUrlValue))
                bTryAdmin = true;
        }
    }

    // Handle admin requirement
    if (bTryAdmin)
    {
        if (!IsUserAdmin())
        {
            m_strAdminReason = _("Update compatibility settings");
            return "fail";
        }
    }
    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_InstallNewsItems
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_InstallNewsItems()
{
    // Get install news queue
    CArgMap queue;
    queue.SetFromString(GetApplicationSetting("news-install"));
    SetApplicationSetting("news-install", "");

    std::vector<SString> keyList;
    queue.GetKeys(keyList);
    for (uint i = 0; i < keyList.size(); i++)
    {
        // Install each file
        SString strDate = keyList[i];
        SString strFileLocation = queue.Get(strDate);

        // Save cwd
        SString strSavedDir = GetSystemCurrentDirectory();

        // Calc and make target dir
        SString strTargetDir = PathJoin(GetMTADataPath(), "news", strDate);
        MkDir(strTargetDir);

        // Extract into target dir
        SetCurrentDirectory(strTargetDir);

        // Try to extract the files
        if (!ExtractFiles(strFileLocation, false))
        {
            // If extract failed and update file is an exe, try to run it
            if (ExtractExtension(strFileLocation).CompareI("exe"))
                ShellExecuteBlocking("open", strFileLocation, "-s");
        }

        // Restore cwd
        SetCurrentDirectory(strSavedDir);

        // Check result
        if (FileExists(PathJoin(strTargetDir, "files.xml")))
        {
            SetApplicationSettingInt("news-updated", 1);
            AddReportLog(2051, SString("InstallNewsItems ok for '%s'", *strDate));
        }
        else
        {
            AddReportLog(4048, SString("InstallNewsItems failed with '%s' '%s' '%s'", *strDate, *strFileLocation, *strTargetDir));
        }
    }
    return "ok";
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_Quit
//
//
//
//////////////////////////////////////////////////////////
SString CInstallManager::_Quit()
{
    ExitProcess(0);
    return "ok";
}
