/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

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
        CR "            CALL CheckOnRestartCommand "                               //
        CR "            IF LastResult != ok GOTO update_end: "                     //
        CR " "                                                                     ////// Start of 'update game' //////
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
        CR "gta_version_check:"                                                    ////// Start of 'gta version check' //////
        CR "            CALL ProcessGtaVersionCheck "                              //
        CR "            IF LastResult == ok GOTO gta_version_end: "                //
        CR "            IF LastResult == quit GOTO do_quit: "                      //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO gta_version_check: "              //
        CR "            GOTO do_quit: "                                            // If changes as admin failed, then quit
        CR "gta_version_end: "                                                     ////// End of 'gta version check' //////
        CR " "                                                                     //
        CR "newlayout_check:"                                                      ////// Start of 'new layout check' //////
        CR "            CALL ProcessLayoutChecks "                                 //
        CR "            IF LastResult == ok GOTO newlayout_end: "                  //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO newlayout_check: "                //
        CR "newlayout_end: "                                                       ////// End of 'new layout check' //////
        CR " "                                                                     //
        CR "langfile_check: "                                                      ////// Start of 'Lang file fix' //////
        CR "            CALL ProcessLangFileChecks "                               // Make changes to comply with requirements
        CR "            IF LastResult == ok GOTO langfile_end: "                   //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO langfile_check: "                 //
        CR " "                                                                     //
        CR "langfile_end: "                                                        ////// End of 'Lang file fix' //////
        CR " "                                                                     //
        CR "exepatch_check: "                                                      ////// Start of 'Exe patch fix' //////
        CR "            CALL ProcessExePatchChecks "                               // Make changes to comply with requirements
        CR "            IF LastResult == ok GOTO exepatch_end: "                   //
        CR " "                                                                     //
        CR "            CALL ChangeToAdmin "                                       // If changes failed, try as admin
        CR "            IF LastResult == ok GOTO exepatch_check: "                 //
        CR " "                                                                     //
        CR "exepatch_end: "                                                        ////// End of 'Exe patch fix' //////
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
    m_pSequencer->AddFunction("ProcessGtaVersionCheck", &CInstallManager::_ProcessGtaVersionCheck);
    m_pSequencer->AddFunction("ProcessLayoutChecks", &CInstallManager::_ProcessLayoutChecks);
    m_pSequencer->AddFunction("ProcessLangFileChecks", &CInstallManager::_ProcessLangFileChecks);
    m_pSequencer->AddFunction("ProcessExePatchChecks", &CInstallManager::_ProcessExePatchChecks);
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
    if (!InstallFiles(m_pSequencer->GetVariable(HIDE_PROGRESS) != "no"))
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
// CInstallManager::_ProcessGtaVersionCheck
//
// Make sure gta exe is the correct version
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessGtaVersionCheck()
{
    SString strGtaExe = PathJoin(GetGTAPath(), "gta_sa.exe");
    SString strPatchBase = PathJoin(GetGTAPath(), "audio/CONFIG/PakFiles.dat");
    SString strPatchDiff = PathJoin(GetMTASAPath(), "mta/data/gta_sa_diff.dat");
    SString strGtaExeMd5 = "170B3A9108687B26DA2D8901C6948A18";
    SString strPatchBaseMd5 = "DB1E657A3BAAFBB86CD1B715C5282C66";
    // Reset any bytes MTA may have changed in gta_sa.exe
    std::vector<CFileGenerator::SResetItem> gtaExeResetList = {
        {0x0000008B, 1, {0x42}},                              // bTimestamp
        {0x00000096, 1, {0x0F}},                              // bLargeMem
        {0x000000DF, 1, {0x00}},                              // bDep
        {0x000000F8, 8, {0x00}},                              // bNvightmare ExportDir
        {0x004A1AD0, 96, {0x00}},                             // bNvightmare ExportTable
        {0x004C4588, 4, {0x01, 0x00, 0x00, 0x00}},            // bNvightmare ExportValue
    };
    CFileGenerator fileGenerator(strGtaExe, strGtaExeMd5, gtaExeResetList, strPatchBase, strPatchDiff);

    // Need to fix gta_sa.exe?
    bool bGenerationRequired = fileGenerator.IsGenerationRequired();
    SetApplicationSetting("gta-exe-md5", fileGenerator.GetCurrentTargetMd5());
    if (!bGenerationRequired)
    {
        AddReportLog(2053, "_ProcessGtaVersionCheck: No action required");
        return "ok";
    }

    // Check required GTA file is correct
    SString strPatchBaseCurrentMd5 = GenerateHashHexStringFromFile(EHashFunctionType::MD5, strPatchBase);
    if (strPatchBaseCurrentMd5 != strPatchBaseMd5)
    {
        AddReportLog(5053, SString("_ProcessGtaVersionCheck: Incorrect file '%s' %d %s", *strPatchBase, (int)FileSize(strPatchBase), *strPatchBaseCurrentMd5));
        SString strMessage(_("MTA:SA cannot continue because the following files are incorrect:"));
        strMessage += "\n\n" + strPatchBase;
        BrowseToSolution("gengta_pakfiles", ASK_GO_ONLINE, strMessage);
        return "quit";
    }

    // Ensure GTA exe is not running
    TerminateGTAIfRunning();

    // Backup current gta_sa.exe
    SString strGTAExeBak = strGtaExe + ".bak";
    if (!FileExists(strGTAExeBak))
    {
        FileCopy(strGtaExe, strGTAExeBak);
    }

    // Generate new gta_sa.exe
    CFileGenerator::EResult result = fileGenerator.GenerateFile();

    // Handle result
    if (result != CFileGenerator::EResult::Success)
    {
        if (!IsUserAdmin())
        {
            AddReportLog(3052, SString("_ProcessGtaVersionCheck: GenerateFile failed (%d) - trying as admin %s", result, *fileGenerator.GetErrorRecords()));
            m_strAdminReason = _("Patch GTA");
            return "fail";
        }
        else
        {
            AddReportLog(
                5052, SString("_ProcessGtaVersionCheck: GenerateFile failed (%d) to generate '%s' %s", result, *strGtaExe, *fileGenerator.GetErrorRecords()));
            SString strMessage(_("MTA:SA cannot continue because the following files are incorrect:"));
            strMessage += "\n\n" + strGtaExe;
            strMessage += "\n\n" + _("Error") + SString(" %d", result);
            BrowseToSolution(SString("gengta_error&code=%d", result), ASK_GO_ONLINE, strMessage);
            return "quit";
        }
    }

    SetApplicationSetting("gta-exe-md5", fileGenerator.GetCurrentTargetMd5());
    AddReportLog(2052, "_ProcessGtaVersionCheck: success");
    return "ok";
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
// CInstallManager::UpdateOptimusSymbolExport
//
// Try to update NvightmareChecks
// Return false if admin required
//
//////////////////////////////////////////////////////////
bool CInstallManager::UpdateOptimusSymbolExport()
{
    return _ProcessExePatchChecks() == "ok";
}

//////////////////////////////////////////////////////////
//
// MaybeRenameExe
//
// Figure out whether to use a renamed exe, and return filename to use.
// Also tries file copy if required.
//
//////////////////////////////////////////////////////////
SString CInstallManager::MaybeRenameExe(const SString& strGTAPath)
{
    // Try patch/copy (may fail if not admin)
    _ProcessExePatchChecks();

    SString strGTAEXEPath = PathJoin(strGTAPath, MTA_GTAEXE_NAME);
    if (ShouldUseExeCopy())
    {
        // See if exe copy seems usable
        SString strHTAEXEPath = PathJoin(strGTAPath, MTA_HTAEXE_NAME);
        uint64  uiStdFileSize = FileSize(strGTAEXEPath);
        if (uiStdFileSize && FileSize(strHTAEXEPath) > 10 * 1024 * 1024)
            strGTAEXEPath = strHTAEXEPath;
    }

    return strGTAEXEPath;
}

//////////////////////////////////////////////////////////
//
// CInstallManager::_ProcessExePatchChecks
//
// Check which changes need to be made to the exe and make a copy if required
//
//////////////////////////////////////////////////////////
SString CInstallManager::_ProcessExePatchChecks()
{
    if (!HasGTAPath())
        return "ok";

    SExePatchedStatus reqStatus = GetExePatchRequirements();

    if (!ShouldUseExeCopy())
    {
        // Simple patching if won't be using a exe copy
        if (!SetExePatchedStatus(false, reqStatus))
        {
            m_strAdminReason = GetPatchExeAdminReason(false, reqStatus);
            return "fail";
        }
    }
    else
    {
        // Bit more complercated if will be using a exe copy

        // First check if we need to create/update the exe copy
        if (GetExePatchedStatus(true) != reqStatus || GetExeFileSize(false) != GetExeFileSize(true))
        {
            // Copy
            if (!CopyExe())
            {
                m_strAdminReason = GetPatchExeAdminReason(true, reqStatus);
                return "fail";
            }
            // Apply patches
            if (!SetExePatchedStatus(true, reqStatus))
            {
                m_strAdminReason = GetPatchExeAdminReason(true, reqStatus);
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
    WString strGTAExePathFilename = FromUTF8(GetUsingExePathFilename());
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
        if (!ExtractFiles(strFileLocation))
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
    // return "ok";
}
