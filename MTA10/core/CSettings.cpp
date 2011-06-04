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
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
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
#define CORE_SETTINGS_HEADER_GAME       "GTA GAME CONTROLS"
#define CORE_SETTINGS_HEADER_MP         "MULTIPLAYER CONTROLS"
#define CORE_SETTINGS_HEADER_SPACER     " "
#define CORE_SETTINGS_NO_KEY            " "
#define CORE_SETTINGS_COMMUNITY_TEXT    \
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
    CGUITab *pTabMultiplayer, *pTabVideo, *pTabAudio, *pTabBinds, *pTabControls, *pTabCommunity, *pTabInterface, *pTabAdvanced;
    CGUI *pManager = g_pCore->GetGUI ();

    // Init
    m_bIsModLoaded = false;
    m_bCaptureKey = false;
    m_dwFrameCount = 0;
    m_bShownVolumetricShadowsWarning = false;
    CVector2D vecTemp;

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "SETTINGS" ) );
    m_pWindow->SetCloseButtonEnabled ( true );
    m_pWindow->SetMovable ( true );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    float yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 560.0f / 2, resolution.fY / 2 - 360.0f / 2 + yoff  ), false );

    //m_pWindow->SetPosition ( CVector2D ( 0.15f, 0.20f ), true );
    m_pWindow->SetSize ( CVector2D ( 560.0f, 410.0f ) );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->BringToFront ();

    // Create the tab panel and necessary tabs
    m_pTabs = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( m_pWindow ) );
    m_pTabs->SetPosition ( CVector2D ( 0, 20.0f ) );
    m_pTabs->SetSize ( CVector2D ( 560.0f, 350.0f ) );
    pTabMultiplayer = m_pTabs->CreateTab ( "Multiplayer" );
    pTabVideo = m_pTabs->CreateTab ( "Video" );
    pTabAudio = m_pTabs->CreateTab ( "Audio" );
    pTabBinds = m_pTabs->CreateTab ( "Binds" );
    pTabControls = m_pTabs->CreateTab ( "Controls" );
    pTabCommunity = m_pTabs->CreateTab ( "Community" );
    pTabInterface = m_pTabs->CreateTab ( "Interface" );
    pTabAdvanced = m_pTabs->CreateTab ( "Advanced" );

    // Create buttons
    //  OK button
    m_pButtonOK = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "OK" ) );
    m_pButtonOK->SetPosition ( CVector2D ( 0.5f, 0.92f ), true );

    //  Cancel button
    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Cancel" ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 0.78f, 0.92f ), true );

    /**
     *  Binds tab
     **/
    m_pBindsList = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( pTabBinds, false ) );
    m_pBindsList->SetPosition ( CVector2D ( 0.02f, 0.05f ), true );
    m_pBindsList->SetSize ( CVector2D ( 520.0f, 225.0f ) );
    m_pBindsList->SetSorting ( false );
    m_pBindsList->SetSelectionMode ( SelectionModes::CellSingle );

    m_pBindsDefButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabBinds, "Load defaults" ) );
    m_pBindsDefButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnBindsDefaultClick, this ) );
    m_pBindsDefButton->SetPosition ( CVector2D ( 402.0f, 245.0f ) );

    /**
     *  Controls tab
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
    m_pControlsScrollPane->SetVerticalScrollBar ( true );
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
    m_pSteerWithMouse->SetPosition ( CVector2D ( vecTemp.fX + 320, vecTemp.fY ) );

    m_pLabelMouseSensitivity = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "Mouse sensitivity:" ) );
    m_pLabelMouseSensitivity->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16.0f ) );
    m_pLabelMouseSensitivity->GetPosition ( vecTemp, false );
    m_pLabelMouseSensitivity->AutoSize ( "Mouse sensitivity:" );

    m_pMouseSensitivity = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pControlsPane ) );
    m_pMouseSensitivity->SetPosition ( CVector2D ( vecTemp.fX + 100.0f, vecTemp.fY ) );
    m_pMouseSensitivity->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pMouseSensitivity->SetProperty ( "StepSize", "0.01" );

    m_pLabelMouseSensitivityValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pControlsPane, "0%") );
    m_pLabelMouseSensitivityValue->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY ) );
    m_pLabelMouseSensitivityValue->AutoSize ( "100%" );

    m_pFlyWithMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pControlsPane, "Fly with mouse", true ) );
    m_pFlyWithMouse->SetPosition ( CVector2D ( vecTemp.fX + 320, vecTemp.fY ) );
    m_pFlyWithMouse->GetPosition ( vecTemp, false );

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
     *  Community tab
     **/

    m_pTabs->DeleteTab ( pTabCommunity );

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
    m_pEditNick->SetSize ( CVector2D ( 178.0f, 24.0f ) );
    m_pEditNick->SetMaxLength ( MAX_PLAYER_NICK_LENGTH );
    m_pEditNick->SetTextAcceptedHandler( GUI_CALLBACK( &CSettings::OnOKButtonClick, this ) );

    m_pSavePasswords = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabMultiplayer, "Save server passwords", true ) );
    m_pSavePasswords->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 50.0f ) );
    m_pSavePasswords->GetPosition ( vecTemp, false );
    m_pSavePasswords->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    m_pAutoRefreshBrowser = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabMultiplayer, "Auto-refresh server browser", true ) );
    m_pAutoRefreshBrowser->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 20.0f ) );
    m_pAutoRefreshBrowser->GetPosition ( vecTemp, false );
    m_pAutoRefreshBrowser->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    /**
     *  Audio tab
     **/
    m_pAudioGeneralLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "General" ) );
    m_pAudioGeneralLabel->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
    m_pAudioGeneralLabel->GetPosition ( vecTemp, false );
    m_pAudioGeneralLabel->AutoSize ( "General  " );
    m_pAudioGeneralLabel->SetFont ( "default-bold-small" );

    m_pLabelRadioVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "Radio volume:" ) );
    m_pLabelRadioVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ), false );
    m_pLabelRadioVolume->GetPosition ( vecTemp, false );
    m_pLabelRadioVolume->AutoSize ( "Radio volume:" );

    m_pAudioRadioVolume = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAudio ) );
    m_pAudioRadioVolume->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY ) );
    m_pAudioRadioVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAudioRadioVolume->SetProperty ( "StepSize", "0.01" );

    m_pLabelRadioVolumeValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "0%") );
    m_pLabelRadioVolumeValue->SetPosition ( CVector2D ( vecTemp.fX + 250.0f, vecTemp.fY ) );
    m_pLabelRadioVolumeValue->AutoSize ( "100%" );

    m_pLabelSFXVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "SFX volume:" ) );
    m_pLabelSFXVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pLabelSFXVolume->GetPosition ( vecTemp, false );
    m_pLabelSFXVolume->AutoSize ( "SFX volume:" );

    m_pLabelSFXVolumeValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "0%") );
    m_pLabelSFXVolumeValue->SetPosition ( CVector2D ( vecTemp.fX + 250.0f, vecTemp.fY ) );
    m_pLabelSFXVolumeValue->AutoSize ( "100%" );

    m_pAudioSFXVolume = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAudio ) );
    m_pAudioSFXVolume->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY ) );
    m_pAudioSFXVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAudioSFXVolume->SetProperty ( "StepSize", "0.01" );

    m_pCheckBoxAudioEqualizer = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, "Radio Equalizer", true ) );
    m_pCheckBoxAudioEqualizer->SetPosition ( CVector2D ( vecTemp.fX + 300.0f, vecTemp.fY - 3.0f ) );
    m_pCheckBoxAudioEqualizer->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    m_pCheckBoxAudioAutotune = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, "Radio Auto-tune", true ) );
    m_pCheckBoxAudioAutotune->SetPosition ( CVector2D ( vecTemp.fX + 300.0f, vecTemp.fY + 13.0f ) );
    m_pCheckBoxAudioAutotune->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    m_pLabelMTAVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "MTA volume:" ) );
    m_pLabelMTAVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pLabelMTAVolume->GetPosition ( vecTemp, false );
    m_pLabelMTAVolume->AutoSize ( "MTA volume:" );

    m_pLabelMTAVolumeValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "0%") );
    m_pLabelMTAVolumeValue->SetPosition ( CVector2D ( vecTemp.fX + 250.0f, vecTemp.fY ) );
    m_pLabelMTAVolumeValue->AutoSize ( "100%" );

    m_pAudioMTAVolume = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAudio ) );
    m_pAudioMTAVolume->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY ) );
    m_pAudioMTAVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAudioMTAVolume->SetProperty ( "StepSize", "0.01" );

    m_pAudioUsertrackLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "Usertrack options" ) );
    m_pAudioUsertrackLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 40.0f ), false );
    m_pAudioUsertrackLabel->GetPosition ( vecTemp, false );
    m_pAudioUsertrackLabel->AutoSize ( "Usertrack options  " );
    m_pAudioUsertrackLabel->SetFont ( "default-bold-small" );

    m_pLabelUserTrackMode = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "Play mode:" ) );
    m_pLabelUserTrackMode->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pLabelUserTrackMode->GetPosition ( vecTemp, false );
    m_pLabelUserTrackMode->AutoSize ( "Play mode:" );

    m_pComboUsertrackMode = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAudio, "" ) );
    m_pComboUsertrackMode->SetPosition ( CVector2D ( vecTemp.fX + 80.0f, vecTemp.fY ) );
    m_pComboUsertrackMode->SetSize ( CVector2D ( 160.0f, 80.0f ) );
    m_pComboUsertrackMode->AddItem ( "Radio" )->SetData ( (void*)0 );
    m_pComboUsertrackMode->AddItem ( "Random" )->SetData ( (void*)1 );
    m_pComboUsertrackMode->AddItem ( "Sequential" )->SetData ( (void*)2 );
    m_pComboUsertrackMode->SetReadOnly ( true );

    m_pCheckBoxUserAutoscan = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, "Automatic Media Scan", true ) );
    m_pCheckBoxUserAutoscan->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pCheckBoxUserAutoscan->SetSize ( CVector2D ( 224.0f, 16.0f ) );
    m_pCheckBoxUserAutoscan->GetPosition ( vecTemp, false );

    /**
     *  Video tab
     **/

    m_pVideoGeneralLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "General" ) );
    m_pVideoGeneralLabel->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
    m_pVideoGeneralLabel->GetPosition ( vecTemp, false );
    m_pVideoGeneralLabel->AutoSize ( "General  " );
    m_pVideoGeneralLabel->SetFont ( "default-bold-small" );

    m_pVideoResolutionLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Resolution:" ) );
    m_pVideoResolutionLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
    m_pVideoResolutionLabel->GetPosition ( vecTemp, false );
    m_pVideoResolutionLabel->AutoSize ( "Resolution: " );

    m_pComboResolution = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboResolution->SetPosition ( CVector2D ( vecTemp.fX + 86.0f, vecTemp.fY - 5.0f ) );
    m_pComboResolution->SetSize ( CVector2D ( 200.0f, 160.0f ) );
    m_pComboResolution->SetReadOnly ( true );

    m_pCheckBoxWindowed = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Windowed", true ) );
    m_pCheckBoxWindowed->SetPosition ( CVector2D ( vecTemp.fX + 330.0f, vecTemp.fY - 3.0f ) );
    m_pCheckBoxWindowed->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    float fPosY =  vecTemp.fY;
    m_pCheckBoxMinimize = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Full Screen Minimize", true ) );
    m_pCheckBoxMinimize->SetPosition ( CVector2D ( vecTemp.fX + 330.0f, fPosY + 13.0f ) );
    m_pCheckBoxMinimize->SetSize ( CVector2D ( 224.0f, 16.0f ) );
    if ( !GetVideoModeManager ()->IsMultiMonitor () )
    {
        m_pCheckBoxMinimize->SetVisible ( false );
        fPosY -= 16.0f;
    }

    m_pCheckBoxDisableAero = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Disable Aero Desktop", true ) );
    m_pCheckBoxDisableAero->SetPosition ( CVector2D ( vecTemp.fX + 330.0f, vecTemp.fY + 29.0f ) );
    m_pCheckBoxDisableAero->SetSize ( CVector2D ( 224.0f, 16.0f ) );
    if ( GetApplicationSetting ( "os-version" ) < "6.1" || GetApplicationSettingInt ( "aero-changeable" ) == 0 )
    {
        m_pCheckBoxDisableAero->SetVisible ( false );
        fPosY -= 16.0f;
    }

    m_pCheckBoxMipMapping = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Mip Mapping", true ) );
    m_pCheckBoxMipMapping->SetPosition ( CVector2D ( vecTemp.fX + 330.0f, vecTemp.fY + 45.0f ) );
    m_pCheckBoxMipMapping->SetSize ( CVector2D ( 224.0f, 16.0f ) );
#ifndef MIP_MAPPING_SETTING_APPEARS_TO_DO_SOMETHING
    m_pCheckBoxMipMapping->SetVisible ( false );
    fPosY -= 16.0f;
#endif

    vecTemp.fY -= 5;
    m_pDrawDistanceLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Draw Distance:" ) );
    m_pDrawDistanceLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pDrawDistanceLabel->GetPosition ( vecTemp, false );
    m_pDrawDistanceLabel->AutoSize ( "Draw Distance: " );

    m_pDrawDistance = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabVideo ) );
    m_pDrawDistance->SetPosition ( CVector2D ( vecTemp.fX + 86.0f, vecTemp.fY ) );
    m_pDrawDistance->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pDrawDistance->SetProperty ( "StepSize", "0.01" );

    m_pDrawDistanceValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "0%") );
    m_pDrawDistanceValueLabel->SetPosition ( CVector2D ( vecTemp.fX + 256.0f, vecTemp.fY ) );
    m_pDrawDistanceValueLabel->AutoSize ( "100% " );


    m_pBrightnessLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Brightness:" ) );
    m_pBrightnessLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pBrightnessLabel->GetPosition ( vecTemp, false );
    m_pBrightnessLabel->AutoSize ( "Brightness: " );

    m_pBrightness = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabVideo ) );
    m_pBrightness->SetPosition ( CVector2D ( vecTemp.fX + 86.0f, vecTemp.fY ) );
    m_pBrightness->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pBrightness->SetProperty ( "StepSize", "0.01" );

    m_pBrightnessValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "0%") );
    m_pBrightnessValueLabel->SetPosition ( CVector2D ( vecTemp.fX + 256.0f, vecTemp.fY ) );
    m_pBrightnessValueLabel->AutoSize ( "100% " );

    m_pFXQualityLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "FX Quality:" ) );
    m_pFXQualityLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pFXQualityLabel->GetPosition ( vecTemp, false );
    m_pFXQualityLabel->AutoSize ( "FX Quality: " );

    m_pComboFxQuality = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboFxQuality->SetPosition ( CVector2D ( vecTemp.fX + 86.0f, vecTemp.fY - 1.0f ) );
    m_pComboFxQuality->SetSize ( CVector2D ( 200.0f, 95.0f ) );
    m_pComboFxQuality->AddItem ( "Low" )->SetData ( (void*)0 );
    m_pComboFxQuality->AddItem ( "Medium" )->SetData ( (void*)1 );
    m_pComboFxQuality->AddItem ( "High" )->SetData ( (void*)2 );
    m_pComboFxQuality->AddItem ( "Very high" )->SetData ( (void*)3 );
    m_pComboFxQuality->SetReadOnly ( true );

    m_pCheckBoxVolumetricShadows = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, "Volumetric Shadows", true ) );
    m_pCheckBoxVolumetricShadows->SetPosition ( CVector2D ( vecTemp.fX + 330.0f, vecTemp.fY + 2.0f ) );
    m_pCheckBoxVolumetricShadows->SetSize ( CVector2D ( 224.0f, 16.0f ) );

    m_pAntiAliasingLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Anti-aliasing:" ) );
    m_pAntiAliasingLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pAntiAliasingLabel->GetPosition ( vecTemp, false );
    m_pAntiAliasingLabel->AutoSize ( "Anti-aliasing: " );

    m_pComboAntiAliasing = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboAntiAliasing->SetPosition ( CVector2D ( vecTemp.fX + 86.0f, vecTemp.fY - 1.0f ) );
    m_pComboAntiAliasing->SetSize ( CVector2D ( 200.0f, 95.0f ) );
    m_pComboAntiAliasing->AddItem ( "Off" )->SetData ( (void*)1 );
    m_pComboAntiAliasing->AddItem ( "1x" )->SetData ( (void*)2 );
    m_pComboAntiAliasing->AddItem ( "2x" )->SetData ( (void*)3 );
    m_pComboAntiAliasing->AddItem ( "3x" )->SetData ( (void*)4 );
    m_pComboAntiAliasing->SetReadOnly ( true );


    m_pAspectRatioLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Aspect Ratio:" ) );
    m_pAspectRatioLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pAspectRatioLabel->GetPosition ( vecTemp, false );
    m_pAspectRatioLabel->AutoSize ( "Aspect Ratio: " );

    m_pComboAspectRatio = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboAspectRatio->SetPosition ( CVector2D ( vecTemp.fX + 86.0f, vecTemp.fY - 1.0f ) );
    m_pComboAspectRatio->SetSize ( CVector2D ( 200.0f, 95.0f ) );
    m_pComboAspectRatio->AddItem ( "Auto" )->SetData ( (void*)ASPECT_RATIO_AUTO );
    m_pComboAspectRatio->AddItem ( "4:3" )->SetData ( (void*)ASPECT_RATIO_4_3 );
    m_pComboAspectRatio->AddItem ( "16:10" )->SetData ( (void*)ASPECT_RATIO_16_10 );
    m_pComboAspectRatio->AddItem ( "16:9" )->SetData ( (void*)ASPECT_RATIO_16_9 );
    m_pComboAspectRatio->SetReadOnly ( true );

    m_pStreamingMemoryLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Usable video memory:" ) );
    m_pStreamingMemoryLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pStreamingMemoryLabel->GetPosition ( vecTemp, false );
    m_pStreamingMemoryLabel->AutoSize ( "Usable video memory:" );

    IDirect3DDevice9* pDevice = CCore::GetSingleton().GetGraphics()->GetDevice();
    unsigned int uiMinMemory = SharedUtil::GetMinStreamingMemory ( pDevice );
    unsigned int uiMaxMemory = SharedUtil::GetMaxStreamingMemory ( pDevice );

    m_pStreamingMemory = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabVideo ) );
    m_pStreamingMemory->SetPosition ( CVector2D ( vecTemp.fX + 130.0f, vecTemp.fY ) );
    m_pStreamingMemory->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pStreamingMemory->SetProperty ( "StepSize", SString("%.07lf", 1.0 / (uiMaxMemory - uiMinMemory)) );

    m_pStreamingMemoryValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "0 MB") );
    m_pStreamingMemoryValueLabel->SetPosition ( CVector2D ( vecTemp.fX + 300.0f, vecTemp.fY ) );
    m_pStreamingMemoryValueLabel->AutoSize ( "9999 MB " );

    m_pMapRenderingLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Map rendering options" ) );
    m_pMapRenderingLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 38.0f ) );
    m_pMapRenderingLabel->GetPosition ( vecTemp, false );
    m_pMapRenderingLabel->AutoSize ( "Map rendering options " );
    m_pMapRenderingLabel->SetFont ( "default-bold-small" );

    m_pMapAlphaLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "Opacity:" ) );
    m_pMapAlphaLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 24.0f ) );
    m_pMapAlphaLabel->GetPosition ( vecTemp, false );
    m_pMapAlphaLabel->AutoSize ( "Opacity: " );

    m_pMapAlpha = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabVideo ) );
    m_pMapAlpha->SetPosition ( CVector2D ( vecTemp.fX + 86.0f, vecTemp.fY ) );
    m_pMapAlpha->GetPosition ( vecTemp, false );
    m_pMapAlpha->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pMapAlpha->SetProperty ( "StepSize", "0.01" );

    m_pMapAlphaValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "0%") );
    m_pMapAlphaValueLabel->SetPosition ( CVector2D ( vecTemp.fX + 170.0f, vecTemp.fY ) );
    m_pMapAlphaValueLabel->GetPosition ( vecTemp, false );
    m_pMapAlphaValueLabel->AutoSize ( "100% " );

    /**
     * Interface/chat Tab
     **/
    m_pInterfacePaneScroller = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( pTabInterface ) ); 
    m_pInterfacePaneScroller->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pInterfacePaneScroller->SetPosition ( CVector2D ( 0, 1 ) );
    m_pInterfacePaneScroller->SetSize ( CVector2D ( 539.0f, 292.0f ) );
    m_pInterfacePaneScroller->SetVerticalScrollStepSize ( 0.15f );
    m_pInterfacePaneScroller->SetProperty( "ContentArea", "l:0.000000 t:0.000000 r:0.000000 b:400.000000" ); //Defines the height of the content
    m_pInterfacePaneScroller->SetVerticalScrollBar ( true );

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "General" ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 12.0f ) );
        pLabel->AutoSize ( "General  " );
        pLabel->SetFont ( "default-bold-small" );
    }

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "Skin:" ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 42.0f ) );
        pLabel->AutoSize ( "Skin:" );
    }

    m_pInterfaceSkinSelector = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( m_pInterfacePaneScroller ) );
    m_pInterfaceSkinSelector->SetPosition ( CVector2D ( 60.0f, 40.0f ) );
    m_pInterfaceSkinSelector->SetSize ( CVector2D ( 340.0f, 200.0f ) );
    m_pInterfaceSkinSelector->SetReadOnly ( true );

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "Chat" ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 80.0f ) );
        pLabel->AutoSize ( "Chat  " );
        pLabel->SetFont ( "default-bold-small" );
    }


    // Presets

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "Presets:" ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 112.0f ) );
        pLabel->AutoSize ( "Presets:" );
    }

    m_pChatPresets = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( m_pInterfacePaneScroller ) );
    m_pChatPresets->SetPosition ( CVector2D ( 60.0f, 110.0f ) );
    m_pChatPresets->SetSize ( CVector2D ( 340.0f, 200.0f ) );
    m_pChatPresets->SetReadOnly ( true );

    m_pChatLoadPreset = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pInterfacePaneScroller, "Load" ) );
    m_pChatLoadPreset->SetPosition ( CVector2D ( 410.0f, 110.0f ) );
    m_pChatLoadPreset->SetSize ( CVector2D ( 100.0f, 24.0f ) );

    // Color selection
    CGUITabPanel* pColorTabPanel = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( m_pInterfacePaneScroller ) );
    pColorTabPanel->SetPosition ( CVector2D ( 10.0f, 150.0f ) );
    pColorTabPanel->SetSize ( CVector2D ( 320.0f, 150.0f ) );

    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_BG, "Chat BG", pColorTabPanel );
    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_TEXT, "Chat Text", pColorTabPanel );
    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_INPUT_BG, "Input BG", pColorTabPanel );
    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_INPUT_TEXT, "Input Text", pColorTabPanel );

    // Font Selection
    m_pPaneChatFont = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( m_pInterfacePaneScroller ) ); 
    m_pPaneChatFont->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pPaneChatFont->SetPosition ( CVector2D ( 50.0f, 320.0f ) );
    m_pPaneChatFont->SetSize ( CVector2D ( 250.0f, 33.0f ) );

    CGUILabel* pFontLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pPaneChatFont, "Font:" ) );
    pFontLabel->SetPosition ( CVector2D ( 0.0f, 8.0f ) );
    pFontLabel->AutoSize ( "Font:" );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Tahoma" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ]->SetSelected ( true );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ]->SetPosition ( CVector2D ( 50.0f, 0.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_CLEAR ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Verdana" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_CLEAR ]->SetPosition ( CVector2D ( 150.0f, 0.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_CLEAR ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_BOLD ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Tahoma Bold" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_BOLD ]->SetPosition ( CVector2D ( 50.0f, 18.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_BOLD ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_ARIAL ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Arial" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_ARIAL ]->SetPosition ( CVector2D ( 150.0f, 18.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_ARIAL ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    // Misc. Options
    {
        CGUILabel* pLabel;

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "Lines:" ) );
        pLabel->SetPosition ( CVector2D ( 360.0f, 160.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "Lines:" );

        m_pChatLines = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pInterfacePaneScroller, "" ) );
        m_pChatLines->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatLines->SetSize ( CVector2D ( 110.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "Scale:" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "Scale:" );

        m_pChatScaleX = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pInterfacePaneScroller, "" ) );
        m_pChatScaleX->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatScaleX->SetSize ( CVector2D ( 50.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "x" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 92.5f, vecTemp.fY + 2.0f ) );
        pLabel->AutoSize ( "x" );

        m_pChatScaleY = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pInterfacePaneScroller, "" ) );
        m_pChatScaleY->SetPosition ( CVector2D ( vecTemp.fX + 100.0f, vecTemp.fY - 2.0f ) );
        m_pChatScaleY->SetSize ( CVector2D ( 50.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "Width:" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "Width:" );

        m_pChatWidth = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pInterfacePaneScroller, "" ) );
        m_pChatWidth->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatWidth->SetSize ( CVector2D ( 110.0f, 24.0f ) );

        m_pChatCssBackground = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox( m_pInterfacePaneScroller, "Hide background when\nnot typing" ) );
        m_pChatCssBackground->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        m_pChatCssBackground->GetPosition ( vecTemp );
        m_pChatCssBackground->SetSize ( CVector2D ( 160.0f, 32.0f ) );

        m_pChatCssText = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox( m_pInterfacePaneScroller, "Fade out old lines" ) );
        m_pChatCssText->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 34.0f ) );
        m_pChatCssText->GetPosition ( vecTemp );
        m_pChatCssText->SetSize ( CVector2D ( 160.0f, 16.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "after" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 17.0f, vecTemp.fY + 22.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "after " );

        m_pChatLineLife = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pInterfacePaneScroller, "" ) );
        m_pChatLineLife->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatLineLife->SetSize ( CVector2D ( 70.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "sec" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 115.0f, vecTemp.fY ) );
        pLabel->AutoSize ( "sec" );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "for" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( "for" );

        m_pChatLineFadeout = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pInterfacePaneScroller, "" ) );
        m_pChatLineFadeout->SetPosition ( CVector2D ( vecTemp.fX + 40.0f, vecTemp.fY - 2.0f ) );
        m_pChatLineFadeout->SetSize ( CVector2D ( 70.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pInterfacePaneScroller, "sec" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + 115.0f, vecTemp.fY ) );
        pLabel->AutoSize ( "sec" );
    }

    /**
     *  Advanced tab
     **/
    vecTemp = CVector2D ( 12.f, 12.f );

    // Misc section label
    m_pAdvancedMiscLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Misc" ) );
    m_pAdvancedMiscLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pAdvancedMiscLabel->SetFont ( "default-bold-small" );
    m_pAdvancedMiscLabel->AutoSize ( "Misc" );
    vecTemp.fY += 20;

    // Asynchronous Loading
    m_pAsyncLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Asynchronous Loading:" ) );
    m_pAsyncLabel->SetPosition ( CVector2D ( vecTemp.fX + 10.f, vecTemp.fY ) );
    m_pAsyncLabel->AutoSize ( m_pAsyncLabel->GetText ().c_str () );

    m_pAsyncCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pAsyncCombo->SetPosition ( CVector2D ( vecTemp.fX + 156.0f, vecTemp.fY - 1.0f ) );
    m_pAsyncCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pAsyncCombo->AddItem ( "Off" )->SetData ( (void*)0 );
    m_pAsyncCombo->AddItem ( "On" )->SetData ( (void*)2 );
    m_pAsyncCombo->AddItem ( "Auto" )->SetData ( (void*)1 );
    m_pAsyncCombo->SetReadOnly ( true );

    m_pAsyncLabelInfo = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Experimental feature which\nmay improve performance." ) );
    m_pAsyncLabelInfo->SetPosition ( CVector2D ( vecTemp.fX + 342.f, vecTemp.fY - 4.f ) );
    m_pAsyncLabelInfo->SetFont ( "default-bold-small" );
    m_pAsyncLabelInfo->SetSize ( CVector2D ( 168.0f, 95.0f ) );
    vecTemp.fY += 40-4;

    // Browser scan speed
    m_pBrowserSpeedLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Browser speed:" ) );
    m_pBrowserSpeedLabel->SetPosition ( CVector2D ( vecTemp.fX + 10.f, vecTemp.fY ) );
    m_pBrowserSpeedLabel->AutoSize ( m_pBrowserSpeedLabel->GetText ().c_str () );

    m_pBrowserSpeedCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pBrowserSpeedCombo->SetPosition ( CVector2D ( vecTemp.fX + 156.0f, vecTemp.fY - 1.0f ) );
    m_pBrowserSpeedCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pBrowserSpeedCombo->AddItem ( "Very slow" )->SetData ( (void*)0 );
    m_pBrowserSpeedCombo->AddItem ( "Slow" )->SetData ( (void*)1 );
    m_pBrowserSpeedCombo->AddItem ( "Fast" )->SetData ( (void*)2 );
    m_pBrowserSpeedCombo->SetReadOnly ( true );

    m_pBrowserSpeedLabelInfo = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Older routers may require\na slower scan speed." ) );
    m_pBrowserSpeedLabelInfo->SetPosition ( CVector2D ( vecTemp.fX + 342.f, vecTemp.fY - 4.f ) );
    m_pBrowserSpeedLabelInfo->SetFont ( "default-bold-small" );
    m_pBrowserSpeedLabelInfo->SetSize ( CVector2D ( 168.0f, 95.0f ) );
    vecTemp.fY += 40-4;

    // Single download
    m_pSingleDownloadLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Single connection:" ) );
    m_pSingleDownloadLabel->SetPosition ( CVector2D ( vecTemp.fX + 10.f, vecTemp.fY ) );
    m_pSingleDownloadLabel->AutoSize ( m_pSingleDownloadLabel->GetText ().c_str () );

    m_pSingleDownloadCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pSingleDownloadCombo->SetPosition ( CVector2D ( vecTemp.fX + 156.0f, vecTemp.fY - 1.0f ) );
    m_pSingleDownloadCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pSingleDownloadCombo->AddItem ( "Default" )->SetData ( (void*)0 );
    m_pSingleDownloadCombo->AddItem ( "On" )->SetData ( (void*)1 );
    m_pSingleDownloadCombo->SetReadOnly ( true );

    m_pSingleDownloadLabelInfo = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Switch on to use only one\nconnection when downloading." ) );
    m_pSingleDownloadLabelInfo->SetPosition ( CVector2D ( vecTemp.fX + 342.f, vecTemp.fY - 4.f ) );
    m_pSingleDownloadLabelInfo->SetFont ( "default-bold-small" );
    m_pSingleDownloadLabelInfo->SetSize ( CVector2D ( 168.0f, 95.0f ) );
    vecTemp.fY += 40-4;

    // Code path
    m_pCodePathLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Code path:" ) );
    m_pCodePathLabel->SetPosition ( CVector2D ( vecTemp.fX + 10.f, vecTemp.fY ) );
    m_pCodePathLabel->AutoSize ( m_pCodePathLabel->GetText ().c_str () );

    m_pCodePathCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pCodePathCombo->SetPosition ( CVector2D ( vecTemp.fX + 156.0f, vecTemp.fY - 1.0f ) );
    m_pCodePathCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pCodePathCombo->AddItem ( "Default" )->SetData ( (void*)0 );
    m_pCodePathCombo->AddItem ( "Alternate" )->SetData ( (void*)1 );
    m_pCodePathCombo->SetReadOnly ( true );

    m_pCodePathLabelInfo = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Experimental crash reduction." ) );
    m_pCodePathLabelInfo->SetPosition ( CVector2D ( vecTemp.fX + 342.f, vecTemp.fY - 4.f ) );
    m_pCodePathLabelInfo->SetFont ( "default-bold-small" );
    m_pCodePathLabelInfo->SetSize ( CVector2D ( 168.0f, 95.0f ) );
    vecTemp.fY += 35;

    // Auto updater section label
    m_pAdvancedUpdaterLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Auto updater" ) );
    m_pAdvancedUpdaterLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pAdvancedUpdaterLabel->SetFont ( "default-bold-small" );
    m_pAdvancedUpdaterLabel->AutoSize ( "Auto updater" );
    vecTemp.fY += 20;

    // Update build type
    m_pUpdateBuildTypeLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Update build type:" ) );
    m_pUpdateBuildTypeLabel->SetPosition ( CVector2D ( vecTemp.fX + 10.f, vecTemp.fY ) );
    m_pUpdateBuildTypeLabel->AutoSize ( m_pUpdateBuildTypeLabel->GetText ().c_str () );

    m_pUpdateBuildTypeCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pUpdateBuildTypeCombo->SetPosition ( CVector2D ( vecTemp.fX + 156.0f, vecTemp.fY - 1.0f ) );
    m_pUpdateBuildTypeCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pUpdateBuildTypeCombo->AddItem ( "Default" )->SetData ( (void*)0 );
    m_pUpdateBuildTypeCombo->AddItem ( "Beta" )->SetData ( (void*)1 );
    m_pUpdateBuildTypeCombo->AddItem ( "Nightly" )->SetData ( (void*)2 );
    m_pUpdateBuildTypeCombo->SetReadOnly ( true );

    m_pUpdateBuildTypeLabelInfo = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Select default unless you like\nfilling out bug reports." ) );
    m_pUpdateBuildTypeLabelInfo->SetPosition ( CVector2D ( vecTemp.fX + 342.f, vecTemp.fY - 4.f ) );
    m_pUpdateBuildTypeLabelInfo->SetFont ( "default-bold-small" );
    m_pUpdateBuildTypeLabelInfo->SetSize ( CVector2D ( 168.0f, 95.0f ) );
    vecTemp.fY += 35;

    // Check for updates
    m_pButtonUpdate = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabAdvanced, "Check for update now" ) );
    m_pButtonUpdate->SetPosition ( CVector2D ( vecTemp.fX + 10.f, vecTemp.fY ) );
    m_pButtonUpdate->SetSize ( CVector2D ( 168.0f, 24.0f ) );
    m_pButtonUpdate->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnUpdateButtonClick, this ) );
    vecTemp.fY += 30;

    // Set up the events
    m_pWindow->SetEnterKeyHandler ( GUI_CALLBACK ( &CSettings::OnOKButtonClick, this ) );
    m_pButtonOK->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnOKButtonClick, this ) );
    m_pButtonCancel->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnCancelButtonClick, this ) );
    m_pButtonLogin->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnLoginButtonClick, this ) );
    m_pButtonRegister->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnRegisterButtonClick, this ) );
    m_pChatLoadPreset->SetClickHandler ( GUI_CALLBACK( &CSettings::OnChatLoadPresetClick, this ) );
    m_pInterfaceSkinSelector->SetSelectionHandler ( GUI_CALLBACK(&CSettings::OnSkinChanged, this) );
    m_pMapAlpha->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnMapAlphaChanged, this ) );
    m_pAudioRadioVolume->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnRadioVolumeChanged, this ) );
    m_pAudioSFXVolume->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnSFXVolumeChanged, this ) );
    m_pAudioMTAVolume->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnMTAVolumeChanged, this ) );
    m_pDrawDistance->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnDrawDistanceChanged, this ) );
    m_pBrightness->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnBrightnessChanged, this ) );
    m_pMouseSensitivity->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnMouseSensitivityChanged, this ) );
    m_pComboFxQuality->SetSelectionHandler ( GUI_CALLBACK( &CSettings::OnFxQualityChanged, this ) );
    m_pCheckBoxVolumetricShadows->SetClickHandler ( GUI_CALLBACK( &CSettings::OnVolumetricShadowsClick, this ) );
    m_pStreamingMemory->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnStreamingMemoryChanged, this ) );
    /*
    // Give a warning if no community account settings were stored in config
    CCore::GetSingleton ().ShowMessageBox ( CORE_SETTINGS_COMMUNITY_WARNING, "Multi Theft Auto: Community settings", MB_ICON_WARNING );
    */

    // Load Chat presets
    LoadChatPresets ();

    // Load the load of skins
    LoadSkins();
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

    GetJoystickManager ()->SaveToXML ();
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
    snprintf ( szDeadzone, 10, "%d", JoyMan->GetDeadZone () );
    snprintf ( szSaturation, 10, "%d", JoyMan->GetSaturation () );

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
    GetJoystickManager ()->SetDefaults ();

    // Load the default mouse settings
    CControllerConfigManager * pController = g_pCore->GetGame ()->GetControllerConfigManager ();   

    m_pFlyWithMouse->SetSelected ( pController->GetFlyWithMouse () );
    m_pSteerWithMouse->SetSelected ( pController->GetSteerWithMouse () );
    m_pInvertMouse->SetSelected ( pController->IsMouseInverted () );

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

    SString strResource;

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
        // If it is a resource name
        if ( ucType == 255 )
        {
            strResource = m_pBindsList->GetItemText ( i, m_hBind );
        }
        // If the type is control
        else if ( ucType == KEY_BIND_GTA_CONTROL )
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
                if ( strResource.empty() )
                    pKeyBinds->AddCommand ( pPriKey, szCommand, szArguments );
                else
                    pKeyBinds->AddCommand ( pPriKey->szKey, szCommand, szArguments, true, strResource.c_str () );
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
                    if ( strResource.empty() )
                        pKeyBinds->AddCommand ( pSecKeys[k], szCommand, szArguments );
                    else
                        pKeyBinds->AddCommand ( pSecKeys[k]->szKey, szCommand, szArguments, true, strResource.c_str (), true );

                    // Also add a matching "up" state if applicable
                    CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand ( szCommand, NULL, true, pPriKey->szKey, true, false );
                    if ( pUpBind )
                    {
                        pKeyBinds->AddCommand ( pSecKeys[k]->szKey, szCommand, pUpBind->szArguments, false, pUpBind->szResource, true );
                    }
                }
            }
        }
        else
        {
            assert ( 0 );
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
    //iRows = CORE_SETTINGS_HEADERS;            // (game keys), (multiplayer keys)
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
        CCommandBind* pBind;
        unsigned int uiMatchCount;
    };

    SListedCommand* listedCommands = new SListedCommand [ pKeyBinds->Count ( KEY_BIND_COMMAND ) + pKeyBinds->Count ( KEY_BIND_FUNCTION ) ];
    unsigned int uiNumListedCommands = 0;
    std::map < std::string, int > iResourceItems;
    // Loop through all the bound commands
    list < CKeyBind* > ::const_iterator iter = pKeyBinds->IterBegin ();
    for ( unsigned int uiIndex = 0 ; iter != pKeyBinds->IterEnd (); iter++, uiIndex++ )
    {
        // keys bound to a console command or a function (we don't show keys bound 
        // from gta controls by scripts as these are clearly not user editable)
        if ( (*iter)->GetType () == KEY_BIND_COMMAND )
        {
            CCommandBind* pCommandBind = reinterpret_cast < CCommandBind* > ( *iter );
            if ( pCommandBind->bHitState )
            {
                bool bFoundMatches = false;
                // Loop through the already listed array of commands for matches
                for ( unsigned int i = 0 ; i < uiNumListedCommands ; i++ )
                {
                    SListedCommand* pListedCommand = &listedCommands [ i ];
                    CCommandBind* pListedBind = pListedCommand->pBind;
                    if ( !strcmp ( pListedBind->szCommand, pCommandBind->szCommand ) )
                    {
                        if ( !pListedBind->szArguments || ( pCommandBind->szArguments && !strcmp ( pListedBind->szArguments, pCommandBind->szArguments ) ) )
                        {
                            // If we found a 1st match, add it to the secondary section
                            bFoundMatches = true;
                            for ( int k = 0 ; k < SecKeyNum ; k++ )
                                if ( pListedCommand->uiMatchCount == k )
                                {
                                    m_pBindsList->SetItemText ( pListedCommand->iIndex, m_hSecKeys[k], pCommandBind->boundKey->szKey );
                                    m_pBindsList->SetItemData ( pListedCommand->iIndex, m_hSecKeys[k], pCommandBind );
                                }
                            pListedCommand->uiMatchCount++;
                        }
                    }
                }

                // If there weren't any matches
                if ( !bFoundMatches )
                {
                    unsigned int row = iGameRowCount + 1;
                    // Combine command and arguments
                    SString strDescription;
                    bool bSkip = false;
                    if ( pCommandBind->szResource )
                    {
                        if ( pCommandBind->bActive )
                        {
                            const char* szResource = pCommandBind->szResource;
                            std::string strResource = szResource;
                            if ( iResourceItems.count(strResource) == 0 )
                            {
                                iBind = m_pBindsList->AddRow ( true );
                                m_pBindsList->SetItemText ( iBind, m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true );

                                iBind = m_pBindsList->AddRow ( true );
                                m_pBindsList->SetItemText ( iBind, m_hBind, szResource, false, true );
                                m_pBindsList->SetItemData ( iBind, m_hBind, (void*) 255 );
                                iResourceItems.insert( make_pair(strResource, iBind ) );
                            }
                            row = iResourceItems[strResource];
                            iMultiplayerRowCount++;
                        }
                        else
                            continue;
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

                    if ( !bSkip )
                    {
                        // Add the bind to the list
                        iBind = m_pBindsList->AddRow ( true );
                        m_pBindsList->SetItemText ( iBind, m_hBind, strDescription );
                        m_pBindsList->SetItemText ( iBind, m_hPriKey, pCommandBind->boundKey->szKey );
                        for ( int k = 0 ; k < SecKeyNum ; k++ )
                            m_pBindsList->SetItemText ( iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY );
                        m_pBindsList->SetItemData ( iBind, m_hBind, (void*) KEY_BIND_COMMAND );
                        m_pBindsList->SetItemData ( iBind, m_hPriKey, pCommandBind );

                        // Add it to the already-listed array
                        SListedCommand* pListedCommand = &listedCommands [ uiNumListedCommands ];
                        pListedCommand->iIndex = iBind;
                        pListedCommand->pBind = pCommandBind;
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
        m_pWindow->BringToFront ();
        m_pWindow->Activate ();
        LoadData ();

        // Clear the binds list
        m_pBindsList->Clear ();

        // Re-initialize the binds list
        Initialize ();
    }

    m_pWindow->SetZOrderingEnabled(!bVisible); //Message boxes dont appear on top otherwise
}


bool CSettings::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}

void CSettings::SetIsModLoaded ( bool bLoaded )
{
    m_bIsModLoaded = bLoaded;
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

    // restore old audio settings
    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();
    gameSettings->SetRadioVolume ( m_ucOldRadioVolume );
    gameSettings->SetSFXVolume ( m_ucOldSFXVolume );
    CVARS_SET ( "mtavolume", m_fOldMTAVolume );

    return true;
}


bool CSettings::OnLoginButtonClick ( CGUIElement* pElement )
{
    if ( strcmp ( m_pButtonLogin->GetText().c_str(), "Login" ) == 0 )
    {
        if ( m_pEditUser->GetText().empty() ||
            m_pEditPass->GetText().empty() )
        {
            g_pCore->ShowMessageBox ( "Login Error", "Invalid username/password", MB_BUTTON_OK | MB_ICON_ERROR );
            return true;
        }
        else
        {
            m_pEditUser->SetEnabled ( false );
            m_pEditPass->SetEnabled ( false );
            m_pButtonLogin->SetEnabled ( false );
            m_pButtonLogin->SetText ( "Logging in..." );

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
        g_pCore->ShowMessageBox ( "Login Error", szError, MB_BUTTON_OK | MB_ICON_ERROR );
    }
}


void CSettings::OnLoginStateChange ( bool bResult )
{   // This function is called by CCommunity whenever the logged-in status changes
    m_pEditUser->SetEnabled ( !bResult );
    m_pEditPass->SetEnabled ( !bResult );
    m_pButtonLogin->SetText ( ( bResult ) ? "Logout" : "Login" );
    m_pButtonLogin->SetEnabled ( true );
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
        m_pEditNick->SetText ( GenerateNickname() );
    }

    // Save server password
    CVARS_GET ( "save_server_passwords", bVar ); m_pSavePasswords->SetSelected ( bVar );
    CVARS_GET ( "auto_refresh_browser", bVar ); m_pAutoRefreshBrowser->SetSelected ( bVar );

    // Controls
    CVARS_GET ( "invert_mouse", bVar ); m_pInvertMouse->SetSelected ( bVar );
    CVARS_GET ( "steer_with_mouse", bVar ); m_pSteerWithMouse->SetSelected ( bVar );
    CVARS_GET ( "fly_with_mouse", bVar ); m_pFlyWithMouse->SetSelected ( bVar );
    CVARS_GET ( "classic_controls", bVar ); m_pClassicControls->SetSelected ( bVar );

    CGameSettings * gameSettings = CCore::GetSingleton ( ).GetGame ( )->GetSettings();

    m_pMouseSensitivity->SetScrollPosition ( ( gameSettings->GetMouseSensitivity () - 0.000312f ) / 0.004688f );

    // Community
    CVARS_GET ( "community_username", strVar );
    if ( !strVar.empty () ) m_pEditUser->SetText ( strVar.c_str () );
    CVARS_GET ( "community_password", strVar );
    if ( !strVar.empty () ) m_pEditPass->SetText ( strVar.c_str () );

    // Audio
    m_ucOldRadioVolume = gameSettings->GetRadioVolume();
    m_pAudioRadioVolume->SetScrollPosition ( (float)m_ucOldRadioVolume / 64.0f );
    m_ucOldSFXVolume = gameSettings->GetSFXVolume();
    m_pAudioSFXVolume->SetScrollPosition ( (float)m_ucOldSFXVolume / 64.0f );

    m_pCheckBoxAudioEqualizer->SetSelected ( gameSettings->IsRadioEqualizerEnabled () );
    m_pCheckBoxAudioAutotune->SetSelected ( gameSettings->IsRadioAutotuneEnabled () );
    m_pCheckBoxUserAutoscan->SetSelected ( gameSettings->IsUsertrackAutoScan () );

    unsigned int uiUsertrackMode = gameSettings->GetUsertrackMode ();
    if ( uiUsertrackMode == 0 ) m_pComboUsertrackMode->SetText ( "Radio" );
    else if ( uiUsertrackMode == 1 ) m_pComboUsertrackMode->SetText ( "Random" );
    else if ( uiUsertrackMode == 2 ) m_pComboUsertrackMode->SetText ( "Sequential" );
    
    CVARS_GET ( "mtavolume", m_fOldMTAVolume );
    m_fOldMTAVolume = max( 0.0f, min( 1.0f, m_fOldMTAVolume ) );
    m_pAudioMTAVolume->SetScrollPosition ( m_fOldMTAVolume );

    // Video
    int nextVideoMode;
    bool bNextWindowed;
    bool bNextFSMinimize;
    GetVideoModeManager ()->GetNextVideoMode ( nextVideoMode, bNextWindowed, bNextFSMinimize );

	m_pCheckBoxMipMapping->SetSelected ( gameSettings->IsMipMappingEnabled () );
    m_pCheckBoxWindowed->SetSelected ( bNextWindowed );
    m_pCheckBoxMinimize->SetSelected ( bNextFSMinimize );
    m_pCheckBoxDisableAero->SetSelected ( GetApplicationSettingInt ( "aero-enabled" ) ? false : true );
    m_pDrawDistance->SetScrollPosition ( ( gameSettings->GetDrawDistance () - 0.925f ) / 0.8749f );
    m_pBrightness->SetScrollPosition ( ( float )gameSettings->GetBrightness () / 384 );

    int FxQuality = gameSettings->GetFXQuality();
    if ( FxQuality == 0 ) m_pComboFxQuality->SetText ( "Low" );
    else if ( FxQuality == 1 ) m_pComboFxQuality->SetText ( "Medium" );
    else if ( FxQuality == 2 ) m_pComboFxQuality->SetText ( "High" );
    else if ( FxQuality == 3 ) m_pComboFxQuality->SetText ( "Very high" );

    char AntiAliasing = gameSettings->GetAntiAliasing();
    if ( AntiAliasing == 1 ) m_pComboAntiAliasing->SetText ( "Off" );
    else if ( AntiAliasing == 2 ) m_pComboAntiAliasing->SetText ( "1x" );
    else if ( AntiAliasing == 3 ) m_pComboAntiAliasing->SetText ( "2x" );
    else if ( AntiAliasing == 4 ) m_pComboAntiAliasing->SetText ( "3x" );

    // Aspect ratio
    int aspectRatio;
    CVARS_GET("aspect_ratio", aspectRatio);
    if ( aspectRatio == ASPECT_RATIO_AUTO ) m_pComboAspectRatio->SetText ( "Auto" );
    else if ( aspectRatio == ASPECT_RATIO_4_3 ) m_pComboAspectRatio->SetText ( "4:3" );
    else if ( aspectRatio == ASPECT_RATIO_16_10 ) m_pComboAspectRatio->SetText ( "16:10" );
    else if ( aspectRatio == ASPECT_RATIO_16_9 ) m_pComboAspectRatio->SetText ( "16:9" );

    // Volumetric shadows
    bool bVolumetricShadowsEnabled;
    CVARS_GET("volumetric_shadows", bVolumetricShadowsEnabled);
	m_pCheckBoxVolumetricShadows->SetSelected ( bVolumetricShadowsEnabled );
	m_pCheckBoxVolumetricShadows->SetEnabled ( FxQuality != 0 );

    VideoMode           vidModemInfo;
    int                 vidMode, numVidModes;

    m_pComboResolution->Clear ();
    numVidModes = gameSettings->GetNumVideoModes();

    for (vidMode = 0; vidMode < numVidModes; vidMode++)
    {
        gameSettings->GetVideoModeInfo(&vidModemInfo, vidMode);

        // Remove resolutions that will make the gui unusable
        if ( vidModemInfo.width < 640 || vidModemInfo.height < 480 )
            continue;

        // Check resolution hasn't already been added
        bool bDuplicate = false;
        for ( int i = 1; i < vidMode ; i++ )
        {
            VideoMode info;
            gameSettings->GetVideoModeInfo(&info, i);
            if ( info.width == vidModemInfo.width && info.height == vidModemInfo.height && info.depth == vidModemInfo.depth )
                bDuplicate = true;
        }
        if ( bDuplicate )
            continue;

        SString strMode ( "%lu x %lu x %lu", vidModemInfo.width, vidModemInfo.height, vidModemInfo.depth );

        if ( vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE )
            m_pComboResolution->AddItem ( strMode )->SetData ( (void*)vidMode );

        VideoMode currentInfo;
        gameSettings->GetVideoModeInfo ( &currentInfo, nextVideoMode );

        if ( currentInfo.width == vidModemInfo.width && currentInfo.height == vidModemInfo.height && currentInfo.depth == vidModemInfo.depth )
            m_pComboResolution->SetText ( strMode );
    }

    // Skins
    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);
    unsigned int uiIndex = 0;
    std::string strItemText = m_pInterfaceSkinSelector->GetItemText( uiIndex );
    while ( strItemText != currentSkin )
    {
        strItemText = m_pInterfaceSkinSelector->GetItemText( ++uiIndex );
    }
    m_pInterfaceSkinSelector->SetSelectedItemByIndex(uiIndex);

    // Async loading
    int iVar;
    CVARS_GET ( "async_loading", iVar );
    if ( iVar == 0 ) m_pAsyncCombo->SetText ( "Off" );
    else if ( iVar == 1 ) m_pAsyncCombo->SetText ( "Auto" );
    else if ( iVar == 2 ) m_pAsyncCombo->SetText ( "On" );

    // Browser speed
    CVARS_GET ( "browser_speed", iVar );
    if ( iVar == 0 ) m_pBrowserSpeedCombo->SetText ( "Very slow" );
    else if ( iVar == 1 ) m_pBrowserSpeedCombo->SetText ( "Slow" );
    else if ( iVar == 2 ) m_pBrowserSpeedCombo->SetText ( "Fast" );

    // Single download
    CVARS_GET ( "single_download", iVar );
    if ( iVar == 0 ) m_pSingleDownloadCombo->SetText ( "Default" );
    else if ( iVar == 1 ) m_pSingleDownloadCombo->SetText ( "On" );

    // Code path
    CVARS_GET ( "code_path", iVar );
    if ( iVar == 0 ) m_pCodePathCombo->SetText ( "Default" );
    else if ( iVar == 1 ) m_pCodePathCombo->SetText ( "Alternate" );

    // Update build type
    CVARS_GET ( "update_build_type", iVar );
    if ( iVar == 0 ) m_pUpdateBuildTypeCombo->SetText ( "Default" );
    else if ( iVar == 1 ) m_pUpdateBuildTypeCombo->SetText ( "Beta" );
    else if ( iVar == 2 ) m_pUpdateBuildTypeCombo->SetText ( "Nightly" );

    // Map alpha
    CVARS_GET ( "mapalpha", iVar);
    int iAlphaPercent = ceil( ( (float)Clamp ( 0, iVar, 255 ) / 255 ) * 100 );
    m_pMapAlphaValueLabel->SetText ( SString("%i%%", iAlphaPercent).c_str() );
    float sbPos = (float)iAlphaPercent / 100.0f;
    m_pMapAlpha->SetScrollPosition ( sbPos );

    // Chat
    LoadChatColorFromCVar ( ChatColorTypes::CHAT_COLOR_BG, "chat_color" );
    LoadChatColorFromCVar ( ChatColorTypes::CHAT_COLOR_TEXT, "chat_text_color" );
    LoadChatColorFromCVar ( ChatColorTypes::CHAT_COLOR_INPUT_BG, "chat_input_color" );
    LoadChatColorFromCVar ( ChatColorTypes::CHAT_COLOR_INPUT_TEXT, "chat_input_text_color" );

    unsigned int uiFont;
    CVARS_GET ( "chat_font", uiFont );
    if ( uiFont >= ChatFonts::CHAT_FONT_MAX )
        uiFont = ChatFonts::CHAT_FONT_DEFAULT;
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

    // Streaming memory
    IDirect3DDevice9* pDevice = CCore::GetSingleton().GetGraphics()->GetDevice ();
    unsigned int uiStreamingMemory;
    CVARS_GET ( "streaming_memory", uiStreamingMemory );
    uiStreamingMemory = SharedUtil::Clamp ( SharedUtil::GetMinStreamingMemory(pDevice), uiStreamingMemory, SharedUtil::GetMaxStreamingMemory(pDevice) );
    float fPos = SharedUtil::Unlerp ( SharedUtil::GetMinStreamingMemory(pDevice), uiStreamingMemory, SharedUtil::GetMaxStreamingMemory(pDevice) );
    m_pStreamingMemory->SetScrollPosition ( fPos );
    m_pStreamingMemoryValueLabel->SetText ( SString ( "%u MB", uiStreamingMemory ) );
}

void RestartCallBack ( void* ptr, unsigned int uiButton )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();
    
    if ( uiButton == 1 )
    {
        SetOnQuitCommand ( "restart" );
        CCore::GetSingleton ().Quit ();
    }
}

void CSettings::SaveData ( void )
{
    std::string strVar;
    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();

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

    // Server pass saving
    bool bServerPasswords = m_pSavePasswords->GetSelected ();
    CVARS_SET ( "save_server_passwords", bServerPasswords );
    if ( !bServerPasswords )
    {
        g_pCore->GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetServerBrowser ()->ClearServerPasswords ();
    }

    CVARS_SET ( "auto_refresh_browser", m_pAutoRefreshBrowser->GetSelected () );

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

    // Video
    // get current
    int iNextVidMode;
    bool bNextWindowed;
    bool bNextFSMinimize;
    GetVideoModeManager ()->GetNextVideoMode ( iNextVidMode, bNextWindowed, bNextFSMinimize );
    int iAntiAliasing = gameSettings->GetAntiAliasing ();

    // update from gui
    bNextWindowed = m_pCheckBoxWindowed->GetSelected ();
    if ( CGUIListItem* pSelected = m_pComboResolution->GetSelectedItem () )
        iNextVidMode = ( int ) pSelected->GetData();
    bNextFSMinimize = m_pCheckBoxMinimize->GetSelected();
    if ( CGUIListItem* pSelected = m_pComboAntiAliasing->GetSelectedItem () )
        iAntiAliasing = ( int ) pSelected->GetData();
    int iAeroEnabled = m_pCheckBoxDisableAero->GetSelected() ? 0 : 1;

    // change
    bool bIsVideoModeChanged = GetVideoModeManager ()->SetVideoMode ( iNextVidMode, bNextWindowed, bNextFSMinimize );
    bool bIsAntiAliasingChanged = gameSettings->GetAntiAliasing () != iAntiAliasing;
    bool bIsAeroChanged = GetApplicationSettingInt ( "aero-enabled"  ) != iAeroEnabled;
    if ( bIsVideoModeChanged || bIsAntiAliasingChanged || bIsAeroChanged )
    {
        SString strChangedOptions;
        if ( bIsVideoModeChanged )
        {
            strChangedOptions += "Resolution";
            if ( bNextFSMinimize != GetVideoModeManager ()->IsMinimizeEnabled () )
                strChangedOptions += "/Full Screen Minimize";
        }

        if ( bIsAntiAliasingChanged )
        {
            if ( !strChangedOptions.empty () )
                strChangedOptions += " and ";
            strChangedOptions += "Anti-aliasing";
        }

        if ( bIsAeroChanged )
        {
            if ( !strChangedOptions.empty () )
                strChangedOptions += " and ";
            strChangedOptions += "Aero setting";
        }

        SString strMessage ( "%s will be changed when you next start MTA", strChangedOptions.c_str () );
        strMessage += "\n\nDo you want to restart now?";
        CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
        pQuestionBox->Reset ();
        pQuestionBox->SetTitle ( "RESTART REQUIRED" );
        pQuestionBox->SetMessage ( strMessage );
        pQuestionBox->SetButton ( 0, "No" );
        pQuestionBox->SetButton ( 1, "Yes" );
        pQuestionBox->SetCallback ( RestartCallBack );
        pQuestionBox->Show ();
    }

    gameSettings->SetAntiAliasing ( iAntiAliasing, true );
    gameSettings->SetDrawDistance ( ( m_pDrawDistance->GetScrollPosition () * 0.875f ) + 0.925f );
    gameSettings->SetBrightness ( m_pBrightness->GetScrollPosition () * 384 );
    gameSettings->SetMouseSensitivity ( ( m_pMouseSensitivity->GetScrollPosition () * 0.004688f ) + 0.000312f );
	gameSettings->SetMipMappingEnabled ( m_pCheckBoxMipMapping->GetSelected () );

    // Update Aero override setting. This need to be a registry setting as it's done in the launcher
    SetApplicationSettingInt ( "aero-enabled", m_pCheckBoxDisableAero->GetSelected() ? 0 : 1 );

    // Visual FX Quality
    if ( CGUIListItem* pQualitySelected = m_pComboFxQuality->GetSelectedItem () )
    {
        gameSettings->SetFXQuality ( ( int ) pQualitySelected->GetData() );
    }

    // Aspect ratio
    if ( CGUIListItem* pRatioSelected = m_pComboAspectRatio->GetSelectedItem () )
    {
        eAspectRatio aspectRatio = ( eAspectRatio ) ( int ) pRatioSelected->GetData();
        CVARS_SET("aspect_ratio", aspectRatio);
	    gameSettings->SetAspectRatio ( aspectRatio );
    }

    // Volumetric shadows
    bool bVolumetricShadowsEnabled = m_pCheckBoxVolumetricShadows->GetSelected ();
    CVARS_SET ( "volumetric_shadows", bVolumetricShadowsEnabled );
	gameSettings->SetVolumetricShadowsEnabled ( bVolumetricShadowsEnabled );

    // Async loading
    if ( CGUIListItem* pSelected = m_pAsyncCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "async_loading", iSelected );
        g_pCore->GetGame ()->SetAsyncLoadingFromSettings ( iSelected == 1, iSelected == 2 );
    }

    // Audio
    gameSettings->SetRadioEqualizerEnabled ( m_pCheckBoxAudioEqualizer->GetSelected() );
    gameSettings->SetRadioAutotuneEnabled ( m_pCheckBoxAudioAutotune->GetSelected() );
    gameSettings->SetUsertrackAutoScan ( m_pCheckBoxUserAutoscan->GetSelected() );

    if ( CGUIListItem* pSelected = m_pComboUsertrackMode->GetSelectedItem () )
    {
        gameSettings->SetUsertrackMode ( (int) pSelected->GetData() );
    }

    // Browser speed
    if ( CGUIListItem* pSelected = m_pBrowserSpeedCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "browser_speed", iSelected );
    }

    // Single download
    if ( CGUIListItem* pSelected = m_pSingleDownloadCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "single_download", iSelected );
    }

    // Code path
    if ( CGUIListItem* pSelected = m_pCodePathCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "code_path", iSelected );
    }

    // Update build type
    if ( CGUIListItem* pSelected = m_pUpdateBuildTypeCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "update_build_type", iSelected );
    }

    // Map alpha
    SString sText = m_pMapAlphaValueLabel->GetText ();

    float fMapAlpha = ( ( atof(sText.substr(0, sText.length() - 1 ).c_str() )) / 100 ) * 255;
    CVARS_SET ( "mapalpha", fMapAlpha );

    // Chat
    SaveChatColor ( ChatColorTypes::CHAT_COLOR_BG, "chat_color" );
    SaveChatColor ( ChatColorTypes::CHAT_COLOR_TEXT, "chat_text_color" );
    SaveChatColor ( ChatColorTypes::CHAT_COLOR_INPUT_BG, "chat_input_color" );
    SaveChatColor ( ChatColorTypes::CHAT_COLOR_INPUT_TEXT, "chat_input_text_color" );
    for ( int iFont = 0; iFont < ChatColorTypes::CHAT_COLOR_MAX; iFont ++ )
    {
        if ( m_pRadioChatFont [ iFont ]->GetSelected( ) )
        {
            CVARS_SET ( "chat_font", iFont );
            break;
        }
    }

    strVar = m_pChatScaleX->GetText () + " " + m_pChatScaleY->GetText ();
    CVARS_SET ( "chat_scale", strVar );

    // Test error reporter
    if ( m_pChatLines->GetText () == "crashme" )
        *((int*)NULL) = 0;      

    CVARS_SET ( "chat_lines", m_pChatLines->GetText () );
    CVARS_SET ( "chat_width", m_pChatWidth->GetText () );
    CVARS_SET ( "chat_css_style_text", m_pChatCssText->GetSelected () );
    CVARS_SET ( "chat_css_style_background", m_pChatCssBackground->GetSelected () );
    CVARS_SET ( "chat_line_life", GetMilliseconds ( m_pChatLineLife ) );
    CVARS_SET ( "chat_line_fade_out", GetMilliseconds ( m_pChatLineFadeout ) );

    // Set our new skin last, as it'll destroy all our GUI
    CGUIListItem* pItem = m_pInterfaceSkinSelector->GetSelectedItem ();
    if ( pItem )
        CVARS_SET("current_skin", pItem->GetText());

    // Streaming memory
    float fPos = m_pStreamingMemory->GetScrollPosition ();
    IDirect3DDevice9* pDevice = CCore::GetSingleton().GetGraphics()->GetDevice ();
    int min = SharedUtil::GetMinStreamingMemory ( pDevice );
    int max = SharedUtil::GetMaxStreamingMemory ( pDevice );
    unsigned int value = SharedUtil::Lerp ( min, fPos, max );
    CVARS_SET ( "streaming_memory", value );

    // Save the config here
    CCore::GetSingleton ().SaveConfig ();
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

    m_pChatRedValue [ eType ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "0" ) );
    m_pChatRedValue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY) );
    m_pChatRedValue [ eType ]->AutoSize ( "255 " );

    m_pChatRed [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatRed [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatRed [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
    m_pChatRed [ eType ]->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnChatRedChanged, this ) );
    m_pChatRed [ eType ]->SetProperty ( "StepSize", "0.004" );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "Green:" ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( "Green:" );

    m_pChatGreenValue [ eType ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "0" ) );
    m_pChatGreenValue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY) );
    m_pChatGreenValue [ eType ]->AutoSize ( "255 " );

    m_pChatGreen [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatGreen [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatGreen [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
    m_pChatGreen [ eType ]->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnChatGreenChanged, this ) );
    m_pChatGreen [ eType ]->SetProperty ( "StepSize", "0.004" );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "Blue:" ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( "Blue:" );

    m_pChatBlueValue [ eType ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "0" ) );
    m_pChatBlueValue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY) );
    m_pChatBlueValue [ eType ]->AutoSize ( "255 " );

    m_pChatBlue [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatBlue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatBlue [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
    m_pChatBlue [ eType ]->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnChatBlueChanged, this ) );
    m_pChatBlue [ eType ]->SetProperty ( "StepSize", "0.004" );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "Transparency:" ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( "Transparency:" );

    m_pChatAlphaValue [ eType ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "0" ) );
    m_pChatAlphaValue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY) );
    m_pChatAlphaValue [ eType ]->AutoSize ( "255 " );

    m_pChatAlpha [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatAlpha [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatAlpha [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
    m_pChatAlpha [ eType ]->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnChatAlphaChanged, this ) );
    m_pChatAlpha [ eType ]->SetProperty ( "StepSize", "0.004" );
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

void CSettings::LoadSkins()
{
    std::vector<SString> directories = FindFiles(CalcMTASAPath ( SKINS_PATH ), false, true);

    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);

    for( std::vector<SString>::iterator it = directories.begin(); it != directories.end(); ++it )
    {
        CGUIListItem* item = m_pInterfaceSkinSelector->AddItem ( (*it) );
        // TODO: add validation of the skin

        if(currentSkin == (*it))
            m_pInterfaceSkinSelector->SetSelectedItemByIndex(m_pInterfaceSkinSelector->GetItemIndex(item));
    }   
}

void CSettings::LoadChatColorFromString ( ChatColorType eType, const string& strColor )
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
    if ( !pItem )
        return true;

    CXMLNode* pNode = reinterpret_cast < CXMLNode* > ( pItem->GetData () );
    if ( !pNode )
        return true;

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
                LoadChatColorFromString ( ChatColorTypes::CHAT_COLOR_TEXT, strValue );
            }
            else if ( strTag == "color_background" )
            {
                LoadChatColorFromString ( ChatColorTypes::CHAT_COLOR_BG, strValue );
            }
            else if ( strTag == "color_input_text" )
            {
                LoadChatColorFromString ( ChatColorTypes::CHAT_COLOR_INPUT_TEXT, strValue );
            }
            else if ( strTag == "color_input_background" )
            {
                LoadChatColorFromString ( ChatColorTypes::CHAT_COLOR_INPUT_BG, strValue );
            }
            else if ( strTag == "font" )
            {
                int iValue;
                pSubNode->GetTagContent ( iValue );
                
                if ( iValue < 0 || iValue >= ChatFonts::CHAT_FONT_MAX )
                    iValue = ChatFonts::CHAT_FONT_DEFAULT;
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

    return true;
}


bool CSettings::OnSkinChanged ( CGUIElement* pElement )
{
    CGUIListItem* pItem = m_pInterfaceSkinSelector->GetSelectedItem ();
    if ( !pItem )
        return true;

    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);
    std::string newSkin = pItem->GetText();

    if ( m_bIsModLoaded )
    {
        // Reset our item
        unsigned int uiIndex = 0;
        std::string strItemText = m_pInterfaceSkinSelector->GetItemText( uiIndex );
        while ( strItemText != currentSkin )
        {
            strItemText = m_pInterfaceSkinSelector->GetItemText( ++uiIndex );
        }
        m_pInterfaceSkinSelector->SetSelectedItemByIndex(uiIndex);
        g_pCore->ShowMessageBox ( "Error", "Please disconnect before changing skin", MB_BUTTON_OK | MB_ICON_INFO );
        m_pWindow->MoveToBack();
        return true;
    }

    return true;
}

bool CSettings::OnDrawDistanceChanged ( CGUIElement* pElement )
{
    int iDrawDistance = ( m_pDrawDistance->GetScrollPosition () ) * 100;

    m_pDrawDistanceValueLabel->SetText ( SString("%i%%", iDrawDistance).c_str() );
    return true;
}

bool CSettings::OnBrightnessChanged ( CGUIElement* pElement )
{
    int iBrightness = ( m_pBrightness->GetScrollPosition () ) * 100;

    m_pBrightnessValueLabel->SetText ( SString("%i%%", iBrightness).c_str() );
    return true;
}

bool CSettings::OnStreamingMemoryChanged ( CGUIElement* pElement )
{
    float fPos = m_pStreamingMemory->GetScrollPosition ();
    IDirect3DDevice9* pDevice = CCore::GetSingleton().GetGraphics()->GetDevice ();
    int min = SharedUtil::GetMinStreamingMemory ( pDevice );
    int max = SharedUtil::GetMaxStreamingMemory ( pDevice );
    int value = SharedUtil::Lerp ( min, fPos, max );
    m_pStreamingMemoryValueLabel->SetText ( SString("%i MB", value) );

    return true;
}

bool CSettings::OnMouseSensitivityChanged ( CGUIElement* pElement )
{
    int iMouseSensitivity = ( m_pMouseSensitivity->GetScrollPosition () ) * 100;

    m_pLabelMouseSensitivityValue->SetText ( SString("%i%%", iMouseSensitivity ).c_str () );
    return true;
}

bool CSettings::OnMapAlphaChanged ( CGUIElement* pElement )
{
    int iAlpha = ( m_pMapAlpha->GetScrollPosition () ) * 100;

    m_pMapAlphaValueLabel->SetText ( SString("%i%%", iAlpha).c_str() );
    return true;
}

bool CSettings::OnRadioVolumeChanged ( CGUIElement* pElement)
{
    unsigned char ucVolume = m_pAudioRadioVolume->GetScrollPosition () * 100.0f;
    m_pLabelRadioVolumeValue->SetText ( SString("%i%%", ucVolume).c_str() );

    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();
    gameSettings->SetRadioVolume ( m_pAudioRadioVolume->GetScrollPosition () * 64 );

    return true;
}

bool CSettings::OnSFXVolumeChanged ( CGUIElement* pElement)
{
    unsigned char ucVolume = m_pAudioSFXVolume->GetScrollPosition () * 100.0f;
    m_pLabelSFXVolumeValue->SetText ( SString ( "%i%%", ucVolume ).c_str () );

    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();
    gameSettings->SetSFXVolume ( m_pAudioSFXVolume->GetScrollPosition () * 64 );

    return true;
}

bool CSettings::OnMTAVolumeChanged ( CGUIElement* pElement)
{
    int iVolume = m_pAudioMTAVolume->GetScrollPosition () * 100.0f;
    m_pLabelMTAVolumeValue->SetText ( SString ( "%i%%", iVolume ).c_str () );
    
    CVARS_SET ( "mtavolume", m_pAudioMTAVolume->GetScrollPosition () );

    return true;
}

bool CSettings::OnChatRedChanged ( CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast < CGUIScrollBar* > ( pElement );
    int iValue = ( (float)pScrollBar->GetScrollPosition () * 255.0f );

    if ( pScrollBar == m_pChatRed [ ChatColorTypes::CHAT_COLOR_BG ] )
        m_pChatRedValue [ ChatColorTypes::CHAT_COLOR_BG ]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatRed [ ChatColorTypes::CHAT_COLOR_TEXT ] )
        m_pChatRedValue [ ChatColorTypes::CHAT_COLOR_TEXT]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatRed [ ChatColorTypes::CHAT_COLOR_INPUT_BG ] )
        m_pChatRedValue [ ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatRed [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT ] )
        m_pChatRedValue [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText ( SString("%i", iValue).c_str() );

    return true;
}

bool CSettings::OnChatGreenChanged ( CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast < CGUIScrollBar* > ( pElement );
    int iValue = ( (float)pScrollBar->GetScrollPosition () * 255.0f );

    if ( pScrollBar == m_pChatGreen [ ChatColorTypes::CHAT_COLOR_BG ] )
        m_pChatGreenValue [ ChatColorTypes::CHAT_COLOR_BG ]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatGreen [ ChatColorTypes::CHAT_COLOR_TEXT ] )
        m_pChatGreenValue [ ChatColorTypes::CHAT_COLOR_TEXT]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatGreen [ ChatColorTypes::CHAT_COLOR_INPUT_BG ] )
        m_pChatGreenValue [ ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatGreen [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT ] )
        m_pChatGreenValue [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText ( SString("%i", iValue).c_str() );

    return true;
}

bool CSettings::OnChatBlueChanged ( CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast < CGUIScrollBar* > ( pElement );
    int iValue = ( (float)pScrollBar->GetScrollPosition () * 255.0f );

    if ( pScrollBar == m_pChatBlue [ ChatColorTypes::CHAT_COLOR_BG ] )
        m_pChatBlueValue [ ChatColorTypes::CHAT_COLOR_BG ]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatBlue [ ChatColorTypes::CHAT_COLOR_TEXT ] )
        m_pChatBlueValue [ ChatColorTypes::CHAT_COLOR_TEXT]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatBlue [ ChatColorTypes::CHAT_COLOR_INPUT_BG ] )
        m_pChatBlueValue [ ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatBlue [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT ] )
        m_pChatBlueValue [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText ( SString("%i", iValue).c_str() );

    return true;
}

bool CSettings::OnChatAlphaChanged ( CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast < CGUIScrollBar* > ( pElement );
    int iValue = ( (float)pScrollBar->GetScrollPosition () * 255.0f );

    if ( pScrollBar == m_pChatAlpha [ ChatColorTypes::CHAT_COLOR_BG ] )
        m_pChatAlphaValue [ ChatColorTypes::CHAT_COLOR_BG ]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatAlpha [ ChatColorTypes::CHAT_COLOR_TEXT ] )
        m_pChatAlphaValue [ ChatColorTypes::CHAT_COLOR_TEXT]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatAlpha [ ChatColorTypes::CHAT_COLOR_INPUT_BG ] )
        m_pChatAlphaValue [ ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText ( SString("%i", iValue).c_str() );
    else if ( pScrollBar == m_pChatAlpha [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT ] )
        m_pChatAlphaValue [ ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText ( SString("%i", iValue).c_str() );

    return true;
}

bool CSettings::OnUpdateButtonClick ( CGUIElement* pElement )
{
    // Update build type
    if ( CGUIListItem* pSelected = m_pUpdateBuildTypeCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "update_build_type", iSelected );
    }

    GetVersionUpdater ()->InitiateManualCheck ();
    return true;
}

bool CSettings::OnFxQualityChanged ( CGUIElement* pElement )
{
    CGUIListItem* pItem = m_pComboFxQuality->GetSelectedItem ();
    if ( !pItem )
        return true;

    if ( ( int ) pItem->GetData() == 0 )
    {
        m_pCheckBoxVolumetricShadows->SetSelected ( false );
        m_pCheckBoxVolumetricShadows->SetEnabled ( false );
    }
    else
        m_pCheckBoxVolumetricShadows->SetEnabled ( true );
        
    return true;
}

void VolumetricShadowsCallBack ( void* ptr, unsigned int uiButton )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();
    if ( uiButton == 0 )
        ((CGUICheckBox*)ptr)->SetSelected ( false );
}

bool CSettings::OnVolumetricShadowsClick ( CGUIElement* pElement )
{
    if ( m_pCheckBoxVolumetricShadows->GetSelected () && !m_bShownVolumetricShadowsWarning )
    {
        m_bShownVolumetricShadowsWarning = true;
        SStringX strMessage ( "Volmetric shadows can cause some systems to slow down." );
        strMessage += "\n\nAre you sure you want to enable them?";
        CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
        pQuestionBox->Reset ();
        pQuestionBox->SetTitle ( "PERFORMANCE WARNING" );
        pQuestionBox->SetMessage ( strMessage );
        pQuestionBox->SetButton ( 0, "No" );
        pQuestionBox->SetButton ( 1, "Yes" );
        pQuestionBox->SetCallback ( VolumetricShadowsCallBack, m_pCheckBoxVolumetricShadows );
        pQuestionBox->Show ();
    }
    return true;
}

void NewNicknameCallback ( void* ptr, unsigned int uiButton, std::string strNick )
{
    if ( uiButton == 1 )  // We hit OK
    {
        if ( !CCore::GetSingleton ().IsValidNick ( strNick.c_str () ) )
            CCore::GetSingleton ().ShowMessageBox ( "Error", "Your nickname contains invalid characters!", MB_BUTTON_OK | MB_ICON_INFO );
        else
        {
            CVARS_SET ( "nick", strNick ) ;
            CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();
        }
    }
    else
        CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();
}

void CSettings::RequestNewNickname ( void )
{
        std::string strNick;
        CVARS_GET ( "nick", strNick );

        CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
        pQuestionBox->Reset ();
        pQuestionBox->SetTitle ( "Please enter a nickname" );
        pQuestionBox->SetMessage ( "Please enter a nickname to be used ingame.  \nThis will be your name when you connect to and play in a server"  );
        pQuestionBox->SetButton ( 0, "Cancel" );
        pQuestionBox->SetButton ( 1, "OK" );
        pQuestionBox->SetEditbox ( 0, strNick )->SetMaxLength ( MAX_PLAYER_NICK_LENGTH );
        pQuestionBox->SetCallbackEdit ( NewNicknameCallback );
        pQuestionBox->Show ();
}

