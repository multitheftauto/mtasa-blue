#include <StdInc.h>
#include "CVideoSettingsTab.h"

CVideoSettingsTab::CVideoSettingsTab(CGUITab* pTab) :
    m_pTab(pTab)
{
    CGUI *pManager = g_pCore->GetGUI();
    /**
    *  Video tab
    **/
    float fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
        _("Resolution:"),
        _("FOV:"),
        _("Draw Distance:"),
        _("Brightness:"),
        _("FX Quality:"),
        _("Anisotropic filtering:"),
        _("Anti-aliasing:"),
        _("Aspect Ratio:"),
        _("Opacity:")
    );

    CVector2D vecTemp, vecSize;

    m_bShownVolumetricShadowsWarning = false;

    m_iMaxAnisotropic = g_pDeviceState->AdapterState.MaxAnisotropicSetting;

    m_pVideoGeneralLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("General")));
    m_pVideoGeneralLabel->SetPosition(CVector2D(11, 13));
    m_pVideoGeneralLabel->GetPosition(vecTemp, false);
    m_pVideoGeneralLabel->AutoSize(NULL, 3.0f);
    m_pVideoGeneralLabel->SetFont("default-bold-small");

    m_pVideoResolutionLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Resolution:")));
    m_pVideoResolutionLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 26.0f));
    m_pVideoResolutionLabel->GetPosition(vecTemp, false);
    m_pVideoResolutionLabel->AutoSize();

    m_pComboResolution = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pComboResolution->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 3.0f));
    m_pComboResolution->GetPosition(vecTemp, false);
    m_pComboResolution->SetSize(CVector2D(200.0f, 160.0f));
    m_pComboResolution->GetSize(vecSize);
    m_pComboResolution->SetReadOnly(true);

    m_pCheckBoxWindowed = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Windowed"), true));
    m_pCheckBoxWindowed->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY + 3.0f));
    m_pCheckBoxWindowed->AutoSize(NULL, 20.0f);

    m_pVideoResolutionLabel->GetPosition(vecTemp, false); // Restore our label position

                                                          // Fullscreen mode
    vecTemp.fY += 26;
    m_pFullscreenStyleLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Fullscreen mode:")));
    m_pFullscreenStyleLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pFullscreenStyleLabel->AutoSize();

    m_pFullscreenStyleCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pFullscreenStyleCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pFullscreenStyleCombo->SetSize(CVector2D(200, 95.0f));
    m_pFullscreenStyleCombo->AddItem(_("Standard"))->SetData((void*)FULLSCREEN_STANDARD);
    m_pFullscreenStyleCombo->AddItem(_("Borderless window"))->SetData((void*)FULLSCREEN_BORDERLESS);
    m_pFullscreenStyleCombo->AddItem(_("Borderless keep res"))->SetData((void*)FULLSCREEN_BORDERLESS_KEEP_RES);
    m_pFullscreenStyleCombo->SetReadOnly(true);
    vecTemp.fY += 4;

    m_pCheckBoxMipMapping = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Mip Mapping"), true));
#ifndef MIP_MAPPING_SETTING_APPEARS_TO_DO_SOMETHING
    m_pCheckBoxMipMapping->SetPosition(CVector2D(vecTemp.fX + 340.0f, vecTemp.fY + 45.0f));
    m_pCheckBoxMipMapping->SetSize(CVector2D(224.0f, 16.0f));
    m_pCheckBoxMipMapping->SetVisible(false);
#endif

    vecTemp.fY -= 5;
    m_pFieldOfViewLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("FOV:")));
    m_pFieldOfViewLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pFieldOfViewLabel->GetPosition(vecTemp, false);
    m_pFieldOfViewLabel->AutoSize();

    m_pFieldOfView = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pFieldOfView->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pFieldOfView->GetPosition(vecTemp, false);
    m_pFieldOfView->SetSize(CVector2D(160.0f, 20.0f));
    m_pFieldOfView->GetSize(vecSize);

    m_pFieldOfViewValueLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "70"));
    m_pFieldOfViewValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pFieldOfViewValueLabel->AutoSize("70 ");

    vecTemp.fX = 11;
    m_pDrawDistanceLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Draw Distance:")));
    m_pDrawDistanceLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pDrawDistanceLabel->GetPosition(vecTemp, false);
    m_pDrawDistanceLabel->AutoSize();

    m_pDrawDistance = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pDrawDistance->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pDrawDistance->GetPosition(vecTemp, false);
    m_pDrawDistance->SetSize(CVector2D(160.0f, 20.0f));
    m_pDrawDistance->GetSize(vecSize);
    m_pDrawDistance->SetProperty("StepSize", "0.01");

    m_pDrawDistanceValueLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pDrawDistanceValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pDrawDistanceValueLabel->AutoSize("100% ");

    vecTemp.fX = 11;

    m_pBrightnessLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Brightness:")));
    m_pBrightnessLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pBrightnessLabel->GetPosition(vecTemp, false);
    m_pBrightnessLabel->AutoSize();

    m_pBrightness = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pBrightness->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pBrightness->GetPosition(vecTemp, false);
    m_pBrightness->SetSize(CVector2D(160.0f, 20.0f));
    m_pBrightness->GetSize(vecSize);
    m_pBrightness->SetProperty("StepSize", "0.01");

    m_pBrightnessValueLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pBrightnessValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pBrightnessValueLabel->AutoSize("100% ");

    vecTemp.fX = 11;

    m_pFXQualityLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("FX Quality:")));
    m_pFXQualityLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pFXQualityLabel->GetPosition(vecTemp, false);
    m_pFXQualityLabel->AutoSize();

    m_pComboFxQuality = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pComboFxQuality->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pComboFxQuality->SetSize(CVector2D(200.0f, 95.0f));
    m_pComboFxQuality->AddItem(_("Low"))->SetData((void*)0);
    m_pComboFxQuality->AddItem(_("Medium"))->SetData((void*)1);
    m_pComboFxQuality->AddItem(_("High"))->SetData((void*)2);
    m_pComboFxQuality->AddItem(_("Very high"))->SetData((void*)3);
    m_pComboFxQuality->SetReadOnly(true);

    m_pAnisotropicLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Anisotropic filtering:")));
    m_pAnisotropicLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pAnisotropicLabel->GetPosition(vecTemp, false);
    m_pAnisotropicLabel->AutoSize();

    m_pAnisotropic = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pAnisotropic->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAnisotropic->GetPosition(vecTemp, false);
    m_pAnisotropic->SetSize(CVector2D(160.0f, 20.0f));
    m_pAnisotropic->GetSize(vecSize);
    m_pAnisotropic->SetProperty("StepSize", SString("%1.2f", 1 / (float)m_iMaxAnisotropic));

    m_pAnisotropicValueLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Off")));
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

    m_pAntiAliasingLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Anti-aliasing:")));
    m_pAntiAliasingLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pAntiAliasingLabel->GetPosition(vecTemp, false);
    m_pAntiAliasingLabel->AutoSize();

    m_pComboAntiAliasing = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pComboAntiAliasing->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pComboAntiAliasing->SetSize(CVector2D(200.0f, 95.0f));
    m_pComboAntiAliasing->AddItem(_("Off"))->SetData((void*)1);
    m_pComboAntiAliasing->AddItem(_("1x"))->SetData((void*)2);
    m_pComboAntiAliasing->AddItem(_("2x"))->SetData((void*)3);
    m_pComboAntiAliasing->AddItem(_("3x"))->SetData((void*)4);
    m_pComboAntiAliasing->SetReadOnly(true);


    m_pAspectRatioLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Aspect Ratio:")));
    m_pAspectRatioLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pAspectRatioLabel->GetPosition(vecTemp, false);
    m_pAspectRatioLabel->AutoSize();

    m_pComboAspectRatio = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pComboAspectRatio->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY - 1.0f));
    m_pComboAspectRatio->GetPosition(vecTemp, false);
    m_pComboAspectRatio->SetSize(CVector2D(200.0f, 95.0f));
    m_pComboAspectRatio->GetSize(vecSize);
    m_pComboAspectRatio->AddItem(_("Auto"))->SetData((void*)ASPECT_RATIO_AUTO);
    m_pComboAspectRatio->AddItem(_("4:3"))->SetData((void*)ASPECT_RATIO_4_3);
    m_pComboAspectRatio->AddItem(_("16:10"))->SetData((void*)ASPECT_RATIO_16_10);
    m_pComboAspectRatio->AddItem(_("16:9"))->SetData((void*)ASPECT_RATIO_16_9);
    m_pComboAspectRatio->SetReadOnly(true);

    m_pCheckBoxHudMatchAspectRatio = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("HUD Match Aspect Ratio"), true));
    m_pCheckBoxHudMatchAspectRatio->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY + 3.0f));
    m_pCheckBoxHudMatchAspectRatio->AutoSize(NULL, 20.0f);

    vecTemp.fX = 11;

    m_pCheckBoxVolumetricShadows = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Volumetric Shadows"), true));
    m_pCheckBoxVolumetricShadows->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pCheckBoxVolumetricShadows->AutoSize(NULL, 20.0f);

    m_pCheckBoxGrass = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Grass effect"), true));
    m_pCheckBoxGrass->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 50.0f));
    m_pCheckBoxGrass->AutoSize(NULL, 20.0f);

    m_pCheckBoxHeatHaze = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Heat haze"), true));
    m_pCheckBoxHeatHaze->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 70.0f));
    m_pCheckBoxHeatHaze->AutoSize(NULL, 20.0f);

    m_pCheckBoxTyreSmokeParticles = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Tyre Smoke etc"), true));
    m_pCheckBoxTyreSmokeParticles->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 90.0f));
    m_pCheckBoxTyreSmokeParticles->AutoSize(NULL, 20.0f);

    float fPosY = vecTemp.fY;
    m_pCheckBoxMinimize = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Full Screen Minimize"), true));
    m_pCheckBoxMinimize->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 30.0f));
    m_pCheckBoxMinimize->AutoSize(NULL, 20.0f);

#ifndef SHOWALLSETTINGS
    if (!GetVideoModeManager()->IsMultiMonitor())
    {
        m_pCheckBoxMinimize->SetVisible(false);
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxDeviceSelectionDialog = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Enable Device Selection Dialog"), true));
    m_pCheckBoxDeviceSelectionDialog->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 50.0f));
    m_pCheckBoxDeviceSelectionDialog->AutoSize(NULL, 20.0f);

#ifndef SHOWALLSETTINGS
    if (!GetVideoModeManager()->IsMultiMonitor())
    {
        m_pCheckBoxDeviceSelectionDialog->SetVisible(false);
        fPosY -= 20.0f;
    }
#endif

    m_pCheckBoxShowUnsafeResolutions = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Show unsafe resolutions"), true));
    m_pCheckBoxShowUnsafeResolutions->SetPosition(CVector2D(vecTemp.fX + 245.0f, fPosY + 70.0f));
    m_pCheckBoxShowUnsafeResolutions->AutoSize(NULL, 20.0f);

#ifndef SHOWALLSETTINGS
    if (!CCore::GetSingleton().GetGame()->GetSettings()->HasUnsafeResolutions())
    {
        m_pCheckBoxShowUnsafeResolutions->SetVisible(false);
        fPosY -= 20.0f;
    }
#endif
    vecTemp.fY += 10;

    m_pTab->GetParent()->GetSize(vecTemp);

    m_pVideoDefButton = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Load defaults")));
    m_pVideoDefButton->SetClickHandler(GUI_CALLBACK(&CVideoSettingsTab::OnVideoDefaultClick, this));
    m_pVideoDefButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pVideoDefButton->GetSize(vecSize);
    m_pVideoDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    m_pVideoDefButton->SetZOrderingEnabled(false);

    m_pFieldOfView->SetOnScrollHandler(GUI_CALLBACK(&CVideoSettingsTab::OnFieldOfViewChanged, this));
    m_pDrawDistance->SetOnScrollHandler(GUI_CALLBACK(&CVideoSettingsTab::OnDrawDistanceChanged, this));
    m_pBrightness->SetOnScrollHandler(GUI_CALLBACK(&CVideoSettingsTab::OnBrightnessChanged, this));
    m_pAnisotropic->SetOnScrollHandler(GUI_CALLBACK(&CVideoSettingsTab::OnAnisotropicChanged, this));
    m_pComboFxQuality->SetSelectionHandler(GUI_CALLBACK(&CVideoSettingsTab::OnFxQualityChanged, this));
    m_pCheckBoxVolumetricShadows->SetClickHandler(GUI_CALLBACK(&CVideoSettingsTab::OnVolumetricShadowsClick, this));

    m_pCheckBoxWindowed->SetClickHandler(GUI_CALLBACK(&CVideoSettingsTab::OnWindowedClick, this));
    m_pCheckBoxShowUnsafeResolutions->SetClickHandler(GUI_CALLBACK(&CVideoSettingsTab::ShowUnsafeResolutionsClick, this));
}


void CVideoSettingsTab::UpdateVideoTab(void)
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    bool bNextWindowed;
    bool bNextFSMinimize;
    int iNextVidMode;
    int iNextFullscreenStyle;
    GetVideoModeManager()->GetNextVideoMode(iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle);

    m_pCheckBoxMipMapping->SetSelected(gameSettings->IsMipMappingEnabled());
    m_pCheckBoxWindowed->SetSelected(bNextWindowed);
    m_pCheckBoxMinimize->SetSelected(bNextFSMinimize);
    m_pDrawDistance->SetScrollPosition((gameSettings->GetDrawDistance() - 0.925f) / 0.8749f);
    m_pBrightness->SetScrollPosition((float)gameSettings->GetBrightness() / 384);

    // FieldOfView
    int iFieldOfView;
    CVARS_GET("fov", iFieldOfView);
    m_pFieldOfView->SetScrollPosition((iFieldOfView - 70) / 20.f);

    // Anisotropic filtering
    int iAnisotropic;
    CVARS_GET("anisotropic", iAnisotropic);
    m_pAnisotropic->SetScrollPosition(iAnisotropic / (float)m_iMaxAnisotropic);

    int FxQuality = gameSettings->GetFXQuality();
    if (FxQuality == 0) m_pComboFxQuality->SetText(_("Low"));
    else if (FxQuality == 1) m_pComboFxQuality->SetText(_("Medium"));
    else if (FxQuality == 2) m_pComboFxQuality->SetText(_("High"));
    else if (FxQuality == 3) m_pComboFxQuality->SetText(_("Very high"));

    char AntiAliasing = gameSettings->GetAntiAliasing();
    if (AntiAliasing == 1) m_pComboAntiAliasing->SetText(_("Off"));
    else if (AntiAliasing == 2) m_pComboAntiAliasing->SetText(_("1x"));
    else if (AntiAliasing == 3) m_pComboAntiAliasing->SetText(_("2x"));
    else if (AntiAliasing == 4) m_pComboAntiAliasing->SetText(_("3x"));

    // Aspect ratio
    int aspectRatio;
    CVARS_GET("aspect_ratio", aspectRatio);
    if (aspectRatio == ASPECT_RATIO_AUTO) m_pComboAspectRatio->SetText(_("Auto"));
    else if (aspectRatio == ASPECT_RATIO_4_3) m_pComboAspectRatio->SetText(_("4:3"));
    else if (aspectRatio == ASPECT_RATIO_16_10) m_pComboAspectRatio->SetText(_("16:10"));
    else if (aspectRatio == ASPECT_RATIO_16_9) m_pComboAspectRatio->SetText(_("16:9"));

    // HUD match aspect ratio
    m_pCheckBoxHudMatchAspectRatio->SetSelected(CVARS_GET_VALUE < bool >("hud_match_aspect_ratio"));

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

    PopulateResolutionComboBox();

    // Fullscreen style
    if (iNextFullscreenStyle == FULLSCREEN_STANDARD) m_pFullscreenStyleCombo->SetText(_("Standard"));
    else if (iNextFullscreenStyle == FULLSCREEN_BORDERLESS) m_pFullscreenStyleCombo->SetText(_("Borderless window"));
    else if (iNextFullscreenStyle == FULLSCREEN_BORDERLESS_KEEP_RES) m_pFullscreenStyleCombo->SetText(_("Borderless keep res"));

    UpdateFullScreenComboBoxEnabled();


}


//
// PopulateResolutionComboBox
//
void CVideoSettingsTab::PopulateResolutionComboBox(void)
{
    bool bNextWindowed;
    bool bNextFSMinimize;
    int iNextVidMode;
    int iNextFullscreenStyle;
    GetVideoModeManager()->GetNextVideoMode(iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle);
    bool bShowUnsafeResolutions = m_pCheckBoxShowUnsafeResolutions->GetSelected();

    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    VideoMode           vidModemInfo;
    int                 vidMode, numVidModes;

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
void CVideoSettingsTab::UpdateFullScreenComboBoxEnabled(void)
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


bool CVideoSettingsTab::OnFxQualityChanged(CGUIElement* pElement)
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

    return true;
}

void VolumetricShadowsCallBack(void* ptr, unsigned int uiButton)
{
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->Reset();
    if (uiButton == 0)
        ((CGUICheckBox*)ptr)->SetSelected(false);
}

bool CVideoSettingsTab::OnVolumetricShadowsClick(CGUIElement* pElement)
{
    if (m_pCheckBoxVolumetricShadows->GetSelected() && !m_bShownVolumetricShadowsWarning)
    {
        m_bShownVolumetricShadowsWarning = true;
        SStringX strMessage(_("Volmetric shadows can cause some systems to slow down."
            "\n\nAre you sure you want to enable them?"));
        CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
        pQuestionBox->Reset();
        pQuestionBox->SetTitle(_("PERFORMANCE WARNING"));
        pQuestionBox->SetMessage(strMessage);
        pQuestionBox->SetButton(0, _("No"));
        pQuestionBox->SetButton(1, _("Yes"));
        pQuestionBox->SetCallback(VolumetricShadowsCallBack, m_pCheckBoxVolumetricShadows.get());
        pQuestionBox->Show();
    }
    return true;
}

//
// ShowUnsafeResolutionsClick
//
bool CVideoSettingsTab::ShowUnsafeResolutionsClick(CGUIElement* pElement)
{
    // Change list of available resolutions
    PopulateResolutionComboBox();
    return true;
}

//
// OnWindowedClick
//
bool CVideoSettingsTab::OnWindowedClick(CGUIElement* pElement)
{
    UpdateFullScreenComboBoxEnabled();
    return true;
}

//
// Called when the user clicks on the video 'Load Defaults' button.
//
bool CVideoSettingsTab::OnVideoDefaultClick(CGUIElement* pElement)
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();


    //gameSettings->SetMipMappingEnabled (); // Doesn't appear to even be enabled
    gameSettings->SetFieldOfView(70);
    gameSettings->SetDrawDistance(1.19625f); // All values taken from a default SA install, no gta_sa.set or coreconfig.xml modifications.
    gameSettings->SetBrightness(253);
    gameSettings->SetFXQuality(2);
    gameSettings->SetAntiAliasing(1, true);
    CVARS_SET("aspect_ratio", ASPECT_RATIO_AUTO);
    CVARS_SET("fov", 70);
    CVARS_SET("anisotropic", 0);
    CVARS_SET("volumetric_shadows", false);
    CVARS_SET("grass", true);
    CVARS_SET("heat_haze", true);
    CVARS_SET("tyre_smoke_enabled", true);

    // change
    bool bIsVideoModeChanged = GetVideoModeManager()->SetVideoMode(0, false, false, FULLSCREEN_STANDARD);

    CVARS_SET("streaming_memory", g_pCore->GetMaxStreamingMemory());

    CVARS_SET("mapalpha", 140.25f);

    // Display restart required message if required
    bool bIsAntiAliasingChanged = gameSettings->GetAntiAliasing() != m_pComboAntiAliasing->GetSelectedItemIndex();
    m_bRequiresRestart = bIsVideoModeChanged || bIsAntiAliasingChanged;

    // Update the GUI
    UpdateVideoTab();

    return true;
}


bool CVideoSettingsTab::OnFieldOfViewChanged(CGUIElement* pElement)
{
    int iFieldOfView = Min < int >(4, (m_pFieldOfView->GetScrollPosition()) * (4 + 1)) * 5 + 70;

    m_pFieldOfViewValueLabel->SetText(SString("%i", iFieldOfView).c_str());
    return true;
}

bool CVideoSettingsTab::OnDrawDistanceChanged(CGUIElement* pElement)
{
    int iDrawDistance = (m_pDrawDistance->GetScrollPosition()) * 100;

    m_pDrawDistanceValueLabel->SetText(SString("%i%%", iDrawDistance).c_str());
    return true;
}

bool CVideoSettingsTab::OnBrightnessChanged(CGUIElement* pElement)
{
    int iBrightness = (m_pBrightness->GetScrollPosition()) * 100;

    m_pBrightnessValueLabel->SetText(SString("%i%%", iBrightness).c_str());
    return true;
}

bool CVideoSettingsTab::OnAnisotropicChanged(CGUIElement* pElement)
{
    int iAnisotropic = Min < int >(m_iMaxAnisotropic, (m_pAnisotropic->GetScrollPosition()) * (m_iMaxAnisotropic + 1));

    SString strLabel;
    if (iAnisotropic > 0)
        strLabel = SString("%ix", 1 << iAnisotropic);
    else
        strLabel = _("Off");

    m_pAnisotropicValueLabel->SetText(strLabel);
    return true;
}

void CVideoSettingsTab::SaveData(void)
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    // Video
    // get current
    int iNextVidMode;
    bool bNextWindowed;
    bool bNextFSMinimize;
    int iNextFullscreenStyle;
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


    // change
    bool bIsVideoModeChanged = GetVideoModeManager()->SetVideoMode(iNextVidMode, bNextWindowed, bNextFSMinimize, iNextFullscreenStyle);
    bool bIsAntiAliasingChanged = gameSettings->GetAntiAliasing() != iAntiAliasing;

    gameSettings->SetAntiAliasing(iAntiAliasing, true);
    gameSettings->SetDrawDistance((m_pDrawDistance->GetScrollPosition() * 0.875f) + 0.925f);
    gameSettings->SetBrightness(m_pBrightness->GetScrollPosition() * 384);
    
    gameSettings->SetMipMappingEnabled(m_pCheckBoxMipMapping->GetSelected());


    // iFieldOfView
    int iFieldOfView = Min < int >(4, (m_pFieldOfView->GetScrollPosition()) * (4 + 1)) * 5 + 70;
    CVARS_SET("fov", iFieldOfView);

    // Anisotropic filtering
    int iAnisotropic = Min < int >(m_iMaxAnisotropic, (m_pAnisotropic->GetScrollPosition()) * (m_iMaxAnisotropic + 1));
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
    gameSettings->SetAspectRatio((eAspectRatio)CVARS_GET_VALUE < int >("aspect_ratio"), bHudMatchAspectRatio);

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

    m_bRequiresRestart = bIsVideoModeChanged || bIsAntiAliasingChanged;
}

void CVideoSettingsTab::LoadData()
{
    UpdateVideoTab();
}
