/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        loader/Dialogs.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <sstream>

static bool          bCancelPressed = false;
static bool          bOkPressed = false;
static bool          bOtherPressed = false;
static int           iOtherCode = 0;
static WNDCLASS      splashWindowClass{};
static HWND          splashWindow{};
static HWND          hwndProgressDialog = NULL;
static unsigned long ulProgressStartTime = 0;
static HWND          hwndCrashedDialog = NULL;
static HWND          hwndGraphicsDllDialog = NULL;
static HWND          hwndOptimusDialog = NULL;
static HWND          hwndNoAvDialog = NULL;

/**
 * @brief Automatically destroys a window on scope-exit.
 */
struct WindowScope
{
    WindowScope(HWND handle_) noexcept : handle(handle_) {}

    ~WindowScope() noexcept { DestroyWindow(handle); }

    [[nodiscard]] HWND Release() noexcept { return std::exchange(handle, nullptr); }

    HWND handle{};
};

/**
 * @brief Provides a compatible memory-only device context for a bitmap handle and
 *        automatically destroys the device context and bitmap on scope-exit.
 */
struct BitmapScope
{
    BitmapScope(HDC deviceContext_, HBITMAP bitmap_) noexcept : bitmap(bitmap_)
    {
        if (bitmap != nullptr)
        {
            if (deviceContext = CreateCompatibleDC(deviceContext_))
            {
                previousObject = SelectObject(deviceContext, bitmap_);
            }
        }
    }

    ~BitmapScope() noexcept
    {
        if (previousObject && previousObject != HGDI_ERROR)
            SelectObject(deviceContext, previousObject);

        if (deviceContext)
            DeleteDC(deviceContext);

        if (bitmap)
            DeleteObject(bitmap);
    }

    HDC     deviceContext{};
    HGDIOBJ previousObject{};
    HBITMAP bitmap{};
};

/**
 * @brief Returns the dots per inch (dpi) value for the specified window.
 */
UINT GetWindowDpi(HWND window)
{
    // Minimum version: Windows 10, version 1607
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);

    static GetDpiForWindow_t Win32GetDpiForWindow = ([] {
        HMODULE user32 = LoadLibrary("user32");
        return user32 ? reinterpret_cast<GetDpiForWindow_t>(static_cast<void*>(GetProcAddress(user32, "GetDpiForWindow"))) : nullptr;
    })();

    if (Win32GetDpiForWindow)
        return Win32GetDpiForWindow(window);

    HDC  screenDC = GetDC(NULL);
    auto x = static_cast<UINT>(GetDeviceCaps(screenDC, LOGPIXELSX));
    ReleaseDC(NULL, screenDC);
    return x;
}

/**
 * @brief Returns the width and height of the primary monitor.
 */
SIZE GetPrimaryMonitorSize()
{
    POINT    zero{};
    HMONITOR primaryMonitor = MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    GetMonitorInfo(primaryMonitor, &monitorInfo);

    const RECT& work = monitorInfo.rcWork;
    return {work.right - work.left, work.bottom - work.top};
}

/**
 * @brief Scales the value from the default screen dpi (96) to the provided dpi.
 */
LONG ScaleToDpi(LONG value, UINT dpi)
{
    return static_cast<LONG>(ceil(value * static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI));
}

///////////////////////////////////////////////////////////////////////////
//
// Dialog strings
//
//
///////////////////////////////////////////////////////////////////////////
const char* const dialogStringsYes = _td("Yes");
const char* const dialogStringsNo = _td("No");
const char* const dialogStringsOk = _td("OK");
const char* const dialogStringsQuit = _td("Quit");
const char* const dialogStringsHelp = _td("Help");
const char* const dialogStringsCancel = _td("Cancel");

struct SDialogItemInfo
{
    int               iItemId;
    int               iLeadingSpaces;
    const char* const szItemText;
};

const SDialogItemInfo g_ProgressDialogItems[] = {
    {IDCANCEL, 0, dialogStringsCancel},
    {-1},
};

const SDialogItemInfo g_CrashedDialogItems[] = {
    {0, 0, _td("MTA: San Andreas has encountered a problem")},
    {IDC_CRASH_HEAD, 0, _td("Crash information")},
    {IDC_SEND_DUMP_CHECK, 0, _td("Tick the check box to send this crash info to MTA devs using the 'internet'")},
    {IDC_SEND_DESC_STATIC, 0, _td("Doing so will increase the chance of this crash being fixed.")},
    {IDC_RESTART_QUESTION_STATIC, 1, _td("Do you want to restart MTA: San Andreas ?")},
    {IDCANCEL, 0, dialogStringsNo},
    {IDOK, 0, dialogStringsYes},
    {-1},
};

const SDialogItemInfo g_GraphicsDllDialogItems[] = {
    {0, 0, _td("MTA: San Andreas - Warning")},
    {IDC_D3DDLL_TEXT1, 0, _td("Your Grand Theft Auto: San Andreas install directory contains these files:")},
    {IDC_D3DDLL_TEXT2, 0,
     _td("These files are not required and may interfere with the graphical features in this version of MTA:SA.\n\n"
         "It is recommended that you remove or rename these files.")},
    {IDC_NO_ACTION, 1, _td("Keep these files, but also show this warning on next start")},
    {IDC_CHECK_NOT_AGAIN, 1, _td("Do not remind me about these files again")},
    {IDC_APPLY_AUTOMATIC_CHANGES, 1, _td("Rename these files from *.dll to *.dll.bak")},
    {IDC_BUTTON_SHOW_DIR, 0, _td("Show me these files")},
    {IDOK, 0, _td("Play MTA:SA")},
    {IDCANCEL, 0, dialogStringsQuit},
    {-1},
};

const SDialogItemInfo g_OptimusDialogItems[] = {
    {0, 0, _td("MTA: San Andreas - Confusing options")},
    {IDC_OPTIMUS_TEXT1, 0, _td("NVidia Optimus detected!")},
    {IDC_OPTIMUS_TEXT2, 0, _td("Try each option and see what works:")},
    {IDC_RADIO1, 1, _td("A - Standard NVidia")},
    {IDC_RADIO2, 1, _td("B - Alternate NVidia")},
    {IDC_RADIO3, 1, _td("C - Standard NVidia with exe rename")},
    {IDC_RADIO4, 1, _td("D - Alternate NVidia with exe rename")},
    {IDC_RADIO5, 1, _td("E - Standard Intel")},
    {IDC_RADIO6, 1, _td("F - Alternate Intel")},
    {IDC_RADIO7, 1, _td("G - Standard Intel with exe rename")},
    {IDC_RADIO8, 1, _td("H - Alternate Intel with exe rename")},
    {IDC_OPTIMUS_TEXT3, 0, _td("If you get desperate, this might help:")},
    {IDC_OPTIMUS_TEXT4, 0, _td("If you have already selected an option that works, this might help:")},
    {IDC_CHECK_FORCE_WINDOWED, 1, _td("Force windowed mode")},
    {IDC_CHECK_REMEMBER, 1, _td("Don't show again")},
    {IDC_BUTTON_HELP, 0, dialogStringsHelp},
    {IDOK, 0, dialogStringsOk},
    {-1},
};

const SDialogItemInfo g_NoAvDialogItems[] = {
    {0, 0, _td("MTA: San Andreas")},
    {IDC_NOAV_TEXT1, 0, _td("Warning: Could not detect anti-virus product")},
    {IDC_NOAV_TEXT2, 0,
     _td("MTA could not detect an anti-virus on your PC.\n\n"
         "Viruses interfere with MTA and degrade your gameplay experience.\n\n"
         "Press 'Help' for more information.")},
    {IDC_NOAV_OPT_SKIP, 0, _td("I have already installed an anti-virus")},
    {IDC_NOAV_OPT_BOTNET, 0,
     _td("I will not install an anti-virus.\n"
         "I want my PC to lag and be part of a botnet.")},
    {IDC_BUTTON_HELP, 0, dialogStringsHelp},
    {IDOK, 0, dialogStringsOk},
    {-1},
};

///////////////////////////////////////////////////////////////////////////
//
// InitDialogStrings
//
// Copy strings into dialog
//
///////////////////////////////////////////////////////////////////////////
void InitDialogStrings(HWND hwndDialog, const SDialogItemInfo* dialogItems)
{
    for (uint i = 0; true; i++)
    {
        const SDialogItemInfo& item = dialogItems[i];
        if (item.iItemId == -1)
            return;

        HWND hwndItem;
        if (item.iItemId == 0)
            hwndItem = hwndDialog;
        else
            hwndItem = GetDlgItem(hwndDialog, item.iItemId);

        if (hwndItem)
        {
            SString strItemText = PadLeft(_(item.szItemText), item.iLeadingSpaces, ' ');
#if MTA_DEBUG
            char szPrevText[200] = "";
            GetWindowText(hwndItem, szPrevText, NUMELMS(szPrevText));
            if (!strItemText.EndsWith(szPrevText) && !SStringX(szPrevText).EndsWith(strItemText))
            {
                OutputDebugLine(
                    SString("Possible text mismatch for dialog item (idx:%d id:%d) '%s' (orig:'%s')", i, item.iItemId, item.szItemText, szPrevText));
            }
#endif
            SetWindowTextW(hwndItem, FromUTF8(strItemText));
        }
        else
            OutputDebugLine(SString("No dialog item for (idx:%d id:%d) '%s' ", i, item.iItemId, item.szItemText));
    }
}

///////////////////////////////////////////////////////////////////////////
//
// DialogProc
//
// Generic callback for all our dialogs
//
///////////////////////////////////////////////////////////////////////////
int CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDCANCEL:
                    bCancelPressed = true;
                    return TRUE;
                case IDOK:
                    bOkPressed = true;
                    return TRUE;
                default:
                    if (iOtherCode && iOtherCode == LOWORD(wParam))
                    {
                        bOtherPressed = true;
                        return TRUE;
                    }
            }
    }
    return FALSE;
}

//
// Show splash dialog
//
void ShowSplash(HINSTANCE hInstance)
{
#ifndef MTA_DEBUG
    if (splashWindowClass.hInstance != hInstance)
    {
        splashWindowClass.lpfnWndProc = DefWindowProc;
        splashWindowClass.hInstance = hInstance;
        splashWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        splashWindowClass.lpszClassName = TEXT("SplashWindow");
        RegisterClass(&splashWindowClass);
    }

    if (splashWindow)
    {
        ShowWindow(splashWindow, SW_SHOW);
    }
    else
    {
        WindowScope window(CreateWindowEx(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, splashWindowClass.lpszClassName, NULL, WS_POPUP | WS_VISIBLE,
                                          0, 0, 0, 0, NULL, NULL, hInstance, NULL));

        if (!window.handle)
            return;

        UINT dpi = GetWindowDpi(window.handle);
        SIZE monitorSize = GetPrimaryMonitorSize();

        POINT origin{};
        SIZE  windowSize{ScaleToDpi(500, dpi), ScaleToDpi(245, dpi)};
        origin.x = (monitorSize.cx - windowSize.cx) / 2;
        origin.y = (monitorSize.cy - windowSize.cy) / 2;

        HDC windowHDC = GetWindowDC(window.handle);
        {
            BitmapScope unscaled(windowHDC, LoadBitmap(hInstance, MAKEINTRESOURCE(IDB_BITMAP1)));

            if (!unscaled.bitmap)
                return;

            BitmapScope scaled(windowHDC, CreateCompatibleBitmap(windowHDC, windowSize.cx, windowSize.cy));

            if (!scaled.bitmap)
                return;

            BITMAP bitmap{};
            GetObject(unscaled.bitmap, sizeof(bitmap), &bitmap);

            // Draw the unscaled bitmap to the window-scaled bitmap.
            SetStretchBltMode(scaled.deviceContext, HALFTONE);
            StretchBlt(scaled.deviceContext, 0, 0, windowSize.cx, windowSize.cy, unscaled.deviceContext, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);

            // Update the splash window and draw the scaled bitmap.
            POINT         zero{};
            BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            UpdateLayeredWindow(window.handle, windowHDC, &origin, &windowSize, scaled.deviceContext, &zero, RGB(0, 0, 0), &blend, 0);
        }
        ReleaseDC(window.handle, windowHDC);

        splashWindow = window.Release();
    }

    // Drain messages to allow for repaint in case picture bits were lost during previous operations
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
#endif
}

//
// Hide splash dialog
//
void HideSplash()
{
    if (splashWindow)
    {
        DestroyWindow(splashWindow);
        splashWindow = {};
    }
}

//
// Hide splash dialog temporarily
//
void SuspendSplash()
{
    if (splashWindow)
    {
        ShowWindow(splashWindow, SW_HIDE);
    }
}

//
// Show splash dialog if was previously suspended
//
void ResumeSplash()
{
    if (splashWindow)
    {
        HideSplash();
        ShowSplash(g_hInstance);
    }
}

///////////////////////////////////////////////////////////////
//
// Progress dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowProgressDialog(HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel)
{
    if (!hwndProgressDialog)
    {
        HideSplash();
        bCancelPressed = false;
        hwndProgressDialog = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_PROGRESS_DIALOG), 0, DialogProc);
        dassert((GetWindowLong(hwndProgressDialog, GWL_STYLE) & WS_VISIBLE) == 0);            // Should be Visible: False
        InitDialogStrings(hwndProgressDialog, g_ProgressDialogItems);
        SetWindowTextW(hwndProgressDialog, FromUTF8(strTitle));
        ShowWindow(GetDlgItem(hwndProgressDialog, IDCANCEL), bAllowCancel ? SW_SHOW : SW_HIDE);
        ulProgressStartTime = GetTickCount32();
    }
    SetForegroundWindow(hwndProgressDialog);
    SetWindowPos(hwndProgressDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
}

void HideProgressDialog()
{
    if (hwndProgressDialog)
    {
        // Show progress for at least two seconds
        unsigned long ulTimeElapsed = GetTickCount32() - ulProgressStartTime;
        if (ulTimeElapsed < 2000)
        {
            UpdateProgress(100, 100);
            Sleep(2000 - ulTimeElapsed);
        }

        DestroyWindow(hwndProgressDialog);
        hwndProgressDialog = NULL;
    }
}

// returns true if canceled
bool UpdateProgress(int iPos, int iMax, const SString& strMsg)
{
    if (hwndProgressDialog)
    {
        HWND hwndText = GetDlgItem(hwndProgressDialog, IDC_PROGRESS_STATIC);
        char buffer[1024] = "";
        GetWindowText(hwndText, buffer, NUMELMS(buffer));
        if (strMsg.length() > 0 && strMsg != buffer)
            SetWindowTextW(hwndText, FromUTF8(strMsg));
        HWND hwndBar = GetDlgItem(hwndProgressDialog, IDC_PROGRESS_BAR);
        PostMessage(hwndBar, PBM_SETPOS, iPos * 100 / std::max(1, iMax), 0);
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        return bCancelPressed;
    }
    return false;
}

///////////////////////////////////////////////////////////////
//
// Progress
//
//
//
///////////////////////////////////////////////////////////////
void StartPseudoProgress(HINSTANCE hInstance, const SString& strTitle, const SString& strMsg)
{
    ShowProgressDialog(hInstance, strTitle);
    UpdateProgress(10, 100, strMsg);
    Sleep(100);
    UpdateProgress(30, 100);
}

void StopPseudoProgress()
{
    if (hwndProgressDialog)
    {
        UpdateProgress(60, 100);
        Sleep(100);
        UpdateProgress(90, 100);
        Sleep(100);
        HideProgressDialog();
    }
}

///////////////////////////////////////////////////////////////
//
// Crashed dialog
//
//
//
///////////////////////////////////////////////////////////////
SString ShowCrashedDialog(HINSTANCE hInstance, const SString& strMessage)
{
    if (!hwndCrashedDialog)
    {
        SuspendSplash();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = 0;
        hwndCrashedDialog = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_CRASHED_DIALOG), 0, DialogProc);
        dassert((GetWindowLong(hwndCrashedDialog, GWL_STYLE) & WS_VISIBLE) == 0);            // Should be Visible: False
        InitDialogStrings(hwndCrashedDialog, g_CrashedDialogItems);
        SetWindowTextW(GetDlgItem(hwndCrashedDialog, IDC_CRASH_INFO_EDIT), FromUTF8(strMessage));
        SendDlgItemMessage(hwndCrashedDialog, IDC_SEND_DUMP_CHECK, BM_SETCHECK,
                           GetApplicationSetting("diagnostics", "send-dumps") != "no" ? BST_CHECKED : BST_UNCHECKED, 0);
    }
    SetForegroundWindow(hwndCrashedDialog);
    SetWindowPos(hwndCrashedDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

    while (!bCancelPressed && !bOkPressed)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep(10);
    }

    LRESULT res = SendMessageA(GetDlgItem(hwndCrashedDialog, IDC_SEND_DUMP_CHECK), BM_GETCHECK, 0, 0);
    SetApplicationSetting("diagnostics", "send-dumps", res ? "yes" : "no");

    if (bCancelPressed)
        return "quit";

    ResumeSplash();

    // if ( bOkPressed )
    return "ok";
}

void HideCrashedDialog()
{
    if (hwndCrashedDialog)
    {
        DestroyWindow(hwndCrashedDialog);
        hwndCrashedDialog = NULL;
    }
}

///////////////////////////////////////////////////////////////
//
// Dialog for graphics libraries: d3d9.dll, dxgi.dll etc.
//
//
//
///////////////////////////////////////////////////////////////
void ShowGraphicsDllDialog(HINSTANCE hInstance, const std::vector<GraphicsLibrary>& offenders)
{
    if (offenders.empty())
        return;

    // Create and show dialog
    if (!hwndGraphicsDllDialog)
    {
        std::wstringstream libraryPaths;

        for (std::size_t i = 0; i < offenders.size(); i++)
        {
            libraryPaths << FromUTF8(offenders[i].absoluteFilePath);

            if ((i + 1) < offenders.size())
                libraryPaths << "\r\n";
        }

        SuspendSplash();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = IDC_BUTTON_SHOW_DIR;
        hwndGraphicsDllDialog = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_D3DDLL_DIALOG), 0, DialogProc);
        dassert((GetWindowLong(hwndGraphicsDllDialog, GWL_STYLE) & WS_VISIBLE) == 0);            // Should be Visible: False
        InitDialogStrings(hwndGraphicsDllDialog, g_GraphicsDllDialogItems);
        SetWindowTextW(GetDlgItem(hwndGraphicsDllDialog, IDC_EDIT_GRAPHICS_DLL_PATH), libraryPaths.str().c_str());
        SendMessage(GetDlgItem(hwndGraphicsDllDialog, IDC_NO_ACTION), BM_SETCHECK, BST_CHECKED, 1);
    }
    SetForegroundWindow(hwndGraphicsDllDialog);
    SetWindowPos(hwndGraphicsDllDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

    // Wait for input
    while (!bCancelPressed && !bOkPressed && !bOtherPressed)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep(10);
    }

    // Process input
    bool doNotCheckAgainOption = SendMessageA(GetDlgItem(hwndGraphicsDllDialog, IDC_CHECK_NOT_AGAIN), BM_GETCHECK, 0, 0) == BST_CHECKED;

    for (const GraphicsLibrary& library : offenders)
    {
        SetApplicationSetting("diagnostics", library.appLastHash, library.md5Hash);
        SetApplicationSetting("diagnostics", library.appDontRemind, doNotCheckAgainOption ? "yes" : "no");
    }

    if (bOkPressed)
    {
        if (!doNotCheckAgainOption)
        {
            bool doRenameOption = SendMessageA(GetDlgItem(hwndGraphicsDllDialog, IDC_APPLY_AUTOMATIC_CHANGES), BM_GETCHECK, 0, 0) == BST_CHECKED;

            if (doRenameOption)
            {
                for (const GraphicsLibrary& library : offenders)
                {
                    FileRename(library.absoluteFilePath, library.absoluteFilePath + ".bak");
                }
            }
        }
    }
    else if (bCancelPressed)
    {
        ExitProcess(0);
    }
    else if (bOtherPressed)
    {
        // We grab the first offender's absolute file path to retrieve GTA's install directory
        SString      gtaDirectory = ExtractPath(offenders.front().absoluteFilePath);
        LPITEMIDLIST gtaDirectoryItem = ILCreateFromPathW(FromUTF8(gtaDirectory));

        std::vector<LPITEMIDLIST> selectedItems;

        if (gtaDirectoryItem != nullptr)
        {
            for (const GraphicsLibrary& library : offenders)
            {
                if (LPITEMIDLIST item = ILCreateFromPathW(FromUTF8(library.absoluteFilePath)); item != nullptr)
                {
                    selectedItems.emplace_back(item);
                }
            }
        }

        bool useFallback = true;

        if (gtaDirectoryItem != nullptr)
        {
            // Open a Windows Explorer window for the GTA install directory and select all offending graphic libraries
            if (SUCCEEDED(SHOpenFolderAndSelectItems(gtaDirectoryItem, selectedItems.size(), const_cast<LPCITEMIDLIST*>(selectedItems.data()), 0)))
            {
                useFallback = false;
            }

            for (LPITEMIDLIST item : selectedItems)
                ILFree(item);

            ILFree(gtaDirectoryItem);
        }

        if (useFallback)
        {
            // Fallback: Open the parent directory with a shell command
            ShellExecuteNonBlocking("open", gtaDirectory);
        }

        ExitProcess(0);
    }

    ResumeSplash();
}

void HideGraphicsDllDialog()
{
    if (hwndGraphicsDllDialog)
    {
        DestroyWindow(hwndGraphicsDllDialog);
        hwndGraphicsDllDialog = NULL;
    }
}

///////////////////////////////////////////////////////////////
//
// Optimus dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowOptimusDialog(HINSTANCE hInstance)
{
    if (GetApplicationSettingInt("nvhacks", "optimus-remember-option"))
    {
        return;
    }

    if (GetApplicationSettingInt("nvhacks", "optimus-dialog-skip"))
    {
        SetApplicationSettingInt("nvhacks", "optimus-dialog-skip", 0);
        return;
    }

    uint RadioButtons[] = {IDC_RADIO1, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4, IDC_RADIO5, IDC_RADIO6, IDC_RADIO7, IDC_RADIO8};
    // Create and show dialog
    if (!hwndOptimusDialog)
    {
        SuspendSplash();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = IDC_BUTTON_HELP;
        hwndOptimusDialog = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_OPTIMUS_DIALOG), 0, DialogProc);
        dassert((GetWindowLong(hwndOptimusDialog, GWL_STYLE) & WS_VISIBLE) == 0);            // Should be Visible: False
        InitDialogStrings(hwndOptimusDialog, g_OptimusDialogItems);
        uint uiStartupOption = GetApplicationSettingInt("nvhacks", "optimus-startup-option") % NUMELMS(RadioButtons);
        uint uiForceWindowed = GetApplicationSettingInt("nvhacks", "optimus-force-windowed");
        CheckRadioButton(hwndOptimusDialog, IDC_RADIO1, IDC_RADIO8, RadioButtons[uiStartupOption]);
        CheckDlgButton(hwndOptimusDialog, IDC_CHECK_FORCE_WINDOWED, uiForceWindowed);
    }
    SetForegroundWindow(hwndOptimusDialog);
    SetWindowPos(hwndOptimusDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

    // Wait for input
    while (!bCancelPressed && !bOkPressed && !bOtherPressed)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep(10);
    }

    if (bOtherPressed)
    {
        BrowseToSolution("optimus-startup-option-help", TERMINATE_PROCESS);
    }

    // Process input
    uint uiStartupOption = 0;
    for (; uiStartupOption < NUMELMS(RadioButtons) - 1; uiStartupOption++)
    {
        if (IsDlgButtonChecked(hwndOptimusDialog, RadioButtons[uiStartupOption]) == BST_CHECKED)
            break;
    }
    uint uiForceWindowed = (IsDlgButtonChecked(hwndOptimusDialog, IDC_CHECK_FORCE_WINDOWED) == BST_CHECKED) ? 1 : 0;

    uint uiRememberOption = (IsDlgButtonChecked(hwndOptimusDialog, IDC_CHECK_REMEMBER) == BST_CHECKED) ? 1 : 0;

    SetApplicationSettingInt("nvhacks", "optimus-startup-option", uiStartupOption);
    SetApplicationSettingInt("nvhacks", "optimus-alt-startup", (uiStartupOption & 1) ? 1 : 0);
    SetApplicationSettingInt("nvhacks", "optimus-rename-exe", (uiStartupOption & 2) ? 1 : 0);
    SetApplicationSettingInt("nvhacks", "optimus-export-enablement", (uiStartupOption & 4) ? 0 : 1);
    SetApplicationSettingInt("nvhacks", "optimus-force-windowed", uiForceWindowed);
    SetApplicationSettingInt("nvhacks", "optimus-remember-option", uiRememberOption);

    if (!GetInstallManager()->UpdateOptimusSymbolExport())
    {
        // Restart required to apply change because of permissions
        SetApplicationSettingInt("nvhacks", "optimus-dialog-skip", 1);
        ShellExecuteNonBlocking("open", PathJoin(GetMTASAPath(), MTA_EXE_NAME));
        TerminateProcess(GetCurrentProcess(), 0);
    }
    ResumeSplash();
}

void HideOptimusDialog()
{
    if (hwndOptimusDialog)
    {
        DestroyWindow(hwndOptimusDialog);
        hwndOptimusDialog = NULL;
    }
}

///////////////////////////////////////////////////////////////
//
// No anti-virus dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowNoAvDialog(HINSTANCE hInstance, bool bEnableScaremongering)
{
    uint uiTimeLastAsked = GetApplicationSettingInt("noav-last-asked-time");
    bool bUserSaysNo = GetApplicationSettingInt("noav-user-says-skip") != 0;

    // Don't display dialog on first run
    if (uiTimeLastAsked == 0)
    {
        SetApplicationSettingInt("noav-last-asked-time", 1);
        return;
    }

    // Time to ask again?
    uint uiAskHoursInterval;
    if (!bUserSaysNo)
        uiAskHoursInterval = 1;            // Once an hour if box not ticked
    else
    {
        if (bEnableScaremongering)
            uiAskHoursInterval = 24 * 7;            // Once a week if ticked
        else
            uiAskHoursInterval = 24 * 365 * 1000;            // Once every 1000 years if ticked and WSC not monitoring
    }

    uint uiTimeNow = static_cast<uint>(time(NULL) / 3600LL);
    uint uiHoursSinceLastAsked = uiTimeNow - uiTimeLastAsked;
    if (uiHoursSinceLastAsked < uiAskHoursInterval)
        return;

    // Create and show dialog
    if (!hwndNoAvDialog)
    {
        SuspendSplash();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = IDC_BUTTON_HELP;
        hwndNoAvDialog = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_NOAV_DIALOG), 0, DialogProc);
        dassert((GetWindowLongW(hwndNoAvDialog, GWL_STYLE) & WS_VISIBLE) == 0);            // Should be Visible: False
        InitDialogStrings(hwndNoAvDialog, g_NoAvDialogItems);
        ShowWindow(GetDlgItem(hwndNoAvDialog, IDC_NOAV_OPT_SKIP), bEnableScaremongering ? SW_HIDE : SW_SHOW);
        ShowWindow(GetDlgItem(hwndNoAvDialog, IDC_NOAV_OPT_BOTNET), bEnableScaremongering ? SW_SHOW : SW_HIDE);
    }
    ShowWindow(hwndNoAvDialog, SW_SHOW);            // Show after all changes are complete
    SetForegroundWindow(hwndNoAvDialog);
    SetWindowPos(hwndNoAvDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Wait for input
    while (!bCancelPressed && !bOkPressed && !bOtherPressed)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (GetMessage(&msg, NULL, 0, 0))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep(10);
    }

    // Process input
    if (bOtherPressed)
    {
        ShowWindow(hwndNoAvDialog, SW_HIDE);
        BrowseToSolution("no-anti-virus", TERMINATE_PROCESS);
    }

    LRESULT res = SendMessageA(GetDlgItem(hwndNoAvDialog, IDC_CHECK_NOT_AGAIN), BM_GETCHECK, 0, 0);
    SetApplicationSettingInt("noav-last-asked-time", uiTimeNow);
    SetApplicationSettingInt("noav-user-says-skip", res ? 1 : 0);

    ResumeSplash();
}

void HideNoAvDialog()
{
    if (hwndNoAvDialog)
    {
        DestroyWindow(hwndNoAvDialog);
        hwndNoAvDialog = NULL;
    }
}

#ifdef MTA_DEBUG
///////////////////////////////////////////////////////////////
//
// TestDialogs
//
// For checking string are correct
//
///////////////////////////////////////////////////////////////
void TestDialogs()
{
#if 0
#if 1
    ShowProgressDialog( g_hInstance, _("Searching for Grand Theft Auto San Andreas"), true );
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        UpdateProgress ( i, 100, _("Please start Grand Theft Auto San Andreas") );
        Sleep( 10 );
    }
    HideProgressDialog();

    ShowProgressDialog( g_hInstance, "MTA: San Andreas" );
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        UpdateProgress( i, 100, _("Installing update...") );
        Sleep( 10 );
    }
    HideProgressDialog();

    ShowProgressDialog( g_hInstance, "MTA: San Andreas" );
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        UpdateProgress( i, 100, _("Extracting files..." ) );
        Sleep( 10 );
    }
    HideProgressDialog();

    ShowProgressDialog( g_hInstance, _("Copying files..."), true );
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        if ( i > 66 )
        {
            UpdateProgress ( i, 100, _("Copy finished early. Everything OK.") );
        }
        else
        if ( i > 66 )
        {
            UpdateProgress ( i, 100, _("Finishing...") );
        }
        else
        {
            UpdateProgress ( i, 100, _("Done!") );
        }
        Sleep( 10 );
    }
    HideProgressDialog();
#endif

#if 1
    ShowCrashedDialog( g_hInstance, "test2" );
    HideCrashedDialog();
#endif

#if 1
    SetApplicationSetting("diagnostics", "d3d9-dll-last-hash", "123");
    std::vector<GraphicsLibrary> offenders{GraphicsLibrary{"dummy"}};
    ShowGraphicsDllDialog(g_hInstance, offenders);
    HideGraphicsDllDialog();
#endif

#if 1
    ShowOptimusDialog( g_hInstance );
    HideOptimusDialog();
#endif

#if 1
    // Friendly option
    SetApplicationSettingInt( "noav-last-asked-time", 1 );
    SetApplicationSettingInt( "noav-user-says-skip", 0 );
    ShowNoAvDialog( g_hInstance, true );
    HideNoAvDialog();
#endif

#if 1
    // Scaremongering option option
    SetApplicationSettingInt( "noav-last-asked-time", 1 );
    SetApplicationSettingInt( "noav-user-says-skip", 0 );
    ShowNoAvDialog( g_hInstance, false );
    HideNoAvDialog();
#endif
#endif
}
#endif
