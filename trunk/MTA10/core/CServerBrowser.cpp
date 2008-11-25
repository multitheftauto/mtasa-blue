/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerBrowser.cpp
*  PURPOSE:     In-game server browser user interface
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

extern CCore* g_pCore;

template<> CServerBrowser * CSingleton < CServerBrowser >::m_pSingleton = NULL;

CServerBrowser::CServerBrowser ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Initialize
    m_ulLastUpdateTime = 0;

    // Create serverbrowser window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "Server Browser" ) );
    m_pWindow->SetCloseButtonEnabled ( false );
    m_pWindow->SetMovable ( true );
    m_pWindow->SetSizingEnabled ( false );
    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    float yoff = resolution.fY > 600 ? resolution.fY / 12 : 0.0f;
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - 720.0f / 2, resolution.fY / 2 - 495.0f / 2 + yoff  ), false );
    m_pWindow->SetSize ( CVector2D ( 720.0f, 495.0f ) );
    m_pWindow->SetAlwaysOnTop ( true );

	// Create the serverlist tab panel and some tabs
	m_pTabs = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( m_pWindow ) );
	m_pTabs->SetPosition ( CVector2D ( 0.0f, 0.05f ), true );
	m_pTabs->SetSize ( CVector2D ( 1.0f, 0.878f ), true );		// ACHTUNG: ABSOLUTIFY
    
    // Back button
    m_pButtonBack = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Back" ) );
    m_pButtonBack->SetPosition ( CVector2D ( 0.83f, 0.935f ), true );
    m_pButtonBack->SetSize ( CVector2D ( 0.15f, 0.04f ), true );

    // Create the serverlist status label
    m_pServerListStatus = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Loading..." ) );
    m_pServerListStatus->SetPosition ( CVector2D ( 0.02f, 0.937f ), true );
	m_pServerListStatus->SetSize ( CVector2D ( 0.40f, 0.40f ), true );
    //m_pServerListStatus->SetMinimumSize ( CVector2D ( 1.0f, 1.0f ) );
    //m_pServerListStatus->SetMaximumSize ( CVector2D ( 1.0f, 1.0f ) );

	// Create locked icon
	m_pLockedIcon = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage ( m_pWindow ) );
	m_pLockedIcon->SetVisible ( false );
	m_pLockedIcon->SetFrameEnabled ( false );
	m_pLockedIcon->LoadFromFile ( "cgui\\images\\locked.png" );

    // Serial verification icon
	m_pSerialIcon = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage ( m_pWindow ) );
	m_pSerialIcon->SetVisible ( false );
	m_pSerialIcon->SetFrameEnabled ( false );
	m_pSerialIcon->LoadFromFile ( "cgui\\images\\shield.png" );

    m_pButtonBack->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnBackClick, this ) );

    // Create the tabs
    CreateTab ( ServerBrowserType::INTERNET, "Internet" );
	CreateTab ( ServerBrowserType::LAN, "Lan" );
    CreateTab ( ServerBrowserType::FAVOURITES, "Favourites" );
    CreateTab ( ServerBrowserType::RECENTLY_PLAYED, "Recently Played" );

    // Create the "Add to favourites by IP" button
    m_pButtonFavouritesByIP = reinterpret_cast < CGUIButton * > ( pManager->CreateButton ( m_pTab [ ServerBrowserType::FAVOURITES ], "Add by IP" ) );
    m_pButtonFavouritesByIP->SetPosition ( CVector2D ( 0.30f, 0.93f ), true );
    m_pButtonFavouritesByIP->SetSize ( CVector2D ( 0.25f, 0.04f ), true );
    m_pButtonFavouritesByIP->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFavouritesByIPClick, this ) );

    // Create the "Add to favourites by IP" dialog
    m_pFavouritesAddByIP.SetCallback ( GUI_CALLBACK ( &CServerBrowser::OnFavouritesByIPAddClick, this ) );

    // Login dialog
    m_pBrowserLogin.SetVisible ( false );
    m_pBrowserLogin.SetCallback ( &CServerBrowser::CompleteConnect );
}


CServerBrowser::~CServerBrowser ( void )
{
    // Delete the Tabs
    DeleteTab ( ServerBrowserType::INTERNET );
	DeleteTab ( ServerBrowserType::LAN );
    DeleteTab ( ServerBrowserType::FAVOURITES );
    DeleteTab ( ServerBrowserType::RECENTLY_PLAYED );

    // Delete the GUI items
    delete m_pServerListStatus;
    delete m_pButtonBack;
    delete m_pTabs;
    delete m_pWindow;
}

void CServerBrowser::CreateTab ( ServerBrowserType type, const char* szName )
{
    CGUI *pManager = g_pCore->GetGUI ();

    m_pTab [ type ] = m_pTabs->CreateTab ( szName );

    // Create the serverlist
    m_pServerList [ type ] = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( m_pTab [ type ] ) );
    m_pServerList [ type ]->SetPosition ( CVector2D ( 0.02f, 0.10f ), true );
    m_pServerList [ type ]->SetSize ( CVector2D ( 0.80f, 0.815f ), true );

    // Server player list label
    m_pServerPlayerListLabel [ type ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pTab [ type ], "Player List:" ) );
    m_pServerPlayerListLabel [ type ]->SetPosition ( CVector2D ( 0.83f, 0.29f ), true );
    m_pServerPlayerListLabel [ type ]->AutoSize ( "Player List:" );

    // Server player list
    m_pServerPlayerList [ type ] = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( m_pTab [ type ] ) );
    m_pServerPlayerList [ type ]->SetPosition ( CVector2D ( 0.83f, 0.33f ), true );
    m_pServerPlayerList [ type ]->SetSize ( CVector2D ( 0.15f, 0.635f ), true );
    m_pServerPlayerList [ type ]->SetIgnoreTextSpacer ( true );

    // Filters
    m_pIncludeEmpty [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Empty", true ) );
    m_pIncludeEmpty [ type ]->SetPosition ( CVector2D ( 0.02f, 0.05f ), true );
    m_pIncludeEmpty [ type ]->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnIncludeEmptyClick, this ) );

    m_pIncludeFull [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Full", true ) );
    m_pIncludeFull [ type ]->SetPosition ( CVector2D ( 0.18f, 0.05f ), true );
    m_pIncludeFull [ type ]->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnIncludeFullClick, this ) );

    m_pIncludeLocked [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Locked", true ) );
    m_pIncludeLocked [ type ]->SetPosition ( CVector2D ( 0.32f, 0.05f ), true );
    m_pIncludeLocked [ type ]->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnIncludeLockedClick, this ) );

    if ( type != ServerBrowserType::INTERNET && type != ServerBrowserType::LAN )
    {
        m_pIncludeOffline [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Offline", true ) );
        m_pIncludeOffline [ type ]->SetPosition ( CVector2D ( 0.48f, 0.05f ), true );
        m_pIncludeOffline [ type ]->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnIncludeLockedClick, this ) );
    }
    else
    {
        m_pIncludeOffline [ type ] = NULL;
    }

    // Buttons
    
    // Connect button
    m_pButtonConnect [ type ] = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pTab [ type ], "Connect" ) );
    m_pButtonConnect [ type ]->SetPosition ( CVector2D ( 0.83f, 0.05f ), true );
    m_pButtonConnect [ type ]->SetSize ( CVector2D ( 0.15f, 0.04f ), true );
    m_pButtonConnect [ type ]->SetAlwaysOnTop ( true );

    // Refresh button
    m_pButtonRefresh [ type ] = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pTab [ type ], "Refresh" ) );
    m_pButtonRefresh [ type ]->SetPosition ( CVector2D ( 0.83f, 0.11f ), true );
    m_pButtonRefresh [ type ]->SetSize ( CVector2D ( 0.15f, 0.04f ), true );

    // Add to Favourites button
    if ( type == ServerBrowserType::FAVOURITES )
    {
        m_pButtonFavourites [ type ] = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pTab [ type ], "Remove from Favourites" ) );
    }
    else
    {
        m_pButtonFavourites [ type ] = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pTab [ type ], "Add to Favourites" ) );    
    }

    m_pButtonFavourites [ type ]->SetPosition ( CVector2D ( 0.02f, 0.93f ), true );
    m_pButtonFavourites [ type ]->SetSize ( CVector2D ( 0.25f, 0.04f ), true );

    // Password label
    m_pLabelPassword [ type ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pTab [ type ], "Password:" ) );
    m_pLabelPassword [ type ]->SetPosition ( CVector2D ( 0.83f, 0.17f ), true ); 
    m_pLabelPassword [ type ]->AutoSize ( "Password:" );
    //m_pLabelPassword [ type ]->SetMinimumSize ( CVector2D ( 1.0f, 1.0f ) );
    //m_pLabelPassword [ type ]->SetMaximumSize ( CVector2D ( 1.0f, 1.0f ) );

    // Password edit
    m_pEditPassword [ type ] = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pTab [ type ], "" ) );
    m_pEditPassword [ type ]->SetPosition ( CVector2D ( 0.83f, 0.21f ), true ); 
    m_pEditPassword [ type ]->SetSize ( CVector2D ( 0.15f, 0.055f ), true );
    m_pEditPassword [ type ]->SetMasked ( true );

    // Set up event handlers
    m_pButtonConnect [ type ]->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnConnectClick, this ) );
    m_pButtonRefresh [ type ]->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnRefreshClick, this ) );
    m_pButtonFavourites [ type ]->SetOnClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFavouritesClick, this ) );

    // Server List Columns
    m_hSerial [ type ] = m_pServerList [ type ]->AddColumn ( "", 0.03f );
	m_hLocked [ type ] = m_pServerList [ type ]->AddColumn ( "", 0.03f );
    m_hName [ type ] = m_pServerList [ type ]->AddColumn ( "Name", 0.50f );
    m_hPlayers [ type ] = m_pServerList [ type ]->AddColumn ( "Players", 0.14f );
    m_hPing [ type ] = m_pServerList [ type ]->AddColumn ( "Ping", 0.10f );
    m_hGame [ type ] = m_pServerList [ type ]->AddColumn ( "Game", 0.25f );
    m_hMap [ type ] = m_pServerList [ type ]->AddColumn ( "Map", 0.25f );
    m_hHost [ type ] = m_pServerList [ type ]->AddColumn ( "Host", 0.25f );

    // Player List Columns
    m_hPlayerName [ type ] = m_pServerPlayerList [ type ]->AddColumn ( "Name", 0.75f );
}

void CServerBrowser::DeleteTab ( ServerBrowserType type )
{
    delete m_pButtonConnect [ type ];
    delete m_pButtonRefresh [ type ];
    delete m_pButtonFavourites [ type ];
    
    delete m_pLabelPassword [ type ];
    delete m_pEditPassword [ type ];

    delete m_pIncludeEmpty [ type ];
    delete m_pIncludeFull [ type ];
    delete m_pIncludeLocked [ type ];

    if ( m_pIncludeOffline [ type ] )
    {
        delete m_pIncludeOffline [ type ];
    }

    delete m_pServerPlayerList [ type ];
    delete m_pServerPlayerListLabel [ type ];

    delete m_pServerList [ type ];
    delete m_pTab [ type ];
}

CServerBrowser::ServerBrowserType CServerBrowser::GetCurrentServerBrowserType ( void )
{ 
    ServerBrowserType currentServerBrowserType;

    if ( m_pTabs->IsTabSelected ( m_pTab [ ServerBrowserType::FAVOURITES ] ) )
    {
        currentServerBrowserType = ServerBrowserType::FAVOURITES;
    }
    else if ( m_pTabs->IsTabSelected ( m_pTab [ ServerBrowserType::RECENTLY_PLAYED ] ) )
    {
        currentServerBrowserType = ServerBrowserType::RECENTLY_PLAYED;
    }
	else if ( m_pTabs->IsTabSelected ( m_pTab [ ServerBrowserType::LAN ] ) )
	{
		currentServerBrowserType = ServerBrowserType::LAN;
	}
    else
    {
        currentServerBrowserType = ServerBrowserType::INTERNET;
    }

    return currentServerBrowserType;
}

void CServerBrowser::Update ( void )
{
    CServerList *pList = GetServerList ( GetCurrentServerBrowserType () );

    // Update the current server list class
    pList->Pulse ();

    // If an update is needed, the serverbrowser is visible and it has gone a second since last update
    if ( pList->IsUpdated () && m_pWindow->IsVisible () && ( m_ulLastUpdateTime < CClientTime::GetTime () - SERVER_BROWSER_UPDATE_INTERVAL ) )
    {
        // Update the GUI
        UpdateServerList ( GetCurrentServerBrowserType () );

        // Set the status string
        m_pServerListStatus->SetText ( pList->GetStatus ().c_str () );

        // Update last time updated
        m_ulLastUpdateTime = CClientTime::GetTime ();
    }
}


void CServerBrowser::SetVisible ( bool bVisible )
{
    m_pWindow->SetVisible ( bVisible );
    m_pWindow->BringToFront ();

    // Are we making this window visible?
    if ( bVisible )
    {
        // Start refreshing all servers
        for ( unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++ )
            GetServerList ( (ServerBrowserType)i )->Refresh ();
    }
    else
    {
        m_pFavouritesAddByIP.SetVisible ( false );
    }
}


bool CServerBrowser::IsVisible ( void )
{
    return m_pWindow->IsVisible ();
}


void CServerBrowser::UpdateServerList ( ServerBrowserType Type )
{
	bool bLocked = false;
	bool bLocal = false;
    CServerList *pList;

    // Clear current list
    m_pServerList [ Type ]->Clear ();

    // Clear the player lists
    m_pServerPlayerList [ Type ]->Clear ();

    // Get the appropriate server list
    pList = GetServerList ( Type );

    // Loop the server list
    CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
    int j = 0;
    int k = pList->GetServerCount ();
    if ( k > 0 ) {
        for ( CServerListIterator i = i_b; i != i_e; i++ ) {
            CServerListItem * pServer = *i;

            // Add the item to the list
            if ( pServer->bScanned )
                AddServerToList ( pServer, Type );
            j++;
        }
    }

    /*
    char szTitle [ 128 ] = { 0 };
    _snprintf ( szTitle, 128, "Server Browser - %d servers - %d/%d players", iNumServers, iNumPlayers, iNumPlayerSlots );
    m_pWindow->SetText ( szTitle );
    */
    m_pServerList [ Type ]->ForceUpdate ();
}


void CServerBrowser::AddServerToList ( CServerListItem * pServer, ServerBrowserType Type )
{
    bool bIncludeEmpty  = m_pIncludeEmpty [ Type ]->GetSelected ();
    bool bIncludeFull   = m_pIncludeFull [ Type ]->GetSelected ();
    bool bIncludeLocked = m_pIncludeLocked [ Type ]->GetSelected ();

    if (
        ( pServer->nPlayers > 0 || bIncludeEmpty ) &&
        ( pServer->nPlayers < pServer->nMaxPlayers || pServer->nPlayers == 0 || pServer->nMaxPlayers == 0 || bIncludeFull ) &&
        ( !pServer->bPassworded || bIncludeLocked )
        )
    {
        // Create a new row
        int iIndex = m_pServerList [ Type ]->AddRow ( true );

        // Format some text data
        char buf[32] = {0};
        stringstream ss;
        ss << pServer->nPlayers << " / " << pServer->nMaxPlayers;

        m_pServerList [ Type ]->SetItemText ( iIndex, m_hName [ Type ],     pServer->strName.c_str (), false, false, true );
        m_pServerList [ Type ]->SetItemText ( iIndex, m_hGame [ Type ],     pServer->strType.c_str (), false, false, true );
        m_pServerList [ Type ]->SetItemText ( iIndex, m_hMap [ Type ],      pServer->strMap.c_str (), false, false, true );
        m_pServerList [ Type ]->SetItemText ( iIndex, m_hHost [ Type ],     pServer->strHost.c_str (), true, false, true );
        m_pServerList [ Type ]->SetItemText ( iIndex, m_hPlayers [ Type ],  ss.str ().c_str (), true, false, true );
        m_pServerList [ Type ]->SetItemText ( iIndex, m_hPing [ Type ],     itoa ( pServer->nPing, buf, 10 ), true, false, true );

		// Locked icon
		if ( pServer->bPassworded )
        {
			m_pServerList [ Type ]->SetItemImage ( iIndex, m_hLocked [ Type ], m_pLockedIcon );
        }
        if ( pServer->bSerials )
        {
            m_pServerList [ Type ]->SetItemImage ( iIndex, m_hSerial [ Type ], m_pSerialIcon );
        }
    }
}


CServerList * CServerBrowser::GetServerList ( ServerBrowserType Type )
{
    if ( Type == ServerBrowserType::FAVOURITES )
        return &m_ServersFavourites;
    else if ( Type == ServerBrowserType::INTERNET )
        return &m_ServersInternet;
    else if ( Type == ServerBrowserType::LAN )
        return &m_ServersLAN;
    else if ( Type == ServerBrowserType::RECENTLY_PLAYED)
        return &m_ServersRecent;

    assert ( false );
    return NULL;
}


bool CServerBrowser::OnClick ( CGUIElement* pElement )
{
    ServerBrowserType Type = GetCurrentServerBrowserType ();

    m_pServerPlayerList [ Type ]->Clear ();
 
    // If there is one item selected
    if ( m_pServerList [ Type ]->GetSelectedCount () >= 1 )
    {
        // Get the selected row
        int iSelectedIndex = m_pServerList [ Type ]->GetSelectedItemRow ();

        // Walk the server list looking for this server
        CServerList * pList = GetServerList ( Type );
        CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
        std::string strHost = m_pServerList [ Type ]->GetItemText ( iSelectedIndex, m_hHost [ Type ] );
        for ( i = i_b; i != i_e; i++ ) {
            CServerListItem * pServer = *i;
            if ( strHost.compare ( pServer->strHost ) == 0 ) {
                // We found the server, add all the players
                for ( unsigned int j = 0; j < pServer->vecPlayers.size (); j++ ) {
                    int k = m_pServerPlayerList [ Type ]->AddRow ();
                    m_pServerPlayerList [ Type ]->SetItemText ( k, m_hPlayerName [ Type ], pServer->vecPlayers[k].strName.c_str () );
                }
            }
        }
    }

    return true;
}


bool CServerBrowser::OnDoubleClick ( CGUIElement* pElement )
{
    return ConnectToSelectedServer ();
}

bool CServerBrowser::OnConnectClick ( CGUIElement* pElement )
{   
    return ConnectToSelectedServer ();
}

void CServerBrowser::CompleteConnect ( void )
{
    CServerBrowser *pBrowser = CServerBrowser::GetSingletonPtr ();
    pBrowser->ConnectToSelectedServer();
}

bool CServerBrowser::ConnectToSelectedServer ( void )
{
    ServerBrowserType Type = GetCurrentServerBrowserType ();
    g_pCore->GetConsole()->Printf ( "Test: %d", m_pServerList [ Type ]->GetSelectedCount () );
    m_pServerPlayerList [ Type ]->Clear ();
 
    // If there is one item selected
    if ( m_pServerList [ Type ]->GetSelectedCount () >= 1 )
    {
        // Get the selected row
        int iSelectedIndex = m_pServerList [ Type ]->GetSelectedItemRow ();

        // Walk the server list looking for this server
        CServerList * pList = GetServerList ( Type );
        CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
        std::string strHost = m_pServerList [ Type ]->GetItemText ( iSelectedIndex, m_hHost [ Type ] );
        for ( i = i_b; i != i_e; i++ ) {
            CServerListItem * pServer = *i;
            if ( strHost.compare ( pServer->strHost ) == 0 ) {
                if ( ( pServer->bSerials ) && ( !g_pCore->GetCommunity()->IsLoggedIn() ) )
                {
                    m_pBrowserLogin.SetVisible ( true );
                    return true;
                }
                // Get the nick from the config
                std::string strNick;
                CVARS_GET ( "nick", strNick );

                // Valid nick?
                if ( !CCore::GetSingleton ().IsValidNick ( strNick.c_str () ) )
                {
                    CCore::GetSingleton ().ShowMessageBox ( "Error", "Invalid nickname! Please go to Settings and set a new!", MB_BUTTON_OK | MB_ICON_INFO );
                    return true;
                }

                // Password buffer
                char szPassword [48];
                szPassword [47] = 0;
                strncpy ( szPassword, m_pEditPassword [ Type ]->GetText ().c_str (), 47 );

                // Start the connect
                std::string strIP = pServer->strHost.substr ( 0, pServer->strHost.find ( ':' ) );
                CCore::GetSingleton ().GetConnectManager ()->Connect ( strIP.c_str (), pServer->usGamePort, strNick.c_str (), szPassword );
            }
        }
    }
    else
    {
        CCore::GetSingleton ().ShowMessageBox ( "Information", "You have to select a server to connect to.", MB_BUTTON_OK | MB_ICON_INFO );
    }
    return false;
}



bool CServerBrowser::OnRefreshClick ( CGUIElement* pElement )
{
    GetServerList ( GetCurrentServerBrowserType () )->Refresh ();
    return true;
}

bool CServerBrowser::OnFavouritesClick ( CGUIElement* pElement )
{
    /*
    ServerBrowserType currentServerBrowserType = GetCurrentServerBrowserType ();

    // If there are more than 0 items selected in the browser
    if ( m_pServerList [ currentServerBrowserType ]->GetSelectedCount () > 0 )
    {
        // Get the selected item
        int iIndex = m_pServerList [ currentServerBrowserType ]->GetSelectedItemRow ();

        // Get the selected row's server index
        int iServerListID = (int) m_pServerList [ currentServerBrowserType ]->GetItemData ( iIndex, m_hName [ currentServerBrowserType ] );

        char* szName = m_pServerList [ currentServerBrowserType ]->GetItemText ( iIndex, m_hName [ currentServerBrowserType ] );
        char* szAddress = m_pServerList [ currentServerBrowserType ]->GetItemText ( iIndex, m_hHost [ currentServerBrowserType ] );

        if ( !strncmp( szName, "[Offline] ", 10 ) )
        {
            // Remove the "[Offline] " tag for offline servers
            szName += 10;
        }

        char* szHost = strtok ( szAddress, ":" );
        char* szPort = strtok ( NULL, "\0" );

        if ( szName && szHost && szPort )
        {
            unsigned short usPort = static_cast < unsigned short > ( atoi ( szPort ) );

            if ( currentServerBrowserType == ServerBrowserType::FAVOURITES )
            {
                g_pCore->GetConfig ()->GetFavouriteServers ().RemoveServer ( szHost, usPort );
            }
            else
            {
                // Make sure the user didn't pull a Talidan and add the server again
                if ( !g_pCore->GetConfig ()->GetFavouriteServers ().Exists ( szHost, usPort ) )
                {
                    CSavedServer* pSavedServer = g_pCore->GetConfig ()->GetFavouriteServers ().AddServer ( szName, szHost, usPort );
                    if ( pSavedServer )
                    {
                        pSavedServer->SetOnline ( true, iServerListID );
                    }
                }
            }

            UpdateServerList ();
        }
    }

    return true;
    */
    return true;
}


bool CServerBrowser::OnBackClick ( CGUIElement* pElement )
{
    CMainMenu *pMainMenu = CLocalGUI::GetSingleton ().GetMainMenu ();

    m_pWindow->SetVisible ( false );
    m_pFavouritesAddByIP.SetVisible ( false );
    pMainMenu->m_bIsInSubWindow = false;

    return true;
}


bool CServerBrowser::OnMouseClick ( CGUIMouseEventArgs Args )
{
    if ( Args.pWindow == m_pServerList [ ServerBrowserType::INTERNET ] )
    {
        OnClick ( m_pServerList [ ServerBrowserType::INTERNET ] );
        return true;
    }
    else if ( Args.pWindow == m_pServerList [ ServerBrowserType::LAN ] )
    {
        OnClick ( m_pServerList [ ServerBrowserType::LAN ] );
        return true;
    }
    else if ( Args.pWindow == m_pServerList [ ServerBrowserType::FAVOURITES ] )
    {
        OnClick ( m_pServerList [ ServerBrowserType::FAVOURITES ] );
        return true;
    }
    else if ( Args.pWindow == m_pServerList [ ServerBrowserType::RECENTLY_PLAYED ] )
    {
        OnClick ( m_pServerList [ ServerBrowserType::RECENTLY_PLAYED ] );
        return true;
    }

	return false;
}


bool CServerBrowser::OnMouseDoubleClick ( CGUIMouseEventArgs Args )
{
    if ( Args.pWindow == m_pServerList [ ServerBrowserType::INTERNET ] )
    {
        OnDoubleClick ( m_pServerList [ ServerBrowserType::INTERNET ] );
        return true;
    }
    if ( Args.pWindow == m_pServerList [ ServerBrowserType::LAN ] )
    {
        OnDoubleClick ( m_pServerList [ ServerBrowserType::LAN ] );
        return true;
    }
    else if ( Args.pWindow == m_pServerList [ ServerBrowserType::FAVOURITES ] )
    {
        OnDoubleClick ( m_pServerList [ ServerBrowserType::FAVOURITES ] );
        return true;
    }
    else if ( Args.pWindow == m_pServerList [ ServerBrowserType::RECENTLY_PLAYED ] )
    {
        OnDoubleClick ( m_pServerList [ ServerBrowserType::RECENTLY_PLAYED ] );
        return true;
    }

	return false;
}


bool CServerBrowser::OnIncludeEmptyClick ( CGUIElement* pElement )
{
    UpdateServerList ( GetCurrentServerBrowserType () );

    return true;
}


bool CServerBrowser::OnIncludeFullClick ( CGUIElement* pElement )
{
    UpdateServerList ( GetCurrentServerBrowserType () );

    return true;
}


bool CServerBrowser::OnIncludeLockedClick ( CGUIElement* pElement )
{
    UpdateServerList ( GetCurrentServerBrowserType () );

    return true;
}


bool CServerBrowser::OnIncludeOfflineClick ( CGUIElement* pElement )
{
    UpdateServerList ( GetCurrentServerBrowserType () );

    return true;
}

bool CServerBrowser::OnFavouritesByIPClick ( CGUIElement* pElement )
{
    if ( !m_pFavouritesAddByIP.IsVisible () )
    {
        m_pFavouritesAddByIP.Reset ();
    }
    m_pFavouritesAddByIP.SetVisible ( true );

    return true;
}

bool CServerBrowser::OnFavouritesByIPAddClick ( CGUIElement* pElement )
{
    in_addr Address;
    std::string strHost;

    // Get values
    m_pFavouritesAddByIP.GetHost ( strHost );
    int iPort = m_pFavouritesAddByIP.GetPort () + SERVER_LIST_QUERY_PORT_OFFSET;

    // Parse string and verify correct parameters
    if ( !CServerListItem::Parse ( strHost, Address ) || strHost.length () == 0 || iPort <= 0 )
    {
        CCore::GetSingleton ().ShowMessageBox ( "Error", "Incorrect host or port specified!", MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }

    // Construct list item
    CServerListItem Item ( Address, iPort );

    // Add the item if it doesn't already exist
    if ( !m_ServersFavourites.Exists ( Item ) )
    {
        m_ServersFavourites.Add ( Item );
        UpdateServerList ( ServerBrowserType::FAVOURITES );
    }

    m_pFavouritesAddByIP.Reset ();
    m_pFavouritesAddByIP.SetVisible ( false );

    return true;
}

bool CServerBrowser::LoadServerList ( CXMLNode* pNode, std::string strTagName, CServerList *pList )
{
    CXMLNode* pSubNode = NULL;
    in_addr Address;
    int iPort;

    if ( !pNode ) return false;

    // Loop through all subnodes looking for relevant nodes
    unsigned int uiCount = pNode->GetSubNodeCount ();
    for ( unsigned int i = 0; i < uiCount; i++ )
    {
        pSubNode = pNode->GetSubNode ( i );
        if ( pSubNode && pSubNode->GetTagName ().compare ( strTagName ) == 0 )
        {
            // This node is relevant, so get the attributes we need and add it to the list
            CXMLAttribute* pHostAttribute = pSubNode->GetAttributes ().Find ( "host" );
            CXMLAttribute* pPortAttribute = pSubNode->GetAttributes ().Find ( "port" );
            if ( pHostAttribute && pPortAttribute ) {
                CServerListItem::Parse ( pHostAttribute->GetValue (), Address );
                iPort = atoi ( pPortAttribute->GetValue ().c_str () ) + SERVER_LIST_QUERY_PORT_OFFSET;
                if ( iPort > 0 )
                    pList->Add ( CServerListItem ( Address, iPort ) );
            }
        }
    }
    return true;
}


bool CServerBrowser::SaveServerList ( CXMLNode* pNode, std::string strTagName, CServerList *pList )
{
    if ( !pNode ) return false;

    // Start by clearing out all previous nodes
    pNode->DeleteAllSubNodes ();

    // Iterate through the list, adding any items to our node
    CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
    int j = 0;
    int k = pList->GetServerCount ();
    if ( k > 0 ) {
        for ( CServerListIterator i = i_b; i != i_e; i++ ) {
            CServerListItem * pServer = *i;

            // Add the (scanned) item to the node
            if ( pServer->bScanned ) {
                CXMLNode * pSubNode = pNode->CreateSubNode ( strTagName.c_str () );
                if ( pSubNode )
                {
                    CXMLAttribute* pHostAttribute = pSubNode->GetAttributes ().Create ( "host" );
					pHostAttribute->SetValue ( pServer->strHost.c_str () );
                    
                    CXMLAttribute* pPortAttribute = pSubNode->GetAttributes ().Create ( "port" );
					pPortAttribute->SetValue ( pServer->usGamePort );
                }
            }
            j++;
        }
    }
    return true;
}
