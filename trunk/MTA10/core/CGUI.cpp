/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CGUI.cpp
*  PURPOSE:     Core graphical user interface container class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

using std::string;

template<> CLocalGUI * CSingleton < CLocalGUI >::m_pSingleton = NULL;

#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

CLocalGUI::CLocalGUI ( void )
{
    m_pConsole = NULL;
    m_pMainMenu = NULL;
	//m_pChatBox = NULL;
    m_pChat = NULL;
    m_pDebugView = NULL;

    m_bForceCursorVisible = false;
    m_bChatboxVisible = true;
    m_pDebugViewVisible = false;
    m_bGUIHasInput = false;

	m_bVisibleWindows = false;
	m_iVisibleWindows = 0;
}


CLocalGUI::~CLocalGUI ( void )
{
    // Destroy all GUI elements
    DestroyObjects ();
}


void CLocalGUI::CreateWindows ( void )
{
    CFilePathTranslator     FileTranslator;
    string                  WorkingDirectory;
    char                    szCurDir [ 1024 ];

    // Set the current directory.
    FileTranslator.SetCurrentWorkingDirectory ( "MTA" );
    FileTranslator.GetCurrentWorkingDirectory ( WorkingDirectory );
    GetCurrentDirectory ( sizeof ( szCurDir ), szCurDir );
    SetCurrentDirectory ( WorkingDirectory.c_str ( ) );

    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    // Create chatbox
    m_pChat = new CChat ( pGUI, CVector2D ( 0.0125f, 0.015f ) );
    m_pChat->SetVisible ( false );

    // Create the debug view
    m_pDebugView = new CDebugView ( pGUI, CVector2D ( 0.23f, 0.785f ) );
    m_pDebugView->SetVisible ( false );

	// Create the overlayed version labels
	CVector2D ScreenSize = pGUI->GetResolution ();
	m_pLabelVersionTag = reinterpret_cast < CGUILabel* > ( pGUI->CreateLabel ( MTA_VERSION_TAG ) );
	m_pLabelVersionTag->SetSize ( CVector2D ( 150, 18 ) );
	m_pLabelVersionTag->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
	m_pLabelVersionTag->SetPosition ( CVector2D ( ScreenSize.fX - 150, ScreenSize.fY - 15 ) );
	m_pLabelVersionTag->SetAlpha ( 0.5f );
	m_pLabelVersionTag->SetTextColor ( 255, 255, 255 );
	m_pLabelVersionTag->SetZOrderingEnabled ( false );
    m_pLabelVersionTag->MoveToBack ();
    m_pLabelVersionTag->SetVisible ( false );

    // Create mainmenu
    m_pMainMenu = new CMainMenu ( pGUI );
    m_pMainMenu->SetVisible ( false );

    // Create console
    m_pConsole = new CConsole ( pGUI );
    m_pConsole->SetVisible ( false );

    // Create community registration window
    m_CommunityRegistration.CreateWindows ();
    m_CommunityRegistration.SetVisible ( false );

	// Return the old current dir.
    SetCurrentDirectory ( szCurDir );
}


void CLocalGUI::CreateObjects ( IUnknown* pDevice )
{
    //Temps
    CFilePathTranslator     FileTranslator;
    string                  WorkingDirectory;
    char                    szCurDir [ 1024 ];

    // Store the GUI manager pointer and create the GUI classes
	CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    // Set the current directory.
    FileTranslator.SetCurrentWorkingDirectory ( "MTA" );
    FileTranslator.GetCurrentWorkingDirectory ( WorkingDirectory );
    GetCurrentDirectory ( sizeof ( szCurDir ), szCurDir );
    SetCurrentDirectory ( WorkingDirectory.c_str ( ) );
	
	// Create graphical wrapper object.
	WriteDebugEvent ( "Creating renderer wrapper..." );
	m_pRendererLibrary = new CD3DMGEng ( reinterpret_cast < LPDIRECT3DDEVICE9 > ( pDevice ) );

    // And lot it's fonts
	WriteDebugEvent ( "Loading font texture..." );
	if ( m_pRendererLibrary->LoadFontTextureFromFile ( "cgui\\sans.tga" ) )
	{
		WriteDebugEvent ( "Font texture load successful!" );
	}
	else
	{
		WriteDebugEvent ( "Font texture load failure!" );
	}

	if ( m_pRendererLibrary->LoadFontInfoFromFile ( "cgui\\sans.dat" ) )
	{
		WriteDebugEvent ( "Font data load successful!" );
	}
	else
	{
		WriteDebugEvent ( "Font data load failure!" );
	}

	CreateWindows ();

	// Return the old current dir.
    SetCurrentDirectory ( szCurDir );
}


void CLocalGUI::DestroyWindows ( void )
{
	SAFE_DELETE ( m_pLabelVersionTag );
    SAFE_DELETE ( m_pConsole );
    SAFE_DELETE ( m_pMainMenu );
    SAFE_DELETE ( m_pChat );
    SAFE_DELETE ( m_pDebugView );
}


void CLocalGUI::DestroyObjects ( void )
{
	DestroyWindows ();

    // Destroy and NULL all elements
    SAFE_DELETE ( m_pRendererLibrary );
	SAFE_DELETE ( m_pLabelVersionTag );
}


CD3DMGEng* CLocalGUI::GetRenderingLibrary ( void )
{
    return m_pRendererLibrary;
}


void CLocalGUI::DoPulse ( void )
{
    m_CommunityRegistration.DoPulse ();
}


void CLocalGUI::Draw ( void )
{
    // Get the game interface
    CGame* pGame = CCore::GetSingleton ().GetGame ();
    eSystemState SystemState = pGame->GetSystemState ();
    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    // Update mainmenu stuff
    m_pMainMenu->Update ();

	// Make sure our version labels are always visible
	static short WaitForMenu = 0;
	if ( SystemState == 7 || SystemState == 9 ) {
		if ( WaitForMenu < 250 ) {
			WaitForMenu++;
		} else {
			m_pLabelVersionTag->SetVisible ( true );
		}
	}

    // If we're ingame, make sure the chatbox is drawn
    bool bChatVisible = ( SystemState == 9 /* GS_INGAME */ && m_pMainMenu->GetIsIngame () && m_bChatboxVisible && !CCore::GetSingleton ().IsOfflineMod() );
    if ( m_pChat->IsVisible () != bChatVisible )
        m_pChat->SetVisible ( bChatVisible );
    bool bDebugVisible = ( SystemState == 9 /* GS_INGAME */ && m_pMainMenu->GetIsIngame () && m_pDebugViewVisible && !CCore::GetSingleton ().IsOfflineMod() );
    if ( m_pDebugView->IsVisible () != bDebugVisible )
        m_pDebugView->SetVisible ( bDebugVisible );

    // Make sure the cursor is displayed only when needed
    UpdateCursor ();

	// Draw the chat
    m_pChat->Draw ();
    // Draw the debugger
    m_pDebugView->Draw ();

    // If we're not at the loadingscreen
    static bool bDelayedFrame = false;
    if ( SystemState != 8 || !bDelayedFrame /* GS_INIT_PLAYING_GAME */ )
    {
        // If we have a GUI manager, draw the GUI
        if ( pGUI )
        {
            pGUI->Draw ( );
        }
        else
        {
            WriteDebugEvent ( "WARNING: CLocalGUI::Draw() called, but CLocalGUI::CreateObjects() isn't!" );
        }

        // If the system state was 8, make sure we don't do another delayed frame
        if ( SystemState == 8 )
        {
            bDelayedFrame = true;
        }
    }
}


void CLocalGUI::Invalidate ( void )
{
    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    // Invalidate the GUI
    if ( pGUI )
    {
        pGUI->Invalidate ( );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::Invalidate() called, but CLocalGUI::CreateObjects() isn't!" );
    }

    // Invalidate the renderer library
    if ( m_pRendererLibrary )
    {
        m_pRendererLibrary->OnInvalidateDevice ( );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::Invalidate() called, but CLocalGUI::CreateObjects() isn't!" );
    }
}


void CLocalGUI::Restore ( void )
{
    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    if ( pGUI )
    {
        CFilePathTranslator     FileTranslator;
        string                  WorkingDirectory;
        char                    szCurDir [ 1024 ];

        // We must change the current directory here!
        // This is necessary because if we don't, CLocalGUI will try to load
        // files from the wrong path!

        // Set the current directory.
        FileTranslator.SetCurrentWorkingDirectory ( "MTA" );
        FileTranslator.GetCurrentWorkingDirectory ( WorkingDirectory );
        GetCurrentDirectory ( sizeof ( szCurDir ), szCurDir );
        SetCurrentDirectory ( WorkingDirectory.c_str ( ) );

        // Restore the GUI
        pGUI->Restore ();

	    // Restore our renderer.
        if ( m_pRendererLibrary )
        {
	        m_pRendererLibrary->OnRestoreDevice ( );
        }
        else
        {
            WriteDebugEvent ( "WARNING: CLocalGUI::Restore() called, but CLocalGUI::CreateObjects() isn't!" );
        }

        // Restore the current directory to default.
        SetCurrentDirectory ( szCurDir );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::Restore() called, but CLocalGUI::CreateObjects() isn't!" );
    }
}

void CLocalGUI::DrawMouseCursor ( void )
{
    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    pGUI->DrawMouseCursor ();
}


CConsole* CLocalGUI::GetConsole ( void )
{
    return m_pConsole;
}


void CLocalGUI::SetConsoleVisible ( bool bVisible )
{
    if ( m_pConsole )
    {
        // Only allow the console if the chat doesn't take input
        if ( m_pChat->IsInputVisible () )
            bVisible = false;
        
        // Set the visible state
        m_pConsole->SetVisible ( bVisible );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::HideConsole() called, but CLocalGUI::CreateObjects() isn't!" );
    }
}


bool CLocalGUI::IsConsoleVisible ( void )
{
    if ( m_pConsole )
    {
        return m_pConsole->IsVisible ();
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::IsConsoleVisible() called, but CLocalGUI::CreateObjects() isn't!" );
        return false;
    }
}


void CLocalGUI::EchoConsole ( const char* szText )
{
    if ( m_pConsole )
    {
        m_pConsole->Echo ( szText );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::EchoConsole() called, but CLocalGUI::CreateObjects() isn't" );
    }
}


CMainMenu* CLocalGUI::GetMainMenu ( void )
{
    return m_pMainMenu;
}


void CLocalGUI::SetMainMenuVisible ( bool bVisible )
{
    if ( m_pMainMenu )
    {
        // This code installs the original CCore mouseclick handlers when the ingame menu
        // is shown, and restores the mod mouseclick handlers when the menu is hidden again.
        // This is needed to prevent a crash when double clicking a server in the server browser
        // while already ingame: the mod module gets unloaded while its doubleclick handler is
        // still running.
        m_pMainMenu->SetVisible ( bVisible );
        CGUI* pGUI = CCore::GetSingleton ().GetGUI ();
        if ( bVisible )
        {
            m_ModMouseClickHandler = pGUI->GetMouseClickHandler ();
            m_ModMouseDoubleClickHandler = pGUI->GetMouseDoubleClickHandler ();
            pGUI->SetMouseClickHandler ( GUI_CALLBACK_MOUSE ( &CCore::OnMouseClick, CCore::GetSingletonPtr () ) );
            pGUI->SetMouseDoubleClickHandler ( GUI_CALLBACK_MOUSE ( &CCore::OnMouseDoubleClick, CCore::GetSingletonPtr () ) );
        }
        else
        {
            pGUI->SetMouseClickHandler ( m_ModMouseClickHandler );
            pGUI->SetMouseDoubleClickHandler ( m_ModMouseDoubleClickHandler );
        }
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::SetMainMenuVisible() called, but CLocalGUI::CreateObjects() isn't" );
    }
}


bool CLocalGUI::IsMainMenuVisible ( void )
{
    if ( m_pMainMenu )
    {
        return m_pMainMenu->IsVisible ();
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::IsMainMenuVisible() called, but CLocalGUI::CreateObjects() isn't" );
        return false;
    }
}

/*CChatBox* CLocalGUI::GetChatBox ( void )
{
    return m_pChatBox;
}*/

CChat* CLocalGUI::GetChat ( void )
{
    return m_pChat;
}


CDebugView* CLocalGUI::GetDebugView ( void )
{
    return m_pDebugView;
}

void CLocalGUI::SetChatBoxVisible ( bool bVisible )
{
    if ( m_pChat )
    {
        if ( m_pChat->IsVisible () != bVisible )    
            m_pChat->SetVisible ( bVisible );
        m_bChatboxVisible = bVisible;
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::SetChatBoxVisible() called, but CLocalGUI::CreateObjects() isn't" );
    }
}

void CLocalGUI::SetDebugViewVisible ( bool bVisible )
{
    if ( m_pDebugView )
    {
        if ( m_pDebugView->IsVisible () != bVisible )
            m_pDebugView->SetVisible ( bVisible );
        m_pDebugViewVisible = bVisible;
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::SetDebugViewVisible() called, but CLocalGUI::CreateObjects() isn't" );
    }
}

bool CLocalGUI::IsChatBoxVisible ( void )
{
    if ( m_pChat )
    {
        return m_bChatboxVisible;
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::IsChatBoxVisible() called, but CLocalGUI::CreateObjects() isn't" );
        return false;
    }
}

bool CLocalGUI::IsDebugViewVisible ( void )
{
    if ( m_pDebugView )
    {
        return m_pDebugViewVisible;
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::IsDebugViewVisible() called, but CLocalGUI::CreateObjects() isn't" );
        return false;
    }
}


void CLocalGUI::SetChatBoxInputEnabled ( bool bInputEnabled )
{
    if ( m_pChat )
    {
        m_pChat->SetInputVisible ( bInputEnabled );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::SetChatBoxInputEnabled() called, but CLocalGUI::CreateObjects() isn't" );
    }
}


bool CLocalGUI::IsChatBoxInputEnabled ( void )
{
    if ( m_pChat )
    {
        return m_pChat->IsInputVisible () && m_bChatboxVisible;
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::IsChatBoxInputEnabled() called, but CLocalGUI::CreateObjects() isn't" );
        return false;
    }
}


void CLocalGUI::EchoChat ( const char* szText, bool bColorCoded )
{
    /*if ( m_pChatBox )
    {
        m_pChatBox->EchoChat ( szText );
    }*/
    if ( m_pChat )
    {
        m_pChat->Output ( const_cast < char* > ( szText ), bColorCoded );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::EchoChat() called, but CLocalGUI::CreateObjects() isn't" );
    }
}

void CLocalGUI::EchoDebug ( const char* szText )
{
    if ( m_pDebugView )
    {
        m_pDebugView->Output ( const_cast < char * > ( szText ), false );
    }
    else
    {
        WriteDebugEvent ( "WARNING: CLocalGUI::EchoDebug() called, but CLocalGUI::CreateObjects() isn't" );
    }
}

bool CLocalGUI::ProcessMessage ( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    // If we have the focus, we handle the message
    if ( InputGoesToGUI () )
    {
        // Pass the message to the GUI manager
		// ACHTUNG: fix the CEGUI ones!
        switch ( uMsg )
        {
			case WM_MOUSEWHEEL:
                if ( GET_WHEEL_DELTA_WPARAM ( wParam ) > 0 )  
				    pGUI->ProcessMouseInput ( CGUI_MI_MOUSEWHEEL, 1, NULL );
                else
                    pGUI->ProcessMouseInput ( CGUI_MI_MOUSEWHEEL, 0, NULL );
				return true;

            case WM_MOUSEMOVE:
                pGUI->ProcessMouseInput ( CGUI_MI_MOUSEPOS, LOWORD ( lParam ), HIWORD ( lParam ) );
                return true;

            case WM_LBUTTONDOWN:
                pGUI->ProcessMouseInput ( CGUI_MI_MOUSEDOWN, 0, 0, LeftButton );
                return true;

            case WM_RBUTTONDOWN:
                pGUI->ProcessMouseInput ( CGUI_MI_MOUSEDOWN, 0, 0, RightButton );
                return true;

            case WM_MBUTTONDOWN:
                pGUI->ProcessMouseInput ( CGUI_MI_MOUSEDOWN, 0, 0, MiddleButton );
                return true;

            case WM_LBUTTONUP:
                pGUI->ProcessMouseInput ( CGUI_MI_MOUSEUP, 0, 0, LeftButton );
                return true;

            case WM_RBUTTONUP:
                pGUI->ProcessMouseInput ( CGUI_MI_MOUSEUP, 0, 0, RightButton );
                return true;

			case WM_MBUTTONUP:
                pGUI->ProcessMouseInput ( CGUI_MI_MOUSEUP, 0, 0, MiddleButton );
                return true;
#ifdef WM_XBUTTONDOWN
			case WM_XBUTTONDOWN:
				pGUI->ProcessMouseInput ( CGUI_MI_MOUSEDOWN, 0, 0, X1Button );
				return true;

			case WM_XBUTTONUP:
				pGUI->ProcessMouseInput ( CGUI_MI_MOUSEUP, 0, 0, X1Button );
				return true;
#endif
            case WM_KEYDOWN:
            {
                DWORD dwTemp = TranslateScanCodeToGUIKey ( wParam );
                if ( dwTemp > 0 )
                {
                    pGUI->ProcessKeyboardInput ( dwTemp, true );
                    return true;
                }

                return true;
            }

            case WM_KEYUP:
            {
                DWORD dwTemp = TranslateScanCodeToGUIKey ( wParam );
                if ( dwTemp > 0 )
                {
                    pGUI->ProcessKeyboardInput ( dwTemp, false );
                }

                return false;
            }

            case WM_CHAR:
                pGUI->ProcessCharacter ( wParam );
                return true;
        }
    }

    // The event wasn't handled
    return false;
}


bool CLocalGUI::InputGoesToGUI ( void )
{
    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();
    if ( !pGUI ) return false;

    // Here we're supposed to check if things like menues are up, console is up or the chatbox is expecting input
    // If the console is visible OR the chat is expecting input OR the mainmenu is visible
    return ( IsConsoleVisible () || IsMainMenuVisible () || IsChatBoxInputEnabled () || m_bForceCursorVisible || pGUI->GetGUIInputEnabled () );
}


void CLocalGUI::ForceCursorVisible ( bool bVisible )
{
    m_bForceCursorVisible = bVisible;
}


void CLocalGUI::UpdateCursor ( void )
{
    CGUI* pGUI = CCore::GetSingleton ().GetGUI ();

    static DWORD dwWidth = CDirect3DData::GetSingleton().GetViewportWidth();
    static DWORD dwHeight = CDirect3DData::GetSingleton().GetViewportHeight();
    static POINT pointStoredPosition;
    static bool bFirstRun = true;

    if ( bFirstRun )
    {
        pointStoredPosition.x = dwWidth / 2;
        pointStoredPosition.y = dwHeight / 2;
        bFirstRun = false;
    }
    // Called in each frame to make sure the mouse is only visible when a GUI control that uses the
    // mouse requires it.
    if ( InputGoesToGUI () )
    {
        // We didn't have focus last pulse?
        if ( !m_bGUIHasInput )
        {
            /* Store if the controller was enabled or not and disable it
            CCore::GetSingleton ().GetGame ()->GetPad ()->Disable ( true );
            CCore::GetSingleton ().GetGame ()->GetPad ()->Clear ();*/

            // Restore the mouse cursor to its old position
            SetCursorPos ( pointStoredPosition.x, pointStoredPosition.y );

            // Enable our mouse cursor
            CSetCursorPosHook::GetSingleton ( ).DisableSetCursorPos ();
            pGUI->SetCursorEnabled ( true );

            m_bGUIHasInput = true;
        }
    }
    else
    {
        // We had focus last frame?
        if ( m_bGUIHasInput )
        {
            /* Restore the controller state
            CCore::GetSingleton ().GetGame ()->GetPad ()->Disable ( false );
            CCore::GetSingleton ().GetGame ()->GetPad ()->Clear ();*/

            // Save the mouse cursor position
            GetCursorPos ( &pointStoredPosition );

            // Set the mouse back to the center of the screen (to prevent the game from reacting to its movement)
            SetCursorPos ( dwWidth / 2, dwHeight / 2 );

            // Disable our mouse cursor
            CSetCursorPosHook::GetSingleton ( ).EnableSetCursorPos ();
            pGUI->SetCursorEnabled ( false );

            m_bGUIHasInput = false;
        }
    }
}


DWORD CLocalGUI::TranslateScanCodeToGUIKey ( DWORD dwCharacter )
{
	// The following switch case is necessary to convert input WM_KEY* messages
	// to corresponding DirectInput key messages.  CEGUI needs these.

    switch ( dwCharacter )
    {
        case VK_HOME:       return DIK_HOME;
		case VK_END:		return DIK_END;
        case VK_RETURN:     return DIK_RETURN;
        case VK_TAB:        return DIK_TAB;
        case VK_BACK:       return DIK_BACK;
        case VK_LEFT:       return DIK_LEFTARROW;
        case VK_RIGHT:      return DIK_RIGHTARROW;
        case VK_UP:         return DIK_UPARROW;
        case VK_DOWN:       return DIK_DOWNARROW;
        case VK_LSHIFT:     return DIK_LSHIFT;
        case VK_RSHIFT:     return DIK_RSHIFT;
		case VK_CONTROL:    return DIK_LCONTROL;
        case VK_DELETE:     return DIK_DELETE;
		case 0x56:		    return DIK_V;			// V
		case 0x43:		    return DIK_C;			// C
        case 0x58:		    return DIK_X;			// X
        case 0x41:		    return DIK_A;			// A
        default:            return 0;
    }
}

int CLocalGUI::GetVisibleWindows ( )
{
	// By calling SetVisibleWindows( true ) a counter will be activated.
	// This VisibleWindows counter will increase/decrease on any
	// window show/hide event. By calling GetVisibleWindows( ) you can
	// determine how many windows are currently visible.

	// Return the VisibleWindows counter value
	return m_iVisibleWindows;
}

void CLocalGUI::SetVisibleWindows ( bool bEnable )
{
	// This function can be used to enable or disable the VisibleWindows
	// counter. Use 'true' to enable. Use 'false' to disable.

	// Reset the counter in any case
	m_iVisibleWindows = 0;

	m_bVisibleWindows = bEnable;
}

void CLocalGUI::HiddenHandler ( bool bHandled )
{
	// ACHTUNG: fix CGUIElementEventArgs !
	/*
	const CGUIEventArgs& WindowArgs = reinterpret_cast < const CGUIEventArgs& > ( Args );

	// Check if the counter is enabled, and the window's a FrameWindow
	if ( m_bVisibleWindows && WindowArgs.window->getType( ) == "CGUI/FrameWindow" ) {
		// Decrement the VisibleWindows counter
		m_iVisibleWindows--;
	}
	*/
	return;
}

void CLocalGUI::ShownHandler ( bool bHandled )
{
	// ACHTUNG: fix CGUIElementEventArgs !
	/*
	const CGUIElementEventArgs& WindowArgs = reinterpret_cast < const CGUIElementEventArgs& > ( Args );

	// Check if the counter is enabled, and the window's a FrameWindow
	if ( m_bVisibleWindows && WindowArgs.window->getType( ) == "CGUI/FrameWindow" ) {
		// Increment the VisibleWindows counter
		m_iVisibleWindows++;
	}
	*/
	return;
}

void CLocalGUI::KeyDownHandler ( bool bHandled )
{
	// ACHTUNG: fix CGUIKeyEventArgs !
	/*
	// Cast it to a set of keyboard arguments
	const CEGUI::KeyEventArgs& KeyboardArgs = reinterpret_cast < const CEGUI::KeyEventArgs& > ( Args );

	switch ( KeyboardArgs.scancode )
	{
        // Cut/Copy keys
        case CEGUI::Key::Scan::X:
		case CEGUI::Key::Scan::C:
        {
			if ( KeyboardArgs.sysKeys & CEGUI::Control )
            {
                // Data to copy
                CEGUI::String strTemp;

                // Edit boxes
				CEGUI::Window* Wnd = reinterpret_cast < CEGUI::Window* > ( KeyboardArgs.window );
				if ( Wnd->getType () == "CGUI/Editbox" )
                {
					// Turn our event window into an editbox
					CEGUI::Editbox* WndEdit = reinterpret_cast < CEGUI::Editbox* > ( Wnd );

					// Get the text from the editbox
                    size_t sizeSelectionStart = WndEdit->getSelectionStartIndex ();
                    size_t sizeSelectionLength = WndEdit->getSelectionLength ();
                    strTemp = WndEdit->getText ().substr ( sizeSelectionStart, sizeSelectionLength );

                    // If the user cut, remove the text too
                    if ( KeyboardArgs.scancode == CEGUI::Key::Scan::X )
                    {
                        // Read only?
                        if ( !WndEdit->isReadOnly () )
                        {
                            // Remove the text from the source
                            CEGUI::String strTemp2 = WndEdit->getText ();
                            strTemp2.replace ( sizeSelectionStart, sizeSelectionLength, "", 0 );
                            WndEdit->setText ( strTemp2 );
                        }
                    }
				}

                // Multiline editboxes
				if ( Wnd->getType () == "CGUI/MultiLineEditbox" )
                {
					// Turn our event window into an editbox
					CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast < CEGUI::MultiLineEditbox* > ( Wnd );

					// Get the text from the editbox
                    size_t sizeSelectionStart = WndEdit->getSelectionStartIndex ();
                    size_t sizeSelectionLength = WndEdit->getSelectionLength ();
                    strTemp = WndEdit->getText ().substr ( sizeSelectionStart, sizeSelectionLength );

                    // If the user cut, remove the text too
                    if ( KeyboardArgs.scancode == CEGUI::Key::Scan::X )
                    {
                        // Read only?
                        if ( !WndEdit->isReadOnly () )
                        {
                            // Remove the text from the source
                            CEGUI::String strTemp2 = WndEdit->getText ();
                            strTemp2.replace ( sizeSelectionStart, sizeSelectionLength, "", 0 );
                            WndEdit->setText ( strTemp2 );
                        }
                    }
				}

                // If we got something to copy
                if ( strTemp.length () > 0 )
                {
                    // Open and empty the clipboard
                    OpenClipboard ( NULL );
                    EmptyClipboard ();

                    // Allocate the clipboard buffer and copy the data
                    HGLOBAL hBuf = GlobalAlloc ( GMEM_DDESHARE, strTemp.length () + 1 );
                    char* buf = reinterpret_cast < char* > ( GlobalLock ( hBuf ) );
                    strcpy ( buf , strTemp.c_str () );
                    GlobalUnlock ( hBuf );

                    // Copy the data into the clipboard
                    SetClipboardData ( CF_TEXT , hBuf );

                    // Close the clipboard
                    CloseClipboard( );
                }
			}

			break;
        }

        // Paste keys
		case CEGUI::Key::Scan::V:
        {
			if ( KeyboardArgs.sysKeys & CEGUI::Control )
            {
				CEGUI::Window* Wnd = reinterpret_cast < CEGUI::Window* > ( KeyboardArgs.window );
				if ( Wnd->getType ( ) == "CGUI/Editbox" )
                {
					// Open the clipboard
					OpenClipboard( NULL );

					// Turn our event window into an editbox
					CEGUI::Editbox* WndEdit = reinterpret_cast < CEGUI::Editbox* > ( Wnd );

					// Get the clipboard's data and put it into a char array
					const char * ClipboardBuffer = reinterpret_cast < const char* > ( GetClipboardData ( CF_TEXT ) );

                    // Check to make sure we have valid data.
                    if ( ClipboardBuffer )
                    {
					    // Put the editbox's data into a string and insert the data if it has not reached it's maximum text length
					    CEGUI::String tmp = WndEdit->getText ();
					    if ( ( strlen ( ClipboardBuffer ) + tmp.length () ) < WndEdit->getMaxTextLength( ) )
                        {
                            // Are there characters selected?
                            size_t sizeCaratIndex = 0;
                            if ( WndEdit->getSelectionLength () > 0 )
                            {
                                // Replace what's selected with the pasted buffer and set the new carat index
                                tmp.replace ( WndEdit->getSelectionStartIndex (), WndEdit->getSelectionLength (), ClipboardBuffer, strlen ( ClipboardBuffer ) );
                                sizeCaratIndex = WndEdit->getSelectionStartIndex () + strlen ( ClipboardBuffer );
                            }
                            else
                            {
                                // If not, insert the clipboard buffer where we were and set the new carat index
						        tmp.insert ( WndEdit->getSelectionStartIndex (), ClipboardBuffer , strlen ( ClipboardBuffer ) );
                                sizeCaratIndex = WndEdit->getCaratIndex () + strlen ( ClipboardBuffer );
                            }

                            // Set the new text and move the carat at the end of what we pasted
                            WndEdit->setText ( tmp );
                            WndEdit->setCaratIndex ( sizeCaratIndex );
					    }
                        else
                        {
						    // Fire an event if the editbox is full
						    WndEdit->fireEvent ( CEGUI::Editbox::EventEditboxFull , CEGUI::WindowEventArgs ( WndEdit ) );
					    }
                    }

					// Close the clipboard
					CloseClipboard( );
				}
			}

			break;
        }

        // Select all key
        case CEGUI::Key::Scan::A:
        {
			if ( KeyboardArgs.sysKeys & CEGUI::Control )
            {
                // Edit boxes
			    CEGUI::Window* Wnd = reinterpret_cast < CEGUI::Window* > ( KeyboardArgs.window );
			    if ( Wnd->getType () == "CGUI/Editbox" )
                {
				    // Turn our event window into an editbox
				    CEGUI::Editbox* WndEdit = reinterpret_cast < CEGUI::Editbox* > ( Wnd );
                    WndEdit->setSelection ( 0, WndEdit->getText ().size () );
                }
                else if ( Wnd->getType () == "CGUI/MultiLineEditbox" )
                {
				    // Turn our event window into a multiline editbox
				    CEGUI::MultiLineEditbox* WndEdit = reinterpret_cast < CEGUI::MultiLineEditbox* > ( Wnd );
                    WndEdit->setSelection ( 0, WndEdit->getText ().size () );
                }
            }

            break;
        }
	}
	*/
	return;
}