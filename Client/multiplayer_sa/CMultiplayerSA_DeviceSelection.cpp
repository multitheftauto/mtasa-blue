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

#define FUNC_rwDeviceSystemRequest     0x7F2AB0
#define FUNC_DialogFunc                0x745E50
#define FUNC_RwEngineGetSubSystemInfo  0x7F2C30
#define FUNC_RwEngineGetNumVideoModes  0x7F2CC0
#define FUNC_RwEngineGetVideoModeInfo  0x7F2CF0
#define CLASS_RwGlobals                0xC97B24
#define CLASS_IDirect3D9               0xC97C20
#define NUM_DialogFuncStackPushAddress 0x746239
#define VAR_SavedVideoMode             0xBA6820
#define VAR_CurVideoMode               0x8D6220
#define IDC_DEVICE                     1000
#define IDC_VIDMODE                    1001

// This is copied from SilentPatch:
// https://github.com/CookiePLMonster/SilentPatch/blob/dev/SilentPatch/FriendlyMonitorNames.cpp
std::unordered_map<std::string, std::string> GetFriendlyMonitorNamesForDevicePaths()
{
    std::unordered_map<std::string, std::string> monitorNames;

    HMODULE user32Lib = LoadLibrary(TEXT("user32"));
    if (!user32Lib)
        return monitorNames;

    using GetDisplayConfigBufferSizesFn = decltype(&GetDisplayConfigBufferSizes);
    using QueryDisplayConfigFn = decltype(&QueryDisplayConfig);
    using DisplayConfigGetDeviceInfoFn = decltype(&DisplayConfigGetDeviceInfo);

    GetDisplayConfigBufferSizesFn getDisplayConfigBufferSizes = nullptr;
    QueryDisplayConfigFn          queryDisplayConfig = nullptr;
    DisplayConfigGetDeviceInfoFn  displayConfigGetDeviceInfo = nullptr;

    const auto procGetDisplayConfigBufferSizes = GetProcAddress(user32Lib, "GetDisplayConfigBufferSizes");
    const auto procQueryDisplayConfig = GetProcAddress(user32Lib, "QueryDisplayConfig");
    const auto procDisplayConfigGetDeviceInfo = GetProcAddress(user32Lib, "DisplayConfigGetDeviceInfo");

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
        FreeLibrary(user32Lib);
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
        FreeLibrary(user32Lib);
        return monitorNames;
    }

    for (size_t i = 0; i < pathCount; i++)
    {
        DISPLAYCONFIG_TARGET_DEVICE_NAME targetName = {};
        targetName.header.adapterId = paths[i].targetInfo.adapterId;
        targetName.header.id = paths[i].targetInfo.id;
        targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        targetName.header.size = sizeof(targetName);
        const LONG targetNameResult = displayConfigGetDeviceInfo(&targetName.header);

        DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName = {};
        sourceName.header.adapterId = paths[i].sourceInfo.adapterId;
        sourceName.header.id = paths[i].sourceInfo.id;
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

    FreeLibrary(user32Lib);
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

    auto* pDxDevice = *(IDirect3D9**)CLASS_IDirect3D9;
    if (!pDxDevice)
        return subSystemInfo;

    D3DADAPTER_IDENTIFIER9 identifier;
    if (FAILED(pDxDevice->GetAdapterIdentifier(subSystemIndex, 0, &identifier)))
        return subSystemInfo;

    static const auto friendlyNames = GetFriendlyMonitorNamesForDevicePaths();

    // If we can't find the friendly name, either because it doesn't exist or we're on an ancient Windows, fall back to the device name
    auto it = friendlyNames.find(identifier.DeviceName);
    if (it != friendlyNames.end())
    {
        strncpy_s(subSystemInfo->name, it->second.c_str(), _TRUNCATE);
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
    HWND hwndVidMode = GetDlgItem(window, IDC_VIDMODE);
    if (!hwndVidMode)
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
              [](const DialogResolutionItem& a, const DialogResolutionItem& b)
              {
                  if (a.width != b.width)
                      return a.width > b.width;
                  if (a.height != b.height)
                      return a.height > b.height;
                  if (a.depth != b.depth)
                      return a.depth > b.depth;
                  return a.isWidescreen > b.isWidescreen;
              });

    // Determine target video mode to preserve current/saved selection
    unsigned int savedVidMode = *reinterpret_cast<unsigned int*>(VAR_SavedVideoMode);
    unsigned int curVidMode = *reinterpret_cast<unsigned int*>(VAR_CurVideoMode);
    unsigned int targetVidMode = (savedVidMode > 0 && savedVidMode < numModes) ? savedVidMode : curVidMode;

    VideoMode targetModeInfo{};
    bool      hasTargetInfo = (targetVidMode < numModes) && (RwEngineGetVideoModeInfo(&targetModeInfo, targetVidMode) != nullptr);

    SendMessageA(hwndVidMode, CB_RESETCONTENT, 0, 0);

    int selectedItemIdx = 0;
    for (size_t i = 0; i < resolutions.size(); ++i)
    {
        const auto& res = resolutions[i];
        char        buffer[128];
        if (res.isWidescreen)
            sprintf_s(buffer, "%d x %d x %d Widescreen", res.width, res.height, res.depth);
        else
            sprintf_s(buffer, "%d x %d x %d", res.width, res.height, res.depth);

        int itemIdx = static_cast<int>(SendMessageA(hwndVidMode, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(buffer)));
        if (itemIdx != CB_ERR)
        {
            SendMessageA(hwndVidMode, CB_SETITEMDATA, itemIdx, static_cast<LPARAM>(res.vidModeIndex));

            if (hasTargetInfo && res.width == targetModeInfo.width && res.height == targetModeInfo.height && res.depth == targetModeInfo.depth &&
                res.isWidescreen == ((targetModeInfo.flags & rwVIDEOMODE_XBOX_WIDESCREEN) != 0))
            {
                selectedItemIdx = itemIdx;
            }
        }
    }

    SendMessageA(hwndVidMode, CB_SETCURSEL, selectedItemIdx, 0);

    // Sync GTA's global selected video mode with the current selection
    LRESULT curSel = SendMessageA(hwndVidMode, CB_GETCURSEL, 0, 0);
    if (curSel != CB_ERR)
    {
        LRESULT selectedMode = SendMessageA(hwndVidMode, CB_GETITEMDATA, curSel, 0);
        if (selectedMode != CB_ERR)
        {
            *reinterpret_cast<unsigned int*>(VAR_CurVideoMode) = static_cast<unsigned int>(selectedMode);
        }
    }
}

INT_PTR CALLBACK CustomDlgProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto* orgDialogFunc = (DLGPROC)FUNC_DialogFunc;

    if (msg == WM_INITDIALOG)
    {
        orgDialogFunc(window, msg, wParam, lParam);

        // Set Icon
        HMODULE hGameModule = GetModuleHandle(nullptr);
        SendMessage(window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(LoadIcon(hGameModule, MAKEINTRESOURCE(100))));

        // Make the dialog visible in the task bar
        // https://stackoverflow.com/a/1462811
        SetWindowLongPtr(window, GWL_EXSTYLE, WS_EX_APPWINDOW);
        ShowWindow(window, SW_HIDE);
        ShowWindow(window, SW_SHOW);

        // Sort resolutions and deduplicate
        PopulateAndSortResolutionComboBox(window);
        return FALSE;
    }
    else if (msg == WM_COMMAND)
    {
        WORD controlId = LOWORD(wParam);
        WORD notificationCode = HIWORD(wParam);

        INT_PTR result = orgDialogFunc(window, msg, wParam, lParam);

        // When the adapter selection is changed (IDC_DEVICE = 1000, CBN_SELCHANGE = 1)
        if (controlId == IDC_DEVICE && notificationCode == CBN_SELCHANGE)
        {
            PopulateAndSortResolutionComboBox(window);
        }

        return result;
    }

    return orgDialogFunc(window, msg, wParam, lParam);
}

void CMultiplayerSA::InitHooks_DeviceSelection()
{
    // 0x746239 -> Exact address where the original DialogFunc address is being pushed as an argument to DialogBoxParamA(),
    // we're replacing it with out own proxy function
    MemPut<DLGPROC>(NUM_DialogFuncStackPushAddress, (DLGPROC)&CustomDlgProc);
    HookInstall(FUNC_RwEngineGetSubSystemInfo, (DWORD)RwEngineGetSubSystemInfo_Hooked, 6);
}
