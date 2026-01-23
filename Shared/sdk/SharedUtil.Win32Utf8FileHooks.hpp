/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        SharedUtil.Win32Utf8FileHooks.hpp
 *  PURPOSE:     Hooks for making Windows file functions use UTF8 strings
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include <shlobj.h>
#include <detours.h>
#include <atomic>
#include <string>

/*
The hooks in this file modify the following functions to work correctly with utf8 strings:

    CreateFile
    LoadLibrary
    LoadLibraryEx
    SetDllDirectory
    SetCurrentDirectory
    AddFontResourceEx
    RemoveFontResourceEx
    RemoveDirectory
    GetDiskFreeSpaceEx
    GetFileAttributes
    SetFileAttributes
    ShellExecute
    CreateDirectory
    CopyFile
    MoveFile
    DeleteFile
    GetModuleHandle

Some more will also work correctly with utf8 strings as they eventually call the above hooked functions

BUT
    * Many other functions are not hooked and will need utf8 conversions when used, like these:
        FindFirstFile
        GetModuleFileName
        GetModuleFileNameEx
        GetCurrentDirectory
        GetDllDirectory
        getcwd
            and many more...
*/

#ifdef UTF8_FILE_HOOKS_PERSONALITY_Core
    #include <filesystem>

extern std::filesystem::path g_gtaDirectory;

/**
 * @brief Converts a filesystem path to a string encoded with the active code page.
 * @param path Path to convert to a string
 */
auto ToACP(const std::filesystem::path& path) -> std::string
{
    if (path.empty())
        return {};

    const std::wstring widePath = path.wstring();
    const int          narrowLength = WideCharToMultiByte(CP_ACP, 0, widePath.data(), static_cast<int>(widePath.size()), nullptr, 0, nullptr, nullptr);
    std::string        narrowPath(narrowLength, 0);
    WideCharToMultiByte(CP_ACP, 0, widePath.data(), static_cast<int>(widePath.size()), narrowPath.data(), narrowLength, nullptr, nullptr);
    return narrowPath;
}
#endif

namespace SharedUtil
{
/////////////////////////////////////////////////////////////
//
// Hook variables
//
/////////////////////////////////////////////////////////////
#define HOOKVAR(name) \
    using FUNC_##name = decltype(&name); \
    FUNC_##name pfn##name;

    HOOKVAR(CreateFileA)
    HOOKVAR(LoadLibraryA)
    HOOKVAR(LoadLibraryExA)
    HOOKVAR(SetDllDirectoryA)
    HOOKVAR(SetCurrentDirectoryA)
    HOOKVAR(AddFontResourceExA)
    HOOKVAR(RemoveFontResourceExA)
    HOOKVAR(RemoveDirectoryA)
    HOOKVAR(GetDiskFreeSpaceExA)
    HOOKVAR(GetFileAttributesA)
    HOOKVAR(SetFileAttributesA)
    HOOKVAR(ShellExecuteA)
    HOOKVAR(CreateDirectoryA)
    HOOKVAR(CopyFileA)
    HOOKVAR(MoveFileA)
    HOOKVAR(DeleteFileA)
    HOOKVAR(GetModuleHandleA)

#ifdef MTA_CLIENT
    // Converts codepage paths to UTF8
    SString MakeSurePathIsUTF8(const SString& strOriginal)
    {
    #ifdef UTF8_FILE_HOOKS_PERSONALITY_Core
        static SString gtaDirCP = ToACP(g_gtaDirectory);
        static SString gtaDirUTF8 = g_gtaDirectory.u8string();
        if (!gtaDirCP.empty() && strOriginal.BeginsWithI(gtaDirCP))
        {
            SString tail = strOriginal.SubStr(gtaDirCP.length());
            return PathJoin(gtaDirUTF8, tail);
        }
    #endif

        struct LaunchPathInfo
        {
            SString cp;
            SString utf8;
            LaunchPathInfo()
            {
                char szLaunchPath[2048] = {};
                GetModuleFileNameA(NULL, szLaunchPath, NUMELMS(szLaunchPath) - 1);
                cp = ExtractPath(szLaunchPath);
                utf8 = GetLaunchPath();
            }
        };
        static LaunchPathInfo launchPath;

        if (!launchPath.cp.empty() && strOriginal.BeginsWithI(launchPath.cp))
        {
            SString strPathTail = strOriginal.SubStr(launchPath.cp.length());
            return PathJoin(launchPath.utf8, strPathTail);
        }

        struct ProfilePathInfo
        {
            SString cp;
            SString utf8;
            ProfilePathInfo()
            {
                char    szProfilePath[MAX_PATH] = "";
                wchar_t wszProfilePath[MAX_PATH] = L"";
                if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, szProfilePath)) &&
                    SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, wszProfilePath)))
                {
                    cp = szProfilePath;
                    utf8 = ToUTF8(wszProfilePath);
                }
            }
        };
        static ProfilePathInfo profilePath;

        if (!profilePath.cp.empty() && strOriginal.BeginsWithI(profilePath.cp))
        {
            SString strPathTail = strOriginal.SubStr(profilePath.cp.length());
            return PathJoin(profilePath.utf8, strPathTail);
        }

        return strOriginal;
    }
#endif

#ifdef MTA_CLIENT

    // Reusable event handle pool for async CreateFile operations
    namespace EventPool
    {
        constexpr size_t           POOL_SIZE = 32;
        static std::atomic<HANDLE> ms_Slots[POOL_SIZE] = {};

        static HANDLE Acquire()
        {
            for (size_t i = 0; i < POOL_SIZE; ++i)
            {
                HANDLE hEvent = ms_Slots[i].exchange(nullptr);
                if (hEvent)
                {
                    ResetEvent(hEvent);
                    return hEvent;
                }
            }
            return CreateEventW(nullptr, TRUE, FALSE, nullptr);
        }

        static void Release(HANDLE hEvent)
        {
            if (!hEvent)
                return;

            for (size_t i = 0; i < POOL_SIZE; ++i)
            {
                HANDLE hExpected = nullptr;
                if (ms_Slots[i].compare_exchange_strong(hExpected, hEvent))
                    return;
            }
            CloseHandle(hEvent);
        }
    }

    // Async CreateFile with timeout to prevent NtCreateFile hangs
    namespace AsyncCreateFile
    {
        enum class EState : int
        {
            Running = 0,
            Completed = 1,
            Abandoned = 2
        };

        constexpr DWORD TIMEOUT_MS = 5000;

        struct SCreateFileParams
        {
            std::wstring          wstrFileName;
            DWORD                 dwAccess = 0;
            DWORD                 dwShareMode = 0;
            LPSECURITY_ATTRIBUTES pSecurity = nullptr;
            DWORD                 dwDisposition = 0;
            DWORD                 dwFlags = 0;
            HANDLE                hTemplateFile = nullptr;
            HANDLE                hResult = INVALID_HANDLE_VALUE;
            DWORD                 dwError = ERROR_SUCCESS;
            HANDLE                hEvent = nullptr;
            std::atomic<int>      state{0};
        };

        constexpr size_t                       POOL_SIZE = 32;
        static std::atomic<SCreateFileParams*> ms_ParamsPool[POOL_SIZE] = {};

        static SCreateFileParams* AcquireParams()
        {
            for (size_t i = 0; i < POOL_SIZE; ++i)
            {
                SCreateFileParams* pParams = ms_ParamsPool[i].exchange(nullptr);
                if (pParams)
                {
                    pParams->state = 0;
                    pParams->hResult = INVALID_HANDLE_VALUE;
                    pParams->dwError = ERROR_SUCCESS;
                    pParams->hEvent = nullptr;
                    pParams->wstrFileName.clear();
                    return pParams;
                }
            }
            return new (std::nothrow) SCreateFileParams();
        }

        static void ReleaseParams(SCreateFileParams* pParams)
        {
            if (!pParams)
                return;

            pParams->wstrFileName.clear();

            for (size_t i = 0; i < POOL_SIZE; ++i)
            {
                SCreateFileParams* pExpected = nullptr;
                if (ms_ParamsPool[i].compare_exchange_strong(pExpected, pParams))
                    return;
            }
            delete pParams;
        }

        static DWORD WINAPI WorkerCallback(LPVOID pArg)
        {
            SCreateFileParams* pParams = static_cast<SCreateFileParams*>(pArg);

            pParams->hResult = CreateFileW(pParams->wstrFileName.c_str(), pParams->dwAccess, pParams->dwShareMode, pParams->pSecurity, pParams->dwDisposition,
                                           pParams->dwFlags, pParams->hTemplateFile);
            pParams->dwError = GetLastError();

            int iExpected = static_cast<int>(EState::Running);
            if (pParams->state.compare_exchange_strong(iExpected, static_cast<int>(EState::Completed)))
            {
                SetEvent(pParams->hEvent);
            }
            else
            {
                EventPool::Release(pParams->hEvent);
                if (pParams->hResult != INVALID_HANDLE_VALUE)
                    CloseHandle(pParams->hResult);
                ReleaseParams(pParams);
            }
            return 0;
        }

        static HANDLE DirectCreateFile(LPCWSTR wszFileName, DWORD dwAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES pSecurity, DWORD dwDisposition,
                                       DWORD dwFlags, HANDLE hTemplate)
        {
            return CreateFileW(wszFileName, dwAccess, dwShareMode, pSecurity, dwDisposition, dwFlags, hTemplate);
        }
    }

    static HANDLE CreateFileWithTimeout(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                        DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
    {
        using namespace AsyncCreateFile;

        if (!IsGTAProcess())
            return DirectCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

        // Skip async if caller provided security attributes or template (lifetime issue)
        if (lpSecurityAttributes || hTemplateFile)
            return DirectCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

        SCreateFileParams* pParams = AcquireParams();
        if (!pParams)
        {
            AddReportLog(6214, "CreateFile timeout: alloc failed");
            return DirectCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        try
        {
            pParams->wstrFileName = lpFileName;
        }
        catch (...)
        {
            AddReportLog(6220, "CreateFile timeout: string copy failed");
            ReleaseParams(pParams);
            return DirectCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        HANDLE hEvent = EventPool::Acquire();
        if (!hEvent)
        {
            AddReportLog(6215, "CreateFile timeout: event failed");
            ReleaseParams(pParams);
            return DirectCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        pParams->dwAccess = dwDesiredAccess;
        pParams->dwShareMode = dwShareMode;
        pParams->pSecurity = lpSecurityAttributes;
        pParams->dwDisposition = dwCreationDisposition;
        pParams->dwFlags = dwFlagsAndAttributes;
        pParams->hTemplateFile = hTemplateFile;
        pParams->hEvent = hEvent;

        if (!QueueUserWorkItem(WorkerCallback, pParams, WT_EXECUTELONGFUNCTION))
        {
            AddReportLog(6218, "CreateFile timeout: queue failed");
            EventPool::Release(hEvent);
            ReleaseParams(pParams);
            return DirectCreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
        }

        if (WaitForSingleObject(hEvent, TIMEOUT_MS) == WAIT_OBJECT_0)
        {
            EventPool::Release(hEvent);
            HANDLE hResult = pParams->hResult;
            DWORD  dwError = pParams->dwError;
            ReleaseParams(pParams);
            SetLastError(dwError);
            return hResult;
        }

        // Timeout occurred
        AddReportLog(6212, SString("CreateFile timed out after %ums: %s", TIMEOUT_MS, ToUTF8(lpFileName).c_str()));

        int iExpected = static_cast<int>(EState::Running);
        if (pParams->state.compare_exchange_strong(iExpected, static_cast<int>(EState::Abandoned)))
        {
            SetLastError(ERROR_TIMEOUT);
        }
        else
        {
            EventPool::Release(hEvent);
            DWORD dwActualError = (pParams->hResult == INVALID_HANDLE_VALUE) ? pParams->dwError : ERROR_TIMEOUT;
            if (pParams->hResult != INVALID_HANDLE_VALUE)
                CloseHandle(pParams->hResult);
            ReleaseParams(pParams);
            SetLastError(dwActualError);
        }
        return INVALID_HANDLE_VALUE;
    }

    // Caches UTF8-to-wide path conversions
    namespace PathCache
    {
        struct CacheEntry
        {
            SString      strKey;
            std::wstring wstrValue;
        };

        constexpr size_t                CACHE_SIZE = 64;
        static std::atomic<CacheEntry*> ms_CacheSlots[CACHE_SIZE] = {};

        static uint32_t ComputeHash(const char* szPath)
        {
            uint32_t uiHash = 5381;
            while (*szPath)
            {
                uiHash = ((uiHash << 5) + uiHash) + static_cast<unsigned char>(*szPath);
                ++szPath;
            }
            return uiHash;
        }

        static const std::wstring* Lookup(const char* szPath)
        {
            uint32_t    uiIndex = ComputeHash(szPath) % CACHE_SIZE;
            CacheEntry* pEntry = ms_CacheSlots[uiIndex].load(std::memory_order_acquire);

            if (pEntry && pEntry->strKey == szPath)
                return &pEntry->wstrValue;

            return nullptr;
        }

        static void Store(const char* szPath, const std::wstring& wstrWide)
        {
            uint32_t uiIndex = ComputeHash(szPath) % CACHE_SIZE;

            if (ms_CacheSlots[uiIndex].load(std::memory_order_relaxed))
                return;

            CacheEntry* pEntry = new (std::nothrow) CacheEntry();
            if (!pEntry)
                return;

            try
            {
                pEntry->strKey = szPath;
                pEntry->wstrValue = wstrWide;
            }
            catch (...)
            {
                delete pEntry;
                return;
            }

            CacheEntry* pExpected = nullptr;
            if (!ms_CacheSlots[uiIndex].compare_exchange_strong(pExpected, pEntry, std::memory_order_release))
                delete pEntry;
        }

        static const std::wstring& GetExePath()
        {
            static const std::wstring wstrExePath = FromUTF8(GetLaunchPathFilename());
            return wstrExePath;
        }

        static const std::wstring& GetExeName()
        {
            static const std::wstring wstrExeName = FromUTF8(GetLaunchFilename());
            return wstrExeName;
        }

        // Strips \\?\ and \\?\UNC\ prefixes
        static std::wstring StripExtendedPrefix(const std::wstring& wstrPath)
        {
            if (wstrPath.size() < 4)
                return wstrPath;

            if (wstrPath[0] != L'\\' || wstrPath[1] != L'\\' || wstrPath[2] != L'?' || wstrPath[3] != L'\\')
                return wstrPath;

            if (wstrPath.size() >= 8)
            {
                bool bIsUNC = (wstrPath[4] == L'U' || wstrPath[4] == L'u') && (wstrPath[5] == L'N' || wstrPath[5] == L'n') &&
                              (wstrPath[6] == L'C' || wstrPath[6] == L'c') && wstrPath[7] == L'\\';

                if (bIsUNC)
                    return L"\\" + wstrPath.substr(8);
            }

            return wstrPath.substr(4);
        }

        static std::wstring GetFullPath(const std::wstring& wstrPath)
        {
            if (wstrPath.empty())
                return std::wstring();

            DWORD dwNeeded = GetFullPathNameW(wstrPath.c_str(), 0, nullptr, nullptr);
            if (dwNeeded == 0)
                return wstrPath;

            std::wstring wstrResult(dwNeeded, L'\0');
            DWORD        dwWritten = GetFullPathNameW(wstrPath.c_str(), dwNeeded, &wstrResult[0], nullptr);

            if (dwWritten > 0)
                wstrResult.resize(dwWritten);
            else
                wstrResult = wstrPath;

            return wstrResult;
        }

        static std::wstring ExtractDirectory(const std::wstring& wstrPath)
        {
            std::wstring wstrStripped = StripExtendedPrefix(wstrPath);
            size_t       uiPos = wstrStripped.find_last_of(L"\\/");

            if (uiPos == std::wstring::npos)
                return std::wstring();

            return wstrStripped.substr(0, uiPos + 1);
        }

        static const std::wstring& GetExeDir()
        {
            static const std::wstring wstrExeDir = ExtractDirectory(GetExePath());
            return wstrExeDir;
        }

        static const std::wstring& GetExeDirShort()
        {
            struct Holder
            {
                std::wstring wstrValue;
                Holder()
                {
                    std::wstring wstrDir = GetExeDir();
                    if (wstrDir.empty())
                        return;

                    if (wstrDir.back() == L'\\' || wstrDir.back() == L'/')
                        wstrDir.pop_back();

                    if (wstrDir.empty())
                    {
                        wstrValue = GetExeDir();
                        return;
                    }

                    DWORD dwNeeded = GetShortPathNameW(wstrDir.c_str(), nullptr, 0);
                    if (dwNeeded == 0)
                    {
                        wstrValue = GetExeDir();
                        return;
                    }

                    std::wstring wstrBuffer(dwNeeded, L'\0');
                    DWORD        dwWritten = GetShortPathNameW(wstrDir.c_str(), &wstrBuffer[0], dwNeeded);
                    if (dwWritten == 0)
                    {
                        wstrValue = GetExeDir();
                        return;
                    }

                    wstrBuffer.resize(dwWritten);
                    wstrValue = wstrBuffer + L"\\";
                }
            };
            static Holder holder;
            return holder.wstrValue;
        }

        static const std::wstring& GetExeShort()
        {
            struct Holder
            {
                std::wstring wstrValue;
                Holder()
                {
                    wstrValue = GetExeName();

                    const std::wstring& wstrFullPath = GetExePath();
                    if (wstrFullPath.empty())
                        return;

                    DWORD dwNeeded = GetShortPathNameW(wstrFullPath.c_str(), nullptr, 0);
                    if (dwNeeded == 0)
                        return;

                    std::wstring wstrBuffer(dwNeeded, L'\0');
                    DWORD        dwWritten = GetShortPathNameW(wstrFullPath.c_str(), &wstrBuffer[0], dwNeeded);
                    if (dwWritten == 0)
                        return;

                    wstrBuffer.resize(dwWritten);
                    size_t uiPos = wstrBuffer.find_last_of(L"\\/");

                    if (uiPos == std::wstring::npos)
                        wstrValue = wstrBuffer;
                    else
                        wstrValue = wstrBuffer.substr(uiPos + 1);
                }
            };
            static Holder holder;
            return holder.wstrValue;
        }

        // Checks if path refers to the GTA executable (handles 8.3 names and trailing junk)
        static bool IsExe(const std::wstring& wstrPath)
        {
            if (wstrPath.empty())
                return false;

            const std::wstring& wstrExePath = GetExePath();
            if (wstrExePath.empty())
                return false;

            if (_wcsicmp(wstrPath.c_str(), wstrExePath.c_str()) == 0)
                return true;

            size_t         uiPos = wstrPath.find_last_of(L"\\/");
            const wchar_t* wszName = (uiPos == std::wstring::npos) ? wstrPath.c_str() : wstrPath.c_str() + uiPos + 1;

            size_t uiLen = wcslen(wszName);
            while (uiLen > 0 && (wszName[uiLen - 1] == L' ' || wszName[uiLen - 1] == L'.'))
                --uiLen;

            if (uiLen == 0)
                return false;

            const std::wstring& wstrExeName = GetExeName();
            if (wstrExeName.size() == uiLen && _wcsnicmp(wszName, wstrExeName.c_str(), uiLen) == 0)
                return true;

            const std::wstring& wstrExeShort = GetExeShort();
            if (wstrExeShort.size() == uiLen && _wcsnicmp(wszName, wstrExeShort.c_str(), uiLen) == 0)
                return true;

            return false;
        }
    }
#endif

    HANDLE
    WINAPI
    MyCreateFileA(__in LPCSTR lpFileName, __in DWORD dwDesiredAccess, __in DWORD dwShareMode, __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                  __in DWORD dwCreationDisposition, __in DWORD dwFlagsAndAttributes, __in_opt HANDLE hTemplateFile)
    {
        if (!lpFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return INVALID_HANDLE_VALUE;
        }

#ifdef MTA_CLIENT
        if (IsGTAProcess())
        {
            const std::wstring* pCached = PathCache::Lookup(lpFileName);
            if (pCached)
            {
                if (PathCache::IsExe(*pCached))
                    return CreateFileWithTimeout(pCached->c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                                                 dwFlagsAndAttributes, hTemplateFile);

                return CreateFileW(pCached->c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,
                                   hTemplateFile);
            }

            SString      strFileName = MakeSurePathIsUTF8(lpFileName);
            std::wstring wstrWidePath = FromUTF8(strFileName);
            PathCache::Store(lpFileName, wstrWidePath);

            if (PathCache::IsExe(wstrWidePath))
                return CreateFileWithTimeout(wstrWidePath.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
                                             dwFlagsAndAttributes, hTemplateFile);

            return CreateFileW(wstrWidePath.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,
                               hTemplateFile);
        }
#endif
        SString strFileName = lpFileName;
        return CreateFileW(FromUTF8(strFileName), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes,
                           hTemplateFile);
    }

    HMODULE
    WINAPI
    MyLoadLibraryA(__in LPCSTR lpLibFileName)
    {
        if (!lpLibFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        return LoadLibraryW(FromUTF8(lpLibFileName));
    }

    HMODULE
    WINAPI
    MyLoadLibraryExA(__in LPCSTR lpLibFileName, __reserved HANDLE hFile, __in DWORD dwFlags)
    {
        if (!lpLibFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        return LoadLibraryExW(FromUTF8(lpLibFileName), hFile, dwFlags);
    }

    BOOL WINAPI MySetDllDirectoryA(__in_opt LPCSTR lpPathName)
    {
        if (!lpPathName)
            return SetDllDirectoryW(NULL);
        return SetDllDirectoryW(FromUTF8(lpPathName));
    }

    BOOL WINAPI MySetCurrentDirectoryA(__in LPCSTR lpPathName)
    {
        if (!lpPathName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        SString strPathName = lpPathName;
#ifdef MTA_CLIENT
        if (IsGTAProcess())
            strPathName = MakeSurePathIsUTF8(strPathName);
#endif
        return SetCurrentDirectoryW(FromUTF8(strPathName));
    }

    int WINAPI MyAddFontResourceExA(__in LPCSTR name, __in DWORD fl, __reserved PVOID res)
    {
        if (!name)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return 0;
        }
        return AddFontResourceExW(FromUTF8(name), fl, res);
    }

    BOOL WINAPI MyRemoveFontResourceExA(__in LPCSTR name, __in DWORD fl, __reserved PVOID pdv)
    {
        if (!name)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        return RemoveFontResourceExW(FromUTF8(name), fl, pdv);
    }

    BOOL WINAPI MyRemoveDirectoryA(__in LPCSTR lpPathName)
    {
        if (!lpPathName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        SString strPathName = lpPathName;
#ifdef MTA_CLIENT
        if (IsGTAProcess())
            strPathName = MakeSurePathIsUTF8(lpPathName);
#endif
        return RemoveDirectoryW(FromUTF8(strPathName));
    }

    BOOL WINAPI MyGetDiskFreeSpaceExA(__in_opt LPCSTR lpDirectoryName, __out_opt PULARGE_INTEGER lpFreeBytesAvailableToCaller,
                                      __out_opt PULARGE_INTEGER lpTotalNumberOfBytes, __out_opt PULARGE_INTEGER lpTotalNumberOfFreeBytes)
    {
        if (!lpDirectoryName)
            return GetDiskFreeSpaceExW(NULL, lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
        return GetDiskFreeSpaceExW(FromUTF8(lpDirectoryName), lpFreeBytesAvailableToCaller, lpTotalNumberOfBytes, lpTotalNumberOfFreeBytes);
    }

    DWORD
    WINAPI
    MyGetFileAttributesA(__in LPCSTR lpFileName)
    {
        if (!lpFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return INVALID_FILE_ATTRIBUTES;
        }
        SString strFileName = lpFileName;
#ifdef MTA_CLIENT
        if (IsGTAProcess())
            strFileName = MakeSurePathIsUTF8(strFileName);
#endif
        return GetFileAttributesW(FromUTF8(strFileName));
    }

    BOOL WINAPI MySetFileAttributesA(__in LPCSTR lpFileName, __in DWORD dwFileAttributes)
    {
        if (!lpFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        SString strFileName = lpFileName;
#ifdef MTA_CLIENT
        if (IsGTAProcess())
            strFileName = MakeSurePathIsUTF8(strFileName);
#endif
        return SetFileAttributesW(FromUTF8(strFileName), dwFileAttributes);
    }

    HINSTANCE STDAPICALLTYPE MyShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd)
    {
        if (!lpFile)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return reinterpret_cast<HINSTANCE>(SE_ERR_FNF);
        }
        return ShellExecuteW(hwnd, lpOperation ? FromUTF8(lpOperation).c_str() : NULL, FromUTF8(lpFile), lpParameters ? FromUTF8(lpParameters).c_str() : NULL,
                             lpDirectory ? FromUTF8(lpDirectory).c_str() : NULL, nShowCmd);
    }

    BOOL WINAPI MyCreateDirectoryA(__in LPCSTR lpPathName, __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes)
    {
        if (!lpPathName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        SString strPathName = lpPathName;
#ifdef MTA_CLIENT
        if (IsGTAProcess())
            strPathName = MakeSurePathIsUTF8(strPathName);
#endif
        return CreateDirectoryW(FromUTF8(strPathName), lpSecurityAttributes);
    }

    BOOL WINAPI MyCopyFileA(__in LPCSTR lpExistingFileName, __in LPCSTR lpNewFileName, __in BOOL bFailIfExists)
    {
        if (!lpExistingFileName || !lpNewFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        return CopyFileW(FromUTF8(lpExistingFileName), FromUTF8(lpNewFileName), bFailIfExists);
    }

    BOOL WINAPI MyMoveFileA(__in LPCSTR lpExistingFileName, __in LPCSTR lpNewFileName)
    {
        if (!lpExistingFileName || !lpNewFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        return MoveFileW(FromUTF8(lpExistingFileName), FromUTF8(lpNewFileName));
    }

    BOOL WINAPI MyDeleteFileA(__in LPCSTR lpFileName)
    {
        if (!lpFileName)
        {
            SetLastError(ERROR_INVALID_PARAMETER);
            return FALSE;
        }
        SString strFileName = lpFileName;
#ifdef MTA_CLIENT
        if (IsGTAProcess())
            strFileName = MakeSurePathIsUTF8(strFileName);
#endif
        return DeleteFileW(FromUTF8(strFileName));
    }

    HMODULE
    WINAPI
    MyGetModuleHandleA(__in_opt LPCSTR lpModuleName)
    {
        if (!lpModuleName)
            return GetModuleHandleW(NULL);
        return GetModuleHandleW(FromUTF8(lpModuleName));
    }

    /////////////////////////////////////////////////////////////
    //
    // Hook adding
    //
    /////////////////////////////////////////////////////////////
    void AddUtf8FileHooks()
    {
#define ADDHOOK(module, name) \
    static_assert(std::is_same_v<decltype(pfn##name), decltype(&name)>, "invalid type of " MTA_STR(pfn##name)); \
    pfn##name = reinterpret_cast<decltype(pfn##name)>(DetourFindFunction(module, #name)); \
    DetourAttach(&reinterpret_cast<PVOID&>(pfn##name), My##name); \
    assert(pfn##name);

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        ADDHOOK("Kernel32.dll", CreateFileA)
        ADDHOOK("Kernel32.dll", LoadLibraryA)
        ADDHOOK("Kernel32.dll", LoadLibraryExA)
        ADDHOOK("Kernel32.dll", SetDllDirectoryA)
        ADDHOOK("Kernel32.dll", SetCurrentDirectoryA)
        ADDHOOK("Gdi32.dll", AddFontResourceExA)
        ADDHOOK("Gdi32.dll", RemoveFontResourceExA)
        ADDHOOK("Kernel32.dll", RemoveDirectoryA)
        ADDHOOK("Kernel32.dll", GetDiskFreeSpaceExA)
        ADDHOOK("Kernel32.dll", GetFileAttributesA)
        ADDHOOK("Kernel32.dll", SetFileAttributesA)
        ADDHOOK("Shell32.dll", ShellExecuteA)
        ADDHOOK("Kernel32.dll", CreateDirectoryA)
        ADDHOOK("Kernel32.dll", CopyFileA)
        ADDHOOK("Kernel32.dll", MoveFileA)
        ADDHOOK("Kernel32.dll", DeleteFileA)
        ADDHOOK("Kernel32.dll", GetModuleHandleA)

        DetourTransactionCommit();
    }

    /////////////////////////////////////////////////////////////
    //
    // Hook taking away
    //
    /////////////////////////////////////////////////////////////
    void RemoveUtf8FileHooks()
    {
#define DELHOOK(name) \
    if (pfn##name != nullptr) \
    { \
        DetourDetach(&reinterpret_cast<PVOID&>(pfn##name), My##name); \
        pfn##name = nullptr; \
    }

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DELHOOK(CreateFileA)
        DELHOOK(LoadLibraryA)
        DELHOOK(LoadLibraryExA)
        DELHOOK(SetDllDirectoryA)
        DELHOOK(SetCurrentDirectoryA)
        DELHOOK(AddFontResourceExA)
        DELHOOK(RemoveFontResourceExA)
        DELHOOK(RemoveDirectoryA)
        DELHOOK(GetDiskFreeSpaceExA)
        DELHOOK(GetFileAttributesA)
        DELHOOK(SetFileAttributesA)
        DELHOOK(ShellExecuteA)
        DELHOOK(CreateDirectoryA)
        DELHOOK(CopyFileA)
        DELHOOK(MoveFileA)
        DELHOOK(DeleteFileA)
        DELHOOK(GetModuleHandleA)

        DetourTransactionCommit();
    }
}  // namespace SharedUtil
