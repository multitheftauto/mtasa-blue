/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CSettings.cpp
*  PURPOSE:     In-game settings window
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Jax <>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <utils/CMD5Hasher.h>

#define CORE_SETTINGS_UPDATE_INTERVAL   30         // Settings update interval in frames
#define CORE_SETTINGS_HEADERS           3
#define CORE_SETTINGS_HEADER_GAME	    "GTA GAME CONTROLS"
#define CORE_SETTINGS_HEADER_MP         "MULTIPLAYER CONTROLS"
#define CORE_SETTINGS_HEADER_SPACER     " "
#define CORE_SETTINGS_NO_KEY			" "
#define CORE_SETTINGS_COMMUNITY_TEXT	\
	"Multi Theft Auto: Community is a voluntary community service offering everything\n" \
	"from third-party resources to user-based statistics.\n" \
	"\n" \
	"If you have already registered at community.mtasa.com,\n" \
	"you can fill in your account details below to complete your registration.\n" \
	"This computer will then automatically link to your personal account."

extern CCore* g_pCore;
extern SBindableGTAControl g_bcControls[];
extern SBindableKey g_bkKeys[];

CSettings::CSettings ( void )
{
	CGUITab *pTabMultiplayer, *pTabVideo, *pTabBinds, *pTabControls, *pTabCommunity;
    CGUI *pManager = g_pCore->GetGUI ();

    // Init
    m_bCaptureKey = false;
	m_dwFrameCount = 0;
    CVector2D vecTemp;

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "Settings" ) );
    m_pWindow->SetCloseButtonEnabled ( true );
    m_pWindow->SetMovable ( true );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    float yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 560.0f / 2, resolution.fY / 2 - 360.0f / 2 + yoff  ), false );

    //m_pWindow->SetPosition ( CVector2D ( 0.15f, 0.20f ), true );
    m_pWindow->SetSize ( CVector2D ( 560.0f, 360.0f ) );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );

	// Create the tab panel and necessary tabs
	m_pTabs = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( m_pWindow ) );
	m_pTabs->SetPosition ( CVector2D ( 0, 20.0f ) );
	m_pTabs->SetSize ( CVector2D ( 560.0f, 300.0f ) );
	pTabMultiplayer = m_pTabs->CreateTab ( "Multiplayer" );
	pTabVideo = m_pTabs->CreateTab ( "Video" );
	pTabBinds = m_pTabs->CreateTab ( "Binds" );
    pTabControls = m_pTabs->CreateTab ( "Controls" );
	pTabCommunity = m_pTabs->CreateTab ( "Community" );

    // Create buttons
    //  OK button
    m_pButtonOK = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "OK" ) );
    m_pButtonOK->SetPosition ( CVector2D ( 0.5f, 0.92f ), true );

    //  Cancel button
    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Cancel" ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 0.78f, 0.92f ), true );

	/**
	 *	Binds tab
	 **/
    m_pBindsList = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( pTabBinds, false ) );
    m_pBindsList->SetPosition ( CVector2D ( 0.02f, 0.05f ), true );
    m_pBindsList->SetSize ( CVector2D ( 520.0f, 225.0f ) );
	m_pBindsList->SetSorting ( false );
	m_pBindsList->SetSelectionMode ( CGUIGridList::SelectionMode::CellSingle );

	m_pBindsDefButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabBinds, "Load defaults" ) );
	m_pBindsDefButton->SetOnClickHandler ( GUI_CALLBACK ( &CSettings::OnBindsDefaultClick, this ) );
	m_pBindsDefButton->SetPosition ( CVector2D ( 402.0f, 245.0f ) );

    /**
	 *	Controls tab
	 **/
    // Toggles
    m_pControlsMouseLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, "Mouse options" ) );
    m_pControlsMouseLabel->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
	m_pControlsMouseLabel->AutoSize ( "Mouse options  " );
	m_pControlsMouseLabel->SetFont ( "default-bold-small" );

    m_pInvertMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabControls, "Invert mouse vertically", true ) );
    m_pInvertMouse->SetPosition ( CVector2D ( 0.022f, 0.1f ), true );
    m_pInvertMouse->GetPosition ( vecTemp, false );
    m_pInvertMouse->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    m_pSteerWithMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabControls, "Steer with mouse", true ) );
    m_pSteerWithMouse->SetPosition ( CVector2D ( 0.5f, 0.1f ), true );

    m_pFlyWithMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabControls, "Fly with mouse", true ) );
    m_pFlyWithMouse->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16 ) );

	m_hBind = m_pBindsList->AddColumn ( "DESCRIPTION", 0.35f );
    m_hPriKey = m_pBindsList->AddColumn ( "KEY", 0.24f );
    for ( int k = 0 ; k < SecKeyNum ; k++ )
    	m_hSecKeys[k] = m_pBindsList->AddColumn ( "ALT. KEY", 0.24f );

	/**
	 *	Community tab
	 **/
	m_pLabelCommunity = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabCommunity, CORE_SETTINGS_COMMUNITY_TEXT ) );
    m_pLabelCommunity->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
	m_pLabelCommunity->SetSize ( CVector2D ( 9.956f, 4.414f ), true );
	//m_pLabelCommunity->AutoSize ( CORE_SETTINGS_COMMUNITY_TEXT );

	m_pLabelUser = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabCommunity, "Username:" ) );
    m_pLabelUser->SetPosition ( CVector2D ( 0.022f, 0.46f ), true );
	m_pLabelUser->GetPosition ( vecTemp, false );
	m_pLabelUser->AutoSize ( "Username:" );

	m_pLabelPass = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabCommunity, "Password:" ) );
    m_pLabelPass->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
	m_pLabelPass->AutoSize ( "Password:" );

	m_pEditUser = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabCommunity ) );
    m_pEditUser->SetPosition ( CVector2D ( 0.16f, 0.45f ), true );
	m_pEditUser->GetPosition ( vecTemp, false );
    m_pEditUser->SetSize ( CVector2D ( 168.0f, 24.0f ) );
//    m_pEditUser->SetMaxLength ( 64 );

    m_pEditPass = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabCommunity ) );
    m_pEditPass->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
	m_pEditPass->GetPosition ( vecTemp, false );
    m_pEditPass->SetSize ( CVector2D ( 168.0f, 24.0f ) );
//    m_pEditPass->SetMaxLength ( 64 );
	m_pEditPass->SetMasked ( true );

	m_pButtonLogin = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabCommunity, "Login" ) );
	m_pButtonLogin->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
    m_pButtonLogin->GetPosition ( vecTemp, false );
	m_pButtonLogin->SetSize ( CVector2D ( 168.0f, 24.0f ) );

	m_pButtonRegister = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabCommunity, "Register" ) );
	m_pButtonRegister->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
	m_pButtonRegister->SetSize ( CVector2D ( 168.0f, 24.0f ) );

	/**
	 *	Multiplayer tab
	 **/
    m_pLabelNick = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabMultiplayer, "Nick:" ) );
    m_pLabelNick->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
	m_pLabelNick->AutoSize ( "Nick:" );

    // Nick edit
    m_pEditNick = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabMultiplayer ) );
    m_pEditNick->SetPosition ( CVector2D ( 0.10f, 0.04f ), true );
    m_pEditNick->SetSize ( CVector2D ( 168.0f, 24.0f ) );
    m_pEditNick->SetMaxLength ( 16 );

    // Video tab	
    /*
    m_pCheckBoxVideoWindowed = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Windowed mode", false ) );
    m_pCheckBoxVideoWindowed->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
	m_pCheckBoxVideoWindowed->SetSize ( CVector2D ( 224.0f, 16.0f ) );
    */

    m_pVideoRenderingLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Menu rendering options" ) );
    m_pVideoRenderingLabel->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
	m_pVideoRenderingLabel->AutoSize ( "Menu rendering options  " );
	m_pVideoRenderingLabel->SetFont ( "default-bold-small" );

    m_pCheckBoxMenuDynamic = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Dynamic scene rendering", true ) );
    m_pCheckBoxMenuDynamic->SetPosition ( CVector2D ( 0.022f, 0.1f ), true );
	m_pCheckBoxMenuDynamic->SetSize ( CVector2D ( 224.0f, 16.0f ) );
	m_pCheckBoxMenuDynamic->GetPosition ( vecTemp, false );
	m_pCheckBoxMenuDynamic->SetUserData ( reinterpret_cast < void* > ( eCheckBox::CHECKBOX_MENU_DYNAMIC ) );
	m_pCheckBoxMenuDynamic->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCheckBoxClick, this ) );


    m_pCheckBoxMenuVideo = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Video surface rendering", true ) );
	m_pCheckBoxMenuVideo->SetPosition ( CVector2D ( 0.5f, 0.1f ), true );
	m_pCheckBoxMenuVideo->SetSize ( CVector2D ( 224.0f, 20.0f ) );
	m_pCheckBoxMenuVideo->SetUserData ( reinterpret_cast < void* > ( eCheckBox::CHECKBOX_MENU_VIDEO ) );
	m_pCheckBoxMenuVideo->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCheckBoxClick, this ) );

    m_pCheckBoxMenuPostEffects = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "PS2.0 post-effects", true ) );
	m_pCheckBoxMenuPostEffects->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16 ) );
	m_pCheckBoxMenuPostEffects->SetSize ( CVector2D ( 224.0f, 16.0f ) );
	m_pCheckBoxMenuPostEffects->SetUserData ( reinterpret_cast < void* > ( eCheckBox::CHECKBOX_MENU_POSTEFFECTS ) );
	m_pCheckBoxMenuPostEffects->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCheckBoxClick, this ) );
	
    // Set up the events
    m_pButtonOK->SetOnClickHandler ( GUI_CALLBACK ( &CSettings::OnOKButtonClick, this ) );
    m_pButtonCancel->SetOnClickHandler ( GUI_CALLBACK ( &CSettings::OnCancelButtonClick, this ) );
	m_pButtonLogin->SetOnClickHandler ( GUI_CALLBACK ( &CSettings::OnLoginButtonClick, this ) );
    m_pButtonRegister->SetOnClickHandler ( GUI_CALLBACK ( &CSettings::OnRegisterButtonClick, this ) );
	/*
	// Give a warning if no community account settings were stored in config
	CCore::GetSingleton ().ShowMessageBox ( CORE_SETTINGS_COMMUNITY_WARNING, "Multi Theft Auto: Community settings", MB_ICON_WARNING );
	*/
}


CSettings::~CSettings ( void )
{
    // Destroy
    delete m_pButtonCancel;
    delete m_pButtonOK;
    delete m_pWindow;
}


bool CSettings::OnMouseDoubleClick ( CGUIMouseEventArgs Args )
{
	if ( Args.pWindow == m_pBindsList ) {
		OnBindsListClick ( m_pBindsList );
		return true;
	}
	return false;
}


void CSettings::Update ( void )
{
	// Once each 30 frames
	if ( m_dwFrameCount >= CORE_SETTINGS_UPDATE_INTERVAL ) {
		// Check if any of the settings have been changed by core
        int iMenuOptions;
        CVARS_GET ( "menu_options", iMenuOptions );
		bool bCompare = false;

		bCompare = (iMenuOptions & CMainMenu::eMenuOptions::MENU_DYNAMIC) > 0;
		if ( m_pCheckBoxMenuDynamic->GetSelected () != bCompare )       m_pCheckBoxMenuDynamic->SetSelected ( bCompare );
		bCompare = (iMenuOptions & CMainMenu::eMenuOptions::MENU_VIDEO_ENABLED) > 0;
		if ( m_pCheckBoxMenuVideo->GetSelected () != bCompare )         m_pCheckBoxMenuVideo->SetSelected ( bCompare );
		bCompare = (iMenuOptions & CMainMenu::eMenuOptions::MENU_POSTEFFECTS_ENABLED) > 0;
		if ( m_pCheckBoxMenuPostEffects->GetSelected () != bCompare )   m_pCheckBoxMenuPostEffects->SetSelected ( bCompare );

		m_dwFrameCount = 0;
	}
	m_dwFrameCount++;
}


bool CSettings::OnBindsDefaultClick ( CGUIElement* pElement )
{
	// Load the default binds
	CCore::GetSingleton ().GetKeyBinds ()->LoadDefaultBinds ();

    // Clear the binds list
	m_pBindsList->Clear ();

	// Re-initialize the binds list
	Initialize ();

	return true;
}

// Saves the keybinds
void CSettings::ProcessKeyBinds ( void )
{
	CKeyBindsInterface *pKeyBinds = CCore::GetSingleton ().GetKeyBinds ();

	// Loop through every row in the binds list
	for ( int i = 0; i < m_pBindsList->GetRowCount (); i++ )
	{
        // Get the type and keys
        unsigned char ucType = reinterpret_cast < unsigned char > ( m_pBindsList->GetItemData ( i, m_hBind ) );        
		char* szPri = m_pBindsList->GetItemText ( i, m_hPriKey );
		const SBindableKey* pPriKey = szPri ? pKeyBinds->GetBindableFromKey ( szPri ) : NULL;
 		const SBindableKey* pSecKeys[SecKeyNum];
        for ( int k = 0 ; k < SecKeyNum ; k++ )
        {
            char* szSec = m_pBindsList->GetItemText ( i, m_hSecKeys[k] );
            pSecKeys[k] = szSec ? pKeyBinds->GetBindableFromKey ( szSec ) : NULL;
        }
        
        // If the type is control
		if ( ucType == KEY_BIND_GTA_CONTROL )
        {            
            // Get the previous bind
            CGTAControlBind* pBind = reinterpret_cast < CGTAControlBind* > ( m_pBindsList->GetItemData ( i, m_hPriKey ) );
            if ( pBind )
            {
                // If theres a primary key, change it
                if ( pPriKey )
                    pBind->boundKey = pPriKey;
                else
                    // If not remove the bind
                    pKeyBinds->Remove ( pBind );
            }

            for ( int k = 0 ; k < SecKeyNum ; k++ )
            {
                CGTAControlBind* pSecBind = reinterpret_cast < CGTAControlBind* > ( m_pBindsList->GetItemData ( i, m_hSecKeys[k] ) );
                if ( pSecBind )
                {
                    // If theres a secondary key, change it
                    if ( pSecKeys[k] )
                    {
                        if ( pSecBind->boundKey != pSecKeys[k] )
                        {
                            if ( !pKeyBinds->GTAControlExists ( pSecKeys[k], pSecBind->control ) )
                            {
                                pSecBind->boundKey = pSecKeys[k];
                            }
                            else
                            {
                                // If not remove the bind
                                pKeyBinds->Remove ( pSecBind );
                            }
                        }
                    }
                    else
                    {
                        // If not remove the bind
                        pKeyBinds->Remove ( pSecBind );
                    }
                }
                else if ( pSecKeys[k] && pBind )
                {
                    SBindableGTAControl* pControl = pBind->control;
                    if ( !pKeyBinds->GTAControlExists ( pSecKeys[k], pControl ) )
                        pKeyBinds->AddGTAControl ( pSecKeys[k], pControl );
                }
            }

        }
        // If the type is an empty control (wasn't bound before)
        else if ( ucType == KEY_BIND_UNDEFINED )
        {
            // Grab the stored control
            SBindableGTAControl* pControl = reinterpret_cast < SBindableGTAControl* > ( m_pBindsList->GetItemData ( i, m_hPriKey ) );
            if ( pPriKey )
                // If theres a new key for primary, add a new bind
                pKeyBinds->AddGTAControl ( pPriKey, pControl );
            for ( int k = 0 ; k < SecKeyNum ; k++ )
                if ( pSecKeys[k] )
                 // If theres a new key for secondary, add a new bind
                    pKeyBinds->AddGTAControl ( pSecKeys[k], pControl );
        }
        // If the type is a command
        else if ( ucType == KEY_BIND_COMMAND )
        {
            char* szCmdArgs = m_pBindsList->GetItemText ( i, m_hBind );
            char* szCommand = strtok ( szCmdArgs, ":" );
            char* szArguments = strtok ( NULL, "\0" );
            if ( szArguments )
                szArguments = &szArguments [ 1 ];

			/** Primary keybinds **/
            CCommandBind* pBind = reinterpret_cast < CCommandBind* > ( m_pBindsList->GetItemData ( i, m_hPriKey ) );
			// If a keybind for this command already exists
            if ( pBind )
            {
				// If the user specified a valid primary key
                if ( pPriKey )
                {
					// If the primary key is different than the original one
					if ( pPriKey != pBind->boundKey ) {
						pBind->boundKey = pPriKey;
					}
                }
				// If the primary key field was empty, we can remove the keybind
				else {
                    pKeyBinds->Remove ( pBind );
				}
            }
			// If there was no keybind for this command, create it
            else if ( pPriKey )
            {
				pKeyBinds->AddCommand ( pPriKey, szCommand, szArguments );
            }

			/** Secondary keybinds **/
            for ( int k = 0 ; k < SecKeyNum ; k++ )
            {
                pBind = reinterpret_cast < CCommandBind* > ( m_pBindsList->GetItemData ( i, m_hSecKeys[k] ) );
			    // If this is a valid bind in the keybinds list
                if ( pBind )
                {
				    // And our secondary key field was not empty
                    if ( pSecKeys[k] )
                    {
					    if ( pSecKeys[k] != pBind->boundKey )
                            pBind->boundKey = pSecKeys[k];
                    }
				    // If the secondary key field was empty, we should remove the keybind
				    else
                        pKeyBinds->Remove ( pBind );
                }
			    // If this key bind didn't exist, create it
                else if ( pSecKeys[k] )
				    pKeyBinds->AddCommand ( pSecKeys[k], szCommand, szArguments );
            }
        }
        else if ( ucType == KEY_BIND_FUNCTION ) // keys bound to script functions
        {
            /** Primary keybinds **/
            CKeyFunctionBind* pBind = reinterpret_cast < CKeyFunctionBind* > ( m_pBindsList->GetItemData ( i, m_hPriKey ) );
            CKeyFunctionBind* pSecondBind = NULL; // some binds have a "up" bind too, which we hide from the user
			// If a keybind for this command already exists
            if ( pBind )
            {
                list < CKeyBind* > ::const_iterator iterr = pKeyBinds->IterBegin ();
                for ( unsigned int uiIndex = 0 ; iterr != pKeyBinds->IterEnd (); iterr++, uiIndex++ )
                {
                    eKeyBindType bindType = (*iterr)->GetType ();
                    if ( (*iterr)->GetType () == KEY_BIND_FUNCTION )
                    {
                        CKeyFunctionBind* pBindItem = reinterpret_cast < CKeyFunctionBind* > ( (*iterr) );
                        if ( pBindItem->boundKey == pBind->boundKey && pBindItem->bHitState == !pBind->bHitState )
                        {
                            pSecondBind = pBindItem;
                            break;
                        }
                    }
                }
				// If the user specified a valid primary key
                if ( pPriKey )
                {
					// If the primary key is different than the original one
					if ( pPriKey != pBind->boundKey ) {
						pBind->boundKey = pPriKey;
                        if ( pSecondBind )
                            pSecondBind->boundKey = pPriKey;
					}
                }
				// If the primary key field was empty, we can remove the keybind
				else {
                    pKeyBinds->Remove ( pBind );
                    pKeyBinds->Remove ( pSecondBind );
				}
            }
			// If there was no keybind for this command, create it
            else if ( pPriKey )
            {
                pKeyBinds->AddFunction ( pPriKey, pBind->Handler, pBind->bHitState, pBind->bIgnoreGUI );
            }

            CKeyFunctionBind* pPrimaryBind = pBind;
			/** Secondary keybinds **/
            for ( int k = 0 ; k < SecKeyNum ; k++ )
            {
                pBind = reinterpret_cast < CKeyFunctionBind* > ( m_pBindsList->GetItemData ( i, m_hSecKeys[k] ) );
			    // If this is a valid bind in the keybinds list
                if ( pBind )
                {
                    CKeyFunctionBind* pSecondSecondaryBind = NULL;
                    list < CKeyBind* > ::const_iterator iterr = pKeyBinds->IterBegin ();
                    for ( unsigned int uiIndex = 0 ; iterr != pKeyBinds->IterEnd (); iterr++, uiIndex++ )
                    {
                        eKeyBindType bindType = (*iterr)->GetType ();
                        if ( (*iterr)->GetType () == KEY_BIND_FUNCTION )
                        {
                            CKeyFunctionBind* pBindItem = reinterpret_cast < CKeyFunctionBind* > ( (*iterr) );
                            if ( pBindItem->boundKey == pBind->boundKey && pBindItem->bHitState == !pBind->bHitState )
                            {
                                if ( pSecondBind != pBindItem ) // don't want to find the same bind we found for the primary one
                                {
                                    pSecondSecondaryBind = pBindItem;
                                    break;
                                }
                            }
                        }
                    }

				    // And our secondary key field was not empty
                    if ( pSecKeys[k] )
                    {
					    if ( pSecKeys[k] != pBind->boundKey )
                        {
                            pBind->boundKey = pSecKeys[k];
                            if ( pSecondSecondaryBind )
                                pSecondSecondaryBind->boundKey = pSecKeys[k];
                        }
                    }
				    // If the secondary key field was empty, we should remove the keybind
				    else
                    {
                        pKeyBinds->Remove ( pBind );
                        pKeyBinds->Remove ( pSecondSecondaryBind );
                    }
                }
			    // If this key bind didn't exist, create it
                else if ( pSecKeys[k] && pPrimaryBind )
                {
                    pKeyBinds->AddFunction ( pSecKeys[k], pPrimaryBind->Handler, pPrimaryBind->bHitState, pPrimaryBind->bIgnoreGUI );
                    if ( pSecondBind ) // if the primary key bind had a second bind, we want one too!
                    {
                        pKeyBinds->AddFunction ( pSecKeys[k], pSecondBind->Handler, pSecondBind->bHitState, pSecondBind->bIgnoreGUI );
                    }
                }
            }
        }
	}
}


bool CSettings::OnBindsListClick ( CGUIElement* pElement )
{
	CGUIListItem *pItem = m_pBindsList->GetSelectedItem ();
    if ( pItem )
    {
	    CGUIListItem *pItemBind = m_pBindsList->GetItem ( m_pBindsList->GetItemRowIndex ( pItem ), m_hBind );
	    char szText [ 128 ];

	    // Proceed if the user didn't select the "Bind"-column
	    if ( pItem != pItemBind )
	    {
		    m_uiCaptureKey = 0;
		    m_pSelectedBind = pItem;
            m_bCaptureKey = true;

		    // Determine if the primary or secondary column was selected
		    if ( m_pBindsList->GetItemColumnIndex ( pItem ) == 1/*m_hPriKey  Note: handle is not the same as index */ ) {
			    // Create a messagebox to notify the user
			    //sprintf ( szText, "Press a key to bind to '%s'", pItemBind->GetText ().c_str() );
			    sprintf ( szText, "Press a key to bind, or escape to clear" );
			    CCore::GetSingleton ().ShowMessageBox ( "Binding a primary key", szText, MB_ICON_QUESTION );
		    } else {
			    // Create a messagebox to notify the user
			    //sprintf ( szText, "Press a key to bind to '%s'", pItemBind->GetText ().c_str() );
			    sprintf ( szText, "Press a key to bind, or escape to clear" );
			    CCore::GetSingleton ().ShowMessageBox ( "Binding a secondary key", szText, MB_ICON_QUESTION );
		    }
	    }
    }

	return true;
}

#ifndef WM_XBUTTONDOWN
#define WM_XBUTTONDOWN 0x020B
#endif
#ifndef WM_XBUTTONUP
#define WM_XBUTTONUP 0x020C
#endif

bool CSettings::ProcessMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if ( m_bCaptureKey )
    {        
	    CKeyBindsInterface *pKeyBinds = CCore::GetSingleton ().GetKeyBinds ();

        if ( uMsg != WM_KEYDOWN && uMsg != WM_KEYUP && uMsg != WM_SYSKEYDOWN &&
             uMsg != WM_SYSKEYUP && uMsg != WM_LBUTTONDOWN && uMsg != WM_LBUTTONUP &&
             uMsg != WM_RBUTTONDOWN && uMsg != WM_RBUTTONUP && uMsg != WM_MBUTTONDOWN &&
             uMsg != WM_MBUTTONUP && uMsg != WM_XBUTTONDOWN && uMsg != WM_XBUTTONUP &&
             uMsg != WM_MOUSEWHEEL )
        {
            return false;
        }
        
        // Ignore the first key, which is the one that initiated the capture process
	    if ( m_uiCaptureKey == 0 )
        {
		    m_uiCaptureKey++;
		    return false;
	    }

        bool bState;
        const SBindableKey* pKey = pKeyBinds->GetBindableFromMessage ( uMsg, wParam, lParam, bState );
        
        // If escape was pressed, don't create a bind
        if ( uMsg == WM_KEYDOWN && wParam == VK_ESCAPE )
		{
			m_pSelectedBind->SetText ( CORE_SETTINGS_NO_KEY );
		}
        else if ( pKey )
        {
            m_pSelectedBind->SetText ( pKey->szKey );
	    }
        else
        {
            return false;
        }

	    // Remove the messagebox we created earlier
	    CCore::GetSingleton ().RemoveMessageBox ();

	    // Make sure the list gets redrawed/updated
	    m_pBindsList->Activate ();

        m_bCaptureKey = false;
        return true;
    }
    return false;
}


void CSettings::Initialize ( void )
{
	// Add binds and sections
	bool bPrimaryKey = true;
	int iBind = 0, iRowGame;

    // Add the rows
	CKeyBinds* pKeyBinds = reinterpret_cast < CKeyBinds* > ( CCore::GetSingleton ().GetKeyBinds () );
	iRowGame = m_pBindsList->AddRow ();
	m_pBindsList->SetItemText ( iRowGame, m_hBind, CORE_SETTINGS_HEADER_GAME, false, true );
	m_pBindsList->SetItemText ( m_pBindsList->AddRow (), m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true );
	m_pBindsList->SetItemText ( m_pBindsList->AddRow (), m_hBind, CORE_SETTINGS_HEADER_MP, false, true );
    //iRows = CORE_SETTINGS_HEADERS;			// (game keys), (multiplayer keys)
    int iGameRowCount = 1;
    int iMultiplayerRowCount = 2;

    list < SKeyBindSection * > ::const_iterator iters = m_pKeyBindSections.begin();
    for ( ; iters != m_pKeyBindSections.end(); iters++ )
    {
        (*iters)->currentIndex = -1;
        (*iters)->rowCount = 0;
    }
 
    // Loop through all the available controls
	int i;
    for ( i = 0 ; *g_bcControls [ i ].szControl != NULL ; i++ );
    for ( i-- ; i >= 0 ; i-- )
    {
        SBindableGTAControl* pControl = &g_bcControls [ i ];
        
        // Loop through the binds for a matching control
        unsigned int uiMatchCount = 0;
        list < CKeyBind* > ::const_iterator iter = pKeyBinds->IterBegin ();
        for ( ; iter != pKeyBinds->IterEnd (); iter++ )
        {
            // Is it a control bind
            if ( (*iter)->GetType () == KEY_BIND_GTA_CONTROL )
            {
                CGTAControlBind* pBind = reinterpret_cast < CGTAControlBind* > ( *iter );
                if ( pBind->control == pControl )
                {                
                    // Primary key?
                    if ( uiMatchCount == 0 )
                    {
                        // Add bind to the list
				        iBind = m_pBindsList->InsertRowAfter ( iRowGame );
				        m_pBindsList->SetItemText ( iBind, m_hBind, pControl->szDescription );
				        m_pBindsList->SetItemText ( iBind, m_hPriKey, pBind->boundKey->szKey );
			            for ( int k = 0 ; k < SecKeyNum ; k++ )
                            m_pBindsList->SetItemText ( iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY );
				        m_pBindsList->SetItemData ( iBind, m_hBind, (void*) KEY_BIND_GTA_CONTROL );
                        m_pBindsList->SetItemData ( iBind, m_hPriKey, pBind );
                        iGameRowCount++;
                    }
                    // Secondary keys?
                    else
 			            for ( int k = 0 ; k < SecKeyNum ; k++ )
                            if ( uiMatchCount == k+1 )
                            {
                                m_pBindsList->SetItemText ( iBind, m_hSecKeys[k], pBind->boundKey->szKey );
                                m_pBindsList->SetItemData ( iBind, m_hSecKeys[k], pBind );
                            }
                    uiMatchCount++;
                }
            }
        }
        // If we didnt find any matches
        if ( uiMatchCount == 0 )
        {
            iBind = m_pBindsList->InsertRowAfter ( iRowGame );
			m_pBindsList->SetItemText ( iBind, m_hBind, pControl->szDescription );
			m_pBindsList->SetItemText ( iBind, m_hPriKey, CORE_SETTINGS_NO_KEY );
 	        for ( int k = 0 ; k < SecKeyNum ; k++ )
    			m_pBindsList->SetItemText ( iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY );
			m_pBindsList->SetItemData ( iBind, m_hBind, (void*) KEY_BIND_UNDEFINED );
            m_pBindsList->SetItemData ( iBind, m_hPriKey, pControl );
            iGameRowCount++;
        }
    }

    struct SListedCommand
    {
        int iIndex;
        CKeyBind* pBind;
        unsigned int uiMatchCount;
    };

    SListedCommand* listedCommands = new SListedCommand [ pKeyBinds->Count ( KEY_BIND_COMMAND ) + pKeyBinds->Count ( KEY_BIND_FUNCTION ) ];
    unsigned int uiNumListedCommands = 0;
    // Loop through all the bound commands
    list < CKeyBind* > ::const_iterator iter = pKeyBinds->IterBegin ();
    for ( unsigned int uiIndex = 0 ; iter != pKeyBinds->IterEnd (); iter++, uiIndex++ )
    {
        eKeyBindType bindType = (*iter)->GetType ();
        // keys bound to a console command or a function (we don't show keys bound 
        // from gta controls by scripts as these are clearly not user editable)
        if ( bindType == KEY_BIND_COMMAND || bindType == KEY_BIND_FUNCTION )
        {
            CKeyBindWithState * pBind = reinterpret_cast < CKeyBindWithState* > ( *iter );
            if ( pBind->bHitState )
            {
                bool bFoundMatches = false;
                // Loop through the already listed array of commands for matches
                for ( unsigned int i = 0 ; i < uiNumListedCommands ; i++ )
                {
                    SListedCommand* pListedCommand = &listedCommands [ i ];
                    CKeyBind* pListedBind = pListedCommand->pBind;
                    bool bMatched = false;
                    if ( pListedBind->GetType() == pBind->GetType() )
                    {
                        // Jax: TODO: rewrite? :S
                    }

                    // If we found a 1st match, add it to the secondary section
                    if ( bMatched )
                    {
                        bFoundMatches = true;
             	        for ( int k = 0 ; k < SecKeyNum ; k++ )
                            if ( pListedCommand->uiMatchCount == k )
                            {
                                m_pBindsList->SetItemText ( pListedCommand->iIndex, m_hSecKeys[k], pBind->boundKey->szKey );
                                m_pBindsList->SetItemData ( pListedCommand->iIndex, m_hSecKeys[k], pBind );
                            }
                        pListedCommand->uiMatchCount++;
                    }
                }

                // If there weren't any matches
                if ( !bFoundMatches )
                {
                    unsigned int row = iGameRowCount + 1;
                    // Combine command and arguments
                    char* szDescription = NULL;
                    bool bDeleteDescription = true;
                    bool bSkip = true;
                    if ( bindType == KEY_BIND_COMMAND )
                    {
                        CCommandBind* pCommandBind = reinterpret_cast < CCommandBind* > ( *iter );
                        if ( pCommandBind->szArguments && pCommandBind->szArguments[0] != '\0' )
                        {
                            szDescription = new char [ strlen ( pCommandBind->szCommand ) + strlen ( pCommandBind->szArguments ) + 5 ];
                            sprintf ( szDescription, "%s: %s", pCommandBind->szCommand, pCommandBind->szArguments );
                        }
                        else
                        {
                            szDescription = pCommandBind->szCommand;
                            bDeleteDescription = false;
                        }
                        iMultiplayerRowCount++;
                        bSkip = false;
                    }                    

                    if ( !bSkip )
                    {
				        // Add the bind to the list
				        iBind = m_pBindsList->InsertRowAfter ( row );
                        m_pBindsList->SetItemText ( iBind, m_hBind, szDescription );
				        m_pBindsList->SetItemText ( iBind, m_hPriKey, pBind->boundKey->szKey );
                        for ( int k = 0 ; k < SecKeyNum ; k++ )
                            m_pBindsList->SetItemText ( iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY );
                        m_pBindsList->SetItemData ( iBind, m_hBind, (void*) bindType );
                        m_pBindsList->SetItemData ( iBind, m_hPriKey, pBind );
                        if ( bDeleteDescription )
                            delete [] szDescription;

                        // Add it to the already-listed array
                        SListedCommand* pListedCommand = &listedCommands [ uiNumListedCommands ];
                        pListedCommand->iIndex = iBind;
                        pListedCommand->pBind = pBind;
                        pListedCommand->uiMatchCount = 0;
                        uiNumListedCommands++;
                    }
                }
            }
        }
    }
    delete [] listedCommands;
}


void CSettings::SetVisible ( bool bVisible )
{
    // Hide / show the form
    m_pWindow->SetVisible ( bVisible );

    // Load the config file if the dialog is shown
    if ( bVisible )
    {
        m_pWindow->Activate ();
        LoadData ();

        // Clear the binds list
	    m_pBindsList->Clear ();

	    // Re-initialize the binds list
	    Initialize ();
    }
}


bool CSettings::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}


bool CSettings::OnCheckBoxClick ( CGUIElement* pElement )
{
	// Can't cast directly to CGUICheckBox (or check for matching pointers),
	// because the vtables don't align (pElement is off by +4).
	// So use eCheckBox.

	eCheckBox CheckBoxId = (eCheckBox) reinterpret_cast < int > ( pElement->GetUserData () );

    int iMenuOptions;
    CVARS_GET ( "menu_options", iMenuOptions );

	do {
		// [Menu rendering options - Dynamic scene rendering]
		if ( CheckBoxId == eCheckBox::CHECKBOX_MENU_DYNAMIC ) {
			DWORD dwSelect = (DWORD) m_pCheckBoxMenuDynamic->GetSelected ();
			CVARS_SET ( "menu_options", (unsigned int)(( iMenuOptions & ~CMainMenu::eMenuOptions::MENU_DYNAMIC ) | ( dwSelect * CMainMenu::eMenuOptions::MENU_DYNAMIC )) );
			break;
		}

		// [Menu rendering options - Video surface rendering]
		else if ( CheckBoxId == eCheckBox::CHECKBOX_MENU_VIDEO ) {
			DWORD dwSelect = (DWORD) m_pCheckBoxMenuVideo->GetSelected ();
			CVARS_SET ( "menu_options", (unsigned int)(( iMenuOptions & ~CMainMenu::eMenuOptions::MENU_VIDEO_ENABLED ) | ( dwSelect * CMainMenu::eMenuOptions::MENU_VIDEO_ENABLED )) );
			break;
		}

		// [Menu rendering options - PS2.0 post-effects]
		else if ( CheckBoxId == eCheckBox::CHECKBOX_MENU_POSTEFFECTS ) {
			DWORD dwSelect = (DWORD) m_pCheckBoxMenuPostEffects->GetSelected ();
			CVARS_SET ( "menu_options", (unsigned int)(( iMenuOptions & ~CMainMenu::eMenuOptions::MENU_POSTEFFECTS_ENABLED ) | ( dwSelect * CMainMenu::eMenuOptions::MENU_POSTEFFECTS_ENABLED )) );
			break;
		}

		// No valid candidates
		return false;
	} while ( false );

	// Let the menu load the new values
	CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->LoadMenuOptions ();

	return true;
}


bool CSettings::OnOKButtonClick ( CGUIElement* pElement )
{
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

	// Process keybinds
	ProcessKeyBinds ();
	
    // Invalid nickname?
    if ( !CCore::GetSingleton ().IsValidNick ( m_pEditNick->GetText ().c_str () ) )
    {
		CCore::GetSingleton ().ShowMessageBox ( "Error", "Your nickname contains invalid characters!", MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }

    // Save the config
    SaveData ();

    // Close the window
    m_pWindow->SetVisible ( false );
    pMainMenu->m_bIsInSubWindow = false;

	return true;
}


bool CSettings::OnCancelButtonClick ( CGUIElement* pElement )
{
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    m_pWindow->SetVisible ( false );
    pMainMenu->m_bIsInSubWindow = false;

    return true;
}


bool CSettings::OnLoginButtonClick ( CGUIElement* pElement )
{
    if ( strcmp ( m_pButtonLogin->GetText().c_str(), "Login" ) == 0 )
    {
        if ( !m_pEditUser->GetText().empty() &&
            !m_pEditPass->GetText().empty() )
        {
            // Hash password
            char szPassword[33];
            std::string strPassword;
	        MD5 Password;
	        CMD5Hasher Hasher;
	        Hasher.Calculate ( m_pEditPass->GetText ().c_str(), m_pEditPass->GetText().length(), Password );
	        Hasher.ConvertToHex ( Password, szPassword );
            strPassword = std::string ( szPassword );

            // Check if we need to use the stored password
            std::string strCommunityPassword;
            CVARS_GET ( "community_password", strCommunityPassword );
            if ( m_pEditPass->GetText ().compare ( strCommunityPassword ) == 0 )
                strPassword = strCommunityPassword;

            // Store the user/pass and log in using community
            CCommunity *pCommunity = CCommunity::GetSingletonPtr ();
            pCommunity->SetUsername ( m_pEditUser->GetText () );
            pCommunity->SetPassword ( strPassword );
	        CVARS_SET ( "community_username", m_pEditUser->GetText () );
            CVARS_SET ( "community_password", strPassword );
            pCommunity->Login ( OnLoginCallback, this );
        }
        return false;
    }
    else
    {
        CCommunity::GetSingleton ().Logout ();
    }
    return true;
}


bool CSettings::OnRegisterButtonClick ( CGUIElement* pElement )
{
    g_pCore->GetLocalGUI()->GetCommunityRegistration()->Open ();
    return true;
}


void CSettings::OnLoginCallback ( bool bResult, char* szError, void* obj )
{   // This callback function is called by CCommunity whenever an error has occurred
    if ( !bResult )
    {
        g_pCore->ShowMessageBox ( "Serial Error", szError, MB_BUTTON_OK | MB_ICON_ERROR );
    }
}


void CSettings::OnLoginStateChange ( bool bResult )
{   // This function is called by CCommunity whenever the logged-in status changes
    m_pEditUser->SetEnabled ( !bResult );
    m_pEditPass->SetEnabled ( !bResult );
    m_pButtonLogin->SetText ( ( bResult ) ? "Logout" : "Login" );
    m_pButtonRegister->SetVisible ( !bResult );
}


void CSettings::LoadData ( void )
{
    std::string strVar;
    bool bVar;

    // Put the nick in the edit
    CVARS_GET ( "nick", strVar );

    if ( CCore::GetSingleton ().IsValidNick ( strVar.c_str () ) )
    {
        m_pEditNick->SetText ( strVar.c_str () );
    }
    else
    {
        m_pEditNick->SetText ( "Player" );
    }

    CVARS_GET ( "invert_mouse", bVar ); m_pInvertMouse->SetSelected ( bVar );
    CVARS_GET ( "steer_with_mouse", bVar ); m_pSteerWithMouse->SetSelected ( bVar );
    CVARS_GET ( "fly_with_mouse", bVar ); m_pFlyWithMouse->SetSelected ( bVar );

	// Community
    CVARS_GET ( "community_username", strVar );
	if ( !strVar.empty () ) m_pEditUser->SetText ( strVar.c_str () );
    CVARS_GET ( "community_password", strVar );
	if ( !strVar.empty () ) m_pEditPass->SetText ( strVar.c_str () );
}


void CSettings::SaveData ( void )
{
    std::string strVar;

	// Set and save our settings
    if ( CModManager::GetSingleton ().GetCurrentMod () != NULL )
    {
        CVARS_GET ( "nick", strVar );
        if ( m_pEditNick->GetText ().compare ( strVar ) != 0 )
	        CCore::GetSingleton ().GetCommands ()->Execute ( "nick", m_pEditNick->GetText ().c_str () );
    }
    else
    {
        CVARS_SET ( "nick", m_pEditNick->GetText () );
    }

    // Very hacky
    CControllerConfigManager * pController = g_pCore->GetGame ()->GetControllerConfigManager ();    
    CVARS_SET ( "invert_mouse", m_pInvertMouse->GetSelected () );
    pController->SetMouseInverted ( m_pInvertMouse->GetSelected () );
    CVARS_SET ( "steer_with_mouse", m_pSteerWithMouse->GetSelected () );
    pController->SetSteerWithMouse ( m_pSteerWithMouse->GetSelected () );
    CVARS_SET ( "fly_with_mouse", m_pFlyWithMouse->GetSelected () );
    pController->SetFlyWithMouse ( m_pFlyWithMouse->GetSelected () );

    CCore::GetSingleton ().SaveConfig ();
}

void CSettings::RemoveKeyBindSection ( char * szSectionName )
{
    list < SKeyBindSection* >::iterator iter = m_pKeyBindSections.begin();
    for ( ; iter != m_pKeyBindSections.end(); iter++ )
    {
        if ( strcmp ( (*iter)->szOriginalTitle, szSectionName ) == 0 )
        {
			delete *iter;
            m_pKeyBindSections.erase ( iter );
            break;
        }
    }
}

void CSettings::RemoveAllKeyBindSections ( void )
{
    list < SKeyBindSection* >::const_iterator iter = m_pKeyBindSections.begin();
    for ( ; iter != m_pKeyBindSections.end(); iter++ )
    {
        SKeyBindSection * section = (*iter);
        delete section;
    }
    m_pKeyBindSections.clear();
}

void CSettings::AddKeyBindSection ( char* szSectionName )
{
    m_pKeyBindSections.push_back ( new SKeyBindSection ( szSectionName ) ); 
}