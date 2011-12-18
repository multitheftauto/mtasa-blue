/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CMainMenu.h
*  PURPOSE:     Header file for main menu graphical user interface class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Christian Myhre Lundheim <>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CMainMenu;

#ifndef __CMAINMENU_H
#define __CMAINMENU_H

#include "CCore.h"
#include "CQuickConnect.h"
#include "CQuestionBox.h"
#include "CServerBrowser.h"
#include "CServerInfo.h"
#include "CSettings.h"
#include "CCredits.h"
#include "CGraphics.h"
#include "CMainMenuScene.h"
#include <google/dense_hash_map>
class CNewsBrowser;

#define CORE_MTA_NEWS_ITEMS         3

struct sMenuItem
{
    unsigned char                 menuType;
    int                           drawPositionX;
    int                           drawPositionY;
    int                           nativeSizeX;
    int                           nativeSizeY;
    CGUIStaticImage*              image;
};

class CMainMenu
{
    friend class CQuickConnect;
    friend class CServerBrowser;
    friend class CSettings;
    friend class CCredits;

public:
                        CMainMenu                       ( CGUI* pManager );
                        ~CMainMenu                      ( void );

    void                Update                          ( void );

    void                Show                            ( bool bOverlay );
    void                Hide                            ( void );

    void                SetVisible                      ( bool bVisible, bool bOverlay = true, bool bFrameDelay = true );
    bool                IsVisible                       ( void );
    bool                IsFading                        ( void )    { return m_ucFade == FADE_IN || m_ucFade == FADE_OUT; }

    void                SetIsIngame                     ( bool bIsIngame );
    bool                GetIsIngame                     ( void );

    CServerBrowser*     GetServerBrowser                ( void ) { return &m_ServerBrowser; };
    CSettings*          GetSettingsWindow               ( void ) { return &m_Settings; };
    CQuickConnect*      GetQuickConnectWindow           ( void ) { return &m_QuickConnect; };
    CQuestionBox*       GetQuestionWindow               ( void ) { return &m_QuestionBox; };
    CNewsBrowser*       GetNewsBrowser                  ( void ) { return m_pNewsBrowser; };

    void                SetMenuVerticalPosition         ( int iPosY );
    void                SetMenuUnhovered                ( void );

    void                ChangeCommunityState            ( bool bIn, const std::string& strUsername );
    bool                HasStarted                      ( void ) { return m_bStarted; };

    void                SetNewsHeadline                 ( int iIndex, const SString& strHeadline, const SString& strDate, bool bIsNew );
    void                OnEscapePressedOffLine          ( void );

private:
    sMenuItem*          CreateItem                      ( unsigned char menuType, const char* szFilePath, CVector2D vecRelPosition, CVector2D vecNativeSize );
    bool                SetItemHoverProgress            ( sMenuItem* pItem, float fProgress, bool bHovering );

    bool                OnMenuEnter                     ( CGUIElement* pElement );
    bool                OnMenuExit                      ( CGUIElement* pElement );
    bool                OnMenuClick                     ( CGUIElement* pElement );
    bool                OnQuickConnectButtonClick       ( CGUIElement* pElement );
    bool                OnResumeButtonClick             ( CGUIElement* pElement );
    bool                OnBrowseServersButtonClick      ( CGUIElement* pElement );
    bool                OnHostGameButtonClick           ( CGUIElement* pElement );
    bool                OnDisconnectButtonClick         ( CGUIElement* pElement );
    bool                OnEditorButtonClick             ( CGUIElement* pElement );
    bool                OnSettingsButtonClick           ( CGUIElement* pElement );
    bool                OnAboutButtonClick              ( CGUIElement* pElement );
    bool                OnQuitButtonClick               ( CGUIElement* pElement );
    bool                OnNewsButtonClick               ( CGUIElement* pElement );

    void                HideServerInfo                  ( void );

    CGUI*               m_pManager;

    // Images
    CGUIStaticImage*    m_pBackground;
    CGUIStaticImage*    m_pLogo;
    CGUIStaticImage*    m_pLatestNews;
    CGUIStaticImage*    m_pFiller;
    CGUIStaticImage*    m_pFiller2;
    CGUIStaticImage*    m_pVersion;
    CGUIStaticImage*    m_pMenuArea;

    CGUIScrollPane*     m_pCanvas;

    std::deque < sMenuItem* >    m_menuItems;
    std::set < sMenuItem* >      m_unhoveredItems;
    sMenuItem*                   m_pDisconnect;
    sMenuItem*                   m_pHoveredItem;
    bool                         m_bMouseOverMenu;

    // Submenu classes
    CQuestionBox        m_QuestionBox;
    CQuickConnect       m_QuickConnect;
    CSettings           m_Settings;
    CNewsBrowser*       m_pNewsBrowser;
    CCredits            m_Credits;
    CServerBrowser      m_ServerBrowser;
    CServerInfo         m_ServerInfo;

    // Properties
    bool                m_bIsIngame;
    bool                m_bIsVisible;
    bool                m_bIsFullyVisible;
    bool                m_bFrameDelay;
    bool                m_bIsInSubWindow;
    bool                m_bHideGame;

    int                 m_iXOff;
    int                 m_iYOff;
    int                 m_iMenuSizeX;
    int                 m_iMenuSizeY;
    int                 m_iFirstItemCentre;
    int                 m_iSecondItemCentre;
    //Define our bounding box for menu items
    int                 m_menuAX;
    int                 m_menuAY;
    int                 m_menuBX;
    int                 m_menuBY;

    CGraphics *         m_pGraphics;
    bool                m_bStarted;
    CVector2D           m_ScreenSize;

    // Community
    CGUILabel*          m_pCommunityLabel;

    // Fade variables
    unsigned char       m_ucFade;
    float               m_fFader;

    // Animation variables
    unsigned long       ulPreviousTick;
    int                 m_iMoveStartPos;
    int                 m_iMoveTargetPos;
    unsigned long       m_ulMoveStartTick;

    CGUILabel*          m_pNewsItemLabels[CORE_MTA_NEWS_ITEMS];
    CGUILabel*          m_pNewsItemShadowLabels[CORE_MTA_NEWS_ITEMS];
    CGUILabel*          m_pNewsItemDateLabels[CORE_MTA_NEWS_ITEMS];
    CGUILabel*          m_pNewsItemNEWLabels[CORE_MTA_NEWS_ITEMS];

    // Fade states
    enum eFadeStates {
        FADE_VISIBLE,
        FADE_INVISIBLE,
        FADE_IN,
        FADE_OUT
    };

    // Main menu items
    enum eMenuItems {
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

#endif
