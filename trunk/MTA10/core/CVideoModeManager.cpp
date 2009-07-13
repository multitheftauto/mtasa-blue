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

using std::string;

//////////////////////////////////////////////////////////
//
// Helper stuff
//

#define ZERO_ON_NEW \
    void* operator new ( size_t size )              { void* ptr = ::operator new(size); memset(ptr,0,size); return ptr; } \
    void* operator new ( size_t size, void* where ) { memset(where,0,size); return where; }



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
    virtual void        GetNextVideoMode            ( int& iOutNextVideoMode, bool& bOutNextWindowed );
    virtual bool        SetVideoMode                ( int nextVideoMode, bool bNextWindowed );
    virtual bool        IsWindowed                  ( void );
    virtual bool        IsMultiMonitor              ( void );
    virtual bool        IsMinimizeEnabled           ( void );
    virtual void        SetMinimizeEnabled          ( bool bOn );

private:
    void                LoadCVars                   ( void );
    void                SaveCVars                   ( void );

    bool                m_bForceFullScreenOnce;
    bool                m_bForceWindowed;
    unsigned long       m_ulForceBackBufferWidth;
    unsigned long       m_ulForceBackBufferHeight;
    unsigned long       m_ulFullScreenRefreshRate;
    HWND                m_hDeviceWindow;
    CGameSettings *     m_pGameSettings;
    unsigned long       m_ulMonitorCount;

    int                 m_iCurrentVideoMode;    // VideoMode this run
    bool                m_bCurrentWindowed;
    int                 m_iNextVideoMode;       // VideoMode next run
    bool                m_bNextWindowed;
    bool                m_bFullScreenMinimize;
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

    // Remember this for later
    m_ulFullScreenRefreshRate = pp->FullScreen_RefreshRateInHz;

    // This block helps stability
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


    // Final bits
    if ( m_bCurrentWindowed )
    {
        m_bForceFullScreenOnce = false;
        m_bForceWindowed = true;
        m_ulForceBackBufferWidth  = pp->BackBufferWidth;
        m_ulForceBackBufferHeight = pp->BackBufferHeight;
    }
    else
    {
        m_bForceFullScreenOnce = true;
        m_bForceWindowed = false;
        m_ulForceBackBufferWidth  = pp->BackBufferWidth;
        m_ulForceBackBufferHeight = pp->BackBufferHeight;
    }
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
    // This helps stability
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
    if ( m_bForceFullScreenOnce )
    {
        m_bForceFullScreenOnce = false;
        pp->Windowed = false;
    }
    else
    if ( m_bForceWindowed )
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
        LONG Style = WS_VISIBLE | WS_CLIPSIBLINGS | WS_BORDER | WS_DLGFRAME | WS_SYSMENU;
        SetWindowLong ( m_hDeviceWindow, GWL_STYLE, Style );

        LONG ExStyle = 0;//WS_EX_WINDOWEDGE;
        SetWindowLong ( m_hDeviceWindow, GWL_EXSTYLE, ExStyle );

        // Ensure client area of window is correct size
        RECT ClientRect = { 0, 0, pp->BackBufferWidth, pp->BackBufferHeight };
        AdjustWindowRect( &ClientRect, GetWindowLong(m_hDeviceWindow,GWL_STYLE), FALSE );

        int SizeX = ClientRect.right - ClientRect.left;
        int SizeY = ClientRect.bottom - ClientRect.top;

        SetWindowPos( m_hDeviceWindow, HWND_NOTOPMOST, 0, 0, SizeX, SizeY, SWP_NOMOVE | SWP_FRAMECHANGED );
    }
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::GetNextVideoMode
//
//
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::GetNextVideoMode ( int& iOutNextVideoMode, bool& bOutNextWindowed )
{
    iOutNextVideoMode   = m_iNextVideoMode;
    bOutNextWindowed    = m_bNextWindowed;
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::SetVideoMode
//
// Returns true if restart is required
//
///////////////////////////////////////////////////////////////
bool CVideoModeManager::SetVideoMode ( int iNextVideoMode, bool bNextWindowed )
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
    m_iCurrentVideoMode = m_pGameSettings->GetCurrentVideoMode ();
    CVARS_GET ( "display_windowed",             m_bCurrentWindowed );
    CVARS_GET ( "multimon_fullscreen_minimize", m_bFullScreenMinimize );
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
    return m_bCurrentWindowed;
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
    return m_bFullScreenMinimize;
}


///////////////////////////////////////////////////////////////
//
// CVideoModeManager::SetMinimizeEnabled
//
// Multi-monitor, full screen, minimize
//
///////////////////////////////////////////////////////////////
void CVideoModeManager::SetMinimizeEnabled ( bool bOn )
{
    if ( m_bFullScreenMinimize != bOn )
    {
        m_bFullScreenMinimize = bOn;
        CVARS_SET ( "multimon_fullscreen_minimize", m_bFullScreenMinimize );
    }
}
