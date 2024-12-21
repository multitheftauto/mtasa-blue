/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CSettings.cpp
 *  PURPOSE:     In-game settings window
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include <core/CClientCommands.h>
#include <game/CGame.h>
#include <game/CSettings.h>

using namespace std;

#define CORE_MTA_FILLER "cgui\\images\\mta_filler.png"
#define CORE_SETTINGS_UPDATE_INTERVAL 30            // Settings update interval in frames
#define CORE_SETTINGS_HEADERS 3
#define CORE_SETTINGS_HEADER_SPACER " "
#define CORE_SETTINGS_NO_KEY " "

extern CCore*              g_pCore;
extern SBindableGTAControl g_bcControls[];
extern SBindableKey        g_bkKeys[];

CSettings::CSettings()
{
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    m_fRadioVolume = (float)gameSettings->GetRadioVolume() / 64.0f;
    m_fSFXVolume = (float)gameSettings->GetSFXVolume() / 64.0f;

    m_iMaxAnisotropic = g_pDeviceState->AdapterState.MaxAnisotropicSetting;
    m_pWindow = NULL;
    m_bBrowserListsChanged = false;
    m_bBrowserListsLoadEnabled = false;
    CreateGUI();

    // Disable progress animation if required
    if (GetApplicationSettingInt(GENERAL_PROGRESS_ANIMATION_DISABLE))
    {
        SetApplicationSettingInt(GENERAL_PROGRESS_ANIMATION_DISABLE, 0);
        CVARS_SET("progress_animation", 0);
    }
}

CSettings::~CSettings()
{
    DestroyGUI();
}

void CSettings::CreateGUI()
{
    if (m_pWindow)
        DestroyGUI();

    CGUITab *pTabMultiplayer, *pTabVideo, *pTabAudio, *pTabBinds, *pTabControls, *pTabAdvanced;
    CGUI*    pManager = g_pCore->GetGUI();

    // Init
    m_bIsModLoaded = false;
    m_bCaptureKey = false;
    m_dwFrameCount = 0;
    m_bShownVolumetricShadowsWarning = false;
    m_bShownAllowScreenUploadMessage = false;
    CVector2D vecTemp;
    CVector2D vecSize;

    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();

    CVector2D contentSize(640, 480);
    float     fBottomButtonAreaHeight = 38;
    CVector2D tabPanelPosition;
    CVector2D tabPanelSize = contentSize - CVector2D(0, fBottomButtonAreaHeight);

    // Window size is content size plus window frame edge dims
    CVector2D windowSize = contentSize + CVector2D(9 + 9, 20 + 2);

    if (windowSize.fX <= resolution.fX && windowSize.fY <= resolution.fY)
    {
        // Create window (with frame) if it will fit inside the screen resolution
        CGUIWindow* pWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, _("SETTINGS")));
        pWindow->SetCloseButtonEnabled(true);
        pWindow->SetMovable(true);
        pWindow->SetPosition((resolution - windowSize) / 2);
        pWindow->SetSize(windowSize);
        pWindow->SetSizingEnabled(false);
        pWindow->SetAlwaysOnTop(true);
        pWindow->BringToFront();
        m_pWindow = pWindow;
        tabPanelPosition = CVector2D(0, 20);
    }
    else
    {
        // Otherwise use black background image
        CGUIStaticImage* pFiller = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage());
        pFiller->LoadFromFile(CORE_MTA_FILLER);
        pFiller->SetVisible(false);
        pFiller->SetZOrderingEnabled(false);
        pFiller->SetAlwaysOnTop(true);
        pFiller->MoveToBack();
        pFiller->SetPosition((resolution - contentSize) / 2);
        pFiller->SetSize(contentSize);
        m_pWindow = pFiller;
        tabPanelPosition = CVector2D(0, 0);
    }

    // Create the tab panel and necessary tabs
    m_pTabs = reinterpret_cast<CGUITabPanel*>(pManager->CreateTabPanel(m_pWindow));
    m_pTabs->SetPosition(tabPanelPosition);
    m_pTabs->SetSize(tabPanelSize);
    m_pTabs->SetSelectionHandler(GUI_CALLBACK(&CSettings::OnTabChanged, this));

    pTabMultiplayer = m_pTabs->CreateTab(_("Multiplayer"));
    pTabVideo = m_pTabs->CreateTab(_("Video"));
    pTabAudio = m_pTabs->CreateTab(_("Audio"));
    pTabBinds = m_pTabs->CreateTab(_("Binds"));
    pTabControls = m_pTabs->CreateTab(_("Controls"));
    m_pTabInterface = m_pTabs->CreateTab(_("Interface"));
    m_pTabBrowser = m_pTabs->CreateTab(_("Web Browser"));
    pTabAdvanced = m_pTabs->CreateTab(_("Advanced"));

    // Create buttons
    //  OK button
    m_pButtonOK = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, _("OK")));
    m_pButtonOK->SetPosition(CVector2D(contentSize.fX * 0.5f, tabPanelSize.fY + tabPanelPosition.fY + 8));
    m_pButtonOK->SetZOrderingEnabled(false);

    //  Cancel button
    m_pButtonCancel = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, _("Cancel")));
    m_pButtonCancel->SetPosition(CVector2D(contentSize.fX * 0.78f, tabPanelSize.fY + tabPanelPosition.fY + 8));
    m_pButtonCancel->SetZOrderingEnabled(false);

    /**
     *  Binds tab
     **/
    m_pBindsList = reinterpret_cast<CGUIGridList*>(pManager->CreateGridList(pTabBinds, false));
    m_pBindsList->SetPosition(CVector2D(10, 15));
    m_pBindsList->SetSize(CVector2D(620, 357));
    m_pBindsList->SetSortingEnabled(false);
    m_pBindsList->SetSelectionMode(SelectionModes::CellSingle);
    m_pBindsList->SetDoubleClickHandler(GUI_CALLBACK(&CSettings::OnBindsListClick, this));

    m_pTabs->GetSize(vecTemp);
    m_pBindsDefButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabBinds, _("Load defaults")));
    m_pBindsDefButton->SetClickHandler(GUI_CALLBACK(&CSettings::OnBindsDefaultClick, this));
    m_pBindsDefButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pBindsDefButton->GetSize(vecSize);
    m_pBindsDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    m_pBindsDefButton->SetZOrderingEnabled(false);

    /**
     *  Controls tab
     **/
    float fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", _("Mouse sensitivity:"), _("Vertical aim sensitivity:"));
    vecTemp = CVector2D(0, 13);
    // Mouse Options
    m_pControlsMouseLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Mouse options")));
    m_pControlsMouseLabel->SetPosition(CVector2D(vecTemp.fX + 11, vecTemp.fY));
    m_pControlsMouseLabel->AutoSize(NULL, 20.0f);
    m_pControlsMouseLabel->SetFont("default-bold-small");
    vecTemp.fY += 18;

    vecTemp.fX = 16;
    m_pInvertMouse = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabControls, _("Invert mouse vertically"), true));
    m_pInvertMouse->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pInvertMouse->AutoSize(NULL, 20.0f);

    m_pSteerWithMouse = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabControls, _("Steer with mouse"), true));
    m_pSteerWithMouse->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 16.0f));
    m_pSteerWithMouse->AutoSize(NULL, 20.0f);

    m_pFlyWithMouse = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabControls, _("Fly with mouse"), true));
    m_pFlyWithMouse->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pFlyWithMouse->AutoSize(NULL, 20.0f);

    // MouseSensitivity
    vecTemp.fY += 54.0f;
    m_pLabelMouseSensitivity = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Mouse sensitivity:")));
    m_pLabelMouseSensitivity->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pLabelMouseSensitivity->AutoSize();

    m_pMouseSensitivity = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabControls));
    m_pMouseSensitivity->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pMouseSensitivity->GetPosition(vecTemp);
    m_pMouseSensitivity->SetSize(CVector2D(160.0f, 20.0f));
    m_pMouseSensitivity->GetSize(vecSize);
    m_pMouseSensitivity->SetProperty("StepSize", "0.01");

    m_pLabelMouseSensitivityValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, "0%"));
    m_pLabelMouseSensitivityValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelMouseSensitivityValue->AutoSize("100%");
    vecTemp.fX = 16;
    vecTemp.fY += 24.f;

    // VerticalAimSensitivity
    m_pLabelVerticalAimSensitivity = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Vertical aim sensitivity:")));
    m_pLabelVerticalAimSensitivity->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pLabelVerticalAimSensitivity->AutoSize(m_pLabelVerticalAimSensitivity->GetText().c_str());

    m_pVerticalAimSensitivity = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabControls));
    m_pVerticalAimSensitivity->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pVerticalAimSensitivity->GetPosition(vecTemp);
    m_pVerticalAimSensitivity->SetSize(CVector2D(160.0f, 20.0f));
    m_pVerticalAimSensitivity->GetSize(vecSize);
    m_pVerticalAimSensitivity->SetProperty("StepSize", "0.01");

    m_pLabelVerticalAimSensitivityValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, "0%"));
    m_pLabelVerticalAimSensitivityValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelVerticalAimSensitivityValue->AutoSize("100%");
    vecTemp.fY += 30.f;

    vecTemp.fX = 16;
    // Joypad options
    m_pControlsJoypadLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Joypad options")));
    m_pControlsJoypadLabel->SetPosition(CVector2D(11, vecTemp.fY));
    m_pControlsJoypadLabel->AutoSize(NULL, 5.0f);
    m_pControlsJoypadLabel->SetFont("default-bold-small");
    vecTemp.fY += 18;

    // Create a mini-scrollpane for the radio buttons (only way to group them together)
    m_pControlsInputTypePane = reinterpret_cast<CGUIScrollPane*>(pManager->CreateScrollPane(pTabControls));
    m_pControlsInputTypePane->SetProperty("ContentPaneAutoSized", "False");
    m_pControlsInputTypePane->SetPosition(CVector2D(0, vecTemp.fY));
    m_pControlsInputTypePane->SetSize(CVector2D(1.0f, 0.27f), true);
    m_pControlsInputTypePane->SetZOrderingEnabled(false);

    m_pStandardControls = reinterpret_cast<CGUIRadioButton*>(pManager->CreateRadioButton(m_pControlsInputTypePane, _("Standard controls (Mouse + Keyboard)")));
    m_pStandardControls->SetSelected(true);
    m_pStandardControls->SetPosition(CVector2D(11, 0));
    m_pStandardControls->GetPosition(vecTemp);
    m_pStandardControls->AutoSize(NULL, 20.0f);
    m_pStandardControls->GetSize(vecSize);

    m_pClassicControls = reinterpret_cast<CGUIRadioButton*>(pManager->CreateRadioButton(m_pControlsInputTypePane, _("Classic controls (Joypad)")));
    m_pClassicControls->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 15, vecTemp.fY));
    m_pClassicControls->AutoSize(NULL, 20.0f);

    m_pControlsInputTypePane->GetPosition(vecTemp);
    vecTemp.fY += 24;

    // Advanced Joypad settings
    {
        m_JoypadSettingsRevision = -1;

        CJoystickManagerInterface* JoyMan = GetJoystickManager();

        m_pJoypadName = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls));
        m_pJoypadName->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
        m_pJoypadName->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        m_pJoypadName->SetPosition(CVector2D(270, vecTemp.fY));

        m_pJoypadUnderline = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls));
        m_pJoypadUnderline->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
        m_pJoypadUnderline->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        m_pJoypadUnderline->SetPosition(CVector2D(270, vecTemp.fY + 2));
        vecTemp.fY += 50;

        m_pEditDeadzone = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabControls));
        m_pEditDeadzone->SetPosition(CVector2D(10, vecTemp.fY));
        m_pEditDeadzone->SetSize(CVector2D(45.0f, 24.0f));
        m_pEditDeadzone->SetMaxLength(3);
        m_pEditDeadzone->SetTextChangedHandler(GUI_CALLBACK(&CSettings::OnJoypadTextChanged, this));
        vecTemp.fY += 31;

        m_pEditSaturation = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabControls));
        m_pEditSaturation->SetPosition(CVector2D(10, vecTemp.fY));
        m_pEditSaturation->SetSize(CVector2D(45.0f, 24.0f));
        m_pEditSaturation->SetMaxLength(3);
        m_pEditSaturation->SetTextChangedHandler(GUI_CALLBACK(&CSettings::OnJoypadTextChanged, this));

        CGUILabel* pLabelDeadZone = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Dead Zone")));
        pLabelDeadZone->SetPosition(m_pEditDeadzone->GetPosition() + CVector2D(52.f, 1.f));
        pLabelDeadZone->AutoSize();
        pLabelDeadZone->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);

        CGUILabel* pLabelSaturation = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Saturation")));
        pLabelSaturation->SetPosition(m_pEditSaturation->GetPosition() + CVector2D(52.f, 1.f));
        pLabelSaturation->AutoSize();
        pLabelSaturation->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
        vecTemp.fY += 106;

        CGUILabel* pLabelHelp = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Use the 'Binds' tab for joypad buttons.")));
        pLabelHelp->SetPosition(CVector2D(10, vecTemp.fY));
        pLabelHelp->AutoSize();
        vecTemp.fY += -91;

        // Layout the mapping buttons like a dual axis joypad
        CVector2D vecPosList[] = {CVector2D(162, 202),            // Left Stick
                                  CVector2D(280, 202), CVector2D(221, 182), CVector2D(221, 220),

                                  CVector2D(351, 202),            // Right Stick
                                  CVector2D(469, 202), CVector2D(410, 182), CVector2D(410, 220),

                                  CVector2D(410, 276),            // Acceleration/Brake
                                  CVector2D(221, 276)};

        for (int i = 0; i < JoyMan->GetOutputCount() && i < 10; i++)
        {
            CVector2D vecPos = vecPosList[i];
            vecPos.fY += vecTemp.fY - 231;
            CGUIButton* pButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabControls));
            pButton->SetPosition(vecPos + CVector2D(10, 0));
            pButton->SetSize(CVector2D(48.0f, 24.0f));
            pButton->GetSize(vecSize);
            pButton->SetUserData((void*)i);
            pButton->SetClickHandler(GUI_CALLBACK(&CSettings::OnAxisSelectClick, this));
            pButton->SetZOrderingEnabled(false);

            CGUILabel* pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls));
            pLabel->SetSize(CVector2D(160.0f, 24.0f));
            pLabel->SetPosition(CVector2D((vecPos.fX + 10) + vecSize.fX * 0.5f - 80.0f, vecPos.fY - 26));
            pLabel->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER);
            pLabel->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
            pLabel->SetVisible(i >= 8);            // Hide all labels except 'Acceleration' and 'Brake'

            m_pJoypadLabels.push_back(pLabel);
            m_pJoypadButtons.push_back(pButton);
        }

        vecTemp.fY += -74;
        CGUILabel* pLabelLeft = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Left Stick")));
        pLabelLeft->AutoSize();
        pLabelLeft->GetSize(vecSize);
        pLabelLeft->SetPosition(CVector2D(255.0f - vecSize.fX * 0.5f, vecTemp.fY));
        pLabelLeft->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);

        CGUILabel* pLabelRight = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabControls, _("Right Stick")));
        pLabelRight->AutoSize();
        pLabelRight->GetSize(vecSize);
        pLabelRight->SetPosition(CVector2D(444.0f - vecSize.fX * 0.5f, vecTemp.fY));
        pLabelRight->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
    }

    m_pTabs->GetSize(vecTemp);
    CGUIButton* pControlsDefButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabControls, _("Load defaults")));
    pControlsDefButton->SetClickHandler(GUI_CALLBACK(&CSettings::OnControlsDefaultClick, this));
    pControlsDefButton->AutoSize(NULL, 20.0f, 8.0f);
    pControlsDefButton->GetSize(vecSize);
    pControlsDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    pControlsDefButton->SetZOrderingEnabled(false);

    m_hBind = m_pBindsList->AddColumn(_("DESCRIPTION"), 0.35f);
    m_hPriKey = m_pBindsList->AddColumn(_("KEY"), 0.24f);
    for (int k = 0; k < SecKeyNum; k++)
        m_hSecKeys[k] = m_pBindsList->AddColumn(_("ALT. KEY"), 0.24f);

    /**
     *    Multiplayer tab
     **/
    m_pLabelNick = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabMultiplayer, _("Nick:")));
    m_pLabelNick->SetPosition(CVector2D(11, 13));
    m_pLabelNick->GetPosition(vecTemp, false);
    m_pLabelNick->AutoSize(_("Nick:"));
    m_pLabelNick->GetSize(vecSize);

    // Nick edit
    m_pEditNick = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabMultiplayer));
    m_pEditNick->SetPosition(CVector2D(vecSize.fX + vecTemp.fX + 50.0f, vecTemp.fY - 1.0f));
    m_pEditNick->SetSize(CVector2D(178.0f, 24.0f));
    m_pEditNick->SetMaxLength(MAX_PLAYER_NICK_LENGTH);
    m_pEditNick->SetTextAcceptedHandler(GUI_CALLBACK(&CSettings::OnOKButtonClick, this));

    m_pButtonGenerateNick = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabMultiplayer));
    m_pButtonGenerateNick->SetPosition(CVector2D(vecSize.fX + vecTemp.fX + 50.0f + 178.0f + 5.0f, vecTemp.fY - 1.0f), false);
    m_pButtonGenerateNick->SetSize(CVector2D(26.0f, 26.0f), false);
    m_pButtonGenerateNick->SetClickHandler(GUI_CALLBACK(&CSettings::OnNickButtonClick, this));
    m_pButtonGenerateNick->SetZOrderingEnabled(false);

    m_pButtonGenerateNickIcon = reinterpret_cast<CGUIStaticImage*>(pManager->CreateStaticImage(m_pButtonGenerateNick));
    m_pButtonGenerateNickIcon->SetSize(CVector2D(1, 1), true);
    m_pButtonGenerateNickIcon->LoadFromFile("cgui\\images\\serverbrowser\\refresh.png");
    m_pButtonGenerateNickIcon->SetProperty("MousePassThroughEnabled", "True");
    m_pButtonGenerateNickIcon->SetProperty("DistributeCapturedInputs", "True");

    m_pSavePasswords = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabMultiplayer, _("Save server passwords"), true));
    m_pSavePasswords->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 50.0f));
    m_pSavePasswords->GetPosition(vecTemp, false);
    m_pSavePasswords->AutoSize(NULL, 20.0f);

    m_pAutoRefreshBrowser = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabMultiplayer, _("Auto-refresh server browser"), true));
    m_pAutoRefreshBrowser->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pAutoRefreshBrowser->GetPosition(vecTemp, false);
    m_pAutoRefreshBrowser->AutoSize(NULL, 20.0f);

    m_pCheckBoxAllowScreenUpload = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabMultiplayer, _("Allow screen upload"), true));
    m_pCheckBoxAllowScreenUpload->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxAllowScreenUpload->GetPosition(vecTemp, false);
    m_pCheckBoxAllowScreenUpload->AutoSize(NULL, 20.0f);

    m_pCheckBoxAllowExternalSounds = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabMultiplayer, _("Allow external sounds"), true));
    m_pCheckBoxAllowExternalSounds->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxAllowExternalSounds->GetPosition(vecTemp, false);
    m_pCheckBoxAllowExternalSounds->AutoSize(NULL, 20.0f);

    m_pCheckBoxAlwaysShowTransferBox = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabMultiplayer, _("Always show download window"), false));
    m_pCheckBoxAlwaysShowTransferBox->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxAlwaysShowTransferBox->GetPosition(vecTemp, false);
    m_pCheckBoxAlwaysShowTransferBox->AutoSize(nullptr, 20.0f);

    m_pCheckBoxAllowDiscordRPC = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabMultiplayer, _("Allow connecting with Discord Rich Presence"), false));
    m_pCheckBoxAllowDiscordRPC->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxAllowDiscordRPC->GetPosition(vecTemp, false);
    m_pCheckBoxAllowDiscordRPC->AutoSize(NULL, 20.0f);

    m_pCheckBoxCustomizedSAFiles = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabMultiplayer, _("Use customized GTA:SA files"), true));
    m_pCheckBoxCustomizedSAFiles->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxCustomizedSAFiles->GetPosition(vecTemp, false);
    m_pCheckBoxCustomizedSAFiles->AutoSize(NULL, 20.0f);

    m_pMapRenderingLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabMultiplayer, _("Map rendering options")));
    m_pMapRenderingLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pMapRenderingLabel->GetPosition(vecTemp, false);
    m_pMapRenderingLabel->SetFont("default-bold-small");
    m_pMapRenderingLabel->AutoSize();

    m_pMapAlphaLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabMultiplayer, _("Opacity:")));
    m_pMapAlphaLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 24.0f));
    m_pMapAlphaLabel->GetPosition(vecTemp, false);
    m_pMapAlphaLabel->AutoSize();

    m_pMapAlpha = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabMultiplayer));
    m_pMapAlpha->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pMapAlpha->GetPosition(vecTemp, false);
    m_pMapAlpha->SetSize(CVector2D(160.0f, 20.0f));
    m_pMapAlpha->GetSize(vecSize);
    m_pMapAlpha->SetProperty("StepSize", "0.01");

    m_pMapAlphaValueLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabMultiplayer, "0%"));
    m_pMapAlphaValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pMapAlphaValueLabel->GetPosition(vecTemp, false);
    m_pMapAlphaValueLabel->AutoSize("100%");

    /**
     *  Audio tab
     **/
    fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", _("Master volume:"), _("Radio volume:"), _("SFX volume:"), _("MTA volume:"),
                                               _("Voice volume:"), _("Play mode:"));

    m_pAudioGeneralLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("General")));
    m_pAudioGeneralLabel->SetPosition(CVector2D(11, 13));
    m_pAudioGeneralLabel->GetPosition(vecTemp, false);
    m_pAudioGeneralLabel->AutoSize(NULL, 5.0f);
    m_pAudioGeneralLabel->SetFont("default-bold-small");

    m_pLabelMasterVolume = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("Master volume:")));
    m_pLabelMasterVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f), false);
    m_pLabelMasterVolume->GetPosition(vecTemp, false);
    m_pLabelMasterVolume->AutoSize();

    m_pAudioMasterVolume = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabAudio));
    m_pAudioMasterVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioMasterVolume->GetPosition(vecTemp, false);
    m_pAudioMasterVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioMasterVolume->GetSize(vecSize, false);
    m_pAudioMasterVolume->SetProperty("StepSize", "0.01");

    m_pLabelMasterVolumeValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, "0%"));
    m_pLabelMasterVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelMasterVolumeValue->GetPosition(vecTemp, false);
    m_pLabelMasterVolumeValue->AutoSize("100%");
    m_pLabelMasterVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pLabelRadioVolume = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("Radio volume:")));
    m_pLabelRadioVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f), false);
    m_pLabelRadioVolume->GetPosition(vecTemp, false);
    m_pLabelRadioVolume->AutoSize();

    m_pAudioRadioVolume = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabAudio));
    m_pAudioRadioVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioRadioVolume->GetPosition(vecTemp, false);
    m_pAudioRadioVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioRadioVolume->GetSize(vecSize, false);
    m_pAudioRadioVolume->SetProperty("StepSize", "0.01");

    m_pLabelRadioVolumeValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, "0%"));
    m_pLabelRadioVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelRadioVolumeValue->GetPosition(vecTemp, false);
    m_pLabelRadioVolumeValue->AutoSize("100%");
    m_pLabelRadioVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pLabelSFXVolume = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("SFX volume:")));
    m_pLabelSFXVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pLabelSFXVolume->GetPosition(vecTemp, false);
    m_pLabelSFXVolume->AutoSize();

    m_pAudioSFXVolume = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabAudio));
    m_pAudioSFXVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioSFXVolume->GetPosition(vecTemp, false);
    m_pAudioSFXVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioSFXVolume->GetSize(vecSize, false);
    m_pAudioSFXVolume->SetProperty("StepSize", "0.01");

    m_pLabelSFXVolumeValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, "0%"));
    m_pLabelSFXVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelSFXVolumeValue->GetPosition(vecTemp, false);
    m_pLabelSFXVolumeValue->AutoSize("100%");
    m_pLabelSFXVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pLabelMTAVolume = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("MTA volume:")));
    m_pLabelMTAVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pLabelMTAVolume->GetPosition(vecTemp, false);
    m_pLabelMTAVolume->AutoSize();

    m_pAudioMTAVolume = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabAudio));
    m_pAudioMTAVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioMTAVolume->GetPosition(vecTemp, false);
    m_pAudioMTAVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioMTAVolume->GetSize(vecSize, false);
    m_pAudioMTAVolume->SetProperty("StepSize", "0.01");

    m_pLabelMTAVolumeValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, "0%"));
    m_pLabelMTAVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelMTAVolumeValue->GetPosition(vecTemp, false);
    m_pLabelMTAVolumeValue->AutoSize("100%");
    m_pLabelMTAVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pLabelVoiceVolume = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("Voice volume:")));
    m_pLabelVoiceVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pLabelVoiceVolume->GetPosition(vecTemp, false);
    m_pLabelVoiceVolume->AutoSize();

    m_pAudioVoiceVolume = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabAudio));
    m_pAudioVoiceVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioVoiceVolume->GetPosition(vecTemp, false);
    m_pAudioVoiceVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioVoiceVolume->GetSize(vecSize, false);
    m_pAudioVoiceVolume->SetProperty("StepSize", "0.01");

    m_pLabelVoiceVolumeValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, "0%"));
    m_pLabelVoiceVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelVoiceVolumeValue->GetPosition(vecTemp, false);
    m_pLabelVoiceVolumeValue->AutoSize("100%");
    m_pLabelVoiceVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pAudioRadioLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("Radio options")));
    m_pAudioRadioLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f), false);
    m_pAudioRadioLabel->GetPosition(vecTemp, false);
    m_pAudioRadioLabel->AutoSize(NULL, 20.0f);
    m_pAudioRadioLabel->SetFont("default-bold-small");

    m_pCheckBoxAudioEqualizer = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Radio Equalizer"), true));
    m_pCheckBoxAudioEqualizer->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pCheckBoxAudioEqualizer->AutoSize(NULL, 20.0f);
    m_pCheckBoxAudioEqualizer->GetPosition(vecTemp);

    m_pCheckBoxAudioAutotune = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Radio Auto-tune"), true));
    m_pCheckBoxAudioAutotune->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxAudioAutotune->AutoSize(NULL, 20.0f);
    m_pCheckBoxAudioAutotune->GetPosition(vecTemp);

    m_pAudioUsertrackLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("Usertrack options")));
    m_pAudioUsertrackLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f), false);
    m_pAudioUsertrackLabel->GetPosition(vecTemp, false);
    m_pAudioUsertrackLabel->AutoSize(NULL, 20.0f);
    m_pAudioUsertrackLabel->SetFont("default-bold-small");

    m_pLabelUserTrackMode = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("Play mode:")));
    m_pLabelUserTrackMode->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pLabelUserTrackMode->GetPosition(vecTemp, false);
    m_pLabelUserTrackMode->AutoSize();

    m_pComboUsertrackMode = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAudio, ""));
    m_pComboUsertrackMode->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pComboUsertrackMode->SetSize(CVector2D(160.0f, 80.0f));
    m_pComboUsertrackMode->AddItem(_("Radio"))->SetData((void*)0);
    m_pComboUsertrackMode->AddItem(_("Random"))->SetData((void*)1);
    m_pComboUsertrackMode->AddItem(_("Sequential"))->SetData((void*)2);
    m_pComboUsertrackMode->SetReadOnly(true);

    m_pCheckBoxUserAutoscan = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Automatic Media Scan"), true));
    m_pCheckBoxUserAutoscan->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 52.0f));
    m_pCheckBoxUserAutoscan->AutoSize(NULL, 20.0f);
    m_pCheckBoxUserAutoscan->GetPosition(vecTemp, false);

    m_pAudioRadioLabel->GetPosition(vecTemp, false);
    vecTemp.fX = fIndentX + 173;
    m_pAudioMuteLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAudio, _("Mute options")));
    m_pAudioMuteLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 52.0f));
    m_pAudioMuteLabel->GetPosition(vecTemp, false);
    m_pAudioMuteLabel->AutoSize(NULL, 20.0f);
    m_pAudioMuteLabel->SetFont("default-bold-small");

    m_pCheckBoxMuteMaster = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Mute All sounds when minimized"), true));
    m_pCheckBoxMuteMaster->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pCheckBoxMuteMaster->GetPosition(vecTemp, false);
    m_pCheckBoxMuteMaster->AutoSize(NULL, 20.0f);

    m_pCheckBoxMuteRadio = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Mute Radio sounds when minimized"), true));
    m_pCheckBoxMuteRadio->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxMuteRadio->GetPosition(vecTemp, false);
    m_pCheckBoxMuteRadio->AutoSize(NULL, 20.0f);

    m_pCheckBoxMuteSFX = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Mute SFX sounds when minimized"), true));
    m_pCheckBoxMuteSFX->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxMuteSFX->GetPosition(vecTemp, false);
    m_pCheckBoxMuteSFX->AutoSize(NULL, 20.0f);

    m_pCheckBoxMuteMTA = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Mute MTA sounds when minimized"), true));
    m_pCheckBoxMuteMTA->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxMuteMTA->GetPosition(vecTemp, false);
    m_pCheckBoxMuteMTA->AutoSize(NULL, 20.0f);

    m_pCheckBoxMuteVoice = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAudio, _("Mute Voice sounds when minimized"), true));
    m_pCheckBoxMuteVoice->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxMuteVoice->GetPosition(vecTemp, false);
    m_pCheckBoxMuteVoice->AutoSize(NULL, 20.0f);

    m_pTabs->GetSize(vecTemp);
    m_pAudioDefButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabAudio, _("Load defaults")));
    m_pAudioDefButton->SetClickHandler(GUI_CALLBACK(&CSettings::OnAudioDefaultClick, this));
    m_pAudioDefButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pAudioDefButton->GetSize(vecSize);
    m_pAudioDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    m_pAudioDefButton->SetZOrderingEnabled(false);

    /**
     *  Video tab
     **/
    fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", _("Resolution:"), _("FOV:"), _("Draw Distance:"), _("Brightness:"), _("FX Quality:"),
                                               _("Anisotropic filtering:"), _("Anti-aliasing:"), _("Aspect Ratio:"), _("Opacity:"));

    m_pVideoGeneralLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("General")));
    m_pVideoGeneralLabel->SetPosition(CVector2D(11, 13));
    m_pVideoGeneralLabel->GetPosition(vecTemp, false);
    m_pVideoGeneralLabel->AutoSize(NULL, 3.0f);
    m_pVideoGeneralLabel->SetFont("default-bold-small");

    m_pVideoResolutionLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Resolution:")));
    m_pVideoResolutionLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 26.0f));
    m_pVideoResolutionLabel->GetPosition(vecTemp, false);
    m_pVideoResolutionLabel->AutoSize();

    m_pComboResolution = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabVideo, ""));
    m_pComboResolution->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 3.0f));
    m_pComboResolution->GetPosition(vecTemp, false);
    m_pComboResolution->SetSize(CVector2D(200.0f, 160.0f));
    m_pComboResolution->GetSize(vecSize);
    m_pComboResolution->SetReadOnly(true);

    m_pCheckBoxWindowed = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Windowed"), true));
    m_pCheckBoxWindowed->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY + 3.0f));
    m_pCheckBoxWindowed->AutoSize(NULL, 20.0f);
    m_pCheckBoxWindowed->GetPosition(vecTemp, false);
    m_pCheckBoxWindowed->GetSize(vecSize);

    m_pCheckBoxDPIAware = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("DPI aware"), false));
    m_pCheckBoxDPIAware->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY));
    m_pCheckBoxDPIAware->AutoSize(NULL, 20.0f);

    m_pVideoResolutionLabel->GetPosition(vecTemp, false);            // Restore our label position

    // Fullscreen mode
    vecTemp.fY += 26;
    m_pFullscreenStyleLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Fullscreen mode:")));
    m_pFullscreenStyleLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pFullscreenStyleLabel->AutoSize();

    m_pFullscreenStyleCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabVideo, ""));
    m_pFullscreenStyleCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pFullscreenStyleCombo->SetSize(CVector2D(200, 95.0f));
    m_pFullscreenStyleCombo->AddItem(_("Standard"))->SetData((void*)FULLSCREEN_STANDARD);
    m_pFullscreenStyleCombo->AddItem(_("Borderless window"))->SetData((void*)FULLSCREEN_BORDERLESS);
    m_pFullscreenStyleCombo->AddItem(_("Borderless keep res"))->SetData((void*)FULLSCREEN_BORDERLESS_KEEP_RES);
    m_pFullscreenStyleCombo->SetReadOnly(true);
    vecTemp.fY += 4;

    m_pCheckBoxMipMapping = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Mip Mapping"), true));
#ifndef MIP_MAPPING_SETTING_APPEARS_TO_DO_SOMETHING
    m_pCheckBoxMipMapping->SetPosition(CVector2D(vecTemp.fX + 340.0f, vecTemp.fY + 45.0f));
    m_pCheckBoxMipMapping->SetSize(CVector2D(224.0f, 16.0f));
    m_pCheckBoxMipMapping->SetVisible(false);
#endif

    vecTemp.fY -= 5;
    m_pFieldOfViewLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("FOV:")));
    m_pFieldOfViewLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pFieldOfViewLabel->GetPosition(vecTemp, false);
    m_pFieldOfViewLabel->AutoSize();

    m_pFieldOfView = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabVideo));
    m_pFieldOfView->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pFieldOfView->GetPosition(vecTemp, false);
    m_pFieldOfView->SetSize(CVector2D(160.0f, 20.0f));
    m_pFieldOfView->GetSize(vecSize);

    m_pFieldOfViewValueLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, "70"));
    m_pFieldOfViewValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pFieldOfViewValueLabel->AutoSize("70 ");

    vecTemp.fX = 11;
    m_pDrawDistanceLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Draw Distance:")));
    m_pDrawDistanceLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 26.0f));
    m_pDrawDistanceLabel->GetPosition(vecTemp, false);
    m_pDrawDistanceLabel->AutoSize();

    m_pDrawDistance = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabVideo));
    m_pDrawDistance->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pDrawDistance->GetPosition(vecTemp, false);
    m_pDrawDistance->SetSize(CVector2D(160.0f, 20.0f));
    m_pDrawDistance->GetSize(vecSize);
    m_pDrawDistance->SetProperty("StepSize", "0.01");

    m_pDrawDistanceValueLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, "0%"));
    m_pDrawDistanceValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pDrawDistanceValueLabel->AutoSize("100%");

    vecTemp.fX = 11;

    m_pBrightnessLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Brightness:")));
    m_pBrightnessLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 26.0f));
    m_pBrightnessLabel->GetPosition(vecTemp, false);
    m_pBrightnessLabel->AutoSize();

    m_pBrightness = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabVideo));
    m_pBrightness->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pBrightness->GetPosition(vecTemp, false);
    m_pBrightness->SetSize(CVector2D(160.0f, 20.0f));
    m_pBrightness->GetSize(vecSize);
    m_pBrightness->SetProperty("StepSize", "0.01");

    m_pBrightnessValueLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, "0%"));
    m_pBrightnessValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pBrightnessValueLabel->AutoSize("100%");

    vecTemp.fX = 11;

    m_pFXQualityLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("FX Quality:")));
    m_pFXQualityLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pFXQualityLabel->GetPosition(vecTemp, false);
    m_pFXQualityLabel->AutoSize();

    m_pComboFxQuality = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabVideo, ""));
    m_pComboFxQuality->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pComboFxQuality->SetSize(CVector2D(200.0f, 95.0f));
    m_pComboFxQuality->AddItem(_("Low"))->SetData((void*)0);
    m_pComboFxQuality->AddItem(_("Medium"))->SetData((void*)1);
    m_pComboFxQuality->AddItem(_("High"))->SetData((void*)2);
    m_pComboFxQuality->AddItem(_("Very high"))->SetData((void*)3);
    m_pComboFxQuality->SetReadOnly(true);

    m_pAnisotropicLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Anisotropic filtering:")));
    m_pAnisotropicLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pAnisotropicLabel->GetPosition(vecTemp, false);
    m_pAnisotropicLabel->AutoSize();

    m_pAnisotropic = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabVideo));
    m_pAnisotropic->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAnisotropic->GetPosition(vecTemp, false);
    m_pAnisotropic->SetSize(CVector2D(160.0f, 20.0f));
    m_pAnisotropic->GetSize(vecSize);
    m_pAnisotropic->SetProperty("StepSize", SString("%1.2f", 1 / (float)m_iMaxAnisotropic));

    m_pAnisotropicValueLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Off")));
    m_pAnisotropicValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pAnisotropicValueLabel->SetSize(CVector2D(100.0f, 20.0f));

    if (m_iMaxAnisotropic < 1)
    {
        // Hide if system can't do anisotropic filtering
        m_pFXQualityLabel->GetPosition(vecTemp, false);
        m_pAnisotropicLabel->SetVisible(false);
        m_pAnisotropic->SetVisible(false);
        m_pAnisotropicValueLabel->SetVisible(false);
    }

    vecTemp.fX = 11;

    m_pAntiAliasingLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Anti-aliasing:")));
    m_pAntiAliasingLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pAntiAliasingLabel->GetPosition(vecTemp, false);
    m_pAntiAliasingLabel->AutoSize();

    m_pComboAntiAliasing = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabVideo, ""));
    m_pComboAntiAliasing->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pComboAntiAliasing->SetSize(CVector2D(200.0f, 95.0f));
    m_pComboAntiAliasing->AddItem(_("Off"))->SetData((void*)1);
    m_pComboAntiAliasing->AddItem(_("1x"))->SetData((void*)2);
    m_pComboAntiAliasing->AddItem(_("2x"))->SetData((void*)3);
    m_pComboAntiAliasing->AddItem(_("3x"))->SetData((void*)4);
    m_pComboAntiAliasing->SetReadOnly(true);

    m_pAspectRatioLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabVideo, _("Aspect Ratio:")));
    m_pAspectRatioLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 26.0f));
    m_pAspectRatioLabel->GetPosition(vecTemp, false);
    m_pAspectRatioLabel->AutoSize();

    m_pComboAspectRatio = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabVideo, ""));
    m_pComboAspectRatio->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pComboAspectRatio->GetPosition(vecTemp, false);
    m_pComboAspectRatio->SetSize(CVector2D(200.0f, 95.0f));
    m_pComboAspectRatio->GetSize(vecSize);
    m_pComboAspectRatio->AddItem(_("Auto"))->SetData((void*)ASPECT_RATIO_AUTO);
    m_pComboAspectRatio->AddItem(_("4:3"))->SetData((void*)ASPECT_RATIO_4_3);
    m_pComboAspectRatio->AddItem(_("16:10"))->SetData((void*)ASPECT_RATIO_16_10);
    m_pComboAspectRatio->AddItem(_("16:9"))->SetData((void*)ASPECT_RATIO_16_9);
    m_pComboAspectRatio->SetReadOnly(true);

    m_pCheckBoxHudMatchAspectRatio = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("HUD Match Aspect Ratio"), true));
    m_pCheckBoxHudMatchAspectRatio->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY + 4.0f));
    m_pCheckBoxHudMatchAspectRatio->AutoSize(NULL, 20.0f);

    vecTemp.fX = 11;

    m_pCheckBoxVolumetricShadows = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Volumetric Shadows"), true));
    m_pCheckBoxVolumetricShadows->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pCheckBoxVolumetricShadows->AutoSize(NULL, 20.0f);

    m_pCheckBoxGrass = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Grass effect"), true));
    m_pCheckBoxGrass->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 50.0f));
    m_pCheckBoxGrass->AutoSize(NULL, 20.0f);

    m_pCheckBoxHeatHaze = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Heat haze"), true));
    m_pCheckBoxHeatHaze->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 70.0f));
    m_pCheckBoxHeatHaze->AutoSize(NULL, 20.0f);

    m_pCheckBoxTyreSmokeParticles = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Tyre Smoke etc"), true));
    m_pCheckBoxTyreSmokeParticles->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 90.0f));
    m_pCheckBoxTyreSmokeParticles->AutoSize(NULL, 20.0f);

    m_pCheckBoxDynamicPedShadows = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Dynamic ped shadows"), true));
    m_pCheckBoxDynamicPedShadows->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 110.0f));
    m_pCheckBoxDynamicPedShadows->AutoSize(NULL, 20.0f);

    m_pCheckBoxBlur = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Motion blur"), true));
    m_pCheckBoxBlur->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 130.0f));
    m_pCheckBoxBlur->AutoSize(NULL, 20.0f);

    float fPosY = vecTemp.fY;
    m_pCheckBoxMinimize = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Full Screen Minimize"), true));
    m_pCheckBoxMinimize->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 30.0f));
    m_pCheckBoxMinimize->AutoSize(NULL, 20.0f);

#ifndef SHOWALLSETTINGS
    if (!GetVideoModeManager()->IsMultiMonitor())
    {
        m_pCheckBoxMinimize->SetVisible(false);
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxDeviceSelectionDialog = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Enable Device Selection Dialog"), true));
    m_pCheckBoxDeviceSelectionDialog->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 50.0f));
    m_pCheckBoxDeviceSelectionDialog->AutoSize(NULL, 20.0f);

#ifndef SHOWALLSETTINGS
    if (!GetVideoModeManager()->IsMultiMonitor())
    {
        m_pCheckBoxDeviceSelectionDialog->SetVisible(false);
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxShowUnsafeResolutions = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Show unsafe resolutions"), true));
    m_pCheckBoxShowUnsafeResolutions->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 70.0f));
    m_pCheckBoxShowUnsafeResolutions->AutoSize(NULL, 20.0f);

#ifndef SHOWALLSETTINGS
    if (!CCore::GetSingleton().GetGame()->GetSettings()->HasUnsafeResolutions())
    {
        m_pCheckBoxShowUnsafeResolutions->SetVisible(false);
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxHighDetailVehicles = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Render vehicles always in high detail"), true));
    m_pCheckBoxHighDetailVehicles->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 90.0f));
    m_pCheckBoxHighDetailVehicles->AutoSize(NULL, 20.0f);

    m_pCheckBoxHighDetailPeds = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Render peds always in high detail"), true));
    m_pCheckBoxHighDetailPeds->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 110.0f));
    m_pCheckBoxHighDetailPeds->AutoSize(NULL, 20.0f);

    m_pCheckBoxCoronaReflections = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabVideo, _("Corona rain reflections"), true));
    m_pCheckBoxCoronaReflections->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 130.0f));
    m_pCheckBoxCoronaReflections->AutoSize(NULL, 20.0f);

    vecTemp.fY += 10;

    m_pTabs->GetSize(vecTemp);

    m_pVideoDefButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabVideo, _("Load defaults")));
    m_pVideoDefButton->SetClickHandler(GUI_CALLBACK(&CSettings::OnVideoDefaultClick, this));
    m_pVideoDefButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pVideoDefButton->GetSize(vecSize);
    m_pVideoDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    m_pVideoDefButton->SetZOrderingEnabled(false);

    /**
     * Interface/chat Tab
     **/
    CreateInterfaceTabGUI();

    /**
     * Webbrowser tab
     **/
    m_pLabelBrowserGeneral = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabBrowser, _("General")));
    m_pLabelBrowserGeneral->SetPosition(CVector2D(10.0f, 12.0f));
    m_pLabelBrowserGeneral->GetPosition(vecTemp);
    m_pLabelBrowserGeneral->AutoSize(NULL, 5.0f);
    m_pLabelBrowserGeneral->SetFont("default-bold-small");

    m_pCheckBoxRemoteBrowser = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTabBrowser, _("Enable remote websites"), true));
    m_pCheckBoxRemoteBrowser->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pCheckBoxRemoteBrowser->GetPosition(vecTemp);
    m_pCheckBoxRemoteBrowser->AutoSize(NULL, 20.0f);

    m_pCheckBoxRemoteJavascript = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTabBrowser, _("Enable Javascript on remote websites"), true));
    m_pCheckBoxRemoteJavascript->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 25.0f));
    m_pCheckBoxRemoteJavascript->GetPosition(vecTemp);
    m_pCheckBoxRemoteJavascript->AutoSize(NULL, 20.0f);

    m_pCheckBoxBrowserGPUEnabled = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pTabBrowser, _("Enable GPU rendering"), true));
    m_pCheckBoxBrowserGPUEnabled->SetPosition(CVector2D(vecTemp.fX + 300.0f, vecTemp.fY - 25.0f));
    m_pCheckBoxBrowserGPUEnabled->AutoSize(NULL, 20.0f);

    m_pLabelBrowserCustomBlacklist = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabBrowser, _("Custom blacklist")));
    m_pLabelBrowserCustomBlacklist->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pLabelBrowserCustomBlacklist->GetPosition(vecTemp);
    m_pLabelBrowserCustomBlacklist->AutoSize(NULL, 20.0f);
    m_pLabelBrowserCustomBlacklist->SetFont("default-bold-small");

    m_pEditBrowserBlacklistAdd = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(m_pTabBrowser));
    m_pEditBrowserBlacklistAdd->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 25.0f));
    m_pEditBrowserBlacklistAdd->GetPosition(vecTemp);
    m_pEditBrowserBlacklistAdd->SetSize(CVector2D(209.0f, 22.0f));

    m_pLabelBrowserBlacklistAdd = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pEditBrowserBlacklistAdd, _("Enter a domain e.g. google.com")));
    m_pLabelBrowserBlacklistAdd->SetPosition(CVector2D(10.0f, 3.0f), false);
    m_pLabelBrowserBlacklistAdd->SetTextColor(0, 0, 0);
    m_pLabelBrowserBlacklistAdd->SetSize(CVector2D(1, 1), true);
    m_pLabelBrowserBlacklistAdd->SetAlpha(0.7f);
    m_pLabelBrowserBlacklistAdd->SetProperty("MousePassThroughEnabled", "True");
    m_pLabelBrowserBlacklistAdd->SetProperty("DistributeCapturedInputs", "True");

    m_pButtonBrowserBlacklistAdd = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTabBrowser, _("Block")));
    m_pButtonBrowserBlacklistAdd->SetPosition(CVector2D(vecTemp.fX + m_pEditBrowserBlacklistAdd->GetSize().fX + 2.0f, vecTemp.fY));
    m_pButtonBrowserBlacklistAdd->SetSize(CVector2D(90.0f, 22.0f));

    m_pGridBrowserBlacklist = reinterpret_cast<CGUIGridList*>(pManager->CreateGridList(m_pTabBrowser));
    m_pGridBrowserBlacklist->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pGridBrowserBlacklist->GetPosition(vecTemp);
    m_pGridBrowserBlacklist->SetSize(CVector2D(300.0f, 150.0f));
    m_pGridBrowserBlacklist->AddColumn(_("Domain"), 0.9f);

    m_pButtonBrowserBlacklistRemove = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTabBrowser, _("Remove domain")));
    m_pButtonBrowserBlacklistRemove->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + m_pGridBrowserBlacklist->GetSize().fY + 5.0f));
    m_pButtonBrowserBlacklistRemove->SetSize(CVector2D(140.0f, 22.0f));

    m_pLabelBrowserCustomBlacklist->GetPosition(vecTemp);            // Reset vecTemp

    m_pLabelBrowserCustomWhitelist = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabBrowser, _("Custom whitelist")));
    m_pLabelBrowserCustomWhitelist->SetPosition(CVector2D(vecTemp.fX + 300.0f + 19.0f, vecTemp.fY));
    m_pLabelBrowserCustomWhitelist->GetPosition(vecTemp);
    m_pLabelBrowserCustomWhitelist->AutoSize(NULL, 20.0f);
    m_pLabelBrowserCustomWhitelist->SetFont("default-bold-small");

    m_pEditBrowserWhitelistAdd = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(m_pTabBrowser));
    m_pEditBrowserWhitelistAdd->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 25.0f));
    m_pEditBrowserWhitelistAdd->GetPosition(vecTemp);
    m_pEditBrowserWhitelistAdd->SetSize(CVector2D(209.0f, 22.0f));

    m_pLabelBrowserWhitelistAdd = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pEditBrowserWhitelistAdd, _("Enter a domain e.g. google.com")));
    m_pLabelBrowserWhitelistAdd->SetPosition(CVector2D(10.0f, 3.0f), false);
    m_pLabelBrowserWhitelistAdd->SetTextColor(0, 0, 0);
    m_pLabelBrowserWhitelistAdd->SetSize(CVector2D(1, 1), true);
    m_pLabelBrowserWhitelistAdd->SetAlpha(0.7f);
    m_pLabelBrowserWhitelistAdd->SetProperty("MousePassThroughEnabled", "True");
    m_pLabelBrowserWhitelistAdd->SetProperty("DistributeCapturedInputs", "True");

    m_pButtonBrowserWhitelistAdd = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTabBrowser, _("Allow")));
    m_pButtonBrowserWhitelistAdd->SetPosition(CVector2D(vecTemp.fX + m_pEditBrowserWhitelistAdd->GetSize().fX + 2.0f, vecTemp.fY));
    m_pButtonBrowserWhitelistAdd->SetSize(CVector2D(90.0f, 22.0f));

    m_pGridBrowserWhitelist = reinterpret_cast<CGUIGridList*>(pManager->CreateGridList(m_pTabBrowser));
    m_pGridBrowserWhitelist->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pGridBrowserWhitelist->GetPosition(vecTemp);
    m_pGridBrowserWhitelist->SetSize(CVector2D(300.0f, 150.0f));
    m_pGridBrowserWhitelist->AddColumn(_("Domain"), 0.9f);

    m_pButtonBrowserWhitelistRemove = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTabBrowser, _("Remove domain")));
    m_pButtonBrowserWhitelistRemove->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + m_pGridBrowserWhitelist->GetSize().fY + 5.0f));
    m_pButtonBrowserWhitelistRemove->SetSize(CVector2D(140.0f, 22.0f));

    /**
     *  Advanced tab
     **/
    vecTemp = CVector2D(12.f, 12.f);
    float fComboWidth = 170.f;
    float fHeaderHeight = 20;
    float fLineHeight = 27;

    // Misc section label
    m_pAdvancedMiscLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Misc")));
    m_pAdvancedMiscLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pAdvancedMiscLabel->SetFont("default-bold-small");
    m_pAdvancedMiscLabel->AutoSize();
    vecTemp.fY += fHeaderHeight;

    fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", _("Fast CJ clothes loading:"), _("Browser speed:"), _("Single connection:"), _("Packet tag:"),
                                               _("Progress animation:"), _("Fullscreen mode:"), _("Process priority:"), _("Debug setting:"),
                                               _("Streaming memory:"), _("Update build type:"), _("Install important updates:")) +
               5.0f;

    vecTemp.fX += 10.0f;
    // Fast clothes loading
    m_pFastClothesLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Fast CJ clothes loading:")));
    m_pFastClothesLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pFastClothesLabel->AutoSize();

    m_pFastClothesCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pFastClothesCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pFastClothesCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pFastClothesCombo->AddItem(_("Off"))->SetData((void*)CMultiplayer::FAST_CLOTHES_OFF);
    m_pFastClothesCombo->AddItem(_("On"))->SetData((void*)CMultiplayer::FAST_CLOTHES_ON);
    m_pFastClothesCombo->AddItem(_("Auto"))->SetData((void*)CMultiplayer::FAST_CLOTHES_AUTO);
    m_pFastClothesCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Browser scan speed
    m_pBrowserSpeedLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Browser speed:")));
    m_pBrowserSpeedLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pBrowserSpeedLabel->AutoSize();

    m_pBrowserSpeedCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pBrowserSpeedCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pBrowserSpeedCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pBrowserSpeedCombo->AddItem(_("Very slow"))->SetData((void*)0);
    m_pBrowserSpeedCombo->AddItem(_("Default"))->SetData((void*)1);
    m_pBrowserSpeedCombo->AddItem(_("Fast"))->SetData((void*)2);
    m_pBrowserSpeedCombo->SetReadOnly(true);
    vecTemp.fY += 29;

    // Single download
    m_pSingleDownloadLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Single connection:")));
    m_pSingleDownloadLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pSingleDownloadLabel->AutoSize();

    m_pSingleDownloadCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pSingleDownloadCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pSingleDownloadCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pSingleDownloadCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pSingleDownloadCombo->AddItem(_("On"))->SetData((void*)1);
    m_pSingleDownloadCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Packet tag
    m_pPacketTagLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Packet tag:")));
    m_pPacketTagLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pPacketTagLabel->AutoSize();

    m_pPacketTagCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pPacketTagCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pPacketTagCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pPacketTagCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pPacketTagCombo->AddItem(_("On"))->SetData((void*)1);
    m_pPacketTagCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Progress animation
    m_pProgressAnimationLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Progress animation:")));
    m_pProgressAnimationLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pProgressAnimationLabel->AutoSize();

    m_pProgressAnimationCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pProgressAnimationCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pProgressAnimationCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pProgressAnimationCombo->AddItem(_("Off"))->SetData((void*)0);
    m_pProgressAnimationCombo->AddItem(_("Default"))->SetData((void*)1);
    m_pProgressAnimationCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Process priority
    m_pPriorityLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Process priority:")));
    m_pPriorityLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pPriorityLabel->AutoSize();

    m_pPriorityCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pPriorityCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pPriorityCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pPriorityCombo->AddItem(_("Normal"))->SetData((void*)0);
    m_pPriorityCombo->AddItem(_("Above normal"))->SetData((void*)1);
    m_pPriorityCombo->AddItem(_("High"))->SetData((void*)2);
    m_pPriorityCombo->SetReadOnly(true);
    vecTemp.fY += 29;

    // Debug setting
    m_pDebugSettingLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Debug setting:")));
    m_pDebugSettingLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pDebugSettingLabel->AutoSize();

    m_pDebugSettingCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pDebugSettingCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pDebugSettingCombo->SetSize(CVector2D(fComboWidth, 20.0f * (EDiagnosticDebug::MAX + 1)));
    m_pDebugSettingCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pDebugSettingCombo->AddItem("#6734 Graphics")->SetData((void*)EDiagnosticDebug::GRAPHICS_6734);
    // m_pDebugSettingCombo->AddItem ( "#6778 BIDI" )->SetData ( (void*)EDiagnosticDebug::BIDI_6778 );
    m_pDebugSettingCombo->AddItem("#6732 D3D")->SetData((void*)EDiagnosticDebug::D3D_6732);
    m_pDebugSettingCombo->AddItem("#0000 Log timing")->SetData((void*)EDiagnosticDebug::LOG_TIMING_0000);
    m_pDebugSettingCombo->AddItem("#0000 Joystick")->SetData((void*)EDiagnosticDebug::JOYSTICK_0000);
    m_pDebugSettingCombo->AddItem("#0000 Lua trace")->SetData((void*)EDiagnosticDebug::LUA_TRACE_0000);
    m_pDebugSettingCombo->AddItem("#0000 Resize always")->SetData((void*)EDiagnosticDebug::RESIZE_ALWAYS_0000);
    m_pDebugSettingCombo->AddItem("#0000 Resize never")->SetData((void*)EDiagnosticDebug::RESIZE_NEVER_0000);
    m_pDebugSettingCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    m_pDebugSettingCombo->SetText(_("Default"));
    SetApplicationSetting("diagnostics", "debug-setting", "none");

    // Streaming memory
    m_pStreamingMemoryLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Streaming memory:")));
    m_pStreamingMemoryLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 0.0f));
    m_pStreamingMemoryLabel->AutoSize();

    unsigned int uiMinMemory = g_pCore->GetMinStreamingMemory();
    unsigned int uiMaxMemory = g_pCore->GetMaxStreamingMemory();

    m_pStreamingMemoryMinLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Min")));
    m_pStreamingMemoryMinLabel->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY));
    m_pStreamingMemoryMinLabel->AutoSize();
    m_pStreamingMemoryMinLabel->GetSize(vecSize);
    m_pStreamingMemoryMinLabel->GetPosition(vecTemp);

    m_pStreamingMemory = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTabAdvanced));
    m_pStreamingMemory->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pStreamingMemory->GetPosition(vecTemp);
    m_pStreamingMemory->SetSize(CVector2D(130.0f, 20.0f));
    m_pStreamingMemory->GetSize(vecSize);
    m_pStreamingMemory->SetProperty("StepSize", SString("%.07lf", 1.0 / (uiMaxMemory - uiMinMemory)));

    m_pStreamingMemoryMaxLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Max")));
    m_pStreamingMemoryMaxLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pStreamingMemoryMaxLabel->AutoSize();
    vecTemp.fX = 22.f;
    vecTemp.fY += fLineHeight;

    // Windows 8 compatibility
    m_pWin8Label = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Windows 8 compatibility:")));
    m_pWin8Label->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pWin8Label->AutoSize();

    m_pWin8ColorCheckBox = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAdvanced, _("16-bit color")));
    m_pWin8ColorCheckBox->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY));
    m_pWin8ColorCheckBox->AutoSize(NULL, 20.0f);
    vecTemp.fX += 140;

    m_pWin8MouseCheckBox = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabAdvanced, _("Mouse fix")));
    m_pWin8MouseCheckBox->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY));
    m_pWin8MouseCheckBox->AutoSize(NULL, 20.0f);
    vecTemp.fY += fLineHeight;
    vecTemp.fX -= 140;

    // Hide if not Win8
    if (atoi(GetApplicationSetting("real-os-version")) != 8)
    {
#ifndef MTA_DEBUG            // Don't hide when debugging
        m_pWin8Label->SetVisible(false);
        m_pWin8ColorCheckBox->SetVisible(false);
        m_pWin8MouseCheckBox->SetVisible(false);
        vecTemp.fY -= fLineHeight;
#endif
    }

    // Cache path info
    m_pCachePathLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Client resource files:")));
    m_pCachePathLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pCachePathLabel->AutoSize();

    m_pCachePathShowButton = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabAdvanced, _("Show in Explorer")));
    m_pCachePathShowButton->SetPosition(CVector2D(vecTemp.fX + fIndentX + 1, vecTemp.fY - 1));
    m_pCachePathShowButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pCachePathShowButton->SetClickHandler(GUI_CALLBACK(&CSettings::OnCachePathShowButtonClick, this));
    m_pCachePathShowButton->SetZOrderingEnabled(false);
    m_pCachePathShowButton->GetSize(vecSize);

    SString strFileCachePath = GetCommonRegistryValue("", "File Cache Path");
    m_pCachePathValue = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, strFileCachePath));
    m_pCachePathValue->SetPosition(CVector2D(vecTemp.fX + fIndentX + vecSize.fX + 10, vecTemp.fY + 3));
    m_pCachePathValue->SetFont("default-small");
    m_pCachePathValue->AutoSize();
    vecTemp.fY += fLineHeight;

    // Auto updater section label
    m_pAdvancedUpdaterLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Auto updater")));
    m_pAdvancedUpdaterLabel->SetPosition(CVector2D(vecTemp.fX - 10.0f, vecTemp.fY));
    m_pAdvancedUpdaterLabel->SetFont("default-bold-small");
    m_pAdvancedUpdaterLabel->AutoSize(_("Auto updater"));
    vecTemp.fY += fHeaderHeight;

    // UpdateAutoInstall
    m_pUpdateAutoInstallLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Install important updates:")));
    m_pUpdateAutoInstallLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pUpdateAutoInstallLabel->AutoSize();

    m_pUpdateAutoInstallCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pUpdateAutoInstallCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pUpdateAutoInstallCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pUpdateAutoInstallCombo->AddItem(_("Off"))->SetData((void*)0);
    m_pUpdateAutoInstallCombo->AddItem(_("Default"))->SetData((void*)1);
    m_pUpdateAutoInstallCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    if (true)
    {
        // Always hide for now
        m_pUpdateAutoInstallLabel->SetVisible(false);
        m_pUpdateAutoInstallCombo->SetVisible(false);
        vecTemp.fY -= fLineHeight;
    }

    // Update build type
    m_pUpdateBuildTypeLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, _("Update build type:")));
    m_pUpdateBuildTypeLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pUpdateBuildTypeLabel->AutoSize();

    m_pUpdateBuildTypeCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabAdvanced, ""));
    m_pUpdateBuildTypeCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pUpdateBuildTypeCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pUpdateBuildTypeCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pUpdateBuildTypeCombo->AddItem("Nightly")->SetData((void*)2);
    m_pUpdateBuildTypeCombo->SetReadOnly(true);
    vecTemp.fX += fComboWidth + 15;

    // Check for updates
    m_pButtonUpdate = reinterpret_cast<CGUIButton*>(pManager->CreateButton(pTabAdvanced, _("Check for update now")));
    m_pButtonUpdate->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY));
    m_pButtonUpdate->AutoSize(NULL, 20.0f, 8.0f);
    m_pButtonUpdate->SetClickHandler(GUI_CALLBACK(&CSettings::OnUpdateButtonClick, this));
    m_pButtonUpdate->SetZOrderingEnabled(false);
    vecTemp.fY += fLineHeight;
    vecTemp.fX -= fComboWidth + 15;

    // Description label
    vecTemp.fY = 354 + 10;
    m_pAdvancedSettingDescriptionLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabAdvanced, ""));
    m_pAdvancedSettingDescriptionLabel->SetPosition(CVector2D(vecTemp.fX + 10.f, vecTemp.fY));
    m_pAdvancedSettingDescriptionLabel->SetFont("default-bold-small");
    m_pAdvancedSettingDescriptionLabel->SetSize(CVector2D(500.0f, 95.0f));
    m_pAdvancedSettingDescriptionLabel->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP);

    // Set up the events
    m_pWindow->SetEnterKeyHandler(GUI_CALLBACK(&CSettings::OnOKButtonClick, this));
    m_pButtonOK->SetClickHandler(GUI_CALLBACK(&CSettings::OnOKButtonClick, this));
    m_pButtonCancel->SetClickHandler(GUI_CALLBACK(&CSettings::OnCancelButtonClick, this));
    m_pChatLoadPreset->SetClickHandler(GUI_CALLBACK(&CSettings::OnChatLoadPresetClick, this));
    m_pInterfaceLanguageSelector->SetSelectionHandler(GUI_CALLBACK(&CSettings::OnLanguageChanged, this));
    m_pInterfaceSkinSelector->SetSelectionHandler(GUI_CALLBACK(&CSettings::OnSkinChanged, this));
    m_pMapAlpha->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnMapAlphaChanged, this));
    m_pAudioMasterVolume->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnMasterVolumeChanged, this));
    m_pAudioRadioVolume->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnRadioVolumeChanged, this));
    m_pAudioSFXVolume->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnSFXVolumeChanged, this));
    m_pAudioMTAVolume->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnMTAVolumeChanged, this));
    m_pAudioVoiceVolume->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnVoiceVolumeChanged, this));
    m_pCheckBoxMuteMaster->SetClickHandler(GUI_CALLBACK(&CSettings::OnMasterMuteMinimizedChanged, this));
    m_pCheckBoxMuteRadio->SetClickHandler(GUI_CALLBACK(&CSettings::OnRadioMuteMinimizedChanged, this));
    m_pCheckBoxMuteSFX->SetClickHandler(GUI_CALLBACK(&CSettings::OnSFXMuteMinimizedChanged, this));
    m_pCheckBoxMuteMTA->SetClickHandler(GUI_CALLBACK(&CSettings::OnMTAMuteMinimizedChanged, this));
    m_pCheckBoxMuteVoice->SetClickHandler(GUI_CALLBACK(&CSettings::OnVoiceMuteMinimizedChanged, this));
    m_pFieldOfView->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnFieldOfViewChanged, this));
    m_pDrawDistance->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnDrawDistanceChanged, this));
    m_pBrightness->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnBrightnessChanged, this));
    m_pAnisotropic->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnAnisotropicChanged, this));
    m_pMouseSensitivity->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnMouseSensitivityChanged, this));
    m_pVerticalAimSensitivity->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnVerticalAimSensitivityChanged, this));
    m_pComboFxQuality->SetSelectionHandler(GUI_CALLBACK(&CSettings::OnFxQualityChanged, this));
    m_pCheckBoxVolumetricShadows->SetClickHandler(GUI_CALLBACK(&CSettings::OnVolumetricShadowsClick, this));
    m_pCheckBoxAllowScreenUpload->SetClickHandler(GUI_CALLBACK(&CSettings::OnAllowScreenUploadClick, this));
    m_pCheckBoxAllowExternalSounds->SetClickHandler(GUI_CALLBACK(&CSettings::OnAllowExternalSoundsClick, this));
    m_pCheckBoxAllowDiscordRPC->SetClickHandler(GUI_CALLBACK(&CSettings::OnAllowDiscordRPC, this));
    m_pCheckBoxCustomizedSAFiles->SetClickHandler(GUI_CALLBACK(&CSettings::OnCustomizedSAFilesClick, this));
    m_pCheckBoxWindowed->SetClickHandler(GUI_CALLBACK(&CSettings::OnWindowedClick, this));
    m_pCheckBoxDPIAware->SetClickHandler(GUI_CALLBACK(&CSettings::OnDPIAwareClick, this));
    m_pCheckBoxShowUnsafeResolutions->SetClickHandler(GUI_CALLBACK(&CSettings::ShowUnsafeResolutionsClick, this));
    m_pButtonBrowserBlacklistAdd->SetClickHandler(GUI_CALLBACK(&CSettings::OnBrowserBlacklistAdd, this));
    m_pButtonBrowserBlacklistRemove->SetClickHandler(GUI_CALLBACK(&CSettings::OnBrowserBlacklistRemove, this));
    m_pEditBrowserBlacklistAdd->SetActivateHandler(GUI_CALLBACK(&CSettings::OnBrowserBlacklistDomainAddFocused, this));
    m_pEditBrowserBlacklistAdd->SetDeactivateHandler(GUI_CALLBACK(&CSettings::OnBrowserBlacklistDomainAddDefocused, this));
    m_pButtonBrowserWhitelistAdd->SetClickHandler(GUI_CALLBACK(&CSettings::OnBrowserWhitelistAdd, this));
    m_pButtonBrowserWhitelistRemove->SetClickHandler(GUI_CALLBACK(&CSettings::OnBrowserWhitelistRemove, this));
    m_pEditBrowserWhitelistAdd->SetActivateHandler(GUI_CALLBACK(&CSettings::OnBrowserWhitelistDomainAddFocused, this));
    m_pEditBrowserWhitelistAdd->SetDeactivateHandler(GUI_CALLBACK(&CSettings::OnBrowserWhitelistDomainAddDefocused, this));

    // Set up the events for advanced description
    m_pPriorityLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pPriorityLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pPriorityCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pPriorityCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pFastClothesLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pFastClothesLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pFastClothesCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pFastClothesCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pBrowserSpeedLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pBrowserSpeedLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pBrowserSpeedCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pBrowserSpeedCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pSingleDownloadLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pSingleDownloadLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pSingleDownloadCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pSingleDownloadCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pPacketTagLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pPacketTagLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pPacketTagCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pPacketTagCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pProgressAnimationLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pProgressAnimationLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pProgressAnimationCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pProgressAnimationCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pDebugSettingLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pDebugSettingLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pDebugSettingCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pDebugSettingCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pStreamingMemoryLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pStreamingMemoryLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pStreamingMemory->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pStreamingMemory->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pUpdateBuildTypeLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pUpdateBuildTypeLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pUpdateBuildTypeCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pUpdateBuildTypeCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pWin8ColorCheckBox->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pWin8ColorCheckBox->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pWin8MouseCheckBox->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pWin8MouseCheckBox->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pUpdateAutoInstallLabel->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pUpdateAutoInstallLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    m_pUpdateAutoInstallCombo->SetMouseEnterHandler(GUI_CALLBACK(&CSettings::OnShowAdvancedSettingDescription, this));
    m_pUpdateAutoInstallCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CSettings::OnHideAdvancedSettingDescription, this));

    // Load Chat presets
    LoadChatPresets();

    // Load the load of skins
    LoadSkins();
}

void CSettings::DestroyGUI()
{
    // Destroy
    delete m_pButtonCancel;
    delete m_pButtonOK;
    delete m_pWindow;
    m_pWindow = NULL;
}

void RestartCallBack(void* ptr, unsigned int uiButton)
{
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();

    if (uiButton == 1)
    {
        SetOnQuitCommand("restart");
        CCore::GetSingleton().Quit();
    }
}

void CSettings::ShowRestartQuestion()
{
    SString strMessage = _("Some settings will be changed when you next start MTA");
    strMessage += _("\n\nDo you want to restart now?");
    CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
    pQuestionBox->Reset();
    pQuestionBox->SetTitle(_("RESTART REQUIRED"));
    pQuestionBox->SetMessage(strMessage);
    pQuestionBox->SetButton(0, _("No"));
    pQuestionBox->SetButton(1, _("Yes"));
    pQuestionBox->SetCallback(RestartCallBack);
    pQuestionBox->Show();
}

void DisconnectCallback(void* ptr, unsigned int uiButton)
{
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();

    if (uiButton == 1)
    {
        CCommands::GetSingleton().Execute("disconnect", "");
    }
}

void CSettings::ShowDisconnectQuestion()
{
    SString strMessage = _("Some settings will be changed when you disconnect the current server");
    strMessage += _("\n\nDo you want to disconnect now?");
    CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
    pQuestionBox->Reset();
    pQuestionBox->SetTitle(_("DISCONNECT REQUIRED"));
    pQuestionBox->SetMessage(strMessage);
    pQuestionBox->SetButton(0, _("No"));
    pQuestionBox->SetButton(1, _("Yes"));
    pQuestionBox->SetCallback(DisconnectCallback);
    pQuestionBox->Show();
}

bool CSettings::OnMouseDoubleClick(CGUIMouseEventArgs Args)
{
    if (Args.pWindow == m_pBindsList)
    {
        OnBindsListClick(m_pBindsList);
        return true;
    }

    return false;
}

void CSettings::Update()
{
    // Once each 30 frames
    if (m_dwFrameCount >= CORE_SETTINGS_UPDATE_INTERVAL)
    {
        UpdateJoypadTab();

        m_dwFrameCount = 0;
    }
    m_dwFrameCount++;

    UpdateCaptureAxis();
}

void CSettings::UpdateAudioTab()
{
    float fMasterVolume = 0, fMTAVolume = 0, fVoiceVolume = 0;

    CVARS_GET("mastervolume", fMasterVolume);
    CVARS_GET("mtavolume", fMTAVolume);
    CVARS_GET("voicevolume", fVoiceVolume);

    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    m_pAudioRadioVolume->SetScrollPosition(m_fRadioVolume);
    m_pAudioSFXVolume->SetScrollPosition(m_fSFXVolume);
    m_pAudioMTAVolume->SetScrollPosition(fMTAVolume);
    m_pAudioVoiceVolume->SetScrollPosition(fVoiceVolume);
    // Lastly, set our master volume scroll position
    m_pAudioMasterVolume->SetScrollPosition(fMasterVolume);

    m_pCheckBoxAudioEqualizer->SetSelected(gameSettings->IsRadioEqualizerEnabled());
    m_pCheckBoxAudioAutotune->SetSelected(gameSettings->IsRadioAutotuneEnabled());
    m_pCheckBoxUserAutoscan->SetSelected(gameSettings->IsUsertrackAutoScan());

    CVARS_GET("mute_master_when_minimized", m_bMuteMaster);
    CVARS_GET("mute_radio_when_minimized", m_bMuteRadio);
    CVARS_GET("mute_sfx_when_minimized", m_bMuteSFX);
    CVARS_GET("mute_mta_when_minimized", m_bMuteMTA);
    CVARS_GET("mute_voice_when_minimized", m_bMuteVoice);

    m_pCheckBoxMuteMaster->SetSelected(m_bMuteMaster);
    m_pCheckBoxMuteRadio->SetSelected(m_bMuteRadio);
    m_pCheckBoxMuteSFX->SetSelected(m_bMuteSFX);
    m_pCheckBoxMuteMTA->SetSelected(m_bMuteMTA);
    m_pCheckBoxMuteVoice->SetSelected(m_bMuteVoice);

    m_pCheckBoxMuteRadio->SetEnabled(!m_bMuteMaster);
    m_pCheckBoxMuteSFX->SetEnabled(!m_bMuteMaster);
    m_pCheckBoxMuteMTA->SetEnabled(!m_bMuteMaster);
    m_pCheckBoxMuteVoice->SetEnabled(!m_bMuteMaster);

    m_pComboUsertrackMode->SetSelectedItemByIndex(gameSettings->GetUsertrackMode());
}

void CSettings::UpdateVideoTab()
{
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    bool bNextWindowed;
    bool bNextFSMinimize;
    int  iNextVidMode;
    int  iNextFullscreenStyle;
    GetVideoModeManager()->GetNextVideoMode(iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle);

    m_pCheckBoxMipMapping->SetSelected(gameSettings->IsMipMappingEnabled());
    m_pCheckBoxWindowed->SetSelected(bNextWindowed);
    m_pCheckBoxMinimize->SetSelected(bNextFSMinimize);
    m_pDrawDistance->SetScrollPosition((gameSettings->GetDrawDistance() - 0.925f) / 0.8749f);
    m_pBrightness->SetScrollPosition((float)gameSettings->GetBrightness() / 384);

    // DPI aware
    bool processDPIAware = false;
    CVARS_GET("process_dpi_aware", processDPIAware);
    m_pCheckBoxDPIAware->SetSelected(processDPIAware);

    // FieldOfView
    int iFieldOfView;
    CVARS_GET("fov", iFieldOfView);
    m_pFieldOfView->SetScrollPosition((iFieldOfView - 70) / 20.f);

    // Anisotropic filtering
    int iAnisotropic;
    CVARS_GET("anisotropic", iAnisotropic);
    m_pAnisotropic->SetScrollPosition(iAnisotropic / (float)m_iMaxAnisotropic);

    int FxQuality = gameSettings->GetFXQuality();
    if (FxQuality == 0)
        m_pComboFxQuality->SetText(_("Low"));
    else if (FxQuality == 1)
        m_pComboFxQuality->SetText(_("Medium"));
    else if (FxQuality == 2)
        m_pComboFxQuality->SetText(_("High"));
    else if (FxQuality == 3)
        m_pComboFxQuality->SetText(_("Very high"));

    char AntiAliasing = gameSettings->GetAntiAliasing();
    if (AntiAliasing == 1)
        m_pComboAntiAliasing->SetText(_("Off"));
    else if (AntiAliasing == 2)
        m_pComboAntiAliasing->SetText(_("1x"));
    else if (AntiAliasing == 3)
        m_pComboAntiAliasing->SetText(_("2x"));
    else if (AntiAliasing == 4)
        m_pComboAntiAliasing->SetText(_("3x"));

    // Aspect ratio
    int aspectRatio;
    CVARS_GET("aspect_ratio", aspectRatio);
    if (aspectRatio == ASPECT_RATIO_AUTO)
        m_pComboAspectRatio->SetText(_("Auto"));
    else if (aspectRatio == ASPECT_RATIO_4_3)
        m_pComboAspectRatio->SetText(_("4:3"));
    else if (aspectRatio == ASPECT_RATIO_16_10)
        m_pComboAspectRatio->SetText(_("16:10"));
    else if (aspectRatio == ASPECT_RATIO_16_9)
        m_pComboAspectRatio->SetText(_("16:9"));

    // HUD match aspect ratio
    m_pCheckBoxHudMatchAspectRatio->SetSelected(CVARS_GET_VALUE<bool>("hud_match_aspect_ratio"));

    // Volumetric shadows
    bool bVolumetricShadowsEnabled;
    CVARS_GET("volumetric_shadows", bVolumetricShadowsEnabled);
    m_pCheckBoxVolumetricShadows->SetSelected(bVolumetricShadowsEnabled);
    m_pCheckBoxVolumetricShadows->SetEnabled(FxQuality != 0);

    // Device selection dialog
    bool bDeviceSelectionDialogEnabled = GetApplicationSettingInt("device-selection-disabled") ? false : true;
    m_pCheckBoxDeviceSelectionDialog->SetSelected(bDeviceSelectionDialogEnabled);

    // Show unsafe resolutions
    bool bShowUnsafeResolutions;
    CVARS_GET("show_unsafe_resolutions", bShowUnsafeResolutions);
    m_pCheckBoxShowUnsafeResolutions->SetSelected(bShowUnsafeResolutions);

    // Grass
    bool bGrassEnabled;
    CVARS_GET("grass", bGrassEnabled);
    m_pCheckBoxGrass->SetSelected(bGrassEnabled);
    m_pCheckBoxGrass->SetEnabled(FxQuality != 0);

    // Heat haze
    bool bHeatHazeEnabled;
    CVARS_GET("heat_haze", bHeatHazeEnabled);
    m_pCheckBoxHeatHaze->SetSelected(bHeatHazeEnabled);

    // Tyre smoke
    bool bTyreSmokeEnabled;
    CVARS_GET("tyre_smoke_enabled", bTyreSmokeEnabled);
    m_pCheckBoxTyreSmokeParticles->SetSelected(bTyreSmokeEnabled);

    // High detail vehicles
    bool bHighDetailVehicles;
    CVARS_GET("high_detail_vehicles", bHighDetailVehicles);
    m_pCheckBoxHighDetailVehicles->SetSelected(bHighDetailVehicles);

    // High detail peds
    bool bHighDetailPeds;
    CVARS_GET("high_detail_peds", bHighDetailPeds);
    m_pCheckBoxHighDetailPeds->SetSelected(bHighDetailPeds);

    // Blur
    bool bBlur;
    CVARS_GET("blur", bBlur);
    m_pCheckBoxBlur->SetSelected(bBlur);

    // Corona rain reflections
    bool bCoronaReflections;
    CVARS_GET("corona_reflections", bCoronaReflections);
    m_pCheckBoxCoronaReflections->SetSelected(bCoronaReflections);

    // Dynamic ped shadows
    bool bDynamicPedShadows;
    CVARS_GET("dynamic_ped_shadows", bDynamicPedShadows);
    m_pCheckBoxDynamicPedShadows->SetSelected(bDynamicPedShadows);

    // Enable dynamic ped shadows checkbox if visual quality option is set to high or very high
    m_pCheckBoxDynamicPedShadows->SetEnabled(FxQuality >= 2);

    PopulateResolutionComboBox();

    // Fullscreen style
    if (iNextFullscreenStyle == FULLSCREEN_STANDARD)
        m_pFullscreenStyleCombo->SetText(_("Standard"));
    else if (iNextFullscreenStyle == FULLSCREEN_BORDERLESS)
        m_pFullscreenStyleCombo->SetText(_("Borderless window"));
    else if (iNextFullscreenStyle == FULLSCREEN_BORDERLESS_KEEP_RES)
        m_pFullscreenStyleCombo->SetText(_("Borderless keep res"));

    UpdateFullScreenComboBoxEnabled();

    // Streaming memory
    unsigned int uiStreamingMemory = 0;
    CVARS_GET("streaming_memory", uiStreamingMemory);
    uiStreamingMemory = SharedUtil::Clamp(g_pCore->GetMinStreamingMemory(), uiStreamingMemory, g_pCore->GetMaxStreamingMemory());
    float fPos = SharedUtil::Unlerp(g_pCore->GetMinStreamingMemory(), uiStreamingMemory, g_pCore->GetMaxStreamingMemory());
    m_pStreamingMemory->SetScrollPosition(fPos);

    int iVar = 0;
    CVARS_GET("mapalpha", iVar);
    int iAlphaPercent = ceil(((float)Clamp(0, iVar, 255) / 255) * 100);
    m_pMapAlphaValueLabel->SetText(SString("%i%%", iAlphaPercent).c_str());
    float sbPos = (float)iAlphaPercent / 100.0f;
    m_pMapAlpha->SetScrollPosition(sbPos);
}

//
// PopulateResolutionComboBox
//
void CSettings::PopulateResolutionComboBox()
{
    bool bNextWindowed;
    bool bNextFSMinimize;
    int  iNextVidMode;
    int  iNextFullscreenStyle;
    GetVideoModeManager()->GetNextVideoMode(iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle);
    bool bShowUnsafeResolutions = m_pCheckBoxShowUnsafeResolutions->GetSelected();

    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    VideoMode vidModemInfo;
    int       vidMode, numVidModes;

    m_pComboResolution->Clear();
    numVidModes = gameSettings->GetNumVideoModes();

    for (vidMode = 0; vidMode < numVidModes; vidMode++)
    {
        gameSettings->GetVideoModeInfo(&vidModemInfo, vidMode);

        // Remove resolutions that will make the gui unusable
        if (vidModemInfo.width < 640 || vidModemInfo.height < 480)
            continue;

        // Check resolution hasn't already been added
        bool bDuplicate = false;
        for (int i = 1; i < vidMode; i++)
        {
            VideoMode info;
            gameSettings->GetVideoModeInfo(&info, i);
            if (info.width == vidModemInfo.width && info.height == vidModemInfo.height && info.depth == vidModemInfo.depth)
                bDuplicate = true;
        }
        if (bDuplicate)
            continue;

        // Check resolution is below desktop res unless that is allowed
        if (gameSettings->IsUnsafeResolution(vidModemInfo.width, vidModemInfo.height) && !bShowUnsafeResolutions)
            continue;

        SString strMode("%lu x %lu x %lu", vidModemInfo.width, vidModemInfo.height, vidModemInfo.depth);

        if (vidModemInfo.flags & rwVIDEOMODEEXCLUSIVE)
            m_pComboResolution->AddItem(strMode)->SetData((void*)vidMode);

        VideoMode currentInfo;
        gameSettings->GetVideoModeInfo(&currentInfo, iNextVidMode);

        if (currentInfo.width == vidModemInfo.width && currentInfo.height == vidModemInfo.height && currentInfo.depth == vidModemInfo.depth)
            m_pComboResolution->SetText(strMode);
    }
}

//
// Disable/enable the full screen mode options
//
void CSettings::UpdateFullScreenComboBoxEnabled()
{
    if (m_pCheckBoxWindowed->GetSelected())
    {
        m_pFullscreenStyleLabel->SetAlpha(0.42f);
        m_pFullscreenStyleCombo->SetAlpha(0.42f);
        m_pFullscreenStyleCombo->SetEnabled(false);
    }
    else
    {
        m_pFullscreenStyleLabel->SetAlpha(1);
        m_pFullscreenStyleCombo->SetAlpha(1);
        m_pFullscreenStyleCombo->SetEnabled(true);
    }
}

//
// Saves the Joypad settings
//
void CSettings::ProcessJoypad()
{
    // Update from GUI
    GetJoystickManager()->SetDeadZone(atoi(m_pEditDeadzone->GetText().c_str()));
    GetJoystickManager()->SetSaturation(atoi(m_pEditSaturation->GetText().c_str()));

    GetJoystickManager()->SaveToXML();
}

//
// Update GUI elements for the Joypad Tab
//

void CSettings::UpdateJoypadTab()
{
    CJoystickManagerInterface* JoyMan = GetJoystickManager();

    // Has anything changed?
    if (m_JoypadSettingsRevision == JoyMan->GetSettingsRevision())
        return;

    // Update the joystick name
    string strJoystickName = JoyMan->IsJoypadConnected() ? JoyMan->GetControllerName() : _("Joypad not detected  -  Check connections and restart game");

    m_pJoypadName->SetPosition(CVector2D(270, m_pJoypadName->GetPosition().fY));
    m_pJoypadName->SetText(strJoystickName.c_str());
    m_pJoypadName->AutoSize(strJoystickName.c_str());
    m_pJoypadName->SetPosition(m_pJoypadName->GetPosition() - CVector2D(m_pJoypadName->GetSize().fX * 0.5, 0.0f));

    // Joystick name underline
    string strUnderline = "";
    int    inumChars = m_pJoypadName->GetSize().fX / 7.f + 0.5f;
    for (int i = 0; i < inumChars; i++)
        strUnderline = strUnderline + "_";

    m_pJoypadUnderline->SetPosition(CVector2D(270, m_pJoypadUnderline->GetPosition().fY));
    m_pJoypadUnderline->SetText(strUnderline.c_str());
    m_pJoypadUnderline->AutoSize(strUnderline.c_str());
    m_pJoypadUnderline->SetPosition(m_pJoypadUnderline->GetPosition() - CVector2D(m_pJoypadUnderline->GetSize().fX * 0.5, 0.0f));
    m_pJoypadUnderline->SetVisible(JoyMan->IsJoypadConnected());

    // Update DeadZone and Saturation edit boxes
    char szDeadzone[32] = "";
    char szSaturation[32] = "";
    snprintf(szDeadzone, 10, "%d", JoyMan->GetDeadZone());
    snprintf(szSaturation, 10, "%d", JoyMan->GetSaturation());

    m_pEditDeadzone->SetText(szDeadzone);
    m_pEditSaturation->SetText(szSaturation);

    // Update axes labels and buttons
    for (int i = 0; i < JoyMan->GetOutputCount() && i < (int)m_pJoypadButtons.size(); i++)
    {
        string outputName = JoyMan->GetOutputName(i);                // LeftStickPosX etc
        string inputName = JoyMan->GetOutputInputName(i);            // X+ or RZ- etc

        CGUILabel* pLabel = m_pJoypadLabels[i];
        pLabel->SetText(outputName.c_str());

        CGUIButton* pButton = m_pJoypadButtons[i];
        pButton->SetText(inputName.c_str());
    }

    m_JoypadSettingsRevision = JoyMan->GetSettingsRevision();
}

//
// React to the DeadZone and Saturation edit boxes changing
//
bool CSettings::OnJoypadTextChanged(CGUIElement* pElement)
{
    // Update from GUI
    GetJoystickManager()->SetDeadZone(atoi(m_pEditDeadzone->GetText().c_str()));
    GetJoystickManager()->SetSaturation(atoi(m_pEditSaturation->GetText().c_str()));

    // Dont immediately read back these settings
    m_JoypadSettingsRevision = GetJoystickManager()->GetSettingsRevision();

    return true;
}

//
// Called every frame. If capturing an axis, see if its all done yet.
//
void CSettings::UpdateCaptureAxis()
{
    if (m_bCaptureAxis)
    {
        // Are we done?
        if (GetJoystickManager()->IsAxisBindComplete())
        {
            // Remove the messagebox we created earlier
            CCore::GetSingleton().RemoveMessageBox();

            // Update GUI elements
            UpdateJoypadTab();

            m_bCaptureAxis = false;
        }
    }
}

//
// Called when the user clicks on the video 'Load Defaults' button.
//
bool CSettings::OnVideoDefaultClick(CGUIElement* pElement)
{
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    CVARS_SET("aspect_ratio", ASPECT_RATIO_AUTO);
    CVARS_SET("fov", 70);
    CVARS_SET("anisotropic", 0);
    CVARS_SET("volumetric_shadows", false);
    CVARS_SET("grass", true);
    CVARS_SET("heat_haze", true);
    CVARS_SET("tyre_smoke_enabled", true);
    CVARS_SET("high_detail_vehicles", false);
    CVARS_SET("high_detail_peds", false);
    CVARS_SET("blur", true);
    CVARS_SET("corona_reflections", false);
    CVARS_SET("dynamic_ped_shadows", false);
    gameSettings->UpdateFieldOfViewFromSettings();
    gameSettings->SetDrawDistance(1.19625f);            // All values taken from a default SA install, no gta_sa.set or coreconfig.xml modifications.
    gameSettings->SetBrightness(253);
    gameSettings->SetFXQuality(2);
    gameSettings->SetAntiAliasing(1, true);
    gameSettings->ResetVehiclesLODDistance(false);
    gameSettings->ResetPedsLODDistance(false);
    gameSettings->SetDynamicPedShadowsEnabled(false);

    // change
    bool bIsVideoModeChanged = GetVideoModeManager()->SetVideoMode(0, false, false, FULLSCREEN_STANDARD);

    CVARS_SET("streaming_memory", g_pCore->GetMaxStreamingMemory());

    CVARS_SET("mapalpha", 155);

    // Display restart required message if required
    bool bIsAntiAliasingChanged = gameSettings->GetAntiAliasing() != m_pComboAntiAliasing->GetSelectedItemIndex();
    if (bIsVideoModeChanged || bIsAntiAliasingChanged)
        ShowRestartQuestion();

    // Update the GUI
    UpdateVideoTab();

    return true;
}

void CSettings::ResetGTAVolume()
{
    // This will set the GTA volume to the GTA volume value in the settings,
    // and is not affected by the master volume setting.
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    gameSettings->SetRadioVolume(m_fRadioVolume * 64.0f);
    gameSettings->SetSFXVolume(m_fSFXVolume * 64.0f);
    gameSettings->Save();
}

void CSettings::SetRadioVolume(float fVolume)
{
    fVolume = std::max(0.0f, std::min(fVolume, 1.0f));

    m_fRadioVolume = fVolume;

    CCore::GetSingleton().GetGame()->GetSettings()->SetRadioVolume(m_fRadioVolume * CVARS_GET_VALUE<float>("mastervolume") * 64.0f);
}

void CSettings::SetSFXVolume(float fVolume)
{
    fVolume = std::max(0.0f, std::min(fVolume, 1.0f));

    m_fSFXVolume = fVolume;

    CCore::GetSingleton().GetGame()->GetSettings()->SetSFXVolume(m_fSFXVolume * CVARS_GET_VALUE<float>("mastervolume") * 64.0f);
}

//
// Called when the user clicks on the audio 'Load Defaults' button.
//
bool CSettings::OnAudioDefaultClick(CGUIElement* pElement)
{
    CVARS_SET("mastervolume", 1.0f);
    SetRadioVolume(1.0f);
    SetSFXVolume(1.0f);
    CVARS_SET("mtavolume", 1.0f);
    CVARS_SET("voicevolume", 1.0f);

    CVARS_SET("mute_master_when_minimized", false);
    CVARS_SET("mute_sfx_when_minimized", false);
    CVARS_SET("mute_radio_when_minimized", false);
    CVARS_SET("mute_mta_when_minimized", false);
    CVARS_SET("mute_voice_when_minimized", false);

    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    gameSettings->SetRadioAutotuneEnabled(true);
    gameSettings->SetRadioEqualizerEnabled(true);

    gameSettings->SetUsertrackAutoScan(false);
    gameSettings->SetUsertrackMode(0);

    // Update the GUI
    UpdateAudioTab();

    return true;
}

//
// Called when the user clicks on an map axis button. Starts the capture axis process.
//
bool CSettings::OnAxisSelectClick(CGUIElement* pElement)
{
    int index = reinterpret_cast<int>(pElement->GetUserData());

    if (GetJoystickManager()->BindNextUsedAxisToOutput(index))
    {
        m_bCaptureAxis = true;
        CCore::GetSingleton().ShowMessageBox(_("Binding axis"), _("Move an axis to bind, or escape to clear"), MB_ICON_QUESTION);
    }

    return true;
}

//
// Called when the user clicks on the controls tab 'Load Defaults' button.
//
bool CSettings::OnControlsDefaultClick(CGUIElement* pElement)
{
    CGameSettings*            gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    CControllerConfigManager* pController = g_pCore->GetGame()->GetControllerConfigManager();

    // Load the default settings
    GetJoystickManager()->SetDefaults();
    CVARS_SET("invert_mouse", false);
    CVARS_SET("fly_with_mouse", false);
    CVARS_SET("steer_with_mouse", false);
    CVARS_SET("classic_controls", false);
    pController->SetVerticalAimSensitivity(0.5f);
    CVARS_SET("vertical_aim_sensitivity", pController->GetVerticalAimSensitivityRawValue());
    gameSettings->SetMouseSensitivity(0.5f);

    // Set game vars
    pController->SetMouseInverted(CVARS_GET_VALUE<bool>("invert_mouse"));
    pController->SetFlyWithMouse(CVARS_GET_VALUE<bool>("fly_with_mouse"));
    pController->SetSteerWithMouse(CVARS_GET_VALUE<bool>("steer_with_mouse"));
    pController->SetClassicControls(CVARS_GET_VALUE<bool>("classic_controls"));
    pController->SetVerticalAimSensitivityRawValue(CVARS_GET_VALUE<float>("vertical_aim_sensitivity"));

    // Update the GUI
    UpdateJoypadTab();
    m_pInvertMouse->SetSelected(CVARS_GET_VALUE<bool>("invert_mouse"));
    m_pFlyWithMouse->SetSelected(CVARS_GET_VALUE<bool>("fly_with_mouse"));
    m_pSteerWithMouse->SetSelected(CVARS_GET_VALUE<bool>("steer_with_mouse"));
    m_pStandardControls->SetSelected(!CVARS_GET_VALUE<bool>("classic_controls"));
    m_pClassicControls->SetSelected(CVARS_GET_VALUE<bool>("classic_controls"));
    m_pMouseSensitivity->SetScrollPosition(gameSettings->GetMouseSensitivity());
    m_pVerticalAimSensitivity->SetScrollPosition(pController->GetVerticalAimSensitivity());

    return true;
}

//
// Called when the user clicks on the bind 'Load Defaults' button.
//
bool CSettings::OnBindsDefaultClick(CGUIElement* pElement)
{
    // Load the default binds
    CCore::GetSingleton().GetKeyBinds()->LoadDefaultBinds();

    // Clear the binds list
    m_pBindsList->Clear();

    // Re-initialize the binds list
    Initialize();

    return true;
}

void CSettings::CreateInterfaceTabGUI()
{
    if (!m_pTabInterface)
    {
        return;
    }

    CGUILabel* pLabel;
    CVector2D  vecTemp;
    CGUI*      pManager = g_pCore->GetGUI();

    CVector2D vecSize;
    m_pTabInterface->GetParent()->GetSize(vecSize);
    vecSize.fX -= 20.0f;
    vecSize.fY -= 20.0f;

    SString strLanguage = _("Language:"), strSkin = _("Skin:"), strPresets = _("Presets:");
    float   fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", strLanguage, strSkin, strPresets) + 10.0f;

    float fComboWidth = std::min(250.0f, vecSize.fX - (10.0f + fIndentX + 20.0f));

    //
    // General
    //
    {
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabInterface, _("General")));
        pLabel->SetPosition(CVector2D(10.0f, 5.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize(NULL, 5.0f);
        pLabel->SetFont("default-bold-small");

        // Language
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabInterface, strLanguage));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();

        m_pInterfaceLanguageSelector = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(m_pTabInterface, "English"));
        m_pInterfaceLanguageSelector->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
        m_pInterfaceLanguageSelector->SetSize(CVector2D(fComboWidth, 200.0f));
        m_pInterfaceLanguageSelector->SetReadOnly(true);

        // Grab languages and populate
        for (const auto& strLocale : g_pCore->GetLocalization()->GetAvailableLocales())
        {
            SString strLanguageName = g_pLocalization->GetLanguageNativeName(strLocale);
            m_pInterfaceLanguageSelector->AddItem(strLanguageName)->SetData(strLocale);
        }

        // Skin
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabInterface, strSkin));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 25.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();

        m_pInterfaceSkinSelector = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(m_pTabInterface));
        m_pInterfaceSkinSelector->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
        m_pInterfaceSkinSelector->SetSize(CVector2D(fComboWidth, 200.0f));
        m_pInterfaceSkinSelector->SetReadOnly(true);
    }

    //
    // Chat
    //
    {
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabInterface, _("Chat")));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();
        pLabel->SetFont("default-bold-small");

        // Presets
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pTabInterface, strPresets));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();

        m_pChatPresets = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(m_pTabInterface));
        m_pChatPresets->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
        m_pChatPresets->SetSize(CVector2D(fComboWidth, 200.0f));
        m_pChatPresets->SetReadOnly(true);

        m_pChatLoadPreset = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pTabInterface, _("Load")));
        m_pChatLoadPreset->SetPosition(CVector2D(vecTemp.fX + fIndentX + fComboWidth + 10.0f, vecTemp.fY - 2.0f));
        m_pChatLoadPreset->SetSize(CVector2D(100.0f, 24.0f));
        m_pChatLoadPreset->SetZOrderingEnabled(false);
    }

    // Create section tabpanel
    auto pTabPanel = reinterpret_cast<CGUITabPanel*>(pManager->CreateTabPanel(m_pTabInterface));
    pTabPanel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 40.0f));
    pTabPanel->SetSize(CVector2D(vecSize.fX, vecSize.fY - (vecTemp.fY + 55.0f)));
    pTabPanel->GetSize(vecSize);

    auto pTabColors = pTabPanel->CreateTab(_("Colors"));
    auto pTabLayout = pTabPanel->CreateTab(_("Layout"));
    auto pTabOptions = pTabPanel->CreateTab(_("Options"));

    //
    // Colors
    //
    {
        SString strChatBG = _("Chat Background"), strChatText = _("Chat Text"), strInputBG = _("Input Background"), strInputText = _("Input Text");

        CGUITabPanel* pColorTabPanel = reinterpret_cast<CGUITabPanel*>(pManager->CreateTabPanel(pTabColors));
        pColorTabPanel->SetPosition(CVector2D(10.0f, 10.0f));
        pColorTabPanel->SetSize(CVector2D(vecSize.fX - 20.0f, vecSize.fY - 45.0f));

        CreateChatColorTab(Chat::ColorType::BG, strChatBG, pColorTabPanel);
        CreateChatColorTab(Chat::ColorType::TEXT, strChatText, pColorTabPanel);
        CreateChatColorTab(Chat::ColorType::INPUT_BG, strInputBG, pColorTabPanel);
        CreateChatColorTab(Chat::ColorType::INPUT_TEXT, strInputText, pColorTabPanel);
    }

    //
    // Layout
    //
    {
        // Size of lines and gaps
        float fLineSizeY = 25;
        float fLineGapY = 12;
        float fMarginX = 0;

        // Size
        {
            SString strLines = _("Lines:"), strScale = _("Scale:"), strWidth = _("Width:");
            fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", strLines, strScale, strWidth) + 10.0f;

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, _("Size")));
            pLabel->SetPosition(CVector2D(10.0f, 10.0f));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize(NULL, 5.0f);
            pLabel->SetFont("default-bold-small");

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strLines));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatLines = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatLines->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 3.0f));
            m_pChatLines->SetSize(CVector2D(50.0f, fLineSizeY));

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strScale));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatScaleX = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatScaleX->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 3.0f));
            m_pChatScaleX->SetSize(CVector2D(50.0f, fLineSizeY));
            m_pChatScaleX->GetSize(vecSize);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, "x"));
            pLabel->SetPosition(CVector2D(vecTemp.fX + fIndentX + vecSize.fX + 3.0f, vecTemp.fY));
            pLabel->AutoSize();

            m_pChatScaleY = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatScaleY->SetPosition(CVector2D(vecTemp.fX + fIndentX + vecSize.fX + 10.0f, vecTemp.fY - 3.0f));
            m_pChatScaleY->SetSize(CVector2D(50.0f, fLineSizeY));

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strWidth));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatWidth = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatWidth->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 3.0f));
            m_pChatWidth->SetSize(CVector2D(110.0f, 24.0f));

            m_pChatWidth->GetPosition(vecTemp);
            m_pChatWidth->GetSize(vecSize);
            fMarginX = vecTemp.fX + vecSize.fX + 30.0f;
        }

        // Fading
        {
            SString strAfter = _("after"), strFor = _("for"), strSec = _("sec");
            fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", strAfter, strFor) + 10.0f;

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, _("Fading")));
            pLabel->SetPosition(CVector2D(fMarginX, 10.0f));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize(NULL, 5.0f);
            pLabel->SetFont("default-bold-small");

            m_pChatCssText = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabLayout, _("Fade out old lines")));
            m_pChatCssText->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
            m_pChatCssText->GetPosition(vecTemp);
            m_pChatCssText->AutoSize(NULL, 20.0f);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strAfter));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatLineLife = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatLineLife->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 3.0f));
            m_pChatLineLife->SetSize(CVector2D(50.0f, fLineSizeY));
            m_pChatLineLife->GetSize(vecSize);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strSec));
            pLabel->SetPosition(CVector2D(vecTemp.fX + fIndentX + vecSize.fX + 5.0f, vecTemp.fY));
            pLabel->AutoSize();

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strFor));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatLineFadeout = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatLineFadeout->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 3.0f));
            m_pChatLineFadeout->SetSize(CVector2D(50.0f, fLineSizeY));
            m_pChatLineFadeout->GetSize(vecSize);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strSec));
            pLabel->SetPosition(CVector2D(vecTemp.fX + fIndentX + vecSize.fX + 5.0f, vecTemp.fY));
            pLabel->AutoSize();

            m_pChatCssText->GetPosition(vecTemp);
            m_pChatCssText->GetSize(vecSize);
            fMarginX = vecTemp.fX + std::max(vecSize.fX, fIndentX + pManager->GetTextExtent(strSec) + 50.0f) + 30.0f;
        }

        // Position
        {
            SString strHorizontal = _("Horizontal:"), strVertical = _("Vertical:"), strTextAlign = _("Text-Align:"), strXOffset = _("X-Offset:"),
                    strYOffset = _("Y-Offset:");
            fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", strHorizontal, strVertical, strXOffset, strYOffset, strTextAlign) + 10.0f;

            pTabLayout->GetSize(vecSize);
            fComboWidth = std::min(150.0f, vecSize.fX - fMarginX - fIndentX - 10.0f);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, _("Position")));
            pLabel->SetPosition(CVector2D(fMarginX, 10.0f));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize(NULL, 5.0f);
            pLabel->SetFont("default-bold-small");

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strHorizontal));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatHorizontalCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabLayout, ""));
            m_pChatHorizontalCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
            m_pChatHorizontalCombo->SetSize(CVector2D(fComboWidth, 85.0f));
            m_pChatHorizontalCombo->AddItem(_("Left"))->SetData((void*)Chat::Position::Horizontal::LEFT);
            m_pChatHorizontalCombo->AddItem(_("Center"))->SetData((void*)Chat::Position::Horizontal::CENTER);
            m_pChatHorizontalCombo->AddItem(_("Right"))->SetData((void*)Chat::Position::Horizontal::RIGHT);
            m_pChatHorizontalCombo->SetReadOnly(true);
            m_pChatHorizontalCombo->SetSelectedItemByIndex(0);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strVertical));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatVerticalCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabLayout, ""));
            m_pChatVerticalCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
            m_pChatVerticalCombo->SetSize(CVector2D(fComboWidth, 85.0f));
            m_pChatVerticalCombo->AddItem(_("Top"))->SetData((void*)Chat::Position::Vertical::TOP);
            m_pChatVerticalCombo->AddItem(_("Center"))->SetData((void*)Chat::Position::Vertical::CENTER);
            m_pChatVerticalCombo->AddItem(_("Bottom"))->SetData((void*)Chat::Position::Vertical::BOTTOM);
            m_pChatVerticalCombo->SetReadOnly(true);
            m_pChatVerticalCombo->SetSelectedItemByIndex(0);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strTextAlign));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatTextAlignCombo = reinterpret_cast<CGUIComboBox*>(pManager->CreateComboBox(pTabLayout, ""));
            m_pChatTextAlignCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
            m_pChatTextAlignCombo->SetSize(CVector2D(fComboWidth, 85.0f));
            m_pChatTextAlignCombo->AddItem(_("Left"))->SetData((void*)Chat::Text::Align::LEFT);
            m_pChatTextAlignCombo->AddItem(_("Right"))->SetData((void*)Chat::Text::Align::RIGHT);
            m_pChatTextAlignCombo->SetReadOnly(true);
            m_pChatTextAlignCombo->SetSelectedItemByIndex(0);

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strXOffset));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatOffsetX = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatOffsetX->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 3.0f));
            m_pChatOffsetX->SetSize(CVector2D(fComboWidth, fLineSizeY));

            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabLayout, strYOffset));
            pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize();

            m_pChatOffsetY = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(pTabLayout, ""));
            m_pChatOffsetY->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 3.0f));
            m_pChatOffsetY->SetSize(CVector2D(fComboWidth, fLineSizeY));
        }
    }

    //
    // Options
    //
    {
        // Size of lines and gaps
        float fLineSizeY = 20;
        float fLineGapY = 5;
        float fMarginX = 0;

        // Fonts
        {
            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabOptions, _("Font")));
            pLabel->SetPosition(CVector2D(10.0f, 10.0f));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize(NULL, 5.0f);
            pLabel->SetFont("default-bold-small");

            vecTemp.fY += 30.0f - (fLineSizeY + fLineGapY);
            fIndentX = 0;

            std::map<Chat::Font::eFont, const char*> FontMap = {
                {Chat::Font::DEFAULT, "Tahoma"}, {Chat::Font::CLEAR, "Verdana"}, {Chat::Font::BOLD, "Tahoma Bold"}, {Chat::Font::ARIAL, "Arial"}};

            for (const auto& Font : FontMap)
            {
                float textExtent = pManager->GetTextExtent(Font.second) + 20.0f;
                fIndentX = std::max(fIndentX, textExtent);

                auto pRadioButton = reinterpret_cast<CGUIRadioButton*>(pManager->CreateRadioButton(pTabOptions, Font.second));
                pRadioButton->SetSelected(Font.first == Chat::Font::DEFAULT);
                pRadioButton->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
                pRadioButton->GetPosition(vecTemp);
                pRadioButton->AutoSize(NULL, 20.0f);

                m_pRadioChatFont[Font.first] = pRadioButton;
            }

            fMarginX = vecTemp.fX + fIndentX + 30.0f;
        }

        // Options
        {
            pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTabOptions, _("Options")));
            pLabel->SetPosition(CVector2D(fMarginX, 10.0f));
            pLabel->GetPosition(vecTemp);
            pLabel->AutoSize(NULL, 20.0f);
            pLabel->SetFont("default-bold-small");

            m_pChatCssBackground = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabOptions, _("Hide background when not typing")));
            m_pChatCssBackground->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
            m_pChatCssBackground->GetPosition(vecTemp);
            m_pChatCssBackground->AutoSize(NULL, 20.0f);

            m_pChatNickCompletion = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabOptions, _("Nickname completion using the \"Tab\" key")));
            m_pChatNickCompletion->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            m_pChatNickCompletion->GetPosition(vecTemp);
            m_pChatNickCompletion->AutoSize(NULL, 20.0f);

            m_pFlashWindow = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabOptions, _("Allow server to flash the window")));
            m_pFlashWindow->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            m_pFlashWindow->GetPosition(vecTemp);
            m_pFlashWindow->AutoSize(NULL, 20.0f);

            m_pTrayBalloon = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabOptions, _("Allow tray balloon notifications")));
            m_pTrayBalloon->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            m_pTrayBalloon->GetPosition(vecTemp);
            m_pTrayBalloon->AutoSize(NULL, 20.0f);

            m_pChatTextBlackOutline = reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(pTabOptions, _("Chat text black/white outline")));
            m_pChatTextBlackOutline->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
            m_pChatTextBlackOutline->GetPosition(vecTemp);
            m_pChatTextBlackOutline->AutoSize(NULL, 20.0f);
        }
    }
}

void CSettings::UpdateChatColorPreview(eChatColorType eType)
{
    if (!m_pChatColorPreview[eType])
    {
        return;
    }

    uchar iRed = 0xFF & static_cast<int>(std::floor(m_pChatRed[eType]->GetScrollPosition() * 255.0f + 0.5f));
    uchar iGreen = 0xFF & static_cast<int>(std::floor(m_pChatGreen[eType]->GetScrollPosition() * 255.0f + 0.5f));
    uchar iBlue = 0xFF & static_cast<int>(std::floor(m_pChatBlue[eType]->GetScrollPosition() * 255.0f + 0.5f));
    uchar iAlpha = 0xFF & static_cast<int>(std::floor(m_pChatAlpha[eType]->GetScrollPosition() * 255.0f + 0.5f));

    // AARRGGBB = 8
    char szARGB[9];
    sprintf_s(szARGB, "%02x%02x%02x%02x", iAlpha, iRed, iGreen, iBlue);

    // tl:AARRGGBB tr:AARRGGBB bl:AARRGGBB br:AARRGGBB = 48
    char szValue[49];
    sprintf_s(szValue, "tl:%s tr:%s bl:%s br:%s", szARGB, szARGB, szARGB, szARGB);

    m_pChatColorPreview[eType]->SetProperty("ImageColours", szValue);
}

// Saves the keybinds
void CSettings::ProcessKeyBinds()
{
    CKeyBindsInterface* pKeyBinds = CCore::GetSingleton().GetKeyBinds();

    SString strResource;

    // Loop through every row in the binds list
    for (int i = 0; i < m_pBindsList->GetRowCount(); i++)
    {
        // Get the type and keys
        auto                bindType = static_cast<KeyBindType>(reinterpret_cast<intptr_t>(m_pBindsList->GetItemData(i, m_hBind)));
        const char*         szPri = m_pBindsList->GetItemText(i, m_hPriKey);
        const SBindableKey* pPriKey = pKeyBinds->GetBindableFromKey(szPri);
        const SBindableKey* pSecKeys[SecKeyNum];
        for (int k = 0; k < SecKeyNum; k++)
        {
            const char* szSec = m_pBindsList->GetItemText(i, m_hSecKeys[k]);
            pSecKeys[k] = pKeyBinds->GetBindableFromKey(szSec);
        }
        // If it is a resource name
        if (static_cast<intptr_t>(bindType) == 255)
        {
            strResource = m_pBindsList->GetItemText(i, m_hBind);
        }
        // If the type is control
        else if (bindType == KeyBindType::GTA_CONTROL)
        {
            // Get the previous bind
            CGTAControlBind* pBind = reinterpret_cast<CGTAControlBind*>(m_pBindsList->GetItemData(i, m_hPriKey));
            if (pBind)
            {
                // If theres a primary key, change it
                if (pPriKey)
                    pBind->boundKey = pPriKey;
                else
                    // If not remove the bind
                    pKeyBinds->Remove(pBind);
            }

            for (int k = 0; k < SecKeyNum; k++)
            {
                CGTAControlBind* pSecBind = reinterpret_cast<CGTAControlBind*>(m_pBindsList->GetItemData(i, m_hSecKeys[k]));
                if (pSecBind)
                {
                    // If theres a secondary key, change it
                    if (pSecKeys[k])
                    {
                        if (pSecBind->boundKey != pSecKeys[k])
                        {
                            if (!pKeyBinds->GTAControlExists(pSecKeys[k], pSecBind->control))
                            {
                                pSecBind->boundKey = pSecKeys[k];
                            }
                            else
                            {
                                // If not remove the bind
                                pKeyBinds->Remove(pSecBind);
                            }
                        }
                    }
                    else
                    {
                        // If not remove the bind
                        pKeyBinds->Remove(pSecBind);
                    }
                }
                else if (pSecKeys[k] && pBind)
                {
                    SBindableGTAControl* pControl = pBind->control;
                    if (!pKeyBinds->GTAControlExists(pSecKeys[k], pControl))
                        pKeyBinds->AddGTAControl(pSecKeys[k], pControl);
                }
            }
        }
        // If the type is an empty control (wasn't bound before)
        else if (bindType == KeyBindType::UNDEFINED)
        {
            // Grab the stored control
            SBindableGTAControl* pControl = reinterpret_cast<SBindableGTAControl*>(m_pBindsList->GetItemData(i, m_hPriKey));
            if (pPriKey)
                // If theres a new key for primary, add a new bind
                pKeyBinds->AddGTAControl(pPriKey, pControl);
            for (int k = 0; k < SecKeyNum; k++)
                if (pSecKeys[k])
                    // If theres a new key for secondary, add a new bind
                    pKeyBinds->AddGTAControl(pSecKeys[k], pControl);
        }
        // If the type is a command
        else if (bindType == KeyBindType::COMMAND)
        {
            SString strCmdArgs = m_pBindsList->GetItemText(i, m_hBind);

            SString strCommand, strArguments;
            strCmdArgs.Split(": ", &strCommand, &strArguments);

            const char* szCommand = strCommand;
            const char* szArguments = strArguments.empty() ? NULL : strArguments.c_str();

            /** Primary keybinds **/
            CCommandBind* pBind = reinterpret_cast<CCommandBind*>(m_pBindsList->GetItemData(i, m_hPriKey));
            // If a keybind for this command already exists
            if (pBind)
            {
                // If the user specified a valid primary key
                if (pPriKey)
                {
                    // If the primary key is different than the original one
                    if (pPriKey != pBind->boundKey)
                    {
                        // Did we have any keys with the same "up" state?
                        CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                        if (pUpBind)
                        {
                            pKeyBinds->UserChangeCommandBoundKey(pUpBind, pPriKey);
                        }

                        pKeyBinds->UserChangeCommandBoundKey(pBind, pPriKey);
                    }
                }
                // If the primary key field was empty, we can remove the keybind
                else
                {
                    // Remove any matching "up" state binds we may have
                    CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                    if (pUpBind)
                    {
                        pKeyBinds->UserRemoveCommandBoundKey(pUpBind);
                    }
                    pKeyBinds->UserRemoveCommandBoundKey(pBind);
                }
            }
            // If there was no keybind for this command, create it
            else if (pPriKey)
            {
                if (strResource.empty())
                    pKeyBinds->AddCommand(pPriKey, szCommand, szArguments);
                else
                    pKeyBinds->AddCommand(pPriKey->szKey, szCommand, szArguments, true, strResource.c_str());
            }

            /** Secondary keybinds **/
            for (int k = 0; k < SecKeyNum; k++)
            {
                pBind = reinterpret_cast<CCommandBind*>(m_pBindsList->GetItemData(i, m_hSecKeys[k]));
                // If this is a valid bind in the keybinds list
                if (pBind)
                {
                    // And our secondary key field was not empty
                    if (pSecKeys[k])
                    {
                        if (pSecKeys[k] != pBind->boundKey)
                        {
                            // Did we have any keys with the same "up" state?
                            CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                            if (pUpBind)
                            {
                                pKeyBinds->UserChangeCommandBoundKey(pUpBind, pSecKeys[k]);
                            }
                            pKeyBinds->UserChangeCommandBoundKey(pBind, pSecKeys[k]);
                        }
                    }
                    // If the secondary key field was empty, we should remove the keybind
                    else
                    {
                        // Remove any matching "up" state binds we may have
                        CCommandBind* pUpBind = pKeyBinds->FindMatchingUpBind(pBind);
                        if (pUpBind)
                        {
                            pKeyBinds->UserRemoveCommandBoundKey(pUpBind);
                        }
                        pKeyBinds->UserRemoveCommandBoundKey(pBind);
                    }
                }
                // If this key bind didn't exist, create it
                else if (pSecKeys[k])
                {
                    if (strResource.empty())
                        pKeyBinds->AddCommand(pSecKeys[k], szCommand, szArguments);
                    else
                        pKeyBinds->AddCommand(pSecKeys[k]->szKey, szCommand, szArguments, true, strResource.c_str());

                    // Also add a matching "up" state if applicable
                    CCommandBind* pUpBind = pKeyBinds->GetBindFromCommand(szCommand, NULL, true, pPriKey->szKey, true, false);
                    if (pUpBind)
                    {
                        const char* szResource = pUpBind->resource.empty() ? nullptr : pUpBind->resource.c_str();
                        pKeyBinds->AddCommand(pSecKeys[k]->szKey, szCommand, pUpBind->arguments.c_str(), false, szResource);
                    }
                }
            }
        }
        else
        {
            assert(0);
        }
    }
}

bool CSettings::OnBindsListClick(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pBindsList->GetSelectedItem();
    if (pItem)
    {
        CGUIListItem* pItemBind = m_pBindsList->GetItem(m_pBindsList->GetItemRowIndex(pItem), m_hBind);

        // Proceed if the user didn't select the "Bind"-column
        if (pItem != pItemBind)
        {
            m_uiCaptureKey = 0;
            m_pSelectedBind = pItem;
            m_bCaptureKey = true;

            // Determine if the primary or secondary column was selected
            if (m_pBindsList->GetItemColumnIndex(pItem) == 1 /*m_hPriKey  Note: handle is not the same as index */)
            {
                // Create a messagebox to notify the user
                // SString strText = SString::Printf ( "Press a key to bind to '%s'", pItemBind->GetText ().c_str () );
                SString strText = _("Press a key to bind, or escape to clear");
                CCore::GetSingleton().ShowMessageBox(_("Binding a primary key"), strText, MB_ICON_QUESTION);
            }
            else
            {
                // Create a messagebox to notify the user
                // sSString strText = SString::Printf ( "Press a key to bind to '%s'", pItemBind->GetText ().c_str () );
                SString strText = _("Press a key to bind, or escape to clear");
                CCore::GetSingleton().ShowMessageBox(_("Binding a secondary key"), strText, MB_ICON_QUESTION);
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

bool CSettings::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_bCaptureKey)
    {
        CKeyBindsInterface* pKeyBinds = CCore::GetSingleton().GetKeyBinds();

        if (uMsg != WM_KEYDOWN && uMsg != WM_KEYUP && uMsg != WM_SYSKEYDOWN && uMsg != WM_SYSKEYUP && uMsg != WM_LBUTTONDOWN && uMsg != WM_LBUTTONUP &&
            uMsg != WM_RBUTTONDOWN && uMsg != WM_RBUTTONUP && uMsg != WM_MBUTTONDOWN && uMsg != WM_MBUTTONUP && uMsg != WM_XBUTTONDOWN &&
            uMsg != WM_XBUTTONUP && uMsg != WM_MOUSEWHEEL)
        {
            return false;
        }

        // Ignore the first key, which is the one that initiated the capture process
        if (m_uiCaptureKey == 0)
        {
            m_uiCaptureKey++;
            return false;
        }

        bool                bState;
        const SBindableKey* pKey = pKeyBinds->GetBindableFromMessage(uMsg, wParam, lParam, bState);

        // If escape was pressed, don't create a bind
        if (uMsg == WM_KEYDOWN && wParam == VK_ESCAPE)
        {
            m_pSelectedBind->SetText(CORE_SETTINGS_NO_KEY);
        }
        else if (pKey)
        {
            m_pSelectedBind->SetText(pKey->szKey);
        }
        else
        {
            return false;
        }

        // Remove the messagebox we created earlier
        CCore::GetSingleton().RemoveMessageBox();

        // Make sure the list gets redrawed/updated
        m_pBindsList->Activate();

        m_bCaptureKey = false;
        return true;
    }

    return false;
}

void CSettings::Initialize()
{
    using KeyBindPtr = CKeyBindsInterface::KeyBindPtr;

    // Add binds and sections
    bool bPrimaryKey = true;
    int  iBind = 0, iRowGame;

    // Add the rows
    CKeyBinds* pKeyBinds = reinterpret_cast<CKeyBinds*>(CCore::GetSingleton().GetKeyBinds());
    iRowGame = m_pBindsList->AddRow();
    m_pBindsList->SetItemText(iRowGame, m_hBind, _("GTA GAME CONTROLS"), false, true);
    m_pBindsList->SetItemText(m_pBindsList->AddRow(), m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true);
    m_pBindsList->SetItemText(m_pBindsList->AddRow(), m_hBind, _("MULTIPLAYER CONTROLS"), false, true);
    // iRows = CORE_SETTINGS_HEADERS;            // (game keys), (multiplayer keys)
    int iGameRowCount = 1;
    int iMultiplayerRowCount = 2;

    list<SKeyBindSection*>::const_iterator iters = m_pKeyBindSections.begin();
    for (; iters != m_pKeyBindSections.end(); iters++)
    {
        (*iters)->currentIndex = -1;
        (*iters)->rowCount = 0;
    }

    pKeyBinds->SortCommandBinds();

    // Loop through all the available controls
    int i = 0;

    for (; *g_bcControls[i].szControl != NULL; i++)
    {
    }

    for (i--; i >= 0; i--)
    {
        SBindableGTAControl* pControl = &g_bcControls[i];

        // Loop through the binds for a matching control
        size_t numMatches = 0;

        for (KeyBindPtr& bind : *pKeyBinds)
        {
            if (bind->isBeingDeleted || bind->type != KeyBindType::GTA_CONTROL)
                continue;

            auto controlBind = reinterpret_cast<CGTAControlBind*>(bind.get());

            if (controlBind->control != pControl)
                continue;

            if (!numMatches)            // Primary key
            {
                // Add bind to the list
                iBind = m_pBindsList->InsertRowAfter(iRowGame);
                m_pBindsList->SetItemText(iBind, m_hBind, _(pControl->szDescription));
                m_pBindsList->SetItemText(iBind, m_hPriKey, controlBind->boundKey->szKey);
                for (int k = 0; k < SecKeyNum; k++)
                    m_pBindsList->SetItemText(iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY);
                m_pBindsList->SetItemData(iBind, m_hBind, reinterpret_cast<void*>(KeyBindType::GTA_CONTROL));
                m_pBindsList->SetItemData(iBind, m_hPriKey, controlBind);
                iGameRowCount++;
            }
            else            // Secondary key
            {
                for (size_t k = 0; k < SecKeyNum; k++)
                {
                    if (numMatches == k + 1)
                    {
                        m_pBindsList->SetItemText(iBind, m_hSecKeys[k], controlBind->boundKey->szKey);
                        m_pBindsList->SetItemData(iBind, m_hSecKeys[k], controlBind);
                    }
                }
            }

            ++numMatches;
        }

        // If we didnt find any matches
        if (!numMatches)
        {
            iBind = m_pBindsList->InsertRowAfter(iRowGame);
            m_pBindsList->SetItemText(iBind, m_hBind, _(pControl->szDescription));
            m_pBindsList->SetItemText(iBind, m_hPriKey, CORE_SETTINGS_NO_KEY);
            for (int k = 0; k < SecKeyNum; k++)
                m_pBindsList->SetItemText(iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY);
            m_pBindsList->SetItemData(iBind, m_hBind, reinterpret_cast<void*>(KeyBindType::UNDEFINED));
            m_pBindsList->SetItemData(iBind, m_hPriKey, pControl);
            iGameRowCount++;
        }
    }

    struct SListedCommand
    {
        int           iIndex;
        CCommandBind* pBind;
        unsigned int  uiMatchCount;
    };

    auto         listedCommands = std::make_unique<SListedCommand[]>(pKeyBinds->Count(KeyBindType::COMMAND) + pKeyBinds->Count(KeyBindType::FUNCTION));
    unsigned int uiNumListedCommands = 0;

    std::map<std::string, int> iResourceItems;

    for (KeyBindPtr& bind : *pKeyBinds)
    {
        // keys bound to a console command or a function (we don't show keys bound
        // from gta controls by scripts as these are clearly not user editable)
        if (bind->isBeingDeleted || bind->type != KeyBindType::COMMAND)
            continue;

        auto commandBind = reinterpret_cast<CCommandBind*>(bind.get());

        if (!commandBind->triggerState)
            continue;

        bool foundMatches = false;

        // Loop through the already listed array of commands for matches
        for (unsigned int i = 0; i < uiNumListedCommands; i++)
        {
            SListedCommand* pListedCommand = &listedCommands[i];
            CCommandBind*   pListedBind = pListedCommand->pBind;

            if (pListedBind->command == commandBind->command)
            {
                if (pListedBind->arguments.empty() || pListedBind->arguments == commandBind->arguments)
                {
                    // If we found a 1st match, add it to the secondary section
                    foundMatches = true;

                    for (int k = 0; k < SecKeyNum; k++)
                    {
                        if (pListedCommand->uiMatchCount == k)
                        {
                            m_pBindsList->SetItemText(pListedCommand->iIndex, m_hSecKeys[k], commandBind->boundKey->szKey);
                            m_pBindsList->SetItemData(pListedCommand->iIndex, m_hSecKeys[k], commandBind);
                        }
                    }

                    pListedCommand->uiMatchCount++;
                }
            }
        }

        // If there weren't any matches
        if (!foundMatches)
        {
            unsigned int row = iGameRowCount + 1;

            // Combine command and arguments
            SString strDescription;
            bool    bSkip = false;

            if (!commandBind->resource.empty())
            {
                if (commandBind->isActive)
                {
                    const std::string& resource = commandBind->resource;

                    if (iResourceItems.count(resource) == 0)
                    {
                        iBind = m_pBindsList->AddRow(true);
                        m_pBindsList->SetItemText(iBind, m_hBind, CORE_SETTINGS_HEADER_SPACER, false, true);

                        iBind = m_pBindsList->AddRow(true);
                        m_pBindsList->SetItemText(iBind, m_hBind, resource.c_str(), false, true);
                        m_pBindsList->SetItemData(iBind, m_hBind, reinterpret_cast<void*>(255));
                        iResourceItems.insert(make_pair(resource, iBind));
                    }

                    row = iResourceItems[resource];
                    iMultiplayerRowCount++;
                }
                else
                    continue;
            }

            if (!commandBind->arguments.empty())
            {
                strDescription.Format("%s: %s", commandBind->command.c_str(), commandBind->arguments.c_str());
                iMultiplayerRowCount++;
            }
            else
            {
                strDescription = commandBind->command;
                iMultiplayerRowCount++;
            }

            if (!bSkip)
            {
                // Add the bind to the list
                iBind = m_pBindsList->AddRow(true);
                m_pBindsList->SetItemText(iBind, m_hBind, strDescription);
                m_pBindsList->SetItemText(iBind, m_hPriKey, commandBind->boundKey->szKey);
                for (int k = 0; k < SecKeyNum; k++)
                    m_pBindsList->SetItemText(iBind, m_hSecKeys[k], CORE_SETTINGS_NO_KEY);
                m_pBindsList->SetItemData(iBind, m_hBind, reinterpret_cast<void*>(KeyBindType::COMMAND));
                m_pBindsList->SetItemData(iBind, m_hPriKey, commandBind);

                // Add it to the already-listed array
                SListedCommand* pListedCommand = &listedCommands[uiNumListedCommands];
                pListedCommand->iIndex = iBind;
                pListedCommand->pBind = commandBind;
                pListedCommand->uiMatchCount = 0;
                uiNumListedCommands++;
            }
        }
    }
}

void CSettings::SetVisible(bool bVisible)
{
    // Hide / show the form
    m_pWindow->SetVisible(bVisible);

    // Load the config file if the dialog is shown
    if (bVisible)
    {
#ifdef MTA_DEBUG
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
            CreateGUI();            // Recreate GUI (for adjusting layout with edit and continue)
#endif
        m_pWindow->BringToFront();
        m_pWindow->Activate();
        LoadData();

        // Clear the binds list
        m_pBindsList->Clear();

        // Re-initialize the binds list
        Initialize();
    }

    m_pWindow->SetZOrderingEnabled(!bVisible);            // Message boxes dont appear on top otherwise
}

bool CSettings::IsVisible()
{
    return m_pWindow->IsVisible();
}

void CSettings::SetIsModLoaded(bool bLoaded)
{
    m_bIsModLoaded = bLoaded;
}

bool CSettings::OnOKButtonClick(CGUIElement* pElement)
{
    CMainMenu* pMainMenu = CLocalGUI::GetSingleton().GetMainMenu();

    // Process keybinds
    ProcessKeyBinds();
    ProcessJoypad();

    // Invalid nickname?
    if (!CCore::GetSingleton().IsValidNick(m_pEditNick->GetText().c_str()))
    {
        CCore::GetSingleton().ShowMessageBox(_("Error"), _("Your nickname contains invalid characters!"), MB_BUTTON_OK | MB_ICON_INFO);
        return true;
    }

    // Save the config
    SaveData();

    // Close the window
    m_pWindow->SetVisible(false);
    pMainMenu->m_bIsInSubWindow = false;

    return true;
}

bool CSettings::OnNickButtonClick(CGUIElement* pElement)
{
    m_pEditNick->SetText(CNickGen::GetRandomNickname());
    return true;
}

bool CSettings::OnCancelButtonClick(CGUIElement* pElement)
{
    CMainMenu* pMainMenu = CLocalGUI::GetSingleton().GetMainMenu();

    m_pWindow->SetVisible(false);
    pMainMenu->m_bIsInSubWindow = false;

    // restore old audio volume settings
    CVARS_SET("mastervolume", m_fOldMasterVolume);
    SetRadioVolume(m_fOldRadioVolume);
    SetSFXVolume(m_fOldSFXVolume);
    CVARS_SET("mtavolume", m_fOldMTAVolume);
    CVARS_SET("voicevolume", m_fOldVoiceVolume);

    // restore old audio mute settings
    CVARS_SET("mute_master_when_minimized", m_bOldMuteMaster);
    CVARS_SET("mute_radio_when_minimized", m_bOldMuteRadio);
    CVARS_SET("mute_sfx_when_minimized", m_bOldMuteSFX);
    CVARS_SET("mute_mta_when_minimized", m_bOldMuteMTA);
    CVARS_SET("mute_voice_when_minimized", m_bOldMuteVoice);

    m_pCheckBoxMuteMaster->SetSelected(m_bOldMuteMaster);
    m_pCheckBoxMuteRadio->SetSelected(m_bOldMuteRadio);
    m_pCheckBoxMuteSFX->SetSelected(m_bOldMuteSFX);
    m_pCheckBoxMuteMTA->SetSelected(m_bOldMuteMTA);
    m_pCheckBoxMuteVoice->SetSelected(m_bOldMuteVoice);

    m_pCheckBoxMuteRadio->SetEnabled(!m_bOldMuteMaster);
    m_pCheckBoxMuteSFX->SetEnabled(!m_bOldMuteMaster);
    m_pCheckBoxMuteMTA->SetEnabled(!m_bOldMuteMaster);
    m_pCheckBoxMuteVoice->SetEnabled(!m_bOldMuteMaster);

    return true;
}

void CSettings::LoadData()
{
    // Ensure CVARS ranges ok
    CClientVariables::GetSingleton().ValidateValues();

    std::string strVar;
    bool        bVar;

    // Put the nick in the edit
    CVARS_GET("nick", strVar);

    if (CCore::GetSingleton().IsValidNick(strVar.c_str()))
    {
        m_pEditNick->SetText(strVar.c_str());
    }
    else
    {
        m_pEditNick->SetText(CNickGen::GetRandomNickname());
    }

    // Save server password
    CVARS_GET("save_server_passwords", bVar);
    m_pSavePasswords->SetSelected(bVar);
    CVARS_GET("auto_refresh_browser", bVar);
    m_pAutoRefreshBrowser->SetSelected(bVar);

    // Allow screen upload
    bool bAllowScreenUploadEnabled;
    CVARS_GET("allow_screen_upload", bAllowScreenUploadEnabled);
    m_pCheckBoxAllowScreenUpload->SetSelected(bAllowScreenUploadEnabled);

    // Allow external sounds
    bool bAllowExternalSoundsEnabled;
    CVARS_GET("allow_external_sounds", bAllowExternalSoundsEnabled);
    m_pCheckBoxAllowExternalSounds->SetSelected(bAllowExternalSoundsEnabled);

    // Always show transfer box
    bool alwaysShowTransferBox = false;
    CVARS_GET("always_show_transferbox", alwaysShowTransferBox);
    m_pCheckBoxAlwaysShowTransferBox->SetSelected(alwaysShowTransferBox);

    // Allow DiscordRPC
    bool bAllowDiscordRPC;
    CVARS_GET("allow_discord_rpc", bAllowDiscordRPC);
    m_pCheckBoxAllowDiscordRPC->SetSelected(bAllowDiscordRPC);

    // Customized sa files
    m_pCheckBoxCustomizedSAFiles->SetSelected(GetApplicationSettingInt("customized-sa-files-request") != 0);
    m_pCheckBoxCustomizedSAFiles->SetVisible(GetApplicationSettingInt("customized-sa-files-show") != 0);

    // Controls
    CVARS_GET("invert_mouse", bVar);
    m_pInvertMouse->SetSelected(bVar);
    CVARS_GET("steer_with_mouse", bVar);
    m_pSteerWithMouse->SetSelected(bVar);
    CVARS_GET("fly_with_mouse", bVar);
    m_pFlyWithMouse->SetSelected(bVar);
    CVARS_GET("classic_controls", bVar);
    m_pClassicControls->SetSelected(bVar);

    CGameSettings*            gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    CControllerConfigManager* pController = g_pCore->GetGame()->GetControllerConfigManager();

    m_pMouseSensitivity->SetScrollPosition(gameSettings->GetMouseSensitivity());
    pController->SetVerticalAimSensitivityRawValue(CVARS_GET_VALUE<float>("vertical_aim_sensitivity"));
    m_pVerticalAimSensitivity->SetScrollPosition(pController->GetVerticalAimSensitivity());

    // Audio
    m_pCheckBoxAudioEqualizer->SetSelected(gameSettings->IsRadioEqualizerEnabled());
    m_pCheckBoxAudioAutotune->SetSelected(gameSettings->IsRadioAutotuneEnabled());
    m_pCheckBoxUserAutoscan->SetSelected(gameSettings->IsUsertrackAutoScan());

    CVARS_GET("mute_master_when_minimized", m_bOldMuteMaster);
    CVARS_GET("mute_radio_when_minimized", m_bOldMuteRadio);
    CVARS_GET("mute_sfx_when_minimized", m_bOldMuteSFX);
    CVARS_GET("mute_mta_when_minimized", m_bOldMuteMTA);
    CVARS_GET("mute_voice_when_minimized", m_bOldMuteVoice);

    m_pCheckBoxMuteMaster->SetSelected(m_bOldMuteMaster);
    m_pCheckBoxMuteRadio->SetSelected(m_bOldMuteRadio);
    m_pCheckBoxMuteSFX->SetSelected(m_bOldMuteSFX);
    m_pCheckBoxMuteMTA->SetSelected(m_bOldMuteMTA);
    m_pCheckBoxMuteVoice->SetSelected(m_bOldMuteVoice);

    m_pCheckBoxMuteRadio->SetEnabled(!m_bOldMuteMaster);
    m_pCheckBoxMuteSFX->SetEnabled(!m_bOldMuteMaster);
    m_pCheckBoxMuteMTA->SetEnabled(!m_bOldMuteMaster);
    m_pCheckBoxMuteVoice->SetEnabled(!m_bOldMuteMaster);

    unsigned int uiUsertrackMode = gameSettings->GetUsertrackMode();
    if (uiUsertrackMode == 0)
        m_pComboUsertrackMode->SetText(_("Radio"));
    else if (uiUsertrackMode == 1)
        m_pComboUsertrackMode->SetText(_("Random"));
    else if (uiUsertrackMode == 2)
        m_pComboUsertrackMode->SetText(_("Sequential"));

    CVARS_GET("mastervolume", m_fOldMasterVolume);
    CVARS_GET("mtavolume", m_fOldMTAVolume);
    CVARS_GET("voicevolume", m_fOldVoiceVolume);
    m_fOldMasterVolume = max(0.0f, min(1.0f, m_fOldMasterVolume));
    m_fOldRadioVolume = max(0.0f, min(1.0f, m_fRadioVolume));
    m_fOldSFXVolume = max(0.0f, min(1.0f, m_fSFXVolume));
    m_fOldMTAVolume = max(0.0f, min(1.0f, m_fOldMTAVolume));
    m_fOldVoiceVolume = max(0.0f, min(1.0f, m_fOldVoiceVolume));
    m_pAudioMasterVolume->SetScrollPosition(m_fOldMasterVolume);
    m_pAudioRadioVolume->SetScrollPosition(m_fOldRadioVolume);
    m_pAudioSFXVolume->SetScrollPosition(m_fOldSFXVolume);
    m_pAudioMTAVolume->SetScrollPosition(m_fOldMTAVolume);
    m_pAudioVoiceVolume->SetScrollPosition(m_fOldVoiceVolume);

    UpdateVideoTab();

    // Locale
    SString strLocale;
    CVARS_GET("locale", strLocale);
    unsigned int uiIndex = 0;
    while (uiIndex != m_pInterfaceLanguageSelector->GetItemCount())
    {
        CGUIListItem* pItem = m_pInterfaceLanguageSelector->GetItemByIndex(uiIndex);
        if (((const char*)pItem->GetData()) == strLocale)
            break;

        uiIndex++;
    }
    m_pInterfaceLanguageSelector->SetSelectedItemByIndex(uiIndex);

    // Skins
    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);

    for (size_t i = 0; i < m_pInterfaceSkinSelector->GetItemCount(); i++)
    {
        std::string strItemText = m_pInterfaceSkinSelector->GetItemText(i);
        if (currentSkin == strItemText)
        {
            m_pInterfaceSkinSelector->SetSelectedItemByIndex(i);
            break;
        }
    }

    // Process priority
    int iVar;
    CVARS_GET("process_priority", iVar);
    if (iVar == 0)
        m_pPriorityCombo->SetText(_("Normal"));
    else if (iVar == 1)
        m_pPriorityCombo->SetText(_("Above normal"));
    else if (iVar == 2)
        m_pPriorityCombo->SetText(_("High"));
    int PriorityClassList[] = {NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS};
    SetPriorityClass(GetCurrentProcess(), PriorityClassList[CVARS_GET_VALUE<int>("process_priority") % 3]);

    // Fast clothes loading
    CVARS_GET("fast_clothes_loading", iVar);
    if (iVar == CMultiplayer::FAST_CLOTHES_OFF)
        m_pFastClothesCombo->SetText(_("Off"));
    else if (iVar == CMultiplayer::FAST_CLOTHES_AUTO)
        m_pFastClothesCombo->SetText(_("Auto"));
    else if (iVar == CMultiplayer::FAST_CLOTHES_ON)
        m_pFastClothesCombo->SetText(_("On"));

    // Browser speed
    CVARS_GET("browser_speed", iVar);
    if (iVar == 0)
        m_pBrowserSpeedCombo->SetText(_("Very slow"));
    else if (iVar == 1)
        m_pBrowserSpeedCombo->SetText(_("Default"));
    else if (iVar == 2)
        m_pBrowserSpeedCombo->SetText(_("Fast"));

    // Single download
    CVARS_GET("single_download", iVar);
    if (iVar == 0)
        m_pSingleDownloadCombo->SetText(_("Default"));
    else if (iVar == 1)
        m_pSingleDownloadCombo->SetText(_("On"));

    // Packet tag
    CVARS_GET("packet_tag", iVar);
    if (iVar == 0)
        m_pPacketTagCombo->SetText(_("Default"));
    else if (iVar == 1)
        m_pPacketTagCombo->SetText(_("On"));

    // Progress animation
    CVARS_GET("progress_animation", iVar);
    if (iVar == 0)
        m_pProgressAnimationCombo->SetText(_("Off"));
    else if (iVar == 1)
        m_pProgressAnimationCombo->SetText(_("Default"));

    // Windows 8 16-bit color
    iVar = GetApplicationSettingInt("Win8Color16");
    m_pWin8ColorCheckBox->SetSelected(iVar != 0);

    // Windows 8 mouse fix
    iVar = GetApplicationSettingInt("Win8MouseFix");
    m_pWin8MouseCheckBox->SetSelected(iVar != 0);

    // Update build type
    CVARS_GET("update_build_type", iVar);
    if (iVar == 0 || iVar == 1)
        m_pUpdateBuildTypeCombo->SetText(_("Default"));
    else if (iVar == 2)
        m_pUpdateBuildTypeCombo->SetText("Nightly");

    // Update auto install
    CVARS_GET("update_auto_install", iVar);
    if (iVar == 0)
        m_pUpdateAutoInstallCombo->SetText(_("Off"));
    else if (iVar == 1)
        m_pUpdateAutoInstallCombo->SetText("Default");

    // Chat
    LoadChatColorFromCVar(Chat::ColorType::BG, "chat_color");
    LoadChatColorFromCVar(Chat::ColorType::TEXT, "chat_text_color");
    LoadChatColorFromCVar(Chat::ColorType::INPUT_BG, "chat_input_color");
    LoadChatColorFromCVar(Chat::ColorType::INPUT_TEXT, "chat_input_text_color");

    unsigned int uiFont;
    CVARS_GET("chat_font", uiFont);
    if (uiFont >= Chat::Font::MAX)
        uiFont = Chat::Font::DEFAULT;
    m_pRadioChatFont[uiFont]->SetSelected(true);

    CVARS_GET("chat_lines", strVar);
    m_pChatLines->SetText(strVar.c_str());

    try
    {
        CVARS_GET("chat_scale", strVar);
        stringstream ss(strVar);
        ss >> strVar;
        m_pChatScaleX->SetText(SString("%1.1f", atof(strVar.c_str())));
        ss >> strVar;
        m_pChatScaleY->SetText(SString("%1.1f", atof(strVar.c_str())));
    }
    catch (...)
    {
    }

    CVARS_GET("chat_width", strVar);
    m_pChatWidth->SetText(strVar.c_str());
    CVARS_GET("chat_css_style_text", bVar);
    m_pChatCssText->SetSelected(bVar);
    CVARS_GET("chat_css_style_background", bVar);
    m_pChatCssBackground->SetSelected(bVar);
    CVARS_GET("chat_nickcompletion", bVar);
    m_pChatNickCompletion->SetSelected(bVar);
    CVARS_GET("chat_text_outline", bVar);
    m_pChatTextBlackOutline->SetSelected(bVar);

    {
        int iVar;
        CVARS_GET("chat_line_life", iVar);
        SetMilliseconds(m_pChatLineLife, iVar);

        CVARS_GET("chat_line_fade_out", iVar);
        SetMilliseconds(m_pChatLineFadeout, iVar);
    }

    // Chat position
    CVARS_GET("chat_position_horizontal", iVar);
    if (iVar > Chat::Position::Horizontal::RIGHT)
        iVar = Chat::Position::Horizontal::LEFT;
    m_pChatHorizontalCombo->SetSelectedItemByIndex(iVar);

    CVARS_GET("chat_position_vertical", iVar);
    if (iVar > Chat::Position::Vertical::BOTTOM)
        iVar = Chat::Position::Vertical::TOP;
    m_pChatVerticalCombo->SetSelectedItemByIndex(iVar);

    CVARS_GET("chat_text_alignment", iVar);
    if (iVar > Chat::Text::Align::RIGHT)
        iVar = Chat::Text::Align::LEFT;
    m_pChatTextAlignCombo->SetSelectedItemByIndex(iVar);

    CVARS_GET("chat_position_offset_x", strVar);
    m_pChatOffsetX->SetText(strVar.c_str());
    CVARS_GET("chat_position_offset_y", strVar);
    m_pChatOffsetY->SetText(strVar.c_str());

    // Interface
    CVARS_GET("server_can_flash_window", bVar);
    m_pFlashWindow->SetSelected(bVar);
    CVARS_GET("allow_tray_notifications", bVar);
    m_pTrayBalloon->SetSelected(bVar);

    // Browser
    CVARS_GET("browser_remote_websites", bVar);
    m_pCheckBoxRemoteBrowser->SetSelected(bVar);
    CVARS_GET("browser_remote_javascript", bVar);
    m_pCheckBoxRemoteJavascript->SetSelected(bVar);
    CVARS_GET("browser_enable_gpu", bVar);
    m_pCheckBoxBrowserGPUEnabled->SetSelected(bVar);

    ReloadBrowserLists();
}

void CSettings::ReloadBrowserLists()
{
    m_pGridBrowserBlacklist->Clear();
    m_pGridBrowserWhitelist->Clear();
    m_bBrowserListsChanged = false;
    if (m_bBrowserListsLoadEnabled)
    {
        auto                                  pWebCore = g_pCore->GetWebCore();
        std::vector<std::pair<SString, bool>> customBlacklist;
        pWebCore->GetFilterEntriesByType(customBlacklist, eWebFilterType::WEBFILTER_USER);
        for (std::vector<std::pair<SString, bool>>::iterator iter = customBlacklist.begin(); iter != customBlacklist.end(); ++iter)
        {
            if (iter->second == false)
                m_pGridBrowserBlacklist->SetItemText(m_pGridBrowserBlacklist->AddRow(), 1, iter->first);
            else
                m_pGridBrowserWhitelist->SetItemText(m_pGridBrowserWhitelist->AddRow(), 1, iter->first);
        }
    }
}

bool CSettings::OnTabChanged(CGUIElement* pElement)
{
    if (pElement == m_pTabBrowser && !m_bBrowserListsLoadEnabled)
    {
        // Load browser lists when tab is selected for the first time
        m_bBrowserListsLoadEnabled = true;
        ReloadBrowserLists();
    }
    return true;
}

void CSettings::SaveData()
{
    std::string    strVar;
    CGameSettings* gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    // Set and save our settings
    if (CModManager::GetSingleton().GetCurrentMod() != NULL)
    {
        CVARS_GET("nick", strVar);
        if (m_pEditNick->GetText().compare(strVar) != 0)
            CCore::GetSingleton().GetCommands()->Execute("nick", m_pEditNick->GetText().c_str());
    }
    else
    {
        CVARS_SET("nick", m_pEditNick->GetText());
    }

    // Server pass saving
    bool bServerPasswords = m_pSavePasswords->GetSelected();
    CVARS_SET("save_server_passwords", bServerPasswords);
    if (!bServerPasswords)
    {
        g_pCore->GetSingleton().GetLocalGUI()->GetMainMenu()->GetServerBrowser()->ClearServerPasswords();
    }

    CVARS_SET("auto_refresh_browser", m_pAutoRefreshBrowser->GetSelected());

    // Very hacky
    CControllerConfigManager* pController = g_pCore->GetGame()->GetControllerConfigManager();
    CVARS_SET("invert_mouse", m_pInvertMouse->GetSelected());
    pController->SetMouseInverted(m_pInvertMouse->GetSelected());
    CVARS_SET("steer_with_mouse", m_pSteerWithMouse->GetSelected());
    pController->SetSteerWithMouse(m_pSteerWithMouse->GetSelected());
    CVARS_SET("fly_with_mouse", m_pFlyWithMouse->GetSelected());
    pController->SetFlyWithMouse(m_pFlyWithMouse->GetSelected());
    CVARS_SET("classic_controls", m_pClassicControls->GetSelected());
    pController->SetClassicControls(m_pClassicControls->GetSelected());
    pController->SetVerticalAimSensitivity(m_pVerticalAimSensitivity->GetScrollPosition());
    CVARS_SET("vertical_aim_sensitivity", pController->GetVerticalAimSensitivityRawValue());

    // Video
    // get current
    int  iNextVidMode;
    bool bNextWindowed;
    bool bNextFSMinimize;
    int  iNextFullscreenStyle;
    GetVideoModeManager()->GetNextVideoMode(iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle);
    int iAntiAliasing = gameSettings->GetAntiAliasing();

    // update from gui
    bNextWindowed = m_pCheckBoxWindowed->GetSelected();
    if (CGUIListItem* pSelected = m_pComboResolution->GetSelectedItem())
        iNextVidMode = (int)pSelected->GetData();
    bNextFSMinimize = m_pCheckBoxMinimize->GetSelected();
    if (CGUIListItem* pSelected = m_pFullscreenStyleCombo->GetSelectedItem())
        iNextFullscreenStyle = (int)pSelected->GetData();
    if (CGUIListItem* pSelected = m_pComboAntiAliasing->GetSelectedItem())
        iAntiAliasing = (int)pSelected->GetData();
    bool bCustomizedSAFilesEnabled = m_pCheckBoxCustomizedSAFiles->GetSelected();
    bool bCustomizedSAFilesWasEnabled = GetApplicationSettingInt("customized-sa-files-request") != 0;

    if (CGUIListItem* pSelected = m_pPriorityCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("process_priority", iSelected);
        int PriorityClassList[] = {NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS};
        SetPriorityClass(GetCurrentProcess(), PriorityClassList[CVARS_GET_VALUE<int>("process_priority") % 3]);
    }

    // change
    bool bIsVideoModeChanged = GetVideoModeManager()->SetVideoMode(iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle);
    bool bIsAntiAliasingChanged = gameSettings->GetAntiAliasing() != iAntiAliasing;
    bool bIsCustomizedSAFilesChanged = bCustomizedSAFilesWasEnabled != bCustomizedSAFilesEnabled;

    gameSettings->SetAntiAliasing(iAntiAliasing, true);
    gameSettings->SetDrawDistance((m_pDrawDistance->GetScrollPosition() * 0.875f) + 0.925f);
    gameSettings->SetBrightness(m_pBrightness->GetScrollPosition() * 384);
    gameSettings->SetMouseSensitivity(m_pMouseSensitivity->GetScrollPosition());
    gameSettings->SetMipMappingEnabled(m_pCheckBoxMipMapping->GetSelected());
    SetApplicationSettingInt("customized-sa-files-request", bCustomizedSAFilesEnabled ? 1 : 0);

    // Process DPI awareness
    bool previousProcessDPIAware = false;
    CVARS_GET("process_dpi_aware", previousProcessDPIAware);

    const bool processsDPIAwareChanged = (m_pCheckBoxDPIAware->GetSelected() != previousProcessDPIAware);

    if (processsDPIAwareChanged)
        CVARS_SET("process_dpi_aware", !previousProcessDPIAware);

    // iFieldOfView
    int iFieldOfView = std::min<int>(4, (m_pFieldOfView->GetScrollPosition()) * (4 + 1)) * 5 + 70;
    CVARS_SET("fov", iFieldOfView);
    gameSettings->UpdateFieldOfViewFromSettings();

    // Anisotropic filtering
    int iAnisotropic = std::min<int>(m_iMaxAnisotropic, (m_pAnisotropic->GetScrollPosition()) * (m_iMaxAnisotropic + 1));
    CVARS_SET("anisotropic", iAnisotropic);

    // Visual FX Quality
    if (CGUIListItem* pQualitySelected = m_pComboFxQuality->GetSelectedItem())
    {
        gameSettings->SetFXQuality((int)pQualitySelected->GetData());
    }

    // Aspect ratio
    if (CGUIListItem* pRatioSelected = m_pComboAspectRatio->GetSelectedItem())
    {
        eAspectRatio aspectRatio = (eAspectRatio)(int)pRatioSelected->GetData();
        CVARS_SET("aspect_ratio", aspectRatio);
    }

    // HUD match aspect ratio
    bool bHudMatchAspectRatio = m_pCheckBoxHudMatchAspectRatio->GetSelected();
    CVARS_SET("hud_match_aspect_ratio", bHudMatchAspectRatio);
    gameSettings->SetAspectRatio((eAspectRatio)CVARS_GET_VALUE<int>("aspect_ratio"), bHudMatchAspectRatio);

    // Volumetric shadows
    bool bVolumetricShadowsEnabled = m_pCheckBoxVolumetricShadows->GetSelected();
    CVARS_SET("volumetric_shadows", bVolumetricShadowsEnabled);
    gameSettings->SetVolumetricShadowsEnabled(bVolumetricShadowsEnabled);

    // Device selection dialog
    bool bDeviceSelectionDialogEnabled = m_pCheckBoxDeviceSelectionDialog->GetSelected();
    SetApplicationSettingInt("device-selection-disabled", bDeviceSelectionDialogEnabled ? 0 : 1);

    // Show unsafe resolutions
    bool bShowUnsafeResolutions = m_pCheckBoxShowUnsafeResolutions->GetSelected();
    CVARS_SET("show_unsafe_resolutions", bShowUnsafeResolutions);

    // Allow screen upload
    bool bAllowScreenUploadEnabled = m_pCheckBoxAllowScreenUpload->GetSelected();
    CVARS_SET("allow_screen_upload", bAllowScreenUploadEnabled);

    // Allow external sounds
    bool bAllowExternalSoundsEnabled = m_pCheckBoxAllowExternalSounds->GetSelected();
    CVARS_SET("allow_external_sounds", bAllowExternalSoundsEnabled);

    // Always show transfer box
    bool alwaysShowTransferBox = m_pCheckBoxAlwaysShowTransferBox->GetSelected();
    CVARS_SET("always_show_transferbox", alwaysShowTransferBox);
    g_pCore->GetModManager()->TriggerCommand(mtasa::CMD_ALWAYS_SHOW_TRANSFERBOX, alwaysShowTransferBox);

    // Allow DiscordRPC
    bool bAllowDiscordRPC = m_pCheckBoxAllowDiscordRPC->GetSelected();
    CVARS_SET("allow_discord_rpc", bAllowDiscordRPC);
    g_pCore->GetDiscord()->SetDiscordRPCEnabled(bAllowDiscordRPC);

    if (bAllowDiscordRPC)
    {
        const auto discord = g_pCore->GetDiscord();

        if (discord)
        {
            const char* state = _("Main menu");

            if (g_pCore->IsConnected())
            {                
                state = _("In-game");

                const SString& serverName = g_pCore->GetLastConnectedServerName();
                discord->SetPresenceDetails(serverName.c_str(), false);
            }

            discord->SetPresenceState(state, false);
        }
    }

    // Grass
    bool bGrassEnabled = m_pCheckBoxGrass->GetSelected();
    CVARS_SET("grass", bGrassEnabled);
    gameSettings->SetGrassEnabled(bGrassEnabled);

    // Heat haze
    bool bHeatHazeEnabled = m_pCheckBoxHeatHaze->GetSelected();
    CVARS_SET("heat_haze", bHeatHazeEnabled);
    g_pCore->GetMultiplayer()->SetHeatHazeEnabled(bHeatHazeEnabled);

    // Tyre smoke particles
    bool bTyreSmokeEnabled = m_pCheckBoxTyreSmokeParticles->GetSelected();
    CVARS_SET("tyre_smoke_enabled", bTyreSmokeEnabled);
    g_pCore->GetMultiplayer()->SetTyreSmokeEnabled(bTyreSmokeEnabled);

    // High detail vehicles
    bool bHighDetailVehicles = m_pCheckBoxHighDetailVehicles->GetSelected();
    CVARS_SET("high_detail_vehicles", bHighDetailVehicles);
    gameSettings->ResetVehiclesLODDistance(false);

    // High detail peds
    bool bHighDetailPeds = m_pCheckBoxHighDetailPeds->GetSelected();
    CVARS_SET("high_detail_peds", bHighDetailPeds);
    gameSettings->ResetPedsLODDistance(false);

    // Blur
    bool bBlur = m_pCheckBoxBlur->GetSelected();
    CVARS_SET("blur", bBlur);
    gameSettings->ResetBlurEnabled();

    // Corona rain reflections
    bool bCoronaReflections = m_pCheckBoxCoronaReflections->GetSelected();
    CVARS_SET("corona_reflections", bCoronaReflections);
    gameSettings->ResetCoronaReflectionsEnabled();

    // Dynamic ped shadows
    bool bDynamicPedShadows = m_pCheckBoxDynamicPedShadows->GetSelected();
    CVARS_SET("dynamic_ped_shadows", bDynamicPedShadows);
    gameSettings->SetDynamicPedShadowsEnabled(bDynamicPedShadows);

    // Fast clothes loading
    if (CGUIListItem* pSelected = m_pFastClothesCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("fast_clothes_loading", iSelected);
        g_pCore->GetMultiplayer()->SetFastClothesLoading((CMultiplayer::EFastClothesLoading)iSelected);
    }

    // Audio
    gameSettings->SetRadioEqualizerEnabled(m_pCheckBoxAudioEqualizer->GetSelected());
    gameSettings->SetRadioAutotuneEnabled(m_pCheckBoxAudioAutotune->GetSelected());
    gameSettings->SetUsertrackAutoScan(m_pCheckBoxUserAutoscan->GetSelected());

    if (CGUIListItem* pSelected = m_pComboUsertrackMode->GetSelectedItem())
    {
        gameSettings->SetUsertrackMode((int)pSelected->GetData());
    }

    // Browser speed
    if (CGUIListItem* pSelected = m_pBrowserSpeedCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("browser_speed", iSelected);
    }

    // Single download
    if (CGUIListItem* pSelected = m_pSingleDownloadCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("single_download", iSelected);
    }

    // Packet tag
    if (CGUIListItem* pSelected = m_pPacketTagCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("packet_tag", iSelected);
    }

    // Progress animation
    if (CGUIListItem* pSelected = m_pProgressAnimationCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("progress_animation", iSelected);
    }

    // Windows 8 16-bit color
    SetApplicationSettingInt("Win8Color16", m_pWin8ColorCheckBox->GetSelected());

    // Windows 8 mouse fix
    SetApplicationSettingInt("Win8MouseFix", m_pWin8MouseCheckBox->GetSelected());

    // Debug setting
    if (CGUIListItem* pSelected = m_pDebugSettingCombo->GetSelectedItem())
    {
        EDiagnosticDebugType iSelected = (EDiagnosticDebugType)(int)pSelected->GetData();
        g_pCore->SetDiagnosticDebug(iSelected);
    }

    // Update build type
    if (CGUIListItem* pSelected = m_pUpdateBuildTypeCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("update_build_type", iSelected);
    }

    // Update auto install
    if (CGUIListItem* pSelected = m_pUpdateAutoInstallCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("update_auto_install", iSelected);
    }

    // Map alpha
    SString sText = m_pMapAlphaValueLabel->GetText();

    float fMapAlpha = ((atof(sText.substr(0, sText.length() - 1).c_str())) / 100) * 255;
    CVARS_SET("mapalpha", fMapAlpha);

    // Language
    CGUIListItem* pItem = m_pInterfaceLanguageSelector->GetSelectedItem();
    if (pItem)
    {
        const char* szItemText = (const char*)pItem->GetData();
        CVARS_SET("locale", std::string(szItemText));
        SetApplicationSetting("locale", szItemText);
    }

    // Chat
    SaveChatColor(Chat::ColorType::BG, "chat_color");
    SaveChatColor(Chat::ColorType::TEXT, "chat_text_color");
    SaveChatColor(Chat::ColorType::INPUT_BG, "chat_input_color");
    SaveChatColor(Chat::ColorType::INPUT_TEXT, "chat_input_text_color");
    for (int iFont = 0; iFont < Chat::ColorType::MAX; iFont++)
    {
        if (m_pRadioChatFont[iFont]->GetSelected())
        {
            CVARS_SET("chat_font", iFont);
            break;
        }
    }

    strVar = m_pChatScaleX->GetText() + " " + m_pChatScaleY->GetText();
    CVARS_SET("chat_scale", strVar);
    CVARS_SET("chat_lines", m_pChatLines->GetText());
    CVARS_SET("chat_width", m_pChatWidth->GetText());
    CVARS_SET("chat_css_style_text", m_pChatCssText->GetSelected());
    CVARS_SET("chat_css_style_background", m_pChatCssBackground->GetSelected());
    CVARS_SET("chat_nickcompletion", m_pChatNickCompletion->GetSelected());
    CVARS_SET("chat_text_outline", m_pChatTextBlackOutline->GetSelected());
    CVARS_SET("chat_line_life", GetMilliseconds(m_pChatLineLife));
    CVARS_SET("chat_line_fade_out", GetMilliseconds(m_pChatLineFadeout));

    CVARS_SET("chat_position_offset_x", m_pChatOffsetX->GetText());
    CVARS_SET("chat_position_offset_y", m_pChatOffsetY->GetText());
    if (CGUIListItem* pSelected = m_pChatHorizontalCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("chat_position_horizontal", iSelected);
    }
    if (CGUIListItem* pSelected = m_pChatVerticalCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("chat_position_vertical", iSelected);
    }
    if (CGUIListItem* pSelected = m_pChatTextAlignCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("chat_text_alignment", iSelected);
    }

    // Interface
    CVARS_SET("server_can_flash_window", m_pFlashWindow->GetSelected());
    CVARS_SET("allow_tray_notifications", m_pTrayBalloon->GetSelected());

    // Set our new skin last, as it'll destroy all our GUI
    pItem = m_pInterfaceSkinSelector->GetSelectedItem();
    if (pItem)
        CVARS_SET("current_skin", pItem->GetText());

    // Streaming memory
    float        fPos = m_pStreamingMemory->GetScrollPosition();
    int          min = g_pCore->GetMinStreamingMemory();
    int          max = g_pCore->GetMaxStreamingMemory();
    unsigned int value = SharedUtil::Lerp(min, fPos, max);
    CVARS_SET("streaming_memory", value);

    // Webbrowser settings
    bool bOldRemoteWebsites, bOldRemoteJavascript;
    CVARS_GET("browser_remote_websites", bOldRemoteWebsites);
    CVARS_GET("browser_remote_javascript", bOldRemoteJavascript);

    bool bBrowserSettingChanged = false;
    if (bOldRemoteWebsites != m_pCheckBoxRemoteBrowser->GetSelected() || bOldRemoteJavascript != m_pCheckBoxRemoteJavascript->GetSelected())
    {
        bBrowserSettingChanged = true;
        CVARS_SET("browser_remote_websites", m_pCheckBoxRemoteBrowser->GetSelected());
        CVARS_SET("browser_remote_javascript", m_pCheckBoxRemoteJavascript->GetSelected());
    }

    if (m_bBrowserListsLoadEnabled)
    {
        auto                 pWebCore = g_pCore->GetWebCore();
        std::vector<SString> customBlacklist;
        for (int i = 0; i < m_pGridBrowserBlacklist->GetRowCount(); ++i)
        {
            customBlacklist.push_back(m_pGridBrowserBlacklist->GetItemText(i, 1));
        }
        pWebCore->WriteCustomList("customblacklist", customBlacklist);

        std::vector<SString> customWhitelist;
        for (int i = 0; i < m_pGridBrowserWhitelist->GetRowCount(); ++i)
        {
            customWhitelist.push_back(m_pGridBrowserWhitelist->GetItemText(i, 1));
        }
        pWebCore->WriteCustomList("customwhitelist", customWhitelist);

        if (m_bBrowserListsChanged)
            bBrowserSettingChanged = true;
    }

    bool bBrowserGPUEnabled = false;
    CVARS_GET("browser_enable_gpu", bBrowserGPUEnabled);

    bool bBrowserGPUSetting = m_pCheckBoxBrowserGPUEnabled->GetSelected();
    bool bBrowserGPUSettingChanged = (bBrowserGPUSetting != bBrowserGPUEnabled);
    CVARS_SET("browser_enable_gpu", bBrowserGPUSetting);

    // Ensure CVARS ranges ok
    CClientVariables::GetSingleton().ValidateValues();

    // Save the config here
    CCore::GetSingleton().SaveConfig();
    // Save the single player settings (e.g. video mode, volume)
    gameSettings->Save();

    // Ask to restart?
    if (bIsVideoModeChanged || bIsAntiAliasingChanged || bIsCustomizedSAFilesChanged || processsDPIAwareChanged || bBrowserGPUSettingChanged)
        ShowRestartQuestion();
    else if (CModManager::GetSingleton().IsLoaded() && bBrowserSettingChanged)
        ShowDisconnectQuestion();
}

void CSettings::RemoveKeyBindSection(char* szSectionName)
{
    list<SKeyBindSection*>::iterator iter = m_pKeyBindSections.begin();
    for (; iter != m_pKeyBindSections.end(); iter++)
    {
        if (strcmp((*iter)->szOriginalTitle, szSectionName) == 0)
        {
            delete *iter;
            m_pKeyBindSections.erase(iter);
            break;
        }
    }
}

void CSettings::RemoveAllKeyBindSections()
{
    list<SKeyBindSection*>::const_iterator iter = m_pKeyBindSections.begin();
    for (; iter != m_pKeyBindSections.end(); iter++)
    {
        SKeyBindSection* section = (*iter);
        delete section;
    }
    m_pKeyBindSections.clear();
}

void CSettings::AddKeyBindSection(char* szSectionName)
{
    m_pKeyBindSections.push_back(new SKeyBindSection(szSectionName));
}

void CSettings::CreateChatColorTab(eChatColorType eType, const char* szName, CGUITabPanel* pParent)
{
    CVector2D vecTemp;

    // Create the GUI Elements
    CGUI*      pManager = g_pCore->GetGUI();
    CGUITab*   pTab = pParent->CreateTab(szName);
    CGUILabel* pLabel;

    // Size of lines and gaps
    float fLineSizeY = 20;
    float fLineGapY = 12;
    float fMarginX = 0;
    float fIndentX = 0;

    //
    // Color
    //
    {
        SString strRed = _("Red:"), strGreen = _("Green:"), strBlue = _("Blue:"), strTransparency = _("Transparency:");
        fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal", strRed, strGreen, strBlue, strTransparency) + 10.0f;

        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, _("Color")));
        pLabel->SetPosition(CVector2D(10.0f, 10.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize(NULL, 5.0f);
        pLabel->SetFont("default-bold-small");

        // Red
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, strRed));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();

        m_pChatRed[eType] = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTab));
        m_pChatRed[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatRed[eType]->SetSize(CVector2D(175.0f, fLineSizeY));
        m_pChatRed[eType]->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnChatRedChanged, this));
        m_pChatRed[eType]->SetProperty("StepSize", "0.004");

        m_pChatRedValue[eType] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, "0"));
        m_pChatRedValue[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX + 185.0f, vecTemp.fY));
        m_pChatRedValue[eType]->AutoSize("255 ");

        // Green
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, strGreen));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();

        m_pChatGreen[eType] = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTab));
        m_pChatGreen[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatGreen[eType]->SetSize(CVector2D(175.0f, fLineSizeY));
        m_pChatGreen[eType]->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnChatGreenChanged, this));
        m_pChatGreen[eType]->SetProperty("StepSize", "0.004");

        m_pChatGreenValue[eType] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, "0"));
        m_pChatGreenValue[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX + 185.0f, vecTemp.fY));
        m_pChatGreenValue[eType]->AutoSize("255 ");

        // Blue
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, strBlue));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();

        m_pChatBlue[eType] = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTab));
        m_pChatBlue[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatBlue[eType]->SetSize(CVector2D(175.0f, fLineSizeY));
        m_pChatBlue[eType]->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnChatBlueChanged, this));
        m_pChatBlue[eType]->SetProperty("StepSize", "0.004");

        m_pChatBlueValue[eType] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, "0"));
        m_pChatBlueValue[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX + 185.0f, vecTemp.fY));
        m_pChatBlueValue[eType]->AutoSize("255 ");

        // Transparency
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, strTransparency));
        pLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize();

        m_pChatAlpha[eType] = reinterpret_cast<CGUIScrollBar*>(pManager->CreateScrollBar(true, pTab));
        m_pChatAlpha[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatAlpha[eType]->SetSize(CVector2D(175.0f, fLineSizeY));
        m_pChatAlpha[eType]->SetOnScrollHandler(GUI_CALLBACK(&CSettings::OnChatAlphaChanged, this));
        m_pChatAlpha[eType]->SetProperty("StepSize", "0.004");

        m_pChatAlphaValue[eType] = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, "0"));
        m_pChatAlphaValue[eType]->SetPosition(CVector2D(vecTemp.fX + fIndentX + 185.0f, vecTemp.fY));
        m_pChatAlphaValue[eType]->AutoSize("255 ");

        fMarginX = vecTemp.fX + fIndentX + 185.0f + pManager->GetTextExtent("255") + 30.0f;
    }

    //
    // Preview
    //
    {
        pLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(pTab, _("Preview")));
        pLabel->SetPosition(CVector2D(fMarginX, 10.0f));
        pLabel->GetPosition(vecTemp);
        pLabel->AutoSize(NULL, 5.0f);
        pLabel->SetFont("default-bold-small");

        float fSize = fLineSizeY * 4 + fLineGapY * 3;

        auto pPreviewButton = pManager->CreateButton(pTab);
        pPreviewButton->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f - 2.0f));
        pPreviewButton->SetSize(CVector2D(fSize, fSize));
        pPreviewButton->SetAlpha(1.0f);
        pPreviewButton->Deactivate();

        auto pPreviewImage = pManager->CreateStaticImage(pPreviewButton);
        pPreviewImage->SetPosition(CVector2D(10.0f, 10.0f));
        pPreviewImage->SetSize(CVector2D(fSize - 20.0f, fSize - 20.0f));

        if (pPreviewImage->LoadFromFile(CalcMTASAPath("MTA\\cgui\\images\\radarset\\01.png")))
        {
            m_pChatColorPreview[eType] = pPreviewImage;
            UpdateChatColorPreview(eType);
        }
        else
        {
            m_pChatColorPreview[eType] = nullptr;
            delete pPreviewButton;
            pPreviewImage = nullptr;
            pPreviewButton = nullptr;
        }
    }
}

void CSettings::LoadChatColorFromCVar(eChatColorType eType, const char* szCVar)
{
    // Load the color according to the cvars and update the gui
    CColor pColor;
    CVARS_GET(szCVar, pColor);
    SetChatColorValues(eType, pColor);
}

void CSettings::SaveChatColor(eChatColorType eType, const char* szCVar)
{
    // Save the color to the cvar if it's different
    CColor pColor, pOldColor;
    pColor = GetChatColorValues(eType);
    CVARS_GET(szCVar, pOldColor);

    if (pColor.R != pOldColor.R || pColor.G != pOldColor.G || pColor.B != pOldColor.B || pColor.A != pOldColor.A)
        CVARS_SET(szCVar, pColor);
}

CColor CSettings::GetChatColorValues(eChatColorType eType)
{
    // Retrieve the color according to the scrollbar values
    CColor pColor;
    pColor.R = m_pChatRed[eType]->GetScrollPosition() * 255;
    pColor.G = m_pChatGreen[eType]->GetScrollPosition() * 255;
    pColor.B = m_pChatBlue[eType]->GetScrollPosition() * 255;
    pColor.A = m_pChatAlpha[eType]->GetScrollPosition() * 255;
    return pColor;
}

void CSettings::SetChatColorValues(eChatColorType eType, CColor pColor)
{
    // Set the scrollbar position based on the color
    m_pChatRed[eType]->SetScrollPosition((float)pColor.R / 255.0f);
    m_pChatGreen[eType]->SetScrollPosition((float)pColor.G / 255.0f);
    m_pChatBlue[eType]->SetScrollPosition((float)pColor.B / 255.0f);
    m_pChatAlpha[eType]->SetScrollPosition((float)pColor.A / 255.0f);
}

void CSettings::LoadChatPresets()
{
    CXMLFile* pPresetsFile = CCore::GetSingleton().GetXML()->CreateXML(CalcMTASAPath(CHAT_PRESETS_PATH));
    if (pPresetsFile && pPresetsFile->Parse())
    {
        CXMLNode* pPresetsRoot = pPresetsFile->GetRootNode();
        if (!pPresetsRoot)
            pPresetsRoot = pPresetsFile->CreateRootNode(CHAT_PRESETS_ROOT);

        list<CXMLNode*>::const_iterator iter = pPresetsRoot->ChildrenBegin();
        for (; iter != pPresetsRoot->ChildrenEnd(); iter++)
        {
            CXMLNode* pNode = reinterpret_cast<CXMLNode*>(*iter);
            if (pNode->GetTagName().compare("preset") == 0)
            {
                CXMLAttribute* pName = pNode->GetAttributes().Find("name");
                if (pName && pName->GetValue()[1])
                {
                    CGUIListItem* pItem = m_pChatPresets->AddItem(pName->GetValue().c_str());
                    pItem->SetData(pNode);
                }
            }
        }
    }
}

void CSettings::LoadSkins()
{
    std::vector<SString> directories = FindFiles(CalcMTASAPath(SKINS_PATH), false, true);

    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);

    for (std::vector<SString>::iterator it = directories.begin(); it != directories.end(); ++it)
    {
        CGUIListItem* item = m_pInterfaceSkinSelector->AddItem((*it));
        // TODO: add validation of the skin

        if (currentSkin == (*it))
            m_pInterfaceSkinSelector->SetSelectedItemByIndex(m_pInterfaceSkinSelector->GetItemIndex(item));
    }
}

void CSettings::LoadChatColorFromString(eChatColorType eType, const string& strColor)
{
    CColor       pColor;
    stringstream ss(strColor);
    int          iR, iG, iB, iA;

    try
    {
        ss >> iR >> iG >> iB >> iA;
        pColor.R = iR;
        pColor.G = iG;
        pColor.B = iB;
        pColor.A = iA;
        SetChatColorValues(eType, pColor);
    }
    catch (...)
    {
    }
}

int CSettings::GetMilliseconds(CGUIEdit* pEdit)
{
    // Note to anyone listening: stringstream does not handle out of range numbers well
    double dValue = strtol(pEdit->GetText().c_str(), NULL, 0);
    dValue *= 1000;
    dValue = Clamp<double>(INT_MIN, dValue, INT_MAX);
    return static_cast<int>(dValue);
}

void CSettings::SetMilliseconds(CGUIEdit* pEdit, int iValue)
{
    stringstream ss;
    ss << (float)iValue / 1000;
    pEdit->SetText(ss.str().c_str());
}

bool CSettings::OnChatLoadPresetClick(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pChatPresets->GetSelectedItem();
    if (!pItem)
        return true;

    CXMLNode* pNode = reinterpret_cast<CXMLNode*>(pItem->GetData());
    if (!pNode)
        return true;

    list<CXMLNode*>::const_iterator iter = pNode->ChildrenBegin();
    for (; iter != pNode->ChildrenEnd(); iter++)
    {
        // Load all settings provided
        CXMLNode* pSubNode = reinterpret_cast<CXMLNode*>(*iter);
        string    strTag = pSubNode->GetTagName();
        string    strValue = pSubNode->GetTagContent();

        if (strValue.length() > 0)
        {
            if (strTag == "color_text")
            {
                LoadChatColorFromString(Chat::ColorType::TEXT, strValue);
            }
            else if (strTag == "color_background")
            {
                LoadChatColorFromString(Chat::ColorType::BG, strValue);
            }
            else if (strTag == "color_input_text")
            {
                LoadChatColorFromString(Chat::ColorType::INPUT_TEXT, strValue);
            }
            else if (strTag == "color_input_background")
            {
                LoadChatColorFromString(Chat::ColorType::INPUT_BG, strValue);
            }
            else if (strTag == "font")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);

                if (iValue < 0 || iValue >= Chat::Font::MAX)
                    iValue = Chat::Font::DEFAULT;
                m_pRadioChatFont[iValue]->SetSelected(true);
            }
            else if (strTag == "lines")
            {
                m_pChatLines->SetText(strValue.c_str());
            }
            else if (strTag == "scale")
            {
                try
                {
                    stringstream ss(strValue);
                    ss >> strValue;
                    m_pChatScaleX->SetText(strValue.c_str());
                    ss >> strValue;
                    m_pChatScaleY->SetText(strValue.c_str());
                }
                catch (...)
                {
                }
            }
            else if (strTag == "position_horizontal")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);

                if (iValue >= Chat::Position::Horizontal::LEFT && iValue <= Chat::Position::Horizontal::RIGHT)
                {
                    m_pChatHorizontalCombo->SetSelectedItemByIndex(iValue);
                }
            }
            else if (strTag == "position_vertical")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);

                if (iValue >= Chat::Position::Vertical::TOP && iValue <= Chat::Position::Vertical::BOTTOM)
                {
                    m_pChatVerticalCombo->SetSelectedItemByIndex(iValue);
                }
            }
            else if (strTag == "text_alignment")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);

                if (iValue >= Chat::Text::Align::LEFT && iValue <= Chat::Text::Align::RIGHT)
                {
                    m_pChatTextAlignCombo->SetSelectedItemByIndex(iValue);
                }
            }
            else if (strTag == "offset_x")
            {
                m_pChatOffsetX->SetText(strValue.c_str());
            }
            else if (strTag == "offset_y")
            {
                m_pChatOffsetY->SetText(strValue.c_str());
            }
            else if (strTag == "width")
            {
                m_pChatWidth->SetText(strValue.c_str());
            }
            else if (strTag == "css_text")
            {
                bool bValue;
                pSubNode->GetTagContent(bValue);
                m_pChatCssText->SetSelected(bValue);
            }
            else if (strTag == "css_background")
            {
                bool bValue;
                pSubNode->GetTagContent(bValue);
                m_pChatCssBackground->SetSelected(bValue);
            }
            else if (strTag == "line_life")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);
                SetMilliseconds(m_pChatLineLife, iValue);
            }
            else if (strTag == "line_fadeout")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);
                SetMilliseconds(m_pChatLineFadeout, iValue);
            }
        }
    }

    return true;
}

bool CSettings::OnLanguageChanged(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pInterfaceLanguageSelector->GetSelectedItem();
    if (!pItem)
        return true;

    if (m_bIsModLoaded)
    {
        // Reset our item
        SString strLocale;
        CVARS_GET("locale", strLocale);
        unsigned int uiIndex = 0;
        while (uiIndex != m_pInterfaceLanguageSelector->GetItemCount())
        {
            CGUIListItem* pItem = m_pInterfaceLanguageSelector->GetItemByIndex(uiIndex);
            if (((const char*)pItem->GetData()) == strLocale)
                break;

            uiIndex++;
        }
        m_pInterfaceLanguageSelector->SetSelectedItemByIndex(uiIndex);

        g_pCore->ShowMessageBox(_("Error") + _E("CC82"), _("Please disconnect before changing language"), MB_BUTTON_OK | MB_ICON_INFO);
        m_pWindow->MoveToBack();
        return true;
    }

    return true;
}

bool CSettings::OnSkinChanged(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pInterfaceSkinSelector->GetSelectedItem();
    if (!pItem)
        return true;

    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);
    std::string newSkin = pItem->GetText();

    if (m_bIsModLoaded)
    {
        // Reset our item
        unsigned int uiIndex = 0;
        std::string  strItemText = m_pInterfaceSkinSelector->GetItemText(uiIndex);
        while (strItemText != currentSkin)
        {
            strItemText = m_pInterfaceSkinSelector->GetItemText(++uiIndex);
        }
        m_pInterfaceSkinSelector->SetSelectedItemByIndex(uiIndex);
        g_pCore->ShowMessageBox(_("Error") + _E("CC80"), _("Please disconnect before changing skin"), MB_BUTTON_OK | MB_ICON_INFO);
        m_pWindow->MoveToBack();
        return true;
    }

    return true;
}

bool CSettings::OnFieldOfViewChanged(CGUIElement* pElement)
{
    int iFieldOfView = std::min<int>(4, (m_pFieldOfView->GetScrollPosition()) * (4 + 1)) * 5 + 70;

    m_pFieldOfViewValueLabel->SetText(SString("%i", iFieldOfView).c_str());
    return true;
}

bool CSettings::OnDrawDistanceChanged(CGUIElement* pElement)
{
    int iDrawDistance = (m_pDrawDistance->GetScrollPosition()) * 100;

    m_pDrawDistanceValueLabel->SetText(SString("%i%%", iDrawDistance).c_str());
    return true;
}

bool CSettings::OnBrightnessChanged(CGUIElement* pElement)
{
    int iBrightness = (m_pBrightness->GetScrollPosition()) * 100;

    m_pBrightnessValueLabel->SetText(SString("%i%%", iBrightness).c_str());
    return true;
}

bool CSettings::OnAnisotropicChanged(CGUIElement* pElement)
{
    int iAnisotropic = std::min<int>(m_iMaxAnisotropic, (m_pAnisotropic->GetScrollPosition()) * (m_iMaxAnisotropic + 1));

    SString strLabel;
    if (iAnisotropic > 0)
        strLabel = SString("%ix", 1 << iAnisotropic);
    else
        strLabel = _("Off");

    m_pAnisotropicValueLabel->SetText(strLabel);
    return true;
}

bool CSettings::OnMouseSensitivityChanged(CGUIElement* pElement)
{
    int iMouseSensitivity = (m_pMouseSensitivity->GetScrollPosition()) * 100;

    m_pLabelMouseSensitivityValue->SetText(SString("%i%%", iMouseSensitivity).c_str());
    return true;
}

bool CSettings::OnVerticalAimSensitivityChanged(CGUIElement* pElement)
{
    int iSensitivity = m_pVerticalAimSensitivity->GetScrollPosition() * 100;
    m_pLabelVerticalAimSensitivityValue->SetText(SString("%i%%", iSensitivity));
    return true;
}

bool CSettings::OnMapAlphaChanged(CGUIElement* pElement)
{
    int iAlpha = (m_pMapAlpha->GetScrollPosition()) * 100;

    m_pMapAlphaValueLabel->SetText(SString("%i%%", iAlpha).c_str());
    return true;
}

bool CSettings::OnMasterVolumeChanged(CGUIElement* pElement)
{
    int iVolume = m_pAudioMasterVolume->GetScrollPosition() * 100.0f;
    m_pLabelMasterVolumeValue->SetText(SString("%i%%", iVolume).c_str());

    CVARS_SET("mastervolume", m_pAudioMasterVolume->GetScrollPosition());

    OnRadioVolumeChanged(nullptr);
    OnSFXVolumeChanged(nullptr);
    OnMTAVolumeChanged(nullptr);
    OnVoiceVolumeChanged(nullptr);

    return true;
}

bool CSettings::OnRadioVolumeChanged(CGUIElement* pElement)
{
    int iVolume = m_pAudioRadioVolume->GetScrollPosition() * 100.0f;
    m_pLabelRadioVolumeValue->SetText(SString("%i%%", iVolume).c_str());

    SetRadioVolume(m_pAudioRadioVolume->GetScrollPosition());

    return true;
}

bool CSettings::OnSFXVolumeChanged(CGUIElement* pElement)
{
    int iVolume = m_pAudioSFXVolume->GetScrollPosition() * 100.0f;
    m_pLabelSFXVolumeValue->SetText(SString("%i%%", iVolume).c_str());

    SetSFXVolume(m_pAudioSFXVolume->GetScrollPosition());

    return true;
}

bool CSettings::OnMTAVolumeChanged(CGUIElement* pElement)
{
    int iVolume = m_pAudioMTAVolume->GetScrollPosition() * 100.0f;
    m_pLabelMTAVolumeValue->SetText(SString("%i%%", iVolume).c_str());

    CVARS_SET("mtavolume", m_pAudioMTAVolume->GetScrollPosition());

    return true;
}

bool CSettings::OnMasterMuteMinimizedChanged(CGUIElement* pElement)
{
    bool bSelected = m_pCheckBoxMuteMaster->GetSelected();
    m_pCheckBoxMuteRadio->SetEnabled(!bSelected);
    m_pCheckBoxMuteSFX->SetEnabled(!bSelected);
    m_pCheckBoxMuteMTA->SetEnabled(!bSelected);
    m_pCheckBoxMuteVoice->SetEnabled(!bSelected);
    CVARS_SET("mute_master_when_minimized", bSelected);
    return true;
}

bool CSettings::OnRadioMuteMinimizedChanged(CGUIElement* pElement)
{
    CVARS_SET("mute_radio_when_minimized", m_pCheckBoxMuteRadio->GetSelected());
    return true;
}

bool CSettings::OnSFXMuteMinimizedChanged(CGUIElement* pElement)
{
    CVARS_SET("mute_sfx_when_minimized", m_pCheckBoxMuteSFX->GetSelected());
    return true;
}

bool CSettings::OnMTAMuteMinimizedChanged(CGUIElement* pElement)
{
    CVARS_SET("mute_mta_when_minimized", m_pCheckBoxMuteMTA->GetSelected());
    return true;
}

bool CSettings::OnVoiceMuteMinimizedChanged(CGUIElement* pElement)
{
    CVARS_SET("mute_voice_when_minimized", m_pCheckBoxMuteVoice->GetSelected());
    return true;
}

bool CSettings::OnVoiceVolumeChanged(CGUIElement* pElement)
{
    int iVolume = m_pAudioVoiceVolume->GetScrollPosition() * 100.0f;
    m_pLabelVoiceVolumeValue->SetText(SString("%i%%", iVolume).c_str());

    CVARS_SET("voicevolume", m_pAudioVoiceVolume->GetScrollPosition());

    return true;
}

bool CSettings::OnChatRedChanged(CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast<CGUIScrollBar*>(pElement);
    int            iValue = static_cast<int>(pScrollBar->GetScrollPosition() * 255.0f + 0.5f);

    for (int eType = Chat::ColorType::BG; eType < Chat::ColorType::MAX; ++eType)
    {
        if (pScrollBar == m_pChatRed[eType])
        {
            m_pChatRedValue[eType]->SetText(SString("%i", iValue).c_str());
            UpdateChatColorPreview(static_cast<eChatColorType>(eType));
            return true;
        }
    }

    return false;
}

bool CSettings::OnChatGreenChanged(CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast<CGUIScrollBar*>(pElement);
    int            iValue = static_cast<int>(pScrollBar->GetScrollPosition() * 255.0f + 0.5f);

    for (int eType = Chat::ColorType::BG; eType < Chat::ColorType::MAX; ++eType)
    {
        if (pScrollBar == m_pChatGreen[eType])
        {
            m_pChatGreenValue[eType]->SetText(SString("%i", iValue).c_str());
            UpdateChatColorPreview(static_cast<eChatColorType>(eType));
            return true;
        }
    }

    return false;
}

bool CSettings::OnChatBlueChanged(CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast<CGUIScrollBar*>(pElement);
    int            iValue = static_cast<int>(pScrollBar->GetScrollPosition() * 255.0f + 0.5f);

    for (int eType = Chat::ColorType::BG; eType < Chat::ColorType::MAX; ++eType)
    {
        if (pScrollBar == m_pChatBlue[eType])
        {
            m_pChatBlueValue[eType]->SetText(SString("%i", iValue).c_str());
            UpdateChatColorPreview(static_cast<eChatColorType>(eType));
            return true;
        }
    }

    return false;
}

bool CSettings::OnChatAlphaChanged(CGUIElement* pElement)
{
    CGUIScrollBar* pScrollBar = reinterpret_cast<CGUIScrollBar*>(pElement);
    int            iValue = static_cast<int>(pScrollBar->GetScrollPosition() * 255.0f + 0.5f);

    for (int eType = Chat::ColorType::BG; eType < Chat::ColorType::MAX; ++eType)
    {
        if (pScrollBar == m_pChatAlpha[eType])
        {
            m_pChatAlphaValue[eType]->SetText(SString("%i", iValue).c_str());
            UpdateChatColorPreview(static_cast<eChatColorType>(eType));
            return true;
        }
    }

    return false;
}

bool CSettings::OnUpdateButtonClick(CGUIElement* pElement)
{
    // Update build type
    if (CGUIListItem* pSelected = m_pUpdateBuildTypeCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("update_build_type", iSelected);
    }

    GetVersionUpdater()->InitiateManualCheck();
    return true;
}

bool CSettings::OnCachePathShowButtonClick(CGUIElement* pElement)
{
    SString strFileCachePath = GetCommonRegistryValue("", "File Cache Path");
    if (DirectoryExists(strFileCachePath))
        ShellExecuteNonBlocking("open", strFileCachePath);
    return true;
}

bool CSettings::OnFxQualityChanged(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pComboFxQuality->GetSelectedItem();
    if (!pItem)
        return true;

    if ((int)pItem->GetData() == 0)
    {
        m_pCheckBoxVolumetricShadows->SetSelected(false);
        m_pCheckBoxVolumetricShadows->SetEnabled(false);
        m_pCheckBoxGrass->SetEnabled(false);
    }
    else
    {
        m_pCheckBoxVolumetricShadows->SetEnabled(true);
        m_pCheckBoxGrass->SetEnabled(true);
    }

    // Enable dynamic ped shadows checkbox if visual quality option is set to high or very high
    m_pCheckBoxDynamicPedShadows->SetEnabled((int)pItem->GetData() >= 2);
    return true;
}

void VolumetricShadowsCallBack(void* ptr, unsigned int uiButton)
{
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();
    if (uiButton == 0)
        ((CGUICheckBox*)ptr)->SetSelected(false);
}

bool CSettings::OnVolumetricShadowsClick(CGUIElement* pElement)
{
    if (m_pCheckBoxVolumetricShadows->GetSelected() && !m_bShownVolumetricShadowsWarning)
    {
        m_bShownVolumetricShadowsWarning = true;
        SStringX strMessage(
            _("Volmetric shadows can cause some systems to slow down."
              "\n\nAre you sure you want to enable them?"));
        CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
        pQuestionBox->Reset();
        pQuestionBox->SetTitle(_("PERFORMANCE WARNING"));
        pQuestionBox->SetMessage(strMessage);
        pQuestionBox->SetButton(0, _("No"));
        pQuestionBox->SetButton(1, _("Yes"));
        pQuestionBox->SetCallback(VolumetricShadowsCallBack, m_pCheckBoxVolumetricShadows);
        pQuestionBox->Show();
    }
    return true;
}

//
// AllowScreenUpload
//
bool CSettings::OnAllowScreenUploadClick(CGUIElement* pElement)
{
    if (!m_pCheckBoxAllowScreenUpload->GetSelected() && !m_bShownAllowScreenUploadMessage)
    {
        m_bShownAllowScreenUploadMessage = true;
        SString strMessage;
        strMessage +=
            _("Screen upload is required by some servers for anti-cheat purposes."
              "\n\n(The chat box and GUI is excluded from the upload)\n");
        CCore::GetSingleton().ShowMessageBox(_("SCREEN UPLOAD INFORMATION"), strMessage, MB_BUTTON_OK | MB_ICON_INFO);
    }
    return true;
}

//
// AllowExternalSounds
//
bool CSettings::OnAllowExternalSoundsClick(CGUIElement* pElement)
{
    if (!m_pCheckBoxAllowExternalSounds->GetSelected() && !m_bShownAllowExternalSoundsMessage)
    {
        m_bShownAllowExternalSoundsMessage = true;
        SString strMessage;
        strMessage +=
            _("Some scripts may play sounds, such as radio, from the internet."
              "\n\nDisabling this setting may decrease network"
              "\nbandwidth consumption.\n");
        CCore::GetSingleton().ShowMessageBox(_("EXTERNAL SOUNDS"), strMessage, MB_BUTTON_OK | MB_ICON_INFO);
    }
    return true;
}

//
// DiscordRPC
//
bool CSettings::OnAllowDiscordRPC(CGUIElement* pElement)
{
    bool isEnabled = m_pCheckBoxAllowDiscordRPC->GetSelected();
    g_pCore->GetDiscord()->SetDiscordRPCEnabled(isEnabled);

    if (isEnabled)
        ShowRichPresenceShareDataQuestionBox(); // show question box

    return true;
}

static void ShowRichPresenceShareDataCallback(void* ptr, unsigned int uiButton)
{
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();

    CVARS_SET("discord_rpc_share_data", static_cast<bool>(uiButton));
}

void CSettings::ShowRichPresenceShareDataQuestionBox() const
{
    SStringX strMessage(
        _("It seems that you have the Rich Presence connection option enabled."
          "\nDo you want to allow servers to share their data?"
          "\n\nThis includes yours unique ID identifier."));
    CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
    pQuestionBox->Reset();
    pQuestionBox->SetTitle(_("CONSENT TO ALLOW DATA SHARING"));
    pQuestionBox->SetMessage(strMessage);
    pQuestionBox->SetButton(0, _("No"));
    pQuestionBox->SetButton(1, _("Yes"));
    pQuestionBox->SetCallback(ShowRichPresenceShareDataCallback);
    pQuestionBox->Show();
}

//
// CustomizedSAFiles
//
void CustomizedSAFilesCallBack(void* ptr, unsigned int uiButton)
{
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();
    if (uiButton == 0)
        ((CGUICheckBox*)ptr)->SetSelected(false);
}

bool CSettings::OnCustomizedSAFilesClick(CGUIElement* pElement)
{
    if (m_pCheckBoxCustomizedSAFiles->GetSelected())
    {
        SString strMessage;
        strMessage +=
            _("Some files in your GTA:SA data directory are customized."
              "\nMTA will only use these modified files if this check box is ticked."
              "\n\nHowever, CUSTOMIZED GTA:SA FILES ARE BLOCKED BY MANY SERVERS"
              "\n\nAre you sure you want to use them?");
        CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
        pQuestionBox->Reset();
        pQuestionBox->SetTitle(_("CUSTOMIZED GTA:SA FILES"));
        pQuestionBox->SetMessage(strMessage);
        pQuestionBox->SetButton(0, _("No"));
        pQuestionBox->SetButton(1, _("Yes"));
        pQuestionBox->SetCallback(CustomizedSAFilesCallBack, m_pCheckBoxCustomizedSAFiles);
        pQuestionBox->Show();
    }
    return true;
}

//
// ShowUnsafeResolutionsClick
//
bool CSettings::ShowUnsafeResolutionsClick(CGUIElement* pElement)
{
    // Change list of available resolutions
    PopulateResolutionComboBox();
    return true;
}

//
// OnWindowedClick
//
bool CSettings::OnWindowedClick(CGUIElement* pElement)
{
    UpdateFullScreenComboBoxEnabled();
    return true;
}

//
// OnDPIAwareClick
//
static void DPIAwareQuestionCallBack(void* userdata, unsigned int uiButton);

bool CSettings::OnDPIAwareClick(CGUIElement* pElement)
{
    static bool shownWarning = false;

    if (m_pCheckBoxDPIAware->GetSelected() && !shownWarning)
    {
        shownWarning = true;

        SStringX strMessage(
            _("Enabling DPI awareness is an experimental feature and\n"
              "we only recommend it when you play MTA:SA on a scaled monitor.\n"
              "You may experience graphical issues if you enable this option."
              "\n\nAre you sure you want to enable this option?"));
        CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
        pQuestionBox->Reset();
        pQuestionBox->SetTitle(_("EXPERIMENTAL FEATURE"));
        pQuestionBox->SetMessage(strMessage);
        pQuestionBox->SetButton(0, _("No"));
        pQuestionBox->SetButton(1, _("Yes"));
        pQuestionBox->SetCallback(DPIAwareQuestionCallBack, m_pCheckBoxDPIAware);
        pQuestionBox->Show();
    }

    return true;
}

static void DPIAwareQuestionCallBack(void* userdata, unsigned int uiButton)
{
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();

    if (uiButton == 0)
    {
        auto const checkBox = reinterpret_cast<CGUICheckBox*>(userdata);
        checkBox->SetSelected(false);
    }
}

bool CSettings::OnBrowserBlacklistAdd(CGUIElement* pElement)
{
    SString strDomain = m_pEditBrowserBlacklistAdd->GetText();
    if (!strDomain.empty())
    {
        bool bExists = false;
        for (int i = 0; i < m_pGridBrowserBlacklist->GetRowCount(); ++i)
        {
            if (m_pGridBrowserBlacklist->GetItemText(i, 1) == strDomain)
            {
                bExists = true;
                break;
            }
        }

        if (!bExists)
        {
            m_pGridBrowserBlacklist->SetItemText(m_pGridBrowserBlacklist->AddRow(), 1, strDomain);
            m_bBrowserListsChanged = true;
        }
    }
    return true;
}

bool CSettings::OnBrowserBlacklistRemove(CGUIElement* pElement)
{
    int iSelectedRow = m_pGridBrowserBlacklist->GetSelectedItemRow();
    if (iSelectedRow > -1)
    {
        m_pGridBrowserBlacklist->RemoveRow(iSelectedRow);
        m_bBrowserListsChanged = true;
    }

    return true;
}

bool CSettings::OnBrowserBlacklistDomainAddFocused(CGUIElement* pElement)
{
    m_pLabelBrowserBlacklistAdd->SetVisible(false);
    return true;
}

bool CSettings::OnBrowserBlacklistDomainAddDefocused(CGUIElement* pElement)
{
    if (m_pEditBrowserBlacklistAdd->GetText() == "")
        m_pLabelBrowserBlacklistAdd->SetVisible(true);
    return true;
}

bool CSettings::OnBrowserWhitelistAdd(CGUIElement* pElement)
{
    SString strDomain = m_pEditBrowserWhitelistAdd->GetText();
    if (!strDomain.empty())
    {
        bool bExists = false;
        for (int i = 0; i < m_pGridBrowserWhitelist->GetRowCount(); ++i)
        {
            if (m_pGridBrowserWhitelist->GetItemText(i, 1) == strDomain)
            {
                bExists = true;
                break;
            }
        }

        if (!bExists)
        {
            m_pGridBrowserWhitelist->SetItemText(m_pGridBrowserWhitelist->AddRow(), 1, strDomain);
            m_bBrowserListsChanged = true;
        }
    }
    return true;
}

bool CSettings::OnBrowserWhitelistRemove(CGUIElement* pElement)
{
    int iSelectedRow = m_pGridBrowserWhitelist->GetSelectedItemRow();
    if (iSelectedRow > -1)
    {
        m_pGridBrowserWhitelist->RemoveRow(iSelectedRow);
        m_bBrowserListsChanged = true;
    }

    return true;
}

bool CSettings::OnBrowserWhitelistDomainAddFocused(CGUIElement* pElement)
{
    m_pLabelBrowserWhitelistAdd->SetVisible(false);
    return true;
}

bool CSettings::OnBrowserWhitelistDomainAddDefocused(CGUIElement* pElement)
{
    if (m_pEditBrowserWhitelistAdd->GetText() == "")
        m_pLabelBrowserWhitelistAdd->SetVisible(true);
    return true;
}

void NewNicknameCallback(void* ptr, unsigned int uiButton, std::string strNick)
{
    if (uiButton == 1)            // We hit OK
    {
        if (!CCore::GetSingleton().IsValidNick(strNick.c_str()))
            CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC81"), _("Your nickname contains invalid characters!"), MB_BUTTON_OK | MB_ICON_INFO);
        else
        {
            CVARS_SET("nick", strNick);
            CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();
        }
    }
    else
        CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();
}

void CSettings::RequestNewNickname()
{
    std::string strNick;
    CVARS_GET("nick", strNick);

    CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
    pQuestionBox->Reset();
    pQuestionBox->SetTitle(_("Please enter a nickname"));
    pQuestionBox->SetMessage(_("Please enter a nickname to be used ingame.  \nThis will be your name when you connect to and play in a server"));
    pQuestionBox->SetButton(0, _("Cancel"));
    pQuestionBox->SetButton(1, _("OK"));
    pQuestionBox->SetEditbox(0, strNick)->SetMaxLength(MAX_PLAYER_NICK_LENGTH);
    pQuestionBox->SetCallbackEdit(NewNicknameCallback);
    pQuestionBox->Show();
}

bool CSettings::OnShowAdvancedSettingDescription(CGUIElement* pElement)
{
    CGUILabel*     pLabel = dynamic_cast<CGUILabel*>(pElement);
    CGUIComboBox*  pComboBox = dynamic_cast<CGUIComboBox*>(pElement);
    CGUIScrollBar* pScrollBar = dynamic_cast<CGUIScrollBar*>(pElement);
    CGUICheckBox*  pCheckBox = dynamic_cast<CGUICheckBox*>(pElement);

    std::string strText = "";

    if (pLabel && pLabel == m_pPriorityLabel || pComboBox && pComboBox == m_pPriorityCombo)
        strText = std::string(_("Process priority:")) + " " + std::string(_("Very experimental feature."));
    else if (pLabel && pLabel == m_pFastClothesLabel || pComboBox && pComboBox == m_pFastClothesCombo)
        strText = std::string(_("Fast CJ clothes loading:")) + " " + std::string(_("Stops stalls with CJ variations (Uses 65MB more RAM)"));
    else if (pLabel && pLabel == m_pBrowserSpeedLabel || pComboBox && pComboBox == m_pBrowserSpeedCombo)
        strText = std::string(_("Browser speed:")) + " " + std::string(_("Older routers may require a slower scan speed."));
    else if (pLabel && pLabel == m_pSingleDownloadLabel || pComboBox && pComboBox == m_pSingleDownloadCombo)
        strText = std::string(_("Single connection:")) + " " + std::string(_("Switch on to use only one connection when downloading."));
    else if (pLabel && pLabel == m_pPacketTagLabel || pComboBox && pComboBox == m_pPacketTagCombo)
        strText = std::string(_("Packet tag:")) + " " + std::string(_("Tag network packets to help ISPs identify MTA traffic."));
    else if (pLabel && pLabel == m_pProgressAnimationLabel || pComboBox && pComboBox == m_pProgressAnimationCombo)
        strText = std::string(_("Progress animation:")) + " " + std::string(_("Spinning circle animation at the bottom of the screen"));
    else if (pLabel && pLabel == m_pDebugSettingLabel || pComboBox && pComboBox == m_pDebugSettingCombo)
        strText = std::string(_("Debug setting:")) + " " + std::string(_("Select default always. (This setting is not saved)"));
    else if (pLabel && pLabel == m_pStreamingMemoryLabel || pScrollBar && pScrollBar == m_pStreamingMemory)
        strText = std::string(_("Streaming memory:")) + " " + std::string(_("Maximum is usually best"));
    else if (pLabel && pLabel == m_pUpdateBuildTypeLabel || pComboBox && pComboBox == m_pUpdateBuildTypeCombo)
        strText = std::string(_("Auto updater:")) + " " + std::string(_("Select default unless you like filling out bug reports."));
    else if (pLabel && pLabel == m_pUpdateAutoInstallLabel || pComboBox && pComboBox == m_pUpdateAutoInstallCombo)
        strText = std::string(_("Auto updater:")) + " " + std::string(_("Select default to automatically install important updates."));
    else if (pCheckBox && pCheckBox == m_pWin8ColorCheckBox)
        strText = std::string(_("16-bit color:")) + " " + std::string(_("Enable 16 bit color modes - Requires MTA restart"));
    else if (pCheckBox && pCheckBox == m_pWin8MouseCheckBox)
        strText = std::string(_("Mouse fix:")) + " " + std::string(_("Mouse movement fix - May need PC restart"));

    if (strText != "")
        m_pAdvancedSettingDescriptionLabel->SetText(strText.c_str());

    return true;
}

bool CSettings::OnHideAdvancedSettingDescription(CGUIElement* pElement)
{
    m_pAdvancedSettingDescriptionLabel->SetText("");

    return true;
}

void CSettings::SetSelectedIndex(unsigned int uiIndex)
{
    unsigned int uiTabCount = m_pTabs->GetTabCount();

    if (uiIndex < uiTabCount)
    {
        m_pTabs->SetSelectedIndex(uiIndex);
    }
}

void CSettings::TabSkip(bool bBackwards)
{
    unsigned int uiTabCount = m_pTabs->GetTabCount();

    if (bBackwards)
    {
        unsigned int uiIndex = m_pTabs->GetSelectedIndex() - 1;

        if (m_pTabs->GetSelectedIndex() == 0)
        {
            uiIndex = uiTabCount - 1;
        }

        SetSelectedIndex(uiIndex);
    }
    else
    {
        unsigned int uiIndex = m_pTabs->GetSelectedIndex() + 1;
        unsigned int uiNewIndex = uiIndex % uiTabCount;

        SetSelectedIndex(uiNewIndex);
    }
}

bool CSettings::IsActive()
{
    return m_pWindow->IsActive();
}
