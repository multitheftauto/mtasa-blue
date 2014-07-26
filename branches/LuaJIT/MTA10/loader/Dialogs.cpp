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

static bool bCancelPressed = false;
static bool bOkPressed = false;
static bool bOtherPressed = false;
static int iOtherCode = 0;
static HWND hwndSplash = NULL;
static HWND hwndProgressDialog = NULL;
static unsigned long ulProgressStartTime = 0;
static HWND hwndCrashedDialog = NULL;
static HWND hwndD3dDllDialog = NULL;
static HWND hwndOptimusDialog = NULL;
static HWND hwndNoAvDialog = NULL;


///////////////////////////////////////////////////////////////////////////
//
// Dialog strings
//
//
///////////////////////////////////////////////////////////////////////////
const char* dialogStringsYes    = _td("Yes");
const char* dialogStringsNo     = _td("No");
const char* dialogStringsOk     = _td("OK");
const char* dialogStringsQuit   = _td("Quit");
const char* dialogStringsHelp   = _td("Help");
const char* dialogStringsCancel = _td("Cancel");

struct SDialogItemInfo {
    int iItemId;
    int iLeadingSpaces;
    const char* szItemText;
};

SDialogItemInfo g_ProgressDialogItems[] = {
            { IDCANCEL,                     0, dialogStringsCancel },
            { -1 },
            };

SDialogItemInfo g_CrashedDialogItems[] = {
            { 0,                            0, _td("MTA: San Andreas has encountered a problem") },
            { IDC_CRASH_HEAD,               0, _td("Crash information") },
            { IDC_SEND_DUMP_CHECK,          0, _td("Tick the check box to send this crash info to MTA devs using the 'internet'") },
            { IDC_SEND_DESC_STATIC,         0, _td("Doing so will increase the chance of this crash being fixed.") },
            { IDC_RESTART_QUESTION_STATIC,  1, _td("Do you want to restart MTA: San Andreas ?") },
            { IDCANCEL,                     0, dialogStringsNo },
            { IDOK,                         0, dialogStringsYes },
            { -1 },
            };

SDialogItemInfo g_D3dDllDialogItems[] = {
            { 0,                            0, _td("MTA: San Andreas - Warning") },
            { IDC_D3DDLL_TEXT1,             0, _td("Your Grand Theft Auto: San Andreas install directory contains a d3d9.dll file:") },
            { IDC_D3DDLL_TEXT2,             0, _td("The file is not required and may interfere with the graphical features in this version of MTA:SA.\n\n"
                                                   "It is recommended that you remove or rename d3d9.dll") },
            { IDC_CHECK_NOT_AGAIN,          1, _td("Do not tell me about this d3d9.dll again") },
            { IDC_BUTTON_SHOW_DIR,          0, _td("Show me the file") },
            { IDOK,                         0, _td("Play MTA:SA") },
            { IDCANCEL,                     0, dialogStringsQuit },
            { -1 },
            };


SDialogItemInfo g_OptimusDialogItems[] = {
            { 0,                            0, _td("MTA: San Andreas - Confusing options") },
            { IDC_OPTIMUS_TEXT1,            0, _td("NVidia Optimus detected!") },
            { IDC_OPTIMUS_TEXT2,            0, _td("Try each option and see what works:") },
            { IDC_RADIO1,                   1, _td("A - Standard NVidia") },
            { IDC_RADIO2,                   1, _td("B - Alternate NVidia") },
            { IDC_RADIO3,                   1, _td("C - Standard NVidia with exe rename") },
            { IDC_RADIO4,                   1, _td("D - Alternate NVidia with exe rename") },
            { IDC_RADIO5,                   1, _td("E - Standard Intel") },
            { IDC_RADIO6,                   1, _td("F - Alternate Intel") },
            { IDC_RADIO7,                   1, _td("G - Standard Intel with exe rename") },
            { IDC_RADIO8,                   1, _td("H - Alternate Intel with exe rename") },
            { IDC_OPTIMUS_TEXT3,            0, _td("If you get desperate, this might help:") },
            { IDC_CHECK_FORCE_WINDOWED,     1, _td("Force windowed mode") },
            { IDC_BUTTON_HELP,              0, dialogStringsHelp },
            { IDOK,                         0, dialogStringsOk },
            { -1 },
            };

SDialogItemInfo g_NoAvDialogItems[] = {
            { 0,                            0, _td("MTA: San Andreas") },
            { IDC_NOAV_TEXT1,               0, _td("Warning: Could not detect anti-virus product") },
            { IDC_NOAV_TEXT2,               0, _td("MTA could not detect an anti-virus on your PC.\n\n"
                                                   "Viruses interfere with MTA and degrade your gameplay experience.\n\n"
                                                   "Press 'Help' for more information.") },
            { IDC_NOAV_OPT_SKIP,            0, _td("I have already installed an anti-virus") },
            { IDC_NOAV_OPT_BOTNET,          0, _td("I will not install an anti-virus.\n"
                                                   "I want my PC to lag and be part of a botnet.") },
            { IDC_BUTTON_HELP,              0, dialogStringsHelp },
            { IDOK,                         0, dialogStringsOk },
            { -1 },
            };


///////////////////////////////////////////////////////////////////////////
//
// InitDialogStrings
//
// Copy strings into dialog
//
///////////////////////////////////////////////////////////////////////////
void InitDialogStrings( HWND hwndDialog, const SDialogItemInfo* dialogItems )
{
    for ( uint i = 0 ; true ; i++ )
    {
        const SDialogItemInfo& item = dialogItems[i];
        if ( item.iItemId == -1 )
            return;

        HWND hwndItem;
        if ( item.iItemId == 0 )
            hwndItem = hwndDialog;
        else
            hwndItem = GetDlgItem( hwndDialog, item.iItemId );

        if ( hwndItem )
        {
            SString strItemText = PadLeft( item.szItemText, item.iLeadingSpaces, ' ' );
#if MTA_DEBUG
            char szPrevText[200] = "";
            GetWindowText( hwndItem, szPrevText, NUMELMS( szPrevText ) );
            if ( !strItemText.EndsWith( szPrevText ) && !SStringX( szPrevText ).EndsWith( strItemText ) )
            {
                OutputDebugLine( SString( "Possible text mismatch for dialog item (idx:%d id:%d) '%s' (orig:'%s')", i, item.iItemId, item.szItemText, szPrevText ) );
            }
#endif
            SetWindowText( hwndItem, _(strItemText) );
        }
        else
            OutputDebugLine( SString( "No dialog item for (idx:%d id:%d) '%s' ", i, item.iItemId, item.szItemText ) );
    }
}


///////////////////////////////////////////////////////////////////////////
//
// DialogProc
//
// Generic callback for all our dialogs
//
///////////////////////////////////////////////////////////////////////////
int CALLBACK DialogProc ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
                    if ( iOtherCode && iOtherCode == LOWORD(wParam) )
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
void ShowSplash ( HINSTANCE hInstance )
{
#ifndef MTA_DEBUG
    if ( !hwndSplash )
    {
        hwndSplash = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DialogProc );

        HWND hBitmap = GetDlgItem ( hwndSplash, IDC_SPLASHBITMAP );
        RECT splashRect;
        GetWindowRect ( hBitmap, &splashRect );
        int iScreenWidth = GetSystemMetrics ( SM_CXSCREEN );
        int iScreenHeight = GetSystemMetrics ( SM_CYSCREEN );
        int iWindowWidth = splashRect.right - splashRect.left;
        int iWindowHeight = splashRect.bottom - splashRect.top;

        // Adjust and center the window (to be DPI-aware)
        SetWindowPos ( hwndSplash, NULL, (iScreenWidth-iWindowWidth)/2, (iScreenHeight-iWindowHeight)/2, iWindowWidth, iWindowHeight, 0 );
    }
    SetForegroundWindow ( hwndSplash );
    SetWindowPos ( hwndSplash, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
#endif
}


//
// Hide splash dialog
//
void HideSplash ( void )
{
    if ( hwndSplash )
    {
        DestroyWindow ( hwndSplash );
        hwndSplash = NULL;
    }
}

//
// Hide splash dialog temporarily
//
void SuspendSplash ( void )
{
    if ( hwndSplash )
    {
        ShowWindow( hwndSplash, SW_HIDE );
    }
}

//
// Show splash dialog if was previously suspended
//
void ResumeSplash ( void )
{
    if ( hwndSplash )
    {
        HideSplash();
        ShowSplash( g_hInstance );
    }
}


///////////////////////////////////////////////////////////////
//
// Progress dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowProgressDialog( HINSTANCE hInstance, const SString& strTitle, bool bAllowCancel )
{
    if ( !hwndProgressDialog )
    {
        HideSplash ();
        bCancelPressed = false;
        hwndProgressDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_PROGRESS_DIALOG), 0, DialogProc );
        dassert( ( GetWindowLong( hwndProgressDialog, GWL_STYLE ) & WS_VISIBLE ) == 0 );    // Should be Visible: False
        InitDialogStrings( hwndProgressDialog, g_ProgressDialogItems );
        SetWindowText ( hwndProgressDialog, strTitle );
        ShowWindow( GetDlgItem( hwndProgressDialog, IDCANCEL ), bAllowCancel ? SW_SHOW : SW_HIDE );
        ulProgressStartTime = GetTickCount32 ();
    }
    SetForegroundWindow ( hwndProgressDialog );
    SetWindowPos ( hwndProgressDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );
}

void HideProgressDialog ( void )
{
    if ( hwndProgressDialog )
    {
        // Show progress for at least two seconds
        unsigned long ulTimeElapsed = GetTickCount32 () - ulProgressStartTime;
        if ( ulTimeElapsed < 2000 )
        {
            UpdateProgress( 100, 100 );
            Sleep ( 2000 - ulTimeElapsed );
        }

        DestroyWindow ( hwndProgressDialog );
        hwndProgressDialog = NULL;
    }
}


// returns true if canceled
bool UpdateProgress( int iPos, int iMax, const SString& strMsg )
{
    if ( hwndProgressDialog)
    {
        HWND hwndText = GetDlgItem( hwndProgressDialog, IDC_PROGRESS_STATIC );
        char buffer[ 1024 ] = "";
        GetWindowText ( hwndText, buffer, NUMELMS(buffer) );
        if ( strMsg.length () > 0 && strMsg != buffer )
            SetWindowText ( hwndText, strMsg );
        HWND hwndBar = GetDlgItem( hwndProgressDialog, IDC_PROGRESS_BAR );
        PostMessage(hwndBar, PBM_SETPOS, iPos * 100 / Max ( 1, iMax ), 0 );
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
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
void StartPseudoProgress( HINSTANCE hInstance, const SString& strTitle, const SString& strMsg )
{
    ShowProgressDialog ( hInstance, strTitle );
    UpdateProgress( 10, 100, strMsg );
    Sleep ( 100 );
    UpdateProgress( 30, 100 );
}

void StopPseudoProgress( void )
{
    if ( hwndProgressDialog )
    {
        UpdateProgress( 60, 100 );
        Sleep ( 100 );
        UpdateProgress( 90, 100 );
        Sleep ( 100 );
        HideProgressDialog ();
    }
}


///////////////////////////////////////////////////////////////
//
// Crashed dialog
//
//
//
///////////////////////////////////////////////////////////////
SString ShowCrashedDialog( HINSTANCE hInstance, const SString& strMessage )
{
    if ( !hwndCrashedDialog )
    {
        SuspendSplash ();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = 0;
        hwndCrashedDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_CRASHED_DIALOG), 0, DialogProc );
        dassert( ( GetWindowLong( hwndCrashedDialog, GWL_STYLE ) & WS_VISIBLE ) == 0 );    // Should be Visible: False
        InitDialogStrings( hwndCrashedDialog, g_CrashedDialogItems );
        SetWindowText ( GetDlgItem( hwndCrashedDialog, IDC_CRASH_INFO_EDIT ), strMessage );
        SendDlgItemMessage( hwndCrashedDialog, IDC_SEND_DUMP_CHECK, BM_SETCHECK, GetApplicationSetting ( "diagnostics", "send-dumps" ) != "no" ? BST_CHECKED : BST_UNCHECKED, 0 );
    }
    SetForegroundWindow ( hwndCrashedDialog );
    SetWindowPos ( hwndCrashedDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );

    while ( !bCancelPressed && !bOkPressed )
    {
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep( 10 );
    }

    LRESULT res = SendMessageA( GetDlgItem( hwndCrashedDialog, IDC_SEND_DUMP_CHECK ), BM_GETCHECK, 0, 0 );
    SetApplicationSetting ( "diagnostics", "send-dumps", res ? "yes" : "no" );

    if ( bCancelPressed )
        return "quit";

    ResumeSplash();

    //if ( bOkPressed )
        return "ok";
}


void HideCrashedDialog ( void )
{
    if ( hwndCrashedDialog )
    {
        DestroyWindow ( hwndCrashedDialog );
        hwndCrashedDialog = NULL;
    }
}


///////////////////////////////////////////////////////////////
//
// d3d dll dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowD3dDllDialog( HINSTANCE hInstance, const SString& strPath )
{
    // Calc hash of target file
    SString strFileHash;
    MD5 md5;
    CMD5Hasher Hasher;
    if ( Hasher.Calculate ( strPath, md5 ) )
    {
        char szHashResult[33];
        Hasher.ConvertToHex ( md5, szHashResult );
        strFileHash = szHashResult;
    }

	// Maybe skip dialog
    if ( GetApplicationSetting ( "diagnostics", "d3d9-dll-last-hash" ) == strFileHash )
    {
        if ( GetApplicationSetting ( "diagnostics", "d3d9-dll-not-again" ) == "yes" )
            return;
    }

	// Create and show dialog
    if ( !hwndD3dDllDialog )
    {
        SuspendSplash ();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = IDC_BUTTON_SHOW_DIR;
        hwndD3dDllDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_D3DDLL_DIALOG), 0, DialogProc );
        dassert( ( GetWindowLong( hwndD3dDllDialog, GWL_STYLE ) & WS_VISIBLE ) == 0 );    // Should be Visible: False
        InitDialogStrings( hwndD3dDllDialog, g_D3dDllDialogItems );
        SetWindowText ( GetDlgItem( hwndD3dDllDialog, IDC_EDIT_D3DDLL_PATH ), strPath );
    }
    SetForegroundWindow ( hwndD3dDllDialog );
    SetWindowPos ( hwndD3dDllDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );

	// Wait for input
    while ( !bCancelPressed && !bOkPressed && !bOtherPressed )
    {
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep( 10 );
    }

	// Process input
    LRESULT res = SendMessageA( GetDlgItem( hwndD3dDllDialog, IDC_CHECK_NOT_AGAIN ), BM_GETCHECK, 0, 0 );
    SetApplicationSetting ( "diagnostics", "d3d9-dll-last-hash", strFileHash );
    SetApplicationSetting ( "diagnostics", "d3d9-dll-not-again", res ? "yes" : "no" );

    if ( bCancelPressed )
    {
        ExitProcess(0);
    }
    if ( bOtherPressed )
    {
        if ( ITEMIDLIST *pidl = ILCreateFromPathW ( FromUTF8( strPath ) ) )
        {
            SHOpenFolderAndSelectItems ( pidl, 0, 0, 0 );
            ILFree ( pidl );
        }
        else
            ShellExecuteNonBlocking ( "open", ExtractPath ( strPath ) );

        ExitProcess(0);
    }
    ResumeSplash();
}


void HideD3dDllDialog ( void )
{
    if ( hwndD3dDllDialog )
    {
        DestroyWindow ( hwndD3dDllDialog );
        hwndD3dDllDialog = NULL;
    }
}


///////////////////////////////////////////////////////////////
//
// Optimus dialog
//
//
//
///////////////////////////////////////////////////////////////
void ShowOptimusDialog( HINSTANCE hInstance )
{
    if ( GetApplicationSettingInt( "nvhacks", "optimus-dialog-skip" ) )
    {
        SetApplicationSettingInt( "nvhacks", "optimus-dialog-skip", 0 );
        return;
    }

    uint RadioButtons[] = { IDC_RADIO1, IDC_RADIO2, IDC_RADIO3, IDC_RADIO4, IDC_RADIO5, IDC_RADIO6, IDC_RADIO7, IDC_RADIO8 };
	// Create and show dialog
    if ( !hwndOptimusDialog )
    {
        SuspendSplash ();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = IDC_BUTTON_HELP;
        hwndOptimusDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_OPTIMUS_DIALOG), 0, DialogProc );
        dassert( ( GetWindowLong( hwndOptimusDialog, GWL_STYLE ) & WS_VISIBLE ) == 0 );    // Should be Visible: False
        InitDialogStrings( hwndOptimusDialog, g_OptimusDialogItems );
        uint uiStartupOption = GetApplicationSettingInt( "nvhacks", "optimus-startup-option" ) % NUMELMS( RadioButtons );
        uint uiForceWindowed = GetApplicationSettingInt( "nvhacks", "optimus-force-windowed" );
        CheckRadioButton( hwndOptimusDialog, IDC_RADIO1, IDC_RADIO8, RadioButtons[ uiStartupOption ] );
        CheckDlgButton( hwndOptimusDialog, IDC_CHECK_FORCE_WINDOWED, uiForceWindowed );
    }
    SetForegroundWindow ( hwndOptimusDialog );
    SetWindowPos ( hwndOptimusDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW );

	// Wait for input
    while ( !bCancelPressed && !bOkPressed && !bOtherPressed )
    {
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep( 10 );
    }

    if ( bOtherPressed )
    {
        BrowseToSolution ( "optimus-startup-option-help", TERMINATE_PROCESS );
    }

	// Process input
    uint uiStartupOption = 0;
    for ( ; uiStartupOption < NUMELMS( RadioButtons ) - 1 ; uiStartupOption++ )
    {
        if ( IsDlgButtonChecked( hwndOptimusDialog, RadioButtons[ uiStartupOption ] ) == BST_CHECKED )
            break;
    }
    uint uiForceWindowed = ( IsDlgButtonChecked( hwndOptimusDialog, IDC_CHECK_FORCE_WINDOWED ) == BST_CHECKED ) ? 1 : 0;

    SetApplicationSettingInt( "nvhacks", "optimus-startup-option", uiStartupOption );
    SetApplicationSettingInt( "nvhacks", "optimus-alt-startup", ( uiStartupOption & 1 ) ? 1 : 0 );
    SetApplicationSettingInt( "nvhacks", "optimus-rename-exe", ( uiStartupOption & 2 ) ? 1 : 0 );
    SetApplicationSettingInt( "nvhacks", "optimus-export-enablement", ( uiStartupOption & 4 ) ? 0 : 1 );
    SetApplicationSettingInt( "nvhacks", "optimus-force-windowed", uiForceWindowed );

    if ( !GetInstallManager()->UpdateOptimusSymbolExport() )
    {
        // Restart required to apply change because of permissions
        SetApplicationSettingInt( "nvhacks", "optimus-dialog-skip", 1 );
        ShellExecuteNonBlocking( "open", PathJoin ( GetMTASAPath (), MTA_EXE_NAME ) );
        TerminateProcess( GetCurrentProcess(), 0 );
    }
    ResumeSplash();
}


void HideOptimusDialog ( void )
{
    if ( hwndOptimusDialog )
    {
        DestroyWindow ( hwndOptimusDialog );
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
void ShowNoAvDialog( HINSTANCE hInstance, bool bEnableScaremongering )
{
    uint uiTimeLastAsked = GetApplicationSettingInt( "noav-last-asked-time" );
    bool bUserSaysNo = GetApplicationSettingInt( "noav-user-says-skip" ) != 0;

    // Don't display dialog on first run
    if ( uiTimeLastAsked == 0 )
    {
        SetApplicationSettingInt( "noav-last-asked-time", 1 );
        return;
    }

    // Time to ask again?
    uint uiAskHoursInterval;
    if ( !bUserSaysNo )
        uiAskHoursInterval = 1;        // Once an hour if box not ticked
    else
    {
        if ( bEnableScaremongering )
            uiAskHoursInterval = 24 * 7;            // Once a week if ticked
        else
            uiAskHoursInterval = 24 * 365 * 1000;   // Once every 1000 years if ticked and WSC not monitoring
    }

    uint uiTimeNow = static_cast < uint >( time( NULL ) / 3600LL );
    uint uiHoursSinceLastAsked = uiTimeNow - uiTimeLastAsked;
    if ( uiHoursSinceLastAsked < uiAskHoursInterval )
        return;

	// Create and show dialog
    if ( !hwndNoAvDialog )
    {
        SuspendSplash ();
        bCancelPressed = false;
        bOkPressed = false;
        bOtherPressed = false;
        iOtherCode = IDC_BUTTON_HELP;
        hwndNoAvDialog = CreateDialog ( hInstance, MAKEINTRESOURCE(IDD_NOAV_DIALOG), 0, DialogProc );
        dassert( ( GetWindowLong( hwndNoAvDialog, GWL_STYLE ) & WS_VISIBLE ) == 0 );    // Should be Visible: False
        InitDialogStrings( hwndNoAvDialog, g_NoAvDialogItems );
        ShowWindow( GetDlgItem( hwndNoAvDialog, IDC_NOAV_OPT_SKIP ), bEnableScaremongering ? SW_HIDE : SW_SHOW );
        ShowWindow( GetDlgItem( hwndNoAvDialog, IDC_NOAV_OPT_BOTNET ), bEnableScaremongering ? SW_SHOW : SW_HIDE );
    }
    ShowWindow( hwndNoAvDialog, SW_SHOW );  // Show after all changes are complete
    SetForegroundWindow ( hwndNoAvDialog );
    SetWindowPos ( hwndNoAvDialog, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	// Wait for input
    while ( !bCancelPressed && !bOkPressed && !bOtherPressed )
    {
        MSG msg;
        while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
        {
            if( GetMessage( &msg, NULL, 0, 0 ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        Sleep( 10 );
    }

	// Process input
    if ( bOtherPressed )
    {
        ShowWindow( hwndNoAvDialog, SW_HIDE );
        BrowseToSolution ( "no-anti-virus", TERMINATE_PROCESS );
    }

    LRESULT res = SendMessageA( GetDlgItem( hwndNoAvDialog, IDC_CHECK_NOT_AGAIN ), BM_GETCHECK, 0, 0 );
    SetApplicationSettingInt( "noav-last-asked-time", uiTimeNow );
    SetApplicationSettingInt( "noav-user-says-skip", res ? 1 : 0 );

    ResumeSplash();
}


void HideNoAvDialog ( void )
{
    if ( hwndNoAvDialog )
    {
        DestroyWindow ( hwndNoAvDialog );
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
void TestDialogs( void )
{
#if 0
    ShowProgressDialog( g_hInstance, "test1" );
    for ( uint i = 0 ; i < 100 ; i++ )
    {
        UpdateProgress( i, 100, SString( "%d", i ) );
        Sleep( 10 );
    }
    HideProgressDialog();
#endif

#if 0
    ShowCrashedDialog( g_hInstance, "test2" );
    HideCrashedDialog();
#endif

#if 0
    SetApplicationSetting ( "diagnostics", "d3d9-dll-last-hash", "123" );
    ShowD3dDllDialog( g_hInstance, "c:\\dummy path\\" );
    HideD3dDllDialog();
#endif

#if 0
    ShowOptimusDialog( g_hInstance );
    HideOptimusDialog();
#endif

#if 0
    // Friendly option
    SetApplicationSettingInt( "noav-last-asked-time", 1 );
    SetApplicationSettingInt( "noav-user-says-skip", 0 );
    ShowNoAvDialog( g_hInstance, true );
    HideNoAvDialog();
#endif

#if 0
    // Scaremongering option option
    SetApplicationSettingInt( "noav-last-asked-time", 1 );
    SetApplicationSettingInt( "noav-user-says-skip", 0 );
    ShowNoAvDialog( g_hInstance, false );
    HideNoAvDialog();
#endif
}
#endif
