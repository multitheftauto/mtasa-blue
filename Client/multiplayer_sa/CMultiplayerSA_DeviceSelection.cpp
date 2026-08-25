/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        multiplayer_sa/CMultiplayerSA_DeviceSelection.cpp
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#include <cstring>
#include <vector>
#include <algorithm>
#include <game/CSettings.h>

#define FUNC_rwDeviceSystemRequest    0x7F2AB0
#define FUNC_DialogFunc               0x745E50
#define FUNC_RwEngineGetSubSystemInfo 0x7F2C30
#define FUNC_RwEngineGetNumVideoModes 0x7F2CC0
#define FUNC_RwEngineGetVideoModeInfo 0x7F2CF0
#define CLASS_RwGlobals               0xC97B24
#define CLASS_IDirect3D9              0xC97C20
#define VAR_SavedVideoMode            0xBA6820
#define VAR_CurVideoMode              0x8D6220
#define IDC_DEVICE                    1000
#define IDC_VIDMODE                   1001

#if defined(_MSC_VER)
    #pragma comment( \
        linker, \
        "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

INT_PTR CALLBACK CustomDlgProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);

//
// Query friendly monitor display names from active display paths via Windows CCD APIs.
// Dynamically imported to ensure backwards compatibility across all Windows versions.
//
std::unordered_map<std::string, std::string> GetFriendlyMonitorNamesForDevicePaths()
{
    std::unordered_map<std::string, std::string> monitorNames;

    HMODULE user32Module = LoadLibrary(TEXT("user32"));
    if (!user32Module)
        return monitorNames;

    using GetDisplayConfigBufferSizesFn = decltype(&GetDisplayConfigBufferSizes);
    using QueryDisplayConfigFn = decltype(&QueryDisplayConfig);
    using DisplayConfigGetDeviceInfoFn = decltype(&DisplayConfigGetDeviceInfo);

    GetDisplayConfigBufferSizesFn getDisplayConfigBufferSizes = nullptr;
    QueryDisplayConfigFn          queryDisplayConfig = nullptr;
    DisplayConfigGetDeviceInfoFn  displayConfigGetDeviceInfo = nullptr;

    const auto procGetDisplayConfigBufferSizes = GetProcAddress(user32Module, "GetDisplayConfigBufferSizes");
    const auto procQueryDisplayConfig = GetProcAddress(user32Module, "QueryDisplayConfig");
    const auto procDisplayConfigGetDeviceInfo = GetProcAddress(user32Module, "DisplayConfigGetDeviceInfo");

    static_assert(sizeof(getDisplayConfigBufferSizes) == sizeof(procGetDisplayConfigBufferSizes), "Unexpected function pointer size");
    static_assert(sizeof(queryDisplayConfig) == sizeof(procQueryDisplayConfig), "Unexpected function pointer size");
    static_assert(sizeof(displayConfigGetDeviceInfo) == sizeof(procDisplayConfigGetDeviceInfo), "Unexpected function pointer size");

    if (procGetDisplayConfigBufferSizes)
        std::memcpy(&getDisplayConfigBufferSizes, &procGetDisplayConfigBufferSizes, sizeof(getDisplayConfigBufferSizes));
    if (procQueryDisplayConfig)
        std::memcpy(&queryDisplayConfig, &procQueryDisplayConfig, sizeof(queryDisplayConfig));
    if (procDisplayConfigGetDeviceInfo)
        std::memcpy(&displayConfigGetDeviceInfo, &procDisplayConfigGetDeviceInfo, sizeof(displayConfigGetDeviceInfo));
    if (!getDisplayConfigBufferSizes || !queryDisplayConfig || !displayConfigGetDeviceInfo)
    {
        FreeLibrary(user32Module);
        return monitorNames;
    }

    UINT32                                     pathCount, modeCount;
    std::unique_ptr<DISPLAYCONFIG_PATH_INFO[]> paths;
    std::unique_ptr<DISPLAYCONFIG_MODE_INFO[]> modes;

    LONG result = ERROR_SUCCESS;
    do
    {
        result = getDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount);
        if (result != ERROR_SUCCESS)
        {
            break;
        }
        paths = std::make_unique<DISPLAYCONFIG_PATH_INFO[]>(pathCount);
        modes = std::make_unique<DISPLAYCONFIG_MODE_INFO[]>(modeCount);
        result = queryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.get(), &modeCount, modes.get(), nullptr);
    } while (result == ERROR_INSUFFICIENT_BUFFER);

    if (result != ERROR_SUCCESS)
    {
        FreeLibrary(user32Module);
        return monitorNames;
    }

    for (size_t index = 0; index < pathCount; index++)
    {
        DISPLAYCONFIG_TARGET_DEVICE_NAME targetName = {};
        targetName.header.adapterId = paths[index].targetInfo.adapterId;
        targetName.header.id = paths[index].targetInfo.id;
        targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        targetName.header.size = sizeof(targetName);
        const LONG targetNameResult = displayConfigGetDeviceInfo(&targetName.header);

        DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {};
        sourceName.header.adapterId = paths[index].sourceInfo.adapterId;
        sourceName.header.id = paths[index].sourceInfo.id;
        sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
        sourceName.header.size = sizeof(sourceName);
        const LONG sourceNameResult = displayConfigGetDeviceInfo(&sourceName.header);
        if (targetNameResult == ERROR_SUCCESS && sourceNameResult == ERROR_SUCCESS && targetName.monitorFriendlyDeviceName[0] != '\0')
        {
            char gdiDeviceName[std::size(sourceName.viewGdiDeviceName)];
            char monitorFriendlyDeviceName[std::size(targetName.monitorFriendlyDeviceName)];
            WideCharToMultiByte(CP_ACP, 0, sourceName.viewGdiDeviceName, -1, gdiDeviceName, static_cast<int>(std::size(gdiDeviceName)), nullptr, nullptr);
            WideCharToMultiByte(CP_ACP, 0, targetName.monitorFriendlyDeviceName, -1, monitorFriendlyDeviceName,
                                static_cast<int>(std::size(monitorFriendlyDeviceName)), nullptr, nullptr);

            monitorNames.try_emplace(gdiDeviceName, monitorFriendlyDeviceName);
        }
    }

    FreeLibrary(user32Module);
    return monitorNames;
}

struct RwSubSystemInfo
{
    char name[80];
};

using rwDeviceSystemRequest = RwSubSystemInfo*(__cdecl*)(RwDevice * device, std::int32_t requestId, RwSubSystemInfo* pOut, void* pInOut, std::int32_t numIn);
static RwSubSystemInfo* RwEngineGetSubSystemInfo_Hooked(RwSubSystemInfo* subSystemInfo, std::int32_t subSystemIndex)
{
    auto* rwGlobals = *(RwGlobals**)CLASS_RwGlobals;
    auto* rwDeviceSystemRequestFunc = (rwDeviceSystemRequest)(FUNC_rwDeviceSystemRequest);
    if (!rwDeviceSystemRequestFunc(&rwGlobals->dOpenDevice, 14, subSystemInfo, nullptr, subSystemIndex))
        return nullptr;

    auto* direct3DDevice = *(IDirect3D9**)CLASS_IDirect3D9;
    if (!direct3DDevice)
        return subSystemInfo;

    D3DADAPTER_IDENTIFIER9 identifier;
    if (FAILED(direct3DDevice->GetAdapterIdentifier(subSystemIndex, 0, &identifier)))
        return subSystemInfo;

    static const auto friendlyNames = GetFriendlyMonitorNamesForDevicePaths();

    // If we cannot find the friendly name (or running on older Windows), fall back to the adapter description
    auto iterator = friendlyNames.find(identifier.DeviceName);
    if (iterator != friendlyNames.end())
    {
        strncpy_s(subSystemInfo->name, iterator->second.c_str(), _TRUNCATE);
    }
    else
    {
        strncpy_s(subSystemInfo->name, identifier.Description, _TRUNCATE);
    }

    return subSystemInfo;
}

struct DialogResolutionItem
{
    int          width;
    int          height;
    int          depth;
    bool         isWidescreen;
    unsigned int vidModeIndex;
    int          refRate;
};

//
// Populate and sort resolution dropdown in the native GTA:SA device selection dialog.
// Direct3D 9 enumerates multiple entries for different refresh rates without showing Hz in the dialog text.
// We deduplicate identical resolutions (keeping the highest refresh rate) and sort them in descending order.
//
static void PopulateAndSortResolutionComboBox(HWND window)
{
    HWND modeComboBox = GetDlgItem(window, IDC_VIDMODE);
    if (!modeComboBox)
        return;

    using RwEngineGetNumVideoModes_t = unsigned int(__cdecl*)();
    using RwEngineGetVideoModeInfo_t = VideoMode*(__cdecl*)(VideoMode*, unsigned int);

    auto RwEngineGetNumVideoModes = reinterpret_cast<RwEngineGetNumVideoModes_t>(FUNC_RwEngineGetNumVideoModes);
    auto RwEngineGetVideoModeInfo = reinterpret_cast<RwEngineGetVideoModeInfo_t>(FUNC_RwEngineGetVideoModeInfo);

    unsigned int numModes = RwEngineGetNumVideoModes();
    if (numModes == 0)
        return;

    std::vector<DialogResolutionItem> resolutions;
    resolutions.reserve(numModes);

    for (unsigned int modeIndex = 0; modeIndex < numModes; ++modeIndex)
    {
        VideoMode modeInfo{};
        if (!RwEngineGetVideoModeInfo(&modeInfo, modeIndex))
            continue;

        // Skip non-exclusive and unusable tiny resolutions
        if (!(modeInfo.flags & rwVIDEOMODEEXCLUSIVE) || modeInfo.width < 640 || modeInfo.height < 480)
            continue;

        bool isWidescreen = (modeInfo.flags & rwVIDEOMODE_XBOX_WIDESCREEN) != 0;

        // Deduplicate identical resolutions; if a duplicate has a higher refresh rate, use its mode index
        bool isDuplicate = false;
        for (auto& existing : resolutions)
        {
            if (existing.width == modeInfo.width && existing.height == modeInfo.height && existing.depth == modeInfo.depth &&
                existing.isWidescreen == isWidescreen)
            {
                if (modeInfo.refRate > existing.refRate)
                {
                    existing.vidModeIndex = modeIndex;
                    existing.refRate = modeInfo.refRate;
                }
                isDuplicate = true;
                break;
            }
        }

        if (!isDuplicate)
        {
            resolutions.push_back({modeInfo.width, modeInfo.height, modeInfo.depth, isWidescreen, modeIndex, modeInfo.refRate});
        }
    }

    if (resolutions.empty())
        return;

    // Sort resolutions descending by width, height, depth, and aspect ratio
    std::sort(resolutions.begin(), resolutions.end(),
              [](const DialogResolutionItem& first, const DialogResolutionItem& second)
              {
                  if (first.width != second.width)
                      return first.width > second.width;
                  if (first.height != second.height)
                      return first.height > second.height;
                  if (first.depth != second.depth)
                      return first.depth > second.depth;
                  return first.isWidescreen > second.isWidescreen;
              });

    // Determine target video mode to preserve current/saved selection
    unsigned int savedVideoMode = *reinterpret_cast<unsigned int*>(VAR_SavedVideoMode);
    unsigned int currentVideoMode = *reinterpret_cast<unsigned int*>(VAR_CurVideoMode);
    unsigned int targetVideoMode = (savedVideoMode > 0 && savedVideoMode < numModes) ? savedVideoMode : currentVideoMode;

    VideoMode targetModeInfo{};
    bool      hasTargetInfo = (targetVideoMode < numModes) && (RwEngineGetVideoModeInfo(&targetModeInfo, targetVideoMode) != nullptr);

    SendMessageA(modeComboBox, CB_RESETCONTENT, 0, 0);

    int selectedItemIndex = 0;
    for (size_t index = 0; index < resolutions.size(); ++index)
    {
        const auto& item = resolutions[index];
        char        buffer[128];
        if (item.isWidescreen)
            sprintf_s(buffer, "%d x %d x %d Widescreen", item.width, item.height, item.depth);
        else
            sprintf_s(buffer, "%d x %d x %d", item.width, item.height, item.depth);

        int itemIndex = static_cast<int>(SendMessageA(modeComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buffer)));
        if (itemIndex != CB_ERR)
        {
            SendMessageA(modeComboBox, CB_SETITEMDATA, itemIndex, static_cast<LPARAM>(item.vidModeIndex));

            if (hasTargetInfo && item.width == targetModeInfo.width && item.height == targetModeInfo.height && item.depth == targetModeInfo.depth &&
                item.isWidescreen == ((targetModeInfo.flags & rwVIDEOMODE_XBOX_WIDESCREEN) != 0))
            {
                selectedItemIndex = itemIndex;
            }
        }
    }

    SendMessageA(modeComboBox, CB_SETCURSEL, selectedItemIndex, 0);

    // Sync GTA's global selected video mode with the current selection
    LRESULT currentSelection = SendMessageA(modeComboBox, CB_GETCURSEL, 0, 0);
    if (currentSelection != CB_ERR)
    {
        LRESULT selectedMode = SendMessageA(modeComboBox, CB_GETITEMDATA, currentSelection, 0);
        if (selectedMode != CB_ERR)
        {
            *reinterpret_cast<unsigned int*>(VAR_CurVideoMode) = static_cast<unsigned int>(selectedMode);
        }
    }
}

//
// Wrapper around DialogBoxParamA that applies DPI awareness context and modern Common Controls v6 styling.
//
static INT_PTR WINAPI DialogBoxParamA_New(HINSTANCE instanceHandle, LPCSTR templateName, HWND parentWindow, DLGPROC dialogFunc, LPARAM initParameter)
{
    using SetThreadDpiAwarenessContextFn = int32_t(WINAPI*)(int32_t);
    SetThreadDpiAwarenessContextFn setThreadDpiAwarenessContext = nullptr;
    int32_t                        previousDpiContext = 0;

    // Dynamically query SetThreadDpiAwarenessContext to ensure older Windows versions continue running cleanly
    HMODULE user32Module = LoadLibraryW(L"user32.dll");
    if (user32Module)
    {
        setThreadDpiAwarenessContext = reinterpret_cast<SetThreadDpiAwarenessContextFn>(GetProcAddress(user32Module, "SetThreadDpiAwarenessContext"));
        if (setThreadDpiAwarenessContext)
        {
            // DPI_AWARENESS_CONTEXT_UNAWARE (-1) allows DWM to scale dialog bitmap proportions cleanly on High-DPI screens
            previousDpiContext = setThreadDpiAwarenessContext(-1);
        }
    }

    // Activate Common Controls v6 manifest embedded in multiplayer_sa.dll for modern visual styles
    ACTCTXW activationContext{};
    activationContext.cbSize = sizeof(activationContext);
    activationContext.hModule = reinterpret_cast<HMODULE>(&__ImageBase);
    activationContext.lpResourceName = MAKEINTRESOURCEW(2);
    activationContext.dwFlags = ACTCTX_FLAG_HMODULE_VALID | ACTCTX_FLAG_RESOURCE_NAME_VALID;

    ULONG_PTR cookie = 0;
    bool      contextActivated = false;
    HANDLE    activationContextHandle = CreateActCtxW(&activationContext);
    if (activationContextHandle != INVALID_HANDLE_VALUE)
    {
        contextActivated = (ActivateActCtx(activationContextHandle, &cookie) != FALSE);
    }

    const INT_PTR dialogResult = DialogBoxParamA(instanceHandle, templateName, parentWindow, CustomDlgProc, initParameter);

    if (contextActivated)
    {
        DeactivateActCtx(0, cookie);
    }
    if (activationContextHandle != INVALID_HANDLE_VALUE)
    {
        ReleaseActCtx(activationContextHandle);
    }

    if (setThreadDpiAwarenessContext)
    {
        setThreadDpiAwarenessContext(previousDpiContext);
    }
    if (user32Module)
    {
        FreeLibrary(user32Module);
    }

    return dialogResult;
}
static auto* const dialogBoxParamANewPtr = &DialogBoxParamA_New;

//
// NOP replacement for GTA's internal SetFocus call during dialog initialization to allow natural Windows focus management.
//
static HWND WINAPI SetFocus_NOP(HWND)
{
    return nullptr;
}
static auto* const setFocusNopPtr = &SetFocus_NOP;

INT_PTR CALLBACK CustomDlgProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto* originalDialogFunc = (DLGPROC)FUNC_DialogFunc;

    if (msg == WM_INITDIALOG)
    {
        originalDialogFunc(window, msg, wParam, lParam);

        // Set Icon
        HMODULE gameModule = GetModuleHandle(nullptr);
        SendMessage(window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(LoadIcon(gameModule, MAKEINTRESOURCE(100))));

        // Make the dialog visible in the task bar
        // https://stackoverflow.com/a/1462811
        SetWindowLongPtr(window, GWL_EXSTYLE, WS_EX_APPWINDOW);
        ShowWindow(window, SW_HIDE);
        ShowWindow(window, SW_SHOW);

        // Apply modern system UI font (e.g. Segoe UI on Win10/11) to all dialog controls
        static HFONT dialogFont = nullptr;
        if (!dialogFont)
        {
            NONCLIENTMETRICSW metrics{};
            metrics.cbSize = sizeof(metrics);
            if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0))
            {
                dialogFont = CreateFontIndirectW(&metrics.lfMessageFont);
            }
            if (!dialogFont)
            {
                dialogFont = static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
            }
        }

        if (dialogFont)
        {
            SendMessageW(window, WM_SETFONT, reinterpret_cast<WPARAM>(dialogFont), TRUE);
            EnumChildWindows(
                window,
                [](HWND childWindow, LPARAM fontParam) -> BOOL
                {
                    SendMessageW(childWindow, WM_SETFONT, static_cast<WPARAM>(fontParam), TRUE);
                    return TRUE;
                },
                reinterpret_cast<LPARAM>(dialogFont));
        }

        // Sort resolutions and deduplicate
        PopulateAndSortResolutionComboBox(window);

        // Return TRUE to rely on Windows to assign focus naturally now that SetFocus hook is in place
        return TRUE;
    }
    else if (msg == WM_COMMAND)
    {
        WORD controlId = LOWORD(wParam);
        WORD notificationCode = HIWORD(wParam);

        // Handle Exit / Cancel button or Escape key cleanly
        if (controlId == IDCANCEL)
        {
            EndDialog(window, 0);
            return TRUE;
        }

        INT_PTR result = originalDialogFunc(window, msg, wParam, lParam);

        // When the adapter selection is changed (IDC_DEVICE = 1000, CBN_SELCHANGE = 1), re-populate resolutions for the new adapter
        if (controlId == IDC_DEVICE && notificationCode == CBN_SELCHANGE)
        {
            PopulateAndSortResolutionComboBox(window);
        }

        return result;
    }

    return originalDialogFunc(window, msg, wParam, lParam);
}

void CMultiplayerSA::InitHooks_DeviceSelection()
{
    // Hook DialogBoxParamA call at 0x746241 to apply modern visual styles and DPI awareness
    MemPut<void*>(0x746241 + 2, (void*)&dialogBoxParamANewPtr);

    // Disable hardcoded SetFocus at 0x745DB3 to enable proper keyboard navigation (Tab/Esc/Enter)
    MemPut<void*>(0x745DB3 + 2, (void*)&setFocusNopPtr);

    // Hook RwEngineGetSubSystemInfo to populate friendly monitor names
    HookInstall(FUNC_RwEngineGetSubSystemInfo, (DWORD)RwEngineGetSubSystemInfo_Hooked, 6);
}
