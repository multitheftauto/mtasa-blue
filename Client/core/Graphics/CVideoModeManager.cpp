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
    virtual void PreCreateDevice(D3DPRESENT_PARAMETERS* pp);
    virtual void PostCreateDevice(IDirect3DDevice9* pD3DDevice, D3DPRESENT_PARAMETERS* pp);
    virtual void PreReset(D3DPRESENT_PARAMETERS* pp);
    virtual void PostReset(D3DPRESENT_PARAMETERS* pp);
    virtual void GetNextVideoMode(int& iOutNextVideoMode, bool& bOutNextWindowed, bool& bOutNextFullScreenMinimize, int& iNextFullscreenStyle);
    virtual bool SetVideoMode(int nextVideoMode, bool bNextWindowed, bool bNextFullScreenMinimize, int iNextFullscreenStyle);
    virtual bool IsWindowed();
    virtual bool IsMultiMonitor();
    virtual bool IsMinimizeEnabled();
    virtual void OnGainFocus();
    virtual void OnLoseFocus();
    virtual void OnPaint();
    virtual bool GetRequiredDisplayResolution(int& iOutWidth, int& iOutHeight, int& iOutColorBits, int& iOutAdapterIndex);
    virtual int  GetFullScreenStyle() { return m_iCurrentFullscreenStyle; }
    virtual bool IsDisplayModeWindowed();

    bool    IsDisplayModeFullScreen();
    bool    IsDisplayModeFullScreenWindow();
    bool    GetCurrentAdapterRect(LPRECT pOutRect);
    SString GetCurrentAdapterDeviceName();

private:
    void    LoadCVars();
    void    SaveCVars();
    bool    GameResMatchesCurrentAdapter();
    SString MakeResolutionString(uint uiWidth, uint uiHeight, uint uiDepth, uint uiAdapter);

    void UpdateMonitor();

    unsigned long  m_ulForceBackBufferWidth;
    unsigned long  m_ulForceBackBufferHeight;
    unsigned long  m_ulForceBackBufferColorDepth;
    HWND           m_hDeviceWindow;
    CGameSettings* m_pGameSettings;
    unsigned long  m_ulMonitorCount;

    int      m_iCurrentVideoMode;            // VideoMode this run
    int      m_iCurrentAdapter;
    bool     m_bCurrentWindowed;
    bool     m_bCurrentFullScreenMinimize;
    int      m_iCurrentFullscreenStyle;
    int      m_iNextVideoMode;            // VideoMode next run
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

CVideoModeManagerInterface* g_pVideoModeManager = NULL;

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
{
    m_pGameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    m_iCurrentVideoMode = 1;
    m_bCurrentWindowed = false;
    m_iNextVideoMode = 1;
    m_bNextWindowed = false;
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
        GetCurrentAdapterRect(&rc);
        int iPosX = (rc.left + rc.right) / 2 - (pp->BackBufferWidth / 2);
        int iPosY = (rc.top + rc.bottom) / 2 - (pp->BackBufferHeight / 2);
        SetWindowLong(m_hDeviceWindow, GWL_STYLE, WS_POPUP);
        MoveWindow(m_hDeviceWindow, iPosX, iPosY, pp->BackBufferWidth, pp->BackBufferHeight, TRUE);
        pp->Windowed = true;
    }
    else if (IsDisplayModeFullScreenWindow())
    {
        RECT rc;
        GetCurrentAdapterRect(&rc);
        SetWindowLong(m_hDeviceWindow, GWL_STYLE, WS_POPUP);
        MoveWindow(m_hDeviceWindow, rc.left, rc.top, pp->BackBufferWidth, pp->BackBufferHeight, TRUE);
        pp->Windowed = true;
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
    if (pp->Windowed)
    {
        // Add frame
        LONG Style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX;
        if (IsDisplayModeWindowed())
            Style |= WS_BORDER | WS_DLGFRAME;

        SetWindowLong(m_hDeviceWindow, GWL_STYLE, Style);

        LONG ExStyle = 0;            // WS_EX_WINDOWEDGE;
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
            ShowWindow(hWnd, SW_MINIMIZE);

            if (!m_bOriginalDesktopResMatches && (m_iCurrentFullscreenStyle == FULLSCREEN_BORDERLESS))
            {
                DEVMODE dmScreenSettings;
                memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
                dmScreenSettings.dmSize = sizeof(dmScreenSettings);

                if (!EnumDisplaySettings(GetCurrentAdapterDeviceName(), ENUM_REGISTRY_SETTINGS, &dmScreenSettings))
                {
                    AddReportLog(7340, SString("EnumDisplaySettings failed for %s", *GetCurrentAdapterDeviceName()));
                    return;
                }

                int iChangeResult = ChangeDisplaySettingsEx(GetCurrentAdapterDeviceName(), &dmScreenSettings, NULL, CDS_RESET, NULL);
                if (iChangeResult != DISP_CHANGE_SUCCESSFUL)
                {
                    AddReportLog(7341, SString("ChangeDisplaySettingsEx failed for %s (%d)", *GetCurrentAdapterDeviceName(), iChangeResult));
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
    if (IsDisplayModeFullScreenWindow())
    {
        RECT rc;
        GetCurrentAdapterRect(&rc);
        MoveWindow(m_hDeviceWindow, rc.left, rc.top, m_ulForceBackBufferWidth, m_ulForceBackBufferHeight, FALSE);
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
    if (iNextVideoMode > 0 && iNextVideoMode < (int)m_pGameSettings->GetNumVideoModes())
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
    m_pGameSettings->SetCurrentVideoMode(m_iNextVideoMode, true);
    CVARS_SET("display_windowed", m_bNextWindowed);
    CVARS_SET("display_fullscreen_style", m_iNextFullscreenStyle);
    CVARS_SET("multimon_fullscreen_minimize", m_bCurrentFullScreenMinimize);

    VideoMode info;
    if (m_pGameSettings->GetVideoModeInfo(&info, m_iNextVideoMode))
    {
        CVARS_SET("display_resolution", MakeResolutionString(info.width, info.height, info.depth, m_iNextAdapter));
    }
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
            if (!EnumDisplayDevicesA(NULL, i, &device, 0))
                break;

            // Calc flags
            bool bAttachedToDesktop = (device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) != 0;
            bool bMirroringDriver = (device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) != 0;
            bool bPrimaryDevice = (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0;

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
    GetCurrentAdapterRect(&rc);
    int iAdapterResX = rc.right - rc.left;
    int iAdapterResY = rc.bottom - rc.top;

    // Here we hope that the color depth is the same across all monitors
    HDC hdcPrimaryMonitor = GetDC(NULL);
    int iDesktopColorDepth = GetDeviceCaps(hdcPrimaryMonitor, BITSPIXEL);

    if (iAdapterResX == m_ulForceBackBufferWidth && iAdapterResY == m_ulForceBackBufferHeight && iDesktopColorDepth == m_ulForceBackBufferColorDepth)
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
SString CVideoModeManager::MakeResolutionString(uint uiWidth, uint uiHeight, uint uiDepth, uint uiAdapter)
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
    if (IsDisplayModeFullScreenWindow())
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
        iOutWidth = atoi(parts[0]);
        iOutHeight = atoi(parts[1]);
    }
    if (parts.size() > 2)
    {
        iOutColorBits = atoi(parts[2]);
    }
    if (parts.size() > 3)
    {
        iOutAdapterIndex = atoi(parts[3]);
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
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    BOOL bResult = GetMonitorInfo(m_hCurrentMonitor, &monitorInfo);
    *pOutRect = monitorInfo.rcMonitor;
    return bResult != 0;
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
    MONITORINFOEX monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFOEX);
    if (GetMonitorInfo(m_hCurrentMonitor, &monitorInfo))
        return monitorInfo.szDevice;
    return "";
}
