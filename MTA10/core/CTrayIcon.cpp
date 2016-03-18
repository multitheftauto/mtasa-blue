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

#define TRAY_MTA_TITLE L"Multi Theft Auto: San Andreas"

extern CCore* g_pCore;

CTrayIcon::CTrayIcon ( void )
    : m_bTrayIconExists { false },
    m_pNID { new NOTIFYICONDATAW { 0 } }
{
    m_pNID->cbSize  = sizeof ( NOTIFYICONDATAW );
    m_pNID->uID     = 0;
    
    StringCchCopyW ( m_pNID->szTip, ARRAYSIZE ( m_pNID->szTip ), TRAY_MTA_TITLE );
    StringCchCopyW ( m_pNID->szInfoTitle, ARRAYSIZE ( m_pNID->szInfoTitle ), TRAY_MTA_TITLE );
}

CTrayIcon::~CTrayIcon ( void )
{
    delete m_pNID;
}

bool CTrayIcon::CreateTrayIcon ( void )
{
    if ( m_bTrayIconExists )
        return true;
    
    m_pNID->hWnd    = g_pCore->GetHookedWindow ( );
    m_pNID->uFlags  = NIF_ICON | NIF_TIP;
    m_pNID->hIcon   = LoadIcon ( NULL, IDI_APPLICATION );
    // m_pNID->hIcon   = LoadIcon ( GetModuleHandle ( NULL ), MAKEINTRESOURCE ( IDI_ICON1 ) );
    
    m_bTrayIconExists = Shell_NotifyIconW ( NIM_ADD, m_pNID ) == S_OK;

    return m_bTrayIconExists;
}

bool CTrayIcon::DestroyTrayIcon ( void )
{
    if ( !m_bTrayIconExists )
        return true;

    Shell_NotifyIconW ( NIM_DELETE, m_pNID );
    m_bTrayIconExists = false;

    return true;
}

bool CTrayIcon::CreateTrayBallon ( SString strText, CTrayIconType trayIconType, bool useSound )
{
    if ( !m_bTrayIconExists )
        if ( !CreateTrayIcon ( ) )
            return false;

    m_pNID->dwInfoFlags = 0;
    m_pNID->uFlags      = NIF_ICON | NIF_TIP | NIF_INFO;
    StringCchCopyW ( m_pNID->szInfo, ARRAYSIZE ( m_pNID->szInfo ), SharedUtil::MbUTF8ToUTF16 ( strText ).c_str ( ) );

    switch ( trayIconType ) {
    case CTrayIconType::Info:
        m_pNID->dwInfoFlags |= NIIF_INFO;
        break;
    case CTrayIconType::Warning:
        m_pNID->dwInfoFlags |= NIIF_WARNING;
        break;
    case CTrayIconType::Error:
        m_pNID->dwInfoFlags |= NIIF_ERROR;
        break;
    case CTrayIconType::Default:
    default:
        break;
    }
    
    if ( !useSound )
        m_pNID->dwInfoFlags |= NIIF_NOSOUND;

    return Shell_NotifyIconW ( NIM_MODIFY, m_pNID ) == S_OK;
}
