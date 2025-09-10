/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        launch/Main.cpp
 *  PURPOSE:     Unchanging .exe that doesn't change (but it did)
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <windows.h>
#include <psapi.h>               // For GetModuleInformation
#include <shlwapi.h>             // For PathCanonicalize
#include <strsafe.h>             // For safe string operations
#include <wintrust.h>            // For WinVerifyTrust
#include <softpub.h>             // For WINTRUST_ACTION_GENERIC_VERIFY_V2
#include <malloc.h>              // For _malloca
#include <string>                // For std::wstring
#include <intrin.h>              // For __readfsdword, __readgsqword
#include <version.h>

// Link to the required libraries
#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "wintrust.lib")

/*
    IMPORTANT

    If this project changes, a new release build should be copied into
    the Shared\data\launchers directory.

    The .exe in Shared\data\launchers will be used by the installer and updater.

    (set flag.new_client_exe on the build server to generate new exe)
*/

// Enable security features at compile time
#ifdef _MSC_VER
    #pragma strict_gs_check(on)

    #if defined(_M_IX86) || defined(_M_X64) || defined(_M_AMD64)
        #pragma comment(linker, "/guard:cf")
    #endif

    #if defined(_M_IX86)
        #pragma check_stack(on)
    #endif
#endif

// Ensure proper struct packing for x86/x64 compatibility
#pragma pack(push, 8)

// Define constants for older SDKs
#ifndef LOAD_WITH_ALTERED_SEARCH_PATH
    #define LOAD_WITH_ALTERED_SEARCH_PATH 0x00000008
#endif

#ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
    #define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR 0x00000100
#endif

#ifndef LOAD_LIBRARY_SEARCH_SYSTEM32
    #define LOAD_LIBRARY_SEARCH_SYSTEM32 0x00000800
#endif

#ifndef PROCESS_DEP_ENABLE
    #define PROCESS_DEP_ENABLE 0x00000001
#endif

#ifndef TRUST_E_NOSIGNATURE
    #define TRUST_E_NOSIGNATURE ((HRESULT)0x800B0100L)
#endif

#ifndef MAXULONG_PTR
    #if defined(_WIN64) || defined(_M_X64) || defined(_M_AMD64)
        #define MAXULONG_PTR 0xffffffffffffffffULL
    #else
        #define MAXULONG_PTR 0xffffffffUL
    #endif
#endif

// Forward declare process mitigation types for Windows 8+ features
// These are conditionally used at runtime if available
struct PROCESS_MITIGATION_ASLR_POLICY_LOCAL
{
    union
    {
        DWORD Flags;
        struct
        {
            DWORD EnableBottomUpRandomization : 1;
            DWORD EnableForceRelocateImages : 1;
            DWORD EnableHighEntropy : 1;
            DWORD DisallowStrippedImages : 1;
            DWORD ReservedFlags : 28;
        };
    };
};

struct PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY_LOCAL
{
    union
    {
        DWORD Flags;
        struct
        {
            DWORD EnableControlFlowGuard : 1;
            DWORD EnableExportSuppression : 1;
            DWORD StrictMode : 1;
            DWORD ReservedFlags : 29;
        };
    };
};

struct PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY_LOCAL
{
    union
    {
        DWORD Flags;
        struct
        {
            DWORD RaiseExceptionOnInvalidHandleReference : 1;
            DWORD HandleExceptionsPermanentlyEnabled : 1;
            DWORD ReservedFlags : 30;
        };
    };
};

// Security constants
namespace SecurityConstants
{
    constexpr size_t MAX_PATH_LENGTH = 260U;
    constexpr size_t MAX_FILENAME_LENGTH = 255U;
    constexpr size_t MAX_CMDLINE_LENGTH = 32768U;
    constexpr DWORD  MAX_MODULE_SIZE = 100U * 1024U * 1024U;
    constexpr DWORD  MIN_MODULE_SIZE = 1024U;
    constexpr DWORD  CRITICAL_SECTION_SPIN_COUNT = 0x00000400;
    constexpr DWORD  PEB_DEBUG_FLAGS = 0x70;            // FLG_HEAP_ENABLE_TAIL_CHECK | FLG_HEAP_ENABLE_FREE_CHECK | FLG_HEAP_VALIDATE_PARAMETERS

#if defined(_M_IX86) || (defined(_WIN32) && !defined(_WIN64))
    constexpr DWORD MAX_ENV_VAR_LENGTH = 8192U;            // 8KB for x86
#else
    constexpr DWORD MAX_ENV_VAR_LENGTH = 32767U;            // 32KB for x64
#endif

    constexpr size_t STACK_ALLOC_LIMIT = 65536U;            // Stack allocation limit
}            // namespace SecurityConstants

// Critical section for thread safety
namespace
{
#if defined(_M_IX86)
    __declspec(align(8)) CRITICAL_SECTION g_DllLoadCS = {};
#else
    CRITICAL_SECTION g_DllLoadCS = {};
#endif
    volatile LONG g_lSecurityInitialized = 0;
    volatile LONG g_lCriticalSectionInitialized = 0;
}            // anonymous namespace

// Helper to get heap termination flag
[[nodiscard]] inline constexpr HEAP_INFORMATION_CLASS GetHeapTerminationOnCorruption() noexcept
{
    return static_cast<HEAP_INFORMATION_CLASS>(1);
}

// RAII wrapper for heap memory
class HeapMemory final
{
public:
    explicit HeapMemory(size_t size) noexcept : m_pMemory(::HeapAlloc(::GetProcessHeap(), HEAP_ZERO_MEMORY, size)) {}

    ~HeapMemory() noexcept
    {
        if (m_pMemory != nullptr)
        {
            static_cast<void>(::HeapFree(::GetProcessHeap(), 0, m_pMemory));
        }
    }

    HeapMemory(const HeapMemory&) = delete;
    HeapMemory& operator=(const HeapMemory&) = delete;
    HeapMemory(HeapMemory&&) = delete;
    HeapMemory& operator=(HeapMemory&&) = delete;

    [[nodiscard]] void*       Get() noexcept { return m_pMemory; }
    [[nodiscard]] const void* Get() const noexcept { return m_pMemory; }
    [[nodiscard]] bool        IsValid() const noexcept { return m_pMemory != nullptr; }

private:
    void* m_pMemory;
};

namespace Security
{
    // Forward declarations
    void CleanupSecurity() noexcept;

    // Enable DEP for process
    void EnableDEP() noexcept
    {
        typedef BOOL(WINAPI * PFN_SetProcessDEPPolicy)(DWORD);
        const HMODULE hKernel32 = ::GetModuleHandleW(L"kernel32.dll");
        if (hKernel32 != nullptr)
        {
            const auto pfnSetDEP = reinterpret_cast<PFN_SetProcessDEPPolicy>(static_cast<void*>(::GetProcAddress(hKernel32, "SetProcessDEPPolicy")));
            if (pfnSetDEP != nullptr)
            {
                static_cast<void>(pfnSetDEP(PROCESS_DEP_ENABLE));
            }
        }
    }

    // Enable process mitigation policies (Windows 8+ only)
    void EnableMitigationPolicies() noexcept
    {
        typedef BOOL(WINAPI * PFN_SetProcessMitigationPolicy)(INT, PVOID, SIZE_T);
        const HMODULE hKernel32 = ::GetModuleHandleW(L"kernel32.dll");
        if (hKernel32 == nullptr)
            return;

        const auto pfnSetMitigation =
            reinterpret_cast<PFN_SetProcessMitigationPolicy>(static_cast<void*>(::GetProcAddress(hKernel32, "SetProcessMitigationPolicy")));
        if (pfnSetMitigation == nullptr)
            return;            // Windows 7 and earlier don't have this function

        // Use local structures to avoid SDK conflicts
        // ProcessControlFlowGuardPolicy = 7
        {
            PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY_LOCAL cfgPolicy = {};
            cfgPolicy.EnableControlFlowGuard = TRUE;
            cfgPolicy.StrictMode = TRUE;
            static_cast<void>(pfnSetMitigation(7, &cfgPolicy, sizeof(cfgPolicy)));
        }

        // ProcessASLRPolicy = 1
        {
            PROCESS_MITIGATION_ASLR_POLICY_LOCAL aslrPolicy = {};
            aslrPolicy.EnableBottomUpRandomization = TRUE;
            aslrPolicy.EnableForceRelocateImages = TRUE;
#if defined(_M_X64) || defined(_WIN64) || defined(_M_AMD64)
            aslrPolicy.EnableHighEntropy = TRUE;
#endif
            static_cast<void>(pfnSetMitigation(1, &aslrPolicy, sizeof(aslrPolicy)));
        }

        // ProcessStrictHandleCheckPolicy = 3
        {
            PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY_LOCAL handlePolicy = {};
            handlePolicy.RaiseExceptionOnInvalidHandleReference = TRUE;
            handlePolicy.HandleExceptionsPermanentlyEnabled = TRUE;
            static_cast<void>(pfnSetMitigation(3, &handlePolicy, sizeof(handlePolicy)));
        }
    }

    // Initialize security context
    bool InitializeSecurity() noexcept
    {
        // Ensure single initialization with interlocked operation
        if (::InterlockedCompareExchange(&g_lSecurityInitialized, 1L, 0L) != 0L)
            return true;            // Already initialized

        // Initialize critical section
        if (::InterlockedCompareExchange(&g_lCriticalSectionInitialized, 1L, 0L) == 0L)
        {
            // Use InitializeCriticalSectionAndSpinCount for better reliability
            if (!::InitializeCriticalSectionAndSpinCount(&g_DllLoadCS, SecurityConstants::CRITICAL_SECTION_SPIN_COUNT))
            {
                static_cast<void>(::InterlockedExchange(&g_lCriticalSectionInitialized, 0L));
                static_cast<void>(::InterlockedExchange(&g_lSecurityInitialized, 0L));
                return false;
            }
        }

        // Enable security features
        EnableDEP();
        static_cast<void>(::HeapSetInformation(nullptr, GetHeapTerminationOnCorruption(), nullptr, 0));
        EnableMitigationPolicies();

        return true;
    }

    // Cleanup security context
    void CleanupSecurity() noexcept
    {
        if (::InterlockedCompareExchange(&g_lCriticalSectionInitialized, 0L, 1L) == 1L)
        {
            ::DeleteCriticalSection(&g_DllLoadCS);
        }
        static_cast<void>(::InterlockedExchange(&g_lSecurityInitialized, 0L));
    }

    // Read PEB field safely
    template <typename T>
    [[nodiscard]] bool ReadPEBField(size_t offset, T& value) noexcept
    {
        __try
        {
#if defined(_M_X64) || defined(_WIN64) || defined(_M_AMD64)
            const auto pPEB = __readgsqword(0x60);
            value = *reinterpret_cast<const T*>(pPEB + offset);
#elif defined(_M_IX86) || (defined(_WIN32) && !defined(_WIN64))
            const auto pPEB = __readfsdword(0x30);
            value = *reinterpret_cast<const T*>(static_cast<ULONG_PTR>(pPEB) + offset);
#else
            return false;
#endif
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }

    // Detect debugging attempts
    [[nodiscard]] bool IsDebuggerPresent() noexcept
    {
        // Method 1: IsDebuggerPresent API
        if (::IsDebuggerPresent())
            return true;

        // Method 2: CheckRemoteDebuggerPresent
        BOOL bDebuggerPresent = FALSE;
        if (::CheckRemoteDebuggerPresent(::GetCurrentProcess(), &bDebuggerPresent) && bDebuggerPresent)
            return true;

        // Method 3: PEB.BeingDebugged flag
        BYTE isDebugged = FALSE;
        if (ReadPEBField(0x02, isDebugged) && isDebugged)
            return true;

        // Method 4: NtGlobalFlag
        DWORD ntGlobalFlag = 0;
#if defined(_M_X64) || defined(_WIN64) || defined(_M_AMD64)
        constexpr size_t NtGlobalFlagOffset = 0xBC;
#else
        constexpr size_t NtGlobalFlagOffset = 0x68;
#endif

        constexpr DWORD DEBUG_FLAGS = SecurityConstants::PEB_DEBUG_FLAGS;
        if (ReadPEBField(NtGlobalFlagOffset, ntGlobalFlag) && (ntGlobalFlag & DEBUG_FLAGS) != 0)
            return true;

        return false;
    }

    // Clean path helper
    [[nodiscard]] std::wstring CleanPathSegments(const std::wstring& path)
    {
        std::wstring cleanPath;
        cleanPath.reserve(path.length());

        size_t start = 0;
        size_t end = path.find(L';');

        while (end != std::wstring::npos)
        {
            const std::wstring segment = path.substr(start, end - start);
            if (!segment.empty() && segment != L"." && segment != L".\\")
            {
                if (!cleanPath.empty())
                    cleanPath += L';';
                cleanPath += segment;
            }
            start = end + 1;
            end = path.find(L';', start);
        }

        // Handle last segment
        if (start < path.length())
        {
            const std::wstring segment = path.substr(start);
            if (!segment.empty() && segment != L"." && segment != L".\\")
            {
                if (!cleanPath.empty())
                    cleanPath += L';';
                cleanPath += segment;
            }
        }

        return cleanPath;
    }

    // Validate environment variables for security
    bool ValidateEnvironment()
    {
        // Check for dangerous environment variables
        constexpr const wchar_t* dangerousVars[] = {L"LD_PRELOAD", L"LD_LIBRARY_PATH", L"_NT_SYMBOL_PATH", L"_NT_ALT_SYMBOL_PATH",
                                                    L"_NT_DEBUGGER_EXTENSION_PATH"};

        for (const auto* var : dangerousVars)
        {
            if (::GetEnvironmentVariableW(var, nullptr, 0) > 0)
            {
                static_cast<void>(::SetEnvironmentVariableW(var, nullptr));
            }
        }

        // Validate PATH doesn't contain current directory
        // Check for integer overflow in buffer size calculation
        if (SecurityConstants::MAX_ENV_VAR_LENGTH > SIZE_MAX / sizeof(wchar_t))
            return false;

        const size_t bufferSize = SecurityConstants::MAX_ENV_VAR_LENGTH * sizeof(wchar_t);

        if (bufferSize > SecurityConstants::STACK_ALLOC_LIMIT)
        {
            HeapMemory heapMem(bufferSize);
            if (!heapMem.IsValid())
                return false;

            auto*       pathBuffer = static_cast<wchar_t*>(heapMem.Get());
            const DWORD pathLen = ::GetEnvironmentVariableW(L"PATH", pathBuffer, SecurityConstants::MAX_ENV_VAR_LENGTH);
            if (pathLen > 0 && pathLen < SecurityConstants::MAX_ENV_VAR_LENGTH)
            {
                const std::wstring cleanPath = CleanPathSegments(pathBuffer);
                static_cast<void>(::SetEnvironmentVariableW(L"PATH", cleanPath.c_str()));
            }
        }
        else
        {
            auto* pathBuffer = static_cast<wchar_t*>(_malloca(bufferSize));
            if (pathBuffer == nullptr)
                return false;

            const DWORD pathLen = ::GetEnvironmentVariableW(L"PATH", pathBuffer, SecurityConstants::MAX_ENV_VAR_LENGTH);
            if (pathLen > 0 && pathLen < SecurityConstants::MAX_ENV_VAR_LENGTH)
            {
                const std::wstring cleanPath = CleanPathSegments(pathBuffer);
                static_cast<void>(::SetEnvironmentVariableW(L"PATH", cleanPath.c_str()));
            }

            _freea(pathBuffer);
        }

        return true;
    }

    // Verify digital signature of a file
    [[nodiscard]] bool VerifyFileSignature(const SString& strFilePath)
    {
        const std::wstring wstrFilePath = FromUTF8(strFilePath);

        WINTRUST_FILE_INFO fileInfo = {};
        fileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
        fileInfo.pcwszFilePath = wstrFilePath.c_str();

        GUID guidAction = WINTRUST_ACTION_GENERIC_VERIFY_V2;

        WINTRUST_DATA winTrustData = {};
        winTrustData.cbStruct = sizeof(WINTRUST_DATA);
        winTrustData.dwUIChoice = WTD_UI_NONE;
        winTrustData.fdwRevocationChecks = WTD_REVOKE_NONE;
        winTrustData.dwUnionChoice = WTD_CHOICE_FILE;
        winTrustData.pFile = &fileInfo;
        winTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
        winTrustData.dwProvFlags = WTD_SAFER_FLAG;

        const LONG lStatus = ::WinVerifyTrust(nullptr, &guidAction, &winTrustData);

        // Clean up
        winTrustData.dwStateAction = WTD_STATEACTION_CLOSE;
        static_cast<void>(::WinVerifyTrust(nullptr, &guidAction, &winTrustData));

#ifdef MTA_DEBUG
        return (lStatus == ERROR_SUCCESS || lStatus == TRUST_E_NOSIGNATURE);
#else
        return (lStatus == ERROR_SUCCESS);
#endif
    }

    // Validate and sanitize command line
    [[nodiscard]] bool ValidateCommandLine(LPSTR lpCmdLine) noexcept
    {
        if (lpCmdLine == nullptr)
            return true;

        size_t        len = 0;
        const HRESULT hr = ::StringCchLengthA(lpCmdLine, SecurityConstants::MAX_CMDLINE_LENGTH + 1, &len);

        if (FAILED(hr) || len > SecurityConstants::MAX_CMDLINE_LENGTH)
            return false;

        // Check for dangerous characters
        constexpr char dangerous[] = {'\r', '\n'};
        for (size_t i = 0; i < len; ++i)
        {
            for (const char ch : dangerous)
            {
                if (lpCmdLine[i] == ch)
                    return false;
            }
        }

        return true;
    }

    // Enhanced path validation with canonicalization
    [[nodiscard]] bool IsValidPath(const SString& strPath)
    {
        if (strPath.empty() || strPath.length() > SecurityConstants::MAX_PATH_LENGTH)
            return false;

        const std::wstring widePath = FromUTF8(strPath);

        wchar_t canonicalPath[MAX_PATH + 1] = {};
        if (!::PathCanonicalizeW(canonicalPath, widePath.c_str()))
            return false;

        const SString strCanonical = ToUTF8(canonicalPath);

        // Check for path traversal patterns
        if (strCanonical.Contains("../") || strCanonical.Contains("..\\"))
            return false;

        // Check for alternative data streams
        const size_t colonPos = strCanonical.find(':');
        if (colonPos != SString::npos && colonPos != 1)
            return false;

        // Check for invalid characters
        constexpr const char* invalidChars = "<>\"|?*";
        for (size_t i = 0; invalidChars[i] != '\0'; ++i)
        {
            if (strPath.find(invalidChars[i]) != SString::npos)
                return false;
        }

        // Check for reserved names
        constexpr const char* reservedNames[] = {"CON",  "PRN",  "AUX",  "NUL",  "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7",
                                                 "COM8", "COM9", "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"};

        const SString strFilename = ExtractFilename(strPath);
        const SString strNameOnly = ExtractBeforeExtension(strFilename);

        for (const auto* reserved : reservedNames)
        {
            if (strNameOnly.CompareI(reserved))
                return false;
        }

        return true;
    }

    [[nodiscard]] bool IsValidDllName(const SString& strDllName) noexcept
    {
        if (strDllName.empty() || strDllName.length() > SecurityConstants::MAX_FILENAME_LENGTH)
            return false;

        if (!strDllName.EndsWithI(".dll"))
            return false;

        if (strDllName.Contains("/") || strDllName.Contains("\\"))
            return false;

        // Whitelist for launcher - only validate loader DLLs
        // The loader will handle validation of other MTA modules
        constexpr const char* allowedDlls[] = {"loader.dll", "loader_d.dll"};
        for (const auto* dll : allowedDlls)
        {
            if (strDllName.CompareI(dll))
                return true;
        }

        return false;
    }

    [[nodiscard]] bool ValidateModuleIntegrity(HMODULE hModule) noexcept
    {
        if (hModule == nullptr || hModule == INVALID_HANDLE_VALUE)
            return false;

        MODULEINFO moduleInfo = {};

        if (!::GetModuleInformation(::GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo)))
            return false;

        if (moduleInfo.lpBaseOfDll == nullptr || moduleInfo.SizeOfImage == 0)
            return false;

        if (moduleInfo.SizeOfImage > SecurityConstants::MAX_MODULE_SIZE || moduleInfo.SizeOfImage < SecurityConstants::MIN_MODULE_SIZE)
            return false;

        MEMORY_BASIC_INFORMATION mbi = {};

        const SIZE_T querySize = ::VirtualQuery(moduleInfo.lpBaseOfDll, &mbi, sizeof(mbi));
        if (querySize == 0)
            return false;

        if (mbi.State != MEM_COMMIT || mbi.Type != MEM_IMAGE)
            return false;

        return true;
    }

    // Verify file attributes
    [[nodiscard]] bool VerifyFileAttributes(HANDLE hFile) noexcept
    {
        LARGE_INTEGER fileSize = {};
        if (!::GetFileSizeEx(hFile, &fileSize))
            return false;

        if (fileSize.QuadPart < 0)
            return false;

#if !defined(_WIN64) && !defined(_M_X64) && !defined(_M_AMD64)
        if (fileSize.HighPart != 0)
            return false;
#endif

        const auto ullFileSize = static_cast<ULONGLONG>(fileSize.QuadPart);
        if (ullFileSize > SecurityConstants::MAX_MODULE_SIZE || ullFileSize < SecurityConstants::MIN_MODULE_SIZE)
            return false;

        BY_HANDLE_FILE_INFORMATION fileInfo = {};
        if (!::GetFileInformationByHandle(hFile, &fileInfo))
            return false;

        if ((fileInfo.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
            return false;

        return true;
    }

    // Helper function with no C++ objects for SEH compatibility
    static HMODULE LoadLibraryWithLock(const wchar_t* pwszPath, DWORD dwFlags) noexcept
    {
        if (pwszPath == nullptr)
            return nullptr;

        HANDLE hFile = ::CreateFileW(pwszPath, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

        if (hFile == INVALID_HANDLE_VALUE)
            return nullptr;

        BOOL bValidFile = VerifyFileAttributes(hFile);
        ::CloseHandle(hFile);

        if (!bValidFile)
            return nullptr;

        // Load the library
        // The digital signature check provides the main security guarantee
        return ::LoadLibraryExW(pwszPath, nullptr, dwFlags);
    }

    // Secure DLL loading with file locking
    [[nodiscard]] HMODULE SecureLoadLibraryWithLockImpl(const SString& strDllPath, DWORD dwFlags)
    {
        // Convert to wide string and call the SEH-safe helper
        std::wstring widePath = FromUTF8(strDllPath);
        return LoadLibraryWithLock(widePath.c_str(), dwFlags);
    }

    [[nodiscard]] HMODULE SecureLoadLibrary(const SString& strDllPath, const SString& strDllName)
    {
        if (!IsValidPath(strDllPath) || !IsValidDllName(strDllName))
        {
            SString strError = "SecureLoadLibrary: Invalid path or DLL name";
            AddReportLog(ReportLogID::LOADER_INVALID_DLL_PATH, strError);
            return nullptr;
        }

        if (!FileExists(strDllPath))
        {
            SString strError = "SecureLoadLibrary: DLL file does not exist";
            AddReportLog(ReportLogID::LOADER_DLL_NOT_EXISTS, strError);
            return nullptr;
        }

#if !defined(MTA_DEBUG) && MTASA_VERSION_TYPE >= VERSION_TYPE_UNTESTED
        if (!VerifyFileSignature(strDllPath))
        {
            SString strError = "SecureLoadLibrary: Invalid or missing digital signature";
            AddReportLog(ReportLogID::LOADER_DLL_INVALID_SIGNATURE, strError);

            return nullptr;
        }
#endif

        HMODULE hModule = nullptr;
        bool    bHasLock = false;

        if (::InterlockedCompareExchange(&g_lCriticalSectionInitialized, 1L, 1L) == 1L)
        {
            ::EnterCriticalSection(&g_DllLoadCS);
            bHasLock = true;
        }

        const bool bHasKB2533623 = (::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"), "AddDllDirectory") != nullptr);

        if (bHasKB2533623)
        {
            // Try standard approach first for compatibility
            hModule = SecureLoadLibraryWithLockImpl(strDllPath, LOAD_WITH_ALTERED_SEARCH_PATH);

            if (hModule == nullptr)
            {
                // Fall back to secure search if standard fails
                hModule = SecureLoadLibraryWithLockImpl(strDllPath, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_SYSTEM32);
            }
        }
        else
        {
            static_cast<void>(::SetDllDirectoryW(L""));
            hModule = SecureLoadLibraryWithLockImpl(strDllPath, LOAD_WITH_ALTERED_SEARCH_PATH);
        }

        static_cast<void>(::SetDllDirectoryW(nullptr));

        if (bHasLock)
        {
            ::LeaveCriticalSection(&g_DllLoadCS);
        }

        if (hModule == nullptr)
        {
            const DWORD dwError = ::GetLastError();
            SString     strError;
            if (!strError.Format("SecureLoadLibrary: LoadLibrary failed with error %lu", dwError).empty())
            {
                AddReportLog(ReportLogID::LOADER_LOADLIBRARY_FAIL, strError);
            }
        }

        return hModule;
    }

    // Check if running with appropriate privileges
    void ValidateProcessPrivileges() noexcept
    {
        HANDLE hToken = nullptr;

        if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hToken))
            return;

        TOKEN_ELEVATION elevation = {};
        DWORD           dwSize = sizeof(elevation);

        BOOL bIsElevated = FALSE;
        if (::GetTokenInformation(hToken, TokenElevation, &elevation, dwSize, &dwSize))
        {
            bIsElevated = elevation.TokenIsElevated;
        }

        static_cast<void>(::CloseHandle(hToken));

        if (bIsElevated)
        {
            SString strWarning = "Warning: Running with elevated privileges";
            AddReportLog(ReportLogID::LOADER_ELEVATED_PRIVILEGE, strWarning);
        }
    }

    // Validate function pointer is within module bounds
    [[nodiscard]] bool ValidateFunctionPointer(HMODULE hModule, const void* pFunction) noexcept
    {
        if (hModule == nullptr || pFunction == nullptr)
            return false;

        MODULEINFO moduleInfo = {};

        if (!::GetModuleInformation(::GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo)))
            return false;

        const auto pModuleStart = reinterpret_cast<ULONG_PTR>(moduleInfo.lpBaseOfDll);
        const auto pFunc = reinterpret_cast<ULONG_PTR>(pFunction);
        const auto ullModuleSize = static_cast<ULONG_PTR>(moduleInfo.SizeOfImage);

        if (ullModuleSize > MAXULONG_PTR - pModuleStart)
            return false;

        const auto pModuleEnd = pModuleStart + ullModuleSize;

        return (pFunc >= pModuleStart && pFunc < pModuleEnd);
    }
}            // namespace Security

// Type definition for DoWinMain function
typedef int(WINAPI* PFNDOWINMAIN)(HINSTANCE, HINSTANCE, LPSTR, int);

// Helper function for calling DoWinMain with SEH (no C++ objects)
static int CallDoWinMainSafe(PFNDOWINMAIN pfnDoWinMain, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) noexcept
{
    __try
    {
        return pfnDoWinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return -1;            // Special error code for exception
    }
}

///////////////////////////////////////////////////////////////
//
// WinMain
//
//
//
///////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, [[maybe_unused]] HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Initialize security subsystem
    if (!Security::InitializeSecurity())
    {
        return 1;
    }

    // Validate environment for security
    if (!Security::ValidateEnvironment())
    {
        Security::CleanupSecurity();
        return 1;
    }

#if !defined(MTA_DEBUG) && MTASA_VERSION_TYPE >= VERSION_TYPE_UNTESTED
    // Anti-debugging check (only in release builds)
    if (Security::IsDebuggerPresent())
    {
        Security::CleanupSecurity();
        return 1;
    }
#endif

    // Validate process privileges
    Security::ValidateProcessPrivileges();

    // Validate command line early
    if (!Security::ValidateCommandLine(lpCmdLine))
    {
        Security::CleanupSecurity();
        return 1;
    }

    // Detect KB2533623 early for security setup
    const bool bHasKB2533623 = (::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"), "AddDllDirectory") != nullptr);

    // Set DLL directory to empty string to remove current directory from search
    // but don't use SetDefaultDllDirectories as it affects the entire process permanently
    static_cast<void>(::SetDllDirectoryW(L""));

    // Check Windows version
    if (!IsWindowsXPSP3OrGreater())
    {
        SString strError = "This version of MTA requires Windows XP SP3 or later";
        BrowseToSolution("launch-xpsp3-check", ASK_GO_ONLINE, strError);
        Security::CleanupSecurity();
        return 1;
    }

    // Group processes under single taskbar button
    SetCurrentProcessExplicitAppUserModelID(L"Multi Theft Auto " MTA_STR(MTASA_VERSION_MAJOR) L"." MTA_STR(MTASA_VERSION_MINOR));

    // Load the loader.dll
    SString strLoaderDllFilename;
#ifdef MTA_DEBUG
    strLoaderDllFilename = "loader_d.dll";
#else
    strLoaderDllFilename = "loader.dll";
#endif

    if (!Security::IsValidDllName(strLoaderDllFilename))
    {
        SString strError = "Invalid loader DLL configuration";
        BrowseToSolution("loader-dll-invalid-name", ASK_GO_ONLINE, strError);
        Security::CleanupSecurity();
        return 1;
    }

    // Build paths
    SString       strMTAFolder = "mta";
    const SString strMTASAPath = PathJoin(GetLaunchPath(), strMTAFolder);
    const SString strLoaderDllPathFilename = PathJoin(strMTASAPath, strLoaderDllFilename);

    // Validate paths
    if (!Security::IsValidPath(strMTASAPath) || !Security::IsValidPath(strLoaderDllPathFilename))
    {
        SString strError = "Security validation failed";
        BrowseToSolution("loader-dll-path-error", ASK_GO_ONLINE, strError);
        Security::CleanupSecurity();
        return 1;
    }

    // Canonicalize and validate path traversal
    wchar_t szCanonicalMTAPath[MAX_PATH + 1] = {};
    wchar_t szCanonicalDllPath[MAX_PATH + 1] = {};

    if (!::PathCanonicalizeW(szCanonicalMTAPath, FromUTF8(strMTASAPath)) || !::PathCanonicalizeW(szCanonicalDllPath, FromUTF8(strLoaderDllPathFilename)))
    {
        SString strError = "Path canonicalization failed";
        BrowseToSolution("loader-dll-canonicalize-error", ASK_GO_ONLINE, strError);
        Security::CleanupSecurity();
        return 1;
    }

    // Ensure DLL is within MTA directory
    const SString strNormalizedMTAPath = ToUTF8(szCanonicalMTAPath);
    const SString strNormalizedDllPath = ToUTF8(szCanonicalDllPath);

    if (!strNormalizedDllPath.BeginsWithI(strNormalizedMTAPath))
    {
        SString strError = "Security violation detected";
        BrowseToSolution("loader-dll-path-traversal", ASK_GO_ONLINE, strError);
        Security::CleanupSecurity();
        return 1;
    }

    // Load DLL securely
    HMODULE hModule = Security::SecureLoadLibrary(strLoaderDllPathFilename, strLoaderDllFilename);

    if (hModule != nullptr)
    {
        if (!Security::ValidateModuleIntegrity(hModule))
        {
            static_cast<void>(::FreeLibrary(hModule));
            hModule = nullptr;
        }
    }
    else
    {
        const DWORD dwLoadLibraryError = ::GetLastError();

        // Fallback: Change directory and try relative path
        if (Security::IsValidPath(strMTASAPath) && DirectoryExists(strMTASAPath))
        {
            wchar_t     szPreviousDir[MAX_PATH + 1] = {};
            const DWORD dwPrevDirLen = ::GetCurrentDirectoryW(MAX_PATH, szPreviousDir);

            if (dwPrevDirLen > 0 && dwPrevDirLen <= MAX_PATH)
            {
                if (::SetCurrentDirectoryW(FromUTF8(strMTASAPath)))
                {
                    hModule = Security::SecureLoadLibrary(strLoaderDllFilename, strLoaderDllFilename);

                    if (hModule != nullptr && !Security::ValidateModuleIntegrity(hModule))
                    {
                        static_cast<void>(::FreeLibrary(hModule));
                        hModule = nullptr;
                    }

                    static_cast<void>(::SetCurrentDirectoryW(szPreviousDir));
                }
            }
        }

        if (hModule == nullptr)
        {
            SString strMessage = "Failed to load loader DLL";

            if (dwLoadLibraryError == ERROR_MOD_NOT_FOUND)
            {
                strMessage = "Missing dependencies or corrupted installation";
            }
            else if (dwLoadLibraryError == ERROR_ACCESS_DENIED)
            {
                strMessage = "Access denied. Check file permissions";
            }
            else if (dwLoadLibraryError == ERROR_INVALID_PARAMETER && !bHasKB2533623)
            {
                strMessage = "Install Windows Update KB2533623 for improved security";
            }

            AddReportLog(ReportLogID::LOADER_MODULE_LOAD_FAIL, strMessage);
            BrowseToSolution("loader-dll-not-loadable", ASK_GO_ONLINE, strMessage);
            Security::CleanupSecurity();
            return 1;
        }
    }

    // Process result
    int iReturnCode = 0;

    if (hModule != nullptr)
    {
        const FARPROC fpDoWinMain = ::GetProcAddress(hModule, "DoWinMain");

#pragma warning(push)
#pragma warning(disable : 4191)            // 'reinterpret_cast': unsafe conversion from 'FARPROC'
        const auto pfnDoWinMain = reinterpret_cast<PFNDOWINMAIN>(fpDoWinMain);
#pragma warning(pop)

        if (pfnDoWinMain != nullptr)
        {
            if (Security::ValidateFunctionPointer(hModule, reinterpret_cast<const void*>(pfnDoWinMain)))
            {
                int iResult = CallDoWinMainSafe(pfnDoWinMain, hInstance, hPrevInstance, lpCmdLine, nCmdShow);
                if (iResult == -1)
                {
                    SString strError = "Exception in DoWinMain";
                    AddReportLog(ReportLogID::LOADER_EXCEPTION, strError);
                    iReturnCode = 1;
                }
                else
                {
                    iReturnCode = iResult;
                }
            }
            else
            {
                SString strError = "Security violation detected";
                BrowseToSolution("loader-dll-code-injection", ASK_GO_ONLINE, strError);
                iReturnCode = 1;
            }
        }
        else
        {
            SString strError = "Failed to find DoWinMain in loader DLL";
            AddReportLog(ReportLogID::LOADER_MODULE_MAIN_FAIL, strError);
            BrowseToSolution("loader-dll-missing-export", ASK_GO_ONLINE, strError);
            iReturnCode = 1;
        }

        static_cast<void>(::FreeLibrary(hModule));
    }

    Security::CleanupSecurity();

    return iReturnCode;
}

// Restore default packing
#pragma pack(pop)
