/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CVideoModeManager.cpp
 *  PURPOSE:     Video mode related operations
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CVideoModeManager.h"
#include <core/D3DProxyDeviceGuids.h>
#include <game/CGame.h>
#include <game/CSettings.h>

///////////////////////////////////////////////////////////////
//
// CVideoModeManager class
//
///////////////////////////////////////////////////////////////
class CVideoModeManager : public CVideoModeManagerInterface
{
public:
    ZERO_ON_NEW
    CVideoModeManager();
    ~CVideoModeManager();

    // CVideoModeManagerInterface methods
    virtual void PreCreateDevice(D3DPRESENT_PARAMETERS* pp) override;
    virtual void PostCreateDevice(IDirect3DDevice9* pD3DDevice, D3DPRESENT_PARAMETERS* pp) override;
    virtual void PreReset(D3DPRESENT_PARAMETERS* pp) override;
    virtual void PostReset(D3DPRESENT_PARAMETERS* pp) override;
    virtual void GetNextVideoMode(int& iOutNextVideoMode, bool& bOutNextWindowed, bool& bOutNextFullScreenMinimize, int& iNextFullscreenStyle) override;
    virtual bool SetVideoMode(int nextVideoMode, bool bNextWindowed, bool bNextFullScreenMinimize, int iNextFullscreenStyle) override;
    virtual bool IsWindowed() override;
    virtual bool IsMultiMonitor() override;
    virtual bool IsMinimizeEnabled() override;
    virtual void OnGainFocus() override;
    virtual void OnLoseFocus() override;
    virtual void OnPaint() override;
    virtual bool GetRequiredDisplayResolution(int& iOutWidth, int& iOutHeight, int& iOutColorBits, int& iOutAdapterIndex) override;
    virtual int  GetFullScreenStyle() override { return m_iCurrentFullscreenStyle; }
    virtual bool IsDisplayModeWindowed() override;
    virtual bool IsDisplayModeFullScreenWindow() override;

    bool    IsDisplayModeFullScreen();
    bool    GetCurrentAdapterRect(LPRECT pOutRect);
    SString GetCurrentAdapterDeviceName();

private:
    void    LoadCVars();
    void    SaveCVars();
    bool    GameResMatchesCurrentAdapter();
    SString MakeResolutionString(UINT uiWidth, UINT uiHeight, UINT uiDepth, UINT uiAdapter);

    void UpdateMonitor();

    ULONG          m_ulForceBackBufferWidth;
    ULONG          m_ulForceBackBufferHeight;
    ULONG          m_ulForceBackBufferColorDepth;
    HWND           m_hDeviceWindow;
    CGameSettings* m_pGameSettings;
    ULONG          m_ulMonitorCount;

    int      m_iCurrentVideoMode;  // VideoMode this run
    int      m_iCurrentAdapter;
    bool     m_bCurrentWindowed;
    bool     m_bCurrentFullScreenMinimize;
    int      m_iCurrentFullscreenStyle;
    int      m_iNextVideoMode;  // VideoMode next run
    int      m_iNextAdapter;
    bool     m_bNextWindowed;
    int      m_iNextFullscreenStyle;
    HMONITOR m_hCurrentMonitor;

    bool m_bPendingGainFocus;
    bool m_bOriginalDesktopResMatches;
};

///////////////////////////////////////////////////////////////
//
// CVideoModeManager instantiation
//
///////////////////////////////////////////////////////////////
CVideoModeManagerInterface* NewVideoModeManager()
{
    return new CVideoModeManager();
}

CVideoModeManagerInterface* g_pVideoModeManager = nullptr;

CVideoModeManagerInterface* GetVideoModeManager()
{
    if (!g_pVideoModeManager)
        g_pVideoModeManager = NewVideoModeManager();
    return g_pVideoModeManager;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager implementation
//
///////////////////////////////////////////////////////////////
CVideoModeManager::CVideoModeManager()
    : m_ulForceBackBufferWidth(0),
      m_ulForceBackBufferHeight(0),
      m_ulForceBackBufferColorDepth(32),
      m_hDeviceWindow(nullptr),
      m_pGameSettings(nullptr),
      m_ulMonitorCount(0),
      m_iCurrentVideoMode(1),
      m_iCurrentAdapter(0),
      m_bCurrentWindowed(false),
      m_bCurrentFullScreenMinimize(false),
      m_iCurrentFullscreenStyle(FULLSCREEN_STANDARD),
      m_iNextVideoMode(1),
      m_iNextAdapter(0),
      m_bNextWindowed(false),
      m_iNextFullscreenStyle(FULLSCREEN_STANDARD),
      m_hCurrentMonitor(nullptr),
      m_bPendingGainFocus(false),
      m_bOriginalDesktopResMatches(false)
{
    m_pGameSettings = CCore::GetSingleton().GetGame()->GetSettings();
}

CVideoModeManager::~CVideoModeManager()
{
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PreCreateDevice
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PreCreateDevice(D3DPRESENT_PARAMETERS* pp)
{
    if (!pp)
        return;

    m_hDeviceWindow = pp->hDeviceWindow;

    // Load settings
    LoadCVars();

    // Prime save values
    m_iNextVideoMode = m_iCurrentVideoMode;
    m_iNextAdapter = m_iCurrentAdapter;
    m_bNextWindowed = m_bCurrentWindowed;
    m_iNextFullscreenStyle = m_iCurrentFullscreenStyle;

    // Set refresh rate to default (automatic)
    pp->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

    if (IsDisplayModeWindowed())
    {
        RECT rc;
        if (GetCurrentAdapterRect(&rc))
        {
            int iPosX = (rc.left + rc.right) / 2 - static_cast<int>(pp->BackBufferWidth) / 2;
            int iPosY = (rc.top + rc.bottom) / 2 - static_cast<int>(pp->BackBufferHeight) / 2;

            if (m_hDeviceWindow)
            {
                SetWindowLong(m_hDeviceWindow, GWL_STYLE, WS_POPUP);
                MoveWindow(m_hDeviceWindow, iPosX, iPosY, static_cast<int>(pp->BackBufferWidth), static_cast<int>(pp->BackBufferHeight), TRUE);
            }
        }
        pp->Windowed = TRUE;
    }
    else if (IsDisplayModeFullScreenWindow())
    {
        RECT rc;
        if (GetCurrentAdapterRect(&rc) && m_hDeviceWindow)
        {
            SetWindowLong(m_hDeviceWindow, GWL_STYLE, WS_POPUP);
            MoveWindow(m_hDeviceWindow, rc.left, rc.top, static_cast<int>(pp->BackBufferWidth), static_cast<int>(pp->BackBufferHeight), TRUE);
        }
        pp->Windowed = TRUE;
    }

    if (pp->SwapEffect == D3DSWAPEFFECT_FLIP && IsDisplayModeWindowed())
    {
        pp->SwapEffect = D3DSWAPEFFECT_DISCARD;
        WriteDebugEvent("Changed SwapEffect From D3DSWAPEFFECT_FLIP to D3DSWAPEFFECT_DISCARD");
    }

    m_ulForceBackBufferWidth = pp->BackBufferWidth;
    m_ulForceBackBufferHeight = pp->BackBufferHeight;
    m_ulForceBackBufferColorDepth = (pp->BackBufferFormat == D3DFMT_R5G6B5) ? 16 : 32;

    m_bOriginalDesktopResMatches = GameResMatchesCurrentAdapter();
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PostCreateDevice
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PostCreateDevice(IDirect3DDevice9* pD3DDevice, D3DPRESENT_PARAMETERS* pp)
{
    if (!pD3DDevice || !pp)
        return;

#ifdef MTA_DEBUG
    {
        // `PostCreateDevice` intentionally operates on the real device (it may call Reset immediately).
        // Log if we ever see the proxy here, as that can introduce recursion/state tracking side-effects.
        IUnknown*      pProxyMarker = nullptr;
        const HRESULT  hr = pD3DDevice->QueryInterface(CProxyDirect3DDevice9_GUID, reinterpret_cast<void**>(&pProxyMarker));
        if (SUCCEEDED(hr) && pProxyMarker)
        {
            pProxyMarker->Release();
        }
    }
#endif

    if (IsDisplayModeWindowed() || IsDisplayModeFullScreenWindow())
        pD3DDevice->Reset(pp);
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PreReset
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PreReset(D3DPRESENT_PARAMETERS* pp)
{
    if (!pp)
        return;

    if (IsDisplayModeWindowed() || IsDisplayModeFullScreenWindow())
    {
        pp->Windowed = true;
    }

    pp->BackBufferWidth = m_ulForceBackBufferWidth;
    pp->BackBufferHeight = m_ulForceBackBufferHeight;
    pp->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PostReset
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PostReset(D3DPRESENT_PARAMETERS* pp)
{
    if (!pp || !m_hDeviceWindow)
        return;

    if (pp->Windowed)
    {
        // Add frame
        LONG Style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX;
        if (IsDisplayModeWindowed())
            Style |= WS_BORDER | WS_DLGFRAME;

        SetWindowLong(m_hDeviceWindow, GWL_STYLE, Style);

        LONG ExStyle = 0;  // WS_EX_WINDOWEDGE;
        SetWindowLong(m_hDeviceWindow, GWL_EXSTYLE, ExStyle);

        // Ensure client area of window is correct size
        RECT ClientRect = {0, 0, static_cast<LONG>(pp->BackBufferWidth), static_cast<LONG>(pp->BackBufferHeight)};
        AdjustWindowRect(&ClientRect, GetWindowLong(m_hDeviceWindow, GWL_STYLE), FALSE);

        int SizeX = ClientRect.right - ClientRect.left;
        int SizeY = ClientRect.bottom - ClientRect.top;

        SetWindowPos(m_hDeviceWindow, HWND_NOTOPMOST, 0, 0, SizeX, SizeY, SWP_NOMOVE | SWP_FRAMECHANGED);

        if (m_bPendingGainFocus)
            OnGainFocus();
    }
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::OnGainFocus
//
// Change desktop resolution if using a full screen window
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::OnGainFocus()
{
    if (m_ulForceBackBufferWidth == 0)
    {
        m_bPendingGainFocus = true;
        return;
    }
    m_bPendingGainFocus = false;

    // Update monitor info
    UpdateMonitor();

    if (IsDisplayModeFullScreenWindow())
    {
        // Change only if needed
        if (!GameResMatchesCurrentAdapter())
        {
            DEVMODE dmScreenSettings;
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
            dmScreenSettings.dmSize = sizeof(dmScreenSettings);

            dmScreenSettings.dmPelsWidth = m_ulForceBackBufferWidth;
            dmScreenSettings.dmPelsHeight = m_ulForceBackBufferHeight;
            dmScreenSettings.dmBitsPerPel = m_ulForceBackBufferColorDepth;
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
            dmScreenSettings.dmDisplayFrequency = D3DPRESENT_RATE_DEFAULT;

            if (ChangeDisplaySettingsEx(GetCurrentAdapterDeviceName(), &dmScreenSettings, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
                return;
        }

        RECT rc;
        GetCurrentAdapterRect(&rc);
        MoveWindow(m_hDeviceWindow, rc.left, rc.top, m_ulForceBackBufferWidth, m_ulForceBackBufferHeight, TRUE);
    }
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::OnLoseFocus
//
// Revert desktop resolution if using a full screen window
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::OnLoseFocus()
{
    m_bPendingGainFocus = false;

    if (m_ulForceBackBufferWidth == 0)
        return;

    // Update monitor info
    UpdateMonitor();

    if (IsDisplayModeFullScreenWindow())
    {
        if (!IsMultiMonitor() || IsMinimizeEnabled())
        {
            HWND hWnd = CCore::GetSingleton().GetHookedWindow();
            if (hWnd)
                ShowWindow(hWnd, SW_MINIMIZE);

            if (!m_bOriginalDesktopResMatches && (m_iCurrentFullscreenStyle == FULLSCREEN_BORDERLESS))
            {
                DEVMODE dmScreenSettings;
                memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
                dmScreenSettings.dmSize = sizeof(dmScreenSettings);

                SString deviceName = GetCurrentAdapterDeviceName();
                if (!EnumDisplaySettingsA(deviceName.c_str(), ENUM_REGISTRY_SETTINGS, &dmScreenSettings))
                {
                    AddReportLog(7340, SString("EnumDisplaySettings failed for %s", deviceName.c_str()));
                    return;
                }

                int iChangeResult = ChangeDisplaySettingsExA(deviceName.c_str(), &dmScreenSettings, nullptr, CDS_RESET, nullptr);
                if (iChangeResult != DISP_CHANGE_SUCCESSFUL)
                {
                    AddReportLog(7341, SString("ChangeDisplaySettingsEx failed for %s (%d)", deviceName.c_str(), iChangeResult));
                    return;
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::OnPaint
//
// Ensure window is in the correct position for fullscreen windowed modes
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::OnPaint()
{
    if (IsDisplayModeFullScreenWindow() && m_hDeviceWindow)
    {
        RECT rc;
        if (GetCurrentAdapterRect(&rc))
        {
            MoveWindow(m_hDeviceWindow, rc.left, rc.top, static_cast<int>(m_ulForceBackBufferWidth), static_cast<int>(m_ulForceBackBufferHeight), FALSE);
        }
    }
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::GetNextVideoMode
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::GetNextVideoMode(int& iOutNextVideoMode, bool& bOutNextWindowed, bool& bOutNextFullScreenMinimize, int& iOutNextFullscreenStyle)
{
    iOutNextVideoMode = m_iNextVideoMode;
    bOutNextWindowed = m_bNextWindowed;
    bOutNextFullScreenMinimize = m_bCurrentFullScreenMinimize;
    iOutNextFullscreenStyle = m_iNextFullscreenStyle;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::SetVideoMode
//
// Returns true if restart is required
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::SetVideoMode(int iNextVideoMode, bool bNextWindowed, bool bNextFullScreenMinimize, int iNextFullscreenStyle)
{
    bool bRequiresRestart = false;

    // Resolution
    if (iNextVideoMode > 0 && m_pGameSettings && iNextVideoMode < static_cast<int>(m_pGameSettings->GetNumVideoModes()))
    {
        if (m_iNextVideoMode != iNextVideoMode)
        {
            m_iNextVideoMode = iNextVideoMode;
            if (m_iCurrentVideoMode != iNextVideoMode)
            {
                bRequiresRestart = true;
            }
        }
    }

    // Windowed
    if (m_bNextWindowed != bNextWindowed)
    {
        m_bNextWindowed = bNextWindowed;
        if (m_bCurrentWindowed != bNextWindowed)
        {
            bRequiresRestart = true;
        }
    }

    // Full Screen Minimize
    m_bCurrentFullScreenMinimize = bNextFullScreenMinimize;

    // Fullscreen style
    if (m_iNextFullscreenStyle != iNextFullscreenStyle)
    {
        m_iNextFullscreenStyle = iNextFullscreenStyle;
        if (m_iCurrentFullscreenStyle != m_iNextFullscreenStyle)
        {
            if (m_iCurrentFullscreenStyle == FULLSCREEN_STANDARD || m_iNextFullscreenStyle == FULLSCREEN_STANDARD)
            {
                bRequiresRestart = true;
            }
            else
            {
                m_iCurrentFullscreenStyle = m_iNextFullscreenStyle;
            }
        }
    }

    SaveCVars();

    return bRequiresRestart;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::LoadCVars
//
// Loads to current
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::LoadCVars()
{
    // Apply override
    if (GetApplicationSettingInt("nvhacks", "optimus-force-windowed"))
    {
        SetApplicationSettingInt("nvhacks", "optimus-force-windowed", 0);
        CVARS_SET("display_windowed", true);
    }

    m_iCurrentAdapter = m_pGameSettings->GetCurrentAdapter();
    m_iCurrentVideoMode = m_pGameSettings->GetCurrentVideoMode();
    CVARS_GET("display_windowed", m_bCurrentWindowed);
    CVARS_GET("display_fullscreen_style", m_iCurrentFullscreenStyle);
    CVARS_GET("multimon_fullscreen_minimize", m_bCurrentFullScreenMinimize);

    // Save the video mode resolution that is being used
    VideoMode info;
    if (m_pGameSettings->GetVideoModeInfo(&info, m_iCurrentVideoMode))
    {
        CVARS_SET("display_resolution", MakeResolutionString(info.width, info.height, info.depth, m_iCurrentAdapter));
    }

    m_hCurrentMonitor = CProxyDirect3D9::StaticGetAdapterMonitor(m_iCurrentAdapter);
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::SaveCVars
//
// Saves from next
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::SaveCVars()
{
    if (m_pGameSettings)
    {
        m_pGameSettings->SetCurrentVideoMode(m_iNextVideoMode, true);

        VideoMode info;
        if (m_pGameSettings->GetVideoModeInfo(&info, m_iNextVideoMode))
        {
            CVARS_SET("display_resolution", MakeResolutionString(info.width, info.height, info.depth, static_cast<UINT>(m_iNextAdapter)));
        }
    }

    CVARS_SET("display_windowed", m_bNextWindowed);
    CVARS_SET("display_fullscreen_style", m_iNextFullscreenStyle);
    CVARS_SET("multimon_fullscreen_minimize", m_bCurrentFullScreenMinimize);
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsWindowed
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsWindowed()
{
    return (IsDisplayModeWindowed() || IsDisplayModeFullScreenWindow());
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsMultiMonitor
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsMultiMonitor()
{
    if (m_ulMonitorCount == 0)
    {
        // Count attached monitors
        for (int i = 0; true; i++)
        {
            DISPLAY_DEVICE device;
            device.cb = sizeof(device);

            // Get next DISPLAY_DEVICE from the system
            if (!EnumDisplayDevicesA(nullptr, static_cast<DWORD>(i), &device, 0))
                break;

            // Calc flags
            bool bAttachedToDesktop = (device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) != 0;
            bool bMirroringDriver = (device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0;

            // Ignore devices that are not required
            if (!bAttachedToDesktop || bMirroringDriver)
                continue;

            m_ulMonitorCount++;
        }
    }

    return m_ulMonitorCount > 1;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsMinimizeEnabled
//
// Multi-monitor, full screen, minimize
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsMinimizeEnabled()
{
    return m_bCurrentFullScreenMinimize;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsDisplayModeWindowed
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsDisplayModeWindowed()
{
    return m_bCurrentWindowed;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsDisplayModeFullScreen
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsDisplayModeFullScreen()
{
    return !m_bCurrentWindowed && (m_iCurrentFullscreenStyle == FULLSCREEN_STANDARD);
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsDisplayModeFullScreenWindow
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsDisplayModeFullScreenWindow()
{
    return !m_bCurrentWindowed && (m_iCurrentFullscreenStyle != FULLSCREEN_STANDARD);
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::GameResMatchesCurrentAdapter
//
// Returns true if desktop matches the game requirement
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::GameResMatchesCurrentAdapter()
{
    RECT rc;
    if (!GetCurrentAdapterRect(&rc))
        return false;

    int iAdapterResX = rc.right - rc.left;
    int iAdapterResY = rc.bottom - rc.top;

    // Here we hope that the color depth is the same across all monitors
    HDC hdcPrimaryMonitor = GetDC(nullptr);
    int iDesktopColorDepth = 32;  // Default fallback

    if (hdcPrimaryMonitor)
    {
        iDesktopColorDepth = GetDeviceCaps(hdcPrimaryMonitor, BITSPIXEL);
        ReleaseDC(nullptr, hdcPrimaryMonitor);
    }

    if (iAdapterResX == static_cast<int>(m_ulForceBackBufferWidth) && iAdapterResY == static_cast<int>(m_ulForceBackBufferHeight) &&
        iDesktopColorDepth == static_cast<int>(m_ulForceBackBufferColorDepth))
    {
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::MakeResolutionString
//
// Make a frendly string for saving to the config file
//
///////////////////////////////////////////////////////////////
SString CVideoModeManager::MakeResolutionString(UINT uiWidth, UINT uiHeight, UINT uiDepth, UINT uiAdapter)
{
    SString strRes("%dx%dx%d", uiWidth, uiHeight, uiDepth);
    if (uiAdapter > 0)
        strRes += SString("x%d", uiAdapter);
    return strRes;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::UpdateMonitor
//
// Updates the stored monitor ref to match the monitor
// with the largest intersection (in borderless window mode)
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::UpdateMonitor()
{
    if (IsDisplayModeFullScreenWindow() && m_hDeviceWindow)
    {
        m_hCurrentMonitor = MonitorFromWindow(m_hDeviceWindow, MONITOR_DEFAULTTONEAREST);
    }
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::GetRequiredDisplayResolution
//
// Get last set/used resolution from MTA configuration
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::GetRequiredDisplayResolution(int& iOutWidth, int& iOutHeight, int& iOutColorBits, int& iOutAdapterIndex)
{
    iOutWidth = 0;
    iOutHeight = 0;
    iOutAdapterIndex = 0;
    iOutColorBits = 32;

    SString strResString;
    CVARS_GET("display_resolution", strResString);

    // Parse string from config
    std::vector<SString> parts;
    strResString.ToLower().Replace(" ", "").Split("x", parts);

    if (parts.size() > 1)
    {
        const char* widthStr = parts[0].c_str();
        const char* heightStr = parts[1].c_str();

        if (widthStr && heightStr)
        {
            long width = strtol(widthStr, nullptr, 10);
            long height = strtol(heightStr, nullptr, 10);

            if (width > 0 && width <= 65535 && height > 0 && height <= 65535)
            {
                iOutWidth = static_cast<int>(width);
                iOutHeight = static_cast<int>(height);
            }
        }
    }

    if (parts.size() > 2)
    {
        const char* colorStr = parts[2].c_str();
        if (colorStr)
        {
            long colorBits = strtol(colorStr, nullptr, 10);
            if (colorBits == 16 || colorBits == 32)
                iOutColorBits = static_cast<int>(colorBits);
        }
    }

    if (parts.size() > 3)
    {
        const char* adapterStr = parts[3].c_str();
        if (adapterStr)
        {
            long adapter = strtol(adapterStr, nullptr, 10);
            if (adapter >= 0)
                iOutAdapterIndex = static_cast<int>(adapter);
        }
    }

    return (iOutWidth > 0) && (iOutHeight > 0) && (iOutColorBits == 16 || iOutColorBits == 32);
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::GetCurrentAdapterRect
//
// Returns true on success
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::GetCurrentAdapterRect(LPRECT pOutRect)
{
    if (!pOutRect)
        return false;

    // Initialize to safe defaults
    pOutRect->left = 0;
    pOutRect->top = 0;
    pOutRect->right = 1024;
    pOutRect->bottom = 768;

    if (!m_hCurrentMonitor)
        return false;

    MONITORINFOEX monitorInfo{};
    monitorInfo.cbSize = sizeof(MONITORINFOEX);

    BOOL bResult = GetMonitorInfoA(m_hCurrentMonitor, &monitorInfo);
    if (bResult)
    {
        *pOutRect = monitorInfo.rcMonitor;
        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////
//
// CVideoModeManager::GetCurrentAdapterDeviceName
//
//
//
///////////////////////////////////////////////////////////////
SString CVideoModeManager::GetCurrentAdapterDeviceName()
{
    if (!m_hCurrentMonitor)
        return "";

    MONITORINFOEX monitorInfo{};
    monitorInfo.cbSize = sizeof(MONITORINFOEX);

    if (GetMonitorInfoA(m_hCurrentMonitor, &monitorInfo))
    {
        // Ensure null termination for x86 safety
        monitorInfo.szDevice[sizeof(monitorInfo.szDevice) - 1] = '\0';
        return std::string(monitorInfo.szDevice);
    }

    return "";
}
