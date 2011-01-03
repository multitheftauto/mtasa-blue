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

CMessageLoopHook::CMessageLoopHook ( )
{
    WriteDebugEvent ( "CMessageLoopHook::CMessageLoopHook" );
    m_HookedWindowProc      = NULL;
    m_HookedWindowHandle    = NULL;
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


LRESULT CALLBACK CMessageLoopHook::ProcessMessage ( HWND hwnd, 
                                                    UINT uMsg, 
                                                    WPARAM wParam, 
                                                    LPARAM lParam )
{
    CMessageLoopHook *  pThis;

    // Get a pointer to ourself.
    pThis = CMessageLoopHook::GetSingletonPtr ( );

    // Log our state
    if ( uMsg == WM_KILLFOCUS || (uMsg == WM_ACTIVATE && LOWORD(wParam) == WA_INACTIVE) )
    {
        g_pCore->SetFocused ( false );
        CSetCursorPosHook::GetSingleton ().DisableSetCursorPos ();
    }
    else if ( uMsg == WM_SETFOCUS || (uMsg == WM_ACTIVATE && LOWORD(wParam) != WA_INACTIVE) )
    {
        g_pCore->SetFocused ( true );
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

    if ( hwnd != pThis->GetHookedWindowHandle () ) return NULL;

    // Make sure our pointers are valid.
    if ( pThis != NULL && CLocalGUI::GetSingletonPtr ( ) != NULL && CCore::GetSingleton ().GetGame () )
    {
        if ( uMsg == WM_KEYUP && wParam == VK_ESCAPE ) 
            return true;

        if ( CKeyBinds::IsFakeCtrl_L ( uMsg, wParam, lParam ) )
            return true;

        // See if this is message was caused by our asynchronous sockets
        if ( uMsg >= WM_ASYNCTRAP && uMsg <= ( WM_ASYNCTRAP + 255 ))
        {
            /* ACHTUNG: uMsg - 10? Windows seems to add 10 or there's a bug in the message code. Hack! */
            // Let the CTCPManager handle it
            CTCPManager::GetSingletonPtr ()->HandleEvent ( ( uMsg - WM_ASYNCTRAP ), lParam );
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
                            pConsole->SetVisible ( false );
                            return true;
                        }

                        // The mainmenu makes sure it isn't hidden if UseIngameButtons == false
                        if ( !CCore::GetSingleton().IsOfflineMod () )
                        {
                            // Stop chat input
                            if ( CLocalGUI::GetSingleton ().IsChatBoxInputEnabled () )
                            {
                                CLocalGUI::GetSingleton ().SetChatBoxInputEnabled ( false );
                                return true;
                            }

                            CLocalGUI::GetSingleton ().SetMainMenuVisible ( !CLocalGUI::GetSingleton ().IsMainMenuVisible () );
                            return true;
                        }
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

                    // If the console is visible, and we pressed down/up, scroll the console history
                    //                          or if we pressed tab, step through possible autocomplete matches
                    if ( CLocalGUI::GetSingleton ().IsConsoleVisible () )
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
                                return true; 
                            }
                            else
                            {
                                CLocalGUI::GetSingleton ().GetConsole ()->ResetAutoCompleteMatch ();
                            }
                        }
                    }
                }
            }
        }

        if ( !bWasCaptureKey )
        {
            // Lead the message through the keybinds message processor
            g_pCore->GetKeyBinds ()->ProcessMessage ( hwnd, uMsg, wParam, lParam );

            // Lead the message through main menu scene, so any scenes can be aborted
            CMainMenuScene::GetSingletonPtr ()->ProcessMessage ( hwnd, uMsg, wParam, lParam );

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


                // Call GTA's window procedure.
                return CallWindowProcW ( pThis->m_HookedWindowProc, hwnd, uMsg, wParam, lParam );
            }
        }
    }

    // Tell windows to handle this message.
    return DefWindowProcW ( hwnd, uMsg, wParam, lParam );
}


HWND CMessageLoopHook::GetHookedWindowHandle ( ) const
{
    return m_HookedWindowHandle;
}
