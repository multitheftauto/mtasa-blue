/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
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
#pragma once

class CAudioSettingsTab
{
public:
    CAudioSettingsTab(CGUITab* pTab);

    void                UpdateAudioTab();
    void SaveData();
    void LoadData();
    void Reset();
private:
    bool                OnAudioDefaultClick(CGUIElement* pElement);
    bool                OnRadioVolumeChanged(CGUIElement* pElement);
    bool                OnSFXVolumeChanged(CGUIElement* pElement);
    bool                OnMTAVolumeChanged(CGUIElement* pElement);
    bool                OnVoiceVolumeChanged(CGUIElement* pElement);

    std::unique_ptr<CGUITab> m_pTab;

    unsigned char       m_ucOldRadioVolume;
    unsigned char       m_ucOldSFXVolume;
    float               m_fOldMTAVolume;
    float               m_fOldVoiceVolume;
    bool                m_bMuteSFX;
    bool                m_bMuteRadio;
    bool                m_bMuteMTA;
    bool                m_bMuteVoice;


    std::unique_ptr<CGUILabel>          m_pAudioGeneralLabel;
    std::unique_ptr<CGUILabel>          m_pUserTrackGeneralLabel;
    std::unique_ptr<CGUILabel>          m_pLabelRadioVolume;
    std::unique_ptr<CGUILabel>          m_pLabelSFXVolume;
    std::unique_ptr<CGUILabel>          m_pLabelMTAVolume;
    std::unique_ptr<CGUILabel>          m_pLabelVoiceVolume;
    std::unique_ptr<CGUILabel>          m_pLabelRadioVolumeValue;
    std::unique_ptr<CGUILabel>          m_pLabelSFXVolumeValue;
    std::unique_ptr<CGUILabel>          m_pLabelMTAVolumeValue;
    std::unique_ptr<CGUILabel>          m_pLabelVoiceVolumeValue;
    std::unique_ptr<CGUIScrollBar>      m_pAudioRadioVolume;
    std::unique_ptr<CGUIScrollBar>      m_pAudioSFXVolume;
    std::unique_ptr<CGUIScrollBar>      m_pAudioMTAVolume;
    std::unique_ptr<CGUIScrollBar>      m_pAudioVoiceVolume;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxAudioEqualizer;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxAudioAutotune;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxMuteSFX;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxMuteRadio;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxMuteMTA;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxMuteVoice;
    std::unique_ptr<CGUILabel>          m_pAudioUsertrackLabel;
    std::unique_ptr<CGUICheckBox>       m_pCheckBoxUserAutoscan;
    std::unique_ptr<CGUILabel>          m_pLabelUserTrackMode;
    std::unique_ptr<CGUIComboBox>       m_pComboUsertrackMode;
    std::unique_ptr<CGUIButton>         m_pAudioDefButton;

};
