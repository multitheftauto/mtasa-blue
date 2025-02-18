/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CMainMenu.h
 *  PURPOSE:     Header file for main menu graphical user interface class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CMainMenu;

#pragma once

#include "CCore.h"
#include "CQuestionBox.h"
#include <ServerBrowser/CServerBrowser.h>
#include <ServerBrowser/CServerInfo.h>
#include "CSettings.h"
#include "CCredits.h"
#include <Graphics/CGraphics.h>
class CNewsBrowser;
class CLanguageSelector;

#define CORE_MTA_NEWS_ITEMS         3

struct sMenuItem
{
    unsigned char    menuType;
    int              drawPositionX;
    int              drawPositionY;
    int              nativeSizeX;
    int              nativeSizeY;
    CGUIStaticImage* image;
};

class CMainMenu
{
    friend class CServerBrowser;
    friend class CSettings;
    friend class CCredits;

public:
    CMainMenu(CGUI* pManager);
    ~CMainMenu();

    void Update();

    void Show(bool bOverlay);
    void Hide();

    void SetVisible(bool bVisible, bool bOverlay = true, bool bFrameDelay = true);
    bool IsVisible();
    bool IsFading() { return m_ucFade == FADE_IN || m_ucFade == FADE_OUT; }

    void SetIsIngame(bool bIsIngame);
    bool GetIsIngame();

    CServerBrowser* GetServerBrowser() { return &m_ServerBrowser; };
    CSettings*      GetSettingsWindow() { return &m_Settings; };
    CQuestionBox*   GetQuestionWindow() { return &m_QuestionBox; };
    CNewsBrowser*   GetNewsBrowser() { return m_pNewsBrowser; };

    void SetMenuVerticalPosition(int iPosY);
    void SetMenuUnhovered();

    bool HasStarted() { return m_bStarted; };

    void SetNewsHeadline(int iIndex, const SString& strHeadline, const SString& strDate, bool bIsNew);
    void ReloadNews();
    void OnEscapePressedOffLine();

    static void StaticWantsToDisconnectCallBack(void* pData, uint uiButton);
    void        WantsToDisconnectCallBack(void* pData, uint uiButton);
    void        AskUserIfHeWantsToDisconnect(uchar menuType);

private:
    sMenuItem* CreateItem(unsigned char menuType, const char* szFilename, CVector2D vecRelPosition);
    bool       SetItemHoverProgress(sMenuItem* pItem, float fProgress, bool bHovering);

    bool OnMenuEnter(CGUIElement* pElement);
    bool OnMenuExit(CGUIElement* pElement);
    bool OnMenuClick(CGUIMouseEventArgs Args);
    bool OnQuickConnectButtonClick(CGUIElement* pElement, bool left);
    bool OnResumeButtonClick(CGUIElement* pElement);
    bool OnBrowseServersButtonClick(CGUIElement* pElement);
    bool OnHostGameButtonClick();
    bool OnDisconnectButtonClick(CGUIElement* pElement);
    bool OnEditorButtonClick();
    bool OnSettingsButtonClick(CGUIElement* pElement);
    bool OnAboutButtonClick(CGUIElement* pElement);
    bool OnQuitButtonClick(CGUIElement* pElement);
    bool OnNewsButtonClick(CGUIElement* pElement);

    void HideServerInfo();

    CGUI* m_pManager;

    // Images
    CGUIStaticImage* m_pBackground;
    CGUIStaticImage* m_pLogo;
    CGUIStaticImage* m_pLatestNews;
    CGUIStaticImage* m_pFiller;
    CGUIStaticImage* m_pFiller2;
    CGUIStaticImage* m_pVersion;
    CGUIStaticImage* m_pMenuArea;

    CGUIScrollPane* m_pCanvas;

    std::deque<sMenuItem*> m_menuItems;
    std::set<sMenuItem*>   m_unhoveredItems;
    sMenuItem*             m_pDisconnect;
    sMenuItem*             m_pHoveredItem;
    bool                   m_bMouseOverMenu;

    // Submenu classes
    CQuestionBox       m_QuestionBox;
    CSettings          m_Settings;
    CNewsBrowser*      m_pNewsBrowser;
    CCredits           m_Credits;
    CServerBrowser     m_ServerBrowser;
    CServerInfo        m_ServerInfo;
    CLanguageSelector* m_pLanguageSelector;

    // Properties
    bool m_bIsIngame;
    bool m_bIsVisible;
    bool m_bIsFullyVisible;
    bool m_bFrameDelay;
    bool m_bIsInSubWindow;
    bool m_bHideGame;

    int m_iXOff;
    int m_iYOff;
    int m_iMenuSizeX;
    int m_iMenuSizeY;
    int m_iFirstItemCentre;
    int m_iSecondItemCentre;
    // Define our bounding box for menu items
    int m_menuAX;
    int m_menuAY;
    int m_menuBX;
    int m_menuBY;

    CGraphics* m_pGraphics;
    bool       m_bStarted;
    CVector2D  m_ScreenSize;

    // Fade variables
    unsigned char m_ucFade;
    float         m_fFader;
    bool          m_bCursorAlphaReset;

    // Animation variables
    unsigned long ulPreviousTick;
    int           m_iMoveStartPos;
    int           m_iMoveTargetPos;
    unsigned long m_ulMoveStartTick;

    CGUILabel* m_pNewsItemLabels[CORE_MTA_NEWS_ITEMS];
    CGUILabel* m_pNewsItemShadowLabels[CORE_MTA_NEWS_ITEMS];
    CGUILabel* m_pNewsItemDateLabels[CORE_MTA_NEWS_ITEMS];
    CGUILabel* m_pNewsItemNEWLabels[CORE_MTA_NEWS_ITEMS];

#ifdef CI_BUILD
    std::unique_ptr<CGUITexture>     m_pFeatureBranchAlertTexture;
    std::unique_ptr<CGUIStaticImage> m_pFeatureBranchAlertImage;
    std::unique_ptr<CGUILabel>       m_pFeatureBranchAlertLabel;
#endif

#if _WIN32_WINNT <= _WIN32_WINNT_WINXP
    std::unique_ptr<CGUITexture>     m_pAlertTexture;
    std::unique_ptr<CGUIStaticImage> m_pAlertImage;
    std::unique_ptr<CGUILabel>       m_pAlertLabel;
#endif

    // Fade states
    enum eFadeStates
    {
        FADE_VISIBLE,
        FADE_INVISIBLE,
        FADE_IN,
        FADE_OUT
    };

    // Main menu items
    enum eMenuItems
    {
        MENU_ITEM_DISCONNECT,
        MENU_ITEM_QUICK_CONNECT,
        MENU_ITEM_BROWSE_SERVERS,
        MENU_ITEM_HOST_GAME,
        MENU_ITEM_MAP_EDITOR,
        MENU_ITEM_SETTINGS,
        MENU_ITEM_ABOUT,
        MENU_ITEM_QUIT,
    };
};
