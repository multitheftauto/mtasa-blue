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
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CServerBrowser;

#ifndef __CSERVERBROWSER_H
#define __CSERVERBROWSER_H

// Amount of server lists/tabs (ServerBrowserType)
#define SERVER_BROWSER_TYPE_COUNT           4

// Server browser list update interval (in ms)
#define SERVER_BROWSER_UPDATE_INTERVAL      250

#include "CMainMenu.h"
#include "CServerList.h"
#include "CSingleton.h"
#include "CFavouritesAddByIP.h"
#include "CCommunityLogin.h"
#include <ctime>


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
    
    void                SetSize                 ( CVector2D& vecSize );
    CVector2D           GetSize                 ( void );

    bool                ConnectToSelectedServer ( void );

    static void         CompleteConnect         ( void );

    CServerList*        GetInternetList         ( void ) { return &m_ServersInternet; };
    CServerList*        GetFavouritesList       ( void ) { return &m_ServersFavourites; };
    CServerList*        GetRecentList           ( void ) { return &m_ServersRecent; };

    void                LoadInternetList        ( );
    void                SaveInternetList        ( );
    void                SaveRecentlyPlayedList  ( );
    void                SaveFavouritesList      ( );

    void                LoadOptions             ( CXMLNode* pNode );
    void                SaveOptions             ( );

    bool                LoadServerList          ( CXMLNode* pNode, const std::string& strTagName, CServerList *pList );
    bool                SaveServerList          ( CXMLNode* pNode, const std::string& strTagName, CServerList *pList );

    void                SetServerPassword       ( const std::string& strHost, const std::string& strPassword );
    std::string         GetServerPassword       ( const std::string& strHost );
    void                ClearServerPasswords    ( void );

protected:
    bool                OnMouseClick            ( CGUIMouseEventArgs Args );
    bool                OnMouseDoubleClick      ( CGUIMouseEventArgs Args );

    int                 m_iSelectedServer[ SERVER_BROWSER_TYPE_COUNT ];

    // Window widgets
    CGUIWindow*         m_pWindow;
    CGUITabPanel*       m_pTabs;
    CGUIButton*         m_pButtonBack;
    CGUILabel*          m_pServerListStatus;
    CGUIStaticImage*    m_pLockedIcon;
    CGUIStaticImage*    m_pSerialIcon;
    CGUIButton*         m_pButtonFavouritesByIP;

    // Classes
    CFavouritesAddByIP  m_pFavouritesAddByIP;
    CCommunityLogin     m_pCommunityLogin;
  
    // Tab controls
    CGUITab*            m_pTab [ SERVER_BROWSER_TYPE_COUNT ];

    CGUIGridList*       m_pServerList [ SERVER_BROWSER_TYPE_COUNT ];
    int                 m_pServerListRevision [ SERVER_BROWSER_TYPE_COUNT ];

    CGUILabel*          m_pServerPlayerListLabel [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIGridList*       m_pServerPlayerList [ SERVER_BROWSER_TYPE_COUNT ];

    CGUIEdit*           m_pEditServerSearch [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIStaticImage*    m_pServerSearchIcon [ SERVER_BROWSER_TYPE_COUNT ];

    CGUICheckBox*       m_pIncludeEmpty [ SERVER_BROWSER_TYPE_COUNT ];
    CGUICheckBox*       m_pIncludeFull [ SERVER_BROWSER_TYPE_COUNT ];
    CGUICheckBox*       m_pIncludeLocked [ SERVER_BROWSER_TYPE_COUNT ];
    CGUICheckBox*       m_pIncludeOffline [ SERVER_BROWSER_TYPE_COUNT ];

    CGUIButton*         m_pButtonConnect [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIButton*         m_pButtonRefresh [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIButton*         m_pButtonFavourites [ SERVER_BROWSER_TYPE_COUNT ];
    
    CGUILabel*          m_pLabelPassword [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIEdit*           m_pEditPassword [ SERVER_BROWSER_TYPE_COUNT ];

    CGUIEdit*           m_pEditPlayerSearch [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIStaticImage*    m_pPlayerSearchIcon [ SERVER_BROWSER_TYPE_COUNT ];

    // Server list columns
    CGUIHandle          m_hSerial [ SERVER_BROWSER_TYPE_COUNT ];
    CGUIHandle          m_hLocked [ SERVER_BROWSER_TYPE_COUNT ];
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

    void                    UpdateServerList                ( ServerBrowserType Type, bool bClearServerList = false);
    CServerList *           GetServerList                   ( ServerBrowserType Type );
    void                    AddServerToList                 ( CServerListItem * pServer, ServerBrowserType Type );
    
    bool                    OnClick                         ( CGUIElement* pElement );
    bool                    OnDoubleClick                   ( CGUIElement* pElement );
    bool                    OnConnectClick                  ( CGUIElement* pElement );
    bool                    OnRefreshClick                  ( CGUIElement* pElement );
    bool                    OnFavouritesClick               ( CGUIElement* pElement );
    bool                    OnBackClick                     ( CGUIElement* pElement );
    bool                    OnFilterChanged                 ( CGUIElement* pElement );
    bool                    OnTabChanged                    ( CGUIElement* pElement );
    bool                    OnFavouritesByIPClick           ( CGUIElement* pElement );
    bool                    OnFavouritesByIPAddClick        ( CGUIElement* pElement );
    bool                    OnWindowSize                    ( CGUIElement* pElement );

    ServerBrowserType       GetCurrentServerBrowserType     ( void );

    CServerListInternet     m_ServersInternet;
    CServerListLAN          m_ServersLAN;
    CServerList             m_ServersFavourites;
    CServerList             m_ServersRecent;

    unsigned long           m_ulLastUpdateTime;
    bool                    m_firstTimeBrowseServer;
    bool                    m_bOptionsLoaded;
    ServerBrowserType       m_PrevServerBrowserType;
};

#endif
