/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CTrayIcon.cpp
*  PURPOSE:     Create and destroy tray icon for MTA
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <strsafe.h>
#include "resource.h"

#define TRAY_BALLOON_TITLE      L"Notification from MTA:SA server"
#define TRAY_ICON_TOOLTIP_TEXT  L"Multi Theft Auto: San Andreas"
#define TRAY_BALLOON_INTERVAL   30000L // ms

extern CCore* g_pCore;
extern HINSTANCE g_hModule;

CTrayIcon::CTrayIcon ( void )
    : m_bTrayIconExists { false }
    , m_pNID { new NOTIFYICONDATAW { 0 } }
    , m_llLastBalloonTime { 0L }
{
    m_pNID->cbSize  = sizeof ( NOTIFYICONDATAW );
    m_pNID->uID     = 0;
    
    StringCchCopyW ( m_pNID->szTip, ARRAYSIZE ( m_pNID->szTip ), TRAY_ICON_TOOLTIP_TEXT );
    StringCchCopyW ( m_pNID->szInfoTitle, ARRAYSIZE ( m_pNID->szInfoTitle ), TRAY_BALLOON_TITLE );
}


CTrayIcon::~CTrayIcon ( void )
{
    if ( m_bTrayIconExists )
        DestroyTrayIcon ( );

    delete m_pNID;
}


bool CTrayIcon::CreateTrayIcon ( void )
{
    if ( m_bTrayIconExists )
        return true;

    // The handle to the core.dll is neccessary here,
    // because Windows will search for the ICON in the executable and not in the DLL
    // Note: Changing the size will not show a higher quality icon in the balloon
    auto hIcon        = LoadImage ( g_hModule, MAKEINTRESOURCE ( IDI_ICON1 ), IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_SHARED | LR_LOADTRANSPARENT );

    m_pNID->hWnd      = g_pCore->GetHookedWindow ( );
    m_pNID->uFlags    = NIF_ICON | NIF_TIP;
    m_pNID->hIcon     = ( hIcon != NULL ) ? ( ( HICON ) hIcon ) : LoadIcon ( NULL, IDI_APPLICATION );
    m_bTrayIconExists = Shell_NotifyIconW ( NIM_ADD, m_pNID ) == TRUE;
   
    return m_bTrayIconExists;
}


void CTrayIcon::DestroyTrayIcon ( void )
{
    if ( !m_bTrayIconExists )
        return;

    m_pNID->uFlags      = 0;
    m_pNID->dwInfoFlags = 0;
    Shell_NotifyIconW ( NIM_DELETE, m_pNID );
    m_bTrayIconExists   = false;
}


bool CTrayIcon::CreateTrayBallon ( SString strText, eTrayIconType trayIconType, bool useSound )
{
    if ( !m_bTrayIconExists )
        if ( !CreateTrayIcon ( ) )
            return false;

    auto currentTime = GetTickCount64_ ( );

    if ( ( currentTime - m_llLastBalloonTime ) < TRAY_BALLOON_INTERVAL )
        return false;
    else
        m_llLastBalloonTime = currentTime;

    m_pNID->dwInfoFlags = 0;
    m_pNID->uFlags      = NIF_ICON | NIF_TIP | NIF_INFO;
    StringCchCopyW ( m_pNID->szInfo, ARRAYSIZE ( m_pNID->szInfo ), SharedUtil::MbUTF8ToUTF16 ( strText ).c_str ( ) );

    switch ( trayIconType ) {
    case ICON_TYPE_INFO:
        m_pNID->dwInfoFlags |= NIIF_INFO;
        break;
    case ICON_TYPE_WARNING:
        m_pNID->dwInfoFlags |= NIIF_WARNING;
        break;
    case ICON_TYPE_ERROR:
        m_pNID->dwInfoFlags |= NIIF_ERROR;
        break;
    case ICON_TYPE_DEFAULT:
    default:
        break;
    }
    
    if ( !useSound )
        m_pNID->dwInfoFlags |= NIIF_NOSOUND;

    return Shell_NotifyIconW ( NIM_MODIFY, m_pNID ) == TRUE;
}
