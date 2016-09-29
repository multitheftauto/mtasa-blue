#include <StdInc.h>
#include "CMultiplayerSettingsTab.h"

CMultiplayerSettingsTab::CMultiplayerSettingsTab(CGUITab* pTab, CSettings* pSettings) :
    m_pTab(pTab)
{
    m_bShownAllowScreenUploadMessage = false;
    CGUI *pManager = g_pCore->GetGUI();
    float fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
        _("Opacity:"),
        _("FOV:"),
        _("Draw Distance:"),
        _("Brightness:"),
        _("FX Quality:"),
        _("Anisotropic filtering:"),
        _("Anti-aliasing:"),
        _("Aspect Ratio:"),
        _("Opacity:")
    );
    /**
    *	Multiplayer tab
    **/
    CVector2D vecTemp, vecSize;
    m_pLabelNick = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Nick:")));
    m_pLabelNick->SetPosition(CVector2D(11, 13));
    m_pLabelNick->GetPosition(vecTemp, false);
    m_pLabelNick->AutoSize(_("Nick:"));
    m_pLabelNick->GetSize(vecSize);

    // Nick edit
    m_pEditNick = std::unique_ptr<CGUIEdit>(pManager->CreateEdit(m_pTab.get()));
    m_pEditNick->SetPosition(CVector2D(vecSize.fX + vecTemp.fX + 50.0f, vecTemp.fY - 1.0f));
    m_pEditNick->SetSize(CVector2D(178.0f, 24.0f));
    m_pEditNick->SetMaxLength(MAX_PLAYER_NICK_LENGTH);
    m_pEditNick->SetTextAcceptedHandler(GUI_CALLBACK(&CSettings::OnOKButtonClick, pSettings));

    m_pSavePasswords = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Save server passwords"), true));
    m_pSavePasswords->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 50.0f));
    m_pSavePasswords->GetPosition(vecTemp, false);
    m_pSavePasswords->AutoSize(NULL, 20.0f);

    m_pAutoRefreshBrowser = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Auto-refresh server browser"), true));
    m_pAutoRefreshBrowser->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pAutoRefreshBrowser->GetPosition(vecTemp, false);
    m_pAutoRefreshBrowser->AutoSize(NULL, 20.0f);

    m_pCheckBoxAllowScreenUpload = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Allow screen upload"), true));
    m_pCheckBoxAllowScreenUpload->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxAllowScreenUpload->GetPosition(vecTemp, false);
    m_pCheckBoxAllowScreenUpload->AutoSize(NULL, 20.0f);

    m_pCheckBoxCustomizedSAFiles = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Use customized GTA:SA files"), true));
    m_pCheckBoxCustomizedSAFiles->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 20.0f));
    m_pCheckBoxCustomizedSAFiles->GetPosition(vecTemp, false);
    m_pCheckBoxCustomizedSAFiles->AutoSize(NULL, 20.0f);

    m_pMapRenderingLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Map rendering options")));
    m_pMapRenderingLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pMapRenderingLabel->GetPosition(vecTemp, false);
    m_pMapRenderingLabel->SetFont("default-bold-small");
    m_pMapRenderingLabel->AutoSize();

    m_pMapAlphaLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Opacity:")));
    m_pMapAlphaLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 24.0f));
    m_pMapAlphaLabel->GetPosition(vecTemp, false);
    m_pMapAlphaLabel->AutoSize();

    m_pMapAlpha = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pMapAlpha->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pMapAlpha->GetPosition(vecTemp, false);
    m_pMapAlpha->SetSize(CVector2D(160.0f, 20.0f));
    m_pMapAlpha->GetSize(vecSize);
    m_pMapAlpha->SetProperty("StepSize", "0.01");

    m_pMapAlphaValueLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pMapAlphaValueLabel->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pMapAlphaValueLabel->GetPosition(vecTemp, false);
    m_pMapAlphaValueLabel->AutoSize("100% ");

    int iVar = 0;
    CVARS_GET("mapalpha", iVar);
    int iAlphaPercent = ceil(((float)Clamp(0, iVar, 255) / 255) * 100);
    m_pMapAlphaValueLabel->SetText(SString("%i%%", iAlphaPercent).c_str());
    float sbPos = (float)iAlphaPercent / 100.0f;
    m_pMapAlpha->SetScrollPosition(sbPos);


    m_pMapAlpha->SetOnScrollHandler(GUI_CALLBACK(&CMultiplayerSettingsTab::OnMapAlphaChanged, this));
    // Allow screen upload
    bool bAllowScreenUploadEnabled;
    CVARS_GET("allow_screen_upload", bAllowScreenUploadEnabled);
    m_pCheckBoxAllowScreenUpload->SetSelected(bAllowScreenUploadEnabled);

    // Customized sa files
    m_pCheckBoxCustomizedSAFiles->SetSelected(GetApplicationSettingInt("customized-sa-files-request") != 0);
    m_pCheckBoxCustomizedSAFiles->SetVisible(GetApplicationSettingInt("customized-sa-files-show") != 0);

    m_pCheckBoxAllowScreenUpload->SetClickHandler(GUI_CALLBACK(&CMultiplayerSettingsTab::OnAllowScreenUploadClick, this));
    m_pCheckBoxCustomizedSAFiles->SetClickHandler(GUI_CALLBACK(&CMultiplayerSettingsTab::OnCustomizedSAFilesClick, this));

}

void CMultiplayerSettingsTab::SaveData(void)
{
    std::string strVar;
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();



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

    bool bCustomizedSAFilesEnabled = m_pCheckBoxCustomizedSAFiles->GetSelected();
    bool bCustomizedSAFilesWasEnabled = GetApplicationSettingInt("customized-sa-files-request") != 0;
    m_bCustomizedSAFilesChanged = bCustomizedSAFilesWasEnabled != bCustomizedSAFilesEnabled;
    SetApplicationSettingInt("customized-sa-files-request", bCustomizedSAFilesEnabled ? 1 : 0);
    // Server pass saving
    bool bServerPasswords = m_pSavePasswords->GetSelected();
    CVARS_SET("save_server_passwords", bServerPasswords);
    if (!bServerPasswords)
    {
        g_pCore->GetSingleton().GetLocalGUI()->GetMainMenu()->GetServerBrowser()->ClearServerPasswords();
    }

    // Map alpha
    SString sText = m_pMapAlphaValueLabel->GetText();

    float fMapAlpha = ((atof(sText.substr(0, sText.length() - 1).c_str())) / 100) * 255;
    CVARS_SET("mapalpha", fMapAlpha);

    CVARS_SET("auto_refresh_browser", m_pAutoRefreshBrowser->GetSelected());

    // Allow screen upload
    bool bAllowScreenUploadEnabled = m_pCheckBoxAllowScreenUpload->GetSelected();
    CVARS_SET("allow_screen_upload", bAllowScreenUploadEnabled);
}

void CMultiplayerSettingsTab::LoadData()
{
    std::string strVar;
    bool bVar;
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
    CVARS_GET("save_server_passwords", bVar); m_pSavePasswords->SetSelected(bVar);
    CVARS_GET("auto_refresh_browser", bVar); m_pAutoRefreshBrowser->SetSelected(bVar);
}





bool CMultiplayerSettingsTab::OnMapAlphaChanged(CGUIElement* pElement)
{
    int iAlpha = (m_pMapAlpha->GetScrollPosition()) * 100;

    m_pMapAlphaValueLabel->SetText(SString("%i%%", iAlpha).c_str());
    return true;
}


//
// AllowScreenUpload
//
bool CMultiplayerSettingsTab::OnAllowScreenUploadClick(CGUIElement* pElement)
{
    if (!m_pCheckBoxAllowScreenUpload->GetSelected() && !m_bShownAllowScreenUploadMessage)
    {
        m_bShownAllowScreenUploadMessage = true;
        SString strMessage;
        strMessage += _("Screen upload is required by some servers for anti-cheat purposes."
            "\n\n(The chat box and GUI is excluded from the upload)\n");
        CCore::GetSingleton().ShowMessageBox(_("SCREEN UPLOAD INFORMATION"), strMessage, MB_BUTTON_OK | MB_ICON_INFO);
    }
    return true;
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

bool CMultiplayerSettingsTab::OnCustomizedSAFilesClick(CGUIElement* pElement)
{
    if (m_pCheckBoxCustomizedSAFiles->GetSelected())
    {
        SString strMessage;
        strMessage += _("Some files in your GTA:SA data directory are customized."
            "\nMTA will only use these modified files if this check box is ticked."
            "\n\nHowever, CUSTOMIZED GTA:SA FILES ARE BLOCKED BY MANY SERVERS"
            "\n\nAre you sure you want to use them?");
        CQuestionBox* pQuestionBox = CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow();
        pQuestionBox->Reset();
        pQuestionBox->SetTitle(_("CUSTOMIZED GTA:SA FILES"));
        pQuestionBox->SetMessage(strMessage);
        pQuestionBox->SetButton(0, _("No"));
        pQuestionBox->SetButton(1, _("Yes"));
        pQuestionBox->SetCallback(CustomizedSAFilesCallBack, m_pCheckBoxCustomizedSAFiles.get());
        pQuestionBox->Show();
    }
    return true;
}




void NewNicknameCallback(void* ptr, unsigned int uiButton, std::string strNick)
{
    if (uiButton == 1)  // We hit OK
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

void CMultiplayerSettingsTab::RequestNewNickname(void)
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
