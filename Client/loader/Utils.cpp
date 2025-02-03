/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Utils.cpp
 *  PURPOSE:     Loading utilities
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "Utils.h"
#include "Main.h"
#include "Dialogs.h"
#include <array>
#include <random>
#include <cryptopp/crc.h>
#include <cryptopp/files.h>
#include <tchar.h>
#include <strsafe.h>
#include <Tlhelp32.h>
#include <Softpub.h>
#include <wintrust.h>
#include <version.h>
#include <windows.h>
#pragma comment (lib, "wintrust")

namespace fs = std::filesystem;

static SString g_strMTASAPath;
static SString g_strGTAPath;
static HANDLE  g_hMutex = NULL;
static HMODULE hLibraryModule = NULL;
HINSTANCE      g_hInstance = NULL;

///////////////////////////////////////////////////////////////////////////
//
// devicePathToWin32Path
//
// Code from the merky depths of MSDN
//
///////////////////////////////////////////////////////////////////////////
WString devicePathToWin32Path(const WString& strDevicePath)
{
    WCHAR pszFilename[MAX_PATH + 2];
    wcsncpy(pszFilename, strDevicePath, MAX_PATH);
    pszFilename[MAX_PATH] = 0;

    // Translate path with device name to drive letters.
    WCHAR szTemp[1024];
    szTemp[0] = '\0';

    if (GetLogicalDriveStringsW(NUMELMS(szTemp) - 1, szTemp))
    {
        WCHAR  szName[MAX_PATH];
        WCHAR  szDrive[3] = L" :";
        BOOL   bFound = FALSE;
        WCHAR* p = szTemp;

        do
        {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDeviceW(szDrive, szName, MAX_PATH))
            {
                UINT uNameLen = wcslen(szName);

                if (uNameLen < MAX_PATH)
                {
                    bFound = wcsnicmp(pszFilename, szName, uNameLen) == 0;

                    if (bFound && *(pszFilename + uNameLen) == L'\\')
                    {
                        // Reconstruct pszFilename using szTempFile
                        // Replace device path with DOS path
                        WCHAR szTempFile[MAX_PATH + 2];
                        StringCchPrintfW(szTempFile, MAX_PATH, L"%s%s", szDrive, pszFilename + uNameLen);
                        szTempFile[MAX_PATH] = 0;
                        StringCchCopyNW(pszFilename, MAX_PATH + 1, szTempFile, wcslen(szTempFile));
                    }
                }
            }
            // Go to the next NULL character.
            while (*p++)
                ;

        } while (!bFound && *p);            // end of string
    }
    return pszFilename;
}

///////////////////////////////////////////////////////////////////////////
//
// GetProcessPathFilename
//
//
//
///////////////////////////////////////////////////////////////////////////
SString GetProcessPathFilename(DWORD processID)
{
    for (int i = 0; i < 2; i++)
    {
        HANDLE hProcess = OpenProcess(i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID);
        if (hProcess)
        {
            WCHAR szProcessName[MAX_PATH] = L"";
            DWORD dwSize = NUMELMS(szProcessName);
            DWORD bOk = QueryFullProcessImageNameW(hProcess, 0, szProcessName, &dwSize);
            CloseHandle(hProcess);
            if (bOk)
            {
                wchar_t szBuffer[MAX_PATH * 2] = L"";
                if (GetLongPathNameW(szProcessName, szBuffer, NUMELMS(szBuffer) - 1))
                {
                    return ToUTF8(szBuffer);
                }
            }
        }
    }

    {
        {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
            if (hProcess)
            {
                WCHAR szProcessName[MAX_PATH] = L"";
                DWORD bOk = GetModuleFileNameExW(hProcess, NULL, szProcessName, NUMELMS(szProcessName));
                CloseHandle(hProcess);
                if (bOk)
                {
                    wchar_t szBuffer[MAX_PATH * 2] = L"";
                    if (GetLongPathNameW(szProcessName, szBuffer, NUMELMS(szBuffer) - 1))
                    {
                        return ToUTF8(szBuffer);
                    }
                }
            }
        }

        for (int i = 0; i < 2; i++)
        {
            HANDLE hProcess = OpenProcess(i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID);
            if (hProcess)
            {
                WCHAR szProcessName[MAX_PATH] = L"";
                DWORD bOk = GetProcessImageFileNameW(hProcess, szProcessName, NUMELMS(szProcessName));
                CloseHandle(hProcess);
                if (bOk)
                {
                    wchar_t szBuffer[MAX_PATH * 2] = L"";
                    if (GetLongPathNameW(devicePathToWin32Path(szProcessName), szBuffer, NUMELMS(szBuffer) - 1))
                    {
                        return ToUTF8(szBuffer);
                    }
                }
            }
        }
    }

    if (_NtQuerySystemInformation)
    {
        SYSTEM_PROCESS_IMAGE_NAME_INFORMATION info;
        WCHAR                                 szProcessName[MAX_PATH] = L"";
        info.ProcessId = (HANDLE)processID;
        info.ImageName.Length = 0;
        info.ImageName.MaximumLength = sizeof(szProcessName);
        info.ImageName.Buffer = szProcessName;

        NTSTATUS status = _NtQuerySystemInformation(SystemProcessImageNameInformation, &info, sizeof(info), NULL);
        if (NT_SUCCESS(status))
        {
            WString strProcessName = WStringX(info.ImageName.Buffer, info.ImageName.Length / 2);
            return ToUTF8(devicePathToWin32Path(strProcessName));
        }
    }

    return "";
}

///////////////////////////////////////////////////////////////////////////
//
// GetProcessFilename
//
// More reliable than GetProcessPathFilename, but no path
//
///////////////////////////////////////////////////////////////////////////
SString GetProcessFilename(DWORD processID)
{
    SString         strFilename;
    HANDLE          hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32W pe = {sizeof(PROCESSENTRY32W)};
    if (Process32FirstW(hSnapshot, &pe))
    {
        do
        {
            if (pe.th32ProcessID == processID)
            {
                strFilename = ToUTF8(pe.szExeFile);
                break;
            }
        } while (Process32NextW(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return strFilename;
}

///////////////////////////////////////////////////////////////////////////
//
// MyEnumProcesses
//
//
//
///////////////////////////////////////////////////////////////////////////
std::vector<DWORD> MyEnumProcesses(bool bInclude64bit, bool bIncludeCurrent)
{
    uint               uiSize = 200;
    std::vector<DWORD> processIdList;
    while (true)
    {
        processIdList.resize(uiSize);
        DWORD pBytesReturned = 0;
        if (!EnumProcesses(&processIdList[0], uiSize * sizeof(DWORD), &pBytesReturned))
        {
            processIdList.clear();
            break;
        }
        uint uiNumProcessIds = pBytesReturned / sizeof(DWORD);

        if (uiNumProcessIds != uiSize)
        {
            processIdList.resize(uiNumProcessIds);
            break;
        }

        uiSize *= 2;
    }

    // Filter list
    std::vector<DWORD> filteredList;
    for (auto processId : processIdList)
    {
        if (!bInclude64bit && !Is32bitProcess(processId))
            continue;
        if (!bIncludeCurrent && processId == GetCurrentProcessId())
            continue;
        filteredList.push_back(processId);
    }

    return filteredList;
}

///////////////////////////////////////////////////////////////////////////
//
// FindProcessId
//
// Find a process id by process name
//
///////////////////////////////////////////////////////////////////////////
DWORD FindProcessId(const SString& processName)
{
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (processesSnapshot == INVALID_HANDLE_VALUE)
        return 0;

    Process32First(processesSnapshot, &processInfo);
    do
    {
        if (processName.CompareI(processInfo.szExeFile))
        {
            CloseHandle(processesSnapshot);
            return processInfo.th32ProcessID;
        }
    } while (Process32Next(processesSnapshot, &processInfo));

    CloseHandle(processesSnapshot);
    return 0;
}

///////////////////////////////////////////////////////////////////////////
//
// GetGTAProcessList
//
// Get list of process id's with the image name ending in "gta_sa.exe"
//
///////////////////////////////////////////////////////////////////////////
std::vector<DWORD> GetGTAProcessList()
{
    std::vector<DWORD> result;

    for (auto processId : MyEnumProcesses())
    {
        SString strPathFilename = GetProcessPathFilename(processId);

        if (strPathFilename.EndsWith(GTA_EXE_NAME) || strPathFilename.EndsWith(PROXY_GTA_EXE_NAME) || strPathFilename.EndsWith(STEAM_GTA_EXE_NAME))
            ListAddUnique(result, processId);
    }

    if (DWORD processId = FindProcessId(GTA_EXE_NAME))
        ListAddUnique(result, processId);

    if (DWORD processId = FindProcessId(PROXY_GTA_EXE_NAME))
        ListAddUnique(result, processId);

    if (DWORD processId = FindProcessId(STEAM_GTA_EXE_NAME))
        ListAddUnique(result, processId);

    return result;
}

///////////////////////////////////////////////////////////////////////////
//
// IsGTARunning
//
//
//
///////////////////////////////////////////////////////////////////////////
bool IsGTARunning()
{
    return !GetGTAProcessList().empty();
}

///////////////////////////////////////////////////////////////////////////
//
// TerminateGTAIfRunning
//
//
//
///////////////////////////////////////////////////////////////////////////
void TerminateGTAIfRunning()
{
    std::vector<DWORD> processIdList = GetGTAProcessList();

    // Try to stop all GTA process id's
    for (uint i = 0; i < 3 && processIdList.size(); i++)
    {
        for (auto processId : processIdList)
        {
            TerminateProcess(processId);
        }
        Sleep(1000);
        processIdList = GetGTAProcessList();
    }
}

///////////////////////////////////////////////////////////////////////////
//
// GetOtherMTAProcessList
//
// Get list of process id's with the image name ending with the same name as this process
//
///////////////////////////////////////////////////////////////////////////
std::vector<DWORD> GetOtherMTAProcessList()
{
    std::vector<DWORD> result;

    for (auto processId : MyEnumProcesses())
    {
        SString strPathFilename = GetProcessPathFilename(processId);
        if (strPathFilename.EndsWith(MTA_EXE_NAME))
            ListAddUnique(result, processId);
    }

    if (DWORD processId = FindProcessId(MTA_EXE_NAME))
        ListAddUnique(result, processId);

    // Ignore this process
    ListRemove(result, GetCurrentProcessId());

    return result;
}

///////////////////////////////////////////////////////////////////////////
//
// IsOtherMTARunning
//
//
//
///////////////////////////////////////////////////////////////////////////
bool IsOtherMTARunning()
{
    return !GetOtherMTAProcessList().empty();
}

///////////////////////////////////////////////////////////////////////////
//
// TerminateOtherMTAIfRunning
//
//
//
///////////////////////////////////////////////////////////////////////////
void TerminateOtherMTAIfRunning()
{
    std::vector<DWORD> processIdList = GetOtherMTAProcessList();

    if (processIdList.size())
    {
        // Try to stop all other MTA process id's
        for (uint i = 0; i < 3 && processIdList.size(); i++)
        {
            for (auto processId : processIdList)
            {
                TerminateProcess(processId);
            }
            Sleep(1000);
            processIdList = GetOtherMTAProcessList();
        }
    }
}

//
// Return true if command line contains the string
//
bool CommandLineContains(const SString& strText)
{
    return SStringX(GetCommandLine()).Contains(strText);
}

//
// General error message box
//
void DisplayErrorMessageBox(const SString& strMessage, const SString& strErrorCode, const SString& strTroubleType)
{
    HideSplash();

    if (strTroubleType.empty())
        BrowseToSolution(strTroubleType, SHOW_MESSAGE_ONLY, strMessage, strErrorCode);
    else
        BrowseToSolution(strTroubleType, ASK_GO_ONLINE | TERMINATE_IF_YES, strMessage, strErrorCode);
}

auto GetMTARootDirectory() -> std::filesystem::path
{
    static const auto directory = fs::path{FromUTF8(GetMTASAPath())};
    return directory;
}

auto GetGameBaseDirectory() -> fs::path
{
    static const auto directory = fs::path{FromUTF8(GetGTAPath())};
    return directory;
}

auto GetGameLaunchDirectory() -> fs::path
{
    static const auto directory = fs::path{FromUTF8(GetMTADataPath())} / "GTA San Andreas";
    return directory;
}

auto GetGameExecutablePath() -> std::filesystem::path
{
    static const auto executable = GetGameLaunchDirectory() / GTA_EXE_NAME;
    return executable;
}

void SetMTASAPathSource(bool bReadFromRegistry)
{
    if (bReadFromRegistry)
    {
        g_strMTASAPath = GetMTASABaseDir();
    }
    else
    {
        // Get current module full path
        SString strLaunchPathFilename = GetLaunchPathFilename();

        SString strHash = "-";
        {
            MD5        md5;
            CMD5Hasher Hasher;
            if (Hasher.Calculate(strLaunchPathFilename, md5))
            {
                char szHashResult[33];
                Hasher.ConvertToHex(md5, szHashResult);
                strHash = szHashResult;
            }
        }

        SetRegistryValue("", "Last Run Path", strLaunchPathFilename);
        SetRegistryValue("", "Last Run Path Hash", strHash);
        SetRegistryValue("", "Last Run Path Version", MTA_DM_ASE_VERSION);

        // Strip the module name out of the path.
        SString strLaunchPath = GetLaunchPath();

        // Save to a temp registry key
        SetRegistryValue("", "Last Run Location", strLaunchPath);
        g_strMTASAPath = strLaunchPath;
    }
}

SString GetMTASAPath()
{
    if (g_strMTASAPath == "")
        SetMTASAPathSource(false);
    return g_strMTASAPath;
}

///////////////////////////////////////////////////////////////
//
// LookForGtaProcess
//
//
//
///////////////////////////////////////////////////////////////
bool LookForGtaProcess(SString& strOutPathFilename)
{
    for (auto processId : GetGTAProcessList())
    {
        SString strPathFilename = GetProcessPathFilename(processId);
        if (FileExists(strPathFilename))
        {
            strOutPathFilename = strPathFilename;
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////
//
// DoUserAssistedSearch
//
//
//
///////////////////////////////////////////////////////////////
static const SString DoUserAssistedSearch() noexcept
{
    SString result;

    MessageBox(nullptr, _("Start Grand Theft Auto: San Andreas.\nEnsure the game is placed in the 'Program Files (x86)' folder."), _("Searching for GTA: San Andreas"), MB_OK | MB_ICONINFORMATION);

    while (true)
    {
        SString path;

        if (LookForGtaProcess(path))
        {
            ExtractFilename(path, &result, nullptr);
            TerminateGTAIfRunning();
            return result;
        }

        if (MessageBox(nullptr, _("Sorry, game not found.\nStart Grand Theft Auto: San Andreas and click retry.\nEnsure the game is placed in the 'Program Files (x86)' folder."), _("Searching for GTA: San Andreas"), MB_RETRYCANCEL | MB_ICONWARNING) == IDCANCEL)
            return result;
    }
}

///////////////////////////////////////////////////////////////
//
// GetGamePath
//
//
//
///////////////////////////////////////////////////////////////
ePathResult GetGamePath(SString& strOutResult, bool bFindIfMissing)
{
    // Registry places to look
    std::vector<SString> pathList;

    // Try HKLM "SOFTWARE\\Multi Theft Auto: San Andreas All\\Common\\"
    pathList.push_back(GetCommonRegistryValue("", "GTA:SA Path"));

    // Unicode character check on first one
    if (strlen(pathList[0].c_str()))
    {
        // Check for replacement characters (?), to see if there are any (unsupported) unicode characters
        if (strchr(pathList[0].c_str(), '?') > 0)
            return GAME_PATH_UNICODE_CHARS;
    }

    // Then step through looking for a known existing file
    for (uint i = 0; i < pathList.size(); i++)
    {
        if (pathList[i].empty())
            continue;

        if (FileExists(PathJoin(pathList[i], MTA_GTA_KNOWN_FILE_NAME)))
        {
            strOutResult = pathList[i];
            // Update registry.
            SetCommonRegistryValue("", "GTA:SA Path", strOutResult);
            return GAME_PATH_OK;
        }
    }

    // Try to find?
    if (!bFindIfMissing)
        return GAME_PATH_MISSING;

    // Ask user to browse for GTA
    BROWSEINFOW bi = {0};
    WString     strMessage(_("Select your Grand Theft Auto: San Andreas Installation Directory"));
    bi.lpszTitle = strMessage;
    LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);

    if (pidl != 0)
    {
        wchar_t szBuffer[MAX_PATH];
        // get the name of the  folder
        if (SHGetPathFromIDListW(pidl, szBuffer))
        {
            strOutResult = ToUTF8(szBuffer);
        }

        // free memory used
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }
    }

    // Check browse result
    if (!FileExists(PathJoin(strOutResult, MTA_GTA_KNOWN_FILE_NAME)))
    {
        // If browse didn't help, try another method
        strOutResult = DoUserAssistedSearch();

        if (!FileExists(PathJoin(strOutResult, MTA_GTA_KNOWN_FILE_NAME)))
        {
            // If still not found, give up
            return GAME_PATH_MISSING;
        }
    }

    // File found. Update registry.
    SetCommonRegistryValue("", "GTA:SA Path", strOutResult);
    return GAME_PATH_OK;
}

///////////////////////////////////////////////////////////////
//
// DiscoverGTAPath
//
// Find and cache GTA path
//
///////////////////////////////////////////////////////////////
ePathResult DiscoverGTAPath(bool bFindIfMissing)
{
    return GetGamePath(g_strGTAPath, bFindIfMissing);
}

///////////////////////////////////////////////////////////////
//
// GetGTAPath
//
//
//
///////////////////////////////////////////////////////////////
SString GetGTAPath()
{
    if (g_strGTAPath == "")
        DiscoverGTAPath(false);
    return g_strGTAPath;
}

///////////////////////////////////////////////////////////////
//
// HasGTAPath
//
//
//
///////////////////////////////////////////////////////////////
bool HasGTAPath()
{
    SString strGTAPath = GetGTAPath();
    if (!strGTAPath.empty())
    {
        return FileExists(PathJoin(strGTAPath, GTA_EXE_NAME)) || FileExists(PathJoin(strGTAPath, STEAM_GTA_EXE_NAME));
    }
    return false;
}

///////////////////////////////////////////////////////////////
//
// GetPEFileOffsets
//
// Get some commonly used file offsets
//
///////////////////////////////////////////////////////////////
void GetPEFileOffsets(SPEFileOffsets& outOffsets, const SString& strGTAEXEPath)
{
    outOffsets = {0};
    long NtHeaders = 0;
    ReadFileValue(strGTAEXEPath, NtHeaders, offsetof(IMAGE_DOS_HEADER, e_lfanew));
    outOffsets.TimeDateStamp = NtHeaders + offsetof(IMAGE_NT_HEADERS, FileHeader.TimeDateStamp);
    outOffsets.Characteristics = NtHeaders + offsetof(IMAGE_NT_HEADERS, FileHeader.Characteristics);
    outOffsets.AddressOfEntryPoint = NtHeaders + offsetof(IMAGE_NT_HEADERS, OptionalHeader.AddressOfEntryPoint);
    outOffsets.DllCharacteristics = NtHeaders + offsetof(IMAGE_NT_HEADERS, OptionalHeader.DllCharacteristics);

    ushort usSizeOfOptionalHeader = 0;
    ReadFileValue(strGTAEXEPath, usSizeOfOptionalHeader, NtHeaders + offsetof(IMAGE_NT_HEADERS, FileHeader.SizeOfOptionalHeader));
    ReadFileValue(strGTAEXEPath, outOffsets.sections[0].PointerToRawData,
                  NtHeaders + offsetof(IMAGE_NT_HEADERS, OptionalHeader) + usSizeOfOptionalHeader + offsetof(IMAGE_SECTION_HEADER, PointerToRawData));
}

///////////////////////////////////////////////////////////////
//
// GetGtaFileVersion
//
// Hardcoded numbers used:
//  0x44 - File offset 0x44 is zero in legacy DOS stub. Encrypted exe does not have this.
//  0x347ADD is the section offset equivalent of 0x748ADD as used in CGameSA::FindGameVersion
//  0x53FF and 0x840F are also used in CGameSA::FindGameVersion
//
///////////////////////////////////////////////////////////////
EGtaFileVersion GetGtaFileVersion(const SString& strGTAEXEPath)
{
    SPEFileOffsets fileOffsets;
    GetPEFileOffsets(fileOffsets, strGTAEXEPath);

    char   bIsEncypted = false;
    ushort usIdBytes = 0;
    ReadFileValue(strGTAEXEPath, bIsEncypted, 0x44);
    ReadFileValue(strGTAEXEPath, usIdBytes, 0x347ADD + fileOffsets.sections[0].PointerToRawData);

    EGtaFileVersion versionType = EGtaFileVersion::Unknown;
    if (usIdBytes == 0x53FF)
        versionType = EGtaFileVersion::US;
    else if (usIdBytes == 0x840F)
        versionType = EGtaFileVersion::EU;
    else if (bIsEncypted)
        versionType = EGtaFileVersion::Encrypted;

    return versionType;
}

///////////////////////////////////////////////////////////////
//
// FindFilesRecursive
//
// Return a list of files inside strPath
//
///////////////////////////////////////////////////////////////
void FindFilesRecursive(const SString& strPathMatch, std::vector<SString>& outFileList, uint uiMaxDepth)
{
    SString strPath, strMatch;
    strPathMatch.Split("\\", &strPath, &strMatch, -1);

    std::list<SString> toDoList;
    toDoList.push_back(strPath);
    while (toDoList.size())
    {
        SString strPathHere = toDoList.front();
        toDoList.pop_front();

        std::vector<SString> fileListHere = FindFiles(strPathHere + "\\" + strMatch, true, false);
        std::vector<SString> dirListHere = FindFiles(strPathHere + "\\" + strMatch, false, true);

        for (unsigned int i = 0; i < fileListHere.size(); i++)
        {
            SString filePathName = strPathHere + "\\" + fileListHere[i];
            outFileList.push_back(filePathName);
        }
        for (unsigned int i = 0; i < dirListHere.size(); i++)
        {
            SString dirPathName = strPathHere + "\\" + dirListHere[i];
            toDoList.push_back(dirPathName);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// FindRelevantFiles
//
// Return a list of files and directories insode strPath, but not inside any dir in stopList
//
///////////////////////////////////////////////////////////////
void FindRelevantFiles(const SString& strPath, std::vector<SString>& outFilePathList, std::vector<SString>& outDirPathList,
                       const std::vector<SString>& stopList, unsigned int MaxFiles, unsigned int MaxDirs)
{
    std::list<SString> toDoList;
    toDoList.push_back(strPath);
    outDirPathList.push_back(strPath);
    while (toDoList.size())
    {
        {
            // Update progress bar if visible
            int NumItems = outFilePathList.size() + outDirPathList.size();
            int MaxItems = (MaxFiles ? MaxFiles : 25000) + (MaxDirs ? MaxDirs : 5000);
            if (UpdateProgress(std::min(NumItems, MaxItems), MaxItems * 2, "Checking files..."))
                return;
        }

        SString strPathHere = toDoList.front();
        toDoList.pop_front();

        std::vector<SString> fileListHere = FindFiles(strPathHere + "\\*", true, false);
        std::vector<SString> dirListHere = FindFiles(strPathHere + "\\*", false, true);

        for (unsigned int i = 0; i < fileListHere.size(); i++)
        {
            SString filePathName = strPathHere + "\\" + fileListHere[i];
            outFilePathList.push_back(filePathName);
        }
        for (unsigned int i = 0; i < dirListHere.size(); i++)
        {
            SString dirPathName = strPathHere + "\\" + dirListHere[i];
            outDirPathList.push_back(dirPathName);

            bool bTraverse = true;
            for (unsigned int k = 0; k < stopList.size(); k++)
                if (dirListHere[i].length() >= stopList[k].length())
                    if (dirListHere[i].ToLower().substr(0, stopList[k].length()) == stopList[k])
                        bTraverse = false;
            if (bTraverse)
                toDoList.push_back(dirPathName);
        }

        if (MaxFiles && outFilePathList.size() > MaxFiles)
            if (MaxDirs && outDirPathList.size() > MaxDirs)
                break;
    }
}

///////////////////////////////////////////////////////////////
//
// MakeRandomIndexList
//
// Create a list of randomlu ordered indices from 0 to Size-1
//
///////////////////////////////////////////////////////////////
void MakeRandomIndexList(int Size, std::vector<int>& outList)
{
    for (int i = 0; i < Size; i++)
        outList.push_back(i);

    for (int i = 0; i < Size; i++)
    {
        int otherIdx = rand() % Size;
        int Temp = outList[i];
        outList[i] = outList[otherIdx];
        outList[otherIdx] = Temp;
    }
}

///////////////////////////////////////////////////////////////
//
// GetOSVersion
//
// Affected by compatibility mode
//
///////////////////////////////////////////////////////////////
SOSVersionInfo GetOSVersion()
{
    OSVERSIONINFO versionInfo;
    memset(&versionInfo, 0, sizeof(versionInfo));
    versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
    GetVersionEx(&versionInfo);
    return {versionInfo.dwMajorVersion, versionInfo.dwMinorVersion, versionInfo.dwBuildNumber};
}

///////////////////////////////////////////////////////////////
//
// GetRealOSVersion
//
// Ignoring compatibility mode
//
///////////////////////////////////////////////////////////////
SOSVersionInfo GetRealOSVersion()
{
    static SOSVersionInfo versionInfo = {0};

    // Try WMI first
    if (versionInfo.dwMajor == 0)
    {
        SString              strVersionAndBuild = GetWMIOSVersion();
        std::vector<SString> parts;
        strVersionAndBuild.Split(".", parts);
        if (parts.size() == 3)
        {
            versionInfo.dwMajor = atoi(parts[0]);
            versionInfo.dwMinor = atoi(parts[1]);
            versionInfo.dwBuild = atoi(parts[2]);
        }
    }

    // Fallback to checking file version of kernel32.dll
    if (versionInfo.dwMajor == 0)
    {
        SLibVersionInfo fileInfo;
        if (GetLibVersionInfo("kernel32.dll", &fileInfo))
        {
            versionInfo.dwMajor = HIWORD(fileInfo.dwFileVersionMS);
            versionInfo.dwMinor = LOWORD(fileInfo.dwFileVersionMS);
            versionInfo.dwBuild = HIWORD(fileInfo.dwFileVersionLS);
        }
    }

    return versionInfo;
}

///////////////////////////////////////////////////////////////
//
// IsWindows10OrGreater
//
// Works around limit for applications not manifested for Windows 10
//
///////////////////////////////////////////////////////////////
bool IsWindows10OrGreater()
{
    SOSVersionInfo info = GetRealOSVersion();
    return info.dwMajor >= 10;
}

bool IsWindows10Threshold2OrGreater()
{
    SOSVersionInfo info = GetRealOSVersion();
    return info.dwMajor > 10 || (info.dwMajor == 10 && info.dwBuild >= 10586);
}

///////////////////////////////////////////////////////////////
//
// IsUserAdmin
//
//
//
///////////////////////////////////////////////////////////////
BOOL IsUserAdmin()
/*++
Routine Description: This routine returns TRUE if the caller's
process is a member of the Administrators local group. Caller is NOT
expected to be impersonating anyone and is expected to be able to
open its own process and process token.
Arguments: None.
Return Value:
   TRUE - Caller has Administrators local group.
   FALSE - Caller does not have Administrators local group. --
*/
{
    BOOL                     b;
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    PSID                     AdministratorsGroup;
    b = AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &AdministratorsGroup);
    if (b)
    {
        if (!CheckTokenMembership(NULL, AdministratorsGroup, &b))
        {
            b = FALSE;
        }
        FreeSid(AdministratorsGroup);
    }

    return (b);
}

//////////////////////////////////////////////////////////
//
// RelaunchAsAdmin
//
// Relaunch as admin if user agrees
//
//////////////////////////////////////////////////////////
void RelaunchAsAdmin(const SString& strCmdLine, const SString& strReason)
{
    HideSplash();
    AddReportLog(7115, SString("Loader - Request to elevate privileges (%s)", *strReason));
    MessageBoxUTF8(NULL, SString(_("MTA:SA needs Administrator access for the following task:\n\n  '%s'\n\nPlease confirm in the next window."), *strReason),
                   "Multi Theft Auto: San Andreas", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
    ReleaseSingleInstanceMutex();
    ShellExecuteNonBlocking("runas", PathJoin(GetMTASAPath(), MTA_EXE_NAME), strCmdLine);
}

/////////////////////////////////////////////////////////////////////
//
// GetLibraryHandle
//
//
//
/////////////////////////////////////////////////////////////////////
HMODULE GetLibraryHandle(const SString& strFilename, DWORD* pdwOutLastError)
{
    if (!hLibraryModule)
    {
        // Get path to the relevant file
        SString strLibPath = PathJoin(GetLaunchPath(), "mta");
        SString strLibPathFilename = PathJoin(strLibPath, strFilename);

        SString strPrevCurDir = GetSystemCurrentDirectory();
        SetCurrentDirectory(strLibPath);
        SetDllDirectory(strLibPath);

        hLibraryModule = LoadLibrary(strLibPathFilename);
        if (pdwOutLastError)
            *pdwOutLastError = GetLastError();

        SetCurrentDirectory(strPrevCurDir);
        SetDllDirectory(strPrevCurDir);
    }

    return hLibraryModule;
}

/////////////////////////////////////////////////////////////////////
//
// FreeLibraryHandle
//
//
//
/////////////////////////////////////////////////////////////////////
void FreeLibraryHandle()
{
    if (hLibraryModule)
    {
        FreeLibrary(hLibraryModule);
        hLibraryModule = NULL;
    }
}

/////////////////////////////////////////////////////////////////////
//
// UpdateMTAVersionApplicationSetting
//
// Make sure "mta-version-ext" is correct. eg "1.0.4-9.01234.2.000"
//
/////////////////////////////////////////////////////////////////////
void UpdateMTAVersionApplicationSetting(bool bQuiet)
{
#ifdef MTA_DEBUG
    SString strFilename = "netc_d.dll";
#else
    SString strFilename = "netc.dll";
#endif

    //
    // Determine NetRev number
    //

    // Get saved status
    unsigned short       usNetRev = 65535;
    unsigned short       usNetRel = 0;
    std::vector<SString> parts;
    GetApplicationSetting("mta-version-ext").Split(".", parts);
    if (parts.size() == 6)
    {
        usNetRev = static_cast<unsigned short>(atoi(parts[4]));
        usNetRel = static_cast<unsigned short>(atoi(parts[5]));
    }

    DWORD   dwLastError = 0;
    HMODULE hModule = GetLibraryHandle(strFilename, &dwLastError);
    if (hModule)
    {
        typedef void (*PFNINITNETREV)(const char*, const char*, const char*);
        PFNINITNETREV pfnInitNetRev = static_cast<PFNINITNETREV>(static_cast<PVOID>(GetProcAddress(hModule, "InitNetRev")));
        if (pfnInitNetRev)
            pfnInitNetRev(GetProductRegistryPath(), GetProductCommonDataDir(), GetProductVersion());
        typedef unsigned short (*PFNGETNETREV)();
        PFNGETNETREV pfnGetNetRev = static_cast<PFNGETNETREV>(static_cast<PVOID>(GetProcAddress(hModule, "GetNetRev")));
        if (pfnGetNetRev)
            usNetRev = pfnGetNetRev();
        PFNGETNETREV pfnGetNetRel = static_cast<PFNGETNETREV>(static_cast<PVOID>(GetProcAddress(hModule, "GetNetRel")));
        if (pfnGetNetRel)
            usNetRel = pfnGetNetRel();
    }
    else if (!bQuiet)
    {
        SString strError = GetSystemErrorMessage(dwLastError);
        SString strMessage(_("Error loading %s module! (%s)"), *strFilename.ToLower(), *strError);
        DisplayErrorMessageBox(strMessage, _E("CL38"), "module-not-loadable&name=" + ExtractBeforeExtension(strFilename));
    }

    if (!bQuiet)
        SetApplicationSetting("mta-version-ext", SString("%d.%d.%d-%d.%05d.%c.%03d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR, MTASA_VERSION_MAINTENANCE,
                                                         MTASA_VERSION_TYPE, MTASA_VERSION_BUILD, usNetRev == 65535 ? '+' : usNetRev + '0', usNetRel));
}

///////////////////////////////////////////////////////////////////////////
//
// Is32bitProcess
//
// Determine if processID is a 32 bit process or not.
// Assumes current process is 32 bit.
//
// (Calling GetModuleFileNameEx or EnumProcessModules on a 64bit process from a 32bit process can cause problems)
//
///////////////////////////////////////////////////////////////////////////
bool Is32bitProcess(DWORD processID)
{
    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
    static bool                bDoneGetProcAddress = false;
    static bool                bDoneIs64BitOS = false;
    static bool                bIs64BitOS = false;

    if (!bDoneGetProcAddress)
    {
        // Find 'IsWow64Process'
        bDoneGetProcAddress = true;
        HMODULE hModule = GetModuleHandle("Kernel32.dll");
        fnIsWow64Process = static_cast<LPFN_ISWOW64PROCESS>(static_cast<PVOID>(GetProcAddress(hModule, "IsWow64Process")));
    }

    // Function not there? Must be 32bit everything
    if (!fnIsWow64Process)
        return true;

    // See if this is a 64 bit O/S
    if (!bDoneIs64BitOS)
    {
        bDoneIs64BitOS = true;

        // We know current process is 32 bit. See if it is running under WOW64
        BOOL bIsWow64 = FALSE;
        BOOL bOk = fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
        if (bOk)
        {
            // Must be 64bit O/S if current (32 bit) process is running under WOW64
            if (bIsWow64)
                bIs64BitOS = true;
        }
    }

    // Not 64 bit O/S? Must be 32bit everything
    if (!bIs64BitOS)
        return true;

    // Call 'IsWow64Process' on query process
    for (int i = 0; i < 2; i++)
    {
        HANDLE hProcess = OpenProcess(i == 0 ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processID);

        if (hProcess)
        {
            BOOL bIsWow64 = FALSE;
            BOOL bOk = fnIsWow64Process(hProcess, &bIsWow64);
            CloseHandle(hProcess);

            if (bOk)
            {
                if (bIsWow64 == FALSE)
                    return false;            // 64 bit O/S and process not running under WOW64, so it must be a 64 bit process
                return true;
            }
        }
    }

    return false;            // Can't determine. Guess it's 64 bit
}

///////////////////////////////////////////////////////////////////////////
//
// TerminateProcess
//
// Terminate process from pid
//
///////////////////////////////////////////////////////////////////////////
bool TerminateProcess(DWORD dwProcessID, uint uiExitCode)
{
    bool success = false;

    if (HMODULE handle = GetLibraryHandle("kernel32.dll"); handle)
    {
        using Signature = bool (*)(DWORD, UINT);
        static auto NtTerminateProcess_ = reinterpret_cast<Signature>(static_cast<void*>(GetProcAddress(handle, "NtTerminateProcess")));

        if (NtTerminateProcess_)
        {
            success = NtTerminateProcess_(dwProcessID, uiExitCode);
            AddReportLog(8070, SString("TerminateProcess %d result: %d", dwProcessID, success));
        }
    }

    if (!success)
    {
        if (HANDLE handle = OpenProcess(PROCESS_TERMINATE, 0, dwProcessID); handle)
        {
            success = TerminateProcess(handle, uiExitCode);
            CloseHandle(handle);
        }
    }

    return success;
}

///////////////////////////////////////////////////////////////////////////
//
// CreateSingleInstanceMutex
//
//
//
///////////////////////////////////////////////////////////////////////////
bool CreateSingleInstanceMutex()
{
    HANDLE hMutex = CreateMutex(NULL, FALSE, TEXT(MTA_GUID));

    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        if (hMutex)
            CloseHandle(hMutex);
        return false;
    }
    assert(!g_hMutex);
    g_hMutex = hMutex;
    return true;
}

///////////////////////////////////////////////////////////////////////////
//
// ReleaseSingleInstanceMutex
//
//
//
///////////////////////////////////////////////////////////////////////////
void ReleaseSingleInstanceMutex()
{
    // assert(g_hMutex);
    CloseHandle(g_hMutex);
    g_hMutex = NULL;
}

///////////////////////////////////////////////////////////////////////////
//
// WaitForObject
//
// Wait for object to finish
// Returns non-zero if wait failed.
//
///////////////////////////////////////////////////////////////////////////
uint WaitForObject(HANDLE hProcess, HANDLE hThread, DWORD dwMilliseconds, HANDLE hMutex)
{
    uint uiResult = 0;

    HMODULE hModule = GetLibraryHandle("kernel32.dll");

    if (hModule)
    {
        typedef unsigned long (*PFNWaitForObject)(HANDLE, ...);
        PFNWaitForObject pfnWaitForObject = static_cast<PFNWaitForObject>(static_cast<PVOID>(GetProcAddress(hModule, "WaitForObject")));

        if (!pfnWaitForObject || pfnWaitForObject(hProcess, hThread, dwMilliseconds, hMutex))
            uiResult = 1;
    }

    return uiResult;
}

///////////////////////////////////////////////////////////////////////////
//
// CheckService
//
// Check service status
// Returns false on fail
//
///////////////////////////////////////////////////////////////////////////
bool CheckService(uint uiStage)
{
    HMODULE hModule = GetLibraryHandle("kernel32.dll");

    if (hModule)
    {
        typedef bool (*PFNCheckService)(uint);
        PFNCheckService pfnCheckService = static_cast<PFNCheckService>(static_cast<PVOID>(GetProcAddress(hModule, "CheckService")));

        if (pfnCheckService)
        {
            bool bResult = pfnCheckService(uiStage);
            AddReportLog(8070, SString("CheckService %d result: %d", uiStage, bResult));
            return bResult;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////
//
// GetFileAge
//
// Returns time in seconds since a file/directory was created
//
///////////////////////////////////////////////////////////////////////////
int GetFileAge(const SString& strPathFilename)
{
    WIN32_FIND_DATAW findFileData;
    HANDLE           hFind = FindFirstFileW(FromUTF8(strPathFilename), &findFileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        FILETIME ftNow;
        GetSystemTimeAsFileTime(&ftNow);
        LARGE_INTEGER creationTime = {findFileData.ftCreationTime.dwLowDateTime, static_cast<LONG>(findFileData.ftCreationTime.dwHighDateTime)};
        LARGE_INTEGER timeNow = {ftNow.dwLowDateTime, static_cast<LONG>(ftNow.dwHighDateTime)};
        return static_cast<int>((timeNow.QuadPart - creationTime.QuadPart) / (LONGLONG)10000000);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////
//
// CleanDownloadCache
//
// Remove old files from the download cache
//
///////////////////////////////////////////////////////////////////////////
void CleanDownloadCache()
{
    const uint uiMaxCleanTime = 5;                           // Limit clean time (seconds)
    const uint uiCleanFileAge = 60 * 60 * 24 * 7;            // Delete files older than this

    const time_t tMaxEndTime = time(NULL) + uiMaxCleanTime;

    // Search possible cache locations
    std::list<SString> cacheLocationList;
    cacheLocationList.push_back(PathJoin(GetMTADataPath(), "upcache"));
    cacheLocationList.push_back(PathJoin(GetMTATempPath(), "upcache"));
    cacheLocationList.push_back(GetMTATempPath());

    for (; !cacheLocationList.empty(); cacheLocationList.pop_front())
    {
        // Get list of files & directories in this cache location
        const SString&             strCacheLocation = cacheLocationList.front();
        const std::vector<SString> fileList = FindFiles(PathJoin(strCacheLocation, "\\*"), true, true);

        for (uint i = 0; i < fileList.size(); i++)
        {
            const SString strPathFilename = PathJoin(strCacheLocation, fileList[i]);
            // Check if over 7 days old
            if (GetFileAge(strPathFilename) > uiCleanFileAge)
            {
                // Delete as directory or file
                if (DirectoryExists(strPathFilename))
                    DelTree(strPathFilename, strCacheLocation);
                else
                    FileDelete(strPathFilename);

                // Check time spent
                if (time(NULL) > tMaxEndTime)
                    break;
            }
        }
    }
}

//////////////////////////////////////////////////////////
//
// IsDirectoryEmpty
//
// Returns true if directory does not exist, or it is empty
//
//////////////////////////////////////////////////////////
bool IsDirectoryEmpty(const SString& strSrcBase)
{
    return FindFiles(PathJoin(strSrcBase, "*"), true, true).empty();
}

//////////////////////////////////////////////////////////
//
// GetDiskFreeSpace
//
// Get free disk space in bytes
//
//////////////////////////////////////////////////////////
long long GetDiskFreeSpace(SString strSrcBase)
{
    for (uint i = 0; i < 100; i++)
    {
        ULARGE_INTEGER FreeBytesAvailable;
        if (GetDiskFreeSpaceEx(strSrcBase, &FreeBytesAvailable, NULL, NULL))
            return FreeBytesAvailable.QuadPart;

        strSrcBase = ExtractPath(strSrcBase);
    }
    return 0;
}

//////////////////////////////////////////////////////////
//
// DirectoryCopy
//
// Recursive directory copy
//
//
//////////////////////////////////////////////////////////
void DirectoryCopy(SString strSrcBase, SString strDestBase, bool bShowProgressDialog = false, int iMinFreeSpaceMB = 1)
{
    // Setup diskspace checking
    bool      bCheckFreeSpace = false;
    long long llFreeBytesAvailable = GetDiskFreeSpace(strDestBase);
    if (llFreeBytesAvailable != 0)
        bCheckFreeSpace = (llFreeBytesAvailable < (iMinFreeSpaceMB + 10000) * 0x100000LL);            // Only check if initial freespace is less than 10GB

    if (bShowProgressDialog)
        ShowProgressDialog(g_hInstance, _("Copying files..."), true);

    strSrcBase = PathConform(strSrcBase).TrimEnd(PATH_SEPERATOR);
    strDestBase = PathConform(strDestBase).TrimEnd(PATH_SEPERATOR);

    float              fProgress = 0;
    float              fUseProgress = 0;
    std::list<SString> toDoList;
    toDoList.push_back("");
    while (toDoList.size())
    {
        fProgress += 0.5f;
        fUseProgress = fProgress;
        if (fUseProgress > 50)
            fUseProgress = std::min(100.f, pow(fUseProgress - 50, 0.6f) + 50);

        SString strPathHereBaseRel = toDoList.front();
        toDoList.pop_front();

        SString strPathHereSrc = PathJoin(strSrcBase, strPathHereBaseRel);
        SString strPathHereDest = PathJoin(strDestBase, strPathHereBaseRel);

        std::vector<SString> fileListHere = FindFiles(PathJoin(strPathHereSrc, "*"), true, false);
        std::vector<SString> dirListHere = FindFiles(PathJoin(strPathHereSrc, "*"), false, true);

        // Copy the files at this level
        for (unsigned int i = 0; i < fileListHere.size(); i++)
        {
            SString filePathNameSrc = PathJoin(strPathHereSrc, fileListHere[i]);
            SString filePathNameDest = PathJoin(strPathHereDest, fileListHere[i]);
            if (bShowProgressDialog)
                if (UpdateProgress((int)fUseProgress, 100, SString("...%s", *PathJoin(strPathHereBaseRel, fileListHere[i]))))
                    goto stop_copy;

            // Check enough disk space
            if (bCheckFreeSpace)
            {
                llFreeBytesAvailable -= FileSize(filePathNameSrc);
                if (llFreeBytesAvailable / 0x100000LL < iMinFreeSpaceMB)
                    goto stop_copy;
            }

            FileCopy(filePathNameSrc, filePathNameDest, true);
        }

        // Add the directories at this level
        for (unsigned int i = 0; i < dirListHere.size(); i++)
        {
            SString dirPathNameBaseRel = PathJoin(strPathHereBaseRel, dirListHere[i]);
            toDoList.push_back(dirPathNameBaseRel);
        }
    }

stop_copy:
    if (bShowProgressDialog)
    {
        // Reassuring messages
        if (toDoList.size())
        {
            Sleep(1000);
            UpdateProgress((int)fUseProgress, 100, _("Copy finished early. Everything OK."));
            Sleep(2000);
        }
        else
        {
            fUseProgress = std::max(90.f, fUseProgress);
            UpdateProgress((int)fUseProgress, 100, _("Finishing..."));
            Sleep(1000);
            UpdateProgress(100, 100, _("Done!"));
            Sleep(2000);
        }

        HideProgressDialog();
    }
}

//////////////////////////////////////////////////////////
//
// MaybeShowCopySettingsDialog
//
// For new installs, give the user an option to copy
// settings from a previous version
//
//////////////////////////////////////////////////////////
void MaybeShowCopySettingsDialog()
{
    // Check if coreconfig.xml is present
    const SString strMTASAPath = GetMTASAPath();
    SString       strCurrentConfig = PathJoin(GetMTASAPath(), "mta", "config", "coreconfig.xml");
    if (FileExists(strCurrentConfig))
        return;

    // Check if coreconfig.xml from previous version is present
    SString strCurrentVersion = SString("%d.%d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR);
    SString strPreviousVersion = SString("%d.%d", MTASA_VERSION_MAJOR, MTASA_VERSION_MINOR - 1);
    SString strPreviousPath = GetVersionRegistryValue(strPreviousVersion, "", "Last Run Location");
    if (strPreviousPath.empty())
        return;
    SString strPreviousConfig = PathJoin(strPreviousPath, "mta", "config", "coreconfig.xml");
    if (MTASA_VERSION_MAJOR == 1 && MTASA_VERSION_MINOR == 5)
        strPreviousConfig = PathJoin(strPreviousPath, "mta", "coreconfig.xml");
    if (!FileExists(strPreviousConfig))
        return;

    HideSplash();            // Hide standard MTA splash

    // Show dialog
    SString strMessage;
    strMessage += SString(_("New installation of %s detected.\n"
                            "\n"
                            "Do you want to copy your settings from %s ?"),
                          *strCurrentVersion, *strPreviousVersion);
    int iResponse = MessageBoxUTF8(NULL, strMessage, "MTA: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
    if (iResponse != IDYES)
        return;

    // Copy settings from previous version
    FileCopy(strPreviousConfig, strCurrentConfig);

    // Copy registry setting for aero-enabled
    SString strAeroEnabled = GetVersionRegistryValue(strPreviousVersion, PathJoin("Settings", "general"), "aero-enabled");
    SetApplicationSetting("aero-enabled", strAeroEnabled);

    // Copy some directories if empty
    SString strCurrentNewsDir = PathJoin(GetMTADataPath(), "news");

    SString strPreviousDataPath = PathJoin(GetSystemCommonAppDataPath(), GetProductCommonDataDir(), strPreviousVersion);
    SString strPreviousNewsDir = PathJoin(strPreviousDataPath, "news");

    if (IsDirectoryEmpty(strCurrentNewsDir) && DirectoryExists(strPreviousNewsDir))
        DirectoryCopy(strPreviousNewsDir, strCurrentNewsDir);
}

//////////////////////////////////////////////////////////
//
// CheckAndShowFileOpenFailureMessage
//
// Returns true if message was displayed
//
//////////////////////////////////////////////////////////
bool CheckAndShowFileOpenFailureMessage()
{
    SString strFilename = GetApplicationSetting("diagnostics", "gta-fopen-fail");

    if (!strFilename.empty())
    {
        SetApplicationSetting("diagnostics", "gta-fopen-fail", "");
        SString strMsg(_("GTA:SA had trouble opening the file '%s'"), *strFilename);
        DisplayErrorMessageBox(strMsg, _E("CL31"), SString("gta-fopen-fail&name=%s", *strFilename));
        return true;
    }
    return false;
}

//////////////////////////////////////////////////////////
//
// CheckAndShowMissingFileMessage
//
// Check for missing files that could cause a crash
//
//////////////////////////////////////////////////////////
void CheckAndShowMissingFileMessage()
{
    SString strFilename = PathJoin("text", "american.gxt");

    const SString strGTAPathFilename = PathJoin(GetGTAPath(), strFilename);

    if (!FileExists(strGTAPathFilename))
    {
        SString strMsg(_("GTA:SA is missing the file '%s'."), *strFilename);
        DisplayErrorMessageBox(strMsg, _E("CL36"), SString("gta-file-missing&name=%s", *strFilename));
    }
}

//////////////////////////////////////////////////////////
//
// CheckAndShowModelProblems
//
// Check for flagged model problems
//
//////////////////////////////////////////////////////////
void CheckAndShowModelProblems()
{
    SString strReason;
    int     iModelId = 0;
    CArgMap argMap;
    argMap.SetFromString(GetApplicationSetting("diagnostics", "gta-model-fail"));
    argMap.Get("reason", strReason);
    argMap.Get("id", iModelId);
    SetApplicationSetting("diagnostics", "gta-model-fail", "");

    if (iModelId)
    {
        SString strMsg;
        strMsg += _("GTA:SA had trouble loading a model.");
        strMsg += SString(" (%d)\n\n", iModelId);
        strMsg += _("If you recently modified gta3.img, then try reinstalling GTA:SA.");
        DisplayErrorMessageBox(strMsg, _E("CL34"), SString("gta-model-fail&id=%d&reason=%s", iModelId, *strReason));
    }
}

//////////////////////////////////////////////////////////
//
// CheckAndShowUpgradeProblems
//
// Check for flagged upgrade problems
//
//////////////////////////////////////////////////////////
void CheckAndShowUpgradeProblems()
{
    int     iModelId = 0, iUpgradeId, iFrame;
    CArgMap argMap;
    argMap.SetFromString(GetApplicationSetting("diagnostics", "gta-upgrade-fail"));
    argMap.Get("vehid", iModelId);
    argMap.Get("upgid", iUpgradeId);
    argMap.Get("frame", iFrame);
    SetApplicationSetting("diagnostics", "gta-upgrade-fail", "");

    if (iModelId)
    {
        SString strMsg;
        strMsg += _("GTA:SA had trouble adding an upgrade to a vehicle.");
        strMsg += SString(" (%d)", iModelId);
        DisplayErrorMessageBox(strMsg, _E("CL35"), SString("gta-upgrade-fail&id=%d&upgid=%d&frame=%d", iModelId, iUpgradeId, iFrame));
    }
}

//////////////////////////////////////////////////////////
//
// CheckAndShowImgProblems
//
// Check for flagged img problems
//
//////////////////////////////////////////////////////////
void CheckAndShowImgProblems()
{
    SString strFilename = GetApplicationSetting("diagnostics", "img-file-corrupt");
    SetApplicationSetting("diagnostics", "img-file-corrupt", "");
    if (!strFilename.empty())
    {
        SString strMsg(_("GTA:SA found errors in the file '%s'"), *strFilename);
        DisplayErrorMessageBox(strMsg, _E("CL44"), SString("img-file-corrupt&name=%s", *strFilename));
    }
}

//////////////////////////////////////////////////////////
//
// LoadFunction
//
// Load a library function
//
//////////////////////////////////////////////////////////
void* LoadFunction(const char* libraryName, const char* functionName)
{
    static std::map<std::string, HMODULE, std::less<>> libraries;

    HMODULE handle{};

    if (auto iter = libraries.find(libraryName); iter != libraries.end())
    {
        handle = iter->second;
    }
    else
    {
        if (handle = LoadLibraryA(libraryName))
            libraries[libraryName] = handle;
        else
            return nullptr;
    }

    auto result = static_cast<void*>(GetProcAddress(handle, functionName));
    return result;
}

//////////////////////////////////////////////////////////
//
// BsodDetectionPreLaunch
//
// Possible BSOD situation if a new mini-dump file was created after the last game was started
//
//////////////////////////////////////////////////////////
void BsodDetectionPreLaunch()
{
    // BSOD detection being handled elsewhere ?
    int iBsodDetectSkip = GetApplicationSettingInt(DIAG_BSOD_DETECT_SKIP);
    SetApplicationSettingInt(DIAG_BSOD_DETECT_SKIP, 0);

    // Find latest system minidump file
    SString          strMatch = PathJoin(GetSystemWindowsPath(), "MiniDump", "*");
    SString          strMinidumpTime;
    WIN32_FIND_DATAW findData;
    HANDLE           hFind = FindFirstFileW(FromUTF8(strMatch), &findData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if ((findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == false)
                if (wcscmp(findData.cFileName, L".") && wcscmp(findData.cFileName, L".."))
                {
                    SYSTEMTIME s;
                    FileTimeToSystemTime(&findData.ftCreationTime, &s);
                    SString strCreationTime("%02d-%02d-%02d %02d:%02d:%02d", s.wYear, s.wMonth, s.wDay, s.wHour, s.wMinute, s.wSecond);
                    if (strCreationTime > strMinidumpTime)
                        strMinidumpTime = strCreationTime;
                }
        } while (FindNextFileW(hFind, &findData));
        FindClose(hFind);
    }

    // Is it a new file?
    SString strLastMinidumpTime = GetApplicationSetting("diagnostics", "last-minidump-time");
    if (strMinidumpTime > strLastMinidumpTime)
    {
        SetApplicationSetting("diagnostics", "last-minidump-time", strMinidumpTime);
        IncApplicationSettingInt(DIAG_MINIDUMP_DETECTED_COUNT);

        // Was it created during the game?
        SString strGameBeginTime = GetApplicationSetting("diagnostics", "game-begin-time");
        if (strMinidumpTime > strGameBeginTime && !strGameBeginTime.empty() && iBsodDetectSkip == 0)
        {
            // Ask user to confirm
            int iResponse =
                MessageBoxUTF8(NULL, _("Did your computer restart when playing MTA:SA?"), "MTA: San Andreas", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
            if (iResponse == IDYES)
            {
                SetApplicationSetting("diagnostics", "user-confirmed-bsod-time", strMinidumpTime);
                IncApplicationSettingInt(DIAG_MINIDUMP_CONFIRMED_COUNT);

                // BSOD might be caused by progress animation, so flag for it to be disabled
                SetApplicationSettingInt(GENERAL_PROGRESS_ANIMATION_DISABLE, 1);
            }
        }
    }

    // Log BSOD status
    SString strBsodTime = GetApplicationSetting("diagnostics", "user-confirmed-bsod-time");
    if (!strBsodTime.empty())
        WriteDebugEvent(SString("User confirmed bsod time: %s", *strBsodTime));
}

//////////////////////////////////////////////////////////
//
// BsodDetectionOnGameBegin
//
// Record game start time
//
//////////////////////////////////////////////////////////
void BsodDetectionOnGameBegin()
{
    SetApplicationSetting("diagnostics", "game-begin-time", GetTimeString(true));
}

//////////////////////////////////////////////////////////
//
// BsodDetectionOnGameEnd
//
// Unrecord game start time
//
//////////////////////////////////////////////////////////
void BsodDetectionOnGameEnd()
{
    SetApplicationSetting("diagnostics", "game-begin-time", "");
}

//////////////////////////////////////////////////////////
//
// ForbodenProgramsMessage
//
// Message to advise against running certain other programs
//
//////////////////////////////////////////////////////////
void ForbodenProgramsMessage()
{
    std::vector<SString> forbodenList = {"CheatEngine", "PCHunter"};
    std::vector<SString> foundList;
    for (auto processId : MyEnumProcesses(true))
    {
        SString strFilename = ExtractFilename(GetProcessPathFilename(processId));
        for (const auto& forbodenName : forbodenList)
        {
            if (strFilename.Replace(" ", "").BeginsWithI(forbodenName))
                foundList.push_back(strFilename);
        }
    }

    if (!foundList.empty())
    {
        SString strMessage = _("Please terminate the following programs before continuing:");
        strMessage += "\n\n";
        strMessage += SString::Join("\n", foundList);
        DisplayErrorMessageBox(strMessage, _E("CL39"), "forboden-programs");
        WriteDebugEventAndReport(6550, SString("Showed forboden programs list (%s)", *SString::Join(",", foundList)));
    }
}

//////////////////////////////////////////////////////////
//
// VerifyEmbeddedSignature
//
// Check a file has been signed proper
//
//////////////////////////////////////////////////////////
bool VerifyEmbeddedSignature(const SString& strFilename)
{
    WString            wstrFilename = FromUTF8(strFilename);
    WINTRUST_FILE_INFO FileData;
    memset(&FileData, 0, sizeof(FileData));
    FileData.cbStruct = sizeof(WINTRUST_FILE_INFO);
    FileData.pcwszFilePath = *wstrFilename;

    WINTRUST_DATA WinTrustData;
    memset(&WinTrustData, 0, sizeof(WinTrustData));
    WinTrustData.cbStruct = sizeof(WinTrustData);
    WinTrustData.dwUIChoice = WTD_UI_NONE;
    WinTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    WinTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    WinTrustData.pFile = &FileData;

    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    LONG lStatus = WinVerifyTrust(NULL, &WVTPolicyGUID, &WinTrustData);
    return lStatus == ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////
//
// LogSettings
//
// Dump some settings to the log file to help debugging
//
//////////////////////////////////////////////////////////
void LogSettings()
{
    struct
    {
        int         bSkipIfZero;
        const char* szPath;
        const char* szName;
        const char* szDesc;
    } const settings[] = {
        {false, "general", GENERAL_PROGRESS_ANIMATION_DISABLE, ""},
        {false, "general", "aero-enabled", ""},
        {false, "general", "driver-overrides-disabled", ""},
        {false, "general", "device-selection-disabled", ""},
        {false, "general", "customized-sa-files-using", ""},
        {false, "general", "times-connected", ""},
        {false, "general", "times-connected-editor", ""},
        {false, "nvhacks", "nvidia", ""},
        {false, "nvhacks", "optimus-force-detection", ""},
        {false, "nvhacks", "optimus-export-enablement", ""},
        {false, "nvhacks", "optimus", ""},
        {false, "nvhacks", "optimus-alt-startup", ""},
        {false, "nvhacks", "optimus-force-windowed", ""},
        {false, "nvhacks", "optimus-dialog-skip", ""},
        {false, "nvhacks", "optimus-startup-option", ""},
        {true, "watchdog", "CR1", "COUNTER_CRASH_CHAIN_BEFORE_ONLINE_GAME"},
        {true, "watchdog", "CR2", "COUNTER_CRASH_CHAIN_BEFORE_LOADING_SCREEN"},
        {true, "watchdog", "CR3", "COUNTER_CRASH_CHAIN_BEFORE_USED_MAIN_MENU"},
        {true, "watchdog", "L0", "SECTION_NOT_CLEAN_GTA_EXIT"},
        {true, "watchdog", "L1", "SECTION_NOT_STARTED_ONLINE_GAME"},
        {true, "watchdog", "L2", "SECTION_NOT_SHOWN_LOADING_SCREEN"},
        {true, "watchdog", "L3", "SECTION_STARTUP_FREEZE"},
        {true, "watchdog", "L4", "SECTION_NOT_USED_MAIN_MENU"},
        {true, "watchdog", "L5", "SECTION_POST_INSTALL"},
        {true, "watchdog", "lastruncrash", ""},
        {true, "watchdog", "preload-upgrades", ""},
        {true, "watchdog", "Q0", "SECTION_IS_QUITTING"},
        {true, "watchdog", "uncleanstop", ""},
        {false, "diagnostics", "send-dumps", ""},
        {true, "diagnostics", "last-minidump-time", ""},
        {true, "diagnostics", "user-confirmed-bsod-time", ""},
        {true, DIAG_MINIDUMP_DETECTED_COUNT, ""},
        {true, DIAG_MINIDUMP_CONFIRMED_COUNT, ""},
        {true, DIAG_PRELOAD_UPGRADES_LOWEST_UNSAFE, ""},
        {false, "general", "noav-user-says-skip", ""},
        {false, "general", "noav-last-asked-time", ""},
    };

    for (uint i = 0; i < NUMELMS(settings); i++)
    {
        SString strValue = GetApplicationSetting(settings[i].szPath, settings[i].szName);
        if (!settings[i].bSkipIfZero || atoi(strValue) != 0)
        {
            WriteDebugEvent(SString("%s.%s: %s %s", settings[i].szPath, settings[i].szName, *strValue, settings[i].szDesc));
        }
    }

    uint uiTimeLastAsked = GetApplicationSettingInt("noav-last-asked-time");
    if (uiTimeLastAsked)
    {
        uint uiTimeNow = static_cast<uint>(time(NULL) / 3600LL);
        uint uiHoursSinceLastAsked = uiTimeNow - uiTimeLastAsked;
        WriteDebugEvent(SString("noav-last-asked-time-hours-delta: %d", uiHoursSinceLastAsked));
    }
}

//////////////////////////////////////////////////////////
//
// WriteDebugEventAndReport
//
// Write to logile.txt and report.log
//
//////////////////////////////////////////////////////////
void WriteDebugEventAndReport(uint uiId, const SString& strText)
{
    WriteDebugEvent(strText);
    AddReportLog(uiId, strText);
}

//////////////////////////////////////////////////////////
//
// PadLeft
//
// Add some spaces to make it look nicer
//
//////////////////////////////////////////////////////////
SString PadLeft(const SString& strText, uint uiNumSpaces, char cCharacter)
{
    SString strPad = std::string(uiNumSpaces, cCharacter);
    return strPad + strText.Replace("\n", SStringX("\n") + strPad);
}

//////////////////////////////////////////////////////////
//
// IsDeviceSelectionDialogOpen
//
// Check if device dialog is currently open in multi-monitor situation
//
//////////////////////////////////////////////////////////
BOOL CALLBACK MyEnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    WINDOWINFO windowInfo;
    if (GetWindowInfo(hwnd, &windowInfo))
    {
        if (windowInfo.atomWindowType == reinterpret_cast<uint>(WC_DIALOG))
        {
            DWORD dwWindowProcessId = 0;
            GetWindowThreadProcessId(hwnd, &dwWindowProcessId);
            if (lParam == dwWindowProcessId)
            {
                SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
                return false;
            }
        }
    }
    return true;
}

bool IsDeviceSelectionDialogOpen(DWORD processID)
{
    return !EnumWindows(MyEnumWindowsProc, processID);
}

//////////////////////////////////////////////////////////
//
// Reg helpers for DeleteCompatibilityEntries
//
//
//////////////////////////////////////////////////////////
LONG RegEnumValueString(HKEY hKey, DWORD dwIndex, WString& strOutName)
{
    wchar_t buf[2048] = {0};
    DWORD   dwBufSizeChars = NUMELMS(buf);
    long    result = RegEnumValueW(hKey, dwIndex, buf, &dwBufSizeChars, 0, NULL, NULL, NULL);
    strOutName = buf;
    return result;
}

LONG RegQueryValueString(HKEY hKey, LPCWSTR lpValueName, WString& strOutData)
{
    wchar_t buf[2048] = {0};
    DWORD   dwBufSizeBytes = sizeof(buf);
    DWORD   dwType = REG_SZ;
    long    result = RegQueryValueExW(hKey, lpValueName, NULL, &dwType, (BYTE*)buf, &dwBufSizeBytes);
    strOutData = buf;
    return result;
}

LONG RegSetValueString(HKEY hKey, LPCWSTR lpValueName, const WString& strData)
{
    DWORD dwSizeChars = strData.length() + 1;
    DWORD dwSizeBytes = dwSizeChars * sizeof(WCHAR);
    return RegSetValueExW(hKey, lpValueName, 0, REG_SZ, (const BYTE*)*strData, dwSizeBytes);
}

//////////////////////////////////////////////////////////
//
// WriteCompatibilityEntries
//
// Based on a story by Towncivilian
//
// Returns false if admin needed
//
//////////////////////////////////////////////////////////
bool WriteCompatibilityEntries(const WString& strProgName, const WString& strSubKey, HKEY hKeyRoot, uint uiFlags, const WString& strNewData)
{
    bool bResult = false;

    // Try open/create key for wrting - Failure means admin is required
    HKEY hKey;
    if (RegCreateKeyExW(hKeyRoot, strSubKey, NULL, NULL, 0, KEY_READ | KEY_WRITE | uiFlags, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        bResult = true;
        if (!strNewData.empty())
        {
            // Write new setting
            if (RegSetValueString(hKey, strProgName, strNewData) != ERROR_SUCCESS)
                bResult = false;
        }
        else
        {
            // No setting, so delete the registry value
            if (RegDeleteValueW(hKey, strProgName) != ERROR_SUCCESS)
                bResult = false;
        }

        RegCloseKey(hKey);
    }

    return bResult;
}

//////////////////////////////////////////////////////////
//
// GetProcessListUsingFile
//
// Returns a list of process ids with a handle a specific file.
//
//////////////////////////////////////////////////////////
std::vector<DWORD> GetProcessListUsingFile(const WString& filePath)
{
    if (!_NtQueryInformationFile)
        return {};

    HANDLE fileHandle = CreateFileW(
        /* FileName            */ filePath.c_str(),
        /* DesiredAccess       */ FILE_READ_ATTRIBUTES,
        /* ShareMode           */ FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        /* SecurityAttributes  */ nullptr,
        /* CreationDisposition */ OPEN_EXISTING,
        /* FlagsAndAttributes  */ 0,
        /* TemplateFile        */ nullptr);

    if (fileHandle == INVALID_HANDLE_VALUE)
        return {};

    HANDLE processHeap = GetProcessHeap();
    SIZE_T heapSize = 8192;
    auto   fileInfo = reinterpret_cast<PFILE_PROCESS_IDS_USING_FILE_INFORMATION>(HeapAlloc(processHeap, HEAP_ZERO_MEMORY, heapSize));

    if (!fileInfo)
    {
        CloseHandle(fileHandle);
        return {};
    }

    IO_STATUS_BLOCK ioStatus{};
    NTSTATUS        status = _NtQueryInformationFile(
        /* FileHandle           */ fileHandle,
        /* IoStatusBlock        */ &ioStatus,
        /* FileInformation      */ fileInfo,
        /* Length               */ heapSize,
        /* FileInformationClass */ 47 /* = FileProcessIdsUsingFileInformation */);

    while (status == STATUS_INFO_LENGTH_MISMATCH)
    {
        heapSize *= 2;
        auto reallocated = reinterpret_cast<PFILE_PROCESS_IDS_USING_FILE_INFORMATION>(HeapReAlloc(processHeap, HEAP_ZERO_MEMORY, fileInfo, heapSize));

        if (!reallocated)
        {
            HeapFree(processHeap, 0, fileInfo);
            CloseHandle(fileHandle);
            return {};
        }

        fileInfo = reallocated;

        status = _NtQueryInformationFile(
            /* FileHandle           */ fileHandle,
            /* IoStatusBlock        */ &ioStatus,
            /* FileInformation      */ fileInfo,
            /* Length               */ heapSize,
            /* FileInformationClass */ 47 /* = FileProcessIdsUsingFileInformation */);
    }

    CloseHandle(fileHandle);
    std::vector<DWORD> result;

    if (NT_SUCCESS(status) && fileInfo->NumberOfProcessIdsInList > 0)
    {
        result.reserve(fileInfo->NumberOfProcessIdsInList);

        for (ULONG i = 0; i < fileInfo->NumberOfProcessIdsInList; i++)
        {
            auto processId = static_cast<DWORD>(fileInfo->ProcessIdList[i]);

            if (processId)
                result.emplace_back(processId);
        }
    }

    HeapFree(processHeap, 0, fileInfo);
    return result;
}

auto ComputeCRC32(const char* filePath) -> uint32_t
{
    CryptoPP::CRC32                                         hash{};
    std::array<CryptoPP::byte, CryptoPP::CRC32::DIGESTSIZE> bytes{};

    try
    {
        CryptoPP::FileSource pass(filePath, true, new CryptoPP::HashFilter(hash, new CryptoPP::ArraySink(bytes.data(), bytes.size())));
    }
    catch (const std::exception&)
    {
        return 0;
    }

    uint32_t result{};
    std::copy_n(bytes.data(), std::min(sizeof(result), bytes.size()), reinterpret_cast<uint8_t*>(&result));
    return result;
};

static constexpr std::string_view alphaNumericCharset{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"};

auto GenerateRandomString(size_t length) -> std::string
{
    if (!length)
        return {};

    std::random_device                      engine;
    std::uniform_int_distribution<uint16_t> distribution{0, static_cast<uint16_t>(alphaNumericCharset.size() - 1)};

    std::array<uint16_t, 4096> bytes{};
    length = std::min(bytes.size(), length);
    std::generate_n(bytes.data(), length, [&] { return distribution(engine); });

    std::string result;
    result.reserve(length);

    for (size_t i = 0; i < length; ++i)
        result.push_back(alphaNumericCharset[bytes[i]]);

    return result;
}

bool IsErrorCodeLoggable(const std::error_code& ec)
{
    switch (ec.value())
    {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            return false;
        default:
            return true;
    }
}

bool IsNativeArm64Host()
{
    static bool isArm64 = ([] {
        HMODULE kernel32 = GetModuleHandleW(L"kernel32.dll");

        if (kernel32)
        {
            BOOL(WINAPI * IsWow64Process2_)(HANDLE, USHORT*, USHORT*) = nullptr;
            IsWow64Process2_ = reinterpret_cast<decltype(IsWow64Process2_)>(GetProcAddress(kernel32, "IsWow64Process2"));

            if (IsWow64Process2_)
            {
                USHORT processMachine;
                USHORT nativeMachine;

                if (IsWow64Process2_(GetCurrentProcess(), &processMachine, &nativeMachine))
                {
                    return nativeMachine == IMAGE_FILE_MACHINE_ARM64;
                }
            }
        }

        return false;
    })();

    return isArm64;
}

bool RegQueryInteger(HKEY rootKey, LPCWSTR keyName, LPCWSTR valueName, DWORD& value)
{
    value = {};

    HKEY key{};
    if (RegOpenKeyExW(rootKey, keyName, 0, KEY_READ, &key) != ERROR_SUCCESS)
        return false;

    DWORD   valueType = REG_DWORD;
    DWORD   valueSize = sizeof(value);
    LSTATUS status = RegQueryValueExW(key, valueName, nullptr, &valueType, reinterpret_cast<LPBYTE>(&value), &valueSize);
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

bool RegWriteInteger(HKEY rootKey, LPCWSTR keyName, LPCWSTR valueName, DWORD value)
{
    HKEY key{};
    if (RegCreateKeyExW(rootKey, keyName, 0, 0, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &key, nullptr) != ERROR_SUCCESS)
        return false;

    LSTATUS status = RegSetValueExW(key, valueName, 0, REG_DWORD, reinterpret_cast<LPBYTE>(&value), sizeof(value));
    RegCloseKey(key);
    return status == ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////
//
// ReadCompatibilityEntries
//
// Returns all compatibility entries in a space delimited string
// Note: Windows 8 can have flag characters for the first field. (Have seen ~ $ and ~$)
//
//////////////////////////////////////////////////////////
WString ReadCompatibilityEntries(const WString& strProgName, const WString& strSubKey, HKEY hKeyRoot, uint uiFlags)
{
    WString strResult;

    // Try read only open - Failure probably means the key does not exist
    HKEY hKey;
    if (RegOpenKeyExW(hKeyRoot, strSubKey, NULL, KEY_READ | uiFlags, &hKey) == ERROR_SUCCESS)
    {
        WString strData;
        if (RegQueryValueString(hKey, strProgName, strData) == ERROR_SUCCESS)
        {
            strResult = strData;
        }

        RegCloseKey(hKey);
    }

    return strResult;
}

//////////////////////////////////////////////////////////
//
// DllMain
//
// Used to save handle to loader dll, so we can get at the resources
//
//////////////////////////////////////////////////////////
int WINAPI DllMain(HINSTANCE hModule, DWORD dwReason, PVOID pvNothing)
{
    g_hInstance = hModule;
    return TRUE;
}
