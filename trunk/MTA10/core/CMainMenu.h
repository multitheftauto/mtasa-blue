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
#include "CServerBrowser.h"
#include "CSettings.h"
#include "CCredits.h"
#include "CGraphics.h"
#include "CMainMenuScene.h"

#define CORE_MTA_MENU_ITEMS		8

class CMainMenu
{
    friend CQuickConnect;
    friend CServerBrowser;
    friend CSettings;
    friend CCredits;

public:
	// Menu display options
	typedef enum eMenuOptions {
		MENU_DYNAMIC = 1,
		MENU_VIDEO_ENABLED = 2,
		MENU_POSTEFFECTS_ENABLED = 4,
		MENU_FULL = 0xFF
	};

                        CMainMenu                       ( CGUI* pManager );
                        ~CMainMenu                      ( void );

    void                RefreshPositions                ( void );

    void                Update                          ( void );

	void				Show							( bool bOverlay );
	void				Hide							( void );

	void                SetVisible                      ( bool bVisible, bool bOverlay = true );
    bool                IsVisible                       ( void );
    bool                IsFading                        ( void )    { return m_ucFade == FADE_IN || m_ucFade == FADE_OUT; }

    void                SetIsIngame                     ( bool bIsIngame );
    bool                GetIsIngame                     ( void );

    void                SetServerBrowserVisible         ( bool bVisible );

    CServerBrowser*     GetServerBrowser                ( void ) { return &m_ServerBrowser; };
    CSettings*          GetSettingsWindow               ( void ) { return &m_Settings; };
    CQuickConnect*      GetQuickConnectWindow           ( void ) { return &m_QuickConnect; };

	void				OnInvalidate					( IDirect3DDevice9 * pDevice );
	void				OnRestore						( IDirect3DDevice9 * pDevice );

	static int			GetMenuOptionsDefault			( void ) { return (MENU_FULL); };

	void				LoadMenuOptions					( void );

    void                ChangeCommunityState            ( bool bIn, std::string strUsername );

private:
	void				SetStaticBackground				( bool bEnabled );

	void				CreateItem						( unsigned int uiIndex, CVector2D vecPosition, const char *szText, GUI_CALLBACK pHandler );
    void				SetItemPosition					( unsigned int uiIndex, CVector2D vecPosition, bool bRelative = false );
	void				EnableItem						( unsigned int uiIndex );
	void				DisableItem						( unsigned int uiIndex, bool bHide = false );

	bool                OnQuickConnectButtonClick       ( CGUIElement* pElement );
    bool                OnResumeButtonClick             ( CGUIElement* pElement );
    bool                OnBrowseServersButtonClick      ( CGUIElement* pElement );
    bool                OnHostGameButtonClick           ( CGUIElement* pElement );
    bool                OnDisconnectButtonClick         ( CGUIElement* pElement );
    bool                OnEditorButtonClick             ( CGUIElement* pElement );
    bool                OnSettingsButtonClick           ( CGUIElement* pElement );
    bool                OnAboutButtonClick              ( CGUIElement* pElement );
    bool                OnQuitButtonClick               ( CGUIElement* pElement );

	bool				OnItemEnter						( CGUIElement* pElement );
	bool				OnItemLeave						( CGUIElement* pElement );

    CGUI*				m_pManager;

	// Images
    CGUIStaticImage*    m_pBackground;
	CGUIStaticImage*	m_pHeader;
	CGUIStaticImage*    m_pFiller;

	// Main menu pane and items
	CGUILabel*			m_pItems[CORE_MTA_MENU_ITEMS];
	unsigned int		m_uiItems;

	// Submenu classes
    CQuickConnect       m_QuickConnect;
    CSettings           m_Settings;
    CCredits            m_Credits;
    CServerBrowser      m_ServerBrowser;

	// Properties
    bool                m_bIsIngame;
    bool                m_bIsVisible;
    bool                m_bIsInSubWindow;

	int					m_iButtons;
	float				m_fWindowX;
	float				m_fWindowY;
	float				m_fButtonSpacer;
	float				m_fButtonSpacerTop;
	float				m_fButtonSpacerLeft;
	float				m_fButtonWidth;
	float				m_fButtonHeight;

	CGraphics *			m_pGraphics;
	bool				m_bInitialized;
	bool				m_bStarted;

	CMainMenuScene *	m_pMainMenuScene;

	CGUITexture *		m_pRenderTarget;
	bool				m_bStaticBackground;

    // Community
    CGUILabel*          m_pCommunityLabel;

	// Fade variables
	bool				m_bFadeToCredits;
	unsigned char		m_ucFade;
	float				m_fFader;

	// Main menu items
	enum eMenuItems {
		MENU_ITEM_DISCONNECT,
		MENU_ITEM_QUICK_CONNECT,
		MENU_ITEM_BROWSE_SERVERS,
        MENU_ITEM_HOST_GAME,
		MENU_ITEM_MAP_EDITOR,
        MENU_ITEM_SETTINGS,
		MENU_ITEM_ABOUT,
		MENU_ITEM_QUIT
	};

	// Fade states
	enum eFadeStates {
		FADE_VISIBLE,
		FADE_INVISIBLE,
		FADE_IN,
		FADE_OUT
	};

	// Item states
	enum eItemState {
		ITEM_ENABLED,
		ITEM_DISABLED,
		ITEM_HIDDEN
	};
};

#endif
