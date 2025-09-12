/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        launch/Main.cpp
 *  PURPOSE:     Unchanging .exe that doesn't change
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <version.h>
#include <expected>
#include <string_view>
#include <format>

/*
    IMPORTANT

    If this project changes, a new release build should be copied into
    the Shared\data\launchers directory.

    The .exe in Shared\data\launchers will be used by the installer and updater.

    (set flag.new_client_exe on the build server to generate new exe)
*/

namespace mta::launcher {
 
    enum class LoadResult : std::uint32_t {
        Success = 0,
        PathError = 1,
        LoadError = 2,
        FunctionError = 3
    };

    constexpr const char* DEBUG_LOADER_DLL = "loader_d.dll";
    constexpr const char* RELEASE_LOADER_DLL = "loader.dll";
    constexpr const char* DOWINMAIN_FUNCTION = "DoWinMain";

    [[nodiscard]] std::expected<std::wstring_view, LoadResult> 
    ValidateUTF8Conversion(const wchar_t* converted_path, const SString& original_path) {
        if (!converted_path || std::wcslen(converted_path) == 0) [[unlikely]] {
            AddReportLog(5723, std::format("Launcher Main: Failed to convert DLL path to Unicode: '{}'", original_path.c_str()));
            return std::unexpected(LoadResult::PathError);
        }
        return std::wstring_view{converted_path};
    }

    struct DirectoryState {
        bool changed = false;
        bool dll_directory_changed = false;
    };

    [[nodiscard]] std::expected<HMODULE, DWORD> 
    LoadLibraryWithFallback(const std::wstring_view& dll_path, 
                           const SString& dll_filename,
                           const SString& mta_path, 
                           const SString& original_directory,
                           DirectoryState& dir_state) {
        
        // First attempt - try loading from calculated path without changing directories
        HMODULE module = LoadLibraryW(dll_path.data());
        DWORD error = GetLastError();
        
        if (module) [[likely]] {
            return module;
        }

        // Check if the DLL file actually exists before trying to change directories
        SString dll_path_str = ToUTF8(std::wstring(dll_path));
        if (!FileExists(dll_path_str)) {
            AddReportLog(5717, std::format("Launcher Main: DLL file does not exist: '{}'", dll_path_str.c_str()));
            return std::unexpected(error);
        }

        // Only proceed with directory changes if the MTA directory exists
        if (!DirectoryExists(mta_path)) {
            AddReportLog(5717, std::format("Launcher Main: MTA directory does not exist: '{}'", mta_path.c_str()));
            return std::unexpected(error);
        }

        if (original_directory.empty()) {
            AddReportLog(5716, "Launcher Main: Original directory is empty, cannot safely change directory");
            return std::unexpected(error);
        }

        // Validate UTF8 conversions before directory operations
        WString mta_path_wstring = FromUTF8(mta_path);
        WString dll_filename_wstring = FromUTF8(dll_filename);
        
        if (mta_path_wstring.empty() || dll_filename_wstring.empty()) [[unlikely]] {
            AddReportLog(5724, std::format("Launcher Main: Failed to convert paths to Unicode: MTA='{}', DLL='{}'", 
                        mta_path.c_str(), dll_filename.c_str()));
            return std::unexpected(error);
        }

        const wchar_t* mta_path_wide = mta_path_wstring.c_str();
        const wchar_t* dll_filename_wide = dll_filename_wstring.c_str();

        // Try loading with SetDllDirectory first without changing current directory
        if (SetDllDirectoryW(mta_path_wide)) {
            dir_state.dll_directory_changed = true;
            
            module = LoadLibraryW(dll_filename_wide);
            if (module) [[likely]] {
                AddReportLog(5712, std::format("Launcher Main: LoadLibrary '{}' succeeded with SetDllDirectory to '{}'", 
                            dll_filename.c_str(), mta_path.c_str()));
                return module;
            }
        }

        // If that didn't work, try changing both current directory and DLL search directory
        if (!SetCurrentDirectoryW(mta_path_wide)) {
            DWORD directory_error = GetLastError();
            AddReportLog(5718, std::format("Launcher Main: Failed to change directory to: '{}' (error: {})", 
                        mta_path.c_str(), directory_error));
            return std::unexpected(error);
        }

        dir_state.changed = true;
        
        module = LoadLibraryW(dll_filename_wide);
        DWORD second_attempt_error = GetLastError();
        
        if (module) [[likely]] {
            AddReportLog(5712, std::format("Launcher Main: LoadLibrary '{}' succeeded on change to directory '{}'", 
                        dll_filename.c_str(), mta_path.c_str()));
            return module;
        }

        return std::unexpected(second_attempt_error);
    }

    // Helper function for error mode management
    class ErrorModeGuard {
        DWORD prev_mode;
    public:
        explicit ErrorModeGuard(DWORD mode) : prev_mode(SetErrorMode(mode)) {}
        ~ErrorModeGuard() { SetErrorMode(prev_mode); }
        
        // Delete copy operations for RAII safety
        ErrorModeGuard(const ErrorModeGuard&) = delete;
        ErrorModeGuard& operator=(const ErrorModeGuard&) = delete;
        ErrorModeGuard(ErrorModeGuard&&) = delete;
        ErrorModeGuard& operator=(ErrorModeGuard&&) = delete;
    };

    // Function for executing DoWinMain (with fallback to old approach)
    [[nodiscard]] std::expected<int, LoadResult> 
    ExecuteDoWinMain(HMODULE module, HINSTANCE instance, HINSTANCE prev_instance, 
                    LPSTR cmd_line, int show_cmd, const SString& dll_filename) noexcept {
        
        using DoWinMainFunc = int(*)(HINSTANCE, HINSTANCE, LPSTR, int);

        FARPROC proc_address = GetProcAddress(module, DOWINMAIN_FUNCTION);
        
        if (!proc_address) [[unlikely]] {
            DWORD error = GetLastError();
            AddReportLog(5713, std::format("Launcher Main: GetProcAddress failed for DoWinMain in '{}' (error: {})", 
                        dll_filename.c_str(), error));
            return std::unexpected(LoadResult::FunctionError);
        }

        auto do_win_main = reinterpret_cast<DoWinMainFunc>(proc_address);

        // Use old approach for maximum compatibility
        int result = 1; // Default error result
        
        // Store original error mode to restore after function call
        DWORD prev_error_mode = SetErrorMode(SEM_FAILCRITICALERRORS);
        
        // Call DoWinMain directly
        try {
            result = do_win_main(instance, prev_instance, cmd_line, show_cmd);
        } catch (...) {

            AddReportLog(5722, "Launcher Main: Exception occurred during DoWinMain execution");
            SetErrorMode(prev_error_mode);
            return std::unexpected(LoadResult::FunctionError);
        }
        
        SetErrorMode(prev_error_mode);
        return result;
    }

    class DirectoryRestorer {
        DirectoryState& state;
        SString original_directory;

    public:
        DirectoryRestorer(DirectoryState& dir_state, const SString& orig_dir) 
            : state(dir_state), original_directory(orig_dir) {}

        ~DirectoryRestorer() {
            // Restore DLL directory if it was changed
            if (state.dll_directory_changed) {
                // Reset DLL directory to system default cuz we can't retrieve the original DLL directory
                // (Windows API provides no GetDllDirectory function to save the original state)
                SetDllDirectoryW(nullptr);
            }

            // Restore original directory if it was changed
            if (state.changed && !original_directory.empty()) {
                WString original_directory_wstring = FromUTF8(original_directory);
                if (!original_directory_wstring.empty()) {
                    if (!SetCurrentDirectoryW(original_directory_wstring.c_str())) {
                        AddReportLog(5721, std::format("Launcher Main: Failed to restore original directory: '{}' (error: {})", 
                                    original_directory.c_str(), GetLastError()));
                    }
                } else {
                    AddReportLog(5729, std::format("Launcher Main: Failed to convert original directory to Unicode: '{}'", 
                                original_directory.c_str()));
                }
            }
        }

        // Delete copy operations for RAII safety
        DirectoryRestorer(const DirectoryRestorer&) = delete;
        DirectoryRestorer& operator=(const DirectoryRestorer&) = delete;
        DirectoryRestorer(DirectoryRestorer&&) = delete;
        DirectoryRestorer& operator=(DirectoryRestorer&&) = delete;
    };

    // RAII module guard
    class ModuleGuard {
        HMODULE& module_ref;
    public:
        explicit ModuleGuard(HMODULE& module) : module_ref(module) {}
        ~ModuleGuard() {
            if (module_ref) {
                FreeLibrary(module_ref);
                module_ref = nullptr;
            }
        }
        
        ModuleGuard(const ModuleGuard&) = delete;
        ModuleGuard& operator=(const ModuleGuard&) = delete;
        ModuleGuard(ModuleGuard&&) = delete;
        ModuleGuard& operator=(ModuleGuard&&) = delete;
    };

} // namespace mta::launcher

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    using namespace mta::launcher;
    
    // Group our processes and windows under a single taskbar button
    SetCurrentProcessExplicitAppUserModelID(L"Multi Theft Auto " MTA_STR(MTASA_VERSION_MAJOR) L"." MTA_STR(MTASA_VERSION_MINOR));


    SString loader_dll_filename;
#ifdef MTA_DEBUG
    loader_dll_filename = DEBUG_LOADER_DLL;
#else
    loader_dll_filename = RELEASE_LOADER_DLL;
#endif

    // Path operations
    auto mta_path_result = PathJoin(GetLaunchPath(), "mta");
    if (mta_path_result.empty()) [[unlikely]] {
        AddReportLog(5714, "Launcher Main: Failed to determine MTA path");
        return static_cast<int>(LoadResult::PathError);
    }

    // Debug: Log the actual paths being used
    auto launch_path = GetLaunchPath();
    AddReportLog(5725, std::format("Launcher Main: Launch path: '{}', MTA path: '{}'", launch_path.c_str(), mta_path_result.c_str()));
    
    // Check if MTA directory exists, if not, try alternative locations
    if (!DirectoryExists(mta_path_result)) {
        // Build complex paths step by step to avoid PathJoin argument limits
        std::vector<SString> alternative_paths;
        SString up_one = PathJoin(launch_path, "..");
        SString repo_root = PathJoin(up_one, "..");
        SString current_dir = GetSystemCurrentDirectory();
        SString current_up_one = PathJoin(current_dir, "..");
        
        alternative_paths.push_back(PathJoin(up_one, "Bin", "mta"));                      // Up to repo root, then Bin/mta  
        alternative_paths.push_back(PathJoin(repo_root, "Bin", "mta"));                   // Up from Build dir to repo root, then Bin/mta
        alternative_paths.push_back(PathJoin(up_one, "Output", "mta"));                   // Up one level, then Output/mta
        alternative_paths.push_back(PathJoin(repo_root, "Output", "mta"));                // Up from Build dir to repo root, then Output/mta
        alternative_paths.push_back(PathJoin(current_dir, "mta"));                        // Current working directory
        alternative_paths.push_back(PathJoin(current_dir, "Bin", "mta"));                 // Current working directory + Bin
        alternative_paths.push_back(PathJoin(current_up_one, "Bin", "mta"));              // Up from current + Bin
        alternative_paths.push_back(PathJoin(ExtractPath(launch_path), "mta"));           // Parent directory of launcher
        
        for (const auto& alt_path : alternative_paths) {
            SString normalized_path = PathConform(alt_path);  // Normalize the path
            if (DirectoryExists(normalized_path)) {
                AddReportLog(5726, std::format("Launcher Main: Found alternative MTA path: '{}'", normalized_path.c_str()));
                mta_path_result = normalized_path;
                break;
            }
        }
        
        // If still not found, log all attempted paths for further investigation
        if (!DirectoryExists(mta_path_result)) {
            AddReportLog(5730, "Launcher Main: MTA directory not found in any location. Attempted paths:");
            for (const auto& alt_path : alternative_paths) {
                SString normalized_path = PathConform(alt_path);
                AddReportLog(5731, std::format("  - '{}'", normalized_path.c_str()));
            }
        }
    }

    auto loader_dll_path_result = PathJoin(mta_path_result, loader_dll_filename);
    if (loader_dll_path_result.empty()) [[unlikely]] {
        AddReportLog(5715, std::format("Launcher Main: Failed to construct loader DLL path: base='{}', file='{}'", 
                    mta_path_result.c_str(), loader_dll_filename.c_str()));
        return static_cast<int>(LoadResult::PathError);
    }
    
    // Check if the DLL file actually exists
    if (!FileExists(loader_dll_path_result)) {
        AddReportLog(5727, std::format("Launcher Main: Loader DLL does not exist at expected path: '{}'", loader_dll_path_result.c_str()));
        
        // Create intermediate path variables to avoid PathJoin argument limits
        std::vector<SString> dll_search_paths;
        SString launch_mta = PathJoin(launch_path, "mta");
        SString up_one = PathJoin(launch_path, "..");
        SString repo_root = PathJoin(up_one, "..");
        SString current_dir = GetSystemCurrentDirectory();
        SString current_up = PathJoin(current_dir, "..");
        
        // Same directory as launcher
        dll_search_paths.push_back(PathJoin(launch_path, loader_dll_filename));
        
        // Expected location: launch_path/mta/
        dll_search_paths.push_back(PathJoin(launch_mta, loader_dll_filename));
        
        // Up one, then Bin
        dll_search_paths.push_back(PathJoin(PathJoin(up_one, "Bin"), loader_dll_filename));
        
        // Up one, then Bin/mta
        dll_search_paths.push_back(PathJoin(PathJoin(up_one, "Bin", "mta"), loader_dll_filename));
        
        // Up from Build to repo root, then Bin
        dll_search_paths.push_back(PathJoin(PathJoin(repo_root, "Bin"), loader_dll_filename));
        
        // Up from Build to repo root, then Bin/mta  
        dll_search_paths.push_back(PathJoin(PathJoin(repo_root, "Bin", "mta"), loader_dll_filename));
        
        // Up one, then Output
        dll_search_paths.push_back(PathJoin(PathJoin(up_one, "Output"), loader_dll_filename));
        
        // Up one, then Output/mta
        dll_search_paths.push_back(PathJoin(PathJoin(up_one, "Output", "mta"), loader_dll_filename));
        
        // Up from Build to repo root, then Output
        dll_search_paths.push_back(PathJoin(PathJoin(repo_root, "Output"), loader_dll_filename));
        
        // Up from Build to repo root, then Output/mta
        dll_search_paths.push_back(PathJoin(PathJoin(repo_root, "Output", "mta"), loader_dll_filename));
        
        // Current working directory
        dll_search_paths.push_back(PathJoin(current_dir, loader_dll_filename));
        
        // Current working directory + mta
        dll_search_paths.push_back(PathJoin(PathJoin(current_dir, "mta"), loader_dll_filename));
        
        // Current working directory + Bin
        dll_search_paths.push_back(PathJoin(PathJoin(current_dir, "Bin"), loader_dll_filename));
        
        // Current working directory + Bin/mta
        dll_search_paths.push_back(PathJoin(PathJoin(current_dir, "Bin", "mta"), loader_dll_filename));
        
        // Up from current + Bin
        dll_search_paths.push_back(PathJoin(PathJoin(current_up, "Bin"), loader_dll_filename));
        
        // Up from current + Bin/mta
        dll_search_paths.push_back(PathJoin(PathJoin(current_up, "Bin", "mta"), loader_dll_filename));
        
        bool found_dll = false;
        for (const auto& search_path : dll_search_paths) {
            SString normalized_path = PathConform(search_path);
            if (FileExists(normalized_path)) {
                AddReportLog(5728, std::format("Launcher Main: Found loader DLL at alternative location: '{}'", normalized_path.c_str()));
                loader_dll_path_result = normalized_path;
                mta_path_result = ExtractPath(normalized_path);  // Update MTA path to match
                found_dll = true;
                break;
            }
        }
        
        // If still not found, log all attempted DLL paths for debugging
        if (!found_dll) {
            AddReportLog(5732, "Launcher Main: Loader DLL not found in any location. Tried paths:");
            for (const auto& search_path : dll_search_paths) {
                SString normalized_path = PathConform(search_path);
                AddReportLog(5733, std::format("  - '{}'", normalized_path.c_str()));
            }
            
            // Also suggest building the Loader project
            AddReportLog(5734, "Launcher Main: Build the 'Loader' project to create the missing loader_d.dll file");
        }
    }

    // RAII for error mode management
    ErrorModeGuard error_mode_guard(SEM_FAILCRITICALERRORS);
    auto original_directory = GetSystemCurrentDirectory();
    
    DirectoryState dir_state{};
    DirectoryRestorer directory_restorer{dir_state, original_directory};

    // Validate UTF8 conversion before first LoadLibrary attempt
    WString loader_dll_path_wstring = FromUTF8(loader_dll_path_result);
    auto validation_result = ValidateUTF8Conversion(loader_dll_path_wstring.c_str(), loader_dll_path_result);
    
    if (!validation_result) [[unlikely]] {
        return static_cast<int>(validation_result.error());
    }

    auto load_result = LoadLibraryWithFallback(
        validation_result.value(),
        loader_dll_filename,
        mta_path_result,
        original_directory,
        dir_state
    );

    if (!load_result) [[unlikely]] {
        // Failed to load the library
        DWORD load_library_error = load_result.error();
        auto error_message = GetSystemErrorMessage(load_library_error);
        auto message = std::format("Failed to load: '{}'\n\n{}", 
                                 loader_dll_path_result.c_str(), error_message.c_str());
        
        // Add to report log for debugging
        AddReportLog(5711, message);

        // Validate that the file actually exists before showing missing VC Redist message
        if (FileExists(loader_dll_path_result)) {
            // Error could be due to missing VC Redist or dependency issues
            // Online help page will have VC Redist download link.
            BrowseToSolution("loader-dll-not-loadable", ASK_GO_ONLINE, message);
        } else {
            // The DLL file itself doesn't exist
            auto missing_file_message = std::format("Missing required file: '{}'", 
                                                   loader_dll_path_result.c_str());
            AddReportLog(5720, missing_file_message);
            BrowseToSolution("loader-dll-missing", ASK_GO_ONLINE, missing_file_message);
        }

        return static_cast<int>(LoadResult::LoadError);
    }

    HMODULE module = load_result.value();
    ModuleGuard module_guard{module};

    auto execution_result = ExecuteDoWinMain(
        module, hInstance, hPrevInstance, lpCmdLine, nCmdShow, loader_dll_filename
    );

    if (execution_result) [[likely]] {
        return execution_result.value();
    } else {
        return static_cast<int>(execution_result.error());
    }
}
