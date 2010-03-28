/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerBrowser.cpp
*  PURPOSE:     In-game server browser user interface
*  DEVELOPERS:  Cecill Etheredge <ijsf@gmx.net>
*               Stanislav Bobrov <lil_toady@hotmail.com>
*               Alberto Alonso <rydencillo@gmail.com>
*               Florian Busse <flobu@gmx.net>
*               Sebas Lamers <sebasdevelopment@gmx.com>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

extern CCore* g_pCore;

template<> CServerBrowser * CSingleton < CServerBrowser >::m_pSingleton = NULL;

#define BROWSER_DEFAULTWIDTH	720.0f
#define BROWSER_DEFAULTHEIGHT    495.0f

CServerBrowser::CServerBrowser ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Initialize
    m_ulLastUpdateTime = 0;
    m_firstTimeBrowseServer = true;
    m_OptionsLoaded = false;
    m_PrevServerBrowserType = INTERNET;

    // Create serverbrowser window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "SERVER BROWSER" ) );
    m_pWindow->SetCloseButtonEnabled ( false );
    m_pWindow->SetMovable ( true );
    m_pWindow->SetSizingEnabled ( true );
    CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
    m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - BROWSER_DEFAULTWIDTH / 2, resolution.fY / 2 - BROWSER_DEFAULTHEIGHT / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( BROWSER_DEFAULTWIDTH, BROWSER_DEFAULTHEIGHT ) );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->SetMinimumSize ( CVector2D ( BROWSER_DEFAULTWIDTH, BROWSER_DEFAULTHEIGHT ) );

	// Create the serverlist tab panel and some tabs
	m_pTabs = reinterpret_cast < CGUITabPanel* > ( pManager->CreateTabPanel ( m_pWindow ) );
	m_pTabs->SetPosition ( CVector2D ( 0.0f, 25.0f ) );
	m_pTabs->SetSize ( CVector2D ( BROWSER_DEFAULTWIDTH, BROWSER_DEFAULTHEIGHT - 60.0f ) );
    
    // Back button
    m_pButtonBack = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Back" ) );
    m_pButtonBack->SetPosition ( CVector2D ( BROWSER_DEFAULTWIDTH - 123.0f, BROWSER_DEFAULTHEIGHT - 32.0f ), false );
    m_pButtonBack->SetSize ( CVector2D ( 108.0f, 20.0f ), false );

    // Create the serverlist status label
    m_pServerListStatus = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Loading..." ) );
    m_pServerListStatus->SetPosition ( CVector2D ( 14.0f, BROWSER_DEFAULTHEIGHT - 30.0f ) );
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

    //Set necessary handlers
    m_pButtonBack->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnBackClick, this ) );
    m_pWindow->SetSizedHandler ( GUI_CALLBACK ( &CServerBrowser::OnWindowSize, this ) );

    // Create the tabs
    CreateTab ( ServerBrowserType::INTERNET, "Internet" );
	CreateTab ( ServerBrowserType::LAN, "Lan" );
    CreateTab ( ServerBrowserType::FAVOURITES, "Favourites" );
    CreateTab ( ServerBrowserType::RECENTLY_PLAYED, "Recently Played" );
    
    // Create the "Add to favourites by IP" button
    m_pButtonFavouritesByIP = reinterpret_cast < CGUIButton * > ( pManager->CreateButton ( m_pTab [ ServerBrowserType::FAVOURITES ], "Add by host/ip" ) );
    m_pButtonFavouritesByIP->SetPosition ( CVector2D ( 0.30f, 0.93f ), true );
    m_pButtonFavouritesByIP->SetSize ( CVector2D ( 0.25f, 0.04f ), true );
    m_pButtonFavouritesByIP->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFavouritesByIPClick, this ) );

    // Create the "Add to favourites by IP" dialog
    m_pFavouritesAddByIP.SetCallback ( GUI_CALLBACK ( &CServerBrowser::OnFavouritesByIPAddClick, this ) );

    // Login dialog
    m_pCommunityLogin.SetVisible ( false );
    m_pCommunityLogin.SetCallback ( &CServerBrowser::CompleteConnect );

	// Load options
    LoadOptions ( CCore::GetSingletonPtr ()->GetConfig ( )->FindSubNode ( CONFIG_NODE_SERVER_OPTIONS ) );

    // Save the active tab, needs to be done after at least one tab exists
    m_pTabs->SetSelectionHandler ( GUI_CALLBACK( &CServerBrowser::OnTabChanged, this ) );
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
    m_pServerListRevision [ type ] = 0;
    
    // Server player list label
    m_pServerPlayerListLabel [ type ] = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pTab [ type ], "Player List:" ) );
    m_pServerPlayerListLabel [ type ]->SetPosition ( CVector2D ( 0.83f, 0.34f ), true );
    m_pServerPlayerListLabel [ type ]->AutoSize ( "Player List:" );

    // Server player list
    m_pServerPlayerList [ type ] = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( m_pTab [ type ] ) );
    m_pServerPlayerList [ type ]->SetPosition ( CVector2D ( 0.83f, 0.38f ), true );
    m_pServerPlayerList [ type ]->SetSize ( CVector2D ( 0.15f, 0.585f ), true );
    m_pServerPlayerList [ type ]->SetIgnoreTextSpacer ( true );

    // Filters

    // Server search edit
    m_pEditServerSearch [ type ] = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pTab [ type ], "" ) );
    m_pEditServerSearch [ type ]->SetPosition ( CVector2D ( 0.02f, 0.04f ), true );
    m_pEditServerSearch [ type ]->SetSize ( CVector2D ( 0.19f, 0.05f ), true );
    m_pEditServerSearch [ type ]->SetTextChangedHandler( GUI_CALLBACK( &CServerBrowser::OnFilterChanged, this ) );

    // Server search icon
	m_pServerSearchIcon [ type ] = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage ( m_pEditServerSearch [ type ] ) );
	m_pServerSearchIcon [ type ]->SetPosition ( CVector2D ( 0.85f, 0.15f ), true );
	m_pServerSearchIcon [ type ]->SetSize ( CVector2D ( 16, 14 ), false );
	m_pServerSearchIcon [ type ]->LoadFromFile ( "cgui\\images\\magnfglasssmall.png" );

    // Player search edit
    m_pEditPlayerSearch [ type ] = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pTab [ type ], "" ) );
    m_pEditPlayerSearch [ type ]->SetPosition ( CVector2D ( 0.83f, 0.28f ), true );
    m_pEditPlayerSearch [ type ]->SetSize ( CVector2D ( 0.15f, 0.05f ), true );
    m_pEditPlayerSearch [ type ]->SetTextChangedHandler( GUI_CALLBACK( &CServerBrowser::OnFilterChanged, this ) );

    // Player search icon
	m_pPlayerSearchIcon [ type ] = reinterpret_cast < CGUIStaticImage* > ( pManager->CreateStaticImage ( m_pEditPlayerSearch [ type ] ) );
	m_pPlayerSearchIcon [ type ]->SetPosition ( CVector2D ( 0.8f, 0.15f ), true );
	m_pPlayerSearchIcon [ type ]->SetSize ( CVector2D ( 16, 14 ), false );
	m_pPlayerSearchIcon [ type ]->LoadFromFile ( "cgui\\images\\magnfglasssmall.png" );

    // Include checkboxes
    m_pIncludeEmpty [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Empty", true ) );
    m_pIncludeEmpty [ type ]->SetPosition ( CVector2D ( 0.225f, 0.045f ), true );
    m_pIncludeEmpty [ type ]->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFilterChanged, this ) );

    m_pIncludeFull [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Full", true ) );
    m_pIncludeFull [ type ]->SetPosition ( CVector2D ( 0.38f, 0.045f ), true );
    m_pIncludeFull [ type ]->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFilterChanged, this ) );

    m_pIncludeLocked [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Locked", true ) );
    m_pIncludeLocked [ type ]->SetPosition ( CVector2D ( 0.515f, 0.045f ), true );
    m_pIncludeLocked [ type ]->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFilterChanged, this ) );

    if ( type != ServerBrowserType::INTERNET && type != ServerBrowserType::LAN )
    {
        m_pIncludeOffline [ type ] = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pTab [ type ], "Include Offline", true ) );
        m_pIncludeOffline [ type ]->SetPosition ( CVector2D ( 0.675f, 0.045f ), true );
        m_pIncludeOffline [ type ]->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFilterChanged, this ) );
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
    m_pEditPassword [ type ]->SetSize ( CVector2D ( 0.15f, 0.05f ), true );
    m_pEditPassword [ type ]->SetMasked ( true );
    m_pEditPassword [ type ]->SetTextAcceptedHandler( GUI_CALLBACK( &CServerBrowser::OnConnectClick, this ) );

    // Set up event handlers
    m_pButtonConnect [ type ]->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnConnectClick, this ) );
    m_pButtonRefresh [ type ]->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnRefreshClick, this ) );
    m_pButtonFavourites [ type ]->SetClickHandler ( GUI_CALLBACK ( &CServerBrowser::OnFavouritesClick, this ) );

    // Server List Columns
    m_hSerial [ type ] = m_pServerList [ type ]->AddColumn ( "", 0.03f );
	m_hLocked [ type ] = m_pServerList [ type ]->AddColumn ( "", 0.03f );
    m_hName [ type ] = m_pServerList [ type ]->AddColumn ( "Name", 0.50f );
    m_hPlayers [ type ] = m_pServerList [ type ]->AddColumn ( "Players", 0.14f );
    m_hPing [ type ] = m_pServerList [ type ]->AddColumn ( "Ping", 0.10f );
    m_hGame [ type ] = m_pServerList [ type ]->AddColumn ( "Game", 0.25f );
    m_hMap [ type ] = m_pServerList [ type ]->AddColumn ( "Map", 0.25f );
    m_hHost [ type ] = m_pServerList [ type ]->AddColumn ( "Host", 0.25f );

    // Disable resizing of the first and second columns (Serial Auth & Locked)
    m_pServerList [ type ]->SetColumnSegmentSizingEnabled(0, false);
    m_pServerList [ type ]->SetColumnSegmentSizingEnabled(1, false);

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

    delete m_pEditServerSearch [ type ];
    delete m_pServerSearchIcon [ type ];

    delete m_pEditPlayerSearch [ type ];
    delete m_pPlayerSearchIcon [ type ];

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
    ServerBrowserType Type = GetCurrentServerBrowserType ();
    CServerList *pList = GetServerList ( Type );

    // Update the current server list class
    pList->Pulse ();

    // If an update is needed, the serverbrowser is visible and it has gone some time since last update
    if ( ( pList->IsUpdated () || m_PrevServerBrowserType != Type ) && m_ulLastUpdateTime < CClientTime::GetTime () - SERVER_BROWSER_UPDATE_INTERVAL )
    {
        // Update the GUI
        UpdateServerList ( Type , Type == RECENTLY_PLAYED );

        // Set the status string
        m_pServerListStatus->SetText ( pList->GetStatus ().c_str () );

        // Update last time updated
        m_ulLastUpdateTime = CClientTime::GetTime ();

        // Update last viewed tab
        m_PrevServerBrowserType = Type;
    }
}

CVector2D CServerBrowser::GetSize ( void )
{
    if ( m_pWindow )
    {
        return m_pWindow->GetSize ();
    }
    return CVector2D ();
}


void CServerBrowser::SetSize ( CVector2D& vecSize )
{
    if ( m_pWindow )
    {
        CVector2D resolution = CCore::GetSingleton().GetGUI()->GetResolution();
        // OnWindowSize should do the rest
        m_pWindow->SetSize ( vecSize );
        // Centre the window
        m_pWindow->SetPosition ( CVector2D ( resolution.fX / 2 - vecSize.fX / 2, resolution.fY / 2 - vecSize.fY / 2 ), false );
    }
}

bool CServerBrowser::OnWindowSize ( CGUIElement* pElement )
{
    CVector2D WindowSize = m_pWindow->GetSize ();

	// Update the Tab panel size
	m_pTabs->SetSize ( CVector2D ( WindowSize.fX, WindowSize.fY - 60.0f ) );
    
    // Back button position
    m_pButtonBack->SetPosition ( CVector2D ( WindowSize.fX - 123.0f, WindowSize.fY - 32.0f ) );

    //Status label position
    m_pServerListStatus->SetPosition ( CVector2D ( 14.0f, WindowSize.fY - 30.0f ) );
    
    //Make sure the Icon columns are of the correct size.  Its of a forced relative size - 0.03*562 was the default size.
    for ( unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++ )
    {
        m_pServerList [ i ]->SetColumnWidth ( 0, 18.0f, false );
        m_pServerList [ i ]->SetColumnWidth ( 1, 18.0f, false );
    }
        
    return true;
}


void CServerBrowser::SetVisible ( bool bVisible )
{
    m_pWindow->SetVisible ( bVisible );
    m_pWindow->BringToFront ();

    // Are we making this window visible?
    if ( bVisible )
    {
        if ( m_firstTimeBrowseServer )
        {
            // Start loading all servers
            for ( unsigned int i = 0; i < SERVER_BROWSER_TYPE_COUNT; i++ )
            {
                m_pEditPassword [ i ]->SetText( "" );
                m_iSelectedServer [ i ] = -1;
                GetServerList ( (ServerBrowserType)i )->Refresh ();
            }
            m_firstTimeBrowseServer = false;
        }
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


void CServerBrowser::UpdateServerList ( ServerBrowserType Type, bool bClearServerList )
{

    // Get the appropriate server list
    CServerList* pList = GetServerList ( Type );

    if ( pList->GetRevision () != m_pServerListRevision [ Type ] || bClearServerList )
    {
        m_pServerListRevision [ Type ] = pList->GetRevision ();

        // Clear current list
        m_pServerList [ Type ]->Clear ();

        // Clear the player lists
        m_pServerPlayerList [ Type ]->Clear ();
    }

    // Loop the server list
    CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
    int j = 0;
    int k = pList->GetServerCount ();
    if ( k > 0 )
    {
        for ( CServerListIterator i = i_b; i != i_e; i++ )
        {
            CServerListItem * pServer = *i;

            // Add the item to the list
            if ( ( pServer->bScanned || (pServer->bSkipped && m_pIncludeOffline [ Type ] && m_pIncludeOffline [ Type ]->GetSelected ()) ) &&
                 ( !pServer->bAddedToList[ Type ] || bClearServerList ) )
                AddServerToList ( pServer, Type );
            j++;
        }
    }

    /*
    SString strTitle = SString::Printf ( "Server Browser - %d servers - %d/%d players", iNumServers, iNumPlayers, iNumPlayerSlots );
    m_pWindow->SetText ( strTitle );
    */
    m_pServerList [ Type ]->ForceUpdate ();
    pList->SetUpdated ( false );
}


void CServerBrowser::AddServerToList ( CServerListItem * pServer, ServerBrowserType Type )
{
    pServer->bAddedToList[ Type ] = true;

    bool bIncludeEmpty  = m_pIncludeEmpty [ Type ]->GetSelected ();
    bool bIncludeFull   = m_pIncludeFull [ Type ]->GetSelected ();
    bool bIncludeLocked = m_pIncludeLocked [ Type ]->GetSelected ();
    bool bServerSearchFound = true;
    bool bPlayerSearchFound = true;

    std::string strServerSearchText = m_pEditServerSearch [ Type ]->GetText ();
    std::string strPlayerSearchText = m_pEditPlayerSearch [ Type ]->GetText ();

    if ( !strServerSearchText.empty() )
    {
        for ( unsigned int i = 0; i < strServerSearchText.length (); i++ ) 
            strServerSearchText[i] = tolower ( strServerSearchText[i] );

        // Search for the search text in the servername
        std::string strServerName = pServer->strName;
        for ( unsigned int i = 0; i < strServerName.length (); i ++ ) 
            strServerName[i] = tolower ( strServerName[i] );

        bServerSearchFound = strServerName.find(strServerSearchText) != string::npos;
    }

    if ( !strPlayerSearchText.empty() )
    {
        bPlayerSearchFound = false;

        if ( pServer->nPlayers > 0 )
        {
            for ( unsigned int i = 0; i < strPlayerSearchText.length (); i++ ) 
                strPlayerSearchText[i] = tolower ( strPlayerSearchText[i] );

            // Search for the search text in the names of the players in the server
            for ( unsigned int i = 0; i < pServer->vecPlayers.size (); i++ ) 
            {
                std::string strPlayerName = pServer->vecPlayers[i].c_str ();
                std::string strPlayerNameLower = strPlayerName;

                for ( unsigned int j = 0; j < strPlayerNameLower.length (); j++ )
                    strPlayerNameLower[j] = tolower ( strPlayerNameLower[j] );

                if ( strPlayerNameLower.find(strPlayerSearchText) != string::npos )
                {
                    bPlayerSearchFound = true;
                    int k = m_pServerPlayerList [ Type ]->AddRow ( true );
                    m_pServerPlayerList [ Type ]->SetItemText ( k, m_hPlayerName [ Type ], strPlayerName.c_str (), false, false, true );
                }
            }
        }
    }

    if (
        ( pServer->nPlayers > 0 || bIncludeEmpty ) &&
        ( pServer->nPlayers < pServer->nMaxPlayers || pServer->nPlayers == 0 || pServer->nMaxPlayers == 0 || bIncludeFull ) &&
        ( !pServer->bPassworded || bIncludeLocked ) && bServerSearchFound && bPlayerSearchFound
       )
    {
        // Create a new row
        int iIndex = m_pServerList [ Type ]->AddRow ( true );

        // Format some text data
        char buf[32] = {0};
        stringstream ssPlayers;
        ssPlayers << pServer->nPlayers << " / " << pServer->nMaxPlayers;
        std::string strEndpoint = pServer->strHost + ":" + itoa ( pServer->usGamePort, buf, 10 );

        // The row index could change at any point here if list sorting is enabled
        iIndex = m_pServerList [ Type ]->SetItemText ( iIndex, m_hName [ Type ],     pServer->strName.c_str (), false, false, true );
        iIndex = m_pServerList [ Type ]->SetItemText ( iIndex, m_hGame [ Type ],     pServer->strType.c_str (), false, false, true );
        iIndex = m_pServerList [ Type ]->SetItemText ( iIndex, m_hMap [ Type ],      pServer->strMap.c_str (), false, false, true );
        iIndex = m_pServerList [ Type ]->SetItemText ( iIndex, m_hHost [ Type ],     strEndpoint.c_str (), false, false, true );
        iIndex = m_pServerList [ Type ]->SetItemText ( iIndex, m_hPlayers [ Type ],  ssPlayers.str ().c_str (), true, false, true );
        iIndex = m_pServerList [ Type ]->SetItemText ( iIndex, m_hPing [ Type ],     itoa ( pServer->nPing, buf, 10 ), true, false, true );

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

    char buf[32];

    if ( pElement == m_pServerPlayerList [ Type ] && m_pServerPlayerList [ Type ]->GetSelectedCount () >= 1 )
    {
        // Get the selected row of the player gridlist
        int iSelectedIndex = m_pServerPlayerList [ Type ]->GetSelectedItemRow ();
        std::string strSelectedPlayerName = m_pServerPlayerList [ Type ]->GetItemText ( iSelectedIndex, m_hPlayerName [ Type ] );

        // Walk the server list looking for the player on a server 
        CServerList * pList = GetServerList ( Type );
        CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
        for ( i = i_b; i != i_e; i++ ) 
        {
            CServerListItem * pServer = *i;

            for ( unsigned int j = 0; j < pServer->vecPlayers.size (); j++ )
            {
                std::string strPlayerName = pServer->vecPlayers[j].c_str ();
                if ( strPlayerName.compare ( strSelectedPlayerName ) == 0 )
                {
                    // We found the server on which the player is
                    // Walk the server gridlist looking for the server host to get the row index
                    std::string strEndpoint = pServer->strHost + ":" + itoa ( pServer->usGamePort, buf, 10 );
                    for ( int k = 0; k < m_pServerList [ Type ]->GetRowCount (); k++ )
                    {
                        if ( strEndpoint.compare ( m_pServerList [ Type ]->GetItemText ( k, m_hHost [ Type ] ) ) == 0 )
                        {
                            // We found the index, select it
                            m_pServerList [ Type ]->SetSelectedItem ( k, m_hHost [ Type ], true );

                            // It's not the same server as was selected before, so we update the password
                            if ( k != m_iSelectedServer[ Type ] )
                            {
                                bool bSavedPasswords;
                                CVARS_GET ( "save_server_passwords", bSavedPasswords );
                                if ( pServer->bPassworded && bSavedPasswords )
                                {
                                    m_pEditPassword [ Type ]->SetText ( GetServerPassword(strEndpoint).c_str() );
                                }
                                else
                                {
                                    m_pEditPassword [ Type ]->SetText ( "" );
                                }
                            }

                            // save the selected server
                            m_iSelectedServer [ Type ] = iSelectedIndex;

                            return true;
                        }
                    }
                }
            }
        }
    }
 
    // If there is one item selected
    if ( m_pServerList [ Type ]->GetSelectedCount () >= 1 )
    {
        // Clear the player list
        m_pServerPlayerList [ Type ]->Clear ();

        // Get the selected row
        int iSelectedIndex = m_pServerList [ Type ]->GetSelectedItemRow ();

        // Walk the server list looking for this server
        CServerList * pList = GetServerList ( Type );
        CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
        std::string strSelectedEndpoint = m_pServerList [ Type ]->GetItemText ( iSelectedIndex, m_hHost [ Type ] );
        for ( i = i_b; i != i_e; i++ ) 
        {
            CServerListItem * pServer = *i;
            std::string strEndpoint = pServer->strHost + ":" + itoa ( pServer->usGamePort, buf, 10 );
            if ( strSelectedEndpoint.compare ( strEndpoint ) == 0 ) 
            {
                // We found the server, add all the players
                for ( unsigned int j = 0; j < pServer->vecPlayers.size (); j++ ) 
                {
                    int k = m_pServerPlayerList [ Type ]->AddRow ();
                    m_pServerPlayerList [ Type ]->SetItemText ( k, m_hPlayerName [ Type ], pServer->vecPlayers[j].c_str () );
                }

                // It's not the same server as was selected before, so we update the password
                if ( iSelectedIndex != m_iSelectedServer[ Type ] )
                {
                    bool bSavedPasswords;
                    CVARS_GET ( "save_server_passwords", bSavedPasswords );
                    if ( pServer->bPassworded && bSavedPasswords )
                    {
                        m_pEditPassword [ Type ]->SetText ( GetServerPassword(strEndpoint).c_str() );
                    }
                    else
                    {
                        m_pEditPassword [ Type ]->SetText ( "" );
                    }
                }
            }
        }

        // save the selected server
        m_iSelectedServer [ Type ] = iSelectedIndex;
    }
    else
    {
        m_pEditPassword [ Type ]->SetText ( "" );
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
    m_pServerPlayerList [ Type ]->Clear ();

    char buf[32];
 
    // If there is one item selected
    if ( m_pServerList [ Type ]->GetSelectedCount () >= 1 )
    {
        // Get the selected row
        int iSelectedIndex = m_pServerList [ Type ]->GetSelectedItemRow ();

        // Walk the server list looking for this server
        CServerList * pList = GetServerList ( Type );
        CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
        std::string strSelectedEndpoint = m_pServerList [ Type ]->GetItemText ( iSelectedIndex, m_hHost [ Type ] );
        for ( i = i_b; i != i_e; i++ ) {
            CServerListItem * pServer = *i;
            std::string strEndpoint = pServer->strHost + ":" + itoa ( pServer->usGamePort, buf, 10 );
            if ( strSelectedEndpoint.compare ( strEndpoint ) == 0 )
            {
                if ( ( pServer->bSerials ) && ( !g_pCore->GetCommunity()->IsLoggedIn() ) )
                {
                    m_pCommunityLogin.SetVisible ( true );
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
                
                bool bSavedPasswords;
                CVARS_GET ( "save_server_passwords", bSavedPasswords );
                if ( pServer->bPassworded && bSavedPasswords )
                {
                    SetServerPassword ( strEndpoint, ( std::string )szPassword );
                }
                

                // Start the connect
                CCore::GetSingleton ().GetConnectManager ()->Connect ( pServer->strHost.c_str (), pServer->usGamePort, strNick.c_str (), szPassword );
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
    ServerBrowserType Type = GetCurrentServerBrowserType ();

    GetServerList ( Type )->Refresh ();

    //Clear the search fields
    m_pEditServerSearch [ Type ]->SetText("");
    m_pEditPlayerSearch [ Type ]->SetText("");
    return true;
}

bool CServerBrowser::OnFavouritesClick ( CGUIElement* pElement )
{
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

        char* szHost = strtok ( szAddress, ":" );
        char* szPort = strtok ( NULL, "\0" );

        if ( szName && szHost && szPort )
        {
            unsigned short usPort = static_cast < unsigned short > ( atoi ( szPort ) ) + SERVER_LIST_QUERY_PORT_OFFSET;

            in_addr Address;

            CServerListItem::Parse ( szHost, Address );

            CServerListItem pServer ( Address, usPort );

            if ( currentServerBrowserType == ServerBrowserType::FAVOURITES )
            {
                m_ServersFavourites.Remove ( pServer );
                SaveFavouritesList();
            }
            else
            {
                // Make sure the user didn't pull a Talidan and add the server again
                if ( !m_ServersFavourites.Exists ( pServer ) )
                {
                    m_ServersFavourites.Add ( pServer );
                    SaveFavouritesList();
                }
            }
            UpdateServerList ( ServerBrowserType::FAVOURITES, true );
        }
    }
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
    ServerBrowserType Type = GetCurrentServerBrowserType ();

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
    else if ( Args.pWindow == m_pServerPlayerList [ Type ] && !m_pEditPlayerSearch [ Type ]->GetText ().empty() )
    {
        OnClick ( m_pServerPlayerList [ Type ] );
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

bool CServerBrowser::OnFilterChanged ( CGUIElement* pElement )
{
    UpdateServerList ( GetCurrentServerBrowserType (), true );
    SaveOptions ( );

    return true;
}

bool CServerBrowser::OnTabChanged ( CGUIElement* pElement )
{
    SaveOptions ( );

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
    if ( !CServerListItem::Parse ( strHost.c_str (), Address ) || strHost.length () == 0 || iPort <= 0 )
    {
        CCore::GetSingleton ().ShowMessageBox ( "Error", "Incorrect host or port specified!", MB_BUTTON_OK | MB_ICON_INFO );
        return true;
    }

    // Construct list item
    CServerListItem Item ( Address, iPort, strHost );

    // Add the item if it doesn't already exist
    if ( !m_ServersFavourites.Exists ( Item ) )
    {
        m_ServersFavourites.Add ( Item );
        SaveFavouritesList();
        UpdateServerList ( ServerBrowserType::FAVOURITES );
    }

    m_pFavouritesAddByIP.Reset ();
    m_pFavouritesAddByIP.SetVisible ( false );

    return true;
}

bool CServerBrowser::LoadServerList ( CXMLNode* pNode, const std::string& strTagName, CServerList *pList )
{
    CXMLNode* pSubNode = NULL;
    in_addr Address;
    int iPort;

    if ( !pNode )
        return false;

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
                if ( CServerListItem::Parse ( pHostAttribute->GetValue ().c_str (), Address ) )
                {
                    iPort = atoi ( pPortAttribute->GetValue ().c_str () ) + SERVER_LIST_QUERY_PORT_OFFSET;
                    if ( iPort > 0 )
                        pList->Add ( CServerListItem ( Address, iPort ) );
                }
            }
        }
    }
    pList->SetUpdated ( true );
    return true;
}


void CServerBrowser::LoadInternetList()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    LoadServerList ( pConfig->FindSubNode ( CONFIG_NODE_SERVER_INT ),
            CONFIG_INTERNET_LIST_TAG, GetInternetList () );
}


void CServerBrowser::SaveInternetList()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    CXMLNode* pRecent = pConfig->FindSubNode ( CONFIG_NODE_SERVER_INT );
    if ( !pRecent )
        pRecent = pConfig->CreateSubNode ( CONFIG_NODE_SERVER_INT );
    SaveServerList ( pRecent, CONFIG_INTERNET_LIST_TAG, GetInternetList () );
}


void CServerBrowser::SaveRecentlyPlayedList()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    CXMLNode* pRecent = pConfig->FindSubNode ( CONFIG_NODE_SERVER_REC );
    if ( !pRecent )
        pRecent = pConfig->CreateSubNode ( CONFIG_NODE_SERVER_REC );
    SaveServerList ( pRecent, CONFIG_RECENT_LIST_TAG, GetRecentList () );
}


void CServerBrowser::SaveFavouritesList()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    CXMLNode* pFavourites = pConfig->FindSubNode ( CONFIG_NODE_SERVER_FAV );
    if ( !pFavourites )
        pFavourites = pConfig->CreateSubNode ( CONFIG_NODE_SERVER_FAV );
    SaveServerList ( pFavourites, CONFIG_FAVOURITE_LIST_TAG, GetFavouritesList () );
}


bool CServerBrowser::SaveServerList ( CXMLNode* pNode, const std::string& strTagName, CServerList *pList )
{
    if ( !pNode )
        return false;

    // Start by clearing out all previous nodes
    pNode->DeleteAllSubNodes ();

    // Iterate through the list, adding any items to our node
    CServerListIterator i, i_b = pList->IteratorBegin (), i_e = pList->IteratorEnd ();
    int j = 0;
    int k = pList->GetServerCount ();
    if ( k > 0 )
    {
        for ( CServerListIterator i = i_b; i != i_e; i++ )
        {
            CServerListItem * pServer = *i;

            // Add the item to the node
            CXMLNode * pSubNode = pNode->CreateSubNode ( strTagName.c_str () );
            if ( pSubNode )
            {
                CXMLAttribute* pHostAttribute = pSubNode->GetAttributes ().Create ( "host" );
                std:string strHost = pServer->strHost;
                if ( !pServer->strHostName.empty () )
                    strHost = pServer->strHostName;
				pHostAttribute->SetValue ( strHost.c_str () );
                
                CXMLAttribute* pPortAttribute = pSubNode->GetAttributes ().Create ( "port" );
				pPortAttribute->SetValue ( pServer->usGamePort );
            }
            j++;
        }
    }
    return true;
}

void CServerBrowser::LoadOptions ( CXMLNode* pNode )
{
    if ( !pNode )
        return;

    // loop through all subnodes
    unsigned int uiCount = pNode->GetSubNodeCount ( ); 
    for ( unsigned int ui = 0; ui < uiCount; ui ++ )
    {
        CXMLNode * pSubNode = pNode->GetSubNode ( ui );
        if ( pSubNode && pSubNode->GetTagName ( ).compare ( "list" ) == 0 )
        {
            CXMLAttribute* pListID = pSubNode->GetAttributes ( ).Find ( "id" );
            if ( pListID )
            {
                // Check for a valid list ID
                int i = atoi ( pListID->GetValue ().c_str () );
                if ( i >= 0 && i < SERVER_BROWSER_TYPE_COUNT )
                {
                    // load all checkbox options
                    CXMLAttribute* pIncludeEmpty = pSubNode->GetAttributes ( ).Find ( "include_empty" );
                    if ( pIncludeEmpty )
                        m_pIncludeEmpty[ i ]->SetSelected ( pIncludeEmpty->GetValue ( ).compare ( "1" ) == 0 );

                    CXMLAttribute* pIncludeFull = pSubNode->GetAttributes ( ).Find ( "include_full" );
                    if ( pIncludeFull )
                        m_pIncludeFull[ i ]->SetSelected ( pIncludeFull->GetValue ( ).compare ( "1" ) == 0 );

                    CXMLAttribute* pIncludeLocked = pSubNode->GetAttributes ( ).Find ( "include_locked" );
                    if ( pIncludeLocked )
                        m_pIncludeLocked[ i ]->SetSelected ( pIncludeLocked->GetValue ( ).compare ( "1" ) == 0 );

                    // load 'include offline' if the checkbox exists
                    if ( m_pIncludeOffline[ i ] )
                    {
                        CXMLAttribute* pIncludeOffline = pSubNode->GetAttributes ( ).Find ( "include_offline" );
                        if ( pIncludeOffline )
                            m_pIncludeOffline[ i ]->SetSelected ( pIncludeOffline->GetValue ( ).compare ( "1" ) == 0 );
                    }

                    // restore the active tab
                    CXMLAttribute* pActiveTab = pSubNode->GetAttributes ( ).Find ( "active" );
                    if ( pActiveTab && pActiveTab->GetValue ( ).compare ( "1" ) == 0 )
                        m_pTabs->SetSelectedTab ( m_pTab [ i ] );

                    // restore the search field contents
                    std::string strSearch = pSubNode->GetTagContent ( );
                    if ( strSearch.length ( ) > 0 )
                        m_pEditServerSearch [ i ]->SetText ( strSearch.c_str ( ) );
                }
            }
        }
    }
    m_OptionsLoaded = true;
}

void CServerBrowser::SaveOptions ( )
{
    // Check to make sure if the options were loaded yet, if not the 'changed' events might screw up
    if ( !m_OptionsLoaded )
        return;

    CXMLNode* pConfig = CCore::GetSingletonPtr ( )->GetConfig ( );
    CXMLNode* pOptions = pConfig->FindSubNode ( CONFIG_NODE_SERVER_OPTIONS );
    if ( !pOptions )
    {
        pOptions = pConfig->CreateSubNode ( CONFIG_NODE_SERVER_OPTIONS );
    }
    else
    {
        // start with a clean node
        pOptions->DeleteAllSubNodes ( );
    }

    int iCurrentType = GetCurrentServerBrowserType ( );

    // Save the options for all four lists
    for ( unsigned int ui = 0; ui < SERVER_BROWSER_TYPE_COUNT; ui++ )
    {
        CXMLNode * pSubNode = pOptions->CreateSubNode ( "list" );
        if ( pSubNode ) 
        {
            // ID of the list to save
            CXMLAttribute* pListID = pSubNode->GetAttributes ( ).Create ( "id" );
            pListID->SetValue ( ui );

            // Checkboxes
            CXMLAttribute* pIncludeEmpty = pSubNode->GetAttributes ( ).Create ( "include_empty" );
            pIncludeEmpty->SetValue ( m_pIncludeEmpty [ ui ]->GetSelected ( ) );

            CXMLAttribute* pIncludeFull = pSubNode->GetAttributes ( ).Create ( "include_full" );
            pIncludeFull->SetValue ( m_pIncludeFull [ ui ]->GetSelected ( ) );

            CXMLAttribute* pIncludeLocked = pSubNode->GetAttributes ( ).Create ( "include_locked" );
            pIncludeLocked->SetValue ( m_pIncludeLocked [ ui ]->GetSelected ( ) );

            // Only recently played & favorites have 'Include offline'
            if ( m_pIncludeOffline [ ui ] )
            {
                CXMLAttribute* pIncludeOffline = pSubNode->GetAttributes ( ).Create ( "include_offline" );
                pIncludeOffline->SetValue ( m_pIncludeOffline [ ui ]->GetSelected ( ) );
            }

            // Save the active Tab
            if ( iCurrentType == ui )
            {
                CXMLAttribute* pActive = pSubNode->GetAttributes ( ).Create ( "active" );
                pActive->SetValue ( 1 );
            }

            // save the search box content
            std::string strSearch = m_pEditServerSearch [ ui ]->GetText ( );
            if ( strSearch.length ( ) > 0 )
            {
                pSubNode->SetTagContent ( strSearch.c_str ( ) );
            }
        }
    }
}

void CServerBrowser::SetServerPassword ( const std::string& strHost, const std::string& strPassword )
{
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    CXMLNode* pServerPasswords = pConfig->FindSubNode ( CONFIG_NODE_SERVER_SAVED );
    if ( !pServerPasswords )
    {
        pServerPasswords = pConfig ->CreateSubNode ( CONFIG_NODE_SERVER_SAVED );
    }
    //Check if the server password already exists
    for ( unsigned int i = 0 ; i < pServerPasswords->GetSubNodeCount() ; i++ )
    {    
        CXMLAttributes* pAttributes = &(pServerPasswords->GetSubNode(i)->GetAttributes());
        if ( pAttributes->Find( "host" ) )
        {
            if ( CXMLAttribute* pHost = pAttributes->Find ( "host" ) )
            {
                const std::string& strXMLHost = pHost->GetValue();
                if ( strXMLHost == strHost )
                {
                    CXMLAttribute* pPassword = pAttributes->Create( "password" );
                    pPassword->SetValue(strPassword.c_str());
                    return;
                }
            }
        }
        
    }

    // Otherwise create the node from scratch
    CXMLNode* pNode = pServerPasswords->CreateSubNode( "server" );
    CXMLAttribute* pHostAttribute = pNode->GetAttributes().Create ( "host" );
    pHostAttribute->SetValue(strHost.c_str());
    CXMLAttribute* pPasswordAttribute = pNode->GetAttributes().Create ( "password" );
    pPasswordAttribute->SetValue(strPassword.c_str());
}


std::string CServerBrowser::GetServerPassword ( const std::string& strHost )
{
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    CXMLNode* pServerPasswords = pConfig->FindSubNode ( CONFIG_NODE_SERVER_SAVED );
    if ( !pServerPasswords )
    {
        pServerPasswords = pConfig ->CreateSubNode ( CONFIG_NODE_SERVER_SAVED );
    }
    //Check if the server password already exists
    for ( unsigned int i = 0 ; i < pServerPasswords->GetSubNodeCount() ; i++ )
    {    
        CXMLAttributes* pAttributes = &(pServerPasswords->GetSubNode(i)->GetAttributes());
        if ( pAttributes->Find( "host" ) )
        {
            if ( CXMLAttribute* pHost = pAttributes->Find ( "host" ) )
            {
                const std::string& strXMLHost = pHost->GetValue();
                if ( pHost->GetValue() == strHost )
                {
                    CXMLAttribute* pPassword = pAttributes->Create( "password" );
                    const std::string& strPassword = pPassword->GetValue();
                    return strPassword;
                }
            }
        }
        
    }
    return "";
}


void CServerBrowser::ClearServerPasswords ()
{
    CXMLNode* pConfig = CCore::GetSingletonPtr ()->GetConfig ();
    CXMLNode* pServerPasswords = pConfig->FindSubNode ( CONFIG_NODE_SERVER_SAVED );
    if ( pServerPasswords )
    {
        pServerPasswords->DeleteAllSubNodes();
        pConfig->DeleteSubNode ( pServerPasswords );
    }
}
