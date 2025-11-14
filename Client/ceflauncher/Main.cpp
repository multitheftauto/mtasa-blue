/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        ceflauncher/Main.cpp
 *  PURPOSE:     CEF launcher entry point
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

/*
    IMPORTANT

    If this project changes, a new release build should be copied into
    the Shared\data\launchers directory.

    The .exe in Shared\data\launchers will be used by the installer and updater.

    (set flag.new_cef_exe on the build server to generate new exe)
*/

#include <cstdlib>
#include <functional>

struct HINSTANCE__;
using HINSTANCE = HINSTANCE__*;
using LPSTR = char*;

extern "C" [[nodiscard("InitCEF return value must be used")]] __declspec(dllimport) int __cdecl InitCEF();

// Users are faced with vague crashes in CEFLauncher.exe, so rather than over-engineering all this is intended
// Do note that CEFLauncher.exe ends up hosting any GPU rendering processes of CEF
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    const auto init = std::function<int()>{InitCEF};
    const auto safe_invoke = [&]() noexcept -> int
    {
        try
        {
            return std::invoke(init);
        }
        catch (...)
        {
            return EXIT_FAILURE;
        }
    };
    return safe_invoke();
}
