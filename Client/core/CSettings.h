/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CSettings.h
 *  PURPOSE:     Header file for in-game settings window class
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

class CSettings;

#pragma once

#include <core/CCoreInterface.h>
#include "CMainMenu.h"
#include "CCore.h"

#define SKINS_PATH                    "skins/*"
#define CHAT_PRESETS_PATH             "mta/config/chatboxpresets.xml"
#define CHAT_PRESETS_ROOT             "chatboxpresets"

// #define SHOWALLSETTINGS

struct SKeyBindSection
{
    SKeyBindSection(char* szTitle)
    {
        this->currentIndex = 0;
        this->szTitle = new char[strlen(szTitle) + 1];
        this->szOriginalTitle = new char[strlen(szTitle) + 1];
        // convert to upper case
        for (unsigned int i = 0; i < strlen(szTitle); i++)
        {
            if (isalpha((uchar)szTitle[i]))
                this->szTitle[i] = toupper(szTitle[i]);
            else
                this->szTitle[i] = szTitle[i];
        }

        this->szTitle[strlen(szTitle)] = '\0';

        strcpy(szOriginalTitle, szTitle);

        this->rowCount = 0;

        headerItem = NULL;
    }

    ~SKeyBindSection()
    {
        if (this->szTitle)
            delete[] this->szTitle;

        if (szOriginalTitle)
            delete[] szOriginalTitle;
    }
    int                 currentIndex;            // temporarily stores the index while the list is being created
    char*               szTitle;
    char*               szOriginalTitle;
    int                 rowCount;
    class CGUIListItem* headerItem;
};

class CColor;

enum
{
    FULLSCREEN_STANDARD,
    FULLSCREEN_BORDERLESS,
    FULLSCREEN_BORDERLESS_KEEP_RES,
    FULLSCREEN_BORDERLESS_STRETCHED,
};

class CSettings
{
    friend class CCore;

public:
    CSettings();
    ~CSettings();

    void CreateGUI();
    void DestroyGUI();

    bool ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Update();
    void Initialize();

    void SetVisible(bool bVisible);
    bool IsVisible();

    void SetIsModLoaded(bool bLoaded);

    void LoadData();

    bool IsCapturingKey() { return m_bCaptureKey; }
    void UpdateCaptureAxis();
    void UpdateJoypadTab();

    void UpdateAudioTab();

    void UpdateVideoTab();
    void PopulateResolutionComboBox();
    void UpdateFullScreenComboBoxEnabled();

    void AddKeyBindSection(char* szSectionName);
    void RemoveKeyBindSection(char* szSectionName);
    void RemoveAllKeyBindSections();

    void RequestNewNickname();
    void ShowRestartQuestion();
    void ShowDisconnectQuestion();

    void TabSkip(bool bBackwards);

    bool IsActive();

    void SetSelectedIndex(unsigned int uiIndex);
    void ShowRichPresenceShareDataQuestionBox() const;

protected:
    const static int SecKeyNum = 3;            // Number of secondary keys

    // Keep these protected so we can access them in the event handlers of CClientGame
    CGUIElement*  m_pWindow;
    CGUITabPanel* m_pTabs;
    CGUITab*      m_pTabInterface;
    CGUITab*      m_pTabBrowser;
    CGUIButton*   m_pButtonOK;
    CGUIButton*   m_pButtonCancel;
    CGUILabel*    m_pLabelNick;
    CGUIButton*   m_pButtonGenerateNick;
    CGUIStaticImage*   m_pButtonGenerateNickIcon;
    CGUIEdit*     m_pEditNick;
    CGUICheckBox* m_pSavePasswords;
    CGUICheckBox* m_pAutoRefreshBrowser;

    CGUILabel*     m_pVideoGeneralLabel;
    CGUILabel*     m_pVideoResolutionLabel;
    CGUIComboBox*  m_pComboResolution;
    CGUICheckBox*  m_pCheckBoxMipMapping;
    CGUICheckBox*  m_pCheckBoxWindowed;
    CGUICheckBox*  m_pCheckBoxDPIAware = nullptr;
    CGUICheckBox*  m_pCheckBoxHudMatchAspectRatio;
    CGUICheckBox*  m_pCheckBoxMinimize;
    CGUILabel*     m_pMapRenderingLabel;
    CGUIComboBox*  m_pComboFxQuality;
    CGUILabel*     m_pFXQualityLabel;
    CGUIComboBox*  m_pComboAspectRatio;
    CGUILabel*     m_pAspectRatioLabel;
    CGUICheckBox*  m_pCheckBoxVolumetricShadows;
    CGUICheckBox*  m_pCheckBoxDeviceSelectionDialog;
    CGUICheckBox*  m_pCheckBoxShowUnsafeResolutions;
    CGUICheckBox*  m_pCheckBoxAllowScreenUpload;
    CGUICheckBox*  m_pCheckBoxAllowExternalSounds;
    CGUICheckBox*  m_pCheckBoxCustomizedSAFiles;
    CGUICheckBox*  m_pCheckBoxAllowDiscordRPC;
    CGUICheckBox*  m_pCheckBoxAllowSteamClient = nullptr;
    CGUICheckBox*  m_pCheckBoxAlwaysShowTransferBox;
    CGUICheckBox*  m_pCheckBoxGrass;
    CGUICheckBox*  m_pCheckBoxHeatHaze;
    CGUICheckBox*  m_pCheckBoxTyreSmokeParticles;
    CGUICheckBox*  m_pCheckBoxHighDetailVehicles;
    CGUICheckBox*  m_pCheckBoxHighDetailPeds;
    CGUICheckBox*  m_pCheckBoxBlur;
    CGUICheckBox*  m_pCheckBoxCoronaReflections;
    CGUICheckBox*  m_pCheckBoxDynamicPedShadows;
    CGUILabel*     m_pFieldOfViewLabel;
    CGUIScrollBar* m_pFieldOfView;
    CGUILabel*     m_pFieldOfViewValueLabel;
    CGUILabel*     m_pDrawDistanceLabel;
    CGUIScrollBar* m_pDrawDistance;
    CGUILabel*     m_pDrawDistanceValueLabel;
    CGUILabel*     m_pBrightnessLabel;
    CGUIScrollBar* m_pBrightness;
    CGUILabel*     m_pBrightnessValueLabel;
    CGUILabel*     m_pAnisotropicLabel;
    CGUIScrollBar* m_pAnisotropic;
    CGUILabel*     m_pAnisotropicValueLabel;
    CGUIComboBox*  m_pComboAntiAliasing;
    CGUILabel*     m_pAntiAliasingLabel;
    CGUILabel*     m_pMapAlphaLabel;
    CGUIScrollBar* m_pMapAlpha;
    CGUILabel*     m_pMapAlphaValueLabel;
    CGUILabel*     m_pStreamingMemoryLabel;
    CGUIScrollBar* m_pStreamingMemory;
    CGUILabel*     m_pStreamingMemoryMinLabel;
    CGUILabel*     m_pStreamingMemoryMaxLabel;
    CGUILabel*     m_pStreamingMemoryLabelInfo;
    CGUIButton*    m_pVideoDefButton;

    CGUILabel*     m_pAdvancedSettingDescriptionLabel;
    CGUILabel*     m_pFullscreenStyleLabel;
    CGUIComboBox*  m_pFullscreenStyleCombo;
    CGUILabel*     m_pPriorityLabel;
    CGUIComboBox*  m_pPriorityCombo;
    CGUILabel*     m_pPlayerMapImageLabel;
    CGUIComboBox*  m_pPlayerMapImageCombo;
    CGUILabel*     m_pFastClothesLabel;
    CGUIComboBox*  m_pFastClothesCombo;
    CGUILabel*     m_pAudioGeneralLabel;
    CGUILabel*     m_pUserTrackGeneralLabel;
    CGUILabel*     m_pBrowserSpeedLabel;
    CGUIComboBox*  m_pBrowserSpeedCombo;
    CGUILabel*     m_pSingleDownloadLabel;
    CGUIComboBox*  m_pSingleDownloadCombo;
    CGUILabel*     m_pPacketTagLabel;
    CGUIComboBox*  m_pPacketTagCombo;
    CGUILabel*     m_pProgressAnimationLabel;
    CGUIComboBox*  m_pProgressAnimationCombo;
    CGUILabel*     m_pDebugSettingLabel;
    CGUIComboBox*  m_pDebugSettingCombo;
    CGUILabel*     m_pWin8Label;
    CGUICheckBox*  m_pWin8ColorCheckBox;
    CGUICheckBox*  m_pWin8MouseCheckBox;
    CGUICheckBox*  m_pPhotoSavingCheckbox;
    CGUICheckBox*  m_pCheckBoxAskBeforeDisconnect;
    CGUICheckBox*  m_pProcessAffinityCheckbox;
    CGUILabel*     m_pUpdateBuildTypeLabel;
    CGUIComboBox*  m_pUpdateBuildTypeCombo;
    CGUILabel*     m_pUpdateAutoInstallLabel;
    CGUIComboBox*  m_pUpdateAutoInstallCombo;
    CGUIButton*    m_pButtonUpdate;
    CGUILabel*     m_pAdvancedMiscLabel;
    CGUILabel*     m_pAdvancedUpdaterLabel;
    CGUILabel*     m_pCachePathLabel;
    CGUILabel*     m_pCachePathValue;
    CGUIButton*    m_pCachePathShowButton;
    CGUILabel*     m_pLabelMasterVolume;
    CGUILabel*     m_pLabelRadioVolume;
    CGUILabel*     m_pLabelSFXVolume;
    CGUILabel*     m_pLabelMTAVolume;
    CGUILabel*     m_pLabelVoiceVolume;
    CGUILabel*     m_pLabelMasterVolumeValue;
    CGUILabel*     m_pLabelRadioVolumeValue;
    CGUILabel*     m_pLabelSFXVolumeValue;
    CGUILabel*     m_pLabelMTAVolumeValue;
    CGUILabel*     m_pLabelVoiceVolumeValue;
    CGUIScrollBar* m_pAudioMasterVolume;
    CGUIScrollBar* m_pAudioRadioVolume;
    CGUIScrollBar* m_pAudioSFXVolume;
    CGUIScrollBar* m_pAudioMTAVolume;
    CGUIScrollBar* m_pAudioVoiceVolume;
    CGUILabel*     m_pAudioRadioLabel;
    CGUICheckBox*  m_pCheckBoxAudioEqualizer;
    CGUICheckBox*  m_pCheckBoxAudioAutotune;
    CGUILabel*     m_pAudioMuteLabel;
    CGUICheckBox*  m_pCheckBoxMuteMaster;
    CGUICheckBox*  m_pCheckBoxMuteSFX;
    CGUICheckBox*  m_pCheckBoxMuteRadio;
    CGUICheckBox*  m_pCheckBoxMuteMTA;
    CGUICheckBox*  m_pCheckBoxMuteVoice;
    CGUILabel*     m_pAudioUsertrackLabel;
    CGUICheckBox*  m_pCheckBoxUserAutoscan;
    CGUILabel*     m_pLabelUserTrackMode;
    CGUIComboBox*  m_pComboUsertrackMode;
    CGUIButton*    m_pAudioDefButton;

    CGUIGridList* m_pBindsList;
    CGUIButton*   m_pBindsDefButton;
    CGUIHandle    m_hBind, m_hPriKey, m_hSecKeys[SecKeyNum];

    CGUILabel*               m_pJoypadName;
    CGUILabel*               m_pJoypadUnderline;
    CGUIEdit*                m_pEditDeadzone;
    CGUIEdit*                m_pEditSaturation;
    std::vector<CGUILabel*>  m_pJoypadLabels;
    std::vector<CGUIButton*> m_pJoypadButtons;
    int                      m_JoypadSettingsRevision;

    CGUILabel*     m_pControlsMouseLabel;
    CGUICheckBox*  m_pInvertMouse;
    CGUICheckBox*  m_pSteerWithMouse;
    CGUICheckBox*  m_pFlyWithMouse;
    CGUILabel*     m_pLabelMouseSensitivity;
    CGUIScrollBar* m_pMouseSensitivity;
    CGUILabel*     m_pLabelMouseSensitivityValue;
    CGUILabel*     m_pLabelVerticalAimSensitivity;
    CGUIScrollBar* m_pVerticalAimSensitivity;
    CGUILabel*     m_pLabelVerticalAimSensitivityValue;

    CGUILabel*       m_pControlsJoypadLabel;
    CGUIScrollPane*  m_pControlsInputTypePane;
    CGUIRadioButton* m_pStandardControls;
    CGUIRadioButton* m_pClassicControls;

    CGUIComboBox* m_pInterfaceLanguageSelector;
    CGUIComboBox* m_pInterfaceSkinSelector;
    CGUIButton*   m_pInterfaceLoadSkin;

    CGUIComboBox* m_pChatPresets;
    CGUIButton*   m_pChatLoadPreset;

    CGUIScrollBar* m_pChatRed[Chat::ColorType::MAX];
    CGUIScrollBar* m_pChatGreen[Chat::ColorType::MAX];
    CGUIScrollBar* m_pChatBlue[Chat::ColorType::MAX];
    CGUIScrollBar* m_pChatAlpha[Chat::ColorType::MAX];

    CGUILabel* m_pChatRedValue[Chat::ColorType::MAX];
    CGUILabel* m_pChatGreenValue[Chat::ColorType::MAX];
    CGUILabel* m_pChatBlueValue[Chat::ColorType::MAX];
    CGUILabel* m_pChatAlphaValue[Chat::ColorType::MAX];

    CGUIStaticImage* m_pChatColorPreview[Chat::ColorType::MAX];

    CGUIScrollPane*  m_pPaneChatFont;
    CGUIRadioButton* m_pRadioChatFont[Chat::Font::MAX];

    CGUIComboBox* m_pChatHorizontalCombo;
    CGUIComboBox* m_pChatVerticalCombo;
    CGUIComboBox* m_pChatTextAlignCombo;
    CGUIEdit*     m_pChatOffsetX;
    CGUIEdit*     m_pChatOffsetY;

    CGUIEdit* m_pChatLines;
    CGUIEdit* m_pChatScaleX;
    CGUIEdit* m_pChatScaleY;
    CGUIEdit* m_pChatWidth;

    CGUICheckBox* m_pChatCssBackground;
    CGUICheckBox* m_pChatNickCompletion;
    CGUICheckBox* m_pChatCssText;
    CGUICheckBox* m_pChatTextBlackOutline;
    CGUIEdit*     m_pChatLineLife;
    CGUIEdit*     m_pChatLineFadeout;
    CGUICheckBox* m_pFlashWindow;
    CGUICheckBox* m_pTrayBalloon;

    CGUILabel*    m_pLabelBrowserGeneral;
    CGUICheckBox* m_pCheckBoxRemoteBrowser;
    CGUICheckBox* m_pCheckBoxRemoteJavascript;
    CGUILabel*    m_pLabelBrowserCustomBlacklist;
    CGUIEdit*     m_pEditBrowserBlacklistAdd;
    CGUILabel*    m_pLabelBrowserBlacklistAdd;
    CGUIButton*   m_pButtonBrowserBlacklistAdd;
    CGUIGridList* m_pGridBrowserBlacklist;
    CGUIButton*   m_pButtonBrowserBlacklistRemove;
    CGUILabel*    m_pLabelBrowserCustomWhitelist;
    CGUIEdit*     m_pEditBrowserWhitelistAdd;
    CGUILabel*    m_pLabelBrowserWhitelistAdd;
    CGUIButton*   m_pButtonBrowserWhitelistAdd;
    CGUIGridList* m_pGridBrowserWhitelist;
    CGUIButton*   m_pButtonBrowserWhitelistRemove;
    CGUICheckBox* m_pCheckBoxBrowserGPUEnabled;
    bool          m_bBrowserListsChanged;
    bool          m_bBrowserListsLoadEnabled;

    bool OnJoypadTextChanged(CGUIElement* pElement);
    bool OnAxisSelectClick(CGUIElement* pElement);
    bool OnAudioDefaultClick(CGUIElement* pElement);
    bool OnControlsDefaultClick(CGUIElement* pElement);
    bool OnBindsDefaultClick(CGUIElement* pElement);
    bool OnVideoDefaultClick(CGUIElement* pElement);
    bool OnBindsListClick(CGUIElement* pElement);
    bool OnOKButtonClick(CGUIElement* pElement);
    bool OnNickButtonClick(CGUIElement* pElement);
    bool OnCancelButtonClick(CGUIElement* pElement);
    bool OnFieldOfViewChanged(CGUIElement* pElement);
    bool OnDrawDistanceChanged(CGUIElement* pElement);
    bool OnBrightnessChanged(CGUIElement* pElement);
    bool OnAnisotropicChanged(CGUIElement* pElement);
    bool OnMapAlphaChanged(CGUIElement* pElement);
    bool OnMasterVolumeChanged(CGUIElement* pElement);
    bool OnRadioVolumeChanged(CGUIElement* pElement);
    bool OnSFXVolumeChanged(CGUIElement* pElement);
    bool OnMTAVolumeChanged(CGUIElement* pElement);
    bool OnVoiceVolumeChanged(CGUIElement* pElement);
    bool OnMasterMuteMinimizedChanged(CGUIElement* pElement);
    bool OnRadioMuteMinimizedChanged(CGUIElement* pElement);
    bool OnSFXMuteMinimizedChanged(CGUIElement* pElement);
    bool OnMTAMuteMinimizedChanged(CGUIElement* pElement);
    bool OnVoiceMuteMinimizedChanged(CGUIElement* pElement);
    bool OnChatRedChanged(CGUIElement* pElement);
    bool OnChatGreenChanged(CGUIElement* pElement);
    bool OnChatBlueChanged(CGUIElement* pElement);
    bool OnChatAlphaChanged(CGUIElement* pElement);
    bool OnUpdateButtonClick(CGUIElement* pElement);
    bool OnCachePathShowButtonClick(CGUIElement* pElement);
    bool OnMouseSensitivityChanged(CGUIElement* pElement);
    bool OnVerticalAimSensitivityChanged(CGUIElement* pElement);
    bool OnBrowserBlacklistAdd(CGUIElement* pElement);
    bool OnBrowserBlacklistRemove(CGUIElement* pElement);
    bool OnBrowserBlacklistDomainAddFocused(CGUIElement* pElement);
    bool OnBrowserBlacklistDomainAddDefocused(CGUIElement* pElement);
    bool OnBrowserWhitelistAdd(CGUIElement* pElement);
    bool OnBrowserWhitelistRemove(CGUIElement* pElement);
    bool OnBrowserWhitelistDomainAddFocused(CGUIElement* pElement);
    bool OnBrowserWhitelistDomainAddDefocused(CGUIElement* pElement);

    bool OnMouseDoubleClick(CGUIMouseEventArgs Args);

    bool OnChatLoadPresetClick(CGUIElement* pElement);

    bool OnLanguageChanged(CGUIElement* pElement);
    bool OnSkinChanged(CGUIElement* pElement);

    bool OnFxQualityChanged(CGUIElement* pElement);
    bool OnVolumetricShadowsClick(CGUIElement* pElement);
    bool OnAllowScreenUploadClick(CGUIElement* pElement);
    bool OnAllowExternalSoundsClick(CGUIElement* pElement);
    bool OnAllowDiscordRPC(CGUIElement* pElement);
    bool OnCustomizedSAFilesClick(CGUIElement* pElement);
    bool ShowUnsafeResolutionsClick(CGUIElement* pElement);
    bool OnWindowedClick(CGUIElement* pElement);
    bool OnDPIAwareClick(CGUIElement* pElement);
    bool OnShowAdvancedSettingDescription(CGUIElement* pElement);
    bool OnHideAdvancedSettingDescription(CGUIElement* pElement);
    bool OnTabChanged(CGUIElement* pElement);
    bool OnAffinityClick(CGUIElement* pElement);
    void ReloadBrowserLists();

private:
    void CreateInterfaceTabGUI();
    void UpdateChatColorPreview(eChatColorType eType);

    void ProcessKeyBinds();
    void ProcessJoypad();

    void SaveData();

    void LoadSkins();

    void   LoadChatPresets();
    void   CreateChatColorTab(eChatColorType eType, const char* szName, CGUITabPanel* pParent);
    void   LoadChatColorFromCVar(eChatColorType eType, const char* szCVar);
    void   LoadChatColorFromString(eChatColorType eType, const std::string& strColor);
    void   SaveChatColor(eChatColorType eType, const char* szCVar);
    CColor GetChatColorValues(eChatColorType eType);
    void   SetChatColorValues(eChatColorType eType, CColor pColor);
    int    GetMilliseconds(CGUIEdit* pEdit);
    void   SetMilliseconds(CGUIEdit* pEdit, int milliseconds);

    void ResetGTAVolume();
    void SetRadioVolume(float fVolume);
    void SetSFXVolume(float fVolume);

    unsigned int m_uiCaptureKey;
    bool         m_bCaptureKey;
    bool         m_bCaptureAxis;

    bool m_bIsModLoaded;

    float m_fRadioVolume;
    float m_fSFXVolume;

    float m_fOldMasterVolume;
    float m_fOldRadioVolume;
    float m_fOldSFXVolume;
    float m_fOldMTAVolume;
    float m_fOldVoiceVolume;

    bool m_bOldMuteMaster;
    bool m_bOldMuteSFX;
    bool m_bOldMuteRadio;
    bool m_bOldMuteMTA;
    bool m_bOldMuteVoice;

    bool m_bMuteMaster;
    bool m_bMuteSFX;
    bool m_bMuteRadio;
    bool m_bMuteMTA;
    bool m_bMuteVoice;

    CGUIListItem* m_pSelectedBind;

    DWORD m_dwFrameCount;
    bool  m_bShownVolumetricShadowsWarning;
    bool  m_bShownAllowScreenUploadMessage;
    bool  m_bShownAllowExternalSoundsMessage;
    int   m_iMaxAnisotropic;

    std::list<SKeyBindSection*> m_pKeyBindSections;
};
