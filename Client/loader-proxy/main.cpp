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

#include <array>
#include <string>
#include <filesystem>
#include <windows.h>
#include <shlobj.h>

namespace fs = std::filesystem;

void SetupLibraryExports(HMODULE Handle) noexcept;
void DisplayErrorMessageBox(const std::wstring& message, const std::wstring& errorCode);
auto GetProcessPath() -> fs::path;
auto LoadWinmmLibrary() -> std::pair<HMODULE, DWORD>;

static HMODULE core{};
static DWORD   winmmSource = 0;

static auto winmm = ([]() -> HMODULE {
    auto [winmm, source] = LoadWinmmLibrary();

    if (!winmm)
    {
        DisplayErrorMessageBox(
            L"Loading system-provided winmm.dll failed.\n\n"
            L"Please ensure that your Windows installation\n"
            L"is not missing files and your user is not lacking\n"
            L"any permission to access the system directory.",
            L"CL52");
        
        return nullptr;
    }

    SetupLibraryExports(winmm);
    winmmSource = source;
    return winmm;
})();

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        if (!winmm)
        {
            DisplayErrorMessageBox(
                L"Loading system-provided winmm.dll failed.\n\n"
                L"Please ensure that your Windows installation\n"
                L"is not missing files and your user is not lacking\n"
                L"any permission to access the system directory.",
                L"CL50");

            return FALSE;
        }

        std::string processName = GetProcessPath().filename().string();

        if (processName == "gta_sa.exe" || processName == "proxy_sa.exe")
        {
#ifdef MTA_DEBUG
            core = LoadLibraryW(L"core_d.dll");
#else
            core = LoadLibraryW(L"core.dll");
#endif

            if (!core)
            {
#ifdef MTA_DEBUG
                DisplayErrorMessageBox(
                    L"Loading core failed. Please ensure that\n"
                    L"the latest DirectX is correctly installed and you executed win-install-data.bat",
                    L"CL24");
#else
                DisplayErrorMessageBox(
                    L"Loading core failed. Please ensure that\n"
                    L"the latest DirectX is correctly installed.",
                    L"CL24");
#endif

                return FALSE;
            }
        }
        else
        {
            DisplayErrorMessageBox(
                    L"Loading core failed. Please ensure that\n"
                    L"you installed and launched MTA:SA properly.",
                    L"CL51");
        }
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        if (core)
        {
            FreeLibrary(core);
            core = nullptr;
        }

        if (winmm)
        {
            FreeLibrary(winmm);
            winmm = nullptr;
        }
    }

    return TRUE;
}

extern "C" __declspec(dllexport) HMODULE mtasaGetLibraryHandle()
{
    return winmm;
}

extern "C" __declspec(dllexport) DWORD mtasaGetLibrarySource()
{
    return winmmSource;
}

void DisplayErrorMessageBox(const std::wstring& message, const std::wstring& errorCode)
{
    std::wstring caption = L"MTA: San Andreas  [" + errorCode + L"]   (CTRL+C to copy)";
    MessageBoxW(nullptr, message.c_str(), caption.c_str(), MB_OK | MB_TOPMOST | MB_ICONWARNING);
    TerminateProcess(GetCurrentProcess(), 1);
}

auto GetProcessPath() -> fs::path
{
    std::wstring filePath(4096, L'\0');

    while (true)
    {
        DWORD length = GetModuleFileNameW(nullptr, filePath.data(), static_cast<DWORD>(filePath.size() + 1));

        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            filePath.resize(filePath.capacity() * 2);
            continue;
        }

        filePath.resize(length);
        return {filePath};
    }
}

auto GetKnownFolderPath(REFKNOWNFOLDERID id, DWORD flags = 0) -> fs::path
{
    wchar_t* path{};

    if (HRESULT hr = SHGetKnownFolderPath(id, flags, nullptr, &path); SUCCEEDED(hr))
    {
        fs::path result(path);
        CoTaskMemFree(path);
        return result;
    }

    return {};
}

auto GetSystemWinmmPath() -> fs::path
{
    std::error_code ec{};
    fs::path        systemPath = GetKnownFolderPath(FOLDERID_SystemX86);

    if (systemPath.empty() || !std::filesystem::is_directory(systemPath, ec))
        return {};

    fs::path winmmPath = systemPath / "winmm.dll";

    if (!fs::is_regular_file(winmmPath, ec))
        return {};

    return winmmPath;
}

auto LoadWinmmLibrary() -> std::pair<HMODULE, DWORD>
{
    HMODULE winmm{};

    if (winmm = LoadLibraryExW(L"winmm.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32))
        return {winmm, 1};

    if (fs::path winmmPath = GetSystemWinmmPath(); !winmmPath.empty())
    {
        if (winmm = LoadLibraryW(winmmPath.wstring().c_str()))
            return {winmm, 2};
    }

    if (winmm = LoadLibraryW(L"winmm.dll"))
        return {winmm, 3};

    return {nullptr, 0};
}
