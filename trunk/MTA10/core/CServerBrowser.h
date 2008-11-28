/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerBrowser.h
*  PURPOSE:     Header file for server browser class
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Chris McArthur <>
*               Christian Myhre Lundheim <>
*               Ed Lyons <eai@opencoding.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerBrowser;

#ifndef __CSERVERBROWSER_H
#define __CSERVERBROWSER_H

#include "CMainMenu.h"
#include "CServerList.h"
#include "CSingleton.h"
#include "CFavouritesAddByIP.h"
#include "CCommunityLogin.h"
#include <ctime>

using namespace std;

// Amount of server lists/tabs (ServerBrowserType)
#define SERVER_BROWSER_TYPE_COUNT           4

// Server browser list update interval (in ms)
#define SERVER_BROWSER_UPDATE_INTERVAL      250

class CServerBrowser : public CSingleton < CServerBrowser >
{
    friend class CCore;

public:
    typedef enum ServerBrowserType
    {
        INTERNET = 0,
		LAN,
        FAVOURITES,
        RECENTLY_PLAYED
    };

public:
    explicit            CServerBrowser          ( void );
                        ~CServerBrowser         ( void );

    void                Update                  ( void );

    void                SetVisible              ( bool bVisible );
    bool                IsVisible               ( void );

    bool                ConnectToSelectedServer ( void );

    static void         CompleteConnect         ( void );

    CServerList*        GetFavouritesList       ( void ) { return &m_ServersFavourites; };
    CServerList*        GetRecentList           ( void ) { return &m_ServersRecent; };

    bool                LoadServerList          ( CXMLNode* pNode, std::string strTagName, CServerList *pList );
    bool                SaveServerList          ( CXMLNode* pNode, std::string strTagName, CServerList *pList );

protected:
    bool                OnMouseClick            ( CGUIMouseEventArgs Args );
	bool				OnMouseDoubleClick		( CGUIMouseEventArgs Args );

    // Window widgets
    CGUIWindow*         m_pWindow;
	CGUITabPanel*		m_pTabs;
    CGUIButton*         m_pButtonBack;
    CGUILabel*          m_pServerListStatus;
    CGUIStaticImage*	m_pLockedIcon;
    CGUIStaticImage*	m_pSerialIcon;
    CGUIButton*         m_pButtonFavouritesByIP;

    // Classes
    CFavouritesAddByIP  m_pFavouritesAddByIP;
    CCommunityLogin     m_pCommunityLogin;
  
    // Tab controls
    CGUITab*            m_pTab [ SERVER_BROWSER_TYPE_COUNT ];

    CGUIGridList*       m_pServerList [ SERVER_BROWSER_TYPE_COUNT ];

    CGUILabel*          m_pServerPlayerListLabel [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIGridList*       m_pServerPlayerList [ SERVER_BROWSER_TYPE_COUNT ];

    CGUICheckBox*       m_pIncludeEmpty [ SERVER_BROWSER_TYPE_COUNT ];
    CGUICheckBox*       m_pIncludeFull [ SERVER_BROWSER_TYPE_COUNT ];
    CGUICheckBox*       m_pIncludeLocked [ SERVER_BROWSER_TYPE_COUNT ];
    CGUICheckBox*       m_pIncludeOffline [ SERVER_BROWSER_TYPE_COUNT ];

    CGUIButton*         m_pButtonConnect [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIButton*         m_pButtonRefresh [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIButton*         m_pButtonFavourites [ SERVER_BROWSER_TYPE_COUNT ];
    
    CGUILabel*          m_pLabelPassword [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIEdit*           m_pEditPassword [ SERVER_BROWSER_TYPE_COUNT ];

    // Server list columns
    CGUIHandle			m_hSerial [ SERVER_BROWSER_TYPE_COUNT ];
	CGUIHandle			m_hLocked [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIHandle          m_hName [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIHandle          m_hPing [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIHandle          m_hPlayers [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIHandle          m_hHost [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIHandle          m_hGame [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIHandle          m_hMap [ SERVER_BROWSER_TYPE_COUNT ];

    // Player list columns
    CGUIHandle          m_hPlayerName [ SERVER_BROWSER_TYPE_COUNT ];

private:
    void                    CreateTab                       ( ServerBrowserType type, const char* szName );
    void                    DeleteTab                       ( ServerBrowserType type );

    void                    UpdateServerList                ( ServerBrowserType Type );
    CServerList *           GetServerList                   ( ServerBrowserType Type );
    void                    AddServerToList                 ( CServerListItem * pServer, ServerBrowserType Type );
    
    bool                    OnClick                         ( CGUIElement* pElement );
    bool                    OnDoubleClick                   ( CGUIElement* pElement );
    bool                    OnConnectClick                  ( CGUIElement* pElement );
    bool                    OnRefreshClick                  ( CGUIElement* pElement );
    bool                    OnFavouritesClick               ( CGUIElement* pElement );
    bool                    OnBackClick                     ( CGUIElement* pElement );
    bool                    OnIncludeEmptyClick             ( CGUIElement* pElement );
    bool                    OnIncludeFullClick              ( CGUIElement* pElement );
    bool                    OnIncludeLockedClick            ( CGUIElement* pElement );
    bool                    OnIncludeOfflineClick           ( CGUIElement* pElement );
    bool                    OnFavouritesByIPClick           ( CGUIElement* pElement );
    bool                    OnFavouritesByIPAddClick        ( CGUIElement* pElement );

    ServerBrowserType       GetCurrentServerBrowserType     ( void );

    CServerListInternet     m_ServersInternet;
    CServerListLAN          m_ServersLAN;
    CServerList             m_ServersFavourites;
    CServerList             m_ServersRecent;

    unsigned long           m_ulLastUpdateTime;
};

#endif
