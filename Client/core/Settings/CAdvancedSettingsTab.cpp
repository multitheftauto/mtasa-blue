#include <StdInc.h>
#include "CAdvancedSettingsTab.h"

CAdvancedSettingsTab::CAdvancedSettingsTab(CGUITab* pTab) :
    m_pTab(pTab)
{
    CGUI *pManager = g_pCore->GetGUI();

    /**
    *  Advanced tab
    **/
    auto vecTemp = CVector2D(12.f, 12.f);
    CVector2D vecSize;
    float fComboWidth = 170.f;
    float fHeaderHeight = 20;
    float fLineHeight = 27;
    float fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
        _("Fast CJ clothes loading:"),
        _("Browser speed:"),
        _("Single connection:"),
        _("Packet tag:"),
        _("Progress animation:"),
        _("Fullscreen mode:"),
        _("Process priority:"),
        _("Debug setting:"),
        _("Streaming memory:"),
        _("Update build type:"),
        _("Install important updates:")
    ) + 5.0f;

    // Misc section label
    m_pAdvancedMiscLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Misc")));
    m_pAdvancedMiscLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pAdvancedMiscLabel->SetFont("default-bold-small");
    m_pAdvancedMiscLabel->AutoSize();
    vecTemp.fY += fHeaderHeight;
    vecTemp.fX += 10.0f;

    // Fast clothes loading
    m_pFastClothesLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Fast CJ clothes loading:")));
    m_pFastClothesLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pFastClothesLabel->AutoSize();

    m_pFastClothesCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pFastClothesCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pFastClothesCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pFastClothesCombo->AddItem(_("Off"))->SetData((void*)CMultiplayer::FAST_CLOTHES_OFF);
    m_pFastClothesCombo->AddItem(_("On"))->SetData((void*)CMultiplayer::FAST_CLOTHES_ON);
    m_pFastClothesCombo->AddItem(_("Auto"))->SetData((void*)CMultiplayer::FAST_CLOTHES_AUTO);
    m_pFastClothesCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Browser scan speed
    m_pBrowserSpeedLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Browser speed:")));
    m_pBrowserSpeedLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pBrowserSpeedLabel->AutoSize();

    m_pBrowserSpeedCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pBrowserSpeedCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pBrowserSpeedCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pBrowserSpeedCombo->AddItem(_("Very slow"))->SetData((void*)0);
    m_pBrowserSpeedCombo->AddItem(_("Default"))->SetData((void*)1);
    m_pBrowserSpeedCombo->AddItem(_("Fast"))->SetData((void*)2);
    m_pBrowserSpeedCombo->SetReadOnly(true);
    vecTemp.fY += 29;

    // Single download
    m_pSingleDownloadLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Single connection:")));
    m_pSingleDownloadLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pSingleDownloadLabel->AutoSize();

    m_pSingleDownloadCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pSingleDownloadCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pSingleDownloadCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pSingleDownloadCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pSingleDownloadCombo->AddItem(_("On"))->SetData((void*)1);
    m_pSingleDownloadCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Packet tag
    m_pPacketTagLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Packet tag:")));
    m_pPacketTagLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pPacketTagLabel->AutoSize();

    m_pPacketTagCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pPacketTagCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pPacketTagCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pPacketTagCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pPacketTagCombo->AddItem(_("On"))->SetData((void*)1);
    m_pPacketTagCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Progress animation
    m_pProgressAnimationLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Progress animation:")));
    m_pProgressAnimationLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pProgressAnimationLabel->AutoSize();

    m_pProgressAnimationCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pProgressAnimationCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pProgressAnimationCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pProgressAnimationCombo->AddItem(_("Off"))->SetData((void*)0);
    m_pProgressAnimationCombo->AddItem(_("Default"))->SetData((void*)1);
    m_pProgressAnimationCombo->SetReadOnly(true);
    vecTemp.fY += fLineHeight;

    // Process priority
    m_pPriorityLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "Process priority:"));
    m_pPriorityLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pPriorityLabel->AutoSize();

    m_pPriorityCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pPriorityCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pPriorityCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pPriorityCombo->AddItem("Normal")->SetData((void*)0);
    m_pPriorityCombo->AddItem("Above normal")->SetData((void*)1);
    m_pPriorityCombo->AddItem("High")->SetData((void*)2);
    m_pPriorityCombo->SetReadOnly(true);
    vecTemp.fY += 29;

    // Debug setting
    m_pDebugSettingLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Debug setting:")));
    m_pDebugSettingLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pDebugSettingLabel->AutoSize();

    m_pDebugSettingCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pDebugSettingCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pDebugSettingCombo->SetSize(CVector2D(fComboWidth, 20.0f * (EDiagnosticDebug::MAX + 1)));
    m_pDebugSettingCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pDebugSettingCombo->AddItem("#6734 Graphics")->SetData((void*)EDiagnosticDebug::GRAPHICS_6734);
    //m_pDebugSettingCombo->AddItem ( "#6778 BIDI" )->SetData ( (void*)EDiagnosticDebug::BIDI_6778 );
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
    m_pStreamingMemoryLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Streaming memory:")));
    m_pStreamingMemoryLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 0.0f));
    m_pStreamingMemoryLabel->AutoSize();

    unsigned int uiMinMemory = g_pCore->GetMinStreamingMemory();
    unsigned int uiMaxMemory = g_pCore->GetMaxStreamingMemory();

    m_pStreamingMemoryMinLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Min")));
    m_pStreamingMemoryMinLabel->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY));
    m_pStreamingMemoryMinLabel->AutoSize();
    m_pStreamingMemoryMinLabel->GetSize(vecSize);
    m_pStreamingMemoryMinLabel->GetPosition(vecTemp);

    m_pStreamingMemory = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pStreamingMemory->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pStreamingMemory->GetPosition(vecTemp);
    m_pStreamingMemory->SetSize(CVector2D(130.0f, 20.0f));
    m_pStreamingMemory->GetSize(vecSize);
    m_pStreamingMemory->SetProperty("StepSize", SString("%.07lf", 1.0 / (uiMaxMemory - uiMinMemory)));

    m_pStreamingMemoryMaxLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Max")));
    m_pStreamingMemoryMaxLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pStreamingMemoryMaxLabel->AutoSize();
    vecTemp.fX = 22.f;
    vecTemp.fY += fLineHeight;

    // Windows 8 compatibility
    m_pWin8Label = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Windows 8 compatibility:")));
    m_pWin8Label->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pWin8Label->AutoSize();
    vecTemp.fX += 20;

    m_pWin8ColorCheckBox = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("16-bit color")));
    m_pWin8ColorCheckBox->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pWin8ColorCheckBox->AutoSize(NULL, 20.0f);
    vecTemp.fX += 90;

    m_pWin8MouseCheckBox = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Mouse fix")));
    m_pWin8MouseCheckBox->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pWin8MouseCheckBox->AutoSize(NULL, 20.0f);
    vecTemp.fY += fLineHeight;
    vecTemp.fX -= 110;

    // Hide if not Win8
    if (atoi(GetApplicationSetting("real-os-version")) != 8)
    {
#ifndef MTA_DEBUG   // Don't hide when debugging
        m_pWin8Label->SetVisible(false);
        m_pWin8ColorCheckBox->SetVisible(false);
        m_pWin8MouseCheckBox->SetVisible(false);
        vecTemp.fY -= fLineHeight;
#endif
    }

    // Cache path info
    m_pCachePathLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Client resource files:")));
    m_pCachePathLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pCachePathLabel->AutoSize();

    m_pCachePathShowButton = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Show in Explorer")));
    m_pCachePathShowButton->SetPosition(CVector2D(vecTemp.fX + fIndentX + 1, vecTemp.fY - 1));
    m_pCachePathShowButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pCachePathShowButton->SetClickHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnCachePathShowButtonClick, this));
    m_pCachePathShowButton->SetZOrderingEnabled(false);
    m_pCachePathShowButton->GetSize(vecSize);

    SString strFileCachePath = GetCommonRegistryValue("", "File Cache Path");
    m_pCachePathValue = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), strFileCachePath));
    m_pCachePathValue->SetPosition(CVector2D(vecTemp.fX + fIndentX + vecSize.fX + 10, vecTemp.fY + 3));
    m_pCachePathValue->SetFont("default-small");
    m_pCachePathValue->AutoSize();
    vecTemp.fY += fLineHeight;

    // Auto updater section label
    m_pAdvancedUpdaterLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Auto updater")));
    m_pAdvancedUpdaterLabel->SetPosition(CVector2D(vecTemp.fX - 10.0f, vecTemp.fY));
    m_pAdvancedUpdaterLabel->SetFont("default-bold-small");
    m_pAdvancedUpdaterLabel->AutoSize(_("Auto updater"));
    vecTemp.fY += fHeaderHeight;

    // UpdateAutoInstall
    m_pUpdateAutoInstallLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Install important updates:")));
    m_pUpdateAutoInstallLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pUpdateAutoInstallLabel->AutoSize();

    m_pUpdateAutoInstallCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
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
    m_pUpdateBuildTypeLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Update build type:")));
    m_pUpdateBuildTypeLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY));
    m_pUpdateBuildTypeLabel->AutoSize();

    m_pUpdateBuildTypeCombo = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pUpdateBuildTypeCombo->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 1.0f));
    m_pUpdateBuildTypeCombo->SetSize(CVector2D(fComboWidth, 95.0f));
    m_pUpdateBuildTypeCombo->AddItem(_("Default"))->SetData((void*)0);
    m_pUpdateBuildTypeCombo->AddItem("Nightly")->SetData((void*)2);
    m_pUpdateBuildTypeCombo->SetReadOnly(true);
    vecTemp.fX += fComboWidth + 15;

    // Check for updates
    m_pButtonUpdate = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Check for update now")));
    m_pButtonUpdate->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY));
    m_pButtonUpdate->AutoSize(NULL, 20.0f, 8.0f);
    m_pButtonUpdate->SetClickHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnUpdateButtonClick, this));
    m_pButtonUpdate->SetZOrderingEnabled(false);
    vecTemp.fY += fLineHeight;
    vecTemp.fX -= fComboWidth + 15;

    // Description label
    vecTemp.fY = 354 + 10;
    m_pAdvancedSettingDescriptionLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), ""));
    m_pAdvancedSettingDescriptionLabel->SetPosition(CVector2D(vecTemp.fX + 10.f, vecTemp.fY));
    m_pAdvancedSettingDescriptionLabel->SetFont("default-bold-small");
    m_pAdvancedSettingDescriptionLabel->SetSize(CVector2D(500.0f, 95.0f));
    m_pAdvancedSettingDescriptionLabel->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP);


    // Set up the events for advanced description 
    m_pPriorityLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pPriorityLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pPriorityCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pPriorityCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pFastClothesLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pFastClothesLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pFastClothesCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pFastClothesCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pBrowserSpeedLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pBrowserSpeedLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pBrowserSpeedCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pBrowserSpeedCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pSingleDownloadLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pSingleDownloadLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pSingleDownloadCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pSingleDownloadCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pPacketTagLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pPacketTagLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pPacketTagCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pPacketTagCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pProgressAnimationLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pProgressAnimationLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pProgressAnimationCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pProgressAnimationCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pDebugSettingLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pDebugSettingLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pDebugSettingCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pDebugSettingCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pStreamingMemoryLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pStreamingMemoryLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pStreamingMemory->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pStreamingMemory->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pUpdateBuildTypeLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pUpdateBuildTypeLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pUpdateBuildTypeCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pUpdateBuildTypeCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pWin8ColorCheckBox->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pWin8ColorCheckBox->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pWin8MouseCheckBox->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pWin8MouseCheckBox->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pUpdateAutoInstallLabel->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pUpdateAutoInstallLabel->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

    m_pUpdateAutoInstallCombo->SetMouseEnterHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnShowAdvancedSettingDescription, this));
    m_pUpdateAutoInstallCombo->SetMouseLeaveHandler(GUI_CALLBACK(&CAdvancedSettingsTab::OnHideAdvancedSettingDescription, this));

}



bool CAdvancedSettingsTab::OnShowAdvancedSettingDescription(CGUIElement* pElement)
{
    CGUILabel* pLabel = dynamic_cast <CGUILabel*> (pElement);
    CGUIComboBox* pComboBox = dynamic_cast <CGUIComboBox*> (pElement);
    CGUIScrollBar* pScrollBar = dynamic_cast <CGUIScrollBar*> (pElement);
    CGUICheckBox* pCheckBox = dynamic_cast <CGUICheckBox*> (pElement);

    std::string strText = "";

    if (pLabel && pLabel == m_pPriorityLabel.get() || pComboBox && pComboBox == m_pPriorityCombo.get())
        strText = std::string(_("Process priority:")) + " " + std::string(_("Very experimental feature."));
    else if (pLabel && pLabel == m_pFastClothesLabel.get() || pComboBox && pComboBox == m_pFastClothesCombo.get())
        strText = std::string(_("Fast CJ clothes loading:")) + " " + std::string(_("Stops stalls with CJ variations (Uses 65MB more RAM)"));
    else if (pLabel && pLabel == m_pBrowserSpeedLabel.get() || pComboBox && pComboBox == m_pBrowserSpeedCombo.get())
        strText = std::string(_("Browser speed:")) + " " + std::string(_("Older routers may require a slower scan speed."));
    else if (pLabel && pLabel == m_pSingleDownloadLabel.get() || pComboBox && pComboBox == m_pSingleDownloadCombo.get())
        strText = std::string(_("Single connection:")) + " " + std::string(_("Switch on to use only one connection when downloading."));
    else if (pLabel && pLabel == m_pPacketTagLabel.get() || pComboBox && pComboBox == m_pPacketTagCombo.get())
        strText = std::string(_("Packet tag:")) + " " + std::string(_("Tag network packets to help ISPs identify MTA traffic."));
    else if (pLabel && pLabel == m_pProgressAnimationLabel.get() || pComboBox && pComboBox == m_pProgressAnimationCombo.get())
        strText = std::string(_("Progress animation:")) + " " + std::string(_("Spinning circle animation at the bottom of the screen"));
    else if (pLabel && pLabel == m_pDebugSettingLabel.get() || pComboBox && pComboBox == m_pDebugSettingCombo.get())
        strText = std::string(_("Debug setting:")) + " " + std::string(_("Select default always. (This setting is not saved)"));
    else if (pLabel && pLabel == m_pStreamingMemoryLabel.get() || pScrollBar && pScrollBar == m_pStreamingMemory.get())
        strText = std::string(_("Streaming memory:")) + " " + std::string(_("Maximum is usually best"));
    else if (pLabel && pLabel == m_pUpdateBuildTypeLabel.get() || pComboBox && pComboBox == m_pUpdateBuildTypeCombo.get())
        strText = std::string(_("Auto updater:")) + " " + std::string(_("Select default unless you like filling out bug reports."));
    else if (pLabel && pLabel == m_pUpdateAutoInstallLabel.get() || pComboBox && pComboBox == m_pUpdateAutoInstallCombo.get())
        strText = std::string(_("Auto updater:")) + " " + std::string(_("Select default to automatically install important updates."));
    else if (pCheckBox && pCheckBox == m_pWin8ColorCheckBox.get())
        strText = std::string(_("16-bit color:")) + " " + std::string(_("Enable 16 bit color modes - Requires MTA restart"));
    else if (pCheckBox && pCheckBox == m_pWin8MouseCheckBox.get())
        strText = std::string(_("Mouse fix:")) + " " + std::string(_("Mouse movement fix - May need PC restart"));

    if (strText != "")
        m_pAdvancedSettingDescriptionLabel->SetText(strText.c_str());

    return true;
}

bool CAdvancedSettingsTab::OnHideAdvancedSettingDescription(CGUIElement* pElement)
{
    m_pAdvancedSettingDescriptionLabel->SetText("");

    return true;
}


void CAdvancedSettingsTab::SaveData(void)
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
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
    // Streaming memory
    float fPos = m_pStreamingMemory->GetScrollPosition();
    int min = g_pCore->GetMinStreamingMemory();
    int max = g_pCore->GetMaxStreamingMemory();
    unsigned int value = SharedUtil::Lerp(min, fPos, max);
    CVARS_SET("streaming_memory", value);


    if (CGUIListItem* pSelected = m_pPriorityCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("process_priority", iSelected);
        int PriorityClassList[] = { NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS };
        SetPriorityClass(GetCurrentProcess(), PriorityClassList[CVARS_GET_VALUE < int >("process_priority") % 3]);
    }


    // Fast clothes loading
    if (CGUIListItem* pSelected = m_pFastClothesCombo->GetSelectedItem())
    {
        int iSelected = (int)pSelected->GetData();
        CVARS_SET("fast_clothes_loading", iSelected);
        g_pCore->GetMultiplayer()->SetFastClothesLoading((CMultiplayer::EFastClothesLoading)iSelected);
    }

}

void CAdvancedSettingsTab::LoadData()
{

    // Process priority
    int iVar;
    CVARS_GET("process_priority", iVar);
    if (iVar == 0) m_pPriorityCombo->SetText("Normal");
    else if (iVar == 1) m_pPriorityCombo->SetText("Above normal");
    else if (iVar == 2) m_pPriorityCombo->SetText("High");
    int PriorityClassList[] = { NORMAL_PRIORITY_CLASS, ABOVE_NORMAL_PRIORITY_CLASS, HIGH_PRIORITY_CLASS };
    SetPriorityClass(GetCurrentProcess(), PriorityClassList[CVARS_GET_VALUE < int >("process_priority") % 3]);

    // Fast clothes loading
    CVARS_GET("fast_clothes_loading", iVar);
    if (iVar == CMultiplayer::FAST_CLOTHES_OFF) m_pFastClothesCombo->SetText(_("Off"));
    else if (iVar == CMultiplayer::FAST_CLOTHES_AUTO) m_pFastClothesCombo->SetText(_("Auto"));
    else if (iVar == CMultiplayer::FAST_CLOTHES_ON) m_pFastClothesCombo->SetText(_("On"));

    // Browser speed
    CVARS_GET("browser_speed", iVar);
    if (iVar == 0) m_pBrowserSpeedCombo->SetText(_("Very slow"));
    else if (iVar == 1) m_pBrowserSpeedCombo->SetText(_("Default"));
    else if (iVar == 2) m_pBrowserSpeedCombo->SetText(_("Fast"));

    // Single download
    CVARS_GET("single_download", iVar);
    if (iVar == 0) m_pSingleDownloadCombo->SetText(_("Default"));
    else if (iVar == 1) m_pSingleDownloadCombo->SetText(_("On"));

    // Packet tag
    CVARS_GET("packet_tag", iVar);
    if (iVar == 0) m_pPacketTagCombo->SetText(_("Default"));
    else if (iVar == 1) m_pPacketTagCombo->SetText(_("On"));

    // Progress animation
    CVARS_GET("progress_animation", iVar);
    if (iVar == 0) m_pProgressAnimationCombo->SetText(_("Off"));
    else if (iVar == 1) m_pProgressAnimationCombo->SetText(_("Default"));

    // Windows 8 16-bit color
    iVar = GetApplicationSettingInt("Win8Color16");
    m_pWin8ColorCheckBox->SetSelected(iVar != 0);

    // Windows 8 mouse fix
    iVar = GetApplicationSettingInt("Win8MouseFix");
    m_pWin8MouseCheckBox->SetSelected(iVar != 0);

    // Update build type
    CVARS_GET("update_build_type", iVar);
    if (iVar == 0 || iVar == 1) m_pUpdateBuildTypeCombo->SetText(_("Default"));
    else if (iVar == 2) m_pUpdateBuildTypeCombo->SetText("Nightly");

    // Update auto install
    CVARS_GET("update_auto_install", iVar);
    if (iVar == 0) m_pUpdateAutoInstallCombo->SetText(_("Off"));
    else if (iVar == 1) m_pUpdateAutoInstallCombo->SetText("Default");

    // Streaming memory
    unsigned int uiStreamingMemory = 0;
    CVARS_GET("streaming_memory", uiStreamingMemory);
    uiStreamingMemory = SharedUtil::Clamp(g_pCore->GetMinStreamingMemory(), uiStreamingMemory, g_pCore->GetMaxStreamingMemory());
    float fPos = SharedUtil::Unlerp(g_pCore->GetMinStreamingMemory(), uiStreamingMemory, g_pCore->GetMaxStreamingMemory());
    m_pStreamingMemory->SetScrollPosition(fPos);
}

bool CAdvancedSettingsTab::OnUpdateButtonClick(CGUIElement* pElement)
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


bool CAdvancedSettingsTab::OnCachePathShowButtonClick(CGUIElement* pElement)
{
    SString strFileCachePath = GetCommonRegistryValue("", "File Cache Path");
    if (DirectoryExists(strFileCachePath))
        ShellExecuteNonBlocking("open", strFileCachePath);
    return true;
}
