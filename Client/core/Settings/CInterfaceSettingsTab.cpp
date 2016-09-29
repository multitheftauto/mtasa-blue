#include <StdInc.h>
#include "CInterfaceSettingsTab.h"

CInterfaceSettingsTab::CInterfaceSettingsTab(CGUITab* pTab, CSettings* pSettings) :
    m_pTab(pTab),
    m_pSettingsWindow(pSettings->GetWindow())
{
    CGUI *pManager = g_pCore->GetGUI();
    /**
    * Interface/chat Tab
    **/
    m_bIsModLoaded = false;
    {
        auto pLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("General")));
        pLabel->SetPosition(CVector2D(10.0f, 12.0f));
        pLabel->AutoSize(NULL, 5.0f);
        pLabel->SetFont("default-bold-small");
    }

    float fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
        _("Language:"),
        _("Skin:"),
        _("Presets:")
    ) + 5.0f;

    CVector2D vecTemp, vecSize;

    {
        m_pLanguageLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Language:")));
        m_pLanguageLabel->SetPosition(CVector2D(10.0f, 35.0f));
        m_pLanguageLabel->GetPosition(vecTemp);
        m_pLanguageLabel->AutoSize();
    }

    m_pInterfaceLanguageSelector = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), "English"));
    m_pInterfaceLanguageSelector->SetPosition(CVector2D(vecTemp.fX + fIndentX, 33.0f));
    m_pInterfaceLanguageSelector->SetSize(CVector2D(350.0f - (vecTemp.fX + fIndentX), 200.0f));
    m_pInterfaceLanguageSelector->SetReadOnly(true);

    // Grab languages and populate
    std::map<SString, SString> m_LanguageMap = g_pCore->GetLocalization()->GetAvailableLanguages();
    m_LanguageMap["English"] = "en_US";
    std::map<SString, SString>::const_iterator itr;
    for (itr = m_LanguageMap.begin(); itr != m_LanguageMap.end(); ++itr)
    {
        CGUIListItem* pItem = m_pInterfaceLanguageSelector->AddItem((*itr).first);
        pItem->SetData((*itr).second);
    }

    {
        m_pSkinLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Skin:")));
        m_pSkinLabel->SetPosition(CVector2D(10.0f, 65.0f));
        m_pSkinLabel->AutoSize();
    }

    m_pInterfaceSkinSelector = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get()));
    m_pInterfaceSkinSelector->SetPosition(CVector2D(vecTemp.fX + fIndentX, 63.0f));
    m_pInterfaceSkinSelector->SetSize(CVector2D(350.0f - (vecTemp.fX + fIndentX), 200.0f));
    m_pInterfaceSkinSelector->SetReadOnly(true);

    {
        m_pChatLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Chat")));
        m_pChatLabel->SetPosition(CVector2D(10.0f, 90.0f));
        m_pChatLabel->AutoSize();
        m_pChatLabel->SetFont("default-bold-small");
    }


    // Presets

    {
        m_pPresetsLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Presets:")));
        m_pPresetsLabel->SetPosition(CVector2D(10.0f, 112.0f));
        m_pPresetsLabel->AutoSize();
    }

    m_pChatPresets = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get()));
    m_pChatPresets->SetPosition(CVector2D(vecTemp.fX + fIndentX, 110.0f));
    m_pChatPresets->SetSize(CVector2D(350.0f - (vecTemp.fX + fIndentX), 200.0f));
    m_pChatPresets->SetReadOnly(true);

    m_pChatLoadPreset = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Load")));
    m_pChatLoadPreset->SetPosition(CVector2D(360.0f, 110.0f));
    m_pChatLoadPreset->SetSize(CVector2D(100.0f, 24.0f));
    m_pChatLoadPreset->SetZOrderingEnabled(false);

    // Color selection
    SString strChatBG = _("Chat BG");
    SString strChatText = _("Chat Text");
    SString strInputBG = _("Input BG");
    SString strInputText = _("Input Text");

    float fColorTabsTextWidth = pManager->GetTextExtent(strChatBG)
        + pManager->GetTextExtent(strChatText)
        + pManager->GetTextExtent(strInputBG)
        + pManager->GetTextExtent(strInputText);

    // Add 30 for each tab
    fColorTabsTextWidth += 30 * 4;
    float fColorTabPanelWidth = Max(350.f, fColorTabsTextWidth);

    m_pColorTabPanel = std::unique_ptr<CGUITabPanel>(pManager->CreateTabPanel(m_pTab.get()));
    m_pColorTabPanel->SetPosition(CVector2D(10.0f, 150.0f));
    m_pColorTabPanel->SetSize(CVector2D(fColorTabPanelWidth, 150.0f));

    CreateChatColorTab(ChatColorTypes::CHAT_COLOR_BG, strChatBG, m_pColorTabPanel.get());
    CreateChatColorTab(ChatColorTypes::CHAT_COLOR_TEXT, strChatText, m_pColorTabPanel.get());
    CreateChatColorTab(ChatColorTypes::CHAT_COLOR_INPUT_BG, strInputBG, m_pColorTabPanel.get());
    CreateChatColorTab(ChatColorTypes::CHAT_COLOR_INPUT_TEXT, strInputText, m_pColorTabPanel.get());

    // Cache position and size from color tab panel (for positioning and height)
    m_pColorTabPanel->GetPosition(vecTemp);
    m_pColorTabPanel->GetSize(vecSize);

    // Font Selection
    CVector2D tabPanelSize;
    pTab->GetParent()->GetSize(tabPanelSize);
    float fChatFontSizeY = tabPanelSize.fY - (vecTemp.fY + vecSize.fY) - 20.0f;

    m_pPaneChatFont = std::unique_ptr<CGUIScrollPane>(pManager->CreateScrollPane(m_pTab.get()));
    m_pPaneChatFont->SetProperty("ContentPaneAutoSized", "False");
    m_pPaneChatFont->SetPosition(CVector2D(10.0f, vecTemp.fY + vecSize.fY + 5.0f));
    m_pPaneChatFont->SetSize(CVector2D(125.0f, fChatFontSizeY));

    m_pFontLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pPaneChatFont.get(), _("Font:")));
    m_pFontLabel->SetPosition(CVector2D(0.0f, 0.0f));
    m_pFontLabel->GetPosition(vecTemp, false);
    m_pFontLabel->AutoSize();
    m_pFontLabel->GetSize(vecSize);
    m_pFontLabel->SetFont("default-bold-small");

    float fFontNamesMarginY = 22.0f;
    float fLineHeight = 20.0f;

    m_pRadioChatFont[ChatFonts::CHAT_FONT_DEFAULT] = std::unique_ptr<CGUIRadioButton>(pManager->CreateRadioButton(m_pPaneChatFont.get(), "Tahoma"));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_DEFAULT]->SetSelected(true);
    m_pRadioChatFont[ChatFonts::CHAT_FONT_DEFAULT]->SetPosition(CVector2D(0.0f, fFontNamesMarginY));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_DEFAULT]->SetSize(CVector2D(100.0f, 15.0f));

    m_pRadioChatFont[ChatFonts::CHAT_FONT_CLEAR] = std::unique_ptr<CGUIRadioButton>(pManager->CreateRadioButton(m_pPaneChatFont.get(), "Verdana"));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_CLEAR]->SetPosition(CVector2D(0.0f, fLineHeight * 1 + fFontNamesMarginY));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_CLEAR]->SetSize(CVector2D(100.0f, 15.0f));

    m_pRadioChatFont[ChatFonts::CHAT_FONT_BOLD] = std::unique_ptr<CGUIRadioButton>(pManager->CreateRadioButton(m_pPaneChatFont.get(), "Tahoma Bold"));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_BOLD]->SetPosition(CVector2D(0.0f, fLineHeight * 2 + fFontNamesMarginY));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_BOLD]->SetSize(CVector2D(100.0f, 15.0f));

    m_pRadioChatFont[ChatFonts::CHAT_FONT_ARIAL] = std::unique_ptr<CGUIRadioButton>(pManager->CreateRadioButton(m_pPaneChatFont.get(), "Arial"));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_ARIAL]->SetPosition(CVector2D(0.0f, fLineHeight * 3 + fFontNamesMarginY));
    m_pRadioChatFont[ChatFonts::CHAT_FONT_ARIAL]->SetSize(CVector2D(100.0f, 15.0f));

    // Misc. Options
    {
        fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
            _("Lines:"),
            _("Scale:"),
            _("Width:")
        );

        // Add a small indent for edit boxes
        fIndentX += 10.0f;

        // Cache position and size from color tab panel (for positioning and height)
        m_pColorTabPanel->GetPosition(vecTemp);
        m_pColorTabPanel->GetSize(vecSize);

        // Background pane in case of overlap with the color panel
        float fBGSizeX = tabPanelSize.fX - (vecTemp.fX + vecSize.fX) - 20.0f;
        float fBGSizeY = vecSize.fY;

        m_pChatOptionsPanel = std::unique_ptr<CGUITabPanel>(pManager->CreateTabPanel(m_pTab.get()));
        m_pChatOptionsPanel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY));
        m_pChatOptionsPanel->SetSize(CVector2D(fBGSizeX, fBGSizeY));

        // Size of lines and gaps
        float fLineSizeY = 30;
        float fLineGapY = 4;

        // Layout tab
        m_pLayoutTab = std::unique_ptr<CGUITab>(m_pChatOptionsPanel->CreateTab(_("Layout")));

        m_pLayoutTabLinesLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pLayoutTab.get(), _("Lines:")));
        m_pLayoutTabLinesLabel->SetPosition(CVector2D(10.0f, 10.0f));
        m_pLayoutTabLinesLabel->GetPosition(vecTemp);
        m_pLayoutTabLinesLabel->AutoSize();
        m_pLayoutTabLinesLabel->SetAlwaysOnTop(true);

        m_pChatLines = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pLayoutTab.get(), ""));
        m_pChatLines->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatLines->SetSize(CVector2D(80.0f, 24.0f));
        m_pChatLines->SetAlwaysOnTop(true);

        m_pLayoutTabScaleLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pLayoutTab.get(), _("Scale:")));
        m_pLayoutTabScaleLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
        m_pLayoutTabScaleLabel->GetPosition(vecTemp);
        m_pLayoutTabScaleLabel->AutoSize();
        m_pLayoutTabScaleLabel->SetAlwaysOnTop(true);

        m_pChatScaleX = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pLayoutTab.get(), ""));
        m_pChatScaleX->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatScaleX->SetSize(CVector2D(35.0f, 24.0f));
        m_pChatScaleX->SetAlwaysOnTop(true);

        m_pLayoutTabXLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pLayoutTab.get(), "x"));
        m_pLayoutTabXLabel->SetPosition(CVector2D(vecTemp.fX + fIndentX + 37.0f, vecTemp.fY + 2.0f));
        m_pLayoutTabXLabel->AutoSize();
        m_pLayoutTabXLabel->SetAlwaysOnTop(true);

        m_pChatScaleY = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pLayoutTab.get(), ""));
        m_pChatScaleY->SetPosition(CVector2D(vecTemp.fX + fIndentX + 45.0f, vecTemp.fY - 2.0f));
        m_pChatScaleY->SetSize(CVector2D(35.0f, 24.0f));
        m_pChatScaleY->SetAlwaysOnTop(true);

        m_pLayoutTabWidthLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pLayoutTab.get(), _("Width:")));
        m_pLayoutTabWidthLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
        m_pLayoutTabWidthLabel->GetPosition(vecTemp);
        m_pLayoutTabWidthLabel->AutoSize();
        m_pLayoutTabWidthLabel->SetAlwaysOnTop(true);

        m_pChatWidth = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pLayoutTab.get(), ""));
        m_pChatWidth->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatWidth->SetSize(CVector2D(80.0f, 24.0f));
        m_pChatWidth->SetAlwaysOnTop(true);

        // Fading tab
        m_pFadingTab = std::unique_ptr<CGUITab>(m_pChatOptionsPanel->CreateTab(_("Fading")));

        fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
            _("after"),
            _("for")
        );

        // Add a small indent for edit boxes
        fIndentX += 10.0f;

        m_pChatCssText = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pFadingTab.get(), _("Fade out old lines")));
        m_pChatCssText->SetPosition(CVector2D(10.0f, 10.0f));
        m_pChatCssText->GetPosition(vecTemp);
        m_pChatCssText->AutoSize(NULL, 20.0f);

        m_pFadingTabAfterLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pFadingTab.get(), _("after")));
        m_pFadingTabAfterLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineSizeY + fLineGapY));
        m_pFadingTabAfterLabel->GetPosition(vecTemp);
        m_pFadingTabAfterLabel->AutoSize();

        m_pChatLineLife = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pFadingTab.get(), ""));
        m_pChatLineLife->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatLineLife->SetSize(CVector2D(45.0f, 24.0f));

        m_pFadingTabSecLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pFadingTab.get(), _("sec")));
        m_pFadingTabSecLabel->SetPosition(CVector2D(vecTemp.fX + fIndentX + 55.0f, vecTemp.fY));
        m_pFadingTabSecLabel->AutoSize();

        m_pFadingTabForLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pFadingTab.get(), _("for")));
        m_pFadingTabForLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 28.0f));
        m_pFadingTabForLabel->GetPosition(vecTemp);
        m_pFadingTabForLabel->AutoSize();

        m_pChatLineFadeout = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pFadingTab.get(), ""));
        m_pChatLineFadeout->SetPosition(CVector2D(vecTemp.fX + fIndentX, vecTemp.fY - 2.0f));
        m_pChatLineFadeout->SetSize(CVector2D(45.0f, 24.0f));

        m_pFadingTabSecLabel2 = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pFadingTab.get(), _("sec")));
        m_pFadingTabSecLabel2->SetPosition(CVector2D(vecTemp.fX + fIndentX + 55.0f, vecTemp.fY));
        m_pFadingTabSecLabel2->AutoSize();

        if (m_pChatCssText->GetSize().fX > 170)
        {
            m_pChatCssText->SetPosition(
                CVector2D(m_pFadingTabSecLabel2->GetPosition().fX + m_pFadingTabSecLabel2->GetSize().fX - m_pChatCssText->GetSize().fX,
                    m_pChatCssText->GetPosition().fY)
            );
        }

        // Cache position and size from font panel
        m_pPaneChatFont->GetPosition(vecTemp);
        m_pPaneChatFont->GetSize(vecSize);

        // Options section
        m_pOptionsLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Options:")));
        m_pOptionsLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 10.0f, vecTemp.fY));
        m_pOptionsLabel->GetPosition(vecTemp, false);
        m_pOptionsLabel->AutoSize();
        m_pOptionsLabel->GetSize(vecSize);
        m_pOptionsLabel->SetFont("default-bold-small");

        m_pChatCssBackground = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Hide background when not typing")));
        m_pChatCssBackground->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fFontNamesMarginY));
        m_pChatCssBackground->GetPosition(vecTemp);
        m_pChatCssBackground->AutoSize(NULL, 20.0f);

        m_pChatNickCompletion = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Nickname completion using the \"Tab\" key")));
        m_pChatNickCompletion->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineHeight));
        m_pChatNickCompletion->GetPosition(vecTemp);
        m_pChatNickCompletion->AutoSize(NULL, 20.0f);

        m_pFlashWindow = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Allow server to flash the window")));
        m_pFlashWindow->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineHeight));
        m_pFlashWindow->GetPosition(vecTemp);
        m_pFlashWindow->AutoSize(NULL, 20.0f);

        m_pTrayBalloon = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Allow tray balloon notifications")));
        m_pTrayBalloon->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + fLineHeight));
        m_pTrayBalloon->GetPosition(vecTemp);
        m_pTrayBalloon->AutoSize(NULL, 20.0f);
    }



    m_pChatLoadPreset->SetClickHandler(GUI_CALLBACK(&CInterfaceSettingsTab::OnChatLoadPresetClick, this));
    m_pInterfaceLanguageSelector->SetSelectionHandler(GUI_CALLBACK(&CInterfaceSettingsTab::OnLanguageChanged, this));
    m_pInterfaceSkinSelector->SetSelectionHandler(GUI_CALLBACK(&CInterfaceSettingsTab::OnSkinChanged, this));

}


bool CInterfaceSettingsTab::OnChatRedChanged(CGUIElement* pElement)
{
    auto pScrollBar = (CGUIScrollBar*)pElement;
    int iValue = ((float)pScrollBar->GetScrollPosition() * 255.0f);

    if (pScrollBar == m_pChatRed[ChatColorTypes::CHAT_COLOR_BG].get())
        m_pChatRedValue[ChatColorTypes::CHAT_COLOR_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatRed[ChatColorTypes::CHAT_COLOR_TEXT].get())
        m_pChatRedValue[ChatColorTypes::CHAT_COLOR_TEXT]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatRed[ChatColorTypes::CHAT_COLOR_INPUT_BG].get())
        m_pChatRedValue[ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatRed[ChatColorTypes::CHAT_COLOR_INPUT_TEXT].get())
        m_pChatRedValue[ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText(SString("%i", iValue).c_str());

    return true;
}

bool CInterfaceSettingsTab::OnChatGreenChanged(CGUIElement* pElement)
{
    auto pScrollBar = (CGUIScrollBar*)pElement;
    int iValue = ((float)pScrollBar->GetScrollPosition() * 255.0f);

    if (pScrollBar == m_pChatGreen[ChatColorTypes::CHAT_COLOR_BG].get())
        m_pChatGreenValue[ChatColorTypes::CHAT_COLOR_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatGreen[ChatColorTypes::CHAT_COLOR_TEXT].get())
        m_pChatGreenValue[ChatColorTypes::CHAT_COLOR_TEXT]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatGreen[ChatColorTypes::CHAT_COLOR_INPUT_BG].get())
        m_pChatGreenValue[ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatGreen[ChatColorTypes::CHAT_COLOR_INPUT_TEXT].get())
        m_pChatGreenValue[ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText(SString("%i", iValue).c_str());

    return true;
}

bool CInterfaceSettingsTab::OnChatBlueChanged(CGUIElement* pElement)
{
    auto pScrollBar = (CGUIScrollBar*)pElement;
    int iValue = ((float)pScrollBar->GetScrollPosition() * 255.0f);

    if (pScrollBar == m_pChatBlue[ChatColorTypes::CHAT_COLOR_BG].get())
        m_pChatBlueValue[ChatColorTypes::CHAT_COLOR_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatBlue[ChatColorTypes::CHAT_COLOR_TEXT].get())
        m_pChatBlueValue[ChatColorTypes::CHAT_COLOR_TEXT]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatBlue[ChatColorTypes::CHAT_COLOR_INPUT_BG].get())
        m_pChatBlueValue[ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatBlue[ChatColorTypes::CHAT_COLOR_INPUT_TEXT].get())
        m_pChatBlueValue[ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText(SString("%i", iValue).c_str());

    return true;
}

bool CInterfaceSettingsTab::OnChatAlphaChanged(CGUIElement* pElement)
{
    auto pScrollBar = (CGUIScrollBar*)pElement;
    int iValue = ((float)pScrollBar->GetScrollPosition() * 255.0f);

    if (pScrollBar == m_pChatAlpha[ChatColorTypes::CHAT_COLOR_BG].get())
        m_pChatAlphaValue[ChatColorTypes::CHAT_COLOR_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatAlpha[ChatColorTypes::CHAT_COLOR_TEXT].get())
        m_pChatAlphaValue[ChatColorTypes::CHAT_COLOR_TEXT]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatAlpha[ChatColorTypes::CHAT_COLOR_INPUT_BG].get())
        m_pChatAlphaValue[ChatColorTypes::CHAT_COLOR_INPUT_BG]->SetText(SString("%i", iValue).c_str());
    else if (pScrollBar == m_pChatAlpha[ChatColorTypes::CHAT_COLOR_INPUT_TEXT].get())
        m_pChatAlphaValue[ChatColorTypes::CHAT_COLOR_INPUT_TEXT]->SetText(SString("%i", iValue).c_str());

    return true;
}

bool CInterfaceSettingsTab::OnChatLoadPresetClick(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pChatPresets->GetSelectedItem();
    if (!pItem)
        return true;

    CXMLNode* pNode = reinterpret_cast <CXMLNode*> (pItem->GetData());
    if (!pNode)
        return true;

    for (auto& pSubNode : pNode->GetChildren())
    {
        // Load all settings provided
        std::string strTag = pSubNode->GetTagName();
        std::string strValue = pSubNode->GetTagContent();

        if (strValue.length() > 0)
        {
            if (strTag == "color_text")
            {
                LoadChatColorFromString(ChatColorTypes::CHAT_COLOR_TEXT, strValue);
            }
            else if (strTag == "color_background")
            {
                LoadChatColorFromString(ChatColorTypes::CHAT_COLOR_BG, strValue);
            }
            else if (strTag == "color_input_text")
            {
                LoadChatColorFromString(ChatColorTypes::CHAT_COLOR_INPUT_TEXT, strValue);
            }
            else if (strTag == "color_input_background")
            {
                LoadChatColorFromString(ChatColorTypes::CHAT_COLOR_INPUT_BG, strValue);
            }
            else if (strTag == "font")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);

                if (iValue < 0 || iValue >= ChatFonts::CHAT_FONT_MAX)
                    iValue = ChatFonts::CHAT_FONT_DEFAULT;
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
                    std::stringstream ss(strValue);
                    ss >> strValue;
                    m_pChatScaleX->SetText(strValue.c_str());
                    ss >> strValue;
                    m_pChatScaleY->SetText(strValue.c_str());
                }
                catch (...)
                {
                }
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
                SetMilliseconds(m_pChatLineLife.get(), iValue);
            }
            else if (strTag == "line_fadeout")
            {
                int iValue;
                pSubNode->GetTagContent(iValue);
                SetMilliseconds(m_pChatLineFadeout.get(), iValue);
            }
        }
    }

    return true;
}


bool CInterfaceSettingsTab::OnLanguageChanged(CGUIElement* pElement)
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
        m_pSettingsWindow->MoveToBack();
        return true;
    }

    return true;
}


bool CInterfaceSettingsTab::OnSkinChanged(CGUIElement* pElement)
{
    CGUIListItem* pItem = m_pInterfaceSkinSelector->GetSelectedItem();
    if (!pItem)
        return true;

    std::string currentSkin;
    CVARS_GET("current_skin", currentSkin);

    if (m_bIsModLoaded)
    {
        // Reset our item
        unsigned int uiIndex = 0;
        std::string strItemText = m_pInterfaceSkinSelector->GetItemText(uiIndex);
        while (strItemText != currentSkin)
        {
            strItemText = m_pInterfaceSkinSelector->GetItemText(++uiIndex);
        }
        m_pInterfaceSkinSelector->SetSelectedItemByIndex(uiIndex);
        g_pCore->ShowMessageBox(_("Error") + _E("CC80"), _("Please disconnect before changing skin"), MB_BUTTON_OK | MB_ICON_INFO);
        m_pSettingsWindow->MoveToBack();
        return true;
    }

    return true;
}

void CInterfaceSettingsTab::CreateChatColorTab(ChatColorType eType, const char* szName, CGUITabPanel* pParent)
{
    CVector2D vecTemp;

    // Create the GUI Elements
    CGUI* pManager = g_pCore->GetGUI();
    CGUITab* pTab = pParent->CreateTab(szName);

    m_pChatRedLabel[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, _("Red:")));
    m_pChatRedLabel[eType]->SetPosition(CVector2D(0.05f, 0.065f), true);
    m_pChatRedLabel[eType]->GetPosition(vecTemp, false);
    m_pChatRedLabel[eType]->AutoSize(_("Red:"));

    m_pChatRedValue[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, "0"));
    m_pChatRedValue[eType]->SetPosition(CVector2D(vecTemp.fX + 270.0f, vecTemp.fY));
    m_pChatRedValue[eType]->AutoSize("255 ");

    m_pChatRed[eType] = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, pTab));
    m_pChatRed[eType]->SetPosition(CVector2D(vecTemp.fX + 90.0f, vecTemp.fY));
    m_pChatRed[eType]->SetSize(CVector2D(175.0f, 20.0f));
    m_pChatRed[eType]->SetOnScrollHandler(GUI_CALLBACK(&CInterfaceSettingsTab::OnChatRedChanged, this));
    m_pChatRed[eType]->SetProperty("StepSize", "0.004");

    m_pChatGreenLabel[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, _("Green:")));
    m_pChatGreenLabel[eType]->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pChatGreenLabel[eType]->GetPosition(vecTemp, false);
    m_pChatGreenLabel[eType]->AutoSize(_("Green:"));

    m_pChatGreenValue[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, "0"));
    m_pChatGreenValue[eType]->SetPosition(CVector2D(vecTemp.fX + 270.0f, vecTemp.fY));
    m_pChatGreenValue[eType]->AutoSize("255 ");

    m_pChatGreen[eType] = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, pTab));
    m_pChatGreen[eType]->SetPosition(CVector2D(vecTemp.fX + 90.0f, vecTemp.fY));
    m_pChatGreen[eType]->SetSize(CVector2D(175.0f, 20.0f));
    m_pChatGreen[eType]->SetOnScrollHandler(GUI_CALLBACK(&CInterfaceSettingsTab::OnChatGreenChanged, this));
    m_pChatGreen[eType]->SetProperty("StepSize", "0.004");

    m_pChatBlueLabel[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, _("Blue:")));
    m_pChatBlueLabel[eType]->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pChatBlueLabel[eType]->GetPosition(vecTemp, false);
    m_pChatBlueLabel[eType]->AutoSize(_("Blue:"));

    m_pChatBlueValue[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, "0"));
    m_pChatBlueValue[eType]->SetPosition(CVector2D(vecTemp.fX + 270.0f, vecTemp.fY));
    m_pChatBlueValue[eType]->AutoSize("255 ");

    m_pChatBlue[eType] = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, pTab));
    m_pChatBlue[eType]->SetPosition(CVector2D(vecTemp.fX + 90.0f, vecTemp.fY));
    m_pChatBlue[eType]->SetSize(CVector2D(175.0f, 20.0f));
    m_pChatBlue[eType]->SetOnScrollHandler(GUI_CALLBACK(&CInterfaceSettingsTab::OnChatBlueChanged, this));
    m_pChatBlue[eType]->SetProperty("StepSize", "0.004");

    m_pChatAlphaLabel[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, _("Transparency:")));
    m_pChatAlphaLabel[eType]->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pChatAlphaLabel[eType]->GetPosition(vecTemp, false);
    m_pChatAlphaLabel[eType]->AutoSize(_("Transparency:"));

    m_pChatAlphaValue[eType] = std::unique_ptr<CGUILabel>(pManager->CreateLabel(pTab, "0"));
    m_pChatAlphaValue[eType]->SetPosition(CVector2D(vecTemp.fX + 270.0f, vecTemp.fY));
    m_pChatAlphaValue[eType]->AutoSize("255 ");

    m_pChatAlpha[eType] = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, pTab));
    m_pChatAlpha[eType]->SetPosition(CVector2D(vecTemp.fX + 90.0f, vecTemp.fY));
    m_pChatAlpha[eType]->SetSize(CVector2D(175.0f, 20.0f));
    m_pChatAlpha[eType]->SetOnScrollHandler(GUI_CALLBACK(&CInterfaceSettingsTab::OnChatAlphaChanged, this));
    m_pChatAlpha[eType]->SetProperty("StepSize", "0.004");
}

void CInterfaceSettingsTab::LoadChatColorFromCVar(ChatColorType eType, const char* szCVar)
{
    // Load the color according to the cvars and update the gui
    CColor pColor;
    CVARS_GET(szCVar, pColor);
    SetChatColorValues(eType, pColor);
}

void CInterfaceSettingsTab::SaveChatColor(ChatColorType eType, const char* szCVar)
{
    // Save the color to the cvar if it's different
    CColor pColor, pOldColor;
    pColor = GetChatColorValues(eType);
    CVARS_GET(szCVar, pOldColor);

    if (pColor.R != pOldColor.R || pColor.G != pOldColor.G || pColor.B != pOldColor.B || pColor.A != pOldColor.A)
        CVARS_SET(szCVar, pColor);
}

CColor CInterfaceSettingsTab::GetChatColorValues(ChatColorType eType)
{
    // Retrieve the color according to the scrollbar values
    CColor pColor;
    pColor.R = m_pChatRed[eType]->GetScrollPosition() * 255;
    pColor.G = m_pChatGreen[eType]->GetScrollPosition() * 255;
    pColor.B = m_pChatBlue[eType]->GetScrollPosition() * 255;
    pColor.A = m_pChatAlpha[eType]->GetScrollPosition() * 255;
    return pColor;
}

void CInterfaceSettingsTab::SetChatColorValues(ChatColorType eType, CColor pColor)
{
    // Set the scrollbar position based on the color
    m_pChatRed[eType]->SetScrollPosition((float)pColor.R / 255.0f);
    m_pChatGreen[eType]->SetScrollPosition((float)pColor.G / 255.0f);
    m_pChatBlue[eType]->SetScrollPosition((float)pColor.B / 255.0f);
    m_pChatAlpha[eType]->SetScrollPosition((float)pColor.A / 255.0f);
}

void CInterfaceSettingsTab::LoadChatPresets()
{
    CXMLFile* pPresetsFile = CCore::GetSingleton().GetXML()->CreateXML(CalcMTASAPath(CHAT_PRESETS_PATH));
    if (pPresetsFile && pPresetsFile->Parse())
    {
        CXMLNode* pPresetsRoot = pPresetsFile->GetRootNode();
        if (!pPresetsRoot)
            pPresetsRoot = pPresetsFile->CreateRootNode(CHAT_PRESETS_ROOT);

        for (auto& pNode : pPresetsRoot->GetChildren())
        {
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

void CInterfaceSettingsTab::LoadSkins()
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

void CInterfaceSettingsTab::LoadChatColorFromString(ChatColorType eType, const std::string& strColor)
{
    CColor pColor;
    std::stringstream ss(strColor);
    int iR, iG, iB, iA;

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

int CInterfaceSettingsTab::GetMilliseconds(CGUIEdit* pEdit)
{
    // Note to anyone listening: stringstream does not handle out of range numbers well
    double dValue = strtol(pEdit->GetText().c_str(), NULL, 0);
    dValue *= 1000;
    dValue = Clamp < double >(INT_MIN, dValue, INT_MAX);
    return static_cast <int> (dValue);
}

void CInterfaceSettingsTab::SetMilliseconds(CGUIEdit* pEdit, int iValue)
{
    std::stringstream ss;
    ss << (float)iValue / 1000;
    pEdit->SetText(ss.str().c_str());
}

void CInterfaceSettingsTab::SaveData(void)
{
    std::string strVar;
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    // Language
    CGUIListItem* pItem = m_pInterfaceLanguageSelector->GetSelectedItem();
    if (pItem)
    {
        const char* szItemText = (const char*)pItem->GetData();
        CVARS_SET("locale", std::string(szItemText));
        SetApplicationSetting("locale", szItemText);
    }

    // Chat
    SaveChatColor(ChatColorTypes::CHAT_COLOR_BG, "chat_color");
    SaveChatColor(ChatColorTypes::CHAT_COLOR_TEXT, "chat_text_color");
    SaveChatColor(ChatColorTypes::CHAT_COLOR_INPUT_BG, "chat_input_color");
    SaveChatColor(ChatColorTypes::CHAT_COLOR_INPUT_TEXT, "chat_input_text_color");
    for (int iFont = 0; iFont < ChatColorTypes::CHAT_COLOR_MAX; iFont++)
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
    CVARS_SET("chat_line_life", GetMilliseconds(m_pChatLineLife.get()));
    CVARS_SET("chat_line_fade_out", GetMilliseconds(m_pChatLineFadeout.get()));

    // Interface
    CVARS_SET("server_can_flash_window", m_pFlashWindow->GetSelected());
    CVARS_SET("allow_tray_notifications", m_pTrayBalloon->GetSelected());

    // Set our new skin last, as it'll destroy all our GUI
    pItem = m_pInterfaceSkinSelector->GetSelectedItem();
    if (pItem)
        CVARS_SET("current_skin", pItem->GetText());
}

void CInterfaceSettingsTab::LoadData()
{
    std::string strVar;
    bool bVar;
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
    uiIndex = 0;
    std::string strItemText = m_pInterfaceSkinSelector->GetItemText(uiIndex);
    while (strItemText != currentSkin)
    {
        strItemText = m_pInterfaceSkinSelector->GetItemText(++uiIndex);
    }
    m_pInterfaceSkinSelector->SetSelectedItemByIndex(uiIndex);


    // Chat
    LoadChatColorFromCVar(ChatColorTypes::CHAT_COLOR_BG, "chat_color");
    LoadChatColorFromCVar(ChatColorTypes::CHAT_COLOR_TEXT, "chat_text_color");
    LoadChatColorFromCVar(ChatColorTypes::CHAT_COLOR_INPUT_BG, "chat_input_color");
    LoadChatColorFromCVar(ChatColorTypes::CHAT_COLOR_INPUT_TEXT, "chat_input_text_color");

    unsigned int uiFont;
    CVARS_GET("chat_font", uiFont);
    if (uiFont >= ChatFonts::CHAT_FONT_MAX)
        uiFont = ChatFonts::CHAT_FONT_DEFAULT;
    m_pRadioChatFont[uiFont]->SetSelected(true);

    CVARS_GET("chat_lines", strVar); m_pChatLines->SetText(strVar.c_str());

    try
    {
        CVARS_GET("chat_scale", strVar);
        std::stringstream ss(strVar);
        ss >> strVar;
        m_pChatScaleX->SetText(strVar.c_str());
        ss >> strVar;
        m_pChatScaleY->SetText(strVar.c_str());
    }
    catch (...)
    {
    }

    CVARS_GET("chat_width", strVar); m_pChatWidth->SetText(strVar.c_str());
    CVARS_GET("chat_css_style_text", bVar); m_pChatCssText->SetSelected(bVar);
    CVARS_GET("chat_css_style_background", bVar); m_pChatCssBackground->SetSelected(bVar);
    CVARS_GET("chat_nickcompletion", bVar); m_pChatNickCompletion->SetSelected(bVar);

    {
        int iVar;
        CVARS_GET("chat_line_life", iVar);
        SetMilliseconds(m_pChatLineLife.get(), iVar);

        CVARS_GET("chat_line_fade_out", iVar);
        SetMilliseconds(m_pChatLineFadeout.get(), iVar);
    }

    // Interface
    CVARS_GET("server_can_flash_window", bVar); m_pFlashWindow->SetSelected(bVar);
    CVARS_GET("allow_tray_notifications", bVar); m_pTrayBalloon->SetSelected(bVar);
}
