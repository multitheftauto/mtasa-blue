/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        launch/Main.cpp
 *  PURPOSE:     Launcher
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// C++ STL
#include <array>               // std::array for safe command buffer
#include <bit>                 // std::bit_cast for safe function pointer conversion
#include <expected>            // std::expected/std::unexpected for error handling
#include <format>              // std::format for string formatting
#include <ranges>              // std::ranges::copy/copy_n for safe copying
#include <span>                // std::span for memory view with bounds checking
#include <string_view>         // std::string_view for string parameters and constants
#include <type_traits>         // std::is_trivially_copyable_v for compile-time checks
#include <utility>             // std::forward for perfect forwarding

// C STL
#include <cstring>             // std::strlen for C string length

// Platform-specific headers
#include <psapi.h>             // GetModuleInformation, MODULEINFO

/*
    IMPORTANT

    If this project changes, a new release build should be copied into
    the Shared\data\launchers directory.

    The .exe in Shared\data\launchers will be used by the installer and updater.

    (set flag.new_client_exe on the build server to generate new exe)
*/

namespace mta::launcher
{
    enum class LoadResult : std::uint32_t
    {
        Success       = 0,
        PathError     = 1,
        LoadError     = 2,
        FunctionError = 3
    };

    enum class DllLoadError : std::uint32_t
    {
        FileNotFound = 1,
        InvalidPath  = 2,
        LoadFailed   = 3
    };

    // concepts for type safety
    template <std::size_t N>
    concept ValidBufferSize = N > 0 && N <= 65536;

#ifdef MTA_DEBUG
    constexpr std::string_view LOADER_DLL = "loader_d.dll";
#else
    constexpr std::string_view LOADER_DLL = "loader.dll";
#endif

    // RAII wrapper for preserving GetLastError
    class [[nodiscard]] LastErrorPreserver
    {
        DWORD saved_error_;

public:
        LastErrorPreserver() noexcept : saved_error_(GetLastError())
        {
        }
        
        ~LastErrorPreserver() noexcept
        {
            SetLastError(saved_error_);
        }
        
        LastErrorPreserver(const LastErrorPreserver&)            = delete;
        LastErrorPreserver& operator=(const LastErrorPreserver&) = delete;
        LastErrorPreserver(LastErrorPreserver&&)                 = delete;
        LastErrorPreserver& operator=(LastErrorPreserver&&)      = delete;
    };

    // RAII scope_exit with perfect forwarding
    template <typename F>
    struct [[nodiscard]] scope_exit
    {
        F f;

        explicit scope_exit(F&& func) noexcept : f(std::forward<F>(func))
        {
        }
        
        ~scope_exit() noexcept
        {
            f();
        }

        scope_exit(const scope_exit&)            = delete;
        scope_exit& operator=(const scope_exit&) = delete;
        scope_exit(scope_exit&&)                 = delete;
        scope_exit& operator=(scope_exit&&)      = delete;
    };

    template <typename F>
    constexpr auto make_scope_exit(F&& f) noexcept
    {
        return scope_exit<F>{std::forward<F>(f)};
    }

    // DLL directory guard with error preservation and optimized allocation
    class [[nodiscard]] DllDirectoryGuard
    {
        std::wstring original_dir;
        bool         changed = false;

public:
        DllDirectoryGuard()
        {
            LastErrorPreserver error_guard;
            DWORD              len = GetDllDirectoryW(0, nullptr);
            if (len > 0)
            {
                try
                {
                    original_dir.resize_and_overwrite(len, [](wchar_t* buf, std::size_t n) -> std::size_t {
                        DWORD copied = GetDllDirectoryW(static_cast<DWORD>(n), buf);
                        return copied < n ? copied : 0;
                    });
                }
                catch (...)
                {
                    original_dir.resize(len);
                    if (GetDllDirectoryW(len, original_dir.data()) == 0)
                        original_dir.clear();
                }
            }
        }

        [[nodiscard]] bool SetDirectory(const wchar_t* path) noexcept
        {
            if (SetDllDirectoryW(path))
            {
                changed = true;
                return true;
            }
            return false;
        }

        ~DllDirectoryGuard() noexcept
        {
            if (changed)
                SetDllDirectoryW(original_dir.empty() ? nullptr : original_dir.c_str());
        }

        DllDirectoryGuard(const DllDirectoryGuard&)            = delete;
        DllDirectoryGuard& operator=(const DllDirectoryGuard&) = delete;
        DllDirectoryGuard(DllDirectoryGuard&&)                 = delete;
        DllDirectoryGuard& operator=(DllDirectoryGuard&&)      = delete;
    };

    // COM initialization with error handling
    [[nodiscard]] std::expected<void, HRESULT> InitializeCOM() noexcept
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr))
            return std::unexpected(hr);
        return {};
    }

    // Library loading with optimized string conversion
    [[nodiscard]] std::expected<HMODULE, DllLoadError> LoadLibrarySafe(const SString& dll_path, std::string_view dll_name, const SString& mta_path)
    {
        WString dll_path_w = FromUTF8(dll_path);
        if (dll_path_w.empty()) [[unlikely]]
            return std::unexpected(DllLoadError::InvalidPath);

        if (HMODULE module = LoadLibraryW(dll_path_w.c_str()))
            return module;

        DllDirectoryGuard dll_guard;
        WString           mta_path_w = FromUTF8(mta_path);
        std::wstring      dll_name_w;
        try
        {
            dll_name_w.resize_and_overwrite(dll_name.size() * 2, [&dll_name](wchar_t* buf, std::size_t n) -> std::size_t {
                SString temp{dll_name.data(), dll_name.size()};
                WString wide = FromUTF8(temp);
                if (wide.empty())
                    return 0;
                std::size_t len = std::min(n, wide.length());
                std::ranges::copy_n(wide.c_str(), len, buf);
                return len;
            });
        }
        catch (...)
        {
            SString temp{dll_name.data(), dll_name.size()};
            WString wide = FromUTF8(temp);
            dll_name_w.assign(wide.begin(), wide.end());
        }

        if (mta_path_w.empty() || dll_name_w.empty()) [[unlikely]]
            return std::unexpected(DllLoadError::InvalidPath);

        if (!dll_guard.SetDirectory(mta_path_w.c_str()))
            return std::unexpected(DllLoadError::LoadFailed);

        std::wstring full_path_w = std::wstring(mta_path_w.c_str()) + L"\\" + dll_name_w;
        if (HMODULE module = LoadLibraryW(full_path_w.c_str()))
            return module;

        return std::unexpected(DllLoadError::LoadFailed);
    }

    // Function execution with bounds checking and secure conversion
    [[nodiscard]] std::expected<int, LoadResult> ExecuteDoWinMain(HMODULE module, HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)
    {
        using DoWinMainFunc = int (*)(HINSTANCE, HINSTANCE, LPSTR, int);

        if (!module) [[unlikely]]
            return std::unexpected(LoadResult::FunctionError);

        FARPROC proc = GetProcAddress(module, "DoWinMain");
        if (!proc) [[unlikely]]
        {
            AddReportLog(5713, "Launcher Main: DoWinMain function not found");
            return std::unexpected(LoadResult::FunctionError);
        }

        // Validate function pointer within module bounds
        MODULEINFO info{};
        if (!GetModuleInformation(GetCurrentProcess(), module, &info, sizeof(info)))
            return std::unexpected(LoadResult::FunctionError);

        std::span<const std::byte> module_span{static_cast<const std::byte*>(info.lpBaseOfDll), info.SizeOfImage};
        auto                       func_ptr = reinterpret_cast<const std::byte*>(proc);

        if (func_ptr < module_span.data() || func_ptr >= (module_span.data() + module_span.size()))
        {
            AddReportLog(5713, "Launcher Main: Function pointer outside module bounds");
            return std::unexpected(LoadResult::FunctionError);
        }

        // Command line handling
        constexpr std::size_t     MAX_CMD = 8192;
        static_assert(ValidBufferSize<MAX_CMD>);

        std::array<char, MAX_CMD> safe_cmd{};
        if (cmd)
        {
            auto src_view = std::string_view{cmd, std::min(std::strlen(cmd), MAX_CMD - 1)};
            std::ranges::copy(src_view, safe_cmd.begin());
        }

        // Function pointer conversion with safety checks
        static_assert(sizeof(FARPROC) == sizeof(DoWinMainFunc), "Function pointer size mismatch");
        static_assert(alignof(FARPROC) == alignof(DoWinMainFunc), "Function pointer alignment mismatch");
        static_assert(std::is_trivially_copyable_v<FARPROC>, "FARPROC must be trivially copyable for bit_cast");
        static_assert(std::is_trivially_copyable_v<DoWinMainFunc>, "DoWinMainFunc must be trivially copyable for bit_cast");

        auto do_win_main = std::bit_cast<DoWinMainFunc>(proc);
        if (!do_win_main) [[unlikely]]
            return std::unexpected(LoadResult::FunctionError);

        try
        {
            return do_win_main(inst, prev, safe_cmd.data(), show);
        }
        catch (...)
        {
            AddReportLog(5722, "Launcher Main: Exception in DoWinMain");
            return std::unexpected(LoadResult::FunctionError);
        }
    }

    // Path validation
    [[nodiscard]] constexpr bool ValidatePathSafety(std::string_view path) noexcept
    {
        return path.find("..") == std::string_view::npos && path.find("//") == std::string_view::npos;
    }

    // Path discovery with validation
    [[nodiscard]] std::expected<SString, LoadResult> FindMtaPath(const SString& launch_path)
    {
        if (!ValidatePathSafety(launch_path)) [[unlikely]]
        {
            AddReportLog(5731, std::format("Launcher Main: Invalid launch path: '{}'", launch_path));
            return std::unexpected(LoadResult::PathError);
        }

        SString mta_path = PathJoin(launch_path, "mta");
        if (!ValidatePathSafety(mta_path)) [[unlikely]]
            return std::unexpected(LoadResult::PathError);

        if (DirectoryExists(mta_path))
            return mta_path;

        SString parent = PathJoin(launch_path, "..");
        if (!ValidatePathSafety(parent)) [[unlikely]]
            return std::unexpected(LoadResult::PathError);

        SString alt = PathJoin(parent, "mta");

        if (ValidatePathSafety(alt) && DirectoryExists(alt))
            return alt;

        AddReportLog(5730, std::format("Launcher Main: MTA directory not found from: '{}'", launch_path));
        return std::unexpected(LoadResult::PathError);
    }
}            // namespace mta::launcher

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    using namespace mta::launcher;

    // Configure silent error handling
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

    // COM initialization
    auto com_result = InitializeCOM();
    if (!com_result) [[unlikely]]
    {
        AddReportLog(5737, std::format("Launcher Main: COM initialization failed (hr=0x{:08X})", static_cast<DWORD>(com_result.error())));
    }

    auto com_cleanup = make_scope_exit([]() noexcept { CoUninitialize(); });

    // Set taskbar grouping
    [[maybe_unused]] HRESULT hr = SetCurrentProcessExplicitAppUserModelID(L"Multi Theft Auto " MTA_STR(MTASA_VERSION_MAJOR) L"." MTA_STR(MTASA_VERSION_MINOR));

    // Path discovery
    SString launch_path     = GetLaunchPath();
    auto    mta_path_result = FindMtaPath(launch_path);

    if (!mta_path_result) [[unlikely]]
    {
        return std::to_underlying(mta_path_result.error());
    }

    SString mta_path = mta_path_result.value();
    SString dll_path = PathJoin(mta_path, SString{LOADER_DLL.data(), LOADER_DLL.size()});

    AddReportLog(5725, std::format("Launcher Main: Launch: '{}', MTA: '{}'", launch_path, mta_path));

    // DLL loading
    auto module_result = LoadLibrarySafe(dll_path, LOADER_DLL, mta_path);

    if (!module_result) [[unlikely]]
    {
        DWORD   error = GetLastError();
        SString msg   = std::format("Launcher Main: Failed to load: '{}'\n\n{}", dll_path, GetSystemErrorMessage(error));
        AddReportLog(5711, msg);

        // Pattern matching
        switch (module_result.error())
        {
            case DllLoadError::FileNotFound:
                AddReportLog(5734, "Launcher Main: Build the 'Loader' project to create loader_d.dll");
                BrowseToSolution("loader-dll-missing", ASK_GO_ONLINE, msg);
                break;
            case DllLoadError::InvalidPath:
            case DllLoadError::LoadFailed:
                BrowseToSolution("loader-dll-not-loadable", ASK_GO_ONLINE, msg);
                break;
        }

        return std::to_underlying(LoadResult::LoadError);
    }

    HMODULE module = module_result.value();

    // RAII module cleanup
    auto module_cleanup = make_scope_exit([module]() noexcept {
        if (module)
            FreeLibrary(module);
    });

    // Execution with concepts
    auto exec_result = ExecuteDoWinMain(module, hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    return exec_result.value_or(std::to_underlying(LoadResult::FunctionError));
}
