/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMessageLoopHook.cpp
*  PURPOSE:     Windows message loop hooking
*  DEVELOPERS:  Christian Myhre Lundheim <>
*               Derek Abdine <>
*               Jax <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

extern CCore* g_pCore;

template<> CMessageLoopHook * CSingleton< CMessageLoopHook >::m_pSingleton = NULL;

WPARAM  CMessageLoopHook::m_LastVirtualKeyCode = NULL;
UCHAR   CMessageLoopHook::m_LastScanCode = NULL;
BYTE*   CMessageLoopHook::m_LastKeyboardState = new BYTE[256];
bool    ms_bIgnoreNextEscapeCharacter = false;

CMessageLoopHook::CMessageLoopHook ( )
{
    WriteDebugEvent ( "CMessageLoopHook::CMessageLoopHook" );
    m_HookedWindowProc      = NULL;
    m_HookedWindowHandle    = NULL;
    m_bRefreshMsgQueueEnabled = true;
    m_MovementDummyWindow   = NULL;
}


CMessageLoopHook::~CMessageLoopHook ( )
{
    WriteDebugEvent ( "CMessageLoopHook::~CMessageLoopHook" );
    m_HookedWindowProc      = NULL;
    m_HookedWindowHandle    = NULL;
}


void CMessageLoopHook::ApplyHook ( HWND hFocusWindow )
{
    if ( m_HookedWindowHandle == NULL && m_HookedWindowProc == NULL )
    {
        // Save the hooked window handle.
        m_HookedWindowHandle = hFocusWindow;

        // Subclass the window procedure.
        m_HookedWindowProc = SubclassWindow ( hFocusWindow, ProcessMessage );

        // Enable Unicode (UTF-16) characters in WM_CHAR messages
        SetWindowLongW ( hFocusWindow, GWL_WNDPROC, GetWindowLong ( hFocusWindow, GWL_WNDPROC ) );


        // Register window class for dummy movement window
        WNDCLASSEX wcDummy;
        wcDummy.cbSize        = sizeof(WNDCLASSEX);
        wcDummy.style         = 0;
        wcDummy.lpfnWndProc   = ProcessDummyWindowMessage;
        wcDummy.cbClsExtra    = 0;
        wcDummy.cbWndExtra    = 0;
        wcDummy.hInstance     = GetModuleHandle(NULL);
        wcDummy.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        wcDummy.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wcDummy.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wcDummy.lpszMenuName  = NULL;
        wcDummy.lpszClassName = "MovementDummy";
        wcDummy.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
        RegisterClassEx(&wcDummy);
    }
}


void CMessageLoopHook::RemoveHook ( )
{
    if ( m_HookedWindowProc != NULL && m_HookedWindowHandle != NULL )
    {
        // Restore the hooked window procedure.
        SubclassWindow ( m_HookedWindowHandle, m_HookedWindowProc );

        // Reset the window handle and procedure variables.
        m_HookedWindowProc      = NULL;
        m_HookedWindowHandle    = NULL;
    }
}


// Disable RefreshMsgQueue when calls inside CMessageLoopHook::ProcessMessage could cause trouble
void CMessageLoopHook::SetRefreshMsgQueueEnabled( bool bEnable )
{
    m_bRefreshMsgQueueEnabled = bEnable;
}


// Process one message every 5 seconds during busy periods to stop Windows 'Not responding' stuff
void CMessageLoopHook::MaybeRefreshMsgQueue( void )
{
    if ( m_ProcessMessageTimer.Get() < 5000 )
        return;
    m_ProcessMessageTimer.Reset();

    if ( !m_bRefreshMsgQueueEnabled )
        return;

    MSG msg;
    if ( PeekMessage ( &msg, NULL, 0, 0,PM_NOREMOVE ) )
    	if ( msg.message == WM_QUIT
    	  || msg.message == WM_CLOSE
    	  || msg.message == WM_DESTROY
    	  || msg.message == WM_NCDESTROY
    	  || msg.message == WM_HSCROLL
    	  || msg.message == WM_VSCROLL
    	  ) 
    	  return; 
    if ( PeekMessage( &msg, NULL, 0, 0,PM_REMOVE ) )
    {
    	TranslateMessage( &msg );
    	DispatchMessage( &msg );
    }
}

LRESULT CALLBACK CMessageLoopHook::ProcessMessage ( HWND hwnd, 
                                                    UINT uMsg, 
                                                    WPARAM wParam, 
                                                    LPARAM lParam )
{
    CMessageLoopHook *  pThis;

    // Get a pointer to ourself.
    pThis = CMessageLoopHook::GetSingletonPtr ( );

    if ( pThis )
        pThis->m_ProcessMessageTimer.Reset();

    // Alternate alt-tab system
    if ( pThis && hwnd == pThis->GetHookedWindowHandle () )
    {
        if ( uMsg == WM_ACTIVATE && LOWORD(wParam) == WA_ACTIVE )
        {
            GetVideoModeManager()->OnGainFocus();
        }
        if ( uMsg == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE )
        {
            GetVideoModeManager()->OnLoseFocus();
        }
        if ( uMsg == WM_PAINT )
        {
            GetVideoModeManager()->OnPaint();
        }
    }

    // Log our state
    if ( uMsg == WM_KILLFOCUS || (uMsg == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE) )
    {
        CSetCursorPosHook::GetSingleton ().DisableSetCursorPos ();
    }
    else if ( uMsg == WM_SETFOCUS || (uMsg == WM_ACTIVATE && LOWORD(wParam) != WA_INACTIVE) )
    {
        if ( !g_pCore->GetLocalGUI ()->InputGoesToGUI () )
            CSetCursorPosHook::GetSingleton ().EnableSetCursorPos ();
    }

    // Prevent GTA from knowing about kill focuses. Prevents pausing.
    if ( uMsg == WM_KILLFOCUS || (uMsg == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE) )
    {
        return true;
    }

    // Disable the system context menu by clicking in the process icon or pressing ALT+SPACE.
    if ( uMsg == WM_SYSCOMMAND )
    {
        if ( wParam == 0xF093 || wParam == SC_KEYMENU || wParam == SC_MOUSEMENU )
            return 0;
    }

    // Quit message?
    if ( uMsg == WM_CLOSE )
    {
        g_pCore->Quit ();
    }

    if ( uMsg == WM_COPYDATA )
    {
        PCOPYDATASTRUCT pCDS = (PCOPYDATASTRUCT) lParam;

        if ( pCDS->dwData == URI_CONNECT )
        {
            LPSTR szConnectInfo      = (LPSTR) pCDS->lpData;
            CCommandFuncs::Connect ( szConnectInfo );
        }
    }

    // Make sure our pointers are valid.
    if ( pThis != NULL && hwnd == pThis->GetHookedWindowHandle () && g_pCore->AreModulesLoaded() )
    {
        g_pCore->UpdateIsWindowMinimized ();  // Force update of stuff

        if ( uMsg == WM_TIMER && wParam == IDT_TIMER1 )
            g_pCore->WindowsTimerHandler();     // Used for 'minimized before first game' pulses

        // Handle IME if input is not for the GUI
        if ( !g_pCore->GetLocalGUI ()->InputGoesToGUI () )
        {
            if ( uMsg == WM_KEYDOWN )
            {
                // Recover virtual key
                if ( wParam == VK_PROCESSKEY )
                    wParam = MapVirtualKey ( lParam >> 16, MAPVK_VSC_TO_VK_EX );
            }

            if ( uMsg == WM_IME_STARTCOMPOSITION || uMsg == WM_IME_ENDCOMPOSITION || uMsg == WM_IME_COMPOSITION )
            {
                // Cancel, stop, block and ignore
                HIMC himc = ImmGetContext ( hwnd );
                ImmNotifyIME ( himc, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
                ImmReleaseContext ( hwnd, himc );
                return true;
            }
        }

        // Pass escape keyup to onClientKey
        if ( uMsg == WM_KEYUP && wParam == VK_ESCAPE )
        {
            g_pCore->GetKeyBinds()->TriggerKeyStrokeHandler ( "escape", uMsg == WM_KEYDOWN, true );
            return true;
        }

        // Suppress auto repeat of escape and console toggle keys
        if ( ( uMsg == WM_KEYDOWN || uMsg == WM_CHAR ) && ( wParam == VK_ESCAPE || wParam == VK_F8 || wParam == '`' ) )
        {
            bool bFirstHit = ( lParam & 0x40000000 ) ? false:true;
            if ( !bFirstHit )
                return true;
        }

        // Slightly hacky way of suppressing escape character when console is closed with escape key
        if ( uMsg == WM_CHAR && wParam == VK_ESCAPE )
        {
            bool bTemp = ms_bIgnoreNextEscapeCharacter;
            ms_bIgnoreNextEscapeCharacter = false;
            if ( bTemp )
                return true;
        }

        if ( CKeyBinds::IsFakeCtrl_L ( uMsg, wParam, lParam ) )
            return true;

        // See if this is message was caused by our asynchronous sockets
        if ( uMsg >= WM_ASYNCTRAP && uMsg <= ( WM_ASYNCTRAP + 511 ))
        {
            /* ACHTUNG: uMsg - 10? Windows seems to add 10 or there's a bug in the message code. Hack! */
            // Let the CTCPManager handle it
            CTCPManager::GetSingletonPtr ()->HandleEvent ( ( uMsg - WM_ASYNCTRAP ), wParam, lParam );
        }

        bool bWasCaptureKey = false;
        CMainMenu* pMainMenu = g_pCore->GetLocalGUI ()->GetMainMenu ();
        if ( pMainMenu )
        {
            CSettings* pSettings = pMainMenu->GetSettingsWindow ();
            if ( pSettings )
            {
                if ( uMsg == WM_KEYDOWN && wParam == VK_ESCAPE && GetJoystickManager ()->IsCapturingAxis () )
                {
                    GetJoystickManager ()->CancelCaptureAxis ( true );
                    return true;
                }
                bWasCaptureKey = ( pSettings->IsCapturingKey () && pSettings->ProcessMessage ( uMsg, wParam, lParam ) );
                if ( !bWasCaptureKey )
                {
                    // If Escape is pressed and we're playing ingame, we show/hide the menu
                    if ( uMsg == WM_KEYDOWN && wParam == VK_ESCAPE && g_pCore->IsConnected () )
                    {
                        // Hide the console
                        CConsoleInterface* pConsole = g_pCore->GetConsole ();
                        if ( pConsole->IsVisible () )
                        {
                            ms_bIgnoreNextEscapeCharacter = true;
                            pConsole->SetVisible ( false );
                            return true;
                        }

                        // The mainmenu makes sure it isn't hidden if UseIngameButtons == false
                        if ( !CCore::GetSingleton().IsOfflineMod () )
                        {
                            if ( g_pCore->GetKeyBinds()->TriggerKeyStrokeHandler ( "escape", uMsg == WM_KEYDOWN, true ) )
                            {
                                // Stop chat input
                                if ( CLocalGUI::GetSingleton ().IsChatBoxInputEnabled () )
                                {
                                    CLocalGUI::GetSingleton ().SetChatBoxInputEnabled ( false );
                                    return true;
                                }

                                CLocalGUI::GetSingleton ().SetMainMenuVisible ( !CLocalGUI::GetSingleton ().IsMainMenuVisible () );
                            }
                            return true;
                        }
                    }
                    else
                    if ( uMsg == WM_KEYDOWN && wParam == VK_ESCAPE && !g_pCore->IsConnected () )
                    {
                        // If Escape is pressed and we're not playing ingame, hide certain windows
                        CLocalGUI::GetSingleton ().GetMainMenu ()->OnEscapePressedOffLine ();
                    }

                    // If F8 is pressed, we show/hide the console
                    if ( ( uMsg == WM_KEYDOWN && wParam == VK_F8 ) || ( uMsg == WM_CHAR && wParam == '`' ) )
                    {
                        eSystemState systemState = g_pCore->GetGame ()->GetSystemState ();
                        if ( CLocalGUI::GetSingleton ().IsConsoleVisible () || systemState == 7 || systemState == 8 || systemState == 9 ) /* GS_FRONTEND, GS_INIT_PLAYING_GAME, GS_PLAYING_GAME */
                        {
                            CLocalGUI::GetSingleton ().SetConsoleVisible ( !CLocalGUI::GetSingleton ().IsConsoleVisible () );           
                        }
                        return true;
                    }

                    // If the console is accepting input, and we pressed down/up, scroll the console history
                    //                          or if we pressed tab, step through possible autocomplete matches
                    if ( CLocalGUI::GetSingleton ().GetConsole()->IsInputActive() )
                    {
                        if ( uMsg == WM_KEYDOWN )
                        {
                            if ( wParam == VK_DOWN )
                            {
                                CLocalGUI::GetSingleton ().GetConsole ()->SetPreviousHistoryText ();
                            }

                            if ( wParam == VK_UP )
                            {
                                CLocalGUI::GetSingleton ().GetConsole ()->SetNextHistoryText ();
                            }

                            if ( wParam == VK_TAB )
                            {
                                CLocalGUI::GetSingleton ().GetConsole ()->SetNextAutoCompleteMatch ();
                            }
                            else
                            {
                                CLocalGUI::GetSingleton ().GetConsole ()->ResetAutoCompleteMatch ();
                            }
                        }
                    }
                    else if ( uMsg == WM_KEYDOWN && CLocalGUI::GetSingleton().GetMainMenu()->GetServerBrowser()->IsAddressBarAwaitingInput() )
                    {
                        if ( wParam == VK_DOWN )
                        {
                            CLocalGUI::GetSingleton().GetMainMenu()->GetServerBrowser()->SetNextHistoryText ( true );
                        }

                        if ( wParam == VK_UP )
                        {
                            CLocalGUI::GetSingleton().GetMainMenu()->GetServerBrowser()->SetNextHistoryText ( false );
                        }

                    }
                }
            }
        }

        if ( !bWasCaptureKey )
        {
            // Store our keydown for backup unicode translation
            if ( uMsg == WM_KEYDOWN )
            {
                m_LastVirtualKeyCode = wParam;
                m_LastScanCode = (BYTE)((lParam >> 16) & 0x000F);
                GetKeyboardState( m_LastKeyboardState );
            }
            // If it was a question mark character, we may have an unprocessed unicode character
            if ( uMsg == WM_CHAR && wParam == 0x3F )
            {
                wchar_t* wcsUnicode = new wchar_t[1];
                ToUnicodeEx ( m_LastVirtualKeyCode, m_LastScanCode, m_LastKeyboardState, wcsUnicode, 1, 0, GetKeyboardLayout(0) );
                wParam = (WPARAM)wcsUnicode[0];
                delete wcsUnicode;
            }

            // Lead the message through the keybinds message processor
            g_pCore->GetKeyBinds ()->ProcessMessage ( hwnd, uMsg, wParam, lParam );

            bool bProcessed = false, bClientProcessed = false;          

            // Check and see if the GUI should process this message
            bProcessed = CLocalGUI::GetSingleton ().ProcessMessage ( hwnd, uMsg, wParam, lParam );
            
            // Check and see if the Core/mod should process this message
            if ( !CCore::GetSingleton ().GetGame ()->IsAtMenu() )
            {            
                pfnProcessMessage pfnClientMessageProcessor = CCore::GetSingleton ().GetClientMessageProcessor();
                if ( pfnClientMessageProcessor )
                {
                    bClientProcessed = pfnClientMessageProcessor ( hwnd, uMsg, wParam, lParam );
                }
            }

            // If GTA can process this key
            if ( !bProcessed && !bClientProcessed )
            {
                // ALWAYS return true on escape to stop us getting to GTA's menu
                if ( uMsg == WM_KEYDOWN && wParam == VK_ESCAPE )
                {
                    return true; 
                }

                // Prevent game window auto-minimizing if full screen and:
                //     1. More than one monitor present
                // and 2. Minimizing option disabled
                // or
                //     1. Starting up (Main menu has not been displayed yet)
                if ( uMsg == WM_ACTIVATE ||
                    uMsg == WM_ACTIVATEAPP ||
                    uMsg == WM_NCACTIVATE ||
                    uMsg == WM_SETFOCUS ||
                    uMsg == WM_KILLFOCUS )
                {
                    if ( !GetVideoModeManager ()->IsWindowed () )
                    {
                        if ( !CLocalGUI::GetSingleton ().GetMainMenu () || !CLocalGUI::GetSingleton ().GetMainMenu ()->HasStarted () )
                            return true;    // No auto-minimize

                        if ( GetVideoModeManager ()->IsMultiMonitor ()
                            && !GetVideoModeManager ()->IsMinimizeEnabled () )
                            return true;    // No auto-minimize
                    }
                }
                /*
                // Should not really ever get here, just in case. 
                else if ( uMsg == WM_SIZE )
                {
                    if ( wParam == SIZE_MINIMIZED )
                    {
                        ShowWindow ( pThis->GetHookedWindowHandle(), SW_RESTORE );
                        return true;
                    }
                }
                */

                if ( uMsg == WM_SYSCOMMAND && wParam == 0xF012 ) // SC_DRAGMOVE
                {
                    CMessageLoopHook::GetSingleton().StartWindowMovement ();
                    return true;
                }


                // If we handled mouse steering, don't let GTA.
                //if ( !CCore::GetSingleton ().GetMouseControl()->ProcessMouseMove ( uMsg, wParam, lParam ) )
                    // Call GTA's window procedure.
                    return CallWindowProcW ( pThis->m_HookedWindowProc, hwnd, uMsg, wParam, lParam );
            }

            // Don't allow DefWindowProc if processed here. (Important for IME)
            return true; 
        }
    }

    // Tell windows to handle this message.
    return DefWindowProcW ( hwnd, uMsg, wParam, lParam );
}

void CMessageLoopHook::StartWindowMovement()
{
    RECT ClientRect;
    GetWindowRect ( m_HookedWindowHandle, &ClientRect );
    POINT CursorPos;
    GetCursorPos ( &CursorPos );

    m_MoveOffset.x = ClientRect.left - CursorPos.x;
    m_MoveOffset.y = ClientRect.top - CursorPos.y;

    m_MovementDummyWindow = CreateWindowEx ( 0, "MovementDummy", "", 0, CW_USEDEFAULT, CW_USEDEFAULT,
        ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top, NULL, NULL, GetModuleHandle(NULL), NULL);

    LONG lStyle = GetWindowLong ( m_MovementDummyWindow, GWL_STYLE );
    lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_SYSMENU);
    SetWindowLong ( m_MovementDummyWindow, GWL_STYLE, lStyle );

    LONG lExStyle = GetWindowLong ( m_MovementDummyWindow, GWL_EXSTYLE );
    lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
    lExStyle |= WS_EX_LAYERED;
    SetWindowLong ( m_MovementDummyWindow, GWL_EXSTYLE, lExStyle );
    SetLayeredWindowAttributes ( m_MovementDummyWindow, 0, 140, LWA_ALPHA );

    SetWindowPos ( m_MovementDummyWindow, NULL, ClientRect.left, ClientRect.top, 0, 0, SWP_DRAWFRAME | SWP_NOZORDER | SWP_NOSIZE );
    //ShowWindow ( m_HookedWindowHandle, SW_HIDE );
    ShowWindow ( m_MovementDummyWindow, SW_SHOW );
    UpdateWindow ( m_MovementDummyWindow );

    // Set mouse capture to handle mouse event also if the cursor is not on the window (due to a too fast mouse)
    SetCapture ( m_MovementDummyWindow );
}

LRESULT CALLBACK CMessageLoopHook::ProcessDummyWindowMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CMessageLoopHook* pThis = CMessageLoopHook::GetSingletonPtr ();

    switch (uMsg)
    {
        case WM_NCMOUSEMOVE:
        case WM_MOUSEMOVE:
        {
            POINT CursorPos;
            GetCursorPos ( &CursorPos );
            SetWindowPos ( hwnd, NULL, CursorPos.x + pThis->m_MoveOffset.x, CursorPos.y + pThis->m_MoveOffset.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
            break;
        }
        case WM_LBUTTONUP:
        {
            // Destroy the dummy window
            DestroyWindow ( hwnd );
            pThis->m_MovementDummyWindow = NULL;

            POINT CursorPos;
            GetCursorPos ( &CursorPos );
            
            // Move the main window to the last position of the dummy window
            SetWindowPos ( pThis->m_HookedWindowHandle, NULL, CursorPos.x + pThis->m_MoveOffset.x, CursorPos.y + pThis->m_MoveOffset.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
            //ShowWindow ( pThis->m_HookedWindowHandle, SW_SHOW );

            // Release mouse capture
            ReleaseCapture ();

            break;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT PS;
            BeginPaint ( hwnd, &PS );
            RECT ClientRect;
            GetClientRect ( hwnd, &ClientRect );
            RECT BorderRect = { 5, 5, ClientRect.right - 5, ClientRect.bottom - 5 };

            // Give it some color
            HBRUSH BorderBrush = CreateSolidBrush ( 0x00000000 );
            HBRUSH FillBrush = CreateSolidBrush ( RGB(255,255,255) );
            FillRect ( PS.hdc, &ClientRect, BorderBrush );
            FillRect ( PS.hdc, &BorderRect, FillBrush );
            DeleteObject ( BorderBrush );
            DeleteObject ( FillBrush );
            EndPaint ( hwnd, &PS );

            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

HWND CMessageLoopHook::GetHookedWindowHandle ( ) const
{
    return m_HookedWindowHandle;
}
