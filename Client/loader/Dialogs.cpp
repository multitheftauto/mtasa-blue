/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/loader/Dialogs.cpp
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "Dialogs.h"
#include "resource.h"
#include "Utils.h"
#include "CInstallManager.h"
#include "Main.h"
#include <sstream>
#include <optional>
#include <string_view>
#include <atomic>
#include <array>

// Define STN_CLICKED if not already defined
#ifndef STN_CLICKED
#define STN_CLICKED 0
#endif

// Define IDC_SEND_DUMP_LABEL if not in resource.h yet
#ifndef IDC_SEND_DUMP_LABEL
#define IDC_SEND_DUMP_LABEL 1040
#endif

static std::atomic<bool> bCancelPressed{false};
static std::atomic<bool> bOkPressed{false};
static std::atomic<bool> bOtherPressed{false};
static int               iOtherCode = 0;
static HWND              hwndProgressDialog = nullptr;
static unsigned long     ulProgressStartTime = 0;
static HWND              hwndCrashedDialog = nullptr;
static HWND              hwndGraphicsDllDialog = nullptr;
static HWND              hwndOptimusDialog = nullptr;
static HWND              hwndNoAvDialog = nullptr;

// Constants for crash dialog timeout
namespace {
    inline constexpr DWORD MAX_WAIT_TIME = 30000;
    static_assert(MAX_WAIT_TIME > 0 && MAX_WAIT_TIME <= 60000, "Timeout must be between 0 and 60 seconds");
    
    inline constexpr std::string_view ERROR_MSG_PREFIX = "ShowCrashedDialog: CreateDialogW failed with error ";
    inline constexpr std::string_view CRASH_MSG_DIALOG_FAIL = "MTA has crashed. (Failed to create crash dialog)";
    inline constexpr std::string_view CRASH_TITLE_FATAL = "MTA: San Andreas - Fatal Error";
    inline constexpr std::wstring_view DEFAULT_ERROR_MSG = L"An error occurred. No details available.";
    inline constexpr size_t MAX_CRASH_MESSAGE_LENGTH = 65536;
    static_assert(ERROR_MSG_PREFIX.size() <= INT_MAX, "ERROR_MSG_PREFIX too long for int cast");
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
    {IDC_RADIO3, 1, _td("C - Standard Intel")},
    {IDC_RADIO4, 1, _td("D - Alternate Intel")},
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
    {IDC_NOAV_OPT_SKIP, 1, _td("I have already installed an anti-virus")},
    {IDC_NOAV_OPT_BOTNET, 1,
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
        case WM_CLOSE:
            // Prevent dialog from closing via WM_CLOSE (Alt+F4, X button, parent termination, etc.)
            // User must explicitly click a button. This prevents premature closure of crash dialog.
            if (hwnd == hwndCrashedDialog)
            {
                // Block WM_CLOSE for crash dialog - require explicit user button press
                return TRUE;
            }
            // Allow other dialogs to close normally
            break;

        case WM_SYSCOMMAND:
            // Block system menu commands that could hide or close the crash dialog
            if (hwnd == hwndCrashedDialog)
            {
                if (wParam == SC_CLOSE || wParam == SC_MINIMIZE || wParam == SC_MAXIMIZE)
                    return TRUE;
            }
            break;

        case WM_COMMAND:
            // Validate message source
            if (lParam != 0 && !IsWindow(reinterpret_cast<HWND>(lParam)))
                break;
            
            switch (LOWORD(wParam))
            {
                case IDCANCEL:
                    // For crash dialog, save checkbox state before exiting
                    if (hwnd == hwndCrashedDialog)
                    {
                        if (HWND hwndCheck = GetDlgItem(hwnd, IDC_SEND_DUMP_CHECK))
                        {
                            LRESULT res = SendMessageA(hwndCheck, BM_GETCHECK, 0, 0);
                            try {
                                SetApplicationSetting("diagnostics", "send-dumps", (res == BST_CHECKED) ? "yes" : "no");
                            }
                            catch (...) {
                            }
                        }
                    }
                    bCancelPressed = true;
                    return TRUE;
                case IDOK:
                    // For crash dialog, save checkbox state before exiting
                    if (hwnd == hwndCrashedDialog)
                    {
                        if (HWND hwndCheck = GetDlgItem(hwnd, IDC_SEND_DUMP_CHECK))
                        {
                            LRESULT res = SendMessageA(hwndCheck, BM_GETCHECK, 0, 0);
                            try {
                                SetApplicationSetting("diagnostics", "send-dumps", (res == BST_CHECKED) ? "yes" : "no");
                            }
                            catch (...) {
                            }
                        }
                    }
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

// This function isn't overengineered, it needs to be extremely secure to avoid maliciously triggered crashes from escaping and doing bad things.
[[nodiscard]] SString ShowCrashedDialog(HINSTANCE hInstance, const SString& strMessage)
{
    using namespace std::string_view_literals;
    
    if (auto validHandle = std::optional{hInstance ? hInstance : GetModuleHandle(nullptr)}; 
        !validHandle || !*validHandle) [[unlikely]]
    {
        try {
            MessageBoxA(nullptr, "MTA has crashed."sv.data(), "MTA: San Andreas"sv.data(), MB_OK | MB_ICONERROR | MB_TOPMOST);
        }
        catch (...) {
        }
        return "quit";
    }
    else
    {
        hInstance = *validHandle;
    }
    
    const auto strSanitizedMessage = [&message = std::as_const(strMessage)]() noexcept -> SString {
        try {
            if constexpr (MAX_CRASH_MESSAGE_LENGTH > 0) {
                const auto messageLength = std::clamp(std::size(message), size_t{0}, MAX_CRASH_MESSAGE_LENGTH);
                if (std::size(message) > MAX_CRASH_MESSAGE_LENGTH)
                    return message.substr(0, messageLength);
                else
                    return message;
            } else {
                return message;
            }
        }
        catch (...) {
            return SString{};
        }
    }();
    
    struct ReentranceGuard final
    {
        std::atomic<bool>& flag;
        bool isValid{true};
        
        explicit ReentranceGuard(std::atomic<bool>& f) noexcept : flag(f),
            isValid(!flag.exchange(true, std::memory_order_acquire))
        {
        }
        
        ~ReentranceGuard() noexcept
        {
            if (isValid) [[likely]]
                flag.store(false, std::memory_order_release);
        }
        
        [[nodiscard]] explicit operator bool() const noexcept { return isValid; }
        
        ReentranceGuard(const ReentranceGuard&) = delete;
        ReentranceGuard& operator=(const ReentranceGuard&) = delete;
        ReentranceGuard(ReentranceGuard&&) = delete;
        ReentranceGuard& operator=(ReentranceGuard&&) = delete;
    };
    
    static std::atomic<bool> bInShowCrashedDialog{false};
    
    try {
        ReentranceGuard guard{bInShowCrashedDialog};
        
        if (!guard) [[unlikely]]
        {
            try {
                MessageBoxA(nullptr, "MTA has crashed."sv.data(), "MTA: San Andreas"sv.data(),
                           MB_OK | MB_ICONERROR | MB_TOPMOST | MB_SETFOREGROUND);
            }
            catch (...) {
            }
            return "quit";
        }
        
        if ([[maybe_unused]] const auto displayResult = ChangeDisplaySettings(nullptr, 0); true) {
            constexpr auto displaySettingsDelay = 300;
            Sleep(displaySettingsDelay);
        }
        
        if (!hwndCrashedDialog)
        {
            try {
                SuspendSplash();
            }
            catch (...) {
            }
            
            // Reset button states
            bCancelPressed.store(false, std::memory_order_release);
            bOkPressed.store(false, std::memory_order_release);
            
            hwndCrashedDialog = CreateDialogW(hInstance, MAKEINTRESOURCEW(IDD_CRASHED_DIALOG), nullptr, DialogProc);
            
            if (!hwndCrashedDialog) [[unlikely]]
            {
                [[nodiscard]] const auto logError = [lastError = static_cast<DWORD>(GetLastError())]() noexcept {
                    try {
                        constexpr auto errorCode = 9001;
                        AddReportLog(errorCode, SString("ShowCrashedDialog: CreateDialogW failed with error %u", lastError));
                    }
                    catch (...) {}
                };
                
                [[nodiscard]] constexpr auto setDumpSetting = []() noexcept -> void {
                    try {
                        SetApplicationSetting("diagnostics"sv.data(), "send-dumps"sv.data(), "yes"sv.data());
                    }
                    catch (...) {}
                };
                
                [[nodiscard]] constexpr auto cleanup = []() noexcept -> void {
                    try {
                        ResumeSplash();
                    }
                    catch (...) {}
                    try {
                        MessageBoxA(nullptr, CRASH_MSG_DIALOG_FAIL.data(), CRASH_TITLE_FATAL.data(), 
                                   MB_OK | MB_ICONERROR | MB_TOPMOST);
                    }
                    catch (...) {}
                };
                
                std::invoke(logError);
                std::invoke(setDumpSetting);
                std::invoke(cleanup);
                return "quit";
            }
            try {
                InitDialogStrings(hwndCrashedDialog, g_CrashedDialogItems);
            }
            catch (...) {
            }
            if (auto hwndCrashInfo = GetDlgItem(hwndCrashedDialog, IDC_CRASH_INFO_EDIT); hwndCrashInfo)
            {
                try {
                    if (const bool isEmpty = std::empty(strSanitizedMessage); isEmpty) [[unlikely]]
                    {
                        SetWindowTextW(hwndCrashInfo, DEFAULT_ERROR_MSG.data());
                    }
                    else
                    {
                        if (const auto wszMessage = FromUTF8(strSanitizedMessage); 
                            wszMessage && (wszMessage[0] != L'\0')) [[likely]]
                        {
                            SetWindowTextW(hwndCrashInfo, wszMessage);
                        }
                        else [[unlikely]]
                        {
                            SetWindowTextW(hwndCrashInfo, DEFAULT_ERROR_MSG.data());
                        }
                    }
                }
                catch (...) {
                }
            }
            if (const auto hwndCheckbox = GetDlgItem(hwndCrashedDialog, IDC_SEND_DUMP_CHECK); hwndCheckbox)
            {
                try {
                    const auto settingValue = std::invoke([]() noexcept -> SString { 
                        try {
                            return GetApplicationSetting("diagnostics"sv.data(), "send-dumps"sv.data());
                        }
                        catch (...) {
                            return "yes";
                        }
                    });
                    
                    const std::array<UINT, 2> checkStates = {BST_UNCHECKED, BST_CHECKED};
                    const auto shouldCheck = !std::empty(settingValue) && settingValue != "no"sv.data();
                    SendDlgItemMessage(hwndCrashedDialog, IDC_SEND_DUMP_CHECK, BM_SETCHECK, 
                                      checkStates[shouldCheck], 0);
                }
                catch (...) {
                }
            }
        }
        if (!hwndCrashedDialog) [[unlikely]]
        {
            try {
                SetApplicationSetting("diagnostics"sv.data(), "send-dumps"sv.data(), "yes"sv.data());
            }
            catch (...) {
            }
            return "quit";
        }
        try {
            SetForegroundWindow(hwndCrashedDialog);
            SetWindowPos(hwndCrashedDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
        }
        catch (...) {
        }
        
        const auto dwStartTime = static_cast<DWORD>(GetTickCount32());
        
        [[nodiscard]] constexpr auto handleTimeout = []() noexcept -> void {
            try {
                SetApplicationSetting("diagnostics"sv.data(), "send-dumps"sv.data(), "yes"sv.data());
            }
            catch (...) {
            }
        };
        
        [[nodiscard]] constexpr auto calculateElapsed = [](const DWORD current, const DWORD start) noexcept -> DWORD {
            if constexpr (sizeof(DWORD) == 4) {
                return (current >= start) 
                    ? (current - start) 
                    : (std::numeric_limits<DWORD>::max() - start + current + 1);
            } else {
                return current - start;
            }
        };
        
        for (;;)
        {
            const auto [cancelPressed, okPressed] = std::pair{bCancelPressed.load(std::memory_order_acquire), 
                                                                  bOkPressed.load(std::memory_order_acquire)};
            if (cancelPressed || okPressed)
                break;
            
            if (const auto dwCurrentTime = static_cast<DWORD>(GetTickCount32()); 
                calculateElapsed(dwCurrentTime, dwStartTime) > MAX_WAIT_TIME) [[unlikely]]
            {
                std::invoke(handleTimeout);
                (void)bCancelPressed.exchange(true, std::memory_order_release);
                break;
            }
            constexpr auto maxMessagesPerIteration = 100;
            auto msg = MSG{};
            auto messageCount = decltype(maxMessagesPerIteration){0};
            
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                if (std::exchange(messageCount, messageCount + 1) >= maxMessagesPerIteration) [[unlikely]]
                    break;
                    
                // Block WM_QUIT and other termination messages during crash dialog.
                // The crash dialog MUST only close via explicit user button press (IDOK/IDCANCEL).
                // External quit requests (from CallFunction:Quit, PostQuitMessage, etc.) are suppressed
                // to prevent premature dialog closure before user can make a choice.
                if (msg.message == WM_QUIT || msg.message == WM_CLOSE || msg.message == WM_DESTROY) [[unlikely]]
                {
                    // Silently discard termination messages - do NOT set bCancelPressed
                    // This prevents external quit requests from closing the crash dialog
                    continue;
                }
                
                if (msg.hwnd && !IsWindow(msg.hwnd)) [[unlikely]]
                    continue;
                
                try {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                catch (...) {
                }
            }
            if (!hwndCrashedDialog || !IsWindow(hwndCrashedDialog)) [[unlikely]]
            {
                (void)bCancelPressed.exchange(true, std::memory_order_release);
                break;
            }
            
            {
                constexpr auto sleepDuration = 10;
                try {
                    Sleep(sleepDuration);
                }
                catch (...) {
                    (void)bCancelPressed.exchange(true, std::memory_order_release);
                    break;
                }
            }
        }            
        
        const auto okWasPressed = bOkPressed.load(std::memory_order_acquire);
        return okWasPressed ? "ok" : "quit";
    }
    catch (...)
    {
        try {
            SetApplicationSetting("diagnostics"sv.data(), "send-dumps"sv.data(), "yes"sv.data());
        }
        catch (...) {
        }
        return "quit";
    }
}

void HideCrashedDialog()
{
    if (hwndCrashedDialog)
    {
        try {
            if (IsWindow(hwndCrashedDialog))
                DestroyWindow(hwndCrashedDialog);
        }
        catch (...) {
        }
        (void)std::exchange(hwndCrashedDialog, nullptr);
    }
    
    try {
        ResumeSplash();
    }
    catch (...) {
    }
}

void ShowOOMMessageBox([[maybe_unused]] HINSTANCE hInstance)
{
    static constexpr const char* fallbackMessage = 
        "The crash you experienced is due to memory abuse by servers. Contact server owner or MTA support.";
    static constexpr const char* fallbackTitle = "MTA: San Andreas - Out of Memory Crash";

    const char* message = _("The crash you experienced is due to memory abuse by servers.\n\n"
        "Even with plenty of RAM, this is a x86 game with address space limits up to 3.6GB "
        "and some highly unoptimized servers with a lot of mods (or just a few poorly written scripts) "
        "can make this happen, as well as lag your game.\n\n"
        "Contact the server owner or MTA support in the MTA official discord (https://discord.gg/mtasa) for more information.");
    
    const char* title = _("MTA: San Andreas - Out of Memory Information");

    MessageBoxA(NULL, 
                message ? message : fallbackMessage,
                title ? title : fallbackTitle,
                MB_OK | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
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

    uint RadioButtons[] = {IDC_RADIO1, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4};
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
        CheckRadioButton(hwndOptimusDialog, IDC_RADIO1, IDC_RADIO4, RadioButtons[uiStartupOption]);
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
    SetApplicationSettingInt("nvhacks", "optimus-export-enablement", (uiStartupOption & 2) ? 0 : 1);
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
        const int textSourceItemId = bEnableScaremongering ? IDC_NOAV_OPT_BOTNET : IDC_NOAV_OPT_SKIP;
        {
            wchar_t text[256] = {};
            GetWindowTextW(GetDlgItem(hwndNoAvDialog, textSourceItemId), text, _countof(text));
            SetWindowTextW(GetDlgItem(hwndNoAvDialog, IDC_CHECK_NOT_AGAIN), text);
        }
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
