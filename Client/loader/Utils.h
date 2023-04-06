/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Utils.h
 *  PURPOSE:     Loading utilities
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <filesystem>

#undef CREATE_SUSPENDED
#define CREATE_SUSPENDED 5

extern HINSTANCE g_hInstance;

enum ePathResult
{
    GAME_PATH_UNICODE_CHARS,
    GAME_PATH_MISSING,
    GAME_PATH_OK,
};

enum
{
    CHECK_SERVICE_POST_INSTALL = 1,
    CHECK_SERVICE_PRE_UNINSTALL = 2,
    CHECK_SERVICE_POST_UPDATE = 3,
    CHECK_SERVICE_PRE_GAME = 4,
    CHECK_SERVICE_POST_GAME = 5,
    CHECK_SERVICE_PRE_CREATE = 6,
    CHECK_SERVICE_POST_CREATE = 7,
    CHECK_SERVICE_RESTART_GAME = 8,
};

struct SOSVersionInfo
{
    DWORD dwMajor;
    DWORD dwMinor;
    DWORD dwBuild;
};

enum class EGtaFileVersion
{
    Unknown,
    US,
    EU,
    Encrypted,
};

struct SPEFileOffsets
{
    uint TimeDateStamp;
    uint Characteristics;
    uint AddressOfEntryPoint;
    uint DllCharacteristics;
    struct
    {
        uint PointerToRawData;
    } sections[1];
};

void TerminateGTAIfRunning();
bool IsGTARunning();
void TerminateOtherMTAIfRunning();
bool IsOtherMTARunning();

std::vector<DWORD> GetGTAProcessList();

bool CommandLineContains(const SString& strText);
void DisplayErrorMessageBox(const SString& strMessage, const SString& strErrorCode = "", const SString& strTroubleType = "");

auto GetMTARootDirectory() -> std::filesystem::path;
auto GetGameBaseDirectory() -> std::filesystem::path;
auto GetGameLaunchDirectory() -> std::filesystem::path;
auto GetGameExecutablePath() -> std::filesystem::path;

void            SetMTASAPathSource(bool bReadFromRegistry);
SString         GetMTASAPath();
ePathResult     DiscoverGTAPath(bool bFindIfMissing);
SString         GetGTAPath();
bool            HasGTAPath();
EGtaFileVersion GetGtaFileVersion(const SString& strGTAEXEPath);
void            GetPEFileOffsets(SPEFileOffsets& outOffsets, const SString& strGTAEXEPath);

void           FindFilesRecursive(const SString& strPathMatch, std::vector<SString>& outFileList, uint uiMaxDepth = 99);
SOSVersionInfo GetOSVersion();
SOSVersionInfo GetRealOSVersion();
bool           IsWindows10OrGreater();
bool           IsWindows10Threshold2OrGreater();

BOOL IsUserAdmin();
void RelaunchAsAdmin(const SString& strCmdLine, const SString& strReason);

void UpdateMTAVersionApplicationSetting(bool bQuiet = false);
bool Is32bitProcess(DWORD processID);
bool TerminateProcess(DWORD dwProcessID, uint uiExitCode = 0);

bool CreateSingleInstanceMutex();
void ReleaseSingleInstanceMutex();

void CleanDownloadCache();

HMODULE GetLibraryHandle(const SString& strFilename, DWORD* pdwOutLastError = NULL);
void    FreeLibraryHandle();
uint    WaitForObject(HANDLE hProcess, HANDLE hThread, DWORD dwMilliseconds, HANDLE hMutex);
bool    CheckService(uint uiStage);

void MaybeShowCopySettingsDialog();

bool CheckAndShowFileOpenFailureMessage();
void CheckAndShowMissingFileMessage();
void CheckAndShowModelProblems();
void CheckAndShowUpgradeProblems();
void CheckAndShowImgProblems();

void               BsodDetectionPreLaunch();
void               BsodDetectionOnGameBegin();
void               BsodDetectionOnGameEnd();
void               ForbodenProgramsMessage();
bool               VerifyEmbeddedSignature(const SString& strFilename);
void               LogSettings();
SString            PadLeft(const SString& strText, uint uiNumSpaces, char cCharacter);
bool               IsDeviceSelectionDialogOpen(DWORD processID);
std::vector<DWORD> MyEnumProcesses(bool bInclude64bit = false, bool bIncludeCurrent = false);
SString            GetProcessPathFilename(DWORD processID);
SString            GetProcessFilename(DWORD processID);
void               WriteDebugEventAndReport(uint uiId, const SString& strText);
WString            ReadCompatibilityEntries(const WString& strProgName, const WString& strSubKey, HKEY hKeyRoot, uint uiFlags);
bool               WriteCompatibilityEntries(const WString& strProgName, const WString& strSubKey, HKEY hKeyRoot, uint uiFlags, const WString& strNewData);
std::vector<DWORD> GetProcessListUsingFile(const WString& filePath);

/**
 * @brief Computes the CRC-32 checksum for a file.
 */
auto ComputeCRC32(const char* filePath) -> uint32_t;

/**
 * @brief Generates a random string with up to 4096 characters.
 * @param length Desired length of the string
 * @return A string with random alpha-numeric characters
 */
auto GenerateRandomString(size_t length) -> std::string;

/**
 * @brief Checks if the error code is important enough to be logged.
 */
bool IsErrorCodeLoggable(const std::error_code& ec);

/**
 * @brief Returns true if Windows is running on ARM64 architecture (via emulation).
*/
bool IsNativeArm64Host();

// Return false on read failure
template <class T>
bool ReadFileValue(const SString& strFilename, T& value, uint uiOffset)
{
    std::ifstream file(FromUTF8(strFilename), std::ios::binary);
    if (file)
    {
        file.seekg(uiOffset);
        file.read((char*)&value, sizeof(T));
    }
    return !file.fail();
}

// Return false on write failure
template <class T>
bool WriteFileValue(const SString& strFilename, const T& value, uint uiOffset)
{
    SetFileAttributes(strFilename, FILE_ATTRIBUTE_NORMAL);
    std::fstream file(FromUTF8(strFilename), std::ios::in | std::ios::out | std::ios::binary);
    if (file)
    {
        file.seekp(uiOffset);
        file.write((const char*)&value, sizeof(T));
    }
    return !file.fail();
}

//
// Determine if game process has gone wonky
//
class CStuckProcessDetector
{
public:
    CStuckProcessDetector(HANDLE hProcess, uint uiThresholdMs)
    {
        m_hProcess = hProcess;
        m_uiThresholdMs = uiThresholdMs;
        m_PrevWorkingSetSize = 0;
    }

    // Returns true if process is active and hasn't changed mem usage for uiThresholdMs
    bool UpdateIsStuck()
    {
        PROCESS_MEMORY_COUNTERS psmemCounter;
        BOOL                    bResult = GetProcessMemoryInfo(m_hProcess, &psmemCounter, sizeof(PROCESS_MEMORY_COUNTERS));
        if (!bResult)
            return false;

        if (m_PrevWorkingSetSize != psmemCounter.WorkingSetSize)
        {
            m_PrevWorkingSetSize = psmemCounter.WorkingSetSize;
            m_StuckTimer.Reset();
        }
        return m_StuckTimer.Get() > m_uiThresholdMs;
    }

    uint         m_uiThresholdMs;
    SIZE_T       m_PrevWorkingSetSize;
    HANDLE       m_hProcess;
    CElapsedTime m_StuckTimer;
};

// For NtQuerySystemInformation
#define STATUS_INFO_LENGTH_MISMATCH    ((NTSTATUS)0xC0000004L)
#define SystemProcessImageNameInformation ((SYSTEM_INFORMATION_CLASS)88)
typedef struct _SYSTEM_PROCESS_IMAGE_NAME_INFORMATION
{
    HANDLE         ProcessId;
    UNICODE_STRING ImageName;
} SYSTEM_PROCESS_IMAGE_NAME_INFORMATION, *PSYSTEM_PROCESS_IMAGE_NAME_INFORMATION;

// For NtQueryInformationFile
typedef struct _FILE_PROCESS_IDS_USING_FILE_INFORMATION
{
    ULONG     NumberOfProcessIdsInList;
    ULONG_PTR ProcessIdList[1];
} FILE_PROCESS_IDS_USING_FILE_INFORMATION, *PFILE_PROCESS_IDS_USING_FILE_INFORMATION;

NTSTATUS NTAPI NtQueryInformationFile(HANDLE FileHandle, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG Length,
                                      UINT FileInformationClass /* FILE_INFORMATION_CLASS */);

void* LoadFunction(const char* libraryName, const char* functionName);

#define _DEFFUNCTION(lib, name) \
    using FUNC_##name = decltype(&name); \
    inline FUNC_##name __##name(void) \
    { \
        static FUNC_##name pfn = NULL; \
        if (!pfn) \
            pfn = (FUNC_##name)LoadFunction(lib, #name); \
        return pfn; \
    }

#define DEFFUNCTION(lib, name) _DEFFUNCTION(lib, name)

#define _NtQuerySystemInformation __NtQuerySystemInformation()
#define _NtQueryInformationFile   __NtQueryInformationFile()

DEFFUNCTION("ntdll", NtQuerySystemInformation)
DEFFUNCTION("ntdll", NtQueryInformationFile)
