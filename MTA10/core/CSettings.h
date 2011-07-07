/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CSettings.h
*  PURPOSE:     Header file for in-game settings window class
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

class CSettings;

#ifndef __CSETTINGS_H
#define __CSETTINGS_H

#include <core/CCoreInterface.h>
#include "CMainMenu.h"
#include "CCore.h"

#define SKINS_PATH                    "skins/*"
#define CHAT_PRESETS_PATH             "mta/chatboxpresets.xml"
#define CHAT_PRESETS_ROOT             "chatboxpresets"

struct SKeyBindSection
{
    SKeyBindSection ( char * szTitle )
    {
        this->currentIndex = 0;
        this->szTitle = new char [ strlen ( szTitle ) + 1 ];
        this->szOriginalTitle = new char [ strlen ( szTitle ) + 1 ];
        // convert to upper case
        for ( unsigned int i = 0; i < strlen(szTitle); i++ )
        {
            if ( isalpha(szTitle[i]) )
                this->szTitle[i] = toupper(szTitle[i]);
            else
                this->szTitle[i] = szTitle[i];
        }

        this->szTitle[strlen(szTitle)] = '\0';

        strcpy ( szOriginalTitle, szTitle );

        this->rowCount = 0;

        headerItem = NULL;
    }

    ~SKeyBindSection ( )
    {
        if ( this->szTitle )
            delete[] this->szTitle;

        if ( szOriginalTitle )
            delete[] szOriginalTitle;
    }
    int currentIndex; // temporarily stores the index while the list is being created
    char* szTitle;
    char* szOriginalTitle;
    int rowCount;
    class CGUIListItem * headerItem;
};

class CColor;

namespace ChatColorTypes
{
    enum ChatColorType
    {
        CHAT_COLOR_BG = 0,
        CHAT_COLOR_TEXT,
        CHAT_COLOR_INPUT_BG,
        CHAT_COLOR_INPUT_TEXT,
        CHAT_COLOR_MAX
    };
}

using ChatColorTypes::ChatColorType;

class CSettings
{
    friend class CCore;

public:
                        CSettings               ( void );
                        ~CSettings              ( void );

    bool                ProcessMessage          ( UINT uMsg, WPARAM wParam, LPARAM lParam );

    void                Update                  ( void );
    void                Initialize              ( void );

    void                SetVisible              ( bool bVisible );
    bool                IsVisible               ( void );

    void                SetIsModLoaded          ( bool bLoaded );

    void                LoadData                ( void );

    inline bool         IsCapturingKey          ( void )            { return m_bCaptureKey; }
    void                UpdateCaptureAxis       ();
    void                UpdateJoypadTab         ();

    void                UpdateAudioTab          ();

    void                UpdateVideoTab          ( bool bIsVideoModeChanged = false );

    void                AddKeyBindSection       ( char * szSectionName );
    void                RemoveKeyBindSection    ( char * szSectionName );
    void                RemoveAllKeyBindSections ( void );

    static void         OnLoginCallback         ( bool bResult, char* szError, void* obj );
    void                OnLoginStateChange      ( bool bResult );

    void                RequestNewNickname      ( void );

protected:
    const static int    SecKeyNum = 3;     // Number of secondary keys

    // Keep these protected so we can access them in the event handlers of CClientGame
    CGUIWindow*         m_pWindow;
    CGUITabPanel*       m_pTabs;
    CGUIButton*         m_pButtonOK;
    CGUIButton*         m_pButtonCancel;
    CGUILabel*          m_pLabelNick;
    CGUIEdit*           m_pEditNick;
    CGUICheckBox*       m_pSavePasswords;
    CGUICheckBox*       m_pAutoRefreshBrowser;

    CGUILabel*          m_pVideoGeneralLabel;
    CGUILabel*          m_pVideoResolutionLabel;
    CGUIComboBox*       m_pComboResolution;
	CGUICheckBox*       m_pCheckBoxMipMapping;
    CGUICheckBox*       m_pCheckBoxWindowed;
    CGUICheckBox*       m_pCheckBoxMinimize;
    CGUICheckBox*       m_pCheckBoxDisableAero;
    CGUILabel*          m_pMapRenderingLabel;
    CGUIComboBox*       m_pComboFxQuality;
    CGUILabel*          m_pFXQualityLabel;
    CGUIComboBox*       m_pComboAspectRatio;
    CGUILabel*          m_pAspectRatioLabel;
	CGUICheckBox*       m_pCheckBoxVolumetricShadows;
    CGUILabel*          m_pDrawDistanceLabel;
    CGUIScrollBar*      m_pDrawDistance;
    CGUILabel*          m_pDrawDistanceValueLabel;
    CGUILabel*          m_pBrightnessLabel;
    CGUIScrollBar*      m_pBrightness;
    CGUILabel*          m_pBrightnessValueLabel;
    CGUIComboBox*       m_pComboAntiAliasing;
    CGUILabel*          m_pAntiAliasingLabel;
    CGUILabel*          m_pMapAlphaLabel;
    CGUIScrollBar*      m_pMapAlpha;
    CGUILabel*          m_pMapAlphaValueLabel;
    CGUILabel*          m_pStreamingMemoryLabel;
    CGUIScrollBar*      m_pStreamingMemory;
    CGUILabel*          m_pStreamingMemoryValueLabel;
    CGUIButton*         m_pVideoDefButton;

    CGUILabel*          m_pAsyncLabelInfo;
    CGUILabel*          m_pAsyncLabel;
    CGUIComboBox*       m_pAsyncCombo;
    CGUILabel*          m_pAudioGeneralLabel;
    CGUILabel*          m_pUserTrackGeneralLabel;
    CGUILabel*          m_pBrowserSpeedLabelInfo;
    CGUILabel*          m_pBrowserSpeedLabel;
    CGUIComboBox*       m_pBrowserSpeedCombo;
    CGUILabel*          m_pSingleDownloadLabelInfo;
    CGUILabel*          m_pSingleDownloadLabel;
    CGUIComboBox*       m_pSingleDownloadCombo;
    CGUILabel*          m_pCodePathLabelInfo;
    CGUILabel*          m_pCodePathLabel;
    CGUIComboBox*       m_pCodePathCombo;
    CGUILabel*          m_pUpdateBuildTypeLabelInfo;
    CGUILabel*          m_pUpdateBuildTypeLabel;
    CGUIComboBox*       m_pUpdateBuildTypeCombo;
    CGUIButton*         m_pButtonUpdate;
    CGUILabel*          m_pAdvancedMiscLabel;
    CGUILabel*          m_pAdvancedUpdaterLabel;

    CGUILabel*          m_pLabelRadioVolume;
    CGUILabel*          m_pLabelSFXVolume;
    CGUILabel*          m_pLabelMTAVolume;
    CGUILabel*          m_pLabelRadioVolumeValue;
    CGUILabel*          m_pLabelSFXVolumeValue;
    CGUILabel*          m_pLabelMTAVolumeValue;
    CGUIScrollBar*      m_pAudioRadioVolume;
    CGUIScrollBar*      m_pAudioSFXVolume;
    CGUIScrollBar*      m_pAudioMTAVolume;
    CGUICheckBox*       m_pCheckBoxAudioEqualizer;
    CGUICheckBox*       m_pCheckBoxAudioAutotune;
    CGUILabel*          m_pAudioUsertrackLabel;
    CGUICheckBox*       m_pCheckBoxUserAutoscan;
    CGUILabel*          m_pLabelUserTrackMode;
    CGUIComboBox*       m_pComboUsertrackMode;
    CGUIButton*         m_pAudioDefButton;

    CGUIGridList*       m_pBindsList;
    CGUIButton*         m_pBindsDefButton;
    CGUIHandle          m_hBind, m_hPriKey, m_hSecKeys[SecKeyNum];

    CGUILabel*                  m_pJoypadName;
    CGUILabel*                  m_pJoypadUnderline;
    CGUIEdit*                   m_pEditDeadzone;
    CGUIEdit*                   m_pEditSaturation;
    std::vector < CGUILabel* >  m_pJoypadLabels;
    std::vector < CGUIButton* > m_pJoypadButtons;
    int                         m_JoypadSettingsRevision;

    CGUILabel*          m_pLabelCommunity;
    CGUILabel*          m_pLabelUser;
    CGUILabel*          m_pLabelPass;
    CGUIEdit*           m_pEditUser;
    CGUIEdit*           m_pEditPass;
    CGUIButton*         m_pButtonLogin;
    CGUIButton*         m_pButtonRegister;

    CGUILabel*          m_pControlsMouseLabel;
    CGUICheckBox*       m_pInvertMouse;
    CGUICheckBox*       m_pSteerWithMouse;
    CGUICheckBox*       m_pFlyWithMouse;
    CGUILabel*          m_pLabelMouseSensitivity;
    CGUIScrollBar*      m_pMouseSensitivity;
    CGUILabel*          m_pLabelMouseSensitivityValue;

    CGUILabel*          m_pControlsJoypadLabel;
    CGUIScrollPane*     m_pControlsInputTypePane;
    CGUIRadioButton*    m_pStandardControls;
    CGUIRadioButton*    m_pClassicControls;

    CGUIComboBox*       m_pInterfaceSkinSelector;
    CGUIButton*         m_pInterfaceLoadSkin;

    CGUIComboBox*       m_pChatPresets;
    CGUIButton*         m_pChatLoadPreset;

    CGUIScrollBar*      m_pChatRed          [ ChatColorTypes::CHAT_COLOR_MAX ];
    CGUIScrollBar*      m_pChatGreen        [ ChatColorTypes::CHAT_COLOR_MAX ];
    CGUIScrollBar*      m_pChatBlue         [ ChatColorTypes::CHAT_COLOR_MAX ];
    CGUIScrollBar*      m_pChatAlpha        [ ChatColorTypes::CHAT_COLOR_MAX ];

    CGUILabel*          m_pChatRedValue     [ ChatColorTypes::CHAT_COLOR_MAX ];
    CGUILabel*          m_pChatGreenValue   [ ChatColorTypes::CHAT_COLOR_MAX ];
    CGUILabel*          m_pChatBlueValue    [ ChatColorTypes::CHAT_COLOR_MAX ];
    CGUILabel*          m_pChatAlphaValue   [ ChatColorTypes::CHAT_COLOR_MAX ];

    CGUIScrollPane*     m_pInterfacePaneScroller;

    CGUIScrollPane*     m_pPaneChatFont;
    CGUIRadioButton*    m_pRadioChatFont    [ ChatFonts::CHAT_FONT_MAX ];

    CGUIEdit*           m_pChatLines;
    CGUIEdit*           m_pChatScaleX;
    CGUIEdit*           m_pChatScaleY;
    CGUIEdit*           m_pChatWidth;

    CGUICheckBox*       m_pChatCssBackground;
    CGUICheckBox*       m_pChatCssText;
    CGUIEdit*           m_pChatLineLife;
    CGUIEdit*           m_pChatLineFadeout;

    bool                OnJoypadTextChanged     ( CGUIElement* pElement );
    bool                OnAxisSelectClick       ( CGUIElement* pElement );
    bool                OnAudioDefaultClick     ( CGUIElement* pElement );
    bool                OnJoypadDefaultClick    ( CGUIElement* pElement );
    bool                OnBindsDefaultClick     ( CGUIElement* pElement );
    bool                OnVideoDefaultClick     ( CGUIElement* pElement );
    bool                OnBindsListClick        ( CGUIElement* pElement );
    bool                OnOKButtonClick         ( CGUIElement* pElement );
    bool                OnCancelButtonClick     ( CGUIElement* pElement );
    bool                OnLoginButtonClick      ( CGUIElement* pElement );
    bool                OnRegisterButtonClick   ( CGUIElement* pElement );
    bool                OnDrawDistanceChanged   ( CGUIElement* pElement );
    bool                OnBrightnessChanged     ( CGUIElement* pElement );
    bool                OnStreamingMemoryChanged( CGUIElement* pElement );
    bool                OnMapAlphaChanged       ( CGUIElement* pElement );
    bool                OnRadioVolumeChanged    ( CGUIElement* pElement );
    bool                OnSFXVolumeChanged      ( CGUIElement* pElement );
    bool                OnMTAVolumeChanged      ( CGUIElement* pElement );
    bool                OnChatRedChanged        ( CGUIElement* pElement );
    bool                OnChatGreenChanged      ( CGUIElement* pElement );
    bool                OnChatBlueChanged       ( CGUIElement* pElement );
    bool                OnChatAlphaChanged      ( CGUIElement* pElement );
    bool                OnUpdateButtonClick     ( CGUIElement* pElement );
    bool                OnMouseSensitivityChanged ( CGUIElement* pElement );

    bool                OnMouseDoubleClick      ( CGUIMouseEventArgs Args );

    bool                OnChatLoadPresetClick   ( CGUIElement* pElement );

    bool                OnSkinChanged ( CGUIElement* pElement );

    bool                OnFxQualityChanged      ( CGUIElement* pElement );
    bool                OnVolumetricShadowsClick ( CGUIElement* pElement );

private:
    void                ProcessKeyBinds         ( void );
    void                ProcessJoypad           ( void );

    void                SaveData                ( void );

    void                LoadSkins               ( void );

    void                LoadChatPresets         ( void );
    void                CreateChatColorTab      ( ChatColorType eType, const char* szName, CGUITabPanel* pParent );
    void                LoadChatColorFromCVar   ( ChatColorType eType, const char* szCVar );
    void                LoadChatColorFromString ( ChatColorType eType, const std::string& strColor );
    void                SaveChatColor           ( ChatColorType eType, const char* szCVar );
    CColor              GetChatColorValues      ( ChatColorType eType );
    void                SetChatColorValues      ( ChatColorType eType, CColor pColor );
    int                 GetMilliseconds         ( CGUIEdit* pEdit );
    void                SetMilliseconds         ( CGUIEdit* pEdit, int milliseconds );

    unsigned int        m_uiCaptureKey;
    bool                m_bCaptureKey;
    bool                m_bCaptureAxis;

    bool                m_bIsModLoaded;

    unsigned char       m_ucOldRadioVolume;
    unsigned char       m_ucOldSFXVolume;
    float               m_fOldMTAVolume;

    CGUIListItem*	    m_pSelectedBind;

    DWORD               m_dwFrameCount;
    bool                m_bShownVolumetricShadowsWarning;

    std::list < SKeyBindSection *> m_pKeyBindSections;

};

#endif
