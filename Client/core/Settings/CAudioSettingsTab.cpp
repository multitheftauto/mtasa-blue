#include <StdInc.h>
#include "CAudioSettingsTab.h"

CAudioSettingsTab::CAudioSettingsTab(CGUITab* pTab) :
    m_pTab(pTab)
{
    CGUI *pManager = g_pCore->GetGUI();
    CVector2D vecTemp, vecSize;

    /**
    *  Audio tab
    **/
    float fIndentX = pManager->CGUI_GetMaxTextExtent("default-normal",
        _("Radio volume:"),
        _("SFX volume:"),
        _("MTA volume:"),
        _("Voice volume:"),
        _("Play mode:")
    );

    m_pAudioGeneralLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("General")));
    m_pAudioGeneralLabel->SetPosition(CVector2D(11, 13));
    m_pAudioGeneralLabel->GetPosition(vecTemp, false);
    m_pAudioGeneralLabel->AutoSize(NULL, 5.0f);
    m_pAudioGeneralLabel->SetFont("default-bold-small");

    m_pLabelRadioVolume = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Radio volume:")));
    m_pLabelRadioVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f), false);
    m_pLabelRadioVolume->GetPosition(vecTemp, false);
    m_pLabelRadioVolume->AutoSize();

    m_pAudioRadioVolume = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pAudioRadioVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioRadioVolume->GetPosition(vecTemp, false);
    m_pAudioRadioVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioRadioVolume->GetSize(vecSize, false);
    m_pAudioRadioVolume->SetProperty("StepSize", "0.01");

    m_pLabelRadioVolumeValue = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pLabelRadioVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelRadioVolumeValue->GetPosition(vecTemp, false);
    m_pLabelRadioVolumeValue->AutoSize("100%");
    m_pLabelRadioVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pLabelSFXVolume = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("SFX volume:")));
    m_pLabelSFXVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pLabelSFXVolume->GetPosition(vecTemp, false);
    m_pLabelSFXVolume->AutoSize();

    m_pAudioSFXVolume = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pAudioSFXVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioSFXVolume->GetPosition(vecTemp, false);
    m_pAudioSFXVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioSFXVolume->GetSize(vecSize, false);
    m_pAudioSFXVolume->SetProperty("StepSize", "0.01");

    m_pLabelSFXVolumeValue = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pLabelSFXVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelSFXVolumeValue->GetPosition(vecTemp, false);
    m_pLabelSFXVolumeValue->AutoSize("100%");
    m_pLabelSFXVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pLabelMTAVolume = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("MTA volume:")));
    m_pLabelMTAVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pLabelMTAVolume->GetPosition(vecTemp, false);
    m_pLabelMTAVolume->AutoSize();

    m_pAudioMTAVolume = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pAudioMTAVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioMTAVolume->GetPosition(vecTemp, false);
    m_pAudioMTAVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioMTAVolume->GetSize(vecSize, false);
    m_pAudioMTAVolume->SetProperty("StepSize", "0.01");

    m_pLabelMTAVolumeValue = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pLabelMTAVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelMTAVolumeValue->GetPosition(vecTemp, false);
    m_pLabelMTAVolumeValue->AutoSize("100%");
    m_pLabelMTAVolumeValue->GetSize(vecSize, false);

    vecTemp.fX = 11;
    m_pLabelVoiceVolume = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Voice volume:")));
    m_pLabelVoiceVolume->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 29.0f));
    m_pLabelVoiceVolume->GetPosition(vecTemp, false);
    m_pLabelVoiceVolume->AutoSize();

    m_pAudioVoiceVolume = std::unique_ptr<CGUIScrollBar>(pManager->CreateScrollBar(true, m_pTab.get()));
    m_pAudioVoiceVolume->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pAudioVoiceVolume->GetPosition(vecTemp, false);
    m_pAudioVoiceVolume->SetSize(CVector2D(160.0f, 20.0f));
    m_pAudioVoiceVolume->GetSize(vecSize, false);
    m_pAudioVoiceVolume->SetProperty("StepSize", "0.01");

    m_pLabelVoiceVolumeValue = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), "0%"));
    m_pLabelVoiceVolumeValue->SetPosition(CVector2D(vecTemp.fX + vecSize.fX + 5.0f, vecTemp.fY));
    m_pLabelVoiceVolumeValue->GetPosition(vecTemp, false);
    m_pLabelVoiceVolumeValue->AutoSize("100%");
    m_pLabelVoiceVolumeValue->GetSize(vecSize, false);


    vecTemp.fX = 11;

    m_pCheckBoxMuteRadio = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Mute Radio sounds when minimized"), true));
    m_pCheckBoxMuteRadio->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pCheckBoxMuteRadio->GetPosition(vecTemp, false);
    m_pCheckBoxMuteRadio->AutoSize(NULL, 20.0f);

    m_pCheckBoxMuteSFX = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Mute SFX sounds when minimized"), true));
    m_pCheckBoxMuteSFX->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 16.0f));
    m_pCheckBoxMuteSFX->GetPosition(vecTemp, false);
    m_pCheckBoxMuteSFX->AutoSize(NULL, 20.0f);

    m_pCheckBoxMuteMTA = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Mute MTA sounds when minimized"), true));
    m_pCheckBoxMuteMTA->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 16.0f));
    m_pCheckBoxMuteMTA->GetPosition(vecTemp, false);
    m_pCheckBoxMuteMTA->AutoSize(NULL, 20.0f);

    m_pCheckBoxMuteVoice = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Mute Voice sounds when minimized"), true));
    m_pCheckBoxMuteVoice->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 16.0f));
    m_pCheckBoxMuteVoice->GetPosition(vecTemp, false);
    m_pCheckBoxMuteVoice->AutoSize(NULL, 20.0f);

    vecTemp.fX = 11;

    m_pCheckBoxAudioEqualizer = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Radio Equalizer"), true));
    m_pCheckBoxAudioEqualizer->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pCheckBoxAudioEqualizer->AutoSize(NULL, 20.0f);

    m_pCheckBoxAudioAutotune = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Radio Auto-tune"), true));
    m_pCheckBoxAudioAutotune->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 50.0f));
    m_pCheckBoxAudioAutotune->AutoSize(NULL, 20.0f);
    m_pCheckBoxAudioAutotune->GetPosition(vecTemp);

    m_pAudioUsertrackLabel = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Usertrack options")));
    m_pAudioUsertrackLabel->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f), false);
    m_pAudioUsertrackLabel->GetPosition(vecTemp, false);
    m_pAudioUsertrackLabel->AutoSize(NULL, 10.0f);
    m_pAudioUsertrackLabel->SetFont("default-bold-small");

    m_pLabelUserTrackMode = std::unique_ptr<CGUILabel>(pManager->CreateLabel(m_pTab.get(), _("Play mode:")));
    m_pLabelUserTrackMode->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 30.0f));
    m_pLabelUserTrackMode->GetPosition(vecTemp, false);
    m_pLabelUserTrackMode->AutoSize();

    m_pComboUsertrackMode = std::unique_ptr<CGUIComboBox>(pManager->CreateComboBox(m_pTab.get(), ""));
    m_pComboUsertrackMode->SetPosition(CVector2D(vecTemp.fX + fIndentX + 5.0f, vecTemp.fY));
    m_pComboUsertrackMode->SetSize(CVector2D(160.0f, 80.0f));
    m_pComboUsertrackMode->AddItem(_("Radio"))->SetData((void*)0);
    m_pComboUsertrackMode->AddItem(_("Random"))->SetData((void*)1);
    m_pComboUsertrackMode->AddItem(_("Sequential"))->SetData((void*)2);
    m_pComboUsertrackMode->SetReadOnly(true);

    m_pCheckBoxUserAutoscan = std::unique_ptr<CGUICheckBox>(pManager->CreateCheckBox(m_pTab.get(), _("Automatic Media Scan"), true));
    m_pCheckBoxUserAutoscan->SetPosition(CVector2D(vecTemp.fX, vecTemp.fY + 32.0f));
    m_pCheckBoxUserAutoscan->AutoSize(NULL, 20.0f);
    m_pCheckBoxUserAutoscan->GetPosition(vecTemp, false);

    m_pTab->GetParent()->GetSize(vecTemp);
    m_pAudioDefButton = std::unique_ptr<CGUIButton>(pManager->CreateButton(m_pTab.get(), _("Load defaults")));
    m_pAudioDefButton->SetClickHandler(GUI_CALLBACK(&CAudioSettingsTab::OnAudioDefaultClick, this));
    m_pAudioDefButton->AutoSize(NULL, 20.0f, 8.0f);
    m_pAudioDefButton->GetSize(vecSize);
    m_pAudioDefButton->SetPosition(CVector2D(vecTemp.fX - vecSize.fX - 12.0f, 387));
    m_pAudioDefButton->SetZOrderingEnabled(false);

    m_pAudioRadioVolume->SetOnScrollHandler(GUI_CALLBACK(&CAudioSettingsTab::OnRadioVolumeChanged, this));
    m_pAudioSFXVolume->SetOnScrollHandler(GUI_CALLBACK(&CAudioSettingsTab::OnSFXVolumeChanged, this));
    m_pAudioMTAVolume->SetOnScrollHandler(GUI_CALLBACK(&CAudioSettingsTab::OnMTAVolumeChanged, this));
    m_pAudioVoiceVolume->SetOnScrollHandler(GUI_CALLBACK(&CAudioSettingsTab::OnVoiceVolumeChanged, this));
}


void CAudioSettingsTab::UpdateAudioTab()
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    float fMTAVolume = 0, fRadioVolume = (float)(gameSettings->GetRadioVolume()) / 64.0f, fSFXVolume = (float)(gameSettings->GetSFXVolume()) / 64.0f;

    CVARS_GET("mtavolume", fMTAVolume);

    m_pAudioMTAVolume->SetScrollPosition(fMTAVolume);
    m_pAudioRadioVolume->SetScrollPosition(fRadioVolume);
    m_pAudioSFXVolume->SetScrollPosition(fSFXVolume);

    m_pCheckBoxAudioEqualizer->SetSelected(gameSettings->IsRadioEqualizerEnabled());
    m_pCheckBoxAudioAutotune->SetSelected(gameSettings->IsRadioAutotuneEnabled());
    m_pCheckBoxUserAutoscan->SetSelected(gameSettings->IsUsertrackAutoScan());

    CVARS_GET("mute_sfx_when_minimized", m_bMuteSFX);
    CVARS_GET("mute_radio_when_minimized", m_bMuteRadio);
    CVARS_GET("mute_mta_when_minimized", m_bMuteMTA);
    CVARS_GET("mute_voice_when_minimized", m_bMuteVoice);
    m_pCheckBoxMuteSFX->SetSelected(m_bMuteSFX);
    m_pCheckBoxMuteRadio->SetSelected(m_bMuteRadio);
    m_pCheckBoxMuteMTA->SetSelected(m_bMuteMTA);
    m_pCheckBoxMuteVoice->SetSelected(m_bMuteVoice);

    m_pComboUsertrackMode->SetSelectedItemByIndex(gameSettings->GetUsertrackMode());

}

//
// Called when the user clicks on the audio 'Load Defaults' button.
//
bool CAudioSettingsTab::OnAudioDefaultClick(CGUIElement* pElement)
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    gameSettings->SetRadioVolume(100);
    gameSettings->SetSFXVolume(100);
    CVARS_SET("mtavolume", 100);

    gameSettings->SetRadioAutotuneEnabled(true);
    gameSettings->SetRadioEqualizerEnabled(true);

    gameSettings->SetUsertrackAutoScan(false);

    CVARS_SET("mute_sfx_when_minimized", false);
    CVARS_SET("mute_radio_when_minimized", false);
    CVARS_SET("mute_mta_when_minimized", false);
    CVARS_SET("mute_voice_when_minimized", false);

    gameSettings->SetUsertrackMode(0);
    // Update the GUI
    UpdateAudioTab();

    return true;
}

bool CAudioSettingsTab::OnRadioVolumeChanged(CGUIElement* pElement)
{
    unsigned char ucVolume = m_pAudioRadioVolume->GetScrollPosition() * 100.0f;
    m_pLabelRadioVolumeValue->SetText(SString("%i%%", ucVolume).c_str());

    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    gameSettings->SetRadioVolume(m_pAudioRadioVolume->GetScrollPosition() * 64);

    return true;
}

bool CAudioSettingsTab::OnSFXVolumeChanged(CGUIElement* pElement)
{
    unsigned char ucVolume = m_pAudioSFXVolume->GetScrollPosition() * 100.0f;
    m_pLabelSFXVolumeValue->SetText(SString("%i%%", ucVolume).c_str());

    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    gameSettings->SetSFXVolume(m_pAudioSFXVolume->GetScrollPosition() * 64);

    return true;
}

bool CAudioSettingsTab::OnMTAVolumeChanged(CGUIElement* pElement)
{
    int iVolume = m_pAudioMTAVolume->GetScrollPosition() * 100.0f;
    m_pLabelMTAVolumeValue->SetText(SString("%i%%", iVolume).c_str());

    CVARS_SET("mtavolume", m_pAudioMTAVolume->GetScrollPosition());

    return true;
}

bool CAudioSettingsTab::OnVoiceVolumeChanged(CGUIElement* pElement)
{
    int iVolume = m_pAudioVoiceVolume->GetScrollPosition() * 100.0f;
    m_pLabelVoiceVolumeValue->SetText(SString("%i%%", iVolume).c_str());

    CVARS_SET("voicevolume", m_pAudioVoiceVolume->GetScrollPosition());

    return true;
}

void CAudioSettingsTab::SaveData(void)
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();

    // Audio
    gameSettings->SetRadioEqualizerEnabled(m_pCheckBoxAudioEqualizer->GetSelected());
    gameSettings->SetRadioAutotuneEnabled(m_pCheckBoxAudioAutotune->GetSelected());
    gameSettings->SetUsertrackAutoScan(m_pCheckBoxUserAutoscan->GetSelected());

    m_bMuteSFX = m_pCheckBoxMuteSFX->GetSelected();
    m_bMuteRadio = m_pCheckBoxMuteRadio->GetSelected();
    m_bMuteMTA = m_pCheckBoxMuteMTA->GetSelected();
    m_bMuteVoice = m_pCheckBoxMuteVoice->GetSelected();
    CVARS_SET("mute_sfx_when_minimized", m_bMuteSFX);
    CVARS_SET("mute_radio_when_minimized", m_bMuteRadio);
    CVARS_SET("mute_mta_when_minimized", m_bMuteMTA);
    CVARS_SET("mute_voice_when_minimized", m_bMuteVoice);

    if (CGUIListItem* pSelected = m_pComboUsertrackMode->GetSelectedItem())
    {
        gameSettings->SetUsertrackMode((int)pSelected->GetData());
    }

}

void CAudioSettingsTab::LoadData()
{

    // Audio
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    m_ucOldRadioVolume = gameSettings->GetRadioVolume();
    m_pAudioRadioVolume->SetScrollPosition((float)m_ucOldRadioVolume / 64.0f);
    m_ucOldSFXVolume = gameSettings->GetSFXVolume();
    m_pAudioSFXVolume->SetScrollPosition((float)m_ucOldSFXVolume / 64.0f);

    m_pCheckBoxAudioEqualizer->SetSelected(gameSettings->IsRadioEqualizerEnabled());
    m_pCheckBoxAudioAutotune->SetSelected(gameSettings->IsRadioAutotuneEnabled());
    m_pCheckBoxUserAutoscan->SetSelected(gameSettings->IsUsertrackAutoScan());

    CVARS_GET("mute_sfx_when_minimized", m_bMuteSFX);
    CVARS_GET("mute_radio_when_minimized", m_bMuteRadio);
    CVARS_GET("mute_mta_when_minimized", m_bMuteMTA);
    CVARS_GET("mute_voice_when_minimized", m_bMuteVoice);
    m_pCheckBoxMuteSFX->SetSelected(m_bMuteSFX);
    m_pCheckBoxMuteRadio->SetSelected(m_bMuteRadio);
    m_pCheckBoxMuteMTA->SetSelected(m_bMuteMTA);
    m_pCheckBoxMuteVoice->SetSelected(m_bMuteVoice);

    unsigned int uiUsertrackMode = gameSettings->GetUsertrackMode();
    if (uiUsertrackMode == 0) m_pComboUsertrackMode->SetText(_("Radio"));
    else if (uiUsertrackMode == 1) m_pComboUsertrackMode->SetText(_("Random"));
    else if (uiUsertrackMode == 2) m_pComboUsertrackMode->SetText(_("Sequential"));

    CVARS_GET("mtavolume", m_fOldMTAVolume);
    CVARS_GET("voicevolume", m_fOldVoiceVolume);
    m_fOldMTAVolume = std::max(0.0f, std::min(1.0f, m_fOldMTAVolume));
    m_fOldVoiceVolume = std::max(0.0f, std::min(1.0f, m_fOldVoiceVolume));
    m_pAudioMTAVolume->SetScrollPosition(m_fOldMTAVolume);
    m_pAudioVoiceVolume->SetScrollPosition(m_fOldVoiceVolume);

}

void CAudioSettingsTab::Reset() 
{
    CGameSettings * gameSettings = CCore::GetSingleton().GetGame()->GetSettings();
    gameSettings->SetRadioVolume(m_ucOldRadioVolume);
    gameSettings->SetSFXVolume(m_ucOldSFXVolume);
    CVARS_SET("mtavolume", m_fOldMTAVolume);
    CVARS_SET("voicevolume", m_fOldVoiceVolume);
}
