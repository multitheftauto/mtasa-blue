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

using namespace std;

#define CORE_SETTINGS_UPDATE_INTERVAL   30         // Settings update interval in frames
#define CORE_SETTINGS_HEADERS           3
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
    m_iMaxAnisotropic = g_pDeviceState->AdapterState.MaxAnisotropicSetting;
    m_pWindow = NULL;
    CreateGUI ();
}


CSettings::~CSettings ( void )
{
    DestroyGUI ();
}


void CSettings::CreateGUI ( void )
{
    if ( m_pWindow )
        DestroyGUI ();

    CGUITab *pTabMultiplayer, *pTabVideo, *pTabAudio, *pTabBinds, *pTabControls, *pTabCommunity, *pTabInterface, *pTabAdvanced;
    CGUI *pManager = g_pCore->GetGUI ();

    // Init
    m_bIsModLoaded = false;
    m_bCaptureKey = false;
    m_dwFrameCount = 0;
    m_bShownVolumetricShadowsWarning = false;
    m_bShownAllowScreenUploadMessage = false;
    CVector2D vecTemp;
    CVector2D vecSize;

    // Create the window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, _("SETTINGS") ) );
    m_pWindow->SetCloseButtonEnabled ( true );
    m_pWindow->SetMovable ( true );

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    float yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 560.0f / 2, resolution.fY / 2 - 480.0f / 2 + yoff  ), false );

    //m_pWindow->SetPosition ( CVector2D ( 0.15f, 0.20f ), true );
    m_pWindow->SetSize ( CVector2D ( 560.0f, 480.0f ) );
    m_pWindow->SetSizingEnabled ( false );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->BringToFront ();

    // Create the tab panel and necessary tabs
    m_pTabs = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( m_pWindow ) );
    m_pTabs->SetPosition ( CVector2D ( 0, 20.0f ) );
    m_pTabs->SetSize ( CVector2D ( 560.0f, 420.0f ) );
    pTabMultiplayer = m_pTabs->CreateTab ( _("Multiplayer") );
    pTabVideo = m_pTabs->CreateTab ( _("Video") );
    pTabAudio = m_pTabs->CreateTab ( _("Audio") );
    pTabBinds = m_pTabs->CreateTab ( _("Binds") );
    pTabControls = m_pTabs->CreateTab ( _("Controls") );
    pTabCommunity = m_pTabs->CreateTab ( _("Community") );
    pTabInterface = m_pTabs->CreateTab ( _("Interface") );
    pTabAdvanced = m_pTabs->CreateTab ( _("Advanced") );

    // Create buttons
    //  OK button
    m_pButtonOK = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, _("OK") ) );
    m_pButtonOK->SetPosition ( CVector2D ( 0.5f, 0.94f ), true );
    m_pButtonOK->SetZOrderingEnabled ( false );

    //  Cancel button
    m_pButtonCancel = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, _("Cancel") ) );
    m_pButtonCancel->SetPosition ( CVector2D ( 0.78f, 0.94f ), true );
    m_pButtonCancel->SetZOrderingEnabled ( false );

    /**
     *  Binds tab
     **/
    m_pBindsList = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( pTabBinds, false ) );
    m_pBindsList->SetPosition ( CVector2D ( 10, 15 ) );
    m_pBindsList->SetSize ( CVector2D ( 520.0f, 335.0f ) );
    m_pBindsList->SetSorting ( false );
    m_pBindsList->SetSelectionMode ( SelectionModes::CellSingle );
    m_pBindsList->SetDoubleClickHandler ( GUI_CALLBACK( &CSettings::OnBindsListClick, this ) );

    m_pTabs->GetSize ( vecTemp );
    m_pBindsDefButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabBinds, _("Load defaults") ) );
    m_pBindsDefButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnBindsDefaultClick, this ) );
    m_pBindsDefButton->AutoSize ( NULL, 20.0f, 8.0f );
    m_pBindsDefButton->GetSize ( vecSize );
    m_pBindsDefButton->SetPosition ( CVector2D ( vecTemp.fX - vecSize.fX - 12.0f, 365 ) );
    m_pBindsDefButton->SetZOrderingEnabled ( false );

    /**
     *  Controls tab
     **/
    float fIndentX = pManager->CGUI_GetMaxTextExtent( "default-normal",
        _("Mouse sensitivity:"),
        _("Vertical aim sensitivity:")
    );
    vecTemp = CVector2D( 0, 13 );
    //Mouse Options
    m_pControlsMouseLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Mouse options") ) );
    m_pControlsMouseLabel->SetPosition ( CVector2D ( vecTemp.fX + 11, vecTemp.fY ) );
    m_pControlsMouseLabel->AutoSize ( NULL, 5.0f );
    m_pControlsMouseLabel->SetFont ( "default-bold-small" );
    vecTemp.fY += 18;

    vecTemp.fX = 16;
    m_pInvertMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabControls, _("Invert mouse vertically"), true ) );
    m_pInvertMouse->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pInvertMouse->AutoSize ( NULL, 20.0f );

    m_pSteerWithMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabControls, _("Steer with mouse"), true ) );
    m_pSteerWithMouse->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16.0f ) );
    m_pSteerWithMouse->AutoSize ( NULL, 20.0f );

    m_pFlyWithMouse = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabControls, _("Fly with mouse"), true ) );
    m_pFlyWithMouse->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pFlyWithMouse->AutoSize ( NULL, 20.0f );

    // MouseSensitivity
    vecTemp.fY += 52.0f;
    m_pLabelMouseSensitivity = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Mouse sensitivity:") ) );
    m_pLabelMouseSensitivity->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pLabelMouseSensitivity->AutoSize ( );

    m_pMouseSensitivity = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabControls ) );
    m_pMouseSensitivity->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pMouseSensitivity->GetPosition ( vecTemp );
    m_pMouseSensitivity->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pMouseSensitivity->GetSize ( vecSize );
    m_pMouseSensitivity->SetProperty ( "StepSize", "0.01" );

    m_pLabelMouseSensitivityValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, "0%") );
    m_pLabelMouseSensitivityValue->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pLabelMouseSensitivityValue->AutoSize ( "100%" );
    vecTemp.fX = 16;
    vecTemp.fY += 26.f;

    // VerticalAimSensitivity
    m_pLabelVerticalAimSensitivity = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Vertical aim sensitivity:") ) );
    m_pLabelVerticalAimSensitivity->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pLabelVerticalAimSensitivity->AutoSize ( m_pLabelVerticalAimSensitivity->GetText().c_str() );

    m_pVerticalAimSensitivity = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabControls ) );
    m_pVerticalAimSensitivity->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pVerticalAimSensitivity->GetPosition ( vecTemp );
    m_pVerticalAimSensitivity->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pVerticalAimSensitivity->GetSize ( vecSize );
    m_pVerticalAimSensitivity->SetProperty ( "StepSize", "0.01" );

    m_pLabelVerticalAimSensitivityValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, "0%") );
    m_pLabelVerticalAimSensitivityValue->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pLabelVerticalAimSensitivityValue->AutoSize ( "100%" );
    vecTemp.fY += 30.f;

    vecTemp.fX = 16;
    //Joypad options
    m_pControlsJoypadLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Joypad options") ) );
    m_pControlsJoypadLabel->SetPosition ( CVector2D ( 11, vecTemp.fY ) );
    m_pControlsJoypadLabel->AutoSize ( NULL, 5.0f );
    m_pControlsJoypadLabel->SetFont ( "default-bold-small" );
    vecTemp.fY += 18;

    //Create a mini-scrollpane for the radio buttons (only way to group them together)
    m_pControlsInputTypePane = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( pTabControls ) ); 
    m_pControlsInputTypePane->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pControlsInputTypePane->SetPosition ( CVector2D ( 0, vecTemp.fY ) );
    m_pControlsInputTypePane->SetSize ( CVector2D ( 1.0f, 0.27f ), true );
    m_pControlsInputTypePane->SetZOrderingEnabled ( false );

    m_pStandardControls = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pControlsInputTypePane, _("Standard controls (Mouse + Keyboard)") ) ); 
    m_pStandardControls->SetSelected ( true );
    m_pStandardControls->SetPosition ( CVector2D ( 11, 0 ) );
    m_pStandardControls->GetPosition ( vecTemp );
    m_pStandardControls->AutoSize ( NULL, 20.0f );
    m_pStandardControls->GetSize ( vecSize );

    m_pClassicControls = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pControlsInputTypePane, _("Classic controls (Joypad)") ) ); 
    m_pClassicControls->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 15, vecTemp.fY ) );
    m_pClassicControls->AutoSize ( NULL, 20.0f );

    m_pControlsInputTypePane->GetPosition ( vecTemp );
    vecTemp.fY += 24;

    //Advanced Joypad settings
    {
        m_JoypadSettingsRevision = -1;

        CJoystickManagerInterface* JoyMan = GetJoystickManager ();

        m_pJoypadName = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls ) );
        m_pJoypadName->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
        m_pJoypadName->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );
        m_pJoypadName->SetPosition ( CVector2D ( 270, vecTemp.fY ) );

        m_pJoypadUnderline = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls ) );
        m_pJoypadUnderline->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER );
        m_pJoypadUnderline->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );
        m_pJoypadUnderline->SetPosition ( CVector2D ( 270, vecTemp.fY + 2 ) );
        vecTemp.fY += 50;

        m_pEditDeadzone = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabControls ) );
        m_pEditDeadzone->SetPosition ( CVector2D ( 10, vecTemp.fY ) );
        m_pEditDeadzone->SetSize ( CVector2D ( 45.0f, 24.0f ) );
        m_pEditDeadzone->SetMaxLength ( 3 );
        m_pEditDeadzone->SetTextChangedHandler ( GUI_CALLBACK ( &CSettings::OnJoypadTextChanged, this ) );
        vecTemp.fY += 31;

        m_pEditSaturation = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabControls ) );
        m_pEditSaturation->SetPosition ( CVector2D ( 10, vecTemp.fY ) );
        m_pEditSaturation->SetSize ( CVector2D ( 45.0f, 24.0f ) );
        m_pEditSaturation->SetMaxLength ( 3 );
        m_pEditSaturation->SetTextChangedHandler ( GUI_CALLBACK ( &CSettings::OnJoypadTextChanged, this ) );

        CGUILabel* pLabelDeadZone = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Dead Zone") ) );
        pLabelDeadZone->SetPosition ( m_pEditDeadzone->GetPosition () + CVector2D ( 52.f, -1.f ) );
        pLabelDeadZone->AutoSize ();
        pLabelDeadZone->SetVerticalAlign( CGUI_ALIGN_VERTICALCENTER );

        CGUILabel* pLabelSaturation = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Saturation") ) );
        pLabelSaturation->SetPosition ( m_pEditSaturation->GetPosition () + CVector2D ( 52.f, -1.f ) );
        pLabelSaturation->AutoSize ();
        pLabelSaturation->SetVerticalAlign( CGUI_ALIGN_VERTICALCENTER );
        vecTemp.fY += 106;

        CGUILabel* pLabelHelp = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Use the 'Binds' tab for joypad buttons.") ) );
        pLabelHelp->SetPosition ( CVector2D ( 10, vecTemp.fY ) );
        pLabelHelp->AutoSize ();
        vecTemp.fY += -91;

        // Layout the mapping buttons like a dual axis joypad
        CVector2D vecPosList[] = {  CVector2D ( 162,  202 ),     // Left Stick
                                    CVector2D ( 280,  202 ),
                                    CVector2D ( 221,  182 ),
                                    CVector2D ( 221,  220 ),

                                    CVector2D ( 351,  202 ),     // Right Stick
                                    CVector2D ( 469,  202 ),
                                    CVector2D ( 410,  182 ),
                                    CVector2D ( 410,  220 ),

                                    CVector2D ( 410,  276 ),     // Acceleration/Brake
                                    CVector2D ( 221,  276 )     };

        for ( int i = 0 ; i < JoyMan->GetOutputCount () && i < 10 ; i++ )
        {
            CVector2D vecPos = vecPosList[i];
            vecPos.fY += vecTemp.fY - 231;
            CGUIButton* pButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabControls ) );
            pButton->SetPosition ( vecPos + CVector2D ( 10, 0 ) );
            pButton->SetSize ( CVector2D ( 48.0f, 24.0f ) );
            pButton->GetSize ( vecSize );
            pButton->SetUserData ( (void*) i );
            pButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnAxisSelectClick, this ) );
            pButton->SetZOrderingEnabled ( false );

            CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls ) );
            pLabel->SetSize ( CVector2D ( 160.0f, 24.0f ) );
            pLabel->SetPosition ( CVector2D ( (vecPos.fX + 10) + vecSize.fX*0.5f - 80.0f, vecPos.fY - 26 ) );
            pLabel->SetHorizontalAlign( CGUI_ALIGN_HORIZONTALCENTER );
            pLabel->SetVerticalAlign( CGUI_ALIGN_VERTICALCENTER );
            pLabel->SetVisible ( i >= 8 );      // Hide all labels except 'Acceleration' and 'Brake'

            m_pJoypadLabels.push_back ( pLabel );
            m_pJoypadButtons.push_back ( pButton );
        }

        vecTemp.fY += -74;
        CGUILabel* pLabelLeft = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Left Stick") ) );
        pLabelLeft->AutoSize ( );
        pLabelLeft->GetSize ( vecSize );
        pLabelLeft->SetPosition ( CVector2D ( 255.0f - vecSize.fX*0.5f, vecTemp.fY ) );
        pLabelLeft->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

        CGUILabel* pLabelRight = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabControls, _("Right Stick") ) );
        pLabelRight->AutoSize ( );
        pLabelRight->GetSize ( vecSize );
        pLabelRight->SetPosition ( CVector2D ( 444.0f - vecSize.fX*0.5f, vecTemp.fY ) );
        pLabelRight->SetVerticalAlign ( CGUI_ALIGN_VERTICALCENTER );

    }

    m_pTabs->GetSize ( vecTemp );
    CGUIButton*  pControlsDefButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabControls, _("Load defaults") ) );
    pControlsDefButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnControlsDefaultClick, this ) );
    pControlsDefButton->AutoSize ( NULL, 20.0f, 8.0f );
    pControlsDefButton->GetSize ( vecSize );
    pControlsDefButton->SetPosition ( CVector2D ( vecTemp.fX - vecSize.fX - 12.0f, 365 ) );
    pControlsDefButton->SetZOrderingEnabled ( false );

    m_hBind = m_pBindsList->AddColumn ( _("DESCRIPTION"), 0.35f );
    m_hPriKey = m_pBindsList->AddColumn ( _("KEY"), 0.24f );
    for ( int k = 0 ; k < SecKeyNum ; k++ )
        m_hSecKeys[k] = m_pBindsList->AddColumn ( _("ALT. KEY"), 0.24f );

    /**
     *  Community tab
     **/

    m_pTabs->DeleteTab ( pTabCommunity );

    m_pLabelCommunity = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabCommunity, CORE_SETTINGS_COMMUNITY_TEXT ) );
    m_pLabelCommunity->SetPosition ( CVector2D ( 0.022f, 0.043f ), true );
    m_pLabelCommunity->SetSize ( CVector2D ( 9.956f, 4.414f ), true );
    //m_pLabelCommunity->AutoSize ( CORE_SETTINGS_COMMUNITY_TEXT );

    m_pLabelUser = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabCommunity, _("Username:") ) );
    m_pLabelUser->SetPosition ( CVector2D ( 0.022f, 0.46f ), true );
    m_pLabelUser->GetPosition ( vecTemp, false );
    m_pLabelUser->AutoSize ( );

    m_pLabelPass = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabCommunity, _("Password:") ) );
    m_pLabelPass->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
    m_pLabelPass->AutoSize ( );

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

    m_pButtonLogin = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabCommunity, _("Login") ) );
    m_pButtonLogin->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
    m_pButtonLogin->GetPosition ( vecTemp, false );
    m_pButtonLogin->SetSize ( CVector2D ( 168.0f, 24.0f ) );
    m_pButtonLogin->SetZOrderingEnabled ( false );

    m_pButtonRegister = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabCommunity, _("Register") ) );
    m_pButtonRegister->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32 ) );
    m_pButtonRegister->SetSize ( CVector2D ( 168.0f, 24.0f ) );
    m_pButtonRegister->SetZOrderingEnabled ( false );

    /**
     *	Multiplayer tab
     **/
    m_pLabelNick = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabMultiplayer, _("Nick:") ) );
    m_pLabelNick->SetPosition ( CVector2D ( 11, 13 ) );
    m_pLabelNick->GetPosition ( vecTemp, false );
    m_pLabelNick->AutoSize ( _("Nick:") );
    m_pLabelNick->GetSize ( vecSize );

    // Nick edit
    m_pEditNick = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabMultiplayer ) );
    m_pEditNick->SetPosition ( CVector2D ( vecSize.fX + vecTemp.fX + 50.0f, vecTemp.fY - 1.0f ) );
    m_pEditNick->SetSize ( CVector2D ( 178.0f, 24.0f ) );
    m_pEditNick->SetMaxLength ( MAX_PLAYER_NICK_LENGTH );
    m_pEditNick->SetTextAcceptedHandler( GUI_CALLBACK( &CSettings::OnOKButtonClick, this ) );

    m_pSavePasswords = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabMultiplayer, _("Save server passwords"), true ) );
    m_pSavePasswords->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 50.0f ) );
    m_pSavePasswords->GetPosition ( vecTemp, false );
    m_pSavePasswords->AutoSize ( NULL, 20.0f );

    m_pAutoRefreshBrowser = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabMultiplayer, _("Auto-refresh server browser"), true ) );
    m_pAutoRefreshBrowser->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 20.0f ) );
    m_pAutoRefreshBrowser->GetPosition ( vecTemp, false );
    m_pAutoRefreshBrowser->AutoSize ( NULL, 20.0f );

    m_pCheckBoxAllowScreenUpload = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabMultiplayer, _("Allow screen upload"), true ) );
    m_pCheckBoxAllowScreenUpload->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 20.0f ) );
    m_pCheckBoxAllowScreenUpload->GetPosition ( vecTemp, false );
    m_pCheckBoxAllowScreenUpload->AutoSize ( NULL, 20.0f );

    m_pCheckBoxCustomizedSAFiles = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabMultiplayer, _("Use customized GTA:SA files"), true ) );
    m_pCheckBoxCustomizedSAFiles->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 20.0f ) );
    m_pCheckBoxCustomizedSAFiles->GetPosition ( vecTemp, false );
    m_pCheckBoxCustomizedSAFiles->AutoSize ( NULL, 20.0f );

    m_pMapRenderingLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabMultiplayer, _("Map rendering options") ) );
    m_pMapRenderingLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pMapRenderingLabel->GetPosition ( vecTemp, false );
    m_pMapRenderingLabel->SetFont ( "default-bold-small" );
    m_pMapRenderingLabel->AutoSize ( );

    m_pMapAlphaLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabMultiplayer, _("Opacity:") ) );
    m_pMapAlphaLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 24.0f ) );
    m_pMapAlphaLabel->GetPosition ( vecTemp, false );
    m_pMapAlphaLabel->AutoSize ( );

    m_pMapAlpha = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabMultiplayer ) );
    m_pMapAlpha->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pMapAlpha->GetPosition ( vecTemp, false );
    m_pMapAlpha->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pMapAlpha->GetSize ( vecSize );
    m_pMapAlpha->SetProperty ( "StepSize", "0.01" );

    m_pMapAlphaValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabMultiplayer, "0%") );
    m_pMapAlphaValueLabel->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pMapAlphaValueLabel->GetPosition ( vecTemp, false );
    m_pMapAlphaValueLabel->AutoSize ( "100% " );


    /**
     *  Audio tab
     **/
    fIndentX = pManager->CGUI_GetMaxTextExtent( "default-normal",
        _("Radio volume:"),
        _("SFX volume:"),
        _("MTA volume:"),
        _("Voice volume:"),
        _("Play mode:")
    );

    m_pAudioGeneralLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, _("General") ) );
    m_pAudioGeneralLabel->SetPosition ( CVector2D ( 11, 13 ) );
    m_pAudioGeneralLabel->GetPosition ( vecTemp, false );
    m_pAudioGeneralLabel->AutoSize ( NULL, 5.0f );
    m_pAudioGeneralLabel->SetFont ( "default-bold-small" );

    m_pLabelRadioVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, _("Radio volume:") ) );
    m_pLabelRadioVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ), false );
    m_pLabelRadioVolume->GetPosition ( vecTemp, false );
    m_pLabelRadioVolume->AutoSize ( );

    m_pAudioRadioVolume = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAudio ) );
    m_pAudioRadioVolume->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pAudioRadioVolume->GetPosition ( vecTemp, false );
    m_pAudioRadioVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAudioRadioVolume->GetSize ( vecSize, false );
    m_pAudioRadioVolume->SetProperty ( "StepSize", "0.01" );

    m_pLabelRadioVolumeValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "0%") );
    m_pLabelRadioVolumeValue->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pLabelRadioVolumeValue->GetPosition ( vecTemp, false );
    m_pLabelRadioVolumeValue->AutoSize ( "100%" );
    m_pLabelRadioVolumeValue->GetSize ( vecSize, false );

    vecTemp.fX = 11;
    m_pLabelSFXVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, _("SFX volume:") ) );
    m_pLabelSFXVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pLabelSFXVolume->GetPosition ( vecTemp, false );
    m_pLabelSFXVolume->AutoSize ( );

    m_pAudioSFXVolume = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAudio ) );
    m_pAudioSFXVolume->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pAudioSFXVolume->GetPosition ( vecTemp, false );
    m_pAudioSFXVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAudioSFXVolume->GetSize ( vecSize, false );
    m_pAudioSFXVolume->SetProperty ( "StepSize", "0.01" );

    m_pLabelSFXVolumeValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "0%") );
    m_pLabelSFXVolumeValue->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pLabelSFXVolumeValue->GetPosition ( vecTemp, false );
    m_pLabelSFXVolumeValue->AutoSize ( "100%" );
    m_pLabelSFXVolumeValue->GetSize ( vecSize, false );

    vecTemp.fX = 11;
    m_pLabelMTAVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, _("MTA volume:") ) );
    m_pLabelMTAVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pLabelMTAVolume->GetPosition ( vecTemp, false );
    m_pLabelMTAVolume->AutoSize ( );

    m_pAudioMTAVolume = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAudio ) );
    m_pAudioMTAVolume->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pAudioMTAVolume->GetPosition ( vecTemp, false );
    m_pAudioMTAVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAudioMTAVolume->GetSize ( vecSize, false );
    m_pAudioMTAVolume->SetProperty ( "StepSize", "0.01" );

    m_pLabelMTAVolumeValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "0%") );
    m_pLabelMTAVolumeValue->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pLabelMTAVolumeValue->GetPosition ( vecTemp, false );
    m_pLabelMTAVolumeValue->AutoSize ( "100%" );
    m_pLabelMTAVolumeValue->GetSize ( vecSize, false );

    vecTemp.fX = 11;
    m_pLabelVoiceVolume = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, _("Voice volume:") ) );
    m_pLabelVoiceVolume->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pLabelVoiceVolume->GetPosition ( vecTemp, false );
    m_pLabelVoiceVolume->AutoSize ( );

    m_pAudioVoiceVolume = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAudio ) );
    m_pAudioVoiceVolume->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pAudioVoiceVolume->GetPosition ( vecTemp, false );
    m_pAudioVoiceVolume->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAudioVoiceVolume->GetSize ( vecSize, false );
    m_pAudioVoiceVolume->SetProperty ( "StepSize", "0.01" );

    m_pLabelVoiceVolumeValue = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, "0%") );
    m_pLabelVoiceVolumeValue->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pLabelVoiceVolumeValue->GetPosition ( vecTemp, false );
    m_pLabelVoiceVolumeValue->AutoSize ( "100%" );
    m_pLabelVoiceVolumeValue->GetSize ( vecSize, false );


    vecTemp.fX = 11;
    
    m_pCheckBoxMuteRadio = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, _("Mute Radio sounds when minimized"), true ) );
    m_pCheckBoxMuteRadio->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pCheckBoxMuteRadio->GetPosition ( vecTemp, false );
    m_pCheckBoxMuteRadio->AutoSize ( NULL, 20.0f );

    m_pCheckBoxMuteSFX = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, _("Mute SFX sounds when minimized"), true ) );
    m_pCheckBoxMuteSFX->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16.0f ) );
    m_pCheckBoxMuteSFX->GetPosition ( vecTemp, false );
    m_pCheckBoxMuteSFX->AutoSize ( NULL, 20.0f );

    m_pCheckBoxMuteMTA = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, _("Mute MTA sounds when minimized"), true ) );
    m_pCheckBoxMuteMTA->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16.0f ) );
    m_pCheckBoxMuteMTA->GetPosition ( vecTemp, false );
    m_pCheckBoxMuteMTA->AutoSize ( NULL, 20.0f );

    m_pCheckBoxMuteVoice = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, _("Mute Voice sounds when minimized"), true ) );
    m_pCheckBoxMuteVoice->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 16.0f ) );
    m_pCheckBoxMuteVoice->GetPosition ( vecTemp, false );
    m_pCheckBoxMuteVoice->AutoSize ( NULL, 20.0f );

    vecTemp.fX = 11;

    m_pCheckBoxAudioEqualizer = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, _("Radio Equalizer"), true ) );
    m_pCheckBoxAudioEqualizer->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pCheckBoxAudioEqualizer->AutoSize ( NULL, 20.0f );

    m_pCheckBoxAudioAutotune = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, _("Radio Auto-tune"), true ) );
    m_pCheckBoxAudioAutotune->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 50.0f ) );
    m_pCheckBoxAudioAutotune->AutoSize ( NULL, 20.0f );
    m_pCheckBoxAudioAutotune->GetPosition ( vecTemp );

    m_pAudioUsertrackLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, _("Usertrack options") ) );
    m_pAudioUsertrackLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ), false );
    m_pAudioUsertrackLabel->GetPosition ( vecTemp, false );
    m_pAudioUsertrackLabel->AutoSize ( NULL, 10.0f );
    m_pAudioUsertrackLabel->SetFont ( "default-bold-small" );

    m_pLabelUserTrackMode = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAudio, _("Play mode:") ) );
    m_pLabelUserTrackMode->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pLabelUserTrackMode->GetPosition ( vecTemp, false );
    m_pLabelUserTrackMode->AutoSize ( );

    m_pComboUsertrackMode = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAudio, "" ) );
    m_pComboUsertrackMode->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pComboUsertrackMode->SetSize ( CVector2D ( 160.0f, 80.0f ) );
    m_pComboUsertrackMode->AddItem ( _("Radio") )->SetData ( (void*)0 );
    m_pComboUsertrackMode->AddItem ( _("Random") )->SetData ( (void*)1 );
    m_pComboUsertrackMode->AddItem ( _("Sequential") )->SetData ( (void*)2 );
    m_pComboUsertrackMode->SetReadOnly ( true );

    m_pCheckBoxUserAutoscan = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAudio, _("Automatic Media Scan"), true ) );
    m_pCheckBoxUserAutoscan->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 32.0f ) );
    m_pCheckBoxUserAutoscan->AutoSize ( NULL, 20.0f );
    m_pCheckBoxUserAutoscan->GetPosition ( vecTemp, false );

    m_pTabs->GetSize ( vecTemp );
    m_pAudioDefButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabAudio, _("Load defaults") ) );
    m_pAudioDefButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnAudioDefaultClick, this ) );
    m_pAudioDefButton->AutoSize ( NULL, 20.0f, 8.0f );
    m_pAudioDefButton->GetSize ( vecSize );
    m_pAudioDefButton->SetPosition ( CVector2D ( vecTemp.fX - vecSize.fX - 12.0f, 365 ) );
    m_pAudioDefButton->SetZOrderingEnabled ( false );

    /**
     *  Video tab
     **/
    fIndentX = pManager->CGUI_GetMaxTextExtent( "default-normal",
        _("Resolution:"),
        _("Draw Distance:"),
        _("Brightness:"),
        _("FX Quality:"),
        _("Anisotropic filtering:"),
        _("Anti-aliasing:"),
        _("Aspect Ratio:"),
        _("Opacity:")
    );

    m_pVideoGeneralLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("General") ) );
    m_pVideoGeneralLabel->SetPosition ( CVector2D ( 11, 13 ) );
    m_pVideoGeneralLabel->GetPosition ( vecTemp, false );
    m_pVideoGeneralLabel->AutoSize ( NULL, 3.0f );
    m_pVideoGeneralLabel->SetFont ( "default-bold-small" );

    m_pVideoResolutionLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("Resolution:") ) );
    m_pVideoResolutionLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
    m_pVideoResolutionLabel->GetPosition ( vecTemp, false );
    m_pVideoResolutionLabel->AutoSize ( );

    m_pComboResolution = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboResolution->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 5.0f ) );
    m_pComboResolution->GetPosition ( vecTemp, false );
    m_pComboResolution->SetSize ( CVector2D ( 200.0f, 160.0f ) );
    m_pComboResolution->GetSize ( vecSize );
    m_pComboResolution->SetReadOnly ( true );

    m_pCheckBoxWindowed = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Windowed"), true ) );
    m_pCheckBoxWindowed->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY + 3.0f ) );
    m_pCheckBoxWindowed->AutoSize ( NULL, 20.0f );

    m_pVideoResolutionLabel->GetPosition ( vecTemp, false ); // Restore our label position

    m_pCheckBoxMipMapping = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Mip Mapping"), true ) );
#ifndef MIP_MAPPING_SETTING_APPEARS_TO_DO_SOMETHING
    m_pCheckBoxMipMapping->SetPosition ( CVector2D ( vecTemp.fX + 340.0f, vecTemp.fY + 45.0f ) );
    m_pCheckBoxMipMapping->SetSize ( CVector2D ( 224.0f, 16.0f ) );
    m_pCheckBoxMipMapping->SetVisible ( false );
#endif

    vecTemp.fY -= 5;
    m_pDrawDistanceLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("Draw Distance:") ) );
    m_pDrawDistanceLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pDrawDistanceLabel->GetPosition ( vecTemp, false );
    m_pDrawDistanceLabel->AutoSize ( );

    m_pDrawDistance = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabVideo ) );
    m_pDrawDistance->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pDrawDistance->GetPosition ( vecTemp, false );
    m_pDrawDistance->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pDrawDistance->GetSize ( vecSize );
    m_pDrawDistance->SetProperty ( "StepSize", "0.01" );

    m_pDrawDistanceValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "0%") );
    m_pDrawDistanceValueLabel->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pDrawDistanceValueLabel->AutoSize ( "100% " );

    vecTemp.fX = 11;

    m_pBrightnessLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("Brightness:") ) );
    m_pBrightnessLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pBrightnessLabel->GetPosition ( vecTemp, false );
    m_pBrightnessLabel->AutoSize ( );

    m_pBrightness = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabVideo ) );
    m_pBrightness->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pBrightness->GetPosition ( vecTemp, false );
    m_pBrightness->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pBrightness->GetSize ( vecSize );
    m_pBrightness->SetProperty ( "StepSize", "0.01" );

    m_pBrightnessValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, "0%") );
    m_pBrightnessValueLabel->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pBrightnessValueLabel->AutoSize ( "100% " );

    vecTemp.fX = 11;

    m_pFXQualityLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("FX Quality:") ) );
    m_pFXQualityLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pFXQualityLabel->GetPosition ( vecTemp, false );
    m_pFXQualityLabel->AutoSize ( );

    m_pComboFxQuality = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboFxQuality->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f ) );
    m_pComboFxQuality->SetSize ( CVector2D ( 200.0f, 95.0f ) );
    m_pComboFxQuality->AddItem ( _("Low") )->SetData ( (void*)0 );
    m_pComboFxQuality->AddItem ( _("Medium") )->SetData ( (void*)1 );
    m_pComboFxQuality->AddItem ( _("High") )->SetData ( (void*)2 );
    m_pComboFxQuality->AddItem ( _("Very high") )->SetData ( (void*)3 );
    m_pComboFxQuality->SetReadOnly ( true );

    m_pAnisotropicLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("Anisotropic filtering:") ) );
    m_pAnisotropicLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 29.0f ) );
    m_pAnisotropicLabel->GetPosition ( vecTemp, false );
    m_pAnisotropicLabel->AutoSize ( );

    m_pAnisotropic = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabVideo ) );
    m_pAnisotropic->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY ) );
    m_pAnisotropic->GetPosition ( vecTemp, false );
    m_pAnisotropic->SetSize ( CVector2D ( 160.0f, 20.0f ) );
    m_pAnisotropic->GetSize ( vecSize );
    m_pAnisotropic->SetProperty ( "StepSize", SString ( "%1.2f", 1 / (float)m_iMaxAnisotropic ) );

    m_pAnisotropicValueLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("Off")) );
    m_pAnisotropicValueLabel->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pAnisotropicValueLabel->SetSize ( CVector2D ( 100.0f, 20.0f ) );

    if ( m_iMaxAnisotropic < 1 )
    {
        // Hide if system can't do anisotropic filtering
        m_pFXQualityLabel->GetPosition ( vecTemp, false );
        m_pAnisotropicLabel->SetVisible ( false );
        m_pAnisotropic->SetVisible ( false );
        m_pAnisotropicValueLabel->SetVisible ( false );
    }

    vecTemp.fX = 11;

    m_pAntiAliasingLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("Anti-aliasing:") ) );
    m_pAntiAliasingLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pAntiAliasingLabel->GetPosition ( vecTemp, false );
    m_pAntiAliasingLabel->AutoSize ( );

    m_pComboAntiAliasing = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboAntiAliasing->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f ) );
    m_pComboAntiAliasing->SetSize ( CVector2D ( 200.0f, 95.0f ) );
    m_pComboAntiAliasing->AddItem ( _("Off") )->SetData ( (void*)1 );
    m_pComboAntiAliasing->AddItem ( _("1x") )->SetData ( (void*)2 );
    m_pComboAntiAliasing->AddItem ( _("2x") )->SetData ( (void*)3 );
    m_pComboAntiAliasing->AddItem ( _("3x") )->SetData ( (void*)4 );
    m_pComboAntiAliasing->SetReadOnly ( true );


    m_pAspectRatioLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabVideo, _("Aspect Ratio:") ) );
    m_pAspectRatioLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pAspectRatioLabel->GetPosition ( vecTemp, false );
    m_pAspectRatioLabel->AutoSize ( );

    m_pComboAspectRatio = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabVideo, "" ) );
    m_pComboAspectRatio->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f ) );
    m_pComboAspectRatio->GetPosition ( vecTemp, false );
    m_pComboAspectRatio->SetSize ( CVector2D ( 200.0f, 95.0f ) );
    m_pComboAspectRatio->GetSize ( vecSize );
    m_pComboAspectRatio->AddItem ( _("Auto") )->SetData ( (void*)ASPECT_RATIO_AUTO );
    m_pComboAspectRatio->AddItem ( _("4:3") )->SetData ( (void*)ASPECT_RATIO_4_3 );
    m_pComboAspectRatio->AddItem ( _("16:10") )->SetData ( (void*)ASPECT_RATIO_16_10 );
    m_pComboAspectRatio->AddItem ( _("16:9") )->SetData ( (void*)ASPECT_RATIO_16_9 );
    m_pComboAspectRatio->SetReadOnly ( true );

    m_pCheckBoxHudMatchAspectRatio = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("HUD Match Aspect Ratio"), true ) );
    m_pCheckBoxHudMatchAspectRatio->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY + 3.0f ) );
    m_pCheckBoxHudMatchAspectRatio->AutoSize ( NULL, 20.0f );

    vecTemp.fX = 11;

    m_pCheckBoxVolumetricShadows = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Volumetric Shadows"), true ) );
    m_pCheckBoxVolumetricShadows->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    m_pCheckBoxVolumetricShadows->AutoSize ( NULL, 20.0f );

    m_pCheckBoxGrass = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Grass effect"), true ) );
    m_pCheckBoxGrass->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 50.0f ) );
    m_pCheckBoxGrass->AutoSize ( NULL, 20.0f );

    m_pCheckBoxHeatHaze = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Heat haze"), true ) );
    m_pCheckBoxHeatHaze->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 70.0f ) );
    m_pCheckBoxHeatHaze->AutoSize ( NULL, 20.0f );

    m_pCheckBoxTyreSmokeParticles = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Tyre Smoke etc"), true ) );
    m_pCheckBoxTyreSmokeParticles->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 90.0f ) );
    m_pCheckBoxTyreSmokeParticles->AutoSize ( NULL, 20.0f );

    float fPosY =  vecTemp.fY;
    m_pCheckBoxMinimize = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Full Screen Minimize"), true ) );
    m_pCheckBoxMinimize->SetPosition ( CVector2D ( vecTemp.fX + 245.0f, fPosY + 30.0f ) );
    m_pCheckBoxMinimize->AutoSize ( NULL, 20.0f );

#ifndef SHOWALLSETTINGS
    if ( !GetVideoModeManager ()->IsMultiMonitor () )
    {
        m_pCheckBoxMinimize->SetVisible ( false );
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxDisableAero = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Disable Aero Desktop"), true ) );
    m_pCheckBoxDisableAero->SetPosition ( CVector2D ( vecTemp.fX + 245.0f, fPosY + 50.0f ) );
    m_pCheckBoxDisableAero->AutoSize ( NULL, 20.0f );

#ifndef SHOWALLSETTINGS
    if ( GetApplicationSetting ( "os-version" ) < "6.1" || GetApplicationSettingInt ( "aero-changeable" ) == 0 )
    {
        m_pCheckBoxDisableAero->SetVisible ( false );
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxDisableDriverOverrides = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Disable Driver Overrides"), true ) );
    m_pCheckBoxDisableDriverOverrides->SetPosition ( CVector2D ( vecTemp.fX + 245.0f, fPosY + 70.0f ) );
    m_pCheckBoxDisableDriverOverrides->AutoSize ( NULL, 20.0f );

#ifndef SHOWALLSETTINGS
    if ( GetApplicationSettingInt ( "nvhacks", "optimus" ) )
    {
        m_pCheckBoxDisableDriverOverrides->SetVisible ( false );
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxDeviceSelectionDialog = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Enable Device Selection Dialog"), true ) );
    m_pCheckBoxDeviceSelectionDialog->SetPosition ( CVector2D ( vecTemp.fX + 245.0f, fPosY + 90.0f ) );
    m_pCheckBoxDeviceSelectionDialog->AutoSize ( NULL, 20.0f );

#ifndef SHOWALLSETTINGS
    if ( !GetVideoModeManager ()->IsMultiMonitor () )
    {
        m_pCheckBoxDeviceSelectionDialog->SetVisible ( false );
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxShowUnsafeResolutions = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabVideo, _("Show unsafe resolutions"), true ) );
    m_pCheckBoxShowUnsafeResolutions->SetPosition ( CVector2D ( vecTemp.fX + 245.0f, fPosY + 110.0f ) );
    m_pCheckBoxShowUnsafeResolutions->AutoSize ( NULL, 20.0f );

#ifndef SHOWALLSETTINGS
    if ( !CCore::GetSingleton ().GetGame ()->GetSettings ()->HasUnsafeResolutions () )
    {
        m_pCheckBoxShowUnsafeResolutions->SetVisible ( false );
        fPosY -= 20.0f;
    }
#endif
    vecTemp.fY += 10;

    m_pTabs->GetSize ( vecTemp );

    m_pVideoDefButton = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabVideo, _("Load defaults") ) );
    m_pVideoDefButton->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnVideoDefaultClick, this ) );
    m_pVideoDefButton->AutoSize ( NULL, 20.0f, 8.0f );
    m_pVideoDefButton->GetSize ( vecSize );
    m_pVideoDefButton->SetPosition ( CVector2D ( vecTemp.fX - vecSize.fX - 12.0f, 365 ) );
    m_pVideoDefButton->SetZOrderingEnabled ( false );

    /**
     * Interface/chat Tab
     **/
    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("General") ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 12.0f ) );
        pLabel->AutoSize ( NULL, 5.0f );
        pLabel->SetFont ( "default-bold-small" );
    }

    fIndentX = pManager->CGUI_GetMaxTextExtent( "default-normal",
        _("Language:"),
        _("Skin:"),
        _("Presets:")
    ) + 5.0f;

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("Language:") ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 35.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( );
    }

    m_pInterfaceLanguageSelector = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabInterface, "English" ) );
    m_pInterfaceLanguageSelector->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, 33.0f ) );
    m_pInterfaceLanguageSelector->SetSize ( CVector2D ( 400.0f - ( vecTemp.fX + fIndentX ), 200.0f ) );
    m_pInterfaceLanguageSelector->SetReadOnly ( true );

    // Grab languages and populate
    std::map<SString,SString> m_LanguageMap = g_pCore->GetLocalization()->GetAvailableLanguages();
    m_LanguageMap["English"] = "en_US";
    std::map<SString,SString>::const_iterator itr;
    for(itr = m_LanguageMap.begin(); itr != m_LanguageMap.end(); ++itr)
    {
        CGUIListItem* pItem = m_pInterfaceLanguageSelector->AddItem((*itr).first );
        pItem->SetData ( (*itr).second );
    }

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("Skin:") ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 65.0f ) );
        pLabel->AutoSize ( );
    }

    m_pInterfaceSkinSelector = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabInterface ) );
    m_pInterfaceSkinSelector->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, 63.0f ) );
    m_pInterfaceSkinSelector->SetSize ( CVector2D ( 400.0f - ( vecTemp.fX + fIndentX ), 200.0f ) );
    m_pInterfaceSkinSelector->SetReadOnly ( true );

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("Chat") ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 90.0f ) );
        pLabel->AutoSize ( );
        pLabel->SetFont ( "default-bold-small" );
    }


    // Presets

    {
        CGUILabel* pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("Presets:") ) );
        pLabel->SetPosition ( CVector2D ( 10.0f, 112.0f ) );
        pLabel->AutoSize ( );
    }

    m_pChatPresets = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabInterface ) );
    m_pChatPresets->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, 110.0f ) );
    m_pChatPresets->SetSize ( CVector2D ( 400.0f - ( vecTemp.fX + fIndentX ), 200.0f ) );
    m_pChatPresets->SetReadOnly ( true );

    m_pChatLoadPreset = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabInterface, _("Load") ) );
    m_pChatLoadPreset->SetPosition ( CVector2D ( 410.0f, 110.0f ) );
    m_pChatLoadPreset->SetSize ( CVector2D ( 100.0f, 24.0f ) );
    m_pChatLoadPreset->SetZOrderingEnabled ( false );

    // Color selection
    SString strChatBG = _("Chat BG");
    SString strChatText = _("Chat Text");
    SString strInputBG = _("Input BG");
    SString strInputText = _("Input Text");

    float fColorTabsTextWidth = pManager->GetTextExtent( strChatBG )
                                + pManager->GetTextExtent( strChatText )
                                + pManager->GetTextExtent( strInputBG )
                                + pManager->GetTextExtent( strInputText );

    // Add 20 for each tab
    fColorTabsTextWidth += 20 * 4;
    float fColorTabPanelWidth = Max( 320.f, fColorTabsTextWidth );

    CGUITabPanel* pColorTabPanel = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( pTabInterface ) );
    pColorTabPanel->SetPosition ( CVector2D ( 10.0f, 150.0f ) );
    pColorTabPanel->SetSize ( CVector2D ( fColorTabPanelWidth, 150.0f ) );

    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_BG, strChatBG, pColorTabPanel );
    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_TEXT, strChatText, pColorTabPanel );
    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_INPUT_BG, strInputBG, pColorTabPanel );
    CreateChatColorTab ( ChatColorTypes::CHAT_COLOR_INPUT_TEXT, strInputText, pColorTabPanel );

    // Font Selection
    m_pPaneChatFont = reinterpret_cast < CGUIScrollPane* > ( pManager->CreateScrollPane ( pTabInterface ) ); 
    m_pPaneChatFont->SetProperty ( "ContentPaneAutoSized", "False" );
    m_pPaneChatFont->SetPosition ( CVector2D ( 10.0f, 310.0f ) );
    m_pPaneChatFont->SetSize ( CVector2D ( 250.0f, 33.0f ) );

    CGUILabel* pFontLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pPaneChatFont, _("Font:") ) );
    pFontLabel->SetPosition ( CVector2D ( 0.0f, 8.0f ) );
    pFontLabel->GetPosition ( vecTemp, false );
    pFontLabel->AutoSize ( );
    pFontLabel->GetSize ( vecSize, false );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Tahoma" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ]->SetSelected ( true );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ]->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, 0.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_DEFAULT ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_CLEAR ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Verdana" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_CLEAR ]->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 120.0f, 0.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_CLEAR ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_BOLD ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Tahoma Bold" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_BOLD ]->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, 18.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_BOLD ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    m_pRadioChatFont [ ChatFonts::CHAT_FONT_ARIAL ] = reinterpret_cast < CGUIRadioButton* > ( pManager->CreateRadioButton ( m_pPaneChatFont, "Arial" ) ); 
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_ARIAL ]->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 120.0f, 18.0f ) );
    m_pRadioChatFont [ ChatFonts::CHAT_FONT_ARIAL ]->SetSize ( CVector2D ( 100.0f, 15.0f ) );

    // Misc. Options
    {
        CGUILabel* pLabel;

        fIndentX = pManager->CGUI_GetMaxTextExtent( "default-normal",
            _("Lines:"),
            _("Scale:"),
            _("Width:")
        );

        // Sizes of things
        float fLabelsWidth = fIndentX;
        float fEditsWidth = 80;
        float fLineSizeY = 24;
        float fLineGapY = 4;

        // Position
        vecTemp.fX = 522 - fEditsWidth - fLabelsWidth;
        vecTemp.fY = 199;

        // Background pane in case of overlap with the color panel
        float fBGSizeX = fEditsWidth + fLabelsWidth;
        float fBGSizeY = fLineSizeY * 3 + fLineGapY * 2;       
        float fBGBorder = 10;
        CGUITabPanel* pMiscOptionsBG = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( pTabInterface ) );
        pMiscOptionsBG->SetPosition( CVector2D( vecTemp.fX - fBGBorder, vecTemp.fY - fBGBorder - 5 ) );
        pMiscOptionsBG->SetSize( CVector2D( fBGSizeX + fBGBorder * 2, fBGSizeY + fBGBorder * 2 + 2 ) );
        pMiscOptionsBG->SetAlpha( 1 );
        pMiscOptionsBG->SetZOrderingEnabled( false );
        pMiscOptionsBG->SetAlwaysOnTop( true );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("Lines:") ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( );
        pLabel->SetAlwaysOnTop ( true );

        m_pChatLines = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabInterface, "" ) );
        m_pChatLines->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 2.0f ) );
        m_pChatLines->SetSize ( CVector2D ( 80.0f, 24.0f ) );
        m_pChatLines->SetAlwaysOnTop ( true );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("Scale:") ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( );
        pLabel->SetAlwaysOnTop ( true );

        m_pChatScaleX = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabInterface, "" ) );
        m_pChatScaleX->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 2.0f ) );
        m_pChatScaleX->SetSize ( CVector2D ( 35.0f, 24.0f ) );
        m_pChatScaleX->SetAlwaysOnTop ( true );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, "x" ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 37.0f, vecTemp.fY + 2.0f ) );
        pLabel->AutoSize ( );
        pLabel->SetAlwaysOnTop ( true );

        m_pChatScaleY = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabInterface, "" ) );
        m_pChatScaleY->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 45.0f, vecTemp.fY - 2.0f ) );
        m_pChatScaleY->SetSize ( CVector2D ( 35.0f, 24.0f ) );
        m_pChatScaleY->SetAlwaysOnTop ( true );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("Width:") ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( );
        pLabel->SetAlwaysOnTop ( true );

        m_pChatWidth = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabInterface, "" ) );
        m_pChatWidth->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 2.0f ) );
        m_pChatWidth->SetSize ( CVector2D ( 80.0f, 24.0f ) );
        m_pChatWidth->SetAlwaysOnTop ( true );


        vecTemp.fX = 360;
        vecTemp.fY = 216;
        fIndentX = pManager->CGUI_GetMaxTextExtent( "default-normal",
            _("after"),
            _("for")
        );

        m_pChatCssText = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox( pTabInterface, _("Fade out old lines") ) );
        m_pChatCssText->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 95.5f ) );
        m_pChatCssText->AutoSize ( NULL, 20.0f );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("after") ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 122.5f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ();

        m_pChatLineLife = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabInterface, "" ) );
        m_pChatLineLife->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 2.0f ) );
        m_pChatLineLife->SetSize ( CVector2D ( 45.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("sec") ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 55.0f, vecTemp.fY ) );
        pLabel->AutoSize ( );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("for") ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 28.0f ) );
        pLabel->GetPosition ( vecTemp );
        pLabel->AutoSize ( );

        m_pChatLineFadeout = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( pTabInterface, "" ) );
        m_pChatLineFadeout->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 2.0f ) );
        m_pChatLineFadeout->SetSize ( CVector2D ( 45.0f, 24.0f ) );

        pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabInterface, _("sec") ) );
        pLabel->SetPosition ( CVector2D ( vecTemp.fX + fIndentX + 55.0f, vecTemp.fY ) );
        pLabel->AutoSize ( );

        if ( m_pChatCssText->GetSize().fX > 170 )
        {
            m_pChatCssText->SetPosition ( 
                CVector2D ( pLabel->GetPosition().fX + pLabel->GetSize().fX - m_pChatCssText->GetSize().fX, 
                            m_pChatCssText->GetPosition().fY ) 
                          );
        }

        m_pChatCssBackground = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox( pTabInterface, _("Hide background when not typing") ) );  //!ACHTUNG: This is nasty.  Use a wrapped textbox
        m_pChatCssBackground->SetPosition ( CVector2D ( 10.0f, 350.0f ) );
        m_pChatCssBackground->GetPosition ( vecTemp );
        m_pChatCssBackground->AutoSize ( NULL, 20.0f );
    }

    /**
     *  Advanced tab
     **/
    vecTemp = CVector2D ( 12.f, 12.f );

    // Misc section label
    m_pAdvancedMiscLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Misc") ) );
    m_pAdvancedMiscLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pAdvancedMiscLabel->SetFont ( "default-bold-small" );
    m_pAdvancedMiscLabel->AutoSize ( );
    vecTemp.fY += 20;

    fIndentX = pManager->CGUI_GetMaxTextExtent( "default-normal",
        _("Fast CJ clothes loading:"),
        _("Browser speed:"),
        _("Single connection:"),
        _("Fullscreen mode:"),
        _("Process priority:"),
        _("Debug setting:"),
        _("Streaming memory:"),
        _("Update build type:")
    ) + 5.0f;

    vecTemp.fX += 10.0f;
    // Fast clothes loading
    m_pFastClothesLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Fast CJ clothes loading:") ) );
    m_pFastClothesLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pFastClothesLabel->AutoSize ( );

    m_pFastClothesCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pFastClothesCombo->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pFastClothesCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pFastClothesCombo->AddItem ( _("Off") )->SetData ( (void*)CMultiplayer::FAST_CLOTHES_OFF );
    m_pFastClothesCombo->AddItem ( _("On") )->SetData ( (void*)CMultiplayer::FAST_CLOTHES_ON );
    m_pFastClothesCombo->AddItem ( _("Auto") )->SetData ( (void*)CMultiplayer::FAST_CLOTHES_AUTO );
    m_pFastClothesCombo->SetReadOnly ( true );
    vecTemp.fY += 29;

    // Browser scan speed
    m_pBrowserSpeedLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Browser speed:") ) );
    m_pBrowserSpeedLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pBrowserSpeedLabel->AutoSize ( );

    m_pBrowserSpeedCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pBrowserSpeedCombo->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pBrowserSpeedCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pBrowserSpeedCombo->AddItem ( _("Very slow") )->SetData ( (void*)0 );
    m_pBrowserSpeedCombo->AddItem ( _("Slow") )->SetData ( (void*)1 );
    m_pBrowserSpeedCombo->AddItem ( _("Fast") )->SetData ( (void*)2 );
    m_pBrowserSpeedCombo->SetReadOnly ( true );
    vecTemp.fY += 29;

    // Single download
    m_pSingleDownloadLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Single connection:") ) );
    m_pSingleDownloadLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pSingleDownloadLabel->AutoSize ( );

    m_pSingleDownloadCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pSingleDownloadCombo->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pSingleDownloadCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pSingleDownloadCombo->AddItem ( _("Default") )->SetData ( (void*)0 );
    m_pSingleDownloadCombo->AddItem ( _("On") )->SetData ( (void*)1 );
    m_pSingleDownloadCombo->SetReadOnly ( true );
    vecTemp.fY += 29;

    // Fullscreen mode
    m_pFullscreenStyleLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Fullscreen mode:") ) );
    m_pFullscreenStyleLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pFullscreenStyleLabel->AutoSize ( );

    m_pFullscreenStyleCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pFullscreenStyleCombo->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pFullscreenStyleCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pFullscreenStyleCombo->AddItem ( _("Standard") )->SetData ( (void*)FULLSCREEN_STANDARD );
    m_pFullscreenStyleCombo->AddItem ( _("Borderless window") )->SetData ( (void*)FULLSCREEN_BORDERLESS );
    m_pFullscreenStyleCombo->AddItem ( _("Borderless keep res") )->SetData ( (void*)FULLSCREEN_BORDERLESS_KEEP_RES );
    m_pFullscreenStyleCombo->SetReadOnly ( true );
    vecTemp.fY += 29;

    // Process priority
    m_pPriorityLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "Process priority:" ) );
    m_pPriorityLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pPriorityLabel->AutoSize ( );

    m_pPriorityCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pPriorityCombo->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pPriorityCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pPriorityCombo->AddItem ( "Normal" )->SetData ( (void*)0 );
    m_pPriorityCombo->AddItem ( "Above normal" )->SetData ( (void*)1 );
    m_pPriorityCombo->AddItem ( "High" )->SetData ( (void*)2 );
    m_pPriorityCombo->SetReadOnly ( true );
    vecTemp.fY += 29;

    // Debug setting
    m_pDebugSettingLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Debug setting:") ) );
    m_pDebugSettingLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pDebugSettingLabel->AutoSize ( );

    m_pDebugSettingCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pDebugSettingCombo->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pDebugSettingCombo->SetSize ( CVector2D ( 148.0f, 20.0f * ( EDiagnosticDebug::MAX + 1 ) ) );
    m_pDebugSettingCombo->AddItem ( _("Default") )->SetData ( (void*)0 );
    m_pDebugSettingCombo->AddItem ( "#6734 Graphics" )->SetData ( (void*)EDiagnosticDebug::GRAPHICS_6734 );
    //m_pDebugSettingCombo->AddItem ( "#6778 BIDI" )->SetData ( (void*)EDiagnosticDebug::BIDI_6778 );
    m_pDebugSettingCombo->AddItem ( "#6732 D3D" )->SetData ( (void*)EDiagnosticDebug::D3D_6732 );
    m_pDebugSettingCombo->AddItem ( "#0000 Log timing" )->SetData ( (void*)EDiagnosticDebug::LOG_TIMING_0000 );
    m_pDebugSettingCombo->AddItem ( "#0000 Joystick" )->SetData ( (void*)EDiagnosticDebug::JOYSTICK_0000 );
    m_pDebugSettingCombo->AddItem ( "#0000 Lua trace" )->SetData ( (void*)EDiagnosticDebug::LUA_TRACE_0000 );
    m_pDebugSettingCombo->SetReadOnly ( true );
    vecTemp.fY += 29;

    m_pDebugSettingCombo->SetText ( _("Default") );
    SetApplicationSetting ( "diagnostics", "debug-setting", "none" );

    // Streaming memory
    m_pStreamingMemoryLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Streaming memory:") ) );
    m_pStreamingMemoryLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 0.0f ) );
    m_pStreamingMemoryLabel->AutoSize ( );

    unsigned int uiMinMemory = g_pCore->GetMinStreamingMemory ();
    unsigned int uiMaxMemory = g_pCore->GetMaxStreamingMemory ();

    m_pStreamingMemoryMinLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Min")) );
    m_pStreamingMemoryMinLabel->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY ) );
    m_pStreamingMemoryMinLabel->AutoSize ( );
    m_pStreamingMemoryMinLabel->GetSize ( vecSize );
    m_pStreamingMemoryMinLabel->GetPosition ( vecTemp );

    m_pStreamingMemory = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTabAdvanced ) );
    m_pStreamingMemory->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pStreamingMemory->GetPosition ( vecTemp );
    m_pStreamingMemory->SetSize ( CVector2D ( 130.0f, 20.0f ) );
    m_pStreamingMemory->GetSize ( vecSize );
    m_pStreamingMemory->SetProperty ( "StepSize", SString("%.07lf", 1.0 / (uiMaxMemory - uiMinMemory)) );

    m_pStreamingMemoryMaxLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Max")) );
    m_pStreamingMemoryMaxLabel->SetPosition ( CVector2D ( vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY ) );
    m_pStreamingMemoryMaxLabel->AutoSize ( );
    vecTemp.fX = 22.f;
    vecTemp.fY += 29;

    // Windows 8 compatibility
    m_pWin8Label = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Windows 8 compatibility:") ) );
    m_pWin8Label->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pWin8Label->AutoSize ( );
    vecTemp.fX += 20;

    m_pWin8ColorCheckBox = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAdvanced, _("16-bit color") ) );
    m_pWin8ColorCheckBox->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pWin8ColorCheckBox->AutoSize ( NULL, 20.0f );
    vecTemp.fX += 90;

    m_pWin8MouseCheckBox = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( pTabAdvanced, _("Mouse fix") ) );
    m_pWin8MouseCheckBox->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pWin8MouseCheckBox->AutoSize ( NULL, 20.0f );
    vecTemp.fY += 29;
    vecTemp.fX -= 110;

    if ( GetApplicationSetting ( "os-version" ) < "6.2" )
    {
        m_pWin8Label->SetVisible ( false );
        m_pWin8ColorCheckBox->SetVisible ( false );
        m_pWin8MouseCheckBox->SetVisible ( false );
        vecTemp.fY -= 29;
    }

    // Auto updater section label
    m_pAdvancedUpdaterLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Auto updater") ) );
    m_pAdvancedUpdaterLabel->SetPosition ( CVector2D ( vecTemp.fX - 10.0f, vecTemp.fY ) );
    m_pAdvancedUpdaterLabel->SetFont ( "default-bold-small" );
    m_pAdvancedUpdaterLabel->AutoSize ( _("Auto updater") );
    vecTemp.fY += 20;

    // Update build type
    m_pUpdateBuildTypeLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, _("Update build type:") ) );
    m_pUpdateBuildTypeLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pUpdateBuildTypeLabel->AutoSize ( );

    m_pUpdateBuildTypeCombo = reinterpret_cast < CGUIComboBox* > ( pManager->CreateComboBox ( pTabAdvanced, "" ) );
    m_pUpdateBuildTypeCombo->SetPosition ( CVector2D ( vecTemp.fX + fIndentX, vecTemp.fY - 1.0f ) );
    m_pUpdateBuildTypeCombo->SetSize ( CVector2D ( 148.0f, 95.0f ) );
    m_pUpdateBuildTypeCombo->AddItem ( _("Default") )->SetData ( (void*)0 );
    m_pUpdateBuildTypeCombo->AddItem ( "Beta" )->SetData ( (void*)1 );
    m_pUpdateBuildTypeCombo->AddItem ( "Nightly" )->SetData ( (void*)2 );
    m_pUpdateBuildTypeCombo->SetReadOnly ( true );
    vecTemp.fY += 29;

    // Check for updates
    m_pButtonUpdate = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( pTabAdvanced, _("Check for update now") ) );
    m_pButtonUpdate->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY ) );
    m_pButtonUpdate->AutoSize ( NULL, 20.0f, 8.0f );
    m_pButtonUpdate->SetClickHandler ( GUI_CALLBACK ( &CSettings::OnUpdateButtonClick, this ) );
    m_pButtonUpdate->SetZOrderingEnabled ( false );

    // Description label
    vecTemp.fY = 354;
    m_pAdvancedSettingDescriptionLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTabAdvanced, "" ) );
    m_pAdvancedSettingDescriptionLabel->SetPosition ( CVector2D ( vecTemp.fX + 10.f, vecTemp.fY ) );
    m_pAdvancedSettingDescriptionLabel->SetFont ( "default-bold-small" );
    m_pAdvancedSettingDescriptionLabel->SetSize ( CVector2D ( 500.0f, 95.0f ) );
    m_pAdvancedSettingDescriptionLabel->SetHorizontalAlign ( CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP );

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
    m_pAudioVoiceVolume->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnVoiceVolumeChanged, this ) );
    m_pDrawDistance->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnDrawDistanceChanged, this ) );
    m_pBrightness->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnBrightnessChanged, this ) );
    m_pAnisotropic->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnAnisotropicChanged, this ) );
    m_pMouseSensitivity->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnMouseSensitivityChanged, this ) );
    m_pVerticalAimSensitivity->SetOnScrollHandler ( GUI_CALLBACK ( &CSettings::OnVerticalAimSensitivityChanged, this ) );
    m_pComboFxQuality->SetSelectionHandler ( GUI_CALLBACK( &CSettings::OnFxQualityChanged, this ) );
    m_pCheckBoxVolumetricShadows->SetClickHandler ( GUI_CALLBACK( &CSettings::OnVolumetricShadowsClick, this ) );
    m_pCheckBoxAllowScreenUpload->SetClickHandler ( GUI_CALLBACK( &CSettings::OnAllowScreenUploadClick, this ) );
    m_pCheckBoxCustomizedSAFiles->SetClickHandler ( GUI_CALLBACK( &CSettings::OnCustomizedSAFilesClick, this ) );
    m_pCheckBoxShowUnsafeResolutions->SetClickHandler ( GUI_CALLBACK( &CSettings::ShowUnsafeResolutionsClick, this ) );
    /*
    // Give a warning if no community account settings were stored in config
    CCore::GetSingleton ().ShowMessageBox ( CORE_SETTINGS_COMMUNITY_WARNING, _("Multi Theft Auto: Community settings"), MB_ICON_WARNING );
    */

    // Set up the events for advanced description 
    m_pPriorityLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pPriorityLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );    

    m_pPriorityCombo->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pPriorityCombo->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pFastClothesLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pFastClothesLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pFastClothesCombo->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pFastClothesCombo->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pBrowserSpeedLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pBrowserSpeedLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pBrowserSpeedCombo->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pBrowserSpeedCombo->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pSingleDownloadLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pSingleDownloadLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pSingleDownloadCombo->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pSingleDownloadCombo->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pFullscreenStyleLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pFullscreenStyleLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pFullscreenStyleCombo->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pFullscreenStyleCombo->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );
    
    m_pDebugSettingLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pDebugSettingLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );
    
    m_pDebugSettingCombo->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pDebugSettingCombo->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pStreamingMemoryLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pStreamingMemoryLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pStreamingMemory->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pStreamingMemory->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pUpdateBuildTypeLabel->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pUpdateBuildTypeLabel->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pUpdateBuildTypeCombo->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pUpdateBuildTypeCombo->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pWin8ColorCheckBox->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pWin8ColorCheckBox->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );

    m_pWin8MouseCheckBox->SetMouseEnterHandler ( GUI_CALLBACK ( &CSettings::OnShowAdvancedSettingDescription, this ) );
    m_pWin8MouseCheckBox->SetMouseLeaveHandler ( GUI_CALLBACK ( &CSettings::OnHideAdvancedSettingDescription, this ) );


    // Load Chat presets
    LoadChatPresets ();

    // Load the load of skins
    LoadSkins();
}


void CSettings::DestroyGUI ( void )
{
    // Destroy
    delete m_pButtonCancel;
    delete m_pButtonOK;
    delete m_pWindow;
    m_pWindow = NULL;
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


void CSettings::ShowRestartQuestion ( void )
{
    SString strMessage = _("Some settings will be changed when you next start MTA");
    strMessage += _("\n\nDo you want to restart now?");
    CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
    pQuestionBox->Reset ();
    pQuestionBox->SetTitle ( _("RESTART REQUIRED") );
    pQuestionBox->SetMessage ( strMessage );
    pQuestionBox->SetButton ( 0, _("No") );
    pQuestionBox->SetButton ( 1, _("Yes") );
    pQuestionBox->SetCallback ( RestartCallBack );
    pQuestionBox->Show ();
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


void CSettings::UpdateAudioTab ()
{
    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();

    float fMTAVolume = 0, fRadioVolume = (float)(gameSettings->GetRadioVolume()) / 64.0f, fSFXVolume = (float)(gameSettings->GetSFXVolume()) / 64.0f;

    CVARS_GET ( "mtavolume", fMTAVolume );

    m_pAudioMTAVolume->SetScrollPosition( fMTAVolume );
    m_pAudioRadioVolume->SetScrollPosition( fRadioVolume );
    m_pAudioSFXVolume->SetScrollPosition( fSFXVolume );

    m_pCheckBoxAudioEqualizer->SetSelected( gameSettings->IsRadioEqualizerEnabled() );
    m_pCheckBoxAudioAutotune->SetSelected( gameSettings->IsRadioAutotuneEnabled() );
    m_pCheckBoxUserAutoscan->SetSelected( gameSettings->IsUsertrackAutoScan() );

    CVARS_GET ( "mute_sfx_when_minimized", m_bMuteSFX );
    CVARS_GET ( "mute_radio_when_minimized", m_bMuteRadio );
    CVARS_GET ( "mute_mta_when_minimized", m_bMuteMTA );
    CVARS_GET ( "mute_voice_when_minimized", m_bMuteVoice );
    m_pCheckBoxMuteSFX->SetSelected ( m_bMuteSFX );
    m_pCheckBoxMuteRadio->SetSelected ( m_bMuteRadio );
    m_pCheckBoxMuteMTA->SetSelected ( m_bMuteMTA );
    m_pCheckBoxMuteVoice->SetSelected ( m_bMuteVoice );

    m_pComboUsertrackMode->SetSelectedItemByIndex( gameSettings->GetUsertrackMode() );

}

void CSettings::UpdateVideoTab ( void )
{
    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();

    bool bNextWindowed;
    bool bNextFSMinimize;
    int iNextVidMode;
    int iNextFullscreenStyle;
    GetVideoModeManager ()->GetNextVideoMode ( iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle );

    m_pCheckBoxMipMapping->SetSelected ( gameSettings->IsMipMappingEnabled () );
    m_pCheckBoxWindowed->SetSelected ( bNextWindowed );
    m_pCheckBoxMinimize->SetSelected ( bNextFSMinimize );
    m_pCheckBoxDisableAero->SetSelected ( GetApplicationSettingInt ( "aero-enabled" ) ? false : true );
    m_pCheckBoxDisableDriverOverrides->SetSelected ( GetApplicationSettingInt ( "driver-overrides-disabled" ) ? true : false );
    m_pDrawDistance->SetScrollPosition ( ( gameSettings->GetDrawDistance () - 0.925f ) / 0.8749f );
    m_pBrightness->SetScrollPosition ( ( float )gameSettings->GetBrightness () / 384 );

    // Anisotropic filtering
    int iAnisotropic;
    CVARS_GET ( "anisotropic", iAnisotropic );
    m_pAnisotropic->SetScrollPosition ( iAnisotropic / ( float )m_iMaxAnisotropic );

    int FxQuality = gameSettings->GetFXQuality();
    if ( FxQuality == 0 ) m_pComboFxQuality->SetText ( _("Low") );
    else if ( FxQuality == 1 ) m_pComboFxQuality->SetText ( _("Medium") );
    else if ( FxQuality == 2 ) m_pComboFxQuality->SetText ( _("High") );
    else if ( FxQuality == 3 ) m_pComboFxQuality->SetText ( _("Very high") );

    char AntiAliasing = gameSettings->GetAntiAliasing();
    if ( AntiAliasing == 1 ) m_pComboAntiAliasing->SetText ( _("Off") );
    else if ( AntiAliasing == 2 ) m_pComboAntiAliasing->SetText ( _("1x") );
    else if ( AntiAliasing == 3 ) m_pComboAntiAliasing->SetText ( _("2x") );
    else if ( AntiAliasing == 4 ) m_pComboAntiAliasing->SetText ( _("3x") );

    // Aspect ratio
    int aspectRatio;
    CVARS_GET("aspect_ratio", aspectRatio);
    if ( aspectRatio == ASPECT_RATIO_AUTO ) m_pComboAspectRatio->SetText ( _("Auto") );
    else if ( aspectRatio == ASPECT_RATIO_4_3 ) m_pComboAspectRatio->SetText ( _("4:3") );
    else if ( aspectRatio == ASPECT_RATIO_16_10 ) m_pComboAspectRatio->SetText ( _("16:10") );
    else if ( aspectRatio == ASPECT_RATIO_16_9 ) m_pComboAspectRatio->SetText ( _("16:9") );

    // HUD match aspect ratio
    m_pCheckBoxHudMatchAspectRatio->SetSelected( CVARS_GET_VALUE < bool > ( "hud_match_aspect_ratio" ) );

    // Volumetric shadows
    bool bVolumetricShadowsEnabled;
    CVARS_GET("volumetric_shadows", bVolumetricShadowsEnabled);
    m_pCheckBoxVolumetricShadows->SetSelected ( bVolumetricShadowsEnabled );
    m_pCheckBoxVolumetricShadows->SetEnabled ( FxQuality != 0 );

    // Device selection dialog
    bool bDeviceSelectionDialogEnabled = GetApplicationSettingInt ( "device-selection-disabled" ) ? false : true;
    m_pCheckBoxDeviceSelectionDialog->SetSelected ( bDeviceSelectionDialogEnabled );

    // Show unsafe resolutions
    bool bShowUnsafeResolutions;
    CVARS_GET("show_unsafe_resolutions", bShowUnsafeResolutions);
    m_pCheckBoxShowUnsafeResolutions->SetSelected ( bShowUnsafeResolutions );

    // Allow screen upload
    bool bAllowScreenUploadEnabled;
    CVARS_GET("allow_screen_upload", bAllowScreenUploadEnabled);
    m_pCheckBoxAllowScreenUpload->SetSelected ( bAllowScreenUploadEnabled );

    // Customized sa files
    m_pCheckBoxCustomizedSAFiles->SetSelected ( GetApplicationSettingInt ( "customized-sa-files-request" ) != 0 );
	m_pCheckBoxCustomizedSAFiles->SetVisible ( GetApplicationSettingInt ( "customized-sa-files-show" ) != 0 );

    // Grass
    bool bGrassEnabled;
    CVARS_GET ( "grass", bGrassEnabled );
    m_pCheckBoxGrass->SetSelected ( bGrassEnabled );
    m_pCheckBoxGrass->SetEnabled ( FxQuality != 0 );

    // Heat haze
    bool bHeatHazeEnabled;
    CVARS_GET ( "heat_haze", bHeatHazeEnabled );
    m_pCheckBoxHeatHaze->SetSelected ( bHeatHazeEnabled );

    // Tyre smoke
    bool bTyreSmokeEnabled;
    CVARS_GET ( "tyre_smoke_enabled", bTyreSmokeEnabled );
    m_pCheckBoxTyreSmokeParticles->SetSelected ( bTyreSmokeEnabled );

    PopulateResolutionComboBox();
    
    // Fullscreen style
    if ( iNextFullscreenStyle == FULLSCREEN_STANDARD ) m_pFullscreenStyleCombo->SetText ( _("Standard") );
    else if ( iNextFullscreenStyle == FULLSCREEN_BORDERLESS ) m_pFullscreenStyleCombo->SetText ( _("Borderless window") );
    else if ( iNextFullscreenStyle == FULLSCREEN_BORDERLESS_KEEP_RES ) m_pFullscreenStyleCombo->SetText ( _("Borderless keep res") );

    // Streaming memory
    unsigned int uiStreamingMemory = 0;
    CVARS_GET ( "streaming_memory", uiStreamingMemory );
    uiStreamingMemory = SharedUtil::Clamp ( g_pCore->GetMinStreamingMemory (), uiStreamingMemory, g_pCore->GetMaxStreamingMemory () );
    float fPos = SharedUtil::Unlerp ( g_pCore->GetMinStreamingMemory (), uiStreamingMemory, g_pCore->GetMaxStreamingMemory () );
    m_pStreamingMemory->SetScrollPosition ( fPos );

    int iVar = 0;
    CVARS_GET ( "mapalpha", iVar );
    int iAlphaPercent = ceil( ( (float)Clamp ( 0, iVar, 255 ) / 255 ) * 100 );
    m_pMapAlphaValueLabel->SetText ( SString("%i%%", iAlphaPercent).c_str() );
    float sbPos = (float)iAlphaPercent / 100.0f;
    m_pMapAlpha->SetScrollPosition ( sbPos );

}


//
// PopulateResolutionComboBox
//
void CSettings::PopulateResolutionComboBox( void )
{
    bool bNextWindowed;
    bool bNextFSMinimize;
    int iNextVidMode;
    int iNextFullscreenStyle;
    GetVideoModeManager ()->GetNextVideoMode ( iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle );
    bool bShowUnsafeResolutions = m_pCheckBoxShowUnsafeResolutions->GetSelected ();

    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();

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

        // Check resolution is below desktop res unless that is allowed
        if ( gameSettings->IsUnsafeResolution( vidModemInfo.width, vidModemInfo.height ) && !bShowUnsafeResolutions )
            continue;

        SString strMode ( "%lu x %lu x %lu", vidModemInfo.width, vidModemInfo.height, vidModemInfo.depth );

        if ( vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE )
            m_pComboResolution->AddItem ( strMode )->SetData ( (void*)vidMode );

        VideoMode currentInfo;
        gameSettings->GetVideoModeInfo ( &currentInfo, iNextVidMode );

        if ( currentInfo.width == vidModemInfo.width && currentInfo.height == vidModemInfo.height && currentInfo.depth == vidModemInfo.depth )
            m_pComboResolution->SetText ( strMode );
    }    
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
    string strJoystickName = JoyMan->IsJoypadConnected () ? JoyMan->GetControllerName () : _("Joypad not detected  -  Check connections and restart game");

    m_pJoypadName->SetPosition ( CVector2D ( 270, m_pJoypadName->GetPosition().fY ) );
    m_pJoypadName->SetText ( strJoystickName.c_str () );
    m_pJoypadName->AutoSize ( strJoystickName.c_str () );
    m_pJoypadName->SetPosition ( m_pJoypadName->GetPosition () - CVector2D ( m_pJoypadName->GetSize ().fX * 0.5, 0.0f ) );


    // Joystick name underline
    string strUnderline = "";
    int inumChars = m_pJoypadName->GetSize ().fX / 7.f + 0.5f;
    for ( int i = 0 ; i < inumChars ; i++ )
        strUnderline = strUnderline + "_";

    m_pJoypadUnderline->SetPosition ( CVector2D ( 270, m_pJoypadUnderline->GetPosition().fY ) );
    m_pJoypadUnderline->SetText ( strUnderline.c_str () );
    m_pJoypadUnderline->AutoSize ( strUnderline.c_str () );
    m_pJoypadUnderline->SetPosition ( m_pJoypadUnderline->GetPosition () - CVector2D ( m_pJoypadUnderline->GetSize ().fX * 0.5, 0.0f ) );
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
// Called when the user clicks on the video 'Load Defaults' button.
//
bool CSettings::OnVideoDefaultClick ( CGUIElement* pElement )
{
    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();


    //gameSettings->SetMipMappingEnabled (); // Doesn't appear to even be enabled
    gameSettings->SetDrawDistance( 1.19625f ); // All values taken from a default SA install, no gta_sa.set or coreconfig.xml modifications.
    gameSettings->SetBrightness ( 253 );
    gameSettings->SetFXQuality ( 2 );
    gameSettings->SetAntiAliasing ( 1, true );
    CVARS_SET ("aspect_ratio", ASPECT_RATIO_AUTO );
    CVARS_SET ("anisotropic", 0 );
    CVARS_SET ("volumetric_shadows", false );
    CVARS_SET ( "grass", true );
    CVARS_SET ( "heat_haze", true );
    CVARS_SET ( "tyre_smoke_enabled", true );

    // change
    bool bIsVideoModeChanged = GetVideoModeManager ()->SetVideoMode ( 0, false, false, FULLSCREEN_STANDARD );

    CVARS_SET ( "streaming_memory", g_pCore->GetMaxStreamingMemory () );

    CVARS_SET ( "mapalpha", 140.25f);

    // Display restart required message if required
    bool bIsAntiAliasingChanged = gameSettings->GetAntiAliasing () != m_pComboAntiAliasing->GetSelectedItemIndex ();
    bool bIsAeroChanged = ( GetApplicationSettingInt ( "aero-enabled"  ) ? false : true ) != m_pCheckBoxDisableAero->GetSelected ();
    bool bIsDriverOverridesChanged = ( GetApplicationSettingInt ( "driver-overrides-disabled"  ) ? true : false ) != m_pCheckBoxDisableDriverOverrides->GetSelected ();
    if ( bIsVideoModeChanged || bIsAntiAliasingChanged || bIsAeroChanged || bIsDriverOverridesChanged )
        ShowRestartQuestion();

    // Update the GUI
    UpdateVideoTab ();

    return true;
}

//
// Called when the user clicks on the audio 'Load Defaults' button.
//
bool CSettings::OnAudioDefaultClick ( CGUIElement* pElement )
{   
    CGameSettings * gameSettings = CCore::GetSingleton ().GetGame ()->GetSettings ();
    gameSettings->SetRadioVolume ( 100 );
    gameSettings->SetSFXVolume ( 100 );
    CVARS_SET ( "mtavolume", 100 );

    gameSettings->SetRadioAutotuneEnabled ( true );
    gameSettings->SetRadioEqualizerEnabled ( true );

    gameSettings->SetUsertrackAutoScan ( false );

    CVARS_SET ( "mute_sfx_when_minimized", false );
    CVARS_SET ( "mute_radio_when_minimized", false );
    CVARS_SET ( "mute_mta_when_minimized", false );
    CVARS_SET ( "mute_voice_when_minimized", false );

    gameSettings->SetUsertrackMode ( 0 );
    // Update the GUI
    UpdateAudioTab ();

    return true;
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
        CCore::GetSingleton ().ShowMessageBox ( _("Binding axis"), _("Move an axis to bind, or escape to clear"), MB_ICON_QUESTION );
    }

    return true;
}


//
// Called when the user clicks on the controls tab 'Load Defaults' button.
//
bool CSettings::OnControlsDefaultClick ( CGUIElement* pElement )
{
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    CControllerConfigManager* pController = g_pCore->GetGame()->GetControllerConfigManager (); 
  
    // Load the default settings
    GetJoystickManager()->SetDefaults();
    CVARS_SET( "invert_mouse",     false );
    CVARS_SET( "fly_with_mouse",   false );
    CVARS_SET( "steer_with_mouse", false );
    CVARS_SET( "classic_controls", false );
    pController->SetVerticalAimSensitivity( 0.5f );
    CVARS_SET( "vertical_aim_sensitivity", pController->GetVerticalAimSensitivityRawValue() );
    gameSettings->SetMouseSensitivity( 0.5f );

    // Set game vars
    pController->SetMouseInverted( CVARS_GET_VALUE < bool > ( "invert_mouse" ) );
    pController->SetFlyWithMouse( CVARS_GET_VALUE < bool > ( "fly_with_mouse" ) );
    pController->SetSteerWithMouse( CVARS_GET_VALUE < bool > ( "steer_with_mouse" ) );
    pController->SetClassicControls( CVARS_GET_VALUE < bool > ( "classic_controls" ) ); 
    pController->SetVerticalAimSensitivityRawValue( CVARS_GET_VALUE < float > ( "vertical_aim_sensitivity" ) ); 

    // Update the GUI
    UpdateJoypadTab();
    m_pInvertMouse->SetSelected( CVARS_GET_VALUE < bool > ( "invert_mouse" ) );
    m_pFlyWithMouse->SetSelected( CVARS_GET_VALUE < bool > ( "fly_with_mouse" ) );
    m_pSteerWithMouse->SetSelected( CVARS_GET_VALUE < bool > ( "steer_with_mouse" ) );
    m_pStandardControls->SetSelected( !CVARS_GET_VALUE < bool > ( "classic_controls" ) );
    m_pClassicControls->SetSelected( CVARS_GET_VALUE < bool > ( "classic_controls" ) );
    m_pMouseSensitivity->SetScrollPosition( gameSettings->GetMouseSensitivity () );
    m_pVerticalAimSensitivity->SetScrollPosition( pController->GetVerticalAimSensitivity () );

    return true;
}

//
// Called when the user clicks on the bind 'Load Defaults' button.
//
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
        const char* szPri = m_pBindsList->GetItemText ( i, m_hPriKey );
        const SBindableKey* pPriKey = pKeyBinds->GetBindableFromKey ( szPri );
        const SBindableKey* pSecKeys[SecKeyNum];
        for ( int k = 0 ; k < SecKeyNum ; k++ )
        {
            const char* szSec = m_pBindsList->GetItemText ( i, m_hSecKeys[k] );
            pSecKeys[k] = pKeyBinds->GetBindableFromKey ( szSec );
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
            SString strCmdArgs = m_pBindsList->GetItemText ( i, m_hBind );

            SString strCommand, strArguments;
            strCmdArgs.Split ( ": ", &strCommand, &strArguments );

            const char* szCommand = strCommand;
            const char* szArguments = strArguments.empty () ? NULL : strArguments.c_str();

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
                SString strText = _("Press a key to bind, or escape to clear");
                CCore::GetSingleton ().ShowMessageBox ( _("Binding a primary key"), strText, MB_ICON_QUESTION );
            } else {
                // Create a messagebox to notify the user
                //sSString strText = SString::Printf ( "Press a key to bind to '%s'", pItemBind->GetText ().c_str () );
                SString strText = _("Press a key to bind, or escape to clear");
                CCore::GetSingleton ().ShowMessageBox ( _("Binding a secondary key"), strText, MB_ICON_QUESTION );
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
    m_pBindsList->SetItemText ( iRowGame, m_hBind, _("GTA GAME CONTROLS"), false, true );
    m_pBindsList->SetItemText ( m_pBindsList->AddRow (), m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true );
    m_pBindsList->SetItemText ( m_pBindsList->AddRow (), m_hBind, _("MULTIPLAYER CONTROLS"), false, true );
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
                        m_pBindsList->SetItemText ( iBind, m_hBind, _(pControl->szDescription) );
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
#ifdef MTA_DEBUG
        if ( ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) != 0 )
            CreateGUI ();   // Recreate GUI (for adjusting layout with edit and continue)
#endif
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
        CCore::GetSingleton ().ShowMessageBox ( _("Error"), _("Your nickname contains invalid characters!"), MB_BUTTON_OK | MB_ICON_INFO );
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
    CVARS_SET ( "voicevolume", m_fOldVoiceVolume );

    return true;
}


bool CSettings::OnLoginButtonClick ( CGUIElement* pElement )
{
    if ( strcmp ( m_pButtonLogin->GetText().c_str(), "Login" ) == 0 )
    {
        if ( m_pEditUser->GetText().empty() ||
            m_pEditPass->GetText().empty() )
        {
            g_pCore->ShowMessageBox ( _("Login Error"), _("Invalid username/password"), MB_BUTTON_OK | MB_ICON_ERROR );
            return true;
        }
        else
        {
            m_pEditUser->SetEnabled ( false );
            m_pEditPass->SetEnabled ( false );
            m_pButtonLogin->SetEnabled ( false );
            m_pButtonLogin->SetText ( _("Logging in...") );

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
        g_pCore->ShowMessageBox ( _("Login Error"), szError, MB_BUTTON_OK | MB_ICON_ERROR );
    }
}


void CSettings::OnLoginStateChange ( bool bResult )
{   // This function is called by CCommunity whenever the logged-in status changes
    m_pEditUser->SetEnabled ( !bResult );
    m_pEditPass->SetEnabled ( !bResult );
    m_pButtonLogin->SetText ( ( bResult ) ? _("Logout") : _("Login") );
    m_pButtonLogin->SetEnabled ( true );
    m_pButtonRegister->SetVisible ( !bResult );
}


void CSettings::LoadData ( void )
{
    // Ensure CVARS ranges ok
    CClientVariables::GetSingleton().ValidateValues ();

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
        m_pEditNick->SetText ( CNickGen::GetRandomNickname() );
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
    CControllerConfigManager* pController = g_pCore->GetGame()->GetControllerConfigManager(); 

    m_pMouseSensitivity->SetScrollPosition ( gameSettings->GetMouseSensitivity () );
    pController->SetVerticalAimSensitivityRawValue( CVARS_GET_VALUE < float > ( "vertical_aim_sensitivity" ) );
    m_pVerticalAimSensitivity->SetScrollPosition( pController->GetVerticalAimSensitivity() );

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

    CVARS_GET ( "mute_sfx_when_minimized", m_bMuteSFX );
    CVARS_GET ( "mute_radio_when_minimized", m_bMuteRadio );
    CVARS_GET ( "mute_mta_when_minimized", m_bMuteMTA );
    CVARS_GET ( "mute_voice_when_minimized", m_bMuteVoice );
    m_pCheckBoxMuteSFX->SetSelected ( m_bMuteSFX );
    m_pCheckBoxMuteRadio->SetSelected ( m_bMuteRadio );
    m_pCheckBoxMuteMTA->SetSelected ( m_bMuteMTA );
    m_pCheckBoxMuteVoice->SetSelected ( m_bMuteVoice );

    unsigned int uiUsertrackMode = gameSettings->GetUsertrackMode ();
    if ( uiUsertrackMode == 0 ) m_pComboUsertrackMode->SetText ( _("Radio") );
    else if ( uiUsertrackMode == 1 ) m_pComboUsertrackMode->SetText ( _("Random") );
    else if ( uiUsertrackMode == 2 ) m_pComboUsertrackMode->SetText ( _("Sequential") );
    
    CVARS_GET ( "mtavolume", m_fOldMTAVolume );
    CVARS_GET ( "voicevolume", m_fOldVoiceVolume );
    m_fOldMTAVolume = max( 0.0f, min( 1.0f, m_fOldMTAVolume ) );
    m_fOldVoiceVolume = max( 0.0f, min( 1.0f, m_fOldVoiceVolume ) );
    m_pAudioMTAVolume->SetScrollPosition ( m_fOldMTAVolume );
    m_pAudioVoiceVolume->SetScrollPosition ( m_fOldVoiceVolume );

    UpdateVideoTab();

    // Locale
    SString strLocale;
    CVARS_GET("locale",strLocale);
    unsigned int uiIndex = 0;
    while ( uiIndex != m_pInterfaceLanguageSelector->GetItemCount() )
    {
        CGUIListItem* pItem =  m_pInterfaceLanguageSelector->GetItemByIndex(uiIndex);
        if ( ((const char*)pItem->GetData()) == strLocale )
            break;

        uiIndex++;
    }
    m_pInterfaceLanguageSelector->SetSelectedItemByIndex(uiIndex);


    // Skins
    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);
    uiIndex = 0;
    std::string strItemText = m_pInterfaceSkinSelector->GetItemText( uiIndex );
    while ( strItemText != currentSkin )
    {
        strItemText = m_pInterfaceSkinSelector->GetItemText( ++uiIndex );
    }
    m_pInterfaceSkinSelector->SetSelectedItemByIndex(uiIndex);

    // Process priority
    int iVar;
    CVARS_GET ( "process_priority", iVar );
    if ( iVar == 0 ) m_pPriorityCombo->SetText ( "Normal" );
    else if ( iVar == 1 ) m_pPriorityCombo->SetText ( "Above normal" );
    else if ( iVar == 2 ) m_pPriorityCombo->SetText ( "High" );
    int PriorityClassList[] = { NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS };
    SetPriorityClass( GetCurrentProcess(), PriorityClassList[ CVARS_GET_VALUE < int > ( "process_priority" ) % 3 ] );

    // Fast clothes loading
    CVARS_GET ( "fast_clothes_loading", iVar );
    if ( iVar == CMultiplayer::FAST_CLOTHES_OFF ) m_pFastClothesCombo->SetText ( _("Off") );
    else if ( iVar == CMultiplayer::FAST_CLOTHES_AUTO ) m_pFastClothesCombo->SetText ( _("Auto") );
    else if ( iVar == CMultiplayer::FAST_CLOTHES_ON ) m_pFastClothesCombo->SetText ( _("On") );

    // Browser speed
    CVARS_GET ( "browser_speed", iVar );
    if ( iVar == 0 ) m_pBrowserSpeedCombo->SetText ( _("Very slow") );
    else if ( iVar == 1 ) m_pBrowserSpeedCombo->SetText ( _("Slow") );
    else if ( iVar == 2 ) m_pBrowserSpeedCombo->SetText ( _("Fast") );

    // Single download
    CVARS_GET ( "single_download", iVar );
    if ( iVar == 0 ) m_pSingleDownloadCombo->SetText ( _("Default") );
    else if ( iVar == 1 ) m_pSingleDownloadCombo->SetText ( _("On") );

    // Windows 8 16-bit color
    iVar = GetApplicationSettingInt( "Win8Color16" );
    m_pWin8ColorCheckBox->SetSelected ( iVar != 0 );

    // Windows 8 mouse fix
    iVar = GetApplicationSettingInt( "Win8MouseFix" );
    m_pWin8MouseCheckBox->SetSelected ( iVar != 0 );

    // Update build type
    CVARS_GET ( "update_build_type", iVar );
    if ( iVar == 0 ) m_pUpdateBuildTypeCombo->SetText ( _("Default") );
    else if ( iVar == 1 ) m_pUpdateBuildTypeCombo->SetText ( "Beta" );
    else if ( iVar == 2 ) m_pUpdateBuildTypeCombo->SetText ( "Nightly" );

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
    CVARS_SET ( "classic_controls", m_pClassicControls->GetSelected () );
    pController->SetClassicControls ( m_pClassicControls->GetSelected () );
    pController->SetVerticalAimSensitivity( m_pVerticalAimSensitivity->GetScrollPosition() );
    CVARS_SET( "vertical_aim_sensitivity", pController->GetVerticalAimSensitivityRawValue() );

    // Video
    // get current
    int iNextVidMode;
    bool bNextWindowed;
    bool bNextFSMinimize;
    int iNextFullscreenStyle;
    GetVideoModeManager ()->GetNextVideoMode ( iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle );
    int iAntiAliasing = gameSettings->GetAntiAliasing ();

    // update from gui
    bNextWindowed = m_pCheckBoxWindowed->GetSelected ();
    if ( CGUIListItem* pSelected = m_pComboResolution->GetSelectedItem () )
        iNextVidMode = ( int ) pSelected->GetData();
    bNextFSMinimize = m_pCheckBoxMinimize->GetSelected();
    if ( CGUIListItem* pSelected = m_pFullscreenStyleCombo->GetSelectedItem () )
        iNextFullscreenStyle = ( int ) pSelected->GetData();
    if ( CGUIListItem* pSelected = m_pComboAntiAliasing->GetSelectedItem () )
        iAntiAliasing = ( int ) pSelected->GetData();
    int iAeroEnabled = m_pCheckBoxDisableAero->GetSelected() ? 0 : 1;
    int iDriverOverridesDisabled = m_pCheckBoxDisableDriverOverrides->GetSelected() ? 1 : 0;
    bool bCustomizedSAFilesEnabled = m_pCheckBoxCustomizedSAFiles->GetSelected();
    bool bCustomizedSAFilesWasEnabled = GetApplicationSettingInt ( "customized-sa-files-request" ) != 0;

    if ( CGUIListItem* pSelected = m_pPriorityCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "process_priority", iSelected );
        int PriorityClassList[] = { NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS };
        SetPriorityClass( GetCurrentProcess(), PriorityClassList[ CVARS_GET_VALUE < int > ( "process_priority" ) % 3 ] );
    }

    // change
    bool bIsVideoModeChanged = GetVideoModeManager ()->SetVideoMode ( iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle );
    bool bIsAntiAliasingChanged = gameSettings->GetAntiAliasing () != iAntiAliasing;
    bool bIsAeroChanged = GetApplicationSettingInt ( "aero-enabled"  ) != iAeroEnabled;
    bool bIsDriverOverridesChanged = GetApplicationSettingInt ( "driver-overrides-disabled"  ) != iDriverOverridesDisabled;
    bool bIsCustomizedSAFilesChanged = bCustomizedSAFilesWasEnabled != bCustomizedSAFilesEnabled;

    gameSettings->SetAntiAliasing ( iAntiAliasing, true );
    gameSettings->SetDrawDistance ( ( m_pDrawDistance->GetScrollPosition () * 0.875f ) + 0.925f );
    gameSettings->SetBrightness ( m_pBrightness->GetScrollPosition () * 384 );
    gameSettings->SetMouseSensitivity ( m_pMouseSensitivity->GetScrollPosition () );
	gameSettings->SetMipMappingEnabled ( m_pCheckBoxMipMapping->GetSelected () );
    SetApplicationSettingInt ( "customized-sa-files-request", bCustomizedSAFilesEnabled ? 1 : 0 );

    // Update Aero override setting. This need to be a registry setting as it's done in the launcher
    SetApplicationSettingInt ( "aero-enabled", m_pCheckBoxDisableAero->GetSelected() ? 0 : 1 );
    SetApplicationSettingInt ( "driver-overrides-disabled", m_pCheckBoxDisableDriverOverrides->GetSelected() ? 1 : 0 );

    // Anisotropic filtering
    int iAnisotropic = Min < int > ( m_iMaxAnisotropic, ( m_pAnisotropic->GetScrollPosition () ) * ( m_iMaxAnisotropic + 1 ) );
    CVARS_SET( "anisotropic", iAnisotropic );

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
    }

    // HUD match aspect ratio
    bool bHudMatchAspectRatio = m_pCheckBoxHudMatchAspectRatio->GetSelected ();
    CVARS_SET ( "hud_match_aspect_ratio", bHudMatchAspectRatio );
    gameSettings->SetAspectRatio ( (eAspectRatio)CVARS_GET_VALUE < int > ( "aspect_ratio" ), bHudMatchAspectRatio );

    // Volumetric shadows
    bool bVolumetricShadowsEnabled = m_pCheckBoxVolumetricShadows->GetSelected ();
    CVARS_SET ( "volumetric_shadows", bVolumetricShadowsEnabled );
	gameSettings->SetVolumetricShadowsEnabled ( bVolumetricShadowsEnabled );

    // Device selection dialog
    bool bDeviceSelectionDialogEnabled = m_pCheckBoxDeviceSelectionDialog->GetSelected ();
    SetApplicationSettingInt ( "device-selection-disabled", bDeviceSelectionDialogEnabled ? 0 : 1 );

    // Show unsafe resolutions
    bool bShowUnsafeResolutions = m_pCheckBoxShowUnsafeResolutions->GetSelected ();
    CVARS_SET("show_unsafe_resolutions", bShowUnsafeResolutions);

    // Allow screen upload
    bool bAllowScreenUploadEnabled = m_pCheckBoxAllowScreenUpload->GetSelected ();
    CVARS_SET ( "allow_screen_upload", bAllowScreenUploadEnabled );

    // Grass
    bool bGrassEnabled = m_pCheckBoxGrass->GetSelected ();
    CVARS_SET ( "grass", bGrassEnabled );
    gameSettings->SetGrassEnabled ( bGrassEnabled );

    // Heat haze
    bool bHeatHazeEnabled = m_pCheckBoxHeatHaze->GetSelected ();
    CVARS_SET ( "heat_haze", bHeatHazeEnabled );
    g_pCore->GetMultiplayer ()->SetHeatHazeEnabled ( bHeatHazeEnabled );

    // Tyre smoke particles
    bool bTyreSmokeEnabled = m_pCheckBoxTyreSmokeParticles->GetSelected ();
    CVARS_SET ( "tyre_smoke_enabled", bTyreSmokeEnabled );
    g_pCore->GetMultiplayer ()->SetTyreSmokeEnabled ( bTyreSmokeEnabled );

    // Fast clothes loading
    if ( CGUIListItem* pSelected = m_pFastClothesCombo->GetSelectedItem () )
    {
        int iSelected = ( int ) pSelected->GetData();
        CVARS_SET ( "fast_clothes_loading", iSelected );
        g_pCore->GetMultiplayer ()->SetFastClothesLoading ( (CMultiplayer::EFastClothesLoading)iSelected );
    }

    // Audio
    gameSettings->SetRadioEqualizerEnabled ( m_pCheckBoxAudioEqualizer->GetSelected() );
    gameSettings->SetRadioAutotuneEnabled ( m_pCheckBoxAudioAutotune->GetSelected() );
    gameSettings->SetUsertrackAutoScan ( m_pCheckBoxUserAutoscan->GetSelected() );

    m_bMuteSFX = m_pCheckBoxMuteSFX->GetSelected ();
    m_bMuteRadio = m_pCheckBoxMuteRadio->GetSelected ();
    m_bMuteMTA = m_pCheckBoxMuteMTA->GetSelected ();
    m_bMuteVoice = m_pCheckBoxMuteVoice->GetSelected ();
    CVARS_SET ( "mute_sfx_when_minimized", m_bMuteSFX );
    CVARS_SET ( "mute_radio_when_minimized", m_bMuteRadio );
    CVARS_SET ( "mute_mta_when_minimized", m_bMuteMTA );
    CVARS_SET ( "mute_voice_when_minimized", m_bMuteVoice );

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

    // Windows 8 16-bit color
    SetApplicationSettingInt( "Win8Color16", m_pWin8ColorCheckBox->GetSelected() );

    // Windows 8 mouse fix
    SetApplicationSettingInt( "Win8MouseFix", m_pWin8MouseCheckBox->GetSelected() );

    // Debug setting
    if ( CGUIListItem* pSelected = m_pDebugSettingCombo->GetSelectedItem () )
    {
        EDiagnosticDebugType iSelected = ( EDiagnosticDebugType )(int) pSelected->GetData();
        g_pCore->SetDiagnosticDebug ( iSelected );
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

    // Language
    std::string strCurrentLocale;
    CVARS_GET("locale", strCurrentLocale );
    bool bIsLocaleChanged = false;

    CGUIListItem* pItem = m_pInterfaceLanguageSelector->GetSelectedItem ();
    if ( pItem )
    {
        const char* szItemText = (const char*)pItem->GetData();
        CVARS_SET("locale", std::string(szItemText) );
        SetApplicationSetting ( "locale", szItemText );
        if ( szItemText != strCurrentLocale )
            bIsLocaleChanged = true;
    }

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
    CVARS_SET ( "chat_lines", m_pChatLines->GetText () );
    CVARS_SET ( "chat_width", m_pChatWidth->GetText () );
    CVARS_SET ( "chat_css_style_text", m_pChatCssText->GetSelected () );
    CVARS_SET ( "chat_css_style_background", m_pChatCssBackground->GetSelected () );
    CVARS_SET ( "chat_line_life", GetMilliseconds ( m_pChatLineLife ) );
    CVARS_SET ( "chat_line_fade_out", GetMilliseconds ( m_pChatLineFadeout ) );

    // Set our new skin last, as it'll destroy all our GUI
    pItem = m_pInterfaceSkinSelector->GetSelectedItem ();
    if ( pItem )
        CVARS_SET("current_skin", pItem->GetText());

    // Streaming memory
    float fPos = m_pStreamingMemory->GetScrollPosition ();
    int min = g_pCore->GetMinStreamingMemory ();
    int max = g_pCore->GetMaxStreamingMemory ();
    unsigned int value = SharedUtil::Lerp ( min, fPos, max );
    CVARS_SET ( "streaming_memory", value );

    // Ensure CVARS ranges ok
    CClientVariables::GetSingleton().ValidateValues ();

    // Save the config here
    CCore::GetSingleton ().SaveConfig ();
    // Save the single player settings (e.g. video mode, volume)
    gameSettings->Save ();

    // Ask to restart?
    if ( bIsVideoModeChanged || bIsAntiAliasingChanged || bIsAeroChanged || bIsDriverOverridesChanged || bIsCustomizedSAFilesChanged || bIsLocaleChanged )
        ShowRestartQuestion();
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

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, _("Red:") ) );
    pLabel->SetPosition ( CVector2D ( 0.05f, 0.065f ), true );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( _("Red:") );

    m_pChatRedValue [ eType ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "0" ) );
    m_pChatRedValue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY) );
    m_pChatRedValue [ eType ]->AutoSize ( "255 " );

    m_pChatRed [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatRed [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatRed [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
    m_pChatRed [ eType ]->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnChatRedChanged, this ) );
    m_pChatRed [ eType ]->SetProperty ( "StepSize", "0.004" );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, _("Green:") ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( _("Green:") );

    m_pChatGreenValue [ eType ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "0" ) );
    m_pChatGreenValue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY) );
    m_pChatGreenValue [ eType ]->AutoSize ( "255 " );

    m_pChatGreen [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatGreen [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatGreen [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
    m_pChatGreen [ eType ]->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnChatGreenChanged, this ) );
    m_pChatGreen [ eType ]->SetProperty ( "StepSize", "0.004" );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, _("Blue:") ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( _("Blue:") );

    m_pChatBlueValue [ eType ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, "0" ) );
    m_pChatBlueValue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 270.0f, vecTemp.fY) );
    m_pChatBlueValue [ eType ]->AutoSize ( "255 " );

    m_pChatBlue [ eType ] = reinterpret_cast < CGUIScrollBar* > ( pManager->CreateScrollBar ( true, pTab ) );
    m_pChatBlue [ eType ]->SetPosition ( CVector2D ( vecTemp.fX + 90.0f, vecTemp.fY ) );
    m_pChatBlue [ eType ]->SetSize ( CVector2D ( 175.0f, 20.0f ) );
    m_pChatBlue [ eType ]->SetOnScrollHandler ( GUI_CALLBACK( &CSettings::OnChatBlueChanged, this ) );
    m_pChatBlue [ eType ]->SetProperty ( "StepSize", "0.004" );

    pLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( pTab, _("Transparency:") ) );
    pLabel->SetPosition ( CVector2D ( vecTemp.fX, vecTemp.fY + 30.0f ) );
    pLabel->GetPosition ( vecTemp, false );
    pLabel->AutoSize ( _("Transparency:") );

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
    // Note to anyone listening: stringstream does not handle out of range numbers well
    double dValue = strtol ( pEdit->GetText ().c_str (), NULL, 0 );
    dValue *= 1000;
    dValue = Clamp < double > ( INT_MIN, dValue, INT_MAX );
    return static_cast < int > ( dValue );
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
        g_pCore->ShowMessageBox ( _("Error")+_E("CC80"), _("Please disconnect before changing skin"), MB_BUTTON_OK | MB_ICON_INFO );
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

bool CSettings::OnAnisotropicChanged ( CGUIElement* pElement )
{
    int iAnisotropic = Min < int > ( m_iMaxAnisotropic, ( m_pAnisotropic->GetScrollPosition () ) * ( m_iMaxAnisotropic + 1 ) );

    SString strLabel;
    if ( iAnisotropic > 0 )
        strLabel = SString ( "%ix", 1 << iAnisotropic );
    else
        strLabel = _("Off");

    m_pAnisotropicValueLabel->SetText ( strLabel );
    return true;
}

bool CSettings::OnMouseSensitivityChanged ( CGUIElement* pElement )
{
    int iMouseSensitivity = ( m_pMouseSensitivity->GetScrollPosition () ) * 100;

    m_pLabelMouseSensitivityValue->SetText ( SString("%i%%", iMouseSensitivity ).c_str () );
    return true;
}

bool CSettings::OnVerticalAimSensitivityChanged ( CGUIElement* pElement )
{
    int iSensitivity = m_pVerticalAimSensitivity->GetScrollPosition() * 100;
    m_pLabelVerticalAimSensitivityValue->SetText( SString("%i%%", iSensitivity ) );
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

bool CSettings::OnVoiceVolumeChanged ( CGUIElement* pElement)
{
    int iVolume = m_pAudioVoiceVolume->GetScrollPosition () * 100.0f;
    m_pLabelVoiceVolumeValue->SetText ( SString ( "%i%%", iVolume ).c_str () );

    CVARS_SET ( "voicevolume", m_pAudioVoiceVolume->GetScrollPosition () );
    
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
        m_pCheckBoxGrass->SetEnabled ( false );
    }
    else
    {
        m_pCheckBoxVolumetricShadows->SetEnabled ( true );
        m_pCheckBoxGrass->SetEnabled ( true );
    }
        
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
        SStringX strMessage ( _("Volmetric shadows can cause some systems to slow down."
                                "\n\nAre you sure you want to enable them?") );
        CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
        pQuestionBox->Reset ();
        pQuestionBox->SetTitle ( _("PERFORMANCE WARNING") );
        pQuestionBox->SetMessage ( strMessage );
        pQuestionBox->SetButton ( 0, _("No") );
        pQuestionBox->SetButton ( 1, _("Yes") );
        pQuestionBox->SetCallback ( VolumetricShadowsCallBack, m_pCheckBoxVolumetricShadows );
        pQuestionBox->Show ();
    }
    return true;
}

//
// AllowScreenUpload
//
bool CSettings::OnAllowScreenUploadClick ( CGUIElement* pElement )
{
    if ( !m_pCheckBoxAllowScreenUpload->GetSelected () && !m_bShownAllowScreenUploadMessage )
    {
        m_bShownAllowScreenUploadMessage = true;
        SString strMessage;
        strMessage += _("Screen upload is required by some servers for anti-cheat purposes."
                        "\n\n(The chat box and GUI is excluded from the upload)\n");
        CCore::GetSingleton ().ShowMessageBox ( _("SCREEN UPLOAD INFORMATION"), strMessage, MB_BUTTON_OK | MB_ICON_INFO );
    }
    return true;
}


//
// CustomizedSAFiles
//
void CustomizedSAFilesCallBack ( void* ptr, unsigned int uiButton )
{
    CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ()->Reset ();
    if ( uiButton == 0 )
        ((CGUICheckBox*)ptr)->SetSelected ( false );
}

bool CSettings::OnCustomizedSAFilesClick ( CGUIElement* pElement )
{
    if ( m_pCheckBoxCustomizedSAFiles->GetSelected () )
    {
        SString strMessage;
        strMessage += _(    "Some files in your GTA:SA data directory are customized."
                            "\nMTA will only use these modified files if this check box is ticked."
                            "\n\nHowever, CUSTOMIZED GTA:SA FILES ARE BLOCKED BY MANY SERVERS"
                            "\n\nAre you sure you want to use them?" );
        CQuestionBox* pQuestionBox = CCore::GetSingleton ().GetLocalGUI ()->GetMainMenu ()->GetQuestionWindow ();
        pQuestionBox->Reset ();
        pQuestionBox->SetTitle ( _("CUSTOMIZED GTA:SA FILES") );
        pQuestionBox->SetMessage ( strMessage );
        pQuestionBox->SetButton ( 0, _("No") );
        pQuestionBox->SetButton ( 1, _("Yes") );
        pQuestionBox->SetCallback ( CustomizedSAFilesCallBack, m_pCheckBoxCustomizedSAFiles );
        pQuestionBox->Show ();
    }
    return true;
}

//
// ShowUnsafeResolutionsClick
//
bool CSettings::ShowUnsafeResolutionsClick ( CGUIElement* pElement )
{
    // Change list of available resolutions
    PopulateResolutionComboBox();
    return true;
}

void NewNicknameCallback ( void* ptr, unsigned int uiButton, std::string strNick )
{
    if ( uiButton == 1 )  // We hit OK
    {
        if ( !CCore::GetSingleton ().IsValidNick ( strNick.c_str () ) )
            CCore::GetSingleton ().ShowMessageBox ( _("Error")+_E("CC81"), _("Your nickname contains invalid characters!"), MB_BUTTON_OK | MB_ICON_INFO );
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
        pQuestionBox->SetTitle ( _("Please enter a nickname") );
        pQuestionBox->SetMessage ( _("Please enter a nickname to be used ingame.  \nThis will be your name when you connect to and play in a server")  );
        pQuestionBox->SetButton ( 0, _("Cancel") );
        pQuestionBox->SetButton ( 1, _("OK") );
        pQuestionBox->SetEditbox ( 0, strNick )->SetMaxLength ( MAX_PLAYER_NICK_LENGTH );
        pQuestionBox->SetCallbackEdit ( NewNicknameCallback );
        pQuestionBox->Show ();
}

bool CSettings::OnShowAdvancedSettingDescription ( CGUIElement* pElement )
{
    CGUILabel* pLabel = dynamic_cast < CGUILabel* > ( pElement );
    CGUIComboBox* pComboBox = dynamic_cast < CGUIComboBox* > ( pElement );
    CGUIScrollBar* pScrollBar = dynamic_cast < CGUIScrollBar* > ( pElement );
    CGUICheckBox* pCheckBox = dynamic_cast < CGUICheckBox* > ( pElement );

    std::string strText = "";

    if ( pLabel && pLabel == m_pPriorityLabel || pComboBox && pComboBox == m_pPriorityCombo )
        strText = std::string( _( "Process priority:" ) ) + " " + std::string( _( "Very experimental feature." ) );
    else if ( pLabel && pLabel == m_pFastClothesLabel || pComboBox && pComboBox == m_pFastClothesCombo )
        strText = std::string( _( "Fast CJ clothes loading:" ) ) + " " + std::string( _( "Stops stalls with CJ variations (Uses 65MB more RAM)" ) );
    else if ( pLabel && pLabel == m_pBrowserSpeedLabel || pComboBox && pComboBox == m_pBrowserSpeedCombo )
        strText = std::string( _( "Browser speed:" ) ) + " " + std::string( _( "Older routers may require a slower scan speed." ) );
    else if ( pLabel && pLabel == m_pSingleDownloadLabel || pComboBox && pComboBox == m_pSingleDownloadCombo )
        strText = std::string( _( "Single connection:" ) ) + " " + std::string( _( "Switch on to use only one connection when downloading." ) );
    else if ( pLabel && pLabel == m_pFullscreenStyleLabel || pComboBox && pComboBox == m_pFullscreenStyleCombo )
        strText = std::string( _( "Fullscreen mode:" ) ) + " " + std::string( _( "Experimental feature." ) );
    else if ( pLabel && pLabel == m_pDebugSettingLabel || pComboBox && pComboBox == m_pDebugSettingCombo )
        strText = std::string( _( "Debug setting:" ) ) + " " + std::string( _( "Select default always. (This setting is not saved)" ) );
    else if ( pLabel && pLabel == m_pStreamingMemoryLabel || pScrollBar && pScrollBar == m_pStreamingMemory )
        strText = std::string( _( "Streaming memory:" ) ) + " " + std::string( _( "Maximum is usually best" ) );
    else if ( pLabel && pLabel == m_pUpdateBuildTypeLabel || pComboBox && pComboBox == m_pUpdateBuildTypeCombo )
        strText = std::string( _( "Auto updater:" ) ) + " " + std::string( _( "Select default unless you like filling out bug reports." ) );
    else if ( pCheckBox && pCheckBox == m_pWin8ColorCheckBox )
        strText = std::string( _( "16-bit color:" ) ) + " " + std::string( _( "Enable 16 bit color modes - Requires MTA restart" ) );
    else if ( pCheckBox && pCheckBox == m_pWin8MouseCheckBox )
        strText = std::string( _( "Mouse fix:" ) ) + " " + std::string( _( "Mouse movement fix - May need PC restart" ) );
    
    if ( strText != "" )
        m_pAdvancedSettingDescriptionLabel->SetText ( strText.c_str () );

    return true;
}

bool CSettings::OnHideAdvancedSettingDescription ( CGUIElement* pElement )
{
    m_pAdvancedSettingDescriptionLabel->SetText ( "" );

    return true;
}
