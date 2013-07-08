/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CVideoModeManager.cpp
*  PURPOSE:     Video mode related operations
*  DEVELOPERS:  ccw <chris@codewave.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>


///////////////////////////////////////////////////////////////
//
// CVideoModeManager class
//
///////////////////////////////////////////////////////////////
class CVideoModeManager : public CVideoModeManagerInterface
{
public:
    ZERO_ON_NEW
                        CVideoModeManager           ( void );
                        ~CVideoModeManager          ( void );

    // CVideoModeManagerInterface methods
    virtual void        PreCreateDevice             ( D3DPRESENT_PARAMETERS* pp );
    virtual void        PostCreateDevice            ( IDirect3DDevice9* pD3DDevice, D3DPRESENT_PARAMETERS* pp );
    virtual void        PreReset                    ( D3DPRESENT_PARAMETERS* pp );
    virtual void        PostReset                   ( D3DPRESENT_PARAMETERS* pp );
    virtual void        GetNextVideoMode            ( int& iOutNextVideoMode, bool& bOutNextWindowed, bool& bOutNextFullScreenMinimize, int& iNextFullscreenStyle );
    virtual bool        SetVideoMode                ( int nextVideoMode, bool bNextWindowed, bool bNextFullScreenMinimize, int iNextFullscreenStyle );
    virtual bool        IsWindowed                  ( void );
    virtual bool        IsMultiMonitor              ( void );
    virtual bool        IsMinimizeEnabled           ( void );
    virtual void        OnGainFocus                 ( void );
    virtual void        OnLoseFocus                 ( void );

    bool                IsDisplayModeWindowed       ( void );
    bool                IsDisplayModeFullScreen     ( void );
    bool                IsDisplayModeFullScreenWindow   ( void );

private:
    void                LoadCVars                   ( void );
    void                SaveCVars                   ( void );
    bool                GameResMatchesPrimaryMonitor ( void );

    unsigned long       m_ulForceBackBufferWidth;
    unsigned long       m_ulForceBackBufferHeight;
    unsigned long       m_ulForceBackBufferColorDepth;
    unsigned long       m_ulFullScreenRefreshRate;
    HWND                m_hDeviceWindow;
    CGameSettings *     m_pGameSettings;
    unsigned long       m_ulMonitorCount;

    int                 m_iCurrentVideoMode;    // VideoMode this run
    bool                m_bCurrentWindowed;
    bool                m_bCurrentFullScreenMinimize;
    int                 m_iCurrentFullscreenStyle;
    int                 m_iNextVideoMode;       // VideoMode next run
    bool                m_bNextWindowed;
    int                 m_iNextFullscreenStyle;

    bool                m_bPendingGainFocus;
    bool                m_bOriginalDesktopResMatches;
};


///////////////////////////////////////////////////////////////
//
// CVideoModeManager instantiation
//
///////////////////////////////////////////////////////////////
CVideoModeManagerInterface* NewVideoModeManager ( void )
{
    return new CVideoModeManager ();
}

CVideoModeManagerInterface* g_pVideoModeManager = NULL;

CVideoModeManagerInterface* GetVideoModeManager ( void )
{
    if ( !g_pVideoModeManager )
        g_pVideoModeManager = NewVideoModeManager ();
    return g_pVideoModeManager;
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager implementation
//
///////////////////////////////////////////////////////////////
CVideoModeManager::CVideoModeManager ( void )
{
    m_pGameSettings = CCore::GetSingleton ( ).GetGame ( )->GetSettings();
    m_iCurrentVideoMode = 1;
    m_bCurrentWindowed = false;
    m_iNextVideoMode = 1;
    m_bNextWindowed = false;
}


CVideoModeManager::~CVideoModeManager ( void )
{
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PreCreateDevice
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PreCreateDevice ( D3DPRESENT_PARAMETERS* pp )
{
    m_hDeviceWindow = pp->hDeviceWindow;

    // Load settings
    LoadCVars ();

    // Prime save values
    m_iNextVideoMode = m_iCurrentVideoMode;
    m_bNextWindowed  = m_bCurrentWindowed;
    m_iNextFullscreenStyle = m_iCurrentFullscreenStyle;

    // Remember this for later
    m_ulFullScreenRefreshRate = pp->FullScreen_RefreshRateInHz;

    if ( IsDisplayModeWindowed() )
    {
        int x, y;
        x = GetSystemMetrics ( SM_CXSCREEN );
        y = GetSystemMetrics ( SM_CYSCREEN );
        SetWindowLong ( m_hDeviceWindow, GWL_STYLE, WS_POPUP );
        MoveWindow ( m_hDeviceWindow, 
                    (x/2)-(pp->BackBufferWidth/2), 
                    (y/2)-(pp->BackBufferHeight/2), 
                    pp->BackBufferWidth,
                    pp->BackBufferHeight,
                    TRUE );
        pp->Windowed = true;
        pp->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    }
    else
    if ( IsDisplayModeFullScreenWindow() )
    {
        SetWindowLong ( m_hDeviceWindow, GWL_STYLE, WS_POPUP );
        MoveWindow ( m_hDeviceWindow, 
                    0, 
                    0, 
                    pp->BackBufferWidth,
                    pp->BackBufferHeight,
                    TRUE );
        pp->Windowed = true;
        pp->FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    }

    m_ulForceBackBufferWidth  = pp->BackBufferWidth;
    m_ulForceBackBufferHeight = pp->BackBufferHeight;
    m_ulForceBackBufferColorDepth = ( pp->BackBufferFormat == D3DFMT_R5G6B5 ) ? 16 : 32;

    m_bOriginalDesktopResMatches = GameResMatchesPrimaryMonitor();
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PostCreateDevice
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PostCreateDevice ( IDirect3DDevice9* pD3DDevice, D3DPRESENT_PARAMETERS* pp )
{
    if ( IsDisplayModeWindowed() || IsDisplayModeFullScreenWindow() )
       pD3DDevice->Reset ( pp );
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PreReset
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PreReset ( D3DPRESENT_PARAMETERS* pp )
{
    if ( IsDisplayModeWindowed() || IsDisplayModeFullScreenWindow() )
    {
        pp->Windowed = true;
    }

    pp->BackBufferWidth = m_ulForceBackBufferWidth;
    pp->BackBufferHeight = m_ulForceBackBufferHeight;
    pp->FullScreen_RefreshRateInHz = pp->Windowed ? D3DPRESENT_RATE_DEFAULT : m_ulFullScreenRefreshRate;
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::PostReset
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::PostReset ( D3DPRESENT_PARAMETERS* pp )
{
    if ( pp->Windowed )
    {
        // Add frame
        LONG Style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_SYSMENU | WS_MINIMIZEBOX;
        if ( IsDisplayModeWindowed() )
            Style |= WS_BORDER | WS_DLGFRAME;

        SetWindowLong ( m_hDeviceWindow, GWL_STYLE, Style );

        LONG ExStyle = 0;//WS_EX_WINDOWEDGE;
        SetWindowLong ( m_hDeviceWindow, GWL_EXSTYLE, ExStyle );

        // Ensure client area of window is correct size
        RECT ClientRect = { 0, 0, pp->BackBufferWidth, pp->BackBufferHeight };
        AdjustWindowRect( &ClientRect, GetWindowLong(m_hDeviceWindow,GWL_STYLE), FALSE );

        int SizeX = ClientRect.right - ClientRect.left;
        int SizeY = ClientRect.bottom - ClientRect.top;

        SetWindowPos( m_hDeviceWindow, HWND_NOTOPMOST, 0, 0, SizeX, SizeY, SWP_NOMOVE | SWP_FRAMECHANGED );

        if ( m_bPendingGainFocus )
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
void CVideoModeManager::OnGainFocus ( void )
{
    if ( m_ulForceBackBufferWidth == 0 )
    {
        m_bPendingGainFocus = true;
        return;
    }
    m_bPendingGainFocus = false;

    if ( IsDisplayModeFullScreenWindow() )
    {
        // Change only if needed
        if ( !GameResMatchesPrimaryMonitor() )
        {
            DEVMODE dmScreenSettings;
            memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );
            dmScreenSettings.dmSize = sizeof( dmScreenSettings );

            dmScreenSettings.dmPelsWidth = m_ulForceBackBufferWidth;
            dmScreenSettings.dmPelsHeight = m_ulForceBackBufferHeight;
            dmScreenSettings.dmBitsPerPel = m_ulForceBackBufferColorDepth;
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
            dmScreenSettings.dmDisplayFrequency = m_ulFullScreenRefreshRate;

            if( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN ) != DISP_CHANGE_SUCCESSFUL )
                return;
        }
    }
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::OnLoseFocus
//
// Revert desktop resolution if using a full screen window
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::OnLoseFocus ( void )
{
    m_bPendingGainFocus = false;

    if ( m_ulForceBackBufferWidth == 0 )
        return;

    if ( IsDisplayModeFullScreenWindow() )
    {
        if ( !IsMultiMonitor() || IsMinimizeEnabled() )
        {
            HWND hWnd = CCore::GetSingleton().GetHookedWindow();
            ShowWindow( hWnd, SW_MINIMIZE );

            if ( !m_bOriginalDesktopResMatches && ( m_iCurrentFullscreenStyle == FULLSCREEN_BORDERLESS ) )
            {
                DEVMODE dmScreenSettings;
                memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );
                dmScreenSettings.dmSize = sizeof( dmScreenSettings );

                dmScreenSettings.dmFields = 0;

                if( ChangeDisplaySettings( &dmScreenSettings, CDS_RESET ) != DISP_CHANGE_SUCCESSFUL )
                    return;
            }
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
void CVideoModeManager::GetNextVideoMode ( int& iOutNextVideoMode, bool& bOutNextWindowed, bool& bOutNextFullScreenMinimize, int& iOutNextFullscreenStyle )
{
    iOutNextVideoMode   = m_iNextVideoMode;
    bOutNextWindowed    = m_bNextWindowed;
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
bool CVideoModeManager::SetVideoMode ( int iNextVideoMode, bool bNextWindowed, bool bNextFullScreenMinimize, int iNextFullscreenStyle )
{
    bool bRequiresRestart = false;

    // Resolution
    if ( iNextVideoMode > 0 && iNextVideoMode < (int)m_pGameSettings->GetNumVideoModes () )
    {
        if ( m_iNextVideoMode != iNextVideoMode )
        {
            m_iNextVideoMode = iNextVideoMode;
            if ( m_iCurrentVideoMode != iNextVideoMode )
            {
                bRequiresRestart = true;
            }
        }
    }

    // Windowed
    if ( m_bNextWindowed != bNextWindowed )
    {
        m_bNextWindowed = bNextWindowed;
        if ( m_bCurrentWindowed != bNextWindowed )
        {
            bRequiresRestart = true;
        }
    }

    // Full Screen Minimize
    m_bCurrentFullScreenMinimize = bNextFullScreenMinimize;

    // Fullscreen style
    if ( m_iNextFullscreenStyle != iNextFullscreenStyle )
    {
        m_iNextFullscreenStyle = iNextFullscreenStyle;
        if ( m_iCurrentFullscreenStyle != m_iNextFullscreenStyle )
        {
            if ( m_iCurrentFullscreenStyle == FULLSCREEN_STANDARD || m_iNextFullscreenStyle == FULLSCREEN_STANDARD )
            {
                bRequiresRestart = true;
            }
            else
            {
                m_iCurrentFullscreenStyle = m_iNextFullscreenStyle;
            }
        }
    }


    SaveCVars ();

    return bRequiresRestart;
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::LoadCVars
//
// Loads to current
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::LoadCVars ( void )
{
    // Upgrade display_alttab_handler
    bool bAltTabHandlerWasEnabled = CVARS_GET_VALUE < bool > ( "display_alttab_handler" );
    int iFullscreenStyle = CVARS_GET_VALUE < int > ( "display_fullscreen_style" );
    if ( bAltTabHandlerWasEnabled && iFullscreenStyle == 0 )
    {
        CVARS_SET ( "display_alttab_handler", false );
        CVARS_SET ( "display_fullscreen_style", FULLSCREEN_BORDERLESS );
    }

    m_iCurrentVideoMode = m_pGameSettings->GetCurrentVideoMode ();
    CVARS_GET ( "display_windowed",             m_bCurrentWindowed );
    CVARS_GET ( "display_fullscreen_style",     m_iCurrentFullscreenStyle );
    CVARS_GET ( "multimon_fullscreen_minimize", m_bCurrentFullScreenMinimize );
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::SaveCVars
//
// Saves from next
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::SaveCVars ( void )
{
    m_pGameSettings->SetCurrentVideoMode ( m_iNextVideoMode, true );
    CVARS_SET ( "display_windowed",             m_bNextWindowed );
    CVARS_SET ( "display_fullscreen_style",     m_iNextFullscreenStyle );
    CVARS_SET ( "multimon_fullscreen_minimize", m_bCurrentFullScreenMinimize );
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsWindowed
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsWindowed ( void )
{
    return ( IsDisplayModeWindowed() || IsDisplayModeFullScreenWindow() );
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsMultiMonitor
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsMultiMonitor ( void )
{
    if ( m_ulMonitorCount == 0 )
    {
        // Count attached monitors
        for ( int i = 0 ; true ; i++ )
        {
            DISPLAY_DEVICE device;
            device.cb = sizeof(device);

            // Get next DISPLAY_DEVICE from the system
            if( !EnumDisplayDevicesA ( NULL, i, &device, 0 ) )
                break;

            // Calc flags
            bool bAttachedToDesktop = ( device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) != 0;
            bool bMirroringDriver   = ( device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER ) != 0;
            bool bPrimaryDevice     = ( device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE ) != 0;

            // Ignore devices that are not required
            if( !bAttachedToDesktop || bMirroringDriver )
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
bool CVideoModeManager::IsMinimizeEnabled ( void )
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
bool CVideoModeManager::IsDisplayModeWindowed( void )
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
bool CVideoModeManager::IsDisplayModeFullScreen( void )
{
    return !m_bCurrentWindowed && ( m_iCurrentFullscreenStyle == FULLSCREEN_STANDARD );
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::IsDisplayModeFullScreenWindow
//
//
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::IsDisplayModeFullScreenWindow( void )
{
    return !m_bCurrentWindowed && ( m_iCurrentFullscreenStyle != FULLSCREEN_STANDARD );
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::GameResMatchesPrimaryMonitor
//
// Returns true if desktop matches the game requirement
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::GameResMatchesPrimaryMonitor( void )
{
    HDC hdcPrimaryMonitor = GetDC( NULL );
    int iDesktopResX = GetDeviceCaps( hdcPrimaryMonitor, HORZRES );
    int iDesktopResY = GetDeviceCaps( hdcPrimaryMonitor, VERTRES );
    int iDesktopColorDepth = GetDeviceCaps( hdcPrimaryMonitor, BITSPIXEL );

    if ( iDesktopResX == m_ulForceBackBufferWidth
        && iDesktopResY == m_ulForceBackBufferHeight
        && iDesktopColorDepth == m_ulForceBackBufferColorDepth )
    {
        return true;
    }

    return false;
}
