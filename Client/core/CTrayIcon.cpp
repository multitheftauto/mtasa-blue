/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CTrayIcon.cpp
 *  PURPOSE:     Create and destroy tray icon for MTA
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <strsafe.h>
#include "resource.h"

#define TRAY_DUMMY_WINDOW_NAME  L"NotificationsDummy"
#define TRAY_BALLOON_TITLE      L"Notification from MTA:SA server"
#define TRAY_ICON_TOOLTIP_TEXT  L"Multi Theft Auto: San Andreas"
#define TRAY_BALLOON_INTERVAL   30000L // ms

extern HINSTANCE g_hModule;

CTrayIcon::CTrayIcon() : m_bTrayIconExists{false}, m_pNID{new NOTIFYICONDATAW{0}}, m_llLastBalloonTime{0L}
{
    m_pNID->cbSize = sizeof(NOTIFYICONDATAW);
    m_pNID->uID = 0;

    StringCchCopyW(m_pNID->szTip, ARRAYSIZE(m_pNID->szTip), TRAY_ICON_TOOLTIP_TEXT);
    StringCchCopyW(m_pNID->szInfoTitle, ARRAYSIZE(m_pNID->szInfoTitle), TRAY_BALLOON_TITLE);
}

CTrayIcon::~CTrayIcon()
{
    if (m_bTrayIconExists)
        DestroyTrayIcon();

    delete m_pNID;
}

bool CTrayIcon::CreateTrayIcon()
{
    if (m_bTrayIconExists)
        return true;

    // Register window class for dummy notifications window
    WNDCLASSEXW wcDummy;
    wcDummy.cbSize = sizeof(WNDCLASSEXW);
    wcDummy.style = 0;
    wcDummy.lpfnWndProc = ProcessNotificationsWindowMessage;
    wcDummy.cbClsExtra = 0;
    wcDummy.cbWndExtra = 0;
    wcDummy.hInstance = g_hModule;
    wcDummy.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcDummy.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcDummy.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcDummy.lpszMenuName = NULL;
    wcDummy.lpszClassName = TRAY_DUMMY_WINDOW_NAME;
    wcDummy.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    if (!RegisterClassExW(&wcDummy))
        return false;

    // Create dummy notifications window
    m_pNID->hWnd = CreateWindowExW(0, TRAY_DUMMY_WINDOW_NAME, L"", 0, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, g_hModule, NULL);
    if (m_pNID->hWnd != NULL)
        SetWindowLongPtrW(m_pNID->hWnd, 0, reinterpret_cast<LONG_PTR>(this));
    else
    {
        UnregisterClassW(TRAY_DUMMY_WINDOW_NAME, g_hModule);
        return false;
    }

    // The handle to the core.dll is neccessary here,
    // because Windows will search for the ICON in the executable and not in the DLL
    // Note: Changing the size will not show a higher quality icon in the balloon
    auto hIcon = LoadImage(g_hModule, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_SHARED | LR_LOADTRANSPARENT);

    m_pNID->uFlags = NIF_ICON | NIF_TIP;
    m_pNID->hIcon = (hIcon != NULL) ? ((HICON)hIcon) : LoadIcon(NULL, IDI_APPLICATION);
    m_bTrayIconExists = Shell_NotifyIconW(NIM_ADD, m_pNID) == TRUE;

    return m_bTrayIconExists;
}

void CTrayIcon::DestroyTrayIcon()
{
    if (!m_bTrayIconExists)
        return;

    m_pNID->uFlags = 0;
    m_pNID->dwInfoFlags = 0;
    Shell_NotifyIconW(NIM_DELETE, m_pNID);
    m_bTrayIconExists = false;

    // Destroy the dummy window
    if (IsWindow(m_pNID->hWnd))
        DestroyWindow(m_pNID->hWnd);

    UnregisterClassW(TRAY_DUMMY_WINDOW_NAME, g_hModule);
}

bool CTrayIcon::CreateTrayBallon(SString strText, eTrayIconType trayIconType, bool useSound)
{
    if (!m_bTrayIconExists)
        if (!CreateTrayIcon())
            return false;

    auto currentTime = GetTickCount64_();

    if ((currentTime - m_llLastBalloonTime) < TRAY_BALLOON_INTERVAL)
        return false;
    else
        m_llLastBalloonTime = currentTime;

    m_pNID->dwInfoFlags = 0;
    m_pNID->uFlags = NIF_ICON | NIF_TIP | NIF_INFO;
    StringCchCopyW(m_pNID->szInfo, ARRAYSIZE(m_pNID->szInfo), SharedUtil::MbUTF8ToUTF16(strText).c_str());

    switch (trayIconType)
    {
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

    if (!useSound)
        m_pNID->dwInfoFlags |= NIIF_NOSOUND;

    return Shell_NotifyIconW(NIM_MODIFY, m_pNID) == TRUE;
}

LRESULT CALLBACK CTrayIcon::ProcessNotificationsWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_DESTROY:
        case WM_NCDESTROY:
        {
            // Destroy the tray icon if dummy window was destroyed
            // In case of unexpected window crash
            CTrayIcon* pThis = reinterpret_cast<CTrayIcon*>(GetWindowLongPtrW(hwnd, 0));
            if (pThis != NULL)
                pThis->DestroyTrayIcon();
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
