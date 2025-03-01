/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Misc.hpp
 *  PURPOSE:
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "SharedUtil.Misc.h"
#include "SharedUtil.Time.h"
#include <map>
#include "UTF8.h"
#include "UTF8Detect.hpp"
#include "CDuplicateLineFilter.h"
#include "version.h"
#ifdef WIN32
    #include <ctime>
    #include <windows.h>
    #include <direct.h>
    #include <shellapi.h>
    #include <TlHelp32.h>
    #include <Psapi.h>
#else
    #include <wctype.h>
    #ifndef _GNU_SOURCE
    # define _GNU_SOURCE    /* See feature_test_macros(7) */
    #endif
    #include <sched.h>
    #include <sys/time.h>
    #include <sys/resource.h>
    #ifndef RUSAGE_THREAD
        #define    RUSAGE_THREAD    1        /* only the calling thread */
    #endif
#endif

#if defined(__APPLE__) && !defined(__aarch64__)
    #include <cpuid.h>
#endif

CCriticalSection     CRefCountable::ms_CS;
std::map<uint, uint> ms_ReportAmountMap;
SString              ms_strProductRegistryPath;
SString              ms_strProductCommonDataDir;
SString              ms_strProductVersion;

struct SReportLine
{
    SString strText;
    uint    uiId;
    void    operator+=(const char* szAppend) { strText += szAppend; }
    bool    operator==(const SReportLine& other) const { return strText == other.strText && uiId == other.uiId; }
};
CDuplicateLineFilter<SReportLine> ms_ReportLineFilter;

#ifdef MTA_CLIENT

#define PRODUCT_REGISTRY_PATH       "Software\\Multi Theft Auto: San Andreas All"       // HKLM
#define PRODUCT_COMMON_DATA_DIR     "MTA San Andreas All"                               // C:\ProgramData
#define TROUBLE_URL1 "https://help.multitheftauto.com/sa/trouble/?v=_VERSION_&id=_ID_&tr=_TROUBLE_"

//
// Output a UTF8 encoded messagebox
// Used in the Win32 Client only
//
#ifdef _WINDOWS_ //Only for modules that use windows.h
int SharedUtil::MessageBoxUTF8(HWND hWnd, SString lpText, SString lpCaption, UINT uType)
{
    // Default to warning icon
    if ((uType & ICON_MASK_VALUE) == 0)
        uType |= ICON_WARNING;
    // Make topmost work
    if (uType & MB_TOPMOST)
        uType |= MB_SYSTEMMODAL;
    WString strText = MbUTF8ToUTF16(lpText);
    WString strCaption = MbUTF8ToUTF16(lpCaption);
    return MessageBoxW(hWnd, strText.c_str(), strCaption.c_str(), uType);
}
#endif

//
// Return full path and filename of parent exe
//
SString SharedUtil::GetParentProcessPathFilename(int pid)
{
    HANDLE          hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W pe = {sizeof(PROCESSENTRY32W)};
    if (Process32FirstW(hSnapshot, &pe))
    {
        do
        {
            if (pe.th32ProcessID == pid)
            {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe.th32ParentProcessID);
                if (hProcess)
                {
                    WCHAR szModuleName[MAX_PATH * 2] = {0};
                    GetModuleFileNameExW(hProcess, nullptr, szModuleName, NUMELMS(szModuleName));
                    CloseHandle(hProcess);
                    SString strModuleName = ToUTF8(szModuleName);
                    if (FileExists(strModuleName))
                    {
                        CloseHandle(hSnapshot);
                        if (IsShortPathName(strModuleName))
                            return GetSystemLongPathName(strModuleName);
                        return strModuleName;
                    }
                }
            }
        } while (Process32NextW(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return "";
}

//
// Get startup directory as saved in the registry by the launcher
// Used in the Win32 Client only
//
SString SharedUtil::GetMTASABaseDir()
{
    static SString strInstallRoot;
    if (strInstallRoot.empty())
    {
        if (IsGTAProcess())
        {
            // Try to get base dir from parent process
            strInstallRoot = ExtractPath(GetParentProcessPathFilename(GetCurrentProcessId()));
        }
        if (strInstallRoot.empty())
        {
            strInstallRoot = GetRegistryValue("", "Last Run Location");
            if (strInstallRoot.empty())
            {
                MessageBoxUTF8(0, _("Multi Theft Auto has not been installed properly, please reinstall."), _("Error") + _E("U01"),
                               MB_OK | MB_ICONERROR | MB_TOPMOST);
                TerminateProcess(GetCurrentProcess(), 9);
            }
        }
    }
    return strInstallRoot;
}

//
// Turns a relative MTASA path i.e. "MTA\file.dat"
// into an absolute MTASA path i.e. "C:\Program Files\MTA San Andreas\MTA\file.dat"
//
SString SharedUtil::CalcMTASAPath(const SString& strPath)
{
    return PathJoin(GetMTASABaseDir(), strPath);
}

//
// Returns true if current process is GTA (i.e not MTA process)
//
bool SharedUtil::IsGTAProcess()
{
    SString strLaunchPathFilename = GetLaunchPathFilename();
    return strLaunchPathFilename.EndsWithI("gta_sa.exe");
}

//
// Write a registry string value
//
static void WriteRegistryStringValue(HKEY hkRoot, const char* szSubKey, const char* szValue, const SString& strBuffer, bool bFlush = false)
{
    HKEY    hkTemp;
    WString wstrSubKey = FromUTF8(szSubKey);
    WString wstrValue = FromUTF8(szValue);
    WString wstrBuffer = FromUTF8(strBuffer);
    RegCreateKeyExW(hkRoot, wstrSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkTemp, NULL);
    if (hkTemp)
    {
        RegSetValueExW(hkTemp, wstrValue, NULL, REG_SZ, (LPBYTE)wstrBuffer.c_str(), (wstrBuffer.length() + 1) * sizeof(wchar_t));
        if (bFlush)
        {
            // Very slow. Only needed if there is a risk of BSOD soon afterwards.
            RegFlushKey(hkTemp);
        }
        RegCloseKey(hkTemp);
    }
}

//
// Read a registry string value
//
static SString ReadRegistryStringValue(HKEY hkRoot, const char* szSubKey, const char* szValue, int* iResult)
{
    // Clear output
    SString strOutResult = "";

    bool    bResult = false;
    HKEY    hkTemp = NULL;
    WString wstrSubKey = FromUTF8(szSubKey);
    WString wstrValue = FromUTF8(szValue);
    if (RegOpenKeyExW(hkRoot, wstrSubKey, 0, KEY_READ, &hkTemp) == ERROR_SUCCESS)
    {
        DWORD dwBufferSize;
        if (RegQueryValueExW(hkTemp, wstrValue, NULL, NULL, NULL, &dwBufferSize) == ERROR_SUCCESS)
        {
            CScopeAlloc<wchar_t> szBuffer(dwBufferSize + sizeof(wchar_t));
            if (RegQueryValueExW(hkTemp, wstrValue, NULL, NULL, (LPBYTE)(wchar_t*)szBuffer, &dwBufferSize) == ERROR_SUCCESS)
            {
                szBuffer[dwBufferSize / sizeof(wchar_t)] = 0;
                strOutResult = ToUTF8((wchar_t*)szBuffer);
                bResult = true;
            }
        }
        RegCloseKey(hkTemp);
    }
    if (iResult)
        *iResult = bResult;
    return strOutResult;
}

//
// Delete a registry key and its subkeys
//
static bool DeleteRegistryKey(HKEY hkRoot, const char* szSubKey)
{
    return RegDeleteKey(hkRoot, szSubKey) == ERROR_SUCCESS;
}

//
// GetMajorVersionString
//
SString SharedUtil::GetMajorVersionString()
{
    return SStringX(MTA_DM_ASE_VERSION).Left(3);
}

//
// GetSystemRegistryValue
//
SString SharedUtil::GetSystemRegistryValue(uint hKey, const SString& strPath, const SString& strName)
{
    return ReadRegistryStringValue((HKEY)hKey, strPath, strName, NULL);
}

//
// New layout:
//              HKLM Software\\Multi Theft Auto: San Andreas All\\Common    - For all versions
//              HKLM Software\\Multi Theft Auto: San Andreas All\\1.1       - For 1.1
//              HKLM Software\\Multi Theft Auto: San Andreas All\\1.2       - For 1.2
//
static SString MakeVersionRegistryPath(const SString& strVersion, const SString& strPath)
{
    SString strResult = PathJoin(GetProductRegistryPath(), strVersion, strPath);
    return strResult.TrimEnd("\\");
}

//
// Registry values
//
// Get/set registry values for the current version
void SharedUtil::SetRegistryValue(const SString& strPath, const SString& strName, const SString& strValue, bool bFlush)
{
    WriteRegistryStringValue(HKEY_LOCAL_MACHINE, MakeVersionRegistryPath(GetMajorVersionString(), strPath), strName, strValue, bFlush);
}

SString SharedUtil::GetRegistryValue(const SString& strPath, const SString& strName)
{
    return ReadRegistryStringValue(HKEY_LOCAL_MACHINE, MakeVersionRegistryPath(GetMajorVersionString(), strPath), strName, NULL);
}

bool SharedUtil::RemoveRegistryKey(const SString& strPath)
{
    return DeleteRegistryKey(HKEY_LOCAL_MACHINE, MakeVersionRegistryPath(GetMajorVersionString(), strPath));
}

// Get/set registry values for a version
void SharedUtil::SetVersionRegistryValue(const SString& strVersion, const SString& strPath, const SString& strName, const SString& strValue)
{
    WriteRegistryStringValue(HKEY_LOCAL_MACHINE, MakeVersionRegistryPath(strVersion, strPath), strName, strValue);
}

SString SharedUtil::GetVersionRegistryValue(const SString& strVersion, const SString& strPath, const SString& strName)
{
    return ReadRegistryStringValue(HKEY_LOCAL_MACHINE, MakeVersionRegistryPath(strVersion, strPath), strName, NULL);
}

// Get/set registry values for all versions (common)
void SharedUtil::SetCommonRegistryValue(const SString& strPath, const SString& strName, const SString& strValue)
{
    WriteRegistryStringValue(HKEY_LOCAL_MACHINE, MakeVersionRegistryPath("Common", strPath), strName, strValue);
}

SString SharedUtil::GetCommonRegistryValue(const SString& strPath, const SString& strName)
{
    return ReadRegistryStringValue(HKEY_LOCAL_MACHINE, MakeVersionRegistryPath("Common", strPath), strName, NULL);
}

//
// Run ShellExecute with these parameters after exit
//
void SharedUtil::SetOnQuitCommand(const SString& strOperation, const SString& strFile, const SString& strParameters, const SString& strDirectory,
                                  const SString& strShowCmd)
{
    // Encode into a string and set a registry key
    SString strValue("%s\t%s\t%s\t%s\t%s", strOperation.c_str(), strFile.c_str(), strParameters.c_str(), strDirectory.c_str(), strShowCmd.c_str());
    SetRegistryValue("", "OnQuitCommand", strValue);
}

//
// What to do on next restart
//
void SharedUtil::SetOnRestartCommand(const SString& strOperation, const SString& strFile, const SString& strParameters, const SString& strDirectory,
                                     const SString& strShowCmd)
{
    // Encode into a string and set a registry key
    SString strVersion("%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD);
    SString strValue("%s\t%s\t%s\t%s\t%s\t%s", strOperation.c_str(), strFile.c_str(), strParameters.c_str(), strDirectory.c_str(), strShowCmd.c_str(),
                     strVersion.c_str());
    SetRegistryValue("", "OnRestartCommand", strValue);
}

//
// What to do on next restart
//
bool SharedUtil::GetOnRestartCommand(SString& strOperation, SString& strFile, SString& strParameters, SString& strDirectory, SString& strShowCmd)
{
    SString strOnRestartCommand = GetRegistryValue("", "OnRestartCommand");
    SetOnRestartCommand("");

    std::vector<SString> vecParts;
    strOnRestartCommand.Split("\t", vecParts);
    if (vecParts.size() > 5 && vecParts[0].length())
    {
        SString strVersion("%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD);
        if (vecParts[5] == strVersion)
        {
            strOperation = vecParts[0];
            strFile = vecParts[1];
            strParameters = vecParts[2];
            strDirectory = vecParts[3];
            strShowCmd = vecParts[4];
            return true;
        }
        AddReportLog(4000, SString("OnRestartCommand disregarded due to version change %s -> %s", vecParts[5].c_str(), strVersion.c_str()));
    }
    return false;
}

//
// What server to connect to after update
//
void SharedUtil::SetPostUpdateConnect(const SString& strHost)
{
    CArgMap argMap;
    argMap.Set("host", strHost);
    argMap.Set("time", std::to_string((int64_t)time(nullptr)));
    SetRegistryValue("", "PostUpdateConnect", argMap.ToString());
}

//
// What server to connect to after update
//
SString SharedUtil::GetPostUpdateConnect()
{
    SString strPostUpdateConnect = GetRegistryValue("", "PostUpdateConnect");
    SetPostUpdateConnect("");

    CArgMap argMap;
    argMap.SetFromString(strPostUpdateConnect);
    SString strHost = argMap.Get("host");
    time_t  timeThen = (time_t)std::atoll(argMap.Get("time"));

    // Expire after 5 mins
    double seconds = difftime(time(NULL), timeThen);
    if ((seconds < 0 || seconds > 60 * 5) && timeThen != 0)
        strHost = "";

    return strHost;
}

//
// Application settings
//

//
// Get/set string
//
void SharedUtil::SetApplicationSetting(const SString& strPath, const SString& strName, const SString& strValue)
{
    SetRegistryValue(PathJoin("Settings", strPath), strName, strValue);
}

SString SharedUtil::GetApplicationSetting(const SString& strPath, const SString& strName)
{
    return GetRegistryValue(PathJoin("Settings", strPath), strName);
}

// Delete a setting key
bool SharedUtil::RemoveApplicationSettingKey(const SString& strPath)
{
    return RemoveRegistryKey(PathJoin("Settings", strPath));
}

//
// Get/set int
//
void SharedUtil::SetApplicationSettingInt(const SString& strPath, const SString& strName, int iValue)
{
    SetApplicationSetting(strPath, strName, SString("%d", iValue));
}

int SharedUtil::GetApplicationSettingInt(const SString& strPath, const SString& strName)
{
    return atoi(GetApplicationSetting(strPath, strName));
}

int SharedUtil::IncApplicationSettingInt(const SString& strPath, const SString& strName)
{
    int iValue = GetApplicationSettingInt(strPath, strName) + 1;
    SetApplicationSettingInt(strPath, strName, iValue);
    return iValue;
}

//
// Get/set string - Which uses 'general' key
//
void SharedUtil::SetApplicationSetting(const SString& strName, const SString& strValue)
{
    SetApplicationSetting("general", strName, strValue);
}

SString SharedUtil::GetApplicationSetting(const SString& strName)
{
    return GetApplicationSetting("general", strName);
}

void SharedUtil::SetApplicationSettingInt(const SString& strName, int iValue)
{
    SetApplicationSettingInt("general", strName, iValue);
}

int SharedUtil::GetApplicationSettingInt(const SString& strName)
{
    return GetApplicationSettingInt("general", strName);
}

int SharedUtil::IncApplicationSettingInt(const SString& strName)
{
    int iValue = GetApplicationSettingInt(strName) + 1;
    SetApplicationSettingInt(strName, iValue);
    return iValue;
}

//
// WatchDog
//

void SharedUtil::WatchDogReset()
{
    RemoveApplicationSettingKey("watchdog");
}

// Section
bool SharedUtil::WatchDogIsSectionOpen(const SString& str)
{
    return GetApplicationSettingInt("watchdog", str) != 0;
}

void SharedUtil::WatchDogBeginSection(const SString& str)
{
    SetApplicationSettingInt("watchdog", str, 1);
}

void SharedUtil::WatchDogCompletedSection(const SString& str)
{
    SetApplicationSettingInt("watchdog", str, 0);
}

// Counter
void SharedUtil::WatchDogIncCounter(const SString& str)
{
    SetApplicationSettingInt("watchdog", str, WatchDogGetCounter(str) + 1);
}

int SharedUtil::WatchDogGetCounter(const SString& str)
{
    return GetApplicationSettingInt("watchdog", str);
}

void SharedUtil::WatchDogClearCounter(const SString& str)
{
    SetApplicationSettingInt("watchdog", str, 0);
}

//
// Unclean stop flag
//
static bool bWatchDogWasUncleanStopCached = false;
static bool bWatchDogWasUncleanStopValue = false;

bool SharedUtil::WatchDogWasUncleanStop()
{
    if (!bWatchDogWasUncleanStopCached)
    {
        bWatchDogWasUncleanStopValue = GetApplicationSettingInt("watchdog", "uncleanstop") != 0;
        bWatchDogWasUncleanStopCached = true;
    }
    return bWatchDogWasUncleanStopValue;
}

void SharedUtil::WatchDogSetUncleanStop(bool bOn)
{
    SetApplicationSettingInt("watchdog", "uncleanstop", bOn);
    bWatchDogWasUncleanStopCached = true;
    bWatchDogWasUncleanStopValue = bOn;
}

//
// Crash flag
//
static bool bWatchDogWasLastRunCrashCached = false;
static bool bWatchDogWasLastRunCrashValue = false;

bool SharedUtil::WatchDogWasLastRunCrash()
{
    if (!bWatchDogWasLastRunCrashCached)
    {
        bWatchDogWasLastRunCrashValue = GetApplicationSettingInt("watchdog", "lastruncrash") != 0;
        bWatchDogWasLastRunCrashCached = true;
    }
    return bWatchDogWasLastRunCrashValue;
}

void SharedUtil::WatchDogSetLastRunCrash(bool bOn)
{
    SetApplicationSettingInt("watchdog", "lastruncrash", bOn);
    bWatchDogWasLastRunCrashCached = true;
    bWatchDogWasLastRunCrashValue = bOn;
}

//
// Special things
//
void SharedUtil::WatchDogUserDidInteractWithMenu()
{
    WatchDogCompletedSection(WD_SECTION_NOT_USED_MAIN_MENU);
    WatchDogCompletedSection(WD_SECTION_POST_INSTALL);
}

void SharedUtil::SetProductRegistryPath(const SString& strRegistryPath)
{
    assert(ms_strProductRegistryPath.empty() && !strRegistryPath.empty());
    ms_strProductRegistryPath = strRegistryPath;
}

const SString& SharedUtil::GetProductRegistryPath()
{
    if (ms_strProductRegistryPath.empty())
        ms_strProductRegistryPath = PRODUCT_REGISTRY_PATH;
    return ms_strProductRegistryPath;
}

void SharedUtil::SetProductCommonDataDir(const SString& strCommonDataDir)
{
    assert(ms_strProductCommonDataDir.empty() && !strCommonDataDir.empty());
    ms_strProductCommonDataDir = strCommonDataDir;
}

const SString& SharedUtil::GetProductCommonDataDir()
{
    if (ms_strProductCommonDataDir.empty())
        ms_strProductCommonDataDir = PRODUCT_COMMON_DATA_DIR;
    return ms_strProductCommonDataDir;
}

void SharedUtil::SetProductVersion(const SString& strVersion)
{
    assert(ms_strProductVersion.empty() && !strVersion.empty());
    ms_strProductVersion = strVersion;
}

const SString& SharedUtil::GetProductVersion()
{
    if (ms_strProductVersion.empty())
        ms_strProductVersion =
            SString("%d.%d.%d-%d.%05d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD);
    return ms_strProductVersion;
}

void SharedUtil::SetClipboardText(const SString& strText)
{
    // If we got something to copy
    if (!strText.empty())
    {
        // Convert it to Unicode
        WString strUTF = MbUTF8ToUTF16(strText);

        // Open and empty the clipboard
        OpenClipboard(NULL);
        EmptyClipboard();

        // Allocate the clipboard buffer and copy the data
        HGLOBAL  hBuf = GlobalAlloc(GMEM_DDESHARE, strUTF.length() * sizeof(wchar_t) + sizeof(wchar_t));
        wchar_t* buf = reinterpret_cast<wchar_t*>(GlobalLock(hBuf));
        wcscpy(buf, strUTF);
        GlobalUnlock(hBuf);

        // Copy the data into the clipboard
        SetClipboardData(CF_UNICODETEXT, hBuf);

        // Close the clipboard
        CloseClipboard();
    }
}

SString SharedUtil::GetClipboardText()
{
    SString data;

    if (OpenClipboard(NULL))
    {
        // Get the clipboard's data
        HANDLE clipboardData = GetClipboardData(CF_UNICODETEXT);
        void*  lockedData = GlobalLock(clipboardData);
        if (lockedData)
            data = UTF16ToMbUTF8(static_cast<wchar_t*>(lockedData));

        GlobalUnlock(clipboardData);
        CloseClipboard();
    }

    return data;
}

//
// Direct players to info about trouble
//
void SharedUtil::BrowseToSolution(const SString& strType, int iFlags, const SString& strMessageBoxMessage, const SString& strErrorCode)
{
    AddReportLog(3200, SString("Trouble %s", *strType));

    // Put args into a string and save in the registry
    CArgMap argMap;
    argMap.Set("type", strType.SplitLeft(";"));
    argMap.Set("flags", iFlags);
    argMap.Set("message", strMessageBoxMessage);
    argMap.Set("ecode", strErrorCode);
    SetApplicationSetting("pending-browse-to-solution", argMap.ToString());

    if (iFlags & EXIT_GAME_FIRST)
    {
        // Exit game and continue in loader.dll
        TerminateProcess(GetCurrentProcess(), 1);
    }

    // Otherwise, do it now
    bool bDidBrowse = ProcessPendingBrowseToSolution();

    // Exit game if required
    int iFlagMatch = bDidBrowse ? TERMINATE_IF_YES : TERMINATE_IF_NO;
    if (iFlags & iFlagMatch)
        TerminateProcess(GetCurrentProcess(), 1);
}

//
// Process next BrowseToSolution
// Return true if did browse
//
bool SharedUtil::ProcessPendingBrowseToSolution()
{
    SString strType, strMessageBoxMessage, strErrorCode;
    int     iFlags;

    // Get args from the registry
    CArgMap argMap;
    argMap.SetFromString(GetApplicationSetting("pending-browse-to-solution"));
    if (!argMap.Get("type", strType))
        return false;
    argMap.Get("message", strMessageBoxMessage);
    argMap.Get("flags", iFlags);
    argMap.Get("ecode", strErrorCode);

    ClearPendingBrowseToSolution();

    SString strTitle("MTA: San Andreas %s   (CTRL+C to copy)", *strErrorCode);
    // Show message if set, ask question if required, and then launch URL
    if (iFlags & ASK_GO_ONLINE)
    {
        if (!strMessageBoxMessage.empty())
            strMessageBoxMessage += "\n\n\n";
        strMessageBoxMessage += _("Do you want to see some on-line help about this problem ?");
        if (IDYES != MessageBoxUTF8(NULL, strMessageBoxMessage, strTitle, MB_YESNO | MB_TOPMOST | (iFlags & ICON_MASK_VALUE)))
            return false;
    }
    else
    {
        if (!strMessageBoxMessage.empty())
            MessageBoxUTF8(NULL, strMessageBoxMessage, strTitle, MB_OK | MB_TOPMOST | (iFlags & ICON_MASK_VALUE));
        if (iFlags & SHOW_MESSAGE_ONLY)
            return true;
    }

    MessageBoxUTF8(
        NULL,
        _("Your browser will now display a web page with some help infomation.\n\n(If the page fails to load, paste (CTRL-V) the URL into your web browser)"),
        "MTA: San Andreas", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);

    SString strQueryURL = GetApplicationSetting("trouble-url");
    if (strQueryURL == "")
        strQueryURL = TROUBLE_URL1;
    strQueryURL = strQueryURL.Replace("%", "_");
    strQueryURL = strQueryURL.Replace("_VERSION_", GetApplicationSetting("mta-version-ext"));
    strQueryURL = strQueryURL.Replace("_ID_", GetApplicationSetting("serial"));
    strQueryURL = strQueryURL.Replace("_TROUBLE_", strType);
    SetClipboardText(strQueryURL);
    ShellExecuteNonBlocking("open", strQueryURL.c_str());

    return true;
}

//
// Clear BrowseToSolution
//
void SharedUtil::ClearPendingBrowseToSolution()
{
    SetApplicationSetting("pending-browse-to-solution", "");
}

//
// For tracking results of new features
//
static SString GetReportLogHeaderText()
{
    SString strMTABuild = GetApplicationSetting("mta-version-ext").SplitRight("-");
    SString strOSVersion = GetApplicationSetting("os-version");
    SString strRealOSVersion = GetApplicationSetting("real-os-version");
    SString strIsAdmin = GetApplicationSetting("is-admin");

    SString strResult = "[";
    if (strMTABuild.length())
        strResult += strMTABuild + " ";
    if (strOSVersion.length())
    {
        if (strOSVersion == strRealOSVersion)
            strResult += strOSVersion + " ";
        else
            strResult += strOSVersion + "(" + strRealOSVersion + ") ";
    }
    if (strIsAdmin == "1")
        strResult += "a";

    return strResult.TrimEnd(" ") + "]";
}

void SharedUtil::AddReportLog(uint uiId, const SString& strText, uint uiAmountLimit)
{
    if (uiAmountLimit)
    {
        uint& uiAmount = MapGet(ms_ReportAmountMap, uiId);
        if (uiAmount++ >= uiAmountLimit)
            return;
    }

    ms_ReportLineFilter.AddLine({strText, uiId});

    SReportLine line;
    while (ms_ReportLineFilter.PopOutputLine(line))
    {
        const SString& strText = line.strText;
        uint           uiId = line.uiId;

        SString strPathFilename = PathJoin(GetMTADataPath(), "report.log");
        MakeSureDirExists(strPathFilename);

        SString strMessage("%u: %s %s [%s] - %s\n", uiId, GetTimeString(true, false).c_str(), GetReportLogHeaderText().c_str(),
                           GetReportLogProcessTag().c_str(), strText.c_str());
        FileAppend(strPathFilename, &strMessage.at(0), strMessage.length());
        OutputDebugLine(SStringX("[ReportLog] ") + strMessage);
    }
}

//
// Track results of new features by avoiding heap memory allocations
// Avoid using this function if you have plenty of memory available
//
void SharedUtil::AddExceptionReportLog(uint uiId, const char* szExceptionName, const char* szExceptionText)
{
    constexpr size_t BOILERPLATE_SIZE = 46;
    constexpr size_t MAX_EXCEPTION_NAME_SIZE = 64;
    constexpr size_t MAX_EXCEPTION_TEXT_SIZE = 256;
    static char      szOutput[BOILERPLATE_SIZE + MAX_EXCEPTION_NAME_SIZE + MAX_EXCEPTION_TEXT_SIZE] = {0};

    SYSTEMTIME s = {0};
    GetSystemTime(&s);

    sprintf_s(szOutput, "%u: %04hu-%02hu-%02hu %02hu:%02hu:%02hu - Caught %.*s exception: %.*s\n", uiId, s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute,
              s.wSecond, MAX_EXCEPTION_NAME_SIZE, szExceptionName, MAX_EXCEPTION_TEXT_SIZE, szExceptionText);

    OutputDebugString("[ReportLog] ");
    OutputDebugString(&szOutput[0]);

    try
    {
        SString strMessage = szOutput;
        SString strPathFilename = PathJoin(GetMTADataPath(), "report.log");
        MakeSureDirExists(strPathFilename);
        FileAppend(strPathFilename, &strMessage.at(0), strMessage.length());
    }
    catch (std::bad_alloc&)
    {
        // At the time of writing this function, it is only called from other 'try-catch' blocks
        // so we can assume we literally ran out of all available memory here and ignore the exception
    }
}

void SharedUtil::SetReportLogContents(const SString& strText)
{
    SString strPathFilename = PathJoin(GetMTADataPath(), "report.log");
    MakeSureDirExists(strPathFilename);
    FileSave(strPathFilename, strText.length() ? &strText.at(0) : NULL, strText.length());
}

SString SharedUtil::GetReportLogContents()
{
    SString strReportFilename = PathJoin(GetMTADataPath(), "report.log");
    // Load file into a string
    std::vector<char> buffer;
    FileLoad(strReportFilename, buffer);
    buffer.push_back(0);
    return &buffer[0];
}

SString SharedUtil::GetReportLogProcessTag()
{
    static SString strResult;
    if (strResult.empty())
    {
        int pid = GetCurrentProcessId();
        if (!IsGTAProcess())
        {
            // Use pid only for launcher
            strResult = SString("%05d", pid);
        }
        else
        {
            // Use pid & parent pid for game
            int             parentPid = 0;
            HANDLE          h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            PROCESSENTRY32W pe = {0};
            pe.dwSize = sizeof(PROCESSENTRY32W);
            if (Process32FirstW(h, &pe))
            {
                do
                {
                    if (pe.th32ProcessID == pid)
                    {
                        parentPid = pe.th32ParentProcessID;
                        break;
                    }
                } while (Process32NextW(h, &pe));
            }
            CloseHandle(h);
            strResult = SString("%05d-%05d", parentPid, pid);
        }
    }
    return strResult;
}

// Client logfile.txt
void WriteEvent(const char* szType, const SString& strText)
{
    // Split into lines if required
    if (strText.Contains("\n"))
    {
        std::vector<SString> lineList;
        strText.Split("\n", lineList);
        for (uint i = 0; i < lineList.size(); i++)
            WriteEvent(szType, lineList[i]);
        return;
    }
    SString strPathFilename = CalcMTASAPath(PathJoin("mta", "logs", "logfile.txt"));
    SString strMessage("%s - %s %s", *GetLocalTimeString(), szType, *strText);
    FileAppend(strPathFilename, strMessage + "\n");
#ifdef MTA_DEBUG
    OutputDebugLine(strMessage);
#endif
}

void SharedUtil::WriteDebugEvent(const SString& strText)
{
    WriteEvent("[DEBUG]", strText);
}

void SharedUtil::WriteErrorEvent(const SString& strText)
{
    WriteEvent("[Error]", strText);
}

void SharedUtil::BeginEventLog()
{
    // Cycle now if flag requires it
    if (GetApplicationSettingInt("no-cycle-event-log") == 0)
    {
        SetApplicationSettingInt("no-cycle-event-log", 1);
        CycleFile(CalcMTASAPath(PathJoin("mta", "logs", "logfile.txt")), 1);
    }
    WriteDebugEvent("BeginEventLog");
}

void SharedUtil::CycleEventLog()
{
    // Set flag to cycle on next start
    SetApplicationSettingInt("no-cycle-event-log", 0);
}

///////////////////////////////////////////////////////////////////////////
//
// SharedUtil::GetSystemErrorMessage
//
// Get Windows error message text from a last error code.
//
///////////////////////////////////////////////////////////////////////////
SString SharedUtil::GetSystemErrorMessage(uint uiError, bool bRemoveNewlines, bool bPrependCode)
{
    SString strResult;

    LPWSTR szErrorText = NULL;
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, uiError,
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&szErrorText, 0, NULL);

    if (szErrorText)
    {
        strResult = UTF16ToMbUTF8(szErrorText);
        LocalFree(szErrorText);
        szErrorText = NULL;
    }

    if (bRemoveNewlines)
        strResult = strResult.Replace("\n", "").Replace("\r", "");

    if (bPrependCode)
        strResult = SString("Error %u: %s", uiError, *strResult);

    return strResult;
}

#ifdef ExpandEnvironmentStringsForUser
//
// eg "%HOMEDRIVE%" -> "C:"
//
SString SharedUtil::ExpandEnvString(const SString& strInput)
{
    HANDLE hProcessToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE, &hProcessToken))
        return strInput;

    const static int iBufferSize = 32000;
    char             envBuf[iBufferSize + 2];
    ExpandEnvironmentStringsForUser(hProcessToken, strInput, envBuf, iBufferSize);
    return envBuf;
}
#endif

///////////////////////////////////////////////////////////////
//
// MyShellExecute
//
// Returns true if successful
//
///////////////////////////////////////////////////////////////
static bool MyShellExecute(bool bBlocking, const SString& strAction, const SString& strInFile, const SString& strInParameters = "",
                           const SString& strDirectory = "", int nShowCmd = SW_SHOWNORMAL)
{
    SString strFile = strInFile;
    SString strParameters = strInParameters;

    if (strAction == "open" && (strFile.BeginsWithI("http://") || strFile.BeginsWithI("https://")) && strParameters.empty())
    {
        strParameters = "url.dll,FileProtocolHandler " + strFile;
        strFile = "rundll32.exe";
    }

    if (bBlocking)
    {
        WString           wstrAction = FromUTF8(strAction);
        WString           wstrFile = FromUTF8(strFile);
        WString           wstrParameters = FromUTF8(strParameters);
        WString           wstrDirectory = FromUTF8(strDirectory);
        SHELLEXECUTEINFOW info;
        memset(&info, 0, sizeof(info));
        info.cbSize = sizeof(info);
        info.fMask = SEE_MASK_NOCLOSEPROCESS;
        info.lpVerb = wstrAction;
        info.lpFile = wstrFile;
        info.lpParameters = wstrParameters;
        info.lpDirectory = wstrDirectory;
        info.nShow = nShowCmd;
        bool bResult = ShellExecuteExW(&info) != FALSE;
        if (info.hProcess)
        {
            WaitForSingleObject(info.hProcess, INFINITE);
            CloseHandle(info.hProcess);
        }
        return bResult;
    }
    else
    {
        int iResult = (int)ShellExecute(NULL, strAction, strFile, strParameters, strDirectory, nShowCmd);
        return iResult > 32;
    }
}

///////////////////////////////////////////////////////////////
//
// SharedUtil::ShellExecuteBlocking
//
//
//
///////////////////////////////////////////////////////////////
bool SharedUtil::ShellExecuteBlocking(const SString& strAction, const SString& strFile, const SString& strParameters, const SString& strDirectory, int nShowCmd)
{
    return MyShellExecute(true, strAction, strFile, strParameters, strDirectory);
}

///////////////////////////////////////////////////////////////
//
// SharedUtil::ShellExecuteNonBlocking
//
//
//
///////////////////////////////////////////////////////////////
bool SharedUtil::ShellExecuteNonBlocking(const SString& strAction, const SString& strFile, const SString& strParameters, const SString& strDirectory,
                                         int nShowCmd)
{
    return MyShellExecute(false, strAction, strFile, strParameters, strDirectory);
}

#endif  // MTA_CLIENT

#ifdef WIN32
#define _WIN32_WINNT_WIN8                   0x0602
///////////////////////////////////////////////////////////////////////////
//
// SharedUtil::IsWindowsVersionOrGreater
//
// From Windows Kits\8.1 VersionHelpers.h
// (Ignores compatibility mode)
//
///////////////////////////////////////////////////////////////////////////
bool SharedUtil::IsWindowsVersionOrGreater(WORD wMajorVersion, WORD wMinorVersion, WORD wServicePackMajor)
{
    OSVERSIONINFOEXW osvi = {sizeof(osvi), 0, 0, 0, 0, {0}, 0, 0};
    DWORDLONG const  dwlConditionMask =
        VerSetConditionMask(VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL),
                            VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL);

    osvi.dwMajorVersion = wMajorVersion;
    osvi.dwMinorVersion = wMinorVersion;
    osvi.wServicePackMajor = wServicePackMajor;

    return VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, dwlConditionMask) != FALSE;
}

bool SharedUtil::IsWindowsXPSP3OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINXP), LOBYTE(_WIN32_WINNT_WINXP), 3);
}

bool SharedUtil::IsWindowsVistaOrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_VISTA), LOBYTE(_WIN32_WINNT_VISTA), 0);
}

bool SharedUtil::IsWindows7OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0);
}

bool SharedUtil::IsWindows8OrGreater()
{
    return IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0);
}
#endif  // WIN32

static uchar ToHexChar(uchar c)
{
    return c > 9 ? c - 10 + 'A' : c + '0';
}

static uchar FromHexChar(uchar c)
{
    return c > '9' ? c - 'A' + 10 : c - '0';
}

SString SharedUtil::EscapeString(const SString& strText, const SString& strDisallowedChars, char cSpecialChar, uchar ucLowerLimit, uchar ucUpperLimit)
{
    // Replace each disallowed char with #FF
    SString strResult;
    for (uint i = 0; i < strText.length(); i++)
    {
        uchar c = strText[i];
        if (strDisallowedChars.find(c) == std::string::npos && c != cSpecialChar && c >= ucLowerLimit && c <= ucUpperLimit)
            strResult += c;
        else
        {
            strResult += cSpecialChar;
            strResult += ToHexChar(c >> 4);
            strResult += ToHexChar(c & 0x0f);
        }
    }
    return strResult;
}

SString SharedUtil::UnescapeString(const SString& strText, char cSpecialChar)
{
    SString strResult;
    // Replace #FF with char
    for (uint i = 0; i < strText.length(); i++)
    {
        uchar c = strText[i];
        if (c == cSpecialChar && i < strText.length() - 2)
        {
            uchar c0 = FromHexChar(strText[++i]);
            uchar c1 = FromHexChar(strText[++i]);
            c = (c0 << 4) | c1;
        }
        strResult += c;
    }
    return strResult;
}

//
// Ensure string does not contain any special URL chars
//
SString SharedUtil::EscapeURLArgument(const SString& strArg)
{
    static SString strDisallowedChars = "!*'();:@&=+$,/?#[] \"%<>\\^`{|}";
    return EscapeString(strArg, strDisallowedChars, '%', 32, 126);
}

//
// Cross platform critical section
//
#ifdef WIN32

SharedUtil::CCriticalSection::CCriticalSection()
{
    m_pCriticalSection = new CRITICAL_SECTION;
    InitializeCriticalSection((CRITICAL_SECTION*)m_pCriticalSection);
}

SharedUtil::CCriticalSection::~CCriticalSection()
{
    DeleteCriticalSection((CRITICAL_SECTION*)m_pCriticalSection);
    delete (CRITICAL_SECTION*)m_pCriticalSection;
}

void SharedUtil::CCriticalSection::Lock()
{
    if (m_pCriticalSection)
        EnterCriticalSection((CRITICAL_SECTION*)m_pCriticalSection);
}

void SharedUtil::CCriticalSection::Unlock()
{
    if (m_pCriticalSection)
        LeaveCriticalSection((CRITICAL_SECTION*)m_pCriticalSection);
}

#else
    #include <pthread.h>

SharedUtil::CCriticalSection::CCriticalSection()
{
    m_pCriticalSection = new pthread_mutex_t;
    pthread_mutex_init((pthread_mutex_t*)m_pCriticalSection, NULL);
}

SharedUtil::CCriticalSection::~CCriticalSection()
{
    pthread_mutex_destroy((pthread_mutex_t*)m_pCriticalSection);
    delete (pthread_mutex_t*)m_pCriticalSection;
}

void SharedUtil::CCriticalSection::Lock()
{
    pthread_mutex_lock((pthread_mutex_t*)m_pCriticalSection);
}

void SharedUtil::CCriticalSection::Unlock()
{
    pthread_mutex_unlock((pthread_mutex_t*)m_pCriticalSection);
}

#endif

//
// Ensure rand() seed gets set to a new unique value
//
void SharedUtil::RandomizeRandomSeed()
{
    srand(rand() + GetTickCount32());
}

#ifdef WIN32
static LONG SafeNtQueryInformationThread(HANDLE ThreadHandle, INT ThreadInformationClass, PVOID ThreadInformation, ULONG ThreadInformationLength,
                                         PULONG ReturnLength)
{
    using FunctionPointer = LONG(__stdcall*)(HANDLE, INT /*= THREADINFOCLASS*/, PVOID, ULONG, PULONG);

    struct FunctionLookup
    {
        FunctionPointer function;
        bool            once;
    };

    static FunctionLookup lookup = {};

    if (!lookup.once)
    {
        lookup.once = true;

        HMODULE ntdll = LoadLibraryA("ntdll.dll");

        if (ntdll)
            lookup.function = static_cast<FunctionPointer>(static_cast<void*>(GetProcAddress(ntdll, "NtQueryInformationThread")));
        else
            return 0xC0000135L;            // STATUS_DLL_NOT_FOUND
    }

    if (lookup.function)
        return lookup.function(ThreadHandle, ThreadInformationClass, ThreadInformation, ThreadInformationLength, ReturnLength);
    else
        return 0xC00000BBL;            // STATUS_NOT_SUPPORTED
}

bool SharedUtil::QueryThreadEntryPointAddress(void* thread, DWORD* entryPointAddress)
{
    return SafeNtQueryInformationThread(thread, 9 /*ThreadQuerySetWin32StartAddress*/, entryPointAddress, sizeof(DWORD), nullptr) == 0;
}

DWORD SharedUtil::GetMainThreadId()
{
    static DWORD dwMainThreadID = 0;

    if (dwMainThreadID == 0)
    {
        // Get the module information for the currently running process
        DWORD      processEntryPointAddress = 0;
        MODULEINFO moduleInfo = {};

        if (GetModuleInformation(GetCurrentProcess(), GetModuleHandle(nullptr), &moduleInfo, sizeof(MODULEINFO)) != 0)
        {
            processEntryPointAddress = reinterpret_cast<DWORD>(moduleInfo.EntryPoint);
        }

        // Find oldest thread in the current process ( http://www.codeproject.com/Questions/78801/How-to-get-the-main-thread-ID-of-a-process-known-b )
        HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

        if (hThreadSnap != INVALID_HANDLE_VALUE)
        {
            ULONGLONG ullMinCreateTime = ULLONG_MAX;

            DWORD currentProcessID = GetCurrentProcessId();

            THREADENTRY32 th32 = {};
            th32.dwSize = sizeof(THREADENTRY32);

            for (BOOL bOK = Thread32First(hThreadSnap, &th32); bOK; bOK = Thread32Next(hThreadSnap, &th32))
            {
                if (th32.th32OwnerProcessID == currentProcessID)
                {
                    HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, TRUE, th32.th32ThreadID);

                    if (hThread)
                    {
                        // Check the thread by entry point first
                        if (processEntryPointAddress != 0)
                        {
                            DWORD entryPointAddress = 0;

                            if (QueryThreadEntryPointAddress(hThread, &entryPointAddress) && entryPointAddress == processEntryPointAddress)
                            {
                                dwMainThreadID = th32.th32ThreadID;
                                CloseHandle(hThread);
                                CloseHandle(hThreadSnap);
                                return dwMainThreadID;
                            }
                        }

                        // If entry point check failed, find the oldest thread in the system
                        FILETIME afTimes[4] = {};

                        if (GetThreadTimes(hThread, &afTimes[0], &afTimes[1], &afTimes[2], &afTimes[3]))
                        {
                            ULONGLONG ullTest = (ULONGLONG(afTimes[0].dwHighDateTime) << 32) + afTimes[0].dwLowDateTime;

                            if (ullTest && ullTest < ullMinCreateTime)
                            {
                                ullMinCreateTime = ullTest;
                                dwMainThreadID = th32.th32ThreadID;
                            }
                        }

                        CloseHandle(hThread);
                    }
                }
            }

            CloseHandle(hThreadSnap);
        }

        // Fallback
        if (dwMainThreadID == 0)
        {
            dwMainThreadID = GetCurrentThreadId();
        }
    }

    return dwMainThreadID;
}
#endif

//
// Return true if currently executing the main thread.
// (Linux: Main thread being defined as the thread the function is first called from.)
//
bool SharedUtil::IsMainThread()
{
#ifdef WIN32
    DWORD mainThreadID = GetMainThreadId();
    DWORD currentThreadID = GetCurrentThreadId();
    return mainThreadID == currentThreadID;
#else
    static pthread_t dwMainThread = pthread_self();
    return pthread_equal(pthread_self(), dwMainThread) != 0;
#endif
}

//
// Expiry stuff
//
#ifdef WIN32
    #include <time.h>

int SharedUtil::GetBuildAge()
{
    tm when;
    memset(&when, 0, sizeof(when));
    when.tm_year = BUILD_YEAR - 1900;
    when.tm_mon = BUILD_MONTH;
    when.tm_mday = BUILD_DAY;
    return (int)(time(NULL) - mktime(&when)) / (60 * 60 * 24);
}

#if defined(MTA_DM_EXPIRE_DAYS)
int SharedUtil::GetDaysUntilExpire()
{
    tm when;
    memset(&when, 0, sizeof(when));
    when.tm_year = BUILD_YEAR - 1900;
    when.tm_mon = BUILD_MONTH;
    when.tm_mday = BUILD_DAY + MTA_DM_EXPIRE_DAYS;
    return (int)(mktime(&when) - time(NULL)) / (60 * 60 * 24);
}

#endif
#endif

//
// Return version of the string with color codes removed
//
SString SharedUtil::RemoveColorCodes(const char* szString)
{
    SString     strOut;
    const char* szStart = szString;
    const char* szEnd = szString;

    while (true)
    {
        if (*szEnd == '\0')
        {
            strOut.append(szStart, szEnd - szStart);
            break;
        }
        else if (IsColorCode(szEnd))
        {
            strOut.append(szStart, szEnd - szStart);
            szStart = szEnd + 7;
            szEnd = szStart;
        }
        else
        {
            szEnd++;
        }
    }

    return strOut;
}

//
// Replace color codes in supplied wide string
//
void SharedUtil::RemoveColorCodesInPlaceW(WString& strText)
{
    uint uiSearchPos = 0;
    while (true)
    {
        std::wstring::size_type uiFoundPos = strText.find(L'#', uiSearchPos);
        if (uiFoundPos == std::wstring::npos)
            break;

        if (IsColorCodeW(strText.c_str() + uiFoundPos))
        {
            strText = strText.SubStr(0, uiFoundPos) + strText.SubStr(uiFoundPos + 7);
        }
        else
        {
            uiSearchPos = uiFoundPos + 1;
        }
    }
}

//
// Returns true if string is a color code
//
bool SharedUtil::IsColorCode(const char* szColorCode)
{
    if (*szColorCode != '#')
        return false;

    bool bValid = true;
    for (int i = 0; i < 6; i++)
    {
        char c = szColorCode[1 + i];
        if (!isdigit((unsigned char)c) && (c < 'A' || c > 'F') && (c < 'a' || c > 'f'))
        {
            bValid = false;
            break;
        }
    }
    return bValid;
}

//
// Returns true if wide string is a color code
//
bool SharedUtil::IsColorCodeW(const wchar_t* wszColorCode)
{
    if (*wszColorCode != L'#')
        return false;

    for (uint i = 0; i < 6; i++)
    {
        wchar_t c = wszColorCode[i + 1];
        if (!iswdigit(c) && (c < 'A' || c > 'F') && (c < 'a' || c > 'f'))
        {
            return false;
        }
    }
    return true;
}

char* SharedUtil::Trim(char* szText)
{
    char*  szOriginal = szText;
    size_t uiLen = 0;

    while (isspace((unsigned char)*szText))
        szText++;

    if (*szText)
    {
        char* p = szText;
        while (*p)
            p++;
        while (isspace((unsigned char)*(--p)))
            ;
        p[1] = '\0';
        uiLen = (size_t)(p - szText + 1);
    }

    if (szText == szOriginal)
        return szText;

    return static_cast<char*>(memmove(szOriginal, szText, uiLen + 1));
}

// Convert a standard multibyte UTF-8 std::string into a UTF-16 std::wstring
std::wstring SharedUtil::MbUTF8ToUTF16(const SString& input)
{
    return utf8_mbstowcs(input);
}

// Convert a UTF-16 std::wstring into a multibyte UTF-8 string
std::string SharedUtil::UTF16ToMbUTF8(const std::wstring& input)
{
    return utf8_wcstombs(input);
}

std::string SharedUtil::UTF16ToMbUTF8(const wchar_t* input)
{
    if (input == nullptr)
        return "";
    return utf8_wcstombs(input);
}

std::string SharedUtil::UTF16ToMbUTF8(const char16_t* input)
{
    return UTF16ToMbUTF8((const wchar_t*)input);
}

// Get UTF8 confidence
int SharedUtil::GetUTF8Confidence(const unsigned char* input, int len)
{
    return icu_getUTF8Confidence(input, len);
}

// Translate a true ANSI string to the UTF-16 equivalent (reencode+convert)
std::wstring SharedUtil::ANSIToUTF16(const SString& input)
{
    size_t len = mbstowcs(NULL, input.c_str(), input.length());
    if (len == (size_t)-1)
        return L"?";
    wchar_t* wcsOutput = new wchar_t[len + 1];
    mbstowcs(wcsOutput, input.c_str(), input.length());
    wcsOutput[len] = 0;            // Null terminate the string
    std::wstring strOutput(wcsOutput);
    delete[] wcsOutput;
    return strOutput;
}

// Check for BOM bytes
bool SharedUtil::IsUTF8BOM(const void* pData, uint uiLength)
{
    const uchar* pCharData = (const uchar*)pData;
    return (uiLength > 2 && pCharData[0] == 0xEF && pCharData[1] == 0xBB && pCharData[2] == 0xBF);
}

// Check for UTF8/ANSI compiled script
bool SharedUtil::IsLuaCompiledScript(const void* pData, uint uiLength)
{
    const uchar* pCharData = (const uchar*)pData;
    if (IsUTF8BOM(pCharData, uiLength))
    {
        pCharData += 3;
        uiLength -= 3;
    }
    return (uiLength > 0 && pCharData[0] == 0x1B);            // Do the same check as what the Lua parser does
}

// Check for obfuscated script
bool SharedUtil::IsLuaObfuscatedScript(const void* pData, uint uiLength)
{
    const uchar* pCharData = (const uchar*)pData;
    return (uiLength > 0 && pCharData[0] == 0x1C);            // Look for our special marker
}

//
// Return true if supplied version string will sort correctly
//
bool SharedUtil::IsValidVersionString(const SString& strVersion)
{
    const SString strCheck = "0.0.0-0.00000.0.000";
    uint          uiLength = std::min(strCheck.length(), strVersion.length());
    for (unsigned int i = 0; i < uiLength; i++)
    {
        uchar c = strVersion[i];
        uchar d = strCheck[i];
        if (!isdigit(c) || !isdigit(d))
            if (c != d)
                return false;
    }
    return true;
}

// Return build number as a 5 character sortable string
SString SharedUtil::ExtractVersionStringBuildNumber(const SString& strVersion)
{
    return strVersion.SubStr(8, 5);
}

//
// Try to make a path relative to the 'resources/' directory
//
SString SharedUtil::ConformResourcePath(const char* szRes, bool bConvertToUnixPathSep)
{
    // Remove up to first '/resources/'
    // else
    // remove up to first '/resource-cache/unzipped/'
    // else
    // remove up to first '/http-client-no-client-cache-files/'
    // else
    // remove up to first '/deathmatch/'
    // else
    // if starts with '...'
    //  remove up to first '/'

    SString strDelimList[] = {"/resources/", "/resource-cache/unzipped/", "/http-client-files-no-client-cache/", "/deathmatch/"};
    SString strText = szRes ? szRes : "";
    char    cPathSep;

    // Handle which path sep char
#ifdef WIN32
    if (!bConvertToUnixPathSep)
    {
        cPathSep = '\\';
        for (unsigned int i = 0; i < NUMELMS(strDelimList); i++)
            strDelimList[i] = strDelimList[i].Replace("/", "\\");
        strText = strText.Replace("/", "\\");
    }
    else
#endif
    {
        cPathSep = '/';
        for (unsigned int i = 0; i < NUMELMS(strDelimList); i++)
            strDelimList[i] = strDelimList[i].Replace("\\", "/");
        strText = strText.Replace("\\", "/");
    }

    for (unsigned int i = 0; i < NUMELMS(strDelimList); i++)
    {
        // Remove up to first occurrence
        int iPos = strText.find(strDelimList[i]);
        if (iPos >= 0)
            return strText.substr(iPos + strDelimList[i].length());
    }

    if (strText.substr(0, 3) == "...")
    {
        // Remove up to first '/'
        int iPos = strText.find(cPathSep);
        if (iPos >= 0)
            return strText.substr(iPos + 1);
    }

    return strText;
}

namespace SharedUtil
{
    CArgMap::CArgMap(const SString& strArgSep, const SString& strPartsSep, const SString& strExtraDisallowedChars)
        : m_strArgSep(strArgSep), m_strPartsSep(strPartsSep)
    {
        m_strDisallowedChars = strExtraDisallowedChars + m_strArgSep + m_strPartsSep;
        m_cEscapeCharacter = '#';
    }

    void CArgMap::SetEscapeCharacter(char cEscapeCharacter) { m_cEscapeCharacter = cEscapeCharacter; }

    void CArgMap::Merge(const CArgMap& other, bool bAllowMultiValues) { MergeFromString(other.ToString(), bAllowMultiValues); }

    void CArgMap::SetFromString(const SString& strLine, bool bAllowMultiValues)
    {
        m_Map.clear();
        MergeFromString(strLine, bAllowMultiValues);
    }

    void CArgMap::MergeFromString(const SString& strLine, bool bAllowMultiValues)
    {
        std::vector<SString> parts;
        strLine.Split(m_strPartsSep, parts);
        for (uint i = 0; i < parts.size(); i++)
        {
            SString strCmd, strArg;
            parts[i].Split(m_strArgSep, &strCmd, &strArg);
            if (!bAllowMultiValues)
                m_Map.erase(strCmd);
            if (strCmd.length())            // Key can not be empty
                MapInsert(m_Map, strCmd, strArg);
        }
    }

    SString CArgMap::ToString() const
    {
        SString strResult;
        for (std::multimap<SString, SString>::const_iterator iter = m_Map.begin(); iter != m_Map.end(); ++iter)
        {
            if (strResult.length())
                strResult += m_strPartsSep;
            strResult += iter->first + m_strArgSep + iter->second;
        }
        return strResult;
    }

    bool CArgMap::HasMultiValues() const
    {
        for (std::multimap<SString, SString>::const_iterator iter = m_Map.begin(); iter != m_Map.end(); ++iter)
        {
            std::vector<SString> newItems;
            MultiFind(m_Map, iter->first, &newItems);
            if (newItems.size() > 1)
                return true;
        }
        return false;
    }

    void CArgMap::RemoveMultiValues()
    {
        if (HasMultiValues())
            SetFromString(ToString(), false);
    }

    SString CArgMap::Escape(const SString& strIn) const { return EscapeString(strIn, m_strDisallowedChars, m_cEscapeCharacter); }

    SString CArgMap::Unescape(const SString& strIn) const { return UnescapeString(strIn, m_cEscapeCharacter); }

    // Set a unique key string value
    void CArgMap::Set(const SString& strCmd, const SString& strValue)
    {
        m_Map.erase(Escape(strCmd));
        Insert(strCmd, strValue);
    }

    // Set a unique key int value
    void CArgMap::Set(const SString& strCmd, int iValue)
    {
        m_Map.erase(Escape(strCmd));
        Insert(strCmd, iValue);
    }

    // Insert a key int value
    void CArgMap::Insert(const SString& strCmd, int iValue) { Insert(strCmd, SString("%d", iValue)); }

    // Insert a key string value
    void CArgMap::Insert(const SString& strCmd, const SString& strValue)
    {
        if (strCmd.length())            // Key can not be empty
            MapInsert(m_Map, Escape(strCmd), Escape(strValue));
    }

    // Test if key exists
    bool CArgMap::Contains(const SString& strCmd) const { return MapFind(m_Map, Escape(strCmd)) != NULL; }

    // First result as string
    bool CArgMap::Get(const SString& strCmd, SString& strOut, const char* szDefault) const
    {
        assert(szDefault);
        if (const SString* pResult = MapFind(m_Map, Escape(strCmd)))
        {
            strOut = Unescape(*pResult);
            return true;
        }
        strOut = szDefault;
        return false;
    }

    // First result as string
    SString CArgMap::Get(const SString& strCmd) const
    {
        SString strResult;
        Get(strCmd, strResult);
        return strResult;
    }

    // All results as strings
    bool CArgMap::Get(const SString& strCmd, std::vector<SString>& outList) const
    {
        std::vector<SString> newItems;
        MultiFind(m_Map, Escape(strCmd), &newItems);
        for (uint i = 0; i < newItems.size(); i++)
            newItems[i] = Unescape(newItems[i]);
        ListAppend(outList, newItems);
        return newItems.size() > 0;
    }

    // First result as int
    bool CArgMap::Get(const SString& strCmd, int& iValue, int iDefault) const
    {
        SString strResult;
        if (Get(strCmd, strResult))
        {
            iValue = atoi(strResult);
            return true;
        }
        iValue = iDefault;
        return false;
    }

    // All keys
    void CArgMap::GetKeys(std::vector<SString>& outList) const
    {
        for (std::multimap<SString, SString>::const_iterator iter = m_Map.begin(); iter != m_Map.end(); ++iter)
            outList.push_back(iter->first);
    }

#ifdef WIN32
    ///////////////////////////////////////////////////////////////////////////
    //
    // GetCurrentProcessorNumberXP for the current thread, especially for Windows XP
    //
    // Only a guide as it could change after the call has returned
    //
    ///////////////////////////////////////////////////////////////////////////
    DWORD _GetCurrentProcessorNumberXP()
    {
    #ifdef WIN_x64
        return 0;
    #elif defined(WIN_arm) || defined(WIN_arm64)
        return 0;
    #else
        _asm
        {
            mov eax, 1
            cpuid
            shr ebx, 24
            mov eax, ebx
        }
    #endif
    }

#endif

    ///////////////////////////////////////////////////////////////////////////
    //
    // GetCurrentProcessorNumber for the current thread.
    //
    // Only a guide as it could change after the call has returned
    //
    ///////////////////////////////////////////////////////////////////////////
    DWORD _GetCurrentProcessorNumber()
    {
#ifdef WIN32
        // Dynamically load GetCurrentProcessorNumber, as it does not exist on XP
        using GetCurrentProcessorNumber_t = DWORD(WINAPI*)();

        static auto FnGetCurrentProcessorNumber = ([]() -> GetCurrentProcessorNumber_t {
            HMODULE kernel32 = LoadLibraryA("kernel32");

            if (kernel32)
                return static_cast<GetCurrentProcessorNumber_t>(static_cast<void*>(GetProcAddress(kernel32, "GetCurrentProcessorNumber")));

            return nullptr;
        })();

        if (FnGetCurrentProcessorNumber)
            return FnGetCurrentProcessorNumber();

        return _GetCurrentProcessorNumberXP();
#elif defined(__APPLE__) && defined(__aarch64__)
        return -1;
#elif defined(__APPLE__)
        // Hacked from https://stackoverflow.com/a/40398183/1517394
        unsigned long cpu;

        uint32_t CPUInfo[4];
        __cpuid_count(1, 0, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);

        /* CPUInfo[1] is EBX, bits 24-31 are APIC ID */
        if ((CPUInfo[3] & (1 << 9)) == 0)
            cpu = -1; /* no APIC on chip */
        else
            cpu = (unsigned)CPUInfo[1] >> 24;

        if (cpu < 0)
            cpu = 0;

        return cpu;
#else
        // This should work on Linux
        return sched_getcpu();
#endif
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    // GetThreadCPUTimes
    //
    // Returns time in microseconds used by this thread
    //
    ///////////////////////////////////////////////////////////////////////////
    void GetThreadCPUTimes(uint64& outUserTime, uint64& outKernelTime)
    {
        outUserTime = 0;
        outKernelTime = 0;
#ifdef WIN32
        FILETIME CreationTime, ExitTime, KernelTime, UserTime;
        if (GetThreadTimes(GetCurrentThread(), &CreationTime, &ExitTime, &KernelTime, &UserTime))
        {
            ((ULARGE_INTEGER*)&outUserTime)->LowPart = UserTime.dwLowDateTime;
            ((ULARGE_INTEGER*)&outUserTime)->HighPart = UserTime.dwHighDateTime;
            ((ULARGE_INTEGER*)&outKernelTime)->LowPart = KernelTime.dwLowDateTime;
            ((ULARGE_INTEGER*)&outKernelTime)->HighPart = KernelTime.dwHighDateTime;
            outUserTime /= 10;
            outKernelTime /= 10;
        }
#else
        rusage usage;
        if (getrusage(RUSAGE_THREAD, &usage) == 0)
        {
            outUserTime = (uint64)usage.ru_utime.tv_sec * 1000000ULL + (uint64)usage.ru_utime.tv_usec;
            outKernelTime = (uint64)usage.ru_stime.tv_sec * 1000000ULL + (uint64)usage.ru_stime.tv_usec;
        }
#endif
    }

    ///////////////////////////////////////////////////////////////////////////
    //
    // UpdateThreadCPUTimes
    //
    // Updates struct with datum
    //
    ///////////////////////////////////////////////////////////////////////////
    void UpdateThreadCPUTimes(SThreadCPUTimesStore& store, long long* pllTickCount)
    {
        // Use supplied tick count if present
        uint64 ullCPUMeasureTimeMs;
        if (pllTickCount)
            ullCPUMeasureTimeMs = *pllTickCount;
        else
            ullCPUMeasureTimeMs = GetTickCount64_();

        if (ullCPUMeasureTimeMs - store.ullPrevCPUMeasureTimeMs > 1000)
        {
            store.uiProcessorNumber = SharedUtil::_GetCurrentProcessorNumber();
            uint64 ullUserTimeUs, ullKernelTimeUs;
            GetThreadCPUTimes(ullUserTimeUs, ullKernelTimeUs);

            float fCPUMeasureTimeDeltaMs = (float)(ullCPUMeasureTimeMs - store.ullPrevCPUMeasureTimeMs);
            float fUserTimeDeltaUs = (float)(ullUserTimeUs - store.ullPrevUserTimeUs);
            float fKernelTimeDeltaUs = (float)(ullKernelTimeUs - store.ullPrevKernelTimeUs);
            if (fCPUMeasureTimeDeltaMs > 0)
            {
                float fPercentScaler = 0.1f / fCPUMeasureTimeDeltaMs;
                store.fUserPercent = fUserTimeDeltaUs * fPercentScaler;
                store.fKernelPercent = fKernelTimeDeltaUs * fPercentScaler;
                store.fTotalCPUPercent = (fUserTimeDeltaUs + fKernelTimeDeltaUs) * fPercentScaler;
            }
            else
            {
                store.fUserPercent = 0;
                store.fKernelPercent = 0;
                store.fTotalCPUPercent = 0;
            }
            store.ullPrevUserTimeUs = ullUserTimeUs;
            store.ullPrevKernelTimeUs = ullKernelTimeUs;
            store.ullPrevCPUMeasureTimeMs = ullCPUMeasureTimeMs;

            // Updated smoothed values
            float fAlpha = 1.f / store.fAvgTimeSeconds;
            store.fUserPercentAvg = Lerp(store.fUserPercentAvg, fAlpha, store.fUserPercent);
            store.fKernelPercentAvg = Lerp(store.fKernelPercentAvg, fAlpha, store.fKernelPercent);
            store.fTotalCPUPercentAvg = Lerp(store.fTotalCPUPercentAvg, fAlpha, store.fTotalCPUPercent);
        }
    }

    //
    // class CRanges
    //
    //  Ranges of numbers. i.e.   100-4000, 5000-6999, 7000-7010
    //
    void CRanges::SetRange(uint uiStart, uint uiLength)
    {
        if (uiLength < 1)
            return;
        uint uiLast = uiStart + uiLength - 1;

        // Make a hole
        UnsetRange(uiStart, uiLength);

        // Insert
        m_StartLastMap[uiStart] = uiLast;

        // Maybe join adjacent ranges one day
    }

    void CRanges::UnsetRange(uint uiStart, uint uiLength)
    {
        if (uiLength < 1)
            return;
        uint uiLast = uiStart + uiLength - 1;

        RemoveObscuredRanges(uiStart, uiLast);

        IterType iterOverlap;
        if (GetRangeOverlappingPoint(uiStart, iterOverlap))
        {
            uint uiOverlapPrevLast = iterOverlap->second;

            // Modify overlapping range last point
            uint uiNewLast = uiStart - 1;
            iterOverlap->second = uiNewLast;

            if (uiOverlapPrevLast > uiLast)
            {
                // Need to add range after hole
                uint uiNewStart = uiLast + 1;
                m_StartLastMap[uiNewStart] = uiOverlapPrevLast;
            }
        }

        if (GetRangeOverlappingPoint(uiLast, iterOverlap))
        {
            // Modify overlapping range start point
            uint uiNewStart = uiLast + 1;
            uint uiOldLast = iterOverlap->second;
            m_StartLastMap.erase(iterOverlap);
            m_StartLastMap[uiNewStart] = uiOldLast;
        }
    }

    // Returns true if any part of the range already exists in the map
    bool CRanges::IsRangeSet(uint uiStart, uint uiLength)
    {
        if (uiLength < 1)
            return false;
        uint uiLast = uiStart + uiLength - 1;

        IterType iter = m_StartLastMap.lower_bound(uiStart);
        // iter is on or after start

        if (iter != m_StartLastMap.end())
        {
            // If start of found range is before or at query last, then range is used
            if (iter->first <= uiLast)
                return true;
        }

        if (iter != m_StartLastMap.begin())
        {
            iter--;
            // iter is now before start

            // If last of found range is after or at query start, then range is used
            if (iter->second >= uiStart)
                return true;
        }

        return false;
    }

    void CRanges::RemoveObscuredRanges(uint uiStart, uint uiLast)
    {
        while (true)
        {
            IterType iter = m_StartLastMap.lower_bound(uiStart);
            // iter is on or after start

            if (iter == m_StartLastMap.end())
                return;

            // If last of found range is after query last, then range is not obscured
            if (iter->second > uiLast)
                return;

            // Remove obscured
            m_StartLastMap.erase(iter);
        }
    }

    bool CRanges::GetRangeOverlappingPoint(uint uiPoint, IterType& result)
    {
        IterType iter = m_StartLastMap.lower_bound(uiPoint);
        // iter is on or after point - So it can't overlap the point

        if (iter != m_StartLastMap.begin())
        {
            iter--;
            // iter is now before point

            // If last of found range is after or at query point, then range is overlapping
            if (iter->second >= uiPoint)
            {
                result = iter;
                return true;
            }
        }
        return false;
    }

}            // namespace SharedUtil

//
// For checking MTA library module versions
//
MTAEXPORT void GetLibMtaVersion(char* pBuffer, uint uiMaxSize)
{
    SString strVersion("%d.%d.%d-%d.%05d.%d"
#ifdef MTASA_VERSION_MAJOR
                       ,
                       MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE, MTASA_VERSION_TYPE, MTASA_VERSION_BUILD
#else
                       ,
                       0, 0, 0, 0, 0
#endif
                       ,
                       0);
    uint uiLengthInclTerm = strVersion.length() + 1;
    STRNCPY(pBuffer, *strVersion, std::max(uiLengthInclTerm, uiMaxSize));
}
