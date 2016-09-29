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

enum
{
    FULLSCREEN_STANDARD,
    FULLSCREEN_BORDERLESS,
    FULLSCREEN_BORDERLESS_KEEP_RES,
    FULLSCREEN_BORDERLESS_STRETCHED,
};


class CInterfaceSettingsTab
{
public:
    CInterfaceSettingsTab(CGUITab* pTab, CSettings* pSettings);
    void SaveData();
    void LoadData();
    void                LoadChatPresets(void);
    void                LoadSkins(void);
    void SetIsModLoaded(bool bLoaded) { m_bIsModLoaded = bLoaded; }
private:
    
    void                CreateChatColorTab(ChatColorType eType, const char* szName, CGUITabPanel* pParent);
    void                LoadChatColorFromCVar(ChatColorType eType, const char* szCVar);
    void                LoadChatColorFromString(ChatColorType eType, const std::string& strColor);
    void                SaveChatColor(ChatColorType eType, const char* szCVar);
    CColor              GetChatColorValues(ChatColorType eType);
    void                SetChatColorValues(ChatColorType eType, CColor pColor);


    bool                OnChatRedChanged(CGUIElement* pElement);
    bool                OnChatGreenChanged(CGUIElement* pElement);
    bool                OnChatBlueChanged(CGUIElement* pElement);
    bool                OnChatAlphaChanged(CGUIElement* pElement);
    int                 GetMilliseconds(CGUIEdit* pEdit);
    void                SetMilliseconds(CGUIEdit* pEdit, int milliseconds);


    bool                OnChatLoadPresetClick(CGUIElement* pElement);

    bool                OnLanguageChanged(CGUIElement* pElement);
    bool                OnSkinChanged(CGUIElement* pElement);

    CGUIElement* m_pSettingsWindow;

    std::unique_ptr<CGUITab> m_pTab;


    bool                m_bIsModLoaded;
    std::unique_ptr<CGUIComboBox>       m_pInterfaceLanguageSelector;
    std::unique_ptr<CGUIComboBox>       m_pInterfaceSkinSelector;
    std::unique_ptr<CGUIButton>         m_pInterfaceLoadSkin;

    std::unique_ptr<CGUIComboBox>       m_pChatPresets;
    std::unique_ptr<CGUIButton>         m_pChatLoadPreset;

    std::unique_ptr<CGUIScrollBar>      m_pChatRed[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUIScrollBar>      m_pChatGreen[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUIScrollBar>      m_pChatBlue[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUIScrollBar>      m_pChatAlpha[ChatColorTypes::CHAT_COLOR_MAX];

    std::unique_ptr<CGUILabel>          m_pChatRedLabel[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUILabel>          m_pChatGreenLabel[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUILabel>          m_pChatBlueLabel[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUILabel>          m_pChatAlphaLabel[ChatColorTypes::CHAT_COLOR_MAX];


    std::unique_ptr<CGUILabel>          m_pChatRedValue[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUILabel>          m_pChatGreenValue[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUILabel>          m_pChatBlueValue[ChatColorTypes::CHAT_COLOR_MAX];
    std::unique_ptr<CGUILabel>          m_pChatAlphaValue[ChatColorTypes::CHAT_COLOR_MAX];

    std::unique_ptr<CGUIScrollPane>     m_pPaneChatFont;
    std::unique_ptr<CGUIRadioButton>    m_pRadioChatFont[ChatFonts::CHAT_FONT_MAX];

    std::unique_ptr<CGUIEdit>           m_pChatLines;
    std::unique_ptr<CGUIEdit>           m_pChatScaleX;
    std::unique_ptr<CGUIEdit>           m_pChatScaleY;
    std::unique_ptr<CGUIEdit>           m_pChatWidth;

    std::unique_ptr<CGUICheckBox>       m_pChatCssBackground;
    std::unique_ptr<CGUICheckBox>       m_pChatNickCompletion;
    std::unique_ptr<CGUICheckBox>       m_pChatCssText;
    std::unique_ptr<CGUIEdit>           m_pChatLineLife;
    std::unique_ptr<CGUIEdit>           m_pChatLineFadeout;
    std::unique_ptr<CGUICheckBox>       m_pFlashWindow;
    std::unique_ptr<CGUICheckBox>       m_pTrayBalloon;
    std::unique_ptr<CGUILabel>          m_pLanguageLabel;
    std::unique_ptr<CGUILabel>          m_pChatLabel;
    std::unique_ptr<CGUILabel>          m_pSkinLabel;
    std::unique_ptr<CGUILabel>          m_pPresetsLabel;
    std::unique_ptr<CGUITabPanel>       m_pColorTabPanel;
    std::unique_ptr<CGUITabPanel>       m_pChatOptionsPanel;
    std::unique_ptr<CGUILabel>          m_pFontLabel;

    std::unique_ptr<CGUITab>            m_pLayoutTab;
    std::unique_ptr<CGUILabel>          m_pLayoutTabLinesLabel;
    std::unique_ptr<CGUILabel>          m_pLayoutTabScaleLabel;
    std::unique_ptr<CGUILabel>          m_pLayoutTabXLabel;
    std::unique_ptr<CGUILabel>          m_pLayoutTabWidthLabel;
   
    std::unique_ptr<CGUITab>            m_pFadingTab;
    std::unique_ptr<CGUILabel>          m_pFadingTabAfterLabel;
    std::unique_ptr<CGUILabel>          m_pFadingTabSecLabel;
    std::unique_ptr<CGUILabel>          m_pFadingTabForLabel;
    std::unique_ptr<CGUILabel>          m_pFadingTabSecLabel2;

    std::unique_ptr<CGUILabel>          m_pOptionsLabel;
};
