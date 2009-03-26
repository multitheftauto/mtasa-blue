/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMainMenu.cpp
*  PURPOSE:     2D Main menu graphical user interface
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>

#define NATIVE_RES_X	1024.0f
#define NATIVE_RES_Y	768.0f
#define MAX_RES_X		NATIVE_RES_X
#define MAX_RES_Y		NATIVE_RES_Y

#define CORE_MTA_MENUITEMS_START_X	80

#define CORE_MTA_BG_MAX_ALPHA		1.00f	//ACHTUNG: Set to 1 for now due to GTA main menu showing through (no delay inserted between Entering game... and loading screen)
#define CORE_MTA_FADER				0.05f // 1/20
#define CORE_MTA_FADER_CREDITS		0.01f

#define CORE_MTA_HIDDEN_ALPHA		0.0f
#define CORE_MTA_DISABLED_ALPHA		0.4f
#define CORE_MTA_ENABLED_ALPHA		1.0f

#define CORE_MTA_STATIC_BG			"cgui\\images\\static.png"
#define CORE_MTA_FILLER				"cgui\\images\\mta_filler.png"
#define CORE_MTA_HEADER				"cgui\\images\\mta_header.png"
#define CORE_MTA_HEADER_X			580
#define CORE_MTA_HEADER_Y			60

static short WaitForMenu = 0;

// Clamps a value between two other values ( min < x < max )
inline float Clamp( float min, float x, float max )
{
    if ( x < min ) return min; else
    if ( x > max ) return max;
    return x;
}

CMainMenu::CMainMenu ( CGUI* pManager )
{
    // Initialize
    m_pManager = pManager;
    m_bIsVisible = false;
    m_bIsIngame = true;
//    m_bIsInSubWindow = false;
	m_bInitialized = false;
	m_bStarted = false;
	m_bStaticBackground = false;
	m_pRenderTarget = NULL;
	m_fFader = 0;
	m_uiItems = 0;
	m_ucFade = FADE_INVISIBLE;
	m_bFadeToCredits = false;

	m_iButtons =			6;
	m_fWindowX =			260.0f;
	m_fWindowY =			330.0f;
	m_fButtonSpacer =		4.0f;
	m_fButtonSpacerTop =	23.0f;
	m_fButtonSpacerLeft =	7.0f;

	// Adjust window size to resolution
	CVector2D ScreenSize = m_pManager->GetResolution ();
	if ( ScreenSize.fX < NATIVE_RES_X ) {
		m_fWindowX *= ScreenSize.fX / MAX_RES_X;
		m_fWindowY *= ScreenSize.fY / MAX_RES_Y;
	}
	m_fButtonWidth =	m_fWindowX-m_fButtonSpacerLeft*2;
	m_fButtonHeight =	(m_fWindowY-m_fButtonSpacerTop-m_fButtonSpacer*(m_iButtons+1))/m_iButtons;

	// Scene background image
    m_pBackground = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
	m_pBackground->LoadFromFile ( CGUI_IMAGE_BACKGROUND );
    m_pBackground->MoveToBack ();
    m_pBackground->SetVisible ( false );
	m_pBackground->SetAlpha ( 0 );
    m_pBackground->SetZOrderingEnabled ( false );

	// Filler background image
    m_pFiller = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage () );
	m_pFiller->LoadFromFile ( CORE_MTA_FILLER );
    m_pFiller->SetFrameEnabled ( false );
    m_pFiller->SetVisible ( false );
    m_pFiller->MoveToBack ();
	m_pFiller->SetVisible ( false );
	m_pFiller->SetEnabled ( false );

	// Header image
	m_pHeader = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage ( m_pBackground ) );
	assert ( m_pHeader->LoadFromFile ( CORE_MTA_HEADER ) == true );
	//m_pHeader->MoveToBack ();
	m_pHeader->SetFrameEnabled ( false );
	m_pHeader->SetVisible ( false );
	m_pHeader->SetSize ( CVector2D ( CORE_MTA_HEADER_X, CORE_MTA_HEADER_Y ) );
	m_pHeader->SetAlpha ( 0 );

	m_pCommunityLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pBackground, "Not logged in" ) );
	m_pCommunityLabel->AutoSize ( "Not logged in" );
    m_pCommunityLabel->SetAlwaysOnTop ( true );
    m_pCommunityLabel->SetAlpha ( 0.7f );
    m_pCommunityLabel->SetVisible ( false );

    std::string strUsername;
    CCore::GetSingleton().GetCommunity()->GetUsername ( strUsername );
    if ( CCore::GetSingleton().GetCommunity()->IsLoggedIn() && !strUsername.empty() )
        ChangeCommunityState ( true, strUsername );

	// Determine some variables based on the current screen size
	unsigned int uiItemStartY = ScreenSize.fY / 2;
	//unsigned int uiItemHeight = ( uiItemStartY / 1.55f ) / CORE_MTA_MENU_ITEMS;
	unsigned int uiItemHeight = 30;

	// Create the menu items
	CreateItem ( MENU_ITEM_DISCONNECT,     CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY ),					"DISCONNECT",			GUI_CALLBACK ( &CMainMenu::OnDisconnectButtonClick, this ) );
    CreateItem ( MENU_ITEM_QUICK_CONNECT,  CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*1 ),	"QUICK CONNECT",		GUI_CALLBACK ( &CMainMenu::OnQuickConnectButtonClick, this ) );
    CreateItem ( MENU_ITEM_BROWSE_SERVERS, CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*2 ),	"BROWSE SERVERS",		GUI_CALLBACK ( &CMainMenu::OnBrowseServersButtonClick, this ) );
    CreateItem ( MENU_ITEM_HOST_GAME,      CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*3 ),	"HOST GAME",		    GUI_CALLBACK ( &CMainMenu::OnHostGameButtonClick, this ) );
    CreateItem ( MENU_ITEM_MAP_EDITOR,     CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*4 ),	"MAP EDITOR",           GUI_CALLBACK ( &CMainMenu::OnEditorButtonClick, this ) );
    CreateItem ( MENU_ITEM_SETTINGS,       CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*5 ),	"SETTINGS",				GUI_CALLBACK ( &CMainMenu::OnSettingsButtonClick, this ) );
    CreateItem ( MENU_ITEM_ABOUT,          CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*6 ),	"ABOUT",				GUI_CALLBACK ( &CMainMenu::OnAboutButtonClick, this ) );
	CreateItem ( MENU_ITEM_QUIT,           CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*7 ),	"QUIT",					GUI_CALLBACK ( &CMainMenu::OnQuitButtonClick, this ) );

    //Refresh all positions
    RefreshPositions ( );

	// Disable the disconnect item
	DisableItem ( MENU_ITEM_DISCONNECT, true );

	// Submenu's
    m_QuickConnect.SetVisible ( false );
    m_ServerBrowser.SetVisible ( false );
    m_Settings.SetVisible ( false );
    m_Credits.SetVisible ( false );

    // We're not ingame
    SetIsIngame ( false );

	// Store the pointer to the graphics subsystem
	m_pGraphics = CGraphics::GetSingletonPtr ();

	// Null the scene pointer
	m_pMainMenuScene = new CMainMenuScene ( this );

    // Load the server lists
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    m_ServerBrowser.LoadServerList ( pConfig->FindSubNode ( CONFIG_NODE_SERVER_FAV ),
        CONFIG_FAVOURITE_LIST_TAG, m_ServerBrowser.GetFavouritesList () );
    m_ServerBrowser.LoadServerList ( pConfig->FindSubNode ( CONFIG_NODE_SERVER_REC ),
        CONFIG_RECENT_LIST_TAG, m_ServerBrowser.GetRecentList () );

	// This class is destroyed when the video mode is changed (vid), so the config can already be loaded
	// If it is already loaded (and this constructor is not called on startup) then load the menu options
//    if ( CClientVariables::GetSingleton ().IsLoaded () )
//		LoadMenuOptions ();
}


CMainMenu::~CMainMenu ( void )
{
    // Save server lists
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();

    CXMLNode* pFavourites = pConfig->FindSubNode ( CONFIG_NODE_SERVER_FAV );
    if ( !pFavourites )
        pFavourites = pConfig->CreateSubNode ( CONFIG_NODE_SERVER_FAV );
    m_ServerBrowser.SaveServerList ( pFavourites, CONFIG_FAVOURITE_LIST_TAG, m_ServerBrowser.GetFavouritesList () );
    
    CXMLNode* pRecent = pConfig->FindSubNode ( CONFIG_NODE_SERVER_REC );
    if ( !pRecent )
        pRecent = pConfig->CreateSubNode ( CONFIG_NODE_SERVER_REC );
    m_ServerBrowser.SaveServerList ( pRecent, CONFIG_RECENT_LIST_TAG, m_ServerBrowser.GetRecentList () );

	// Delete menu items
	for ( unsigned int i = 0; i < CORE_MTA_MENU_ITEMS; i++ ) {
		if ( m_pItems[i] )
            delete m_pItems[i];
	}

    // Destroy GUI items
    delete m_pBackground;
	delete m_pFiller;
	delete m_pHeader;

    // Destroy community label
    delete m_pCommunityLabel;

	// Destroy the scene
	delete m_pMainMenuScene;
}

void CMainMenu::RefreshPositions ( void )
{
    CVector2D ScreenSize = m_pManager->GetResolution ();
	m_pFiller->SetSize ( ScreenSize );
    m_pHeader->SetPosition ( CVector2D ( ScreenSize.fX/2 - CORE_MTA_HEADER_X/2, ScreenSize.fY/6 ), false );
    m_pCommunityLabel->SetPosition ( CVector2D ( 40.0f, ScreenSize.fY - 20.0f ) );

    //Set our individual item positions
    unsigned int uiItemStartY = ScreenSize.fY / 2;
	unsigned int uiItemHeight = 30;
	SetItemPosition ( MENU_ITEM_DISCONNECT,     CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY ), false );
    SetItemPosition ( MENU_ITEM_QUICK_CONNECT,  CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*1 ), false );
    SetItemPosition ( MENU_ITEM_BROWSE_SERVERS, CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*2 ), false );
    SetItemPosition ( MENU_ITEM_HOST_GAME,      CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*3 ), false );
    SetItemPosition ( MENU_ITEM_MAP_EDITOR,     CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*4 ), false );
    SetItemPosition ( MENU_ITEM_SETTINGS,       CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*5 ), false );
    SetItemPosition ( MENU_ITEM_ABOUT,          CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*6 ), false );
	SetItemPosition ( MENU_ITEM_QUIT,           CVector2D ( CORE_MTA_MENUITEMS_START_X, uiItemStartY + uiItemHeight*7 ), false );
}

void CMainMenu::Update ( void )
{
	// Get the game interface and the system state
    CGame* pGame = CCore::GetSingleton ().GetGame ();
    eSystemState SystemState = pGame->GetSystemState ();

    m_Credits.Update ();
	m_Settings.Update ();

	// Fade in
	if ( m_ucFade == FADE_IN ) {
		for ( unsigned int i = 0; i < CORE_MTA_MENU_ITEMS; i++ ) {
			// Determine the clamping value
			float fMax = CORE_MTA_ENABLED_ALPHA;
			switch ( reinterpret_cast < int > ( m_pItems[i]->GetUserData () ) ) {
				case ITEM_DISABLED:
					fMax = CORE_MTA_DISABLED_ALPHA;
					break;
				case ITEM_HIDDEN:
					fMax = CORE_MTA_HIDDEN_ALPHA;
					break;
				case ITEM_ENABLED:
				default:
					break;
			}

			// Set the alpha value
			if ( m_pItems[i] ) m_pItems[i]->SetAlpha ( Clamp ( 0, m_fFader, fMax ) );
		}
		m_pBackground->SetAlpha ( Clamp ( 0, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
		m_pHeader->SetAlpha ( m_fFader );

		// Increment the fader (use the other define if we're fading to the credits)
		if ( m_bFadeToCredits )
			m_fFader += CORE_MTA_FADER_CREDITS;
		else
			m_fFader += CORE_MTA_FADER;

		// If the fade is complete
		if ( m_fFader >= 1 ) {
			m_ucFade = FADE_VISIBLE;
			m_bIsVisible = true;

			// Fade to the credits
			if ( m_bFadeToCredits ) {
				m_bFadeToCredits = false;

				// Start the credits scene
				m_pMainMenuScene->StartCredits ();
			}
		}
	}
	// Fade out
	else if ( m_ucFade == FADE_OUT ) {
		for ( unsigned int i = 0; i < CORE_MTA_MENU_ITEMS; i++ ) {
			// Determine the clamping value
			float fMax = CORE_MTA_ENABLED_ALPHA;
			switch ( reinterpret_cast < int > ( m_pItems[i]->GetUserData () ) ) {
				case ITEM_DISABLED:
					fMax = CORE_MTA_DISABLED_ALPHA;
					break;
				case ITEM_HIDDEN:
					fMax = CORE_MTA_HIDDEN_ALPHA;
					break;
				case ITEM_ENABLED:
				default:
					break;
			}

			// Set the alpha value
			if ( m_pItems[i] ) m_pItems[i]->SetAlpha ( Clamp ( 0, m_fFader, fMax ) );
		}
		m_pBackground->SetAlpha ( Clamp ( 0, m_fFader, CORE_MTA_BG_MAX_ALPHA ) );
		m_pHeader->SetAlpha ( m_fFader );

		m_fFader -= CORE_MTA_FADER;

		// If the fade is complete
		if ( m_fFader <= 0 ) {
			m_ucFade = FADE_INVISIBLE;
			m_bIsVisible = false;

			// Turn the widgets invisible
			m_pBackground->SetVisible ( false );
			m_pHeader->SetVisible ( false );
//			m_pFiller->SetVisible ( false );

			for ( unsigned int i = 0; i < CORE_MTA_MENU_ITEMS; i++ ) {
				if ( m_pItems[i] ) m_pItems[i]->SetVisible ( false );
			}

			// Fade to the credits, so fade in again with the background widget visible only
			if ( m_bFadeToCredits ) {
				m_ucFade = FADE_IN;
				m_fFader = 0.0f;

				m_pBackground->SetVisible ( true );

				// Initialize the credits scene
				m_pMainMenuScene->PreStartCredits ();
			}
		}
	}

    // Force the mainmenu on if we're at GTA's mainmenu or not ingame
    if ( ( SystemState == 7 || SystemState == 9 ) && !m_bIsIngame )
    {
		// Initialize our 3D scene once the device is available
		IDirect3DDevice9 * pDevice = m_pGraphics->GetDevice ();
		if ( !m_bInitialized && pDevice )
        {            
			m_bInitialized = true;

			// If the static flag has not already been set
			if ( m_bStaticBackground )
            {
				SetStaticBackground ( true );
			}
            else
            {
				// Create the texture to be used in the background
				CVector2D ScreenSize = m_pManager->GetResolution ();
				m_pRenderTarget = m_pManager->CreateTexture ();
				m_pRenderTarget->CreateTexture ( ScreenSize.fX, ScreenSize.fY );
				m_pBackground->LoadFromTexture ( m_pRenderTarget );

				// Initialize the scene class
				if ( m_pMainMenuScene->Init3DScene ( m_pRenderTarget->GetD3DTexture (), ScreenSize ) == false ) {
					// If the scene initialisation failed, we need to use a static background
					SetStaticBackground ( true );
				}

				// If we are already past the fading, play the video
				if ( WaitForMenu >= 250 ) {
					m_pMainMenuScene->PlayVideo ();
				}
			}
		}

        // it takes 250 frames for the menu to be shown, we seem to update this twice a frame
		if ( WaitForMenu >= 250 ) {
            m_bIsVisible = true;

			// Start the video
			if ( !m_bStarted ) {
				m_bStarted = true;
				m_pMainMenuScene->PlayVideo ();
			}
		} else
            WaitForMenu++;
    }

    // If we're visible
    if ( m_bIsVisible && SystemState != 8 )
    {
        // If we're at the game's mainmenu, or ingame when m_bIsIngame is true show the background
        if ( SystemState == 7 ||					// GS_FRONTEND
             SystemState == 9 && !m_bIsIngame )		// GS_INGAME
        {
			if ( m_ucFade == FADE_INVISIBLE )
				Show ( false );

			// Only draw the 3D scene if we have a dynamic background
			if ( !m_bStaticBackground && m_bInitialized )
				m_pMainMenuScene->Draw3DScene ();
        }
        else
        {
			if ( m_ucFade == FADE_INVISIBLE )
				Show ( true );
			/*
            m_pBackground->SetVisible ( false );
			m_pFiller->SetVisible ( false );
			m_pHeader->SetVisible ( false );
			*/
        }

        // If we're in a submenu, hide the mainmenu dialog, if not, show it
		/*
        if ( m_bIsInSubWindow )
        {
            m_pWindow->SetVisible ( false );
        }
        else
        {
            m_pWindow->SetVisible ( true );
        }
		*/
    }
    else
    {
		if ( m_ucFade == FADE_VISIBLE )
			Hide ();
		/*
        static bool bDelayedFrame = false;
        if ( SystemState == 8 && !bDelayedFrame )
        {
            bDelayedFrame = true;

            // Show mainmenu and background
            m_pWindow->SetVisible ( false );
            m_pBackground->SetVisible ( true );
			m_pFiller->SetVisible ( true );
			m_pHeader->SetVisible ( true );
        }
        else
        {
            // Hide mainmenu and background
            m_pWindow->SetVisible ( false );
            m_pBackground->SetVisible ( false );
			m_pFiller->SetVisible ( false );
			m_pHeader->SetVisible ( false );
        }
		*/
    }

    // Call subdialog pulses
    m_ServerBrowser.Update ();
}


void CMainMenu::Show ( bool bOverlay )
{
	SetVisible ( true, bOverlay );
}


void CMainMenu::Hide ( void )
{
	SetVisible ( false );
}


void CMainMenu::SetVisible ( bool bVisible, bool bOverlay )
{
    CMultiplayer* pMultiplayer = CCore::GetSingleton ().GetMultiplayer ();
    pMultiplayer->DisablePadHandler ( bVisible );

	if ( ( m_ucFade == FADE_VISIBLE || m_ucFade == FADE_IN ) && bVisible == false  ) {
		m_ucFade = FADE_OUT;
    } else if ( ( m_ucFade == FADE_INVISIBLE || m_ucFade == FADE_OUT ) && bVisible == true ) {
		m_ucFade = FADE_IN;
	}

    // If we're hiding, hide any subwindows we might've had (prevent escaping hiding mousecursor issue)
    if ( !bVisible )
    {
        m_QuickConnect.SetVisible ( false );
        m_ServerBrowser.SetVisible ( false );
        m_Settings.SetVisible ( false );
        m_Credits.SetVisible ( false );
        m_pCommunityLabel->SetVisible ( false );

//        m_bIsInSubWindow = false;
	} else {
		m_pBackground->SetVisible ( true );
		m_pHeader->SetVisible ( true );
		m_pFiller->SetVisible ( !bOverlay );
        m_pCommunityLabel->SetVisible ( true );

		for ( unsigned int i = 0; i < CORE_MTA_MENU_ITEMS; i++ )
        {
			if ( m_pItems[i] ) m_pItems[i]->SetVisible ( reinterpret_cast < int > ( m_pItems[i]->GetUserData () ) != ITEM_HIDDEN );
		}
	}
}


bool CMainMenu::IsVisible ( void )
{
    return m_bIsVisible;
}


void CMainMenu::SetIsIngame ( bool bIsIngame )
{
    // Save some performance by making sure we don't do all this if the new state == old
    if ( m_bIsIngame != bIsIngame )
    {
        m_bIsIngame = bIsIngame;

		// If we're in-game
		if ( bIsIngame ) {
			// Stop the video
			m_pMainMenuScene->StopVideo ();

			// Load and enable the static background
			SetStaticBackground ( true );

			// Enable the disconnect item
			EnableItem ( MENU_ITEM_DISCONNECT );
		} else {
			// Disable the disconnect item
			DisableItem ( MENU_ITEM_DISCONNECT, true );
		}

		// Remove the filler, if needed
		if ( m_bIsVisible )
			m_pFiller->SetVisible ( !bIsIngame );
    }
}


bool CMainMenu::GetIsIngame ( void )
{
    return m_bIsIngame;
}


void CMainMenu::SetServerBrowserVisible ( bool bVisible )
{
    m_ServerBrowser.SetVisible ( bVisible );
//    m_bIsInSubWindow = bVisible;
}


bool CMainMenu::OnQuickConnectButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

//    if ( !m_bIsInSubWindow )
    {
        m_QuickConnect.SetVisible ( true );
//        m_bIsInSubWindow = true;
    }

    return true;
}


bool CMainMenu::OnResumeButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

    SetVisible ( false );
    return true;
}


bool CMainMenu::OnBrowseServersButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

//    if ( !m_bIsInSubWindow )
    {
        m_ServerBrowser.SetVisible ( true );
//        m_bIsInSubWindow = true;
    }

    return true;
}


bool CMainMenu::OnDisconnectButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

    // Send "disconnect" command to the command handler
    CCommands::GetSingleton ().Execute ( "disconnect", "" );

    return true;
}


bool CMainMenu::OnHostGameButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

    // Load deathmatch, but with local play
    CModManager::GetSingleton ().RequestLoad ( "deathmatch", "local" );

    return true;
}


bool CMainMenu::OnEditorButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

    // Load deathmatch, but with local play
    CModManager::GetSingleton ().RequestLoad ( "deathmatch", "editor" );

    return true;
}


bool CMainMenu::OnSettingsButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

//    if ( !m_bIsInSubWindow )
    {
        m_Settings.SetVisible ( true );

        // Jax: not sure why this was commented, need it here if our main-menu is set to AlwaysOnTop
//        m_bIsInSubWindow = true;
    }  

    return true;
}


bool CMainMenu::OnAboutButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

	// Determine if we're ingame or if the background is set to static
	// If so, show the old credits dialog
	if ( m_bIsIngame || m_bStaticBackground ) {
		m_Credits.SetVisible ( true );
	} else {
		Hide ();
		m_bFadeToCredits = true;
		SetVisible ( false );
    }

    return true;
}


bool CMainMenu::OnQuitButtonClick ( CGUIElement* pElement )
{
	// Return if we haven't faded in yet
	if ( m_ucFade != FADE_VISIBLE ) return false;

    // Send "exit" command to the command handler
    CCommands::GetSingleton ().Execute ( "exit", "" );

    return true;
}


void CMainMenu::OnInvalidate ( IDirect3DDevice9 * pDevice )
{
	if ( !m_bInitialized ) return;

	if ( m_pRenderTarget && !m_bStaticBackground ) {
		delete m_pRenderTarget;
		m_pRenderTarget = NULL;
	}

	// Notify the scene class
	if ( !m_bStaticBackground )
		m_pMainMenuScene->OnInvalidate ( pDevice );
}


void CMainMenu::OnRestore ( IDirect3DDevice9 * pDevice )
{
	if ( !m_bInitialized ) return;

	//CVector2D ScreenSize = m_pManager->GetResolution ();
	D3DVIEWPORT9 Viewport;
	pDevice->GetViewport ( &Viewport );
	CVector2D ScreenSize(Viewport.Width,Viewport.Height);

	// If we use the dynamic background
	if ( !m_bStaticBackground )
    {
		// Recreate the render target
		m_pRenderTarget = m_pManager->CreateTexture ();
		m_pRenderTarget ->CreateTexture ( ScreenSize.fX, ScreenSize.fY );
		m_pBackground->LoadFromTexture ( m_pRenderTarget );

		// Notify the scene class
		if ( m_pMainMenuScene )
			m_pMainMenuScene->OnRestore ( pDevice, ScreenSize, m_pRenderTarget->GetD3DTexture () );
	}
}


bool CMainMenu::OnItemEnter ( CGUIElement* pElement )
{
	// If we're not fading, and if the element is enabled
	if ( m_ucFade == FADE_VISIBLE && reinterpret_cast < int > ( pElement->GetUserData() ) == ITEM_ENABLED )
		pElement->SetAlpha ( 0.8f );
	return true;
}


bool CMainMenu::OnItemLeave ( CGUIElement* pElement )
{
	// If we're not fading, and if the element is enabled
	if ( m_ucFade == FADE_VISIBLE && reinterpret_cast < int > ( pElement->GetUserData() ) == ITEM_ENABLED )
		pElement->SetAlpha ( 1.0f );
	return true;
}


void CMainMenu::CreateItem ( unsigned int uiIndex, CVector2D vecPosition, const char *szText, GUI_CALLBACK pHandler )
{
	CGUILabel * pItem = reinterpret_cast < CGUILabel* > ( m_pManager->CreateLabel ( m_pBackground, szText ) );

	// Set the position, font face and size
	pItem->SetPosition ( vecPosition );
	//pItem->SetSize ( CVector2D ( 200, 64 ) );
	pItem->SetFont ( "sans" );
	pItem->AutoSize ( szText );
	//pItem->BringToFront ();
    pItem->SetAlwaysOnTop ( true );
	pItem->SetVisible ( false );
	pItem->SetAlpha ( 0 );

	// Set the text color to white
	pItem->SetTextColor ( 255, 255, 255 );

	// Set the handlers
	pItem->SetClickHandler ( pHandler );
	pItem->SetMouseEnterHandler ( GUI_CALLBACK ( &CMainMenu::OnItemEnter, this ) );
	pItem->SetMouseLeaveHandler ( GUI_CALLBACK ( &CMainMenu::OnItemLeave, this ) );

	// Store the item in the array
	m_pItems[uiIndex] = pItem;
	m_uiItems++;

	// Enable the item
	EnableItem ( uiIndex );
}


void CMainMenu::SetStaticBackground ( bool bEnabled )
{
	if ( !m_bInitialized && bEnabled == m_bStaticBackground ) return;

	if ( bEnabled ) {
		// If we're initialized, we can load the background straight away
		if ( m_bInitialized ) {
			// Enable the static background
			m_pBackground->LoadFromFile ( CORE_MTA_STATIC_BG );
            m_pBackground->SetAlwaysOnTop ( true );
            m_pBackground->MoveToBack ();   // ChrML: Put it on top, but move it to the back of the "always on top" order
		
			// Destroy the dynamic scene
			m_pMainMenuScene->Destroy3DScene ();

			// Destroy the render target as we don't need it anymore
			if ( m_pRenderTarget ) {
				delete m_pRenderTarget;
				m_pRenderTarget = NULL;
			}
		} // Otherwise, Update will handle it on initialisation
	} else {
		// Enable the dynamic scene (force reinitialisation in Update)
		m_bInitialized = false;
	}
	m_bStaticBackground = bEnabled;

	// And disable the dynamic scene option (if we're not ingame, cause that means Init3DScene has failed)
	if (!m_bIsIngame) {
		DWORD dwSelect = (DWORD) !m_bStaticBackground;
        int iMenuOptions;
        CVARS_GET ( "menu_options", iMenuOptions );
		CVARS_SET ( "menu_options", (unsigned int)(( iMenuOptions & ~CMainMenu::eMenuOptions::MENU_DYNAMIC ) | ( dwSelect * CMainMenu::eMenuOptions::MENU_DYNAMIC )) );
	}
}


void CMainMenu::EnableItem ( unsigned int uiIndex )
{
	if ( m_pItems[uiIndex] ) {
		m_pItems[uiIndex]->SetUserData ( reinterpret_cast < void* > ( ITEM_ENABLED ) );
		m_pItems[uiIndex]->SetEnabled ( true );

		// Always show
		m_pItems[uiIndex]->SetVisible ( true );
	}
}


void CMainMenu::DisableItem ( unsigned int uiIndex, bool bHide )
{
	if ( m_pItems[uiIndex] ) {
		m_pItems[uiIndex]->SetEnabled ( false );

		// Hide if needed
		if ( bHide ) {
			m_pItems[uiIndex]->SetVisible ( false );
			m_pItems[uiIndex]->SetUserData ( reinterpret_cast < void* > ( ITEM_HIDDEN ) );
		} else {
			m_pItems[uiIndex]->SetUserData ( reinterpret_cast < void* > ( ITEM_DISABLED ) );
		}
	}
}

void CMainMenu::SetItemPosition ( unsigned int uiIndex, CVector2D vecPosition, bool bRelative )
{
	if ( m_pItems[uiIndex] ) 
    {
        m_pItems[uiIndex]->SetPosition ( vecPosition, bRelative );
	}
}



void CMainMenu::LoadMenuOptions ( void )
{
	int iMenuOptions;
    CVARS_GET ( "menu_options", iMenuOptions );
	
	// Reload the menu options, in case they have changed
	SetStaticBackground                            ( !(iMenuOptions & eMenuOptions::MENU_DYNAMIC) );            // Dynamic or static
	if ( !m_bStaticBackground ) {
		m_pMainMenuScene->SetPostProcessingEnabled ( (iMenuOptions & eMenuOptions::MENU_POSTEFFECTS_ENABLED) > 0 );   // Post-effects
		if ( !m_pMainMenuScene->SetVideoEnabled    ( (iMenuOptions & eMenuOptions::MENU_VIDEO_ENABLED) > 0 )) {       // Video
			// Fatal error, switch back to static
			SetStaticBackground ( true );
		}
	}
}


void CMainMenu::ChangeCommunityState ( bool bIn, std::string strUsername )
{
    if ( bIn )
    {
        SString strText ( "Logged in as: %s", strUsername.c_str () );

        m_pCommunityLabel->SetText ( strText );
	    m_pCommunityLabel->AutoSize ( strText );
        return;
    }
    m_pCommunityLabel->SetText ( "Not logged in" );
	m_pCommunityLabel->AutoSize ( "Not logged in" );
}