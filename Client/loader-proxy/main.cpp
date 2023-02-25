/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        loader-proxy/main.cpp
 *  PURPOSE:     Acts as a proxy for winmm.dll and loads core.dll into the GTA process
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#define WIN32_LEAN_AND_MEAN
#define MTA_CLIENT

#include <string>
#include <filesystem>
#include <cassert>
#include <windows.h>
#include <tlhelp32.h>
#include <ShlObj.h>
#include <ShellScalingApi.h>
#include <version.h>
#include <SharedUtil.h>
#include <SharedUtil.hpp>

constexpr std::wstring_view GTA_EXE_NAME = L"gta_sa.exe";
constexpr std::wstring_view STEAM_GTA_EXE_NAME = L"gta-sa.exe";
constexpr std::wstring_view EXPLORER_EXE_NAME = L"explorer.exe";

#ifdef MTA_DEBUG
    constexpr std::wstring_view MTA_EXE_NAME = L"Multi Theft Auto_d.exe";
    constexpr std::wstring_view CORE_DLL_NAME = L"core_d.dll";
    constexpr std::wstring_view NETC_DLL_NAME = L"netc_d.dll";
#else
    constexpr std::wstring_view MTA_EXE_NAME = L"Multi Theft Auto.exe";
    constexpr std::wstring_view CORE_DLL_NAME = L"core.dll";
    constexpr std::wstring_view NETC_DLL_NAME = L"netc.dll";
#endif

namespace fs = std::filesystem;

BOOL WINAPI MyGetVersionExA(LPOSVERSIONINFOA versionInfo);

BOOL OnLibraryDetach();
BOOL OnLibraryAttach();

auto SetImportProcAddress(const char* moduleName, const char* procedureName, FARPROC replacement) -> FARPROC;
void DisplayErrorMessageBox(const std::wstring& message, const std::wstring& errorCode);
bool DisplayWarningMessageBox(const std::wstring& message, const std::wstring& errorCode);
auto PatchWinmmImports() -> int;
auto GetSystemErrorMessage(DWORD errorCode) -> std::wstring;
auto GetCurrentProcessPath() -> fs::path;
auto GetParentProcessPath() -> fs::path;
void ApplyDpiAwareness();
void ApplyDirectoryInformation(HMODULE library, const std::wstring& mtaDirectory, const std::wstring& gtaDirectory);
auto AppendSystemError(std::wstring message, DWORD errorCode) -> std::wstring;
auto MakeLauncherError(std::wstring message) -> std::wstring;
auto MakeMissingFilesError(std::wstring message) -> std::wstring;
void AddLaunchLog(const char* format, ...);
bool IEqual(std::wstring_view lhs, std::wstring_view rhs);

HMODULE g_exe = nullptr;
HMODULE g_core = nullptr;
HMODULE g_netc = nullptr;

BOOL(WINAPI* Win32GetVersionExA)(LPOSVERSIONINFOA) = nullptr;

BOOL WINAPI DllMain(HINSTANCE dll, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_exe = GetModuleHandleW(nullptr);
        DisableThreadLibraryCalls(dll);
        return OnLibraryAttach();
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        return OnLibraryDetach();
    }

    return TRUE;
}

BOOL OnLibraryDetach()
{
    if (g_core)
    {
        FreeLibrary(g_core);
        g_core = nullptr;
    }

    if (g_netc)
    {
        FreeLibrary(g_netc);
        g_netc = nullptr;
    }

    return TRUE;
}

BOOL OnLibraryAttach()
{
    // Apply dpi awareness for our message boxes.
    ApplyDpiAwareness();

    // Replace the first called imported procedure from the executable.
    FARPROC procedure = SetImportProcAddress("kernel32.dll", "GetVersionExA", reinterpret_cast<FARPROC>(MyGetVersionExA));

    if (!procedure)
    {
        DisplayErrorMessageBox(MakeLauncherError(L"Failed to redirect start procedure."), L"CL50");
        return FALSE;
    }

    Win32GetVersionExA = reinterpret_cast<decltype(Win32GetVersionExA)>(procedure);
    return TRUE;
}

VOID OnGameLaunch()
{
    std::error_code ec{};

    // MTA:SA launches GTA:SA process with the GTA:SA installation directory as the current directory.
    // We can't use the path to the current executable, because it's not in the game directory anymore.
    const fs::path gtaDirectory = fs::current_path(ec);

    if (ec)
    {
        AddLaunchLog("GTA:SA directory not found (%d): %s", ec.value(), ec.message().c_str());
        std::wstring message = L"Unable to determine working directory.";
        DisplayErrorMessageBox(MakeLauncherError(AppendSystemError(message, ec.value())), L"CL51");
        return;
    }

    const fs::path gtaExe = gtaDirectory / GTA_EXE_NAME;

    if (!fs::is_regular_file(gtaExe, ec))
    {
        const fs::path steamExe = gtaDirectory / STEAM_GTA_EXE_NAME;

        if (std::error_code ignore; !fs::is_regular_file(steamExe, ignore))
        {
            AddLaunchLog("GTA:SA executable not found (%d): %s", ec.value(), ec.message().c_str());
            std::wstring message = L"Game executable not found:\n" + gtaExe.wstring();
            DisplayErrorMessageBox(MakeLauncherError(AppendSystemError(message, ec.value())), L"CL51");
            return;
        }
    }

    // Abort if the current process is not the game executable.
    const std::wstring processName = GetCurrentProcessPath().filename().wstring();

    if (!IEqual(GTA_EXE_NAME, processName))
    {
        std::wstring message = L"Executable has an incorrect name (" + processName + L").";
        DisplayErrorMessageBox(MakeLauncherError(message), L"CL52");
        return;
    }

    // MTA:SA must be the parent launcher process in every case.
    const fs::path launcherPath = GetParentProcessPath();

    if (launcherPath.empty())
    {
        AddLaunchLog("Unable to determine launcher executable");
        DisplayErrorMessageBox(MakeLauncherError(L"Unable to determine launcher executable."), L"CL53");
        return;
    }

    // Check if the name of the launcher process matches Multi Theft Auto.
    const std::wstring launcherName = launcherPath.filename().wstring();

    if (!IEqual(MTA_EXE_NAME, launcherName))
    {
        if (IEqual(EXPLORER_EXE_NAME, launcherName))
        {
            DisplayErrorMessageBox(MakeLauncherError(L"Do not run this game from Windows Explorer."), L"CL54");
            return;
        }

        std::wstring message = L"Launcher executable has an incorrect name (" + launcherName + L").";
        
        if (!DisplayWarningMessageBox(MakeLauncherError(message), L"CL54"))
        {
            ExitProcess(1);
            return;
        }
    }

    // Check if models/gta3.img exists because the process will lock indefinitely otherwise.
    const fs::path gtaImage = gtaDirectory / "models" / "gta3.img";

    if (!fs::is_regular_file(gtaImage))
    {
        AddLaunchLog("gta3.img not found (%d): %s", ec.value(), ec.message().c_str());
        std::wstring message = L"gta3.img not found:\n" + gtaImage.wstring();
        DisplayErrorMessageBox(MakeLauncherError(AppendSystemError(message, ec.value())), L"CL51");
        return;
    }

    // Check if the MTA subdirectory exists.
    const fs::path mtaRootDirectory = launcherPath.parent_path();
    const fs::path mtaDirectory = mtaRootDirectory / "MTA";

    if (!fs::is_directory(mtaDirectory, ec))
    {
        AddLaunchLog("MTA subdirectory not found (%d): %s", ec.value(), ec.message().c_str());
        std::wstring message = L"MTA directory does not exist or user is unauthorized:\n" + mtaDirectory.wstring();
        DisplayErrorMessageBox(MakeMissingFilesError(AppendSystemError(message, ec.value())), L"CL55");
        return;
    }

    // Check if the netc.dll library exists in the MTA subdirectory.
    const fs::path netcPath = mtaDirectory / NETC_DLL_NAME;

    if (!fs::is_regular_file(netcPath, ec))
    {
        AddLaunchLog("Network library not found (%d): %s", ec.value(), ec.message().c_str());
        std::wstring message = L"Could not find or access the network library.";
        DisplayErrorMessageBox(MakeLauncherError(AppendSystemError(message, ec.value())), L"CL56");
        return;
    }

    // Check if the core.dll library exists in the MTA subdirectory.
    const fs::path corePath = mtaDirectory / CORE_DLL_NAME;

    if (!fs::is_regular_file(corePath, ec))
    {
        AddLaunchLog("Core library not found (%d): %s", ec.value(), ec.message().c_str());
        std::wstring message = L"Could not find or access the core library.";
        DisplayErrorMessageBox(MakeLauncherError(AppendSystemError(message, ec.value())), L"CL57");
        return;
    }

    // Patch the winmm.dll imports we've taken over with our mtasa.dll library back to the functions from the winmm.dll library.
    if (int error = PatchWinmmImports())
    {
        std::wstring message;

        switch (error)
        {
            case 1:
                message = L"Loading system-provided winmm.dll failed.";
                break;
            case 4:
                message = L"Unable to find winmm.dll import entry.";
                break;
            default:
                message = L"Patching winmm.dll imports failed.";
                break;
        }

        AddLaunchLog("Patching imports has failed (%d)", error);
        DisplayErrorMessageBox(MakeMissingFilesError(message), L"CL58");
        return;
    }

    // For dll searches, this call replaces the current directory entry and turns off 'SafeDllSearchMode'.
    // Meaning it will search the supplied path before the system and windows directory.
    // http://msdn.microsoft.com/en-us/library/ms682586%28VS.85%29.aspx
    SetDllDirectoryW(mtaDirectory.wstring().c_str());

    // Load and set up netc.dll library.
    g_netc = LoadLibraryW(netcPath.wstring().c_str());

    if (!g_netc)
    {
        DWORD errorCode = GetLastError();
        AddLaunchLog("Loading network library has failed (%d): %s", errorCode, std::system_category().message(errorCode).c_str());
        std::wstring message = L"Loading network library has failed.";
        DisplayErrorMessageBox(MakeLauncherError(AppendSystemError(message, errorCode)), L"CL58");
        return;
    }

    ApplyDirectoryInformation(g_netc, mtaRootDirectory.wstring(), gtaDirectory.wstring());

    void (*InitNetRev)(const char*, const char*, const char*) = reinterpret_cast<decltype(InitNetRev)>(GetProcAddress(g_netc, "InitNetRev"));

    if (InitNetRev)
    {
        InitNetRev(GetProductRegistryPath(), GetProductCommonDataDir(), GetProductVersion());
    }

    bool (*CheckService)(unsigned int) = reinterpret_cast<decltype(CheckService)>(GetProcAddress(g_netc, "CheckService"));

    if (!CheckService)
    {
        AddLaunchLog("Network procedure 'CheckService' not found");
        std::wstring message = L"Network library is incompatible.";
        DisplayErrorMessageBox(MakeLauncherError(message), L"CL58");
        return;
    }

    CheckService(7 /* CHECK_SERVICE_POST_CREATE */);

    // Load and set up core.dll library.
    g_core = LoadLibraryW(corePath.wstring().c_str());

    if (!g_core)
    {
        DWORD errorCode = GetLastError();
        AddLaunchLog("Loading core library has failed (%d): %s", errorCode, std::system_category().message(errorCode).c_str());

#ifdef MTA_DEBUG
        DisplayErrorMessageBox(
            L"Loading debug core has failed."
            L"\n\n"
            L"Please ensure that your MTA:SA build was successful and that "
            L"you executed win-install-data.bat to copy dependencies to the output directory.",
            L"CL24");
#else
        DisplayErrorMessageBox(
            L"Loading core has failed."
            L"\n\n"
            L"Please ensure that your MTA:SA installation is complete "
            L"and that the latest DirectX is correctly installed.",
            L"CL24");
#endif
        return;
    }

    ApplyDirectoryInformation(g_core, mtaRootDirectory.wstring(), gtaDirectory.wstring());

    // Initialize and run the core.
    int (*InitializeCore)() = reinterpret_cast<decltype(InitializeCore)>(GetProcAddress(g_core, "InitializeCore"));

    if (!InitializeCore)
    {
        AddLaunchLog("Core procedure 'InitializeCore' not found");
        std::wstring message = L"Core library is incompatible.";
        DisplayErrorMessageBox(MakeMissingFilesError(message), L"CL59");
        return;
    }

    int errorCode = InitializeCore();

    if (errorCode)
    {
        AddLaunchLog("Core initialization failed (%d)", errorCode);
        std::wstring message = L"Core library failed to initialize (code: " + std::to_wstring(errorCode) + L").";
        DisplayErrorMessageBox(MakeLauncherError(message), L"CL59");
        return;
    }
}

BOOL WINAPI MyGetVersionExA(LPOSVERSIONINFOA versionInfo)
{
    // Execute the original function with the given parameter.
    BOOL result = Win32GetVersionExA(versionInfo);

    // Restore the function pointer we've overriden to get here.
    SetImportProcAddress("kernel32.dll", "GetVersionExA", reinterpret_cast<FARPROC>(Win32GetVersionExA));

    // Run our startup code.
    OnGameLaunch();

    return result;
}

/**
 * @brief A placeholder function for fake winmm.dll exports.
*/
EXTERN_C void noreturn()
{
    // We should never enter this function.
    assert(false);
}

/**
 * @brief Replaces an import library procedure with the given replacement function.
 * @param moduleName Name of the import library (case-insensitive)
 * @param procedureName Name of the procedure to be replaced (case-insensitive)
 * @param replacement Function pointer to the replacement
 * @return Previous function pointer on success, a null pointer otherwise
*/
auto SetImportProcAddress(const char* moduleName, const char* procedureName, FARPROC replacement) -> FARPROC
{
    auto base = reinterpret_cast<std::byte*>(g_exe);
    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    auto descriptor = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (; descriptor->Name; ++descriptor)
    {
        auto name = reinterpret_cast<const char*>(base + descriptor->Name);

        if (stricmp(moduleName, name))
            continue;

        auto nameTableEntry = reinterpret_cast<DWORD*>(base + descriptor->FirstThunk);
        auto addressTableEntry = reinterpret_cast<FARPROC*>(nameTableEntry);

        if (descriptor->OriginalFirstThunk)
            nameTableEntry = reinterpret_cast<DWORD*>(base + descriptor->OriginalFirstThunk);

        for (; *nameTableEntry; ++nameTableEntry, ++addressTableEntry)
        {
            if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
            {
                auto ordinal = reinterpret_cast<char const*>(IMAGE_ORDINAL(*nameTableEntry));

                if (procedureName != ordinal)
                    continue;
            }
            else
            {
                const char* importName = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(base + *nameTableEntry)->Name;

                if (stricmp(procedureName, importName))
                    continue;
            }

            DWORD protection;
            VirtualProtect(addressTableEntry, sizeof(FARPROC), PAGE_READWRITE, &protection);
            FARPROC old = *addressTableEntry;
            *addressTableEntry = replacement;
            VirtualProtect(addressTableEntry, sizeof(FARPROC), protection, &protection);
            return old;
        }
    }

    return nullptr;
}

/**
 * @brief Displays a customizable message box to the user.
 * @param message The message to display
 * @param errorCode The error code for the caption
 * @param flags Additional flags to customize the appearance
 * @return User choice
*/
int DisplayMessageBox(const std::wstring& message, const std::wstring& errorCode, UINT flags)
{
    std::wstring caption = L"MTA: San Andreas  [" + errorCode + L"]   (CTRL+C to copy)";
    std::wstring extended = message;
    extended +=
        L"\n\nIf you are unable to resolve this issue on your own, "
        L"please get in contact with our support on www.multitheftauto.com";
    return MessageBoxW(nullptr, extended.c_str(), caption.c_str(), flags | MB_TOPMOST);
}

/**
 * @brief Displays an error message box to the user and exits the current process.
 * @param message The message to display
 * @param errorCode The error code for the caption
*/
void DisplayErrorMessageBox(const std::wstring& message, const std::wstring& errorCode)
{
    DisplayMessageBox(message, errorCode, MB_OK | MB_ICONERROR);
    ExitProcess(1);
}

/**
 * @brief Displays a warning message box to the user and exits the current process if user hits cancel.
 * @param message The message to display
 * @param errorCode The error code for the caption
 * @return False on cancellation, true otherwise
 */
bool DisplayWarningMessageBox(const std::wstring& message, const std::wstring& errorCode)
{
    if (IDCANCEL != DisplayMessageBox(message, errorCode, MB_OKCANCEL | MB_ICONWARNING))
        return true;

    TerminateProcess(GetCurrentProcess(), 1);
    return false;
}

/**
 * @brief Converts the specified path to its long form.
 * @param path The path to convert
 * @return Path in long form
*/
auto ToLongPath(const std::wstring& path) -> fs::path
{
    if (path.empty())
        return {};

    if (path.find(L'~') == std::wstring::npos)
        return fs::path{path};

    std::wstring buffer(path.size() + 256, 0);
    DWORD        numCharacters = GetLongPathNameW(path.data(), buffer.data(), buffer.capacity());

    if (buffer.size() < numCharacters)
    {
        buffer.resize(numCharacters);
        numCharacters = GetLongPathNameW(path.data(), buffer.data(), buffer.capacity());
    }

    buffer.resize(numCharacters);
    return fs::path{buffer};
}

/**
 * @brief Retrieves the full path of the executable image for the specified process.
 * @param process Handle to the process
 * @return Full path to the executable image
*/
auto GetProcessPath(HANDLE process) -> fs::path
{
    std::wstring buffer(4096, 0);
    DWORD        charactersWritten = buffer.capacity();

    if (QueryFullProcessImageNameW(process, 0, buffer.data(), &charactersWritten))
    {
        buffer.resize(charactersWritten);
        return ToLongPath(buffer);
    }

    while (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    {
        buffer.resize(buffer.capacity() * 2);
        charactersWritten = buffer.capacity();

        if (QueryFullProcessImageNameW(process, 0, buffer.data(), &charactersWritten))
            break;
    }

    buffer.resize(charactersWritten);
    return ToLongPath(buffer);
}

/**
 * @brief Retrieves the full path of the executable image for the specified process.
 * @param processId Numeric id of the process
 * @return Full path to the executable image
*/
auto GetProcessPath(DWORD processId) -> fs::path
{
    if (!processId)
        return {};

    HANDLE process{};

    for (int i = 0; i < 2 && !process; ++i)
    {
        DWORD access = (i == 0) ? PROCESS_QUERY_INFORMATION : PROCESS_QUERY_LIMITED_INFORMATION;
        process = OpenProcess(access, FALSE, processId);
    }

    if (!process)
        return {};

    fs::path result = GetProcessPath(process);
    CloseHandle(process);
    return result;
}

/**
 * @brief Retrieves the full path to the current process.
 * @return Full path to the executable image
*/
auto GetCurrentProcessPath() -> fs::path
{
    return GetProcessPath(GetCurrentProcess());
}

/**
 * @brief Retrieves the process id of the parent process.
 * @return Parent process id
*/
auto GetParentProcessId() -> DWORD
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snapshot, &entry))
    {
        const DWORD thisProcess = GetCurrentProcessId();

        do
        {
            if (entry.th32ProcessID == thisProcess)
            {
                CloseHandle(snapshot);
                return entry.th32ParentProcessID;
            }
        } while (Process32NextW(snapshot, &entry));
    }

    CloseHandle(snapshot);
    return 0;
}

/**
 * @brief Retrieves the full path to the parent's process.
 * @return Full path to the executable image
*/
auto GetParentProcessPath() -> fs::path
{
    return GetProcessPath(GetParentProcessId());
}

/**
 * @brief Loads the winmm.dll library.
 * @return A handle to the library
*/
auto LoadWinmmLibrary() -> HMODULE
{
    if (HMODULE winmm = LoadLibraryExW(L"winmm.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32))
        return winmm;

    return LoadLibraryW(L"winmm.dll");
}

/**
 * @brief Replaces fake mtasa.dll imports with functions retrieved from the winmm.dll library.
*/
auto PatchWinmmImports() -> int
{
    auto base = reinterpret_cast<std::byte*>(g_exe);
    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    auto nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    auto descriptor = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    // Iterate through the import descriptors and find the winmm.dll entry, which we renamed to mtasa.dll.
    for (; descriptor->Name; ++descriptor)
    {
        auto name = reinterpret_cast<const char*>(base + descriptor->Name);

        if (stricmp("mtasa.dll", name))
            continue;

        HMODULE winmm = LoadWinmmLibrary();

        if (!winmm)
            return 1;

        auto nameTableEntry = reinterpret_cast<DWORD*>(base + descriptor->FirstThunk);
        auto addressTableEntry = reinterpret_cast<FARPROC*>(nameTableEntry);

        if (descriptor->OriginalFirstThunk)
            nameTableEntry = reinterpret_cast<DWORD*>(base + descriptor->OriginalFirstThunk);

        // Replace every import with the correct function from the winmm.dll library.
        for (; *nameTableEntry; ++nameTableEntry, ++addressTableEntry)
        {
            const char* functionName;

            if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
            {
                functionName = reinterpret_cast<char const*>(IMAGE_ORDINAL(*nameTableEntry));
            }
            else
            {
                functionName = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(base + *nameTableEntry)->Name;
            }

            if (!functionName)
                return 2;

            FARPROC function = GetProcAddress(winmm, functionName);

            if (!function)
                return 3;

            DWORD protection;
            VirtualProtect(addressTableEntry, sizeof(FARPROC), PAGE_READWRITE, &protection);
            *addressTableEntry = function;
            VirtualProtect(addressTableEntry, sizeof(FARPROC), protection, &protection);
        }

        return 0;
    }

    return 4;
}

/**
 * @brief Transforms the numeric error code into a system-generated error string.
*/
auto GetSystemErrorMessage(DWORD errorCode) -> std::wstring
{
    if (!errorCode)
        return {};

    wchar_t* buffer = nullptr;

    DWORD size = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode,
                                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);

    if (!size)
        return {};

    struct ReleaseBuffer
    {
        HLOCAL memory;
        ~ReleaseBuffer() { LocalFree(memory); }
    };

    ReleaseBuffer deleter{buffer};
    std::wstring message{buffer, size};
    return message;
}

/**
 * @brief Applies the highest available form of DPI awareness for this process.
 */
void ApplyDpiAwareness()
{
    // Minimum version: Windows 10, version 1607
    static BOOL(WINAPI * Win32SetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value) = ([] {
        HMODULE user32 = LoadLibraryW(L"user32");
        return user32 ? reinterpret_cast<decltype(Win32SetProcessDpiAwarenessContext)>(GetProcAddress(user32, "SetProcessDpiAwarenessContext")) : nullptr;
    })();

    if (Win32SetProcessDpiAwarenessContext)
    {
        Win32SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
        return;
    }

    // Minimum version: Windows 8.1
    static HRESULT(WINAPI * Win32SetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value) = ([] {
        HMODULE shcore = LoadLibraryW(L"shcore");
        return shcore ? reinterpret_cast<decltype(Win32SetProcessDpiAwareness)>(GetProcAddress(shcore, "SetProcessDpiAwareness")) : nullptr;
    })();

    if (Win32SetProcessDpiAwareness)
    {
        Win32SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
        return;
    }

    // Minimum version: Windows Vista
    SetProcessDPIAware();
}

/**
 * @brief Sets the directory path information for a module.
 * @param library A module handle
 * @param mtaDirectory Path to the MTA:SA root directory
 * @param gtaDirectory Path to the GTA:SA root directory
*/
void ApplyDirectoryInformation(HMODULE library, const std::wstring& mtaDirectory, const std::wstring& gtaDirectory)
{
    // Set the path to the Multi Theft Auto directory.
    void (*SetMTADirectory)(const wchar_t*, size_t) = reinterpret_cast<decltype(SetMTADirectory)>(GetProcAddress(library, "SetMTADirectory"));

    if (SetMTADirectory)
    {
        SetMTADirectory(mtaDirectory.c_str(), mtaDirectory.size());
    }

    // Set the path to the GTA: San Andreas directory.
    void (*SetGTADirectory)(const wchar_t*, size_t) = reinterpret_cast<decltype(SetGTADirectory)>(GetProcAddress(library, "SetGTADirectory"));

    if (SetGTADirectory)
    {
        SetGTADirectory(gtaDirectory.c_str(), gtaDirectory.size());
    }
}

/**
 * @brief Optionally appends a system error text to the message.
 * @param message Message to append to
 * @param errorCode System error code
*/
auto AppendSystemError(std::wstring message, DWORD errorCode) -> std::wstring
{
    std::wstring errorMessage = GetSystemErrorMessage(errorCode);

    if (errorMessage.empty())
        return message;

    // Remove any whitespace characters at the end.
    size_t length = errorMessage.find_last_not_of(L"\t\n\v\f\r ");

    if (length == std::wstring::npos)
        return message;

    errorMessage.resize(length + 1);

    if (!message.empty())
        message += L"\n\n";

    message += L"Error (";
    message += std::to_wstring(errorCode);
    message += L"): ";
    message += errorMessage;
    return message;
}

/**
 * @brief Composes an error message with the plea to launch MTA properly.
 * @param message Message to append to
*/
auto MakeLauncherError(std::wstring message) -> std::wstring
{
    if (!message.empty())
        message += L"\n\n";

    message +=
        L"Please launch the game through the Multi Theft Auto launcher executable. "
        L"You can find the launcher either on your desktop or in the MTA:SA installation directory.";

    return message;
}

/**
 * @brief Composes an error message with the plea to fix missing or broken files.
 * @param message Message to append to
*/
auto MakeMissingFilesError(std::wstring message) -> std::wstring
{
    if (!message.empty())
        message += L"\n\n";

    message +=
        L"Please ensure that both your MTA:SA and Windows installation are not missing files "
        L"and your user is not lacking any permission to access these directories";

    return message;
}

/**
 * @brief Logs a message for the loader proxy to track startup issues.
*/
void AddLaunchLog(const char* format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    SString message;
    message.vFormat(format, arguments);
    va_end(arguments);

    AddReportLog(5720, message);
}

/**
 * @brief Compares two strings for equality (case-insensitive).
*/
bool IEqual(std::wstring_view lhs, std::wstring_view rhs)
{
    if (lhs.size() != rhs.size())
        return false;

    if (lhs.empty() && rhs.empty())
        return true;

    return !wcsnicmp(lhs.data(), rhs.data(), lhs.length());
}
