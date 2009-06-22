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
*               Marcus Bauer <mabako@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"
#include <game/CGame.h>
#include <utils/CMD5Hasher.h>

using namespace std;

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
    CGUITab *pTabMultiplayer, *pTabVideo, *pTabAudio, *pTabBinds, *pTabControls, *pTabCommunity, *pTabChat;
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
    pTabAudio = m_pTabs->CreateTab ( "Audio" );
	pTabBinds = m_pTabs->CreateTab ( "Binds" );
    pTabControls = m_pTabs->CreateTab ( "Controls" );
	pTabCommunity = m_pTabs->CreateTab ( "Community" );
    pTabChat = m_pTabs->CreateTab ( "Chatbox" );

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
	m_pBindsDefButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnBindsDefaultClick, this ) );
	m_pBindsDefButton->SetPosition ( CVector2D ( 402.0f, 245.0f ) );

    /**
	 *	Controls tab
	 **/
    // Toggles
    //Create everything under a scrollpane
#if 0   // Keep this puppy handy 'case we need it again
    CGUIScrollPane* m_pControlsScrollPane = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( pTabControls ) ); 
    m_pControlsScrollPane->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pControlsScrollPane->SetProperty( "ContentArea", "l:0.000000 t:0.000000 r:0.000000 b:395.000000" ); //Defines the height of hte content
    m_pControlsScrollPane->SetPosition ( CVector2D ( 0.0f, 0.0f ), true );
    m_pControlsScrollPane->SetSize ( CVector2D ( 1.0f, 1.0f ), true );
    m_pControlsScrollPane->SetVerticalScrollStepSize ( 0.15f );
    m_pControlsScrollPane->SetVerticalScrollBar(true);
    CGUIElement* pControlsPane = m_pControlsScrollPane;
#else
    CGUIElement* pControlsPane = pTabControls;
#endif

    //Mouse Options
    m_pControlsMouseLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Mouse options" ) );
    m_pControlsMouseLabel->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
	m_pControlsMouseLabel->AutoSize ( "Mouse options  " );
	m_pControlsMouseLabel->SetFont ( "default-bold-small" );

    m_pInvertMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pControlsPane, "Invert mouse vertically", true ) );
    m_pInvertMouse->SetPosition ( CVector2D ( 0.022f, 0.1f ), true );
    m_pInvertMouse->GetPosition ( vecTemp, false );
    m_pInvertMouse->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    m_pSteerWithMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pControlsPane, "Steer with mouse", true ) );
    m_pSteerWithMouse->SetPosition ( CVector2D ( 0.5f, 0.1f ), true );

    m_pFlyWithMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pControlsPane, "Fly with mouse", true ) );
    m_pFlyWithMouse->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16 ) );

    //Joypad options
    m_pControlsJoypadLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Joypad options" ) );
    m_pControlsJoypadLabel->SetPosition ( CVector2D ( 0.022f, 0.27f ), true );
	m_pControlsJoypadLabel->AutoSize ( "Joypad options  " );
	m_pControlsJoypadLabel->SetFont ( "default-bold-small" );

    //Create a mini-scrollpane for the radio buttons (only way to group them together)
    m_pControlsInputTypePane = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( pControlsPane ) ); 
    m_pControlsInputTypePane->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pControlsInputTypePane->SetPosition ( CVector2D ( 0.0f, 0.327f ), true );
    m_pControlsInputTypePane->SetSize ( CVector2D ( 1.0f, 0.27f ), true );

    m_pStandardControls = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pControlsInputTypePane, "Standard controls (Mouse + Keyboard)" ) ); 
    m_pStandardControls->SetSelected ( true );
    m_pStandardControls->SetPosition ( CVector2D ( 0.022f, 0.0f ), true );
    m_pStandardControls->SetSize ( CVector2D ( 240.0f, 15.0f ), false );

    m_pClassicControls = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pControlsInputTypePane, "Classic controls (Joypad)" ) ); 
    m_pClassicControls->SetPosition ( CVector2D ( 0.5f, 0.0f ), true );
    m_pClassicControls->SetSize ( CVector2D ( 270.0f, 15.0f ), false );


    //Advanced Joypad settings
    {
        m_JoypadSettingsRevision = -1;

        CJoystickManagerInterface* JoyMan = GetJoystickManager ();

        m_pJoypadName = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane ) );
        m_pJoypadName->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
        m_pJoypadName->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

        m_pJoypadUnderline = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane ) );
        m_pJoypadUnderline->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
        m_pJoypadUnderline->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

        m_pEditDeadzone = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pControlsPane ) );
        m_pEditDeadzone->SetPosition ( CVector2D ( 0.02f, 0.567f ), true );
        m_pEditDeadzone->SetSize ( CVector2D ( 45.0f, 24.0f ) );
        m_pEditDeadzone->SetMaxLength ( 3 );
        m_pEditDeadzone->SetTextChangedHandler ( GUI_CALLBACK ( &CSettings::OnJoypadTextChanged, this ) );

        m_pEditSaturation = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pControlsPane ) );
        m_pEditSaturation->SetPosition ( CVector2D ( 0.02f, 0.667f ), true );
        m_pEditSaturation->SetSize ( CVector2D ( 45.0f, 24.0f ) );
        m_pEditSaturation->SetMaxLength ( 3 );
        m_pEditSaturation->SetTextChangedHandler ( GUI_CALLBACK ( &CSettings::OnJoypadTextChanged, this ) );

        CGUILabel* pLabelDeadZone = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Dead Zone" ) );
        pLabelDeadZone->SetPosition ( m_pEditDeadzone->GetPosition () + CVector2D ( 52.f, -1.f ) );
	    pLabelDeadZone->SetSize ( CVector2D ( 68.0f, 24.0f ) );
        pLabelDeadZone->SetVerticalAlign( CGUI_ALIGN_VERTICALCENTER );

        CGUILabel* pLabelSaturation = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Saturation" ) );
        pLabelSaturation->SetPosition ( m_pEditSaturation->GetPosition () + CVector2D ( 52.f, -1.f ) );
	    pLabelSaturation->SetSize ( CVector2D ( 68.0f, 24.0f ) );
        pLabelSaturation->SetVerticalAlign( CGUI_ALIGN_VERTICALCENTER );

        CGUIButton*  pJoyDefButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pControlsPane, "Load defaults" ) );
	    pJoyDefButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnJoypadDefaultClick, this ) );
	    pJoyDefButton->SetPosition ( CVector2D ( 0.015f, 0.830f ), true );

        CGUILabel* pLabelHelp = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Use the 'Binds' tab for joypad buttons." ) );
        pLabelHelp->SetPosition ( CVector2D ( 0.02f, 0.91f ), true );
	    pLabelHelp->SetSize ( CVector2D ( 250.0f, 24.0f ) );
        pLabelHelp->SetVerticalAlign( CGUI_ALIGN_VERTICALCENTER );

        // Layout the mapping buttons like a dual axis joypad
        CVector2D vecPosList[] = {  CVector2D ( 0.30f,  0.617f ),     // Left Stick
                                    CVector2D ( 0.52f,  0.617f ),
                                    CVector2D ( 0.41f,  0.516f ),
                                    CVector2D ( 0.41f,  0.718f ),

                                    CVector2D ( 0.65f,  0.617f ),     // Right Stick
                                    CVector2D ( 0.87f,  0.617f ),
                                    CVector2D ( 0.76f,  0.516f ),
                                    CVector2D ( 0.76f,  0.718f ),

                                    CVector2D ( 0.716f, 0.892f ),     // Acceleration/Brake
                                    CVector2D ( 0.455f, 0.892f )     };


        for ( int i = 0 ; i < JoyMan->GetOutputCount () && i < 10 ; i++ )
        {
            CVector2D vecPos = vecPosList[i];

            CGUIButton* pButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pControlsPane ) );
            pButton->SetPosition ( vecPos + CVector2D ( 0, 0 ), true );
            pButton->SetPosition ( pButton->GetPosition() + CVector2D ( 10, 0 ) );
	        pButton->SetSize ( CVector2D ( 48.0f, 24.0f ) );
            pButton->SetUserData ( (void*) i );
	        pButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnAxisSelectClick, this ) );

            CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane ) );
            pLabel->SetPosition ( vecPos + CVector2D ( 0, -0.085f ), true );
            pLabel->SetPosition ( pLabel->GetPosition() - CVector2D ( 10, 0 ));
	        pLabel->SetSize ( CVector2D ( 88.0f, 24.0f ) );
            pLabel->SetHorizontalAlign( CGUI_ALIGN_HORIZONTALCENTER );
            pLabel->SetVerticalAlign( CGUI_ALIGN_VERTICALCENTER );
	        pLabel->SetVisible ( i >= 8 );      // Hide all labels except 'Acceleration' and 'Brake'

            m_pJoypadLabels.push_back ( pLabel );
            m_pJoypadButtons.push_back ( pButton );
        }

        CGUILabel* pLabelLeft = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Left Stick" ) );
        pLabelLeft->SetPosition ( CVector2D ( 0.41f, 0.611f ), true );
	    pLabelLeft->SetSize ( CVector2D ( 68.0f, 24.0f ) );
        pLabelLeft->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
        pLabelLeft->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

        CGUILabel* pLabelRight = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Right Stick" ) );
        pLabelRight->SetPosition ( CVector2D ( 0.76f, 0.611f ), true );
	    pLabelRight->SetSize ( CVector2D ( 68.0f, 24.0f ) );
        pLabelRight->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
        pLabelRight->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

    }

    
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
    m_pEditUser->SetTextAcceptedHandler( GUI_CALLBACK( &CSettings::OnLoginButtonClick, this ) );
//    m_pEditUser->SetMaxLength ( 64 );

    m_pEditPass = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabCommunity ) );
    m_pEditPass->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
	m_pEditPass->GetPosition ( vecTemp, false );
    m_pEditPass->SetSize ( CVector2D ( 168.0f, 24.0f ) );
//    m_pEditPass->SetMaxLength ( 64 );
	m_pEditPass->SetMasked ( true );
    m_pEditPass->SetTextAcceptedHandler( GUI_CALLBACK( &CSettings::OnLoginButtonClick, this ) );

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
    m_pLabelNick->GetPosition ( vecTemp, false );
	m_pLabelNick->AutoSize ( "Nick:" );

    // Nick edit
    m_pEditNick = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabMultiplayer ) );
    m_pEditNick->SetPosition ( CVector2D ( vecTemp.fX + 100.0f, vecTemp.fY - 1.0f ) );
    m_pEditNick->SetSize ( CVector2D ( 168.0f, 24.0f ) );
    m_pEditNick->SetMaxLength ( 16 );

    m_pLabelConnection = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabMultiplayer, "Connection:" ) );
    m_pLabelConnection->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 26.0f ) );
    m_pLabelConnection->GetPosition ( vecTemp, false );
	m_pLabelConnection->AutoSize ( "Connection:" );

    m_pComboConnection = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabMultiplayer, "" ) );
	m_pComboConnection->SetPosition ( CVector2D ( vecTemp.fX + 100.0f, vecTemp.fY - 1.0f ) );
    m_pComboConnection->GetPosition ( vecTemp, false );
	m_pComboConnection->SetSize ( CVector2D ( 168.0f, 90.0f ) );
    m_pComboConnection->SetReadOnly ( true );
    m_pComboConnection->AddItem ( "Lan" );
    m_pComboConnection->AddItem ( "DSL" );
    m_pComboConnection->AddItem ( "Modem" );

    m_pSavePasswords = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabMultiplayer, "Save server passwords", true ) );
    m_pSavePasswords->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 50.0f ) );
    m_pSavePasswords->GetPosition ( vecTemp, false );
    m_pSavePasswords->SetSize ( CVector2D ( 224.0f, 16.0f ) );


	/**
	 *	Audio tab
	 **/
    m_pLabelRadioVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "Radio volume:" ) );
    m_pLabelRadioVolume->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
    m_pLabelRadioVolume->GetPosition ( vecTemp, false );
	m_pLabelRadioVolume->AutoSize ( "Radio volume:" );

    m_pAudioRadioVolume = reinterpret_cast < CGUIProgressBar* > ( pManager->CreateProgressBar ( pTabAudio ) );
    m_pAudioRadioVolume->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY ) );
    m_pAudioRadioVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );

    m_pLabelSFXVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "SFX volume:" ) );
    m_pLabelSFXVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pLabelSFXVolume->GetPosition ( vecTemp, false );
	m_pLabelSFXVolume->AutoSize ( "SFX volume:" );

    m_pAudioSFXVolume = reinterpret_cast < CGUIProgressBar* > ( pManager->CreateProgressBar ( pTabAudio ) );
    m_pAudioSFXVolume->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY ) );
    m_pAudioSFXVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );

    m_pLabelMTAVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "MTA volume:" ) );
    m_pLabelMTAVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pLabelMTAVolume->GetPosition ( vecTemp, false );
	m_pLabelMTAVolume->AutoSize ( "MTA volume:" );

    m_pAudioMTAVolume = reinterpret_cast < CGUIProgressBar* > ( pManager->CreateProgressBar ( pTabAudio ) );
    m_pAudioMTAVolume->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY ) );
    m_pAudioMTAVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );

	/**
	 *	Video tab
	 **/

    m_pVideoGeneralLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "General" ) );
    m_pVideoGeneralLabel->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
    m_pVideoGeneralLabel->GetPosition ( vecTemp, false );
	m_pVideoGeneralLabel->AutoSize ( "General  " );
	m_pVideoGeneralLabel->SetFont ( "default-bold-small" );

    m_pVideoResolutionLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Resolution:" ) );
    m_pVideoResolutionLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pVideoResolutionLabel->GetPosition ( vecTemp, false );
    m_pVideoResolutionLabel->AutoSize ( "Resolution: " );

    m_pComboResolution = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
	m_pComboResolution->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY - 1.0f ) );
	m_pComboResolution->SetSize ( CVector2D ( 200.0f, 160.0f ) );
    m_pComboResolution->SetReadOnly ( true );

    m_pCheckBoxWindowed = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Windowed", true ) );
    m_pCheckBoxWindowed->SetPosition ( CVector2D ( vecTemp.fX + 300.0f, vecTemp.fY ) );
	m_pCheckBoxWindowed->SetSize ( CVector2D ( 224.0f, 16.0f ) );
    // Out for now
    m_pCheckBoxWindowed->SetVisible ( false );

    m_pCheckBoxWideScreen = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Wide Screen", true ) );
    m_pCheckBoxWideScreen->SetPosition ( CVector2D ( vecTemp.fX + 300.0f, vecTemp.fY ) );
	m_pCheckBoxWideScreen->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    m_pVideoRenderingLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Menu rendering options" ) );
    m_pVideoRenderingLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 40.0f ) );
    m_pVideoRenderingLabel->GetPosition ( vecTemp, false );
	m_pVideoRenderingLabel->AutoSize ( "Menu rendering options  " );
	m_pVideoRenderingLabel->SetFont ( "default-bold-small" );

    m_pCheckBoxMenuDynamic = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Dynamic scene rendering", true ) );
    m_pCheckBoxMenuDynamic->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
	m_pCheckBoxMenuDynamic->SetSize ( CVector2D ( 174.0f, 16.0f ) );
	m_pCheckBoxMenuDynamic->GetPosition ( vecTemp, false );
	m_pCheckBoxMenuDynamic->SetUserData ( (void*) eCheckBox::CHECKBOX_MENU_DYNAMIC );

    m_pCheckBoxMenuVideo = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Video surface rendering", true ) );
	m_pCheckBoxMenuVideo->SetPosition ( CVector2D ( vecTemp.fX + 200.0f, vecTemp.fY ) );
	m_pCheckBoxMenuVideo->SetSize ( CVector2D ( 174.0f, 20.0f ) );
	m_pCheckBoxMenuVideo->SetUserData ( (void*) eCheckBox::CHECKBOX_MENU_VIDEO );

    m_pCheckBoxMenuPostEffects = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "PS2.0 post-effects", true ) );
	m_pCheckBoxMenuPostEffects->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16 ) );
	m_pCheckBoxMenuPostEffects->SetSize ( CVector2D ( 174.0f, 16.0f ) );
	m_pCheckBoxMenuPostEffects->SetUserData ( (void*) eCheckBox::CHECKBOX_MENU_POSTEFFECTS );
	
    /**
     * Chat Tab
     **/
    // Presets
    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "Presets:" ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 12.0f ) );
        pLabel->AutoSize ( "Presets:" );
    }

    m_pChatPresets = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabChat ) );
    m_pChatPresets->SetPosition ( CVector2D ( 60.0f, 10.0f ) );
    m_pChatPresets->SetSize ( CVector2D ( 340.0f, 200.0f ) );
    m_pChatPresets->SetReadOnly ( true );

    m_pChatLoadPreset = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabChat, "Load" ) );
    m_pChatLoadPreset->SetPosition ( CVector2D ( 410.0f, 10.0f ) );
    m_pChatLoadPreset->SetSize ( CVector2D ( 100.0f, 24.0f ) );

    // Color selection
    CGUITabPanel* pColorTabPanel = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( pTabChat ) );
    pColorTabPanel->SetPosition ( CVector2D ( 10.0f, 50.0f ) );
    pColorTabPanel->SetSize ( CVector2D ( 320.0f, 150.0f ) );

    CreateChatColorTab ( ChatColorType::CHAT_COLOR_BG, "Chat BG", pColorTabPanel );
    CreateChatColorTab ( ChatColorType::CHAT_COLOR_TEXT, "Chat Text", pColorTabPanel );
    CreateChatColorTab ( ChatColorType::CHAT_COLOR_INPUT_BG, "Input BG", pColorTabPanel );
    CreateChatColorTab ( ChatColorType::CHAT_COLOR_INPUT_TEXT, "Input Text", pColorTabPanel );

    // Font Selection
    m_pPaneChatFont = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( pTabChat ) ); 
    m_pPaneChatFont->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pPaneChatFont->SetPosition ( CVector2D ( 50.0f, 220.0f ) );
    m_pPaneChatFont->SetSize ( CVector2D ( 250.0f, 33.0f ) );

    CGUILabel* pFontLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pPaneChatFont, "Font:" ) );
    pFontLabel->SetPosition ( CVector2D ( 0.0f, 8.0f ) );
    pFontLabel->AutoSize ( "Font:" );

    m_pRadioChatFont [ eChatFont::CHAT_FONT_DEFAULT ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Tahoma" ) ); 
    m_pRadioChatFont [ eChatFont::CHAT_FONT_DEFAULT ]->SetSelected ( true );
    m_pRadioChatFont [ eChatFont::CHAT_FONT_DEFAULT ]->SetPosition ( CVector2D ( 50.0f, 0.0f ) );
    m_pRadioChatFont [ eChatFont::CHAT_FONT_DEFAULT ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ eChatFont::CHAT_FONT_CLEAR ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Verdana" ) ); 
    m_pRadioChatFont [ eChatFont::CHAT_FONT_CLEAR ]->SetPosition ( CVector2D ( 150.0f, 0.0f ) );
    m_pRadioChatFont [ eChatFont::CHAT_FONT_CLEAR ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ eChatFont::CHAT_FONT_BOLD ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Tahoma Bold" ) ); 
    m_pRadioChatFont [ eChatFont::CHAT_FONT_BOLD ]->SetPosition ( CVector2D ( 50.0f, 18.0f ) );
    m_pRadioChatFont [ eChatFont::CHAT_FONT_BOLD ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ eChatFont::CHAT_FONT_ARIAL ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Arial" ) ); 
    m_pRadioChatFont [ eChatFont::CHAT_FONT_ARIAL ]->SetPosition ( CVector2D ( 150.0f, 18.0f ) );
    m_pRadioChatFont [ eChatFont::CHAT_FONT_ARIAL ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    // Misc. Options
    {
        CGUILabel* pLabel;

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "Lines:" ) );
        pLabel->SetPosition ( CVector2D ( 360.0f, 60.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "Lines:" );

        m_pChatLines = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabChat, "" ) );
        m_pChatLines->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatLines->SetSize ( CVector2D ( 110.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "Scale:" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "Scale:" );

        m_pChatScaleX = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabChat, "" ) );
        m_pChatScaleX->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatScaleX->SetSize ( CVector2D ( 50.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "x" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 92.5f, vecTemp.fY + 2.0f ) );
        pLabel->AutoSize ( "x" );

        m_pChatScaleY = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabChat, "" ) );
        m_pChatScaleY->SetPosition ( CVector2D ( vecTemp.fX + 100.0f, vecTemp.fY - 2.0f ) );
        m_pChatScaleY->SetSize ( CVector2D ( 50.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "Width:" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "Width:" );

        m_pChatWidth = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabChat, "" ) );
        m_pChatWidth->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatWidth->SetSize ( CVector2D ( 110.0f, 24.0f ) );

        m_pChatCssBackground = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox( pTabChat, "Hide background when\nnot typing" ) );
        m_pChatCssBackground->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        m_pChatCssBackground->GetPosition ( vecTemp );
        m_pChatCssBackground->SetSize ( CVector2D ( 160.0f, 32.0f ) );

        m_pChatCssText = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox( pTabChat, "Fade out old lines" ) );
        m_pChatCssText->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 34.0f ) );
        m_pChatCssText->GetPosition ( vecTemp );
        m_pChatCssText->SetSize ( CVector2D ( 160.0f, 16.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "after" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 17.0f, vecTemp.fY + 22.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "after " );

        m_pChatLineLife = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabChat, "" ) );
        m_pChatLineLife->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatLineLife->SetSize ( CVector2D ( 70.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "sec" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 115.0f, vecTemp.fY ) );
        pLabel->AutoSize ( "sec" );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "for" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "for" );

        m_pChatLineFadeout = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabChat, "" ) );
        m_pChatLineFadeout->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatLineFadeout->SetSize ( CVector2D ( 70.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabChat, "sec" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 115.0f, vecTemp.fY ) );
        pLabel->AutoSize ( "sec" );
    }

    // Set up the events
    m_pButtonOK->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnOKButtonClick, this ) );
    m_pButtonCancel->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCancelButtonClick, this ) );
	m_pButtonLogin->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnLoginButtonClick, this ) );
    m_pButtonRegister->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnRegisterButtonClick, this ) );
    m_pCheckBoxMenuDynamic->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCheckBoxClick, this ) );
    m_pCheckBoxMenuVideo->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCheckBoxClick, this ) );
    m_pCheckBoxMenuPostEffects->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCheckBoxClick, this ) );
    m_pChatLoadPreset->SetClickHandler ( GUI_CALLBACK( &CSettings::OnChatLoadPresetClick, this ) );
	/*
	// Give a warning if no community account settings were stored in config
	CCore::GetSingleton ().ShowMessageBox ( CORE_SETTINGS_COMMUNITY_WARNING, "Multi Theft Auto: Community settings", MB_ICON_WARNING );
	*/

    // Load Chat presets
    LoadChatPresets ();
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

        UpdateJoypadTab ();

		m_dwFrameCount = 0;
	}
	m_dwFrameCount++;

    UpdateCaptureAxis ();

}


//
// Saves the Joypad settings
//
void CSettings::ProcessJoypad( void )
{
    // Update from GUI
    GetJoystickManager ()->SetDeadZone ( atoi ( m_pEditDeadzone->GetText ().c_str () ) );
    GetJoystickManager ()->SetSaturation ( atoi ( m_pEditSaturation->GetText ().c_str () ) );

    GetJoystickManager ()->SaveConfig ();
}


//
// Update GUI elements for the Joypad Tab
//
void CSettings::UpdateJoypadTab ()
{
    CJoystickManagerInterface* JoyMan = GetJoystickManager ();

    // Has anything changed?
    if ( m_JoypadSettingsRevision == JoyMan->GetSettingsRevision () )
        return;

    // Update the joystick name
    string strJoystickName = JoyMan->IsJoypadConnected () ? JoyMan->GetControllerName () : "Joypad not detected  -  Check connections and restart game";

    m_pJoypadName->SetPosition ( CVector2D ( 0.5f, 0.405f ), true );
	m_pJoypadName->SetText ( strJoystickName.c_str () );
	m_pJoypadName->AutoSize ( strJoystickName.c_str () );
    m_pJoypadName->SetPosition ( m_pJoypadName->GetPosition () - CVector2D ( m_pJoypadName->GetSize ().fX * 0.5, 0.0f ) );


    // Joystick name underline
    string strUnderline = "";
    int inumChars = m_pJoypadName->GetSize ().fX / 7.f + 0.5f;
    for ( int i = 0 ; i < inumChars ; i++ )
        strUnderline = strUnderline + "_";

    m_pJoypadUnderline->SetPosition ( CVector2D ( 0.5f, 0.405f ), true );
	m_pJoypadUnderline->SetText ( strUnderline.c_str () );
	m_pJoypadUnderline->AutoSize ( strUnderline.c_str () );
    m_pJoypadUnderline->SetPosition ( m_pJoypadUnderline->GetPosition () - CVector2D ( m_pJoypadUnderline->GetSize ().fX * 0.5, -2.0f ) );
    m_pJoypadUnderline->SetVisible ( JoyMan->IsJoypadConnected () );


    // Update DeadZone and Saturation edit boxes
    char szDeadzone[32] = "";
    char szSaturation[32] = "";
    itoa ( JoyMan->GetDeadZone (), szDeadzone, 10 );
    itoa ( JoyMan->GetSaturation (), szSaturation, 10 );

    m_pEditDeadzone->SetText ( szDeadzone );
    m_pEditSaturation->SetText ( szSaturation );


    // Update axes labels and buttons
    for ( int i = 0 ; i < JoyMan->GetOutputCount () && i < (int)m_pJoypadButtons.size () ; i++ )
    {
        string outputName = JoyMan->GetOutputName (i);         // LeftStickPosX etc
        string inputName  = JoyMan->GetOutputInputName (i);     // X+ or RZ- etc

        CGUILabel* pLabel = m_pJoypadLabels[i];
        pLabel->SetText ( outputName.c_str () );

        CGUIButton* pButton = m_pJoypadButtons[i];
        pButton->SetText ( inputName.c_str () );
    }

    m_JoypadSettingsRevision = JoyMan->GetSettingsRevision ();
}


//
// React to the DeadZone and Saturation edit boxes changing
//
bool CSettings::OnJoypadTextChanged ( CGUIElement* pElement )
{
    // Update from GUI
    GetJoystickManager ()->SetDeadZone ( atoi ( m_pEditDeadzone->GetText ().c_str () ) );
    GetJoystickManager ()->SetSaturation ( atoi ( m_pEditSaturation->GetText ().c_str () ) );

    // Dont immediately read back these settings
    m_JoypadSettingsRevision = GetJoystickManager ()->GetSettingsRevision ();

	return true;
}


//
// Called every frame. If capturing an axis, see if its all done yet.
//
void CSettings::UpdateCaptureAxis ()
{
    if ( m_bCaptureAxis )
    {
        // Are we done?        
        if ( GetJoystickManager ()->IsAxisBindComplete () )
        {
	        // Remove the messagebox we created earlier
	        CCore::GetSingleton ().RemoveMessageBox ();

	        // Update GUI elements
            UpdateJoypadTab();

            m_bCaptureAxis = false;
        }
    }
}


//
// Called when the user clicks on an map axis button. Starts the capture axis process.
//
bool CSettings::OnAxisSelectClick ( CGUIElement* pElement )
{
    int index = reinterpret_cast < int > ( pElement->GetUserData () );

    if ( GetJoystickManager ()->BindNextUsedAxisToOutput ( index ) )
    {
        m_bCaptureAxis = true;
        CCore::GetSingleton ().ShowMessageBox ( "Binding axis", "Move an axis to bind, or escape to clear", MB_ICON_QUESTION );
    }

	return true;
}


//
// Called when the user clicks on the joypad 'Load Defaults' button.
//
bool CSettings::OnJoypadDefaultClick ( CGUIElement* pElement )
{
	// Load the default binds
	GetJoystickManager ()->LoadDefaultConfig ();

	// Update the GUI
	UpdateJoypadTab ();

	return true;
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
                        // Did we have any keys with the same "up" state?
                        CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand ( szCommand, NULL, true, pBind->boundKey->szKey, true, false );
                        if ( pUpBind )
                        {
                            pUpBind->boundKey = pPriKey;
                        }

						pBind->boundKey = pPriKey;
					}
                }
				// If the primary key field was empty, we can remove the keybind
				else {
                    // Remove any matching "up" state binds we may have
                    CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand ( szCommand, NULL, true, pBind->boundKey->szKey, true, false );
                    if ( pUpBind )
                    {
                        pKeyBinds->Remove ( pUpBind );
                    }
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
                        {
                            // Did we have any keys with the same "up" state?
                            CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand ( szCommand, NULL, true, pBind->boundKey->szKey, true, false );
                            if ( pUpBind )
                            {
                                pUpBind->boundKey = pSecKeys[k];
                            }
                            pBind->boundKey = pSecKeys[k];
                        }
                    }
				    // If the secondary key field was empty, we should remove the keybind
				    else
                    {
                        // Remove any matching "up" state binds we may have
                        CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand ( szCommand, NULL, true, pBind->boundKey->szKey, true, false );
                        if ( pUpBind )
                        {
                             pKeyBinds->Remove ( pUpBind );
                        }
                        pKeyBinds->Remove ( pBind );
                    }
                }
			    // If this key bind didn't exist, create it
                else if ( pSecKeys[k] )
                {
				    pKeyBinds->AddCommand ( pSecKeys[k], szCommand, szArguments );
                    // Also add a matching "up" state if applicable
                    CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand ( szCommand, NULL, true, pPriKey->szKey, true, false );
                    if ( pUpBind )
                    {
                        pKeyBinds->AddCommand ( pSecKeys[k]->szKey, szCommand, pUpBind->szArguments, false, pUpBind->szResource );
                    }
                }
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

	    // Proceed if the user didn't select the "Bind"-column
	    if ( pItem != pItemBind )
	    {
		    m_uiCaptureKey = 0;
		    m_pSelectedBind = pItem;
            m_bCaptureKey = true;

		    // Determine if the primary or secondary column was selected
		    if ( m_pBindsList->GetItemColumnIndex ( pItem ) == 1/*m_hPriKey  Note: handle is not the same as index */ ) {
			    // Create a messagebox to notify the user
			    //SString strText = SString::Printf ( "Press a key to bind to '%s'", pItemBind->GetText ().c_str () );
			    SString strText = "Press a key to bind, or escape to clear";
			    CCore::GetSingleton ().ShowMessageBox ( "Binding a primary key", strText, MB_ICON_QUESTION );
		    } else {
			    // Create a messagebox to notify the user
			    //sSString strText = SString::Printf ( "Press a key to bind to '%s'", pItemBind->GetText ().c_str () );
			    SString strText = "Press a key to bind, or escape to clear";
			    CCore::GetSingleton ().ShowMessageBox ( "Binding a secondary key", strText, MB_ICON_QUESTION );
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
    std::map < std::string, int > iResourceItems;
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
                    SString strDescription;
                    bool bSkip = true;
                    if ( bindType == KEY_BIND_COMMAND )
                    {
                        CCommandBind* pCommandBind = reinterpret_cast < CCommandBind* > ( *iter );
                        bSkip = false;
                        if ( pCommandBind->szResource )
                        {
                            if ( pCommandBind->bActive )
                            {
                                const char* szResource = pCommandBind->szResource;
                                std::string strResource = szResource;
                                if ( iResourceItems.count(strResource) == 0 )
                                {
                                    iBind = m_pBindsList->InsertRowAfter ( m_pBindsList->GetRowCount() );
                                    m_pBindsList->SetItemText ( iBind, m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true );
                                    
                                    iBind = m_pBindsList->InsertRowAfter ( iBind );
                                    m_pBindsList->SetItemText ( iBind, m_hBind, szResource, false, true );
                                    iResourceItems.insert( make_pair(strResource, iBind ) );
                                }
                                row = iResourceItems[strResource];
                                iMultiplayerRowCount++;
                            }
                            else
                            {
                                bSkip = true;
                            }
                        }
                        if ( pCommandBind->szArguments && pCommandBind->szArguments[0] != '\0' )
                        {
                            strDescription.Format ( "%s: %s", pCommandBind->szCommand, pCommandBind->szArguments );
                            iMultiplayerRowCount++;
                        }
                        else
                        {
                            strDescription = pCommandBind->szCommand;
                            iMultiplayerRowCount++;
                        }
                    }                    

                    if ( !bSkip )
                    {
				        // Add the bind to the list
				        iBind = m_pBindsList->InsertRowAfter ( row );
                        m_pBindsList->SetItemText ( iBind, m_hBind, strDescription );
				        m_pBindsList->SetItemText ( iBind, m_hPriKey, pBind->boundKey->szKey );
                        for ( int k = 0 ; k < SecKeyNum ; k++ )
                            m_pBindsList->SetItemText ( iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY );
                        m_pBindsList->SetItemData ( iBind, m_hBind, (void*) bindType );
                        m_pBindsList->SetItemData ( iBind, m_hPriKey, pBind );

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
	ProcessJoypad ();
	
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
        m_pEditPass->SetText ( "" );
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

    // Connection type
    unsigned int uiMTUSize = NET_MTU_LAN;
    CVARS_GET ( "mtu_size", uiMTUSize );
	if ( uiMTUSize == NET_MTU_LAN ) m_pComboConnection->SetText ( "Lan" );
	else if ( uiMTUSize == NET_MTU_DSL ) m_pComboConnection->SetText ( "DSL" );
    else if ( uiMTUSize == NET_MTU_MODEM ) m_pComboConnection->SetText ( "Modem" );
    else m_pComboConnection->SetText ( "Unknown" );

    // Save server password
    CVARS_GET ( "save_server_passwords", bVar ); m_pSavePasswords->SetSelected ( bVar );

    // Controls
    CVARS_GET ( "invert_mouse", bVar ); m_pInvertMouse->SetSelected ( bVar );
    CVARS_GET ( "steer_with_mouse", bVar ); m_pSteerWithMouse->SetSelected ( bVar );
    CVARS_GET ( "fly_with_mouse", bVar ); m_pFlyWithMouse->SetSelected ( bVar );
    CVARS_GET ( "classic_controls", bVar ); m_pClassicControls->SetSelected ( bVar );

	// Community
    CVARS_GET ( "community_username", strVar );
	if ( !strVar.empty () ) m_pEditUser->SetText ( strVar.c_str () );
    CVARS_GET ( "community_password", strVar );
	if ( !strVar.empty () ) m_pEditPass->SetText ( strVar.c_str () );

    // Audio
    CGameSettings * gameSettings = CCore::GetSingleton ( ).GetGame ( )->GetSettings();
    m_pAudioRadioVolume->SetProgress ( gameSettings->GetRadioVolume() / 64 * 100.0f );
    m_pAudioSFXVolume->SetProgress ( gameSettings->GetSFXVolume() / 64 * 100.0f );
    // Video

    VideoMode           vidModemInfo;
    int                 vidMode, numVidModes, currentVidMode;

    m_pComboResolution->Clear ();
    numVidModes = gameSettings->GetNumVideoModes();
    currentVidMode = gameSettings->GetCurrentVideoMode();

    for (vidMode = 0; vidMode < numVidModes; vidMode++)
    {
        gameSettings->GetVideoModeInfo(&vidModemInfo, vidMode);
        
        if ( vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE )
        {
            SString strMode ( "%lu x %lu x %lu", vidModemInfo.width, vidModemInfo.height, vidModemInfo.depth );
            m_pComboResolution->AddItem ( strMode )->SetData ( (void*)vidMode );
        }
        if ( vidMode == currentVidMode )
        {
            SString strMode ( "%lu x %lu x %lu", vidModemInfo.width, vidModemInfo.height, vidModemInfo.depth );
            
            m_pComboResolution->SetText ( strMode );
        }
    }
    m_pCheckBoxWindowed->SetSelected ( currentVidMode == 0 );
    m_pCheckBoxWideScreen->SetSelected ( gameSettings->IsWideScreenEnabled () );

    // Chat
    LoadChatColorFromCVar ( ChatColorType::CHAT_COLOR_BG, "chat_color" );
    LoadChatColorFromCVar ( ChatColorType::CHAT_COLOR_TEXT, "chat_text_color" );
    LoadChatColorFromCVar ( ChatColorType::CHAT_COLOR_INPUT_BG, "chat_input_color" );
    LoadChatColorFromCVar ( ChatColorType::CHAT_COLOR_INPUT_TEXT, "chat_input_text_color" );

    unsigned int uiFont;
    CVARS_GET ( "chat_font", uiFont );
    if ( uiFont >= eChatFont::CHAT_FONT_MAX )
        uiFont = eChatFont::CHAT_FONT_DEFAULT;
    m_pRadioChatFont [ uiFont ]->SetSelected ( true );

    CVARS_GET ( "chat_lines", strVar ); m_pChatLines->SetText ( strVar.c_str () );

    try
    {
        CVARS_GET ( "chat_scale", strVar );
        stringstream ss( strVar );
        ss >> strVar;
        m_pChatScaleX->SetText ( strVar.c_str () );
        ss >> strVar;
        m_pChatScaleY->SetText ( strVar.c_str () );
    }
    catch (...)
    {
    }

    CVARS_GET ( "chat_width", strVar ); m_pChatWidth->SetText ( strVar.c_str () );
    CVARS_GET ( "chat_css_style_text", bVar ); m_pChatCssText->SetSelected ( bVar );
    CVARS_GET ( "chat_css_style_background", bVar ); m_pChatCssBackground->SetSelected ( bVar );

    {
        int iVar;
        CVARS_GET ( "chat_line_life", iVar ); 
        SetMilliseconds ( m_pChatLineLife, iVar );

        CVARS_GET ( "chat_line_fade_out", iVar ); 
        SetMilliseconds ( m_pChatLineFadeout, iVar );
    }
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

    // Connection type
    CCommandFuncs::ConnectionType ( m_pComboConnection->GetText().c_str() );

    // Server pass saving
    bool bServerPasswords = m_pSavePasswords->GetSelected ();
    CVARS_SET ( "save_server_passwords", bServerPasswords );
    if ( !bServerPasswords )
    {
        g_pCore->GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetServerBrowser ()->ClearServerPasswords ();
    }

    // Very hacky
    CControllerConfigManager * pController = g_pCore->GetGame ()->GetControllerConfigManager ();   
    CVARS_SET ( "invert_mouse", m_pInvertMouse->GetSelected () );
    pController->SetMouseInverted ( m_pInvertMouse->GetSelected () );
    CVARS_SET ( "steer_with_mouse", m_pSteerWithMouse->GetSelected () );
    pController->SetSteerWithMouse ( m_pSteerWithMouse->GetSelected () );
    CVARS_SET ( "fly_with_mouse", m_pFlyWithMouse->GetSelected () );
    pController->SetFlyWithMouse ( m_pFlyWithMouse->GetSelected () );

    bool bClassicControls = m_pClassicControls->GetSelected ();
    CVARS_SET ( "classic_controls", bClassicControls );
    bClassicControls ? pController->SetInputType ( NULL ) : pController->SetInputType ( 1 );

    CCore::GetSingleton ().SaveConfig ();

    // Audio
    CGameSettings * gameSettings = CCore::GetSingleton ( ).GetGame ( )->GetSettings();
    gameSettings->SetRadioVolume ( ( unsigned char )( m_pAudioRadioVolume->GetProgress() / 100 * 64 ) );
    gameSettings->SetSFXVolume ( ( unsigned char )( m_pAudioSFXVolume->GetProgress() / 100 * 64 ) );


    // Video
    int numVidModes = gameSettings->GetNumVideoModes(),
        currentVidMode = gameSettings->GetCurrentVideoMode();

    CGUIListItem* pSelected = m_pComboResolution->GetSelectedItem ();
    if ( pSelected )
    {
        g_pCore->GetConsole()->Printf ( "selected" );
        int selectedVidMode = ( int ) pSelected->GetData();;
        g_pCore->GetConsole()->Printf ( "%d", selectedVidMode );
        if ( selectedVidMode != currentVidMode &&
             selectedVidMode <= numVidModes &&
             selectedVidMode >= 0 )
        {
            gameSettings->SetCurrentVideoMode ( selectedVidMode );
            g_pCore->GetLocalGUI()->GetMainMenu ()->RefreshPositions();
        }
    }
    gameSettings->SetWideScreenEnabled ( m_pCheckBoxWideScreen->GetSelected() );
    /*
    currentVidMode = gameSettings->GetCurrentVideoMode();
    if ( m_pCheckBoxWindowed->GetSelected() && currentVidMode != 0 )
        CCommandFuncs::Window ( "" );
    else if ( !m_pCheckBoxWindowed->GetSelected() && currentVidMode == 0 )
        CCommandFuncs::Window ( "" );
    */

    // Chat
    SaveChatColor ( ChatColorType::CHAT_COLOR_BG, "chat_color" );
    SaveChatColor ( ChatColorType::CHAT_COLOR_TEXT, "chat_text_color" );
    SaveChatColor ( ChatColorType::CHAT_COLOR_INPUT_BG, "chat_input_color" );
    SaveChatColor ( ChatColorType::CHAT_COLOR_INPUT_TEXT, "chat_input_text_color" );
    for ( int iFont = 0; iFont < ChatColorType::CHAT_COLOR_MAX; iFont ++ )
    {
        if ( m_pRadioChatFont [ iFont ]->GetSelected( ) )
        {
            CVARS_SET ( "chat_font", iFont );
            break;
        }
    }

    strVar = m_pChatScaleX->GetText () + " " + m_pChatScaleY->GetText ();
    CVARS_SET ( "chat_scale", strVar );

    CVARS_SET ( "chat_lines", m_pChatLines->GetText () );
    CVARS_SET ( "chat_width", m_pChatWidth->GetText () );
    CVARS_SET ( "chat_css_style_text", m_pChatCssText->GetSelected () );
    CVARS_SET ( "chat_css_style_background", m_pChatCssBackground->GetSelected () );
    CVARS_SET ( "chat_line_life", GetMilliseconds ( m_pChatLineLife ) );
    CVARS_SET ( "chat_line_fade_out", GetMilliseconds ( m_pChatLineFadeout ) );

    // Save the single player settings (e.g. video mode, volume)
    gameSettings->Save ();
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

void CSettings::CreateChatColorTab ( ChatColorType eType, const char* szName, CGUITabPanel* pParent )
{
    CVector2D vecTemp;

    // Create the GUI Elements
    CGUI* pManager = g_pCore->GetGUI ();
    CGUITab* pTab = pParent->CreateTab ( szName );
    CGUILabel* pLabel;

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "Red:" ) );
    pLabel->SetPosition ( CVector2D ( 0.05f, 0.065f ), true );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( "Red:" );

    m_pChatRed [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatRed [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatRed [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "Green:" ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( "Green:" );

    m_pChatGreen [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatGreen [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatGreen [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "Blue:" ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( "Blue:" );

    m_pChatBlue [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatBlue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatBlue [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "Transparency:" ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( "Transparency:" );

    m_pChatAlpha [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatAlpha [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatAlpha [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
}

void CSettings::LoadChatColorFromCVar ( ChatColorType eType, const char* szCVar )
{
    // Load the color according to the cvars and update the gui
    CColor pColor;
    CVARS_GET ( szCVar, pColor );
    SetChatColorValues ( eType, pColor );
}

void CSettings::SaveChatColor ( ChatColorType eType, const char* szCVar )
{
    // Save the color to the cvar if it's different
    CColor pColor, pOldColor;
    pColor = GetChatColorValues ( eType );
    CVARS_GET ( szCVar, pOldColor );

    if ( pColor.R != pOldColor.R || pColor.G != pOldColor.G || pColor.B != pOldColor.B || pColor.A != pOldColor.A )
        CVARS_SET ( szCVar, pColor );
}

CColor CSettings::GetChatColorValues ( ChatColorType eType )
{
    // Retrieve the color according to the scrollbar values
    CColor pColor;
    pColor.R = m_pChatRed [ eType ]->GetScrollPosition () * 255;
    pColor.G = m_pChatGreen [ eType ]->GetScrollPosition () * 255;
    pColor.B = m_pChatBlue [ eType ]->GetScrollPosition () * 255;
    pColor.A = m_pChatAlpha [ eType ]->GetScrollPosition () * 255;
    return pColor;
}

void CSettings::SetChatColorValues ( ChatColorType eType, CColor pColor )
{
    // Set the scrollbar position based on the color
    m_pChatRed [ eType ]->SetScrollPosition ( ( float ) pColor.R / 255.0f );
    m_pChatGreen [ eType ]->SetScrollPosition ( ( float ) pColor.G / 255.0f );
    m_pChatBlue [ eType ]->SetScrollPosition ( ( float ) pColor.B / 255.0f );
    m_pChatAlpha [ eType ]->SetScrollPosition ( ( float ) pColor.A / 255.0f );
}

void CSettings::LoadChatPresets( )
{
    CXMLFile* pPresetsFile = CCore::GetSingleton ().GetXML ()->CreateXML ( CalcMTASAPath ( CHAT_PRESETS_PATH ) );
    if ( pPresetsFile && pPresetsFile->Parse () )
    {
        CXMLNode* pPresetsRoot = pPresetsFile->GetRootNode ();
        if ( !pPresetsRoot )
            pPresetsRoot = pPresetsFile->CreateRootNode ( CHAT_PRESETS_ROOT );

        list < CXMLNode* >::const_iterator iter = pPresetsRoot->ChildrenBegin ();
        for ( ; iter != pPresetsRoot->ChildrenEnd (); iter++ )
        {
            CXMLNode* pNode = reinterpret_cast < CXMLNode* > ( *iter );
            if ( pNode->GetTagName ().compare ( "preset" ) == 0 )
            {
                CXMLAttribute* pName = pNode->GetAttributes().Find ( "name" );
                if ( pName && pName->GetValue()[1] )
                {
                    CGUIListItem* pItem = m_pChatPresets->AddItem ( pName->GetValue ().c_str () );
                    pItem->SetData ( pNode );
                }
            }
        }
    }
}

void CSettings::LoadChatColorFromString ( ChatColorType eType, string strColor )
{
    CColor pColor;
    stringstream ss( strColor );
    int iR, iG, iB, iA;

    try
    {
        ss >> iR >> iG >> iB >> iA;
        pColor.R = iR;
        pColor.G = iG;
        pColor.B = iB;
        pColor.A = iA;
        SetChatColorValues ( eType, pColor );
    }
    catch(...)
    {
    }
}

int CSettings::GetMilliseconds ( CGUIEdit* pEdit )
{
    float fValue;
    stringstream ss ( pEdit->GetText () );
    try
    {
        ss >> fValue;
        fValue *= 1000;
        return ( int ) fValue;
    }
    catch(...)
    {
        return 0;
    }
}

void CSettings::SetMilliseconds ( CGUIEdit* pEdit, int iValue )
{
    stringstream ss; 
    ss << ( float ) iValue / 1000;
    pEdit->SetText ( ss.str ().c_str () );
}

bool CSettings::OnChatLoadPresetClick( CGUIElement* pElement )
{
    CGUIListItem* pItem = m_pChatPresets->GetSelectedItem ();
    if ( pItem )
    {
        CXMLNode* pNode = reinterpret_cast < CXMLNode* > ( pItem->GetData () );
        if ( pNode )
        {
            list < CXMLNode* >::const_iterator iter = pNode->ChildrenBegin ();
            for ( ; iter != pNode->ChildrenEnd (); iter++ )
            {
                // Load all settings provided
                CXMLNode* pSubNode = reinterpret_cast < CXMLNode* > ( *iter );
                string strTag = pSubNode->GetTagName ();
                string strValue = pSubNode->GetTagContent();
                
                if ( strValue.length () > 0 )
                {
                    if ( strTag == "color_text" )
                    {
                        LoadChatColorFromString ( ChatColorType::CHAT_COLOR_TEXT, strValue );
                    }
                    else if ( strTag == "color_background" )
                    {
                        LoadChatColorFromString ( ChatColorType::CHAT_COLOR_BG, strValue );
                    }
                    else if ( strTag == "color_input_text" )
                    {
                        LoadChatColorFromString ( ChatColorType::CHAT_COLOR_INPUT_TEXT, strValue );
                    }
                    else if ( strTag == "color_input_background" )
                    {
                        LoadChatColorFromString ( ChatColorType::CHAT_COLOR_INPUT_BG, strValue );
                    }
                    else if ( strTag == "font" )
                    {
                        int iValue;
                        pSubNode->GetTagContent ( iValue );
                        
                        if ( iValue < 0 || iValue >= eChatFont::CHAT_FONT_MAX )
                            iValue = eChatFont::CHAT_FONT_DEFAULT;
                        m_pRadioChatFont [ iValue ]->SetSelected ( true );
                    }
                    else if ( strTag == "lines" )
                    {
                        m_pChatLines->SetText ( strValue.c_str () );
                    }
                    else if ( strTag == "scale" )
                    {
                        try
                        {
                            stringstream ss ( strValue );
                            ss >> strValue;
                            m_pChatScaleX->SetText ( strValue.c_str () );
                            ss >> strValue;
                            m_pChatScaleY->SetText ( strValue.c_str () );
                        }
                        catch(...)
                        {
                        }
                    }
                    else if ( strTag == "width" )
                    {
                        m_pChatWidth->SetText ( strValue.c_str () );
                    }
                    else if ( strTag == "css_text" )
                    {
                        bool bValue;
                        pSubNode->GetTagContent ( bValue );
                        m_pChatCssText->SetSelected ( bValue );
                    }
                    else if ( strTag == "css_background" )
                    {
                        bool bValue;
                        pSubNode->GetTagContent ( bValue );
                        m_pChatCssBackground->SetSelected ( bValue );
                    }
                    else if ( strTag == "line_life" )
                    {
                        int iValue;
                        pSubNode->GetTagContent ( iValue );
                        SetMilliseconds( m_pChatLineLife, iValue );
                    }
                    else if ( strTag == "line_fadeout" )
                    {
                        int iValue;
                        pSubNode->GetTagContent ( iValue );
                        SetMilliseconds( m_pChatLineFadeout, iValue );
                    }
                }
            }
        }
    }
    return true;
}
