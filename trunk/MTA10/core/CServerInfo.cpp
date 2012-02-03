/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerInfo.cpp
*  PURPOSE:     In-game server queue user interface
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

extern CCore* g_pCore;

template<> CServerInfo * CSingleton < CServerInfo >::m_pSingleton = NULL;

CServerInfo::CServerInfo ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Initialize
    m_ulLastUpdateTime = CClientTime::GetTime ();

    // Obtain our screen resolution
    CVector2D vecResolution = CCore::GetSingleton().GetGUI()->GetResolution();

    // Create queue window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "SERVER IS FULL" ) );
    m_pWindow->SetCloseButtonEnabled( false );
    m_pWindow->SetMovable( true );
    m_pWindow->SetSizingEnabled( false );
    m_pWindow->SetPosition ( CVector2D ( vecResolution.fX / 2 - INFO_WINDOW_DEFAULTWIDTH / 2, vecResolution.fY / 2 - INFO_WINDOW_DEFAULTHEIGHT / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH, INFO_WINDOW_DEFAULTHEIGHT ) );
    m_pWindow->BringToFront();
    m_pWindow->SetZOrderingEnabled(false);
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->SetMinimumSize ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH, INFO_WINDOW_DEFAULTHEIGHT ) );

    unsigned int LabelTitlePosX = 0;
    unsigned int LabelTitleSizeX = 0.3f*INFO_WINDOW_DEFAULTWIDTH;
    unsigned int LabelPosX = 0.325f*INFO_WINDOW_DEFAULTWIDTH;
    unsigned int LabelSizeX = 0.55f*INFO_WINDOW_DEFAULTWIDTH;
    unsigned int LabelSizeY = 15;
    unsigned int DrawPosY = 10; //Start position

    // Server Name
    m_pServerNameLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Name:" ) );
    m_pServerNameLabelTitle->SetPosition ( CVector2D (LabelTitlePosX, DrawPosY+=INFO_LABEL_VSPACING+LabelSizeY ), false );
    m_pServerNameLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pServerNameLabelTitle->SetFont ( "default-bold-small" );
    m_pServerNameLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pServerNameLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pServerNameLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pServerNameLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);


    // Server IP
    m_pServerAddressLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Server Address:" ) );
    m_pServerAddressLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=INFO_LABEL_VSPACING+LabelSizeY ), false );
    m_pServerAddressLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pServerAddressLabelTitle->SetFont ( "default-bold-small" );
    m_pServerAddressLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pServerAddressLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pServerAddressLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pServerAddressLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);


    // Gamemode
    m_pGamemodeLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Gamemode:" ) );
    m_pGamemodeLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=INFO_LABEL_VSPACING+LabelSizeY ), false );
    m_pGamemodeLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pGamemodeLabelTitle->SetFont ( "default-bold-small" );
    m_pGamemodeLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pGamemodeLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pGamemodeLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pGamemodeLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Map
    m_pMapLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Map:" ) );
    m_pMapLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=INFO_LABEL_VSPACING+LabelSizeY ), false );
    m_pMapLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pMapLabelTitle->SetFont ( "default-bold-small" );
    m_pMapLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pMapLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pMapLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pMapLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Players
    m_pPlayersLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Players:" ) );
    m_pPlayersLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=INFO_LABEL_VSPACING+LabelSizeY ), false );
    m_pPlayersLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pPlayersLabelTitle->SetFont ( "default-bold-small" );
    m_pPlayersLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pPlayersLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pPlayersLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pPlayersLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Passworded
    m_pPasswordedLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Passworded:" ) );
    m_pPasswordedLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=INFO_LABEL_VSPACING+LabelSizeY ), false );
    m_pPasswordedLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pPasswordedLabelTitle->SetFont ( "default-bold-small" );
    m_pPasswordedLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pPasswordedLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pPasswordedLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pPasswordedLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Latency
    m_pLatencyLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Latency:" ) );
    m_pLatencyLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=INFO_LABEL_VSPACING+LabelSizeY ), false );
    m_pLatencyLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pLatencyLabelTitle->SetFont ( "default-bold-small" );
    m_pLatencyLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pLatencyLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pLatencyLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pLatencyLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Player list
    m_pServerPlayerList = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( m_pWindow ) );
    m_pServerPlayerList->SetPosition ( CVector2D ( INFO_WINDOW_HSPACING*2, DrawPosY+=(INFO_WINDOW_VSPACING+LabelSizeY) ), false );
    unsigned int PlayerListHeight = (INFO_WINDOW_DEFAULTHEIGHT - INFO_WINDOW_VSPACING - INFO_BUTTON_HEIGHT - INFO_WINDOW_VSPACING - LabelSizeY - INFO_WINDOW_VSPACING) - DrawPosY;
    m_pServerPlayerList->SetSize ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH-INFO_WINDOW_HSPACING*4, PlayerListHeight	), false );
    m_pServerPlayerList->SetIgnoreTextSpacer ( true );

    // Column for player names
    m_hPlayerName = m_pServerPlayerList->AddColumn ( "Player list", 0.9f );

    // Now we draw from the bottom
    DrawPosY = INFO_WINDOW_DEFAULTHEIGHT - INFO_WINDOW_VSPACING;

    // Close button
    m_pButtonClose = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Close" ) );
    m_pButtonClose->SetPosition ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH-(INFO_BUTTON_WIDTH*2)-(1.5f*INFO_WINDOW_HSPACING), DrawPosY-=INFO_BUTTON_HEIGHT ), false );
    m_pButtonClose->SetSize ( CVector2D ( INFO_BUTTON_WIDTH, INFO_BUTTON_HEIGHT ), false );
    m_pButtonClose->SetClickHandler ( GUI_CALLBACK ( &CServerInfo::OnCloseClicked, this ) );
    m_pButtonClose->SetZOrderingEnabled ( false );

    // Join Game button
    m_pButtonJoinGame = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Join Game" ) );
    m_pButtonJoinGame->SetPosition ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH-INFO_BUTTON_WIDTH-INFO_WINDOW_HSPACING, DrawPosY ), false );
    m_pButtonJoinGame->SetSize ( CVector2D ( INFO_BUTTON_WIDTH, INFO_BUTTON_HEIGHT ), false );
    m_pButtonJoinGame->SetClickHandler ( GUI_CALLBACK ( &CServerInfo::OnJoinGameClicked, this ) );
    m_pButtonJoinGame->SetZOrderingEnabled ( false );

    // Password entry editbox
    m_pEnterPasswordEdit = reinterpret_cast < CGUIEdit* > ( pManager->CreateEdit ( m_pWindow, "" ) );
    m_pEnterPasswordEdit->SetPosition ( CVector2D ( INFO_WINDOW_HSPACING*2, DrawPosY - 4 ), false );
    m_pEnterPasswordEdit->SetSize ( CVector2D ( INFO_BUTTON_WIDTH*2, INFO_BUTTON_HEIGHT ), false );
    m_pEnterPasswordEdit->SetMasked ( true );
    m_pEnterPasswordEdit->SetTextAcceptedHandler ( GUI_CALLBACK ( &CServerInfo::OnJoinGameClicked, this ) );

    // Autojoin checkbox
    m_pCheckboxAutojoin = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pWindow, "Join the server as soon as a player slot is available.", true ) );
    m_pCheckboxAutojoin->SetPosition ( CVector2D ( INFO_WINDOW_HSPACING*2, DrawPosY-=INFO_WINDOW_VSPACING+LabelSizeY ), false );
    m_pCheckboxAutojoin->SetSize( CVector2D ( INFO_WINDOW_DEFAULTWIDTH, LabelSizeY ), false );

    // Please enter password label
    m_pEnterPasswordLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Please enter the password to the server:" ) );
    m_pEnterPasswordLabel->SetPosition ( CVector2D ( INFO_WINDOW_HSPACING*2, DrawPosY ), false );
    m_pEnterPasswordLabel->SetSize(CVector2D(INFO_WINDOW_DEFAULTWIDTH, LabelSizeY), false);
    m_pEnterPasswordLabel->SetFont("default-bold-small");
    m_pEnterPasswordLabel->SetTextColor(255,0,0);
}

CServerInfo::~CServerInfo ( void )
{
    // Delete our GUI elements
    delete m_pWindow;
    m_pWindow = 0;

    delete m_pServerNameLabel;
    m_pServerNameLabel = 0;

    delete m_pServerAddressLabel;
    m_pServerAddressLabel = 0;

    delete m_pGamemodeLabel;
    m_pGamemodeLabel = 0;

    delete m_pMapLabel;
    m_pMapLabel = 0;

    delete m_pPlayersLabel;
    m_pPlayersLabel = 0;

    delete m_pPasswordedLabel;
    m_pPasswordedLabel = 0;

    delete m_pLatencyLabel;
    m_pLatencyLabel = 0;

    delete m_pServerNameLabelTitle;
    m_pServerNameLabelTitle = 0;

    delete m_pServerAddressLabelTitle;
    m_pServerAddressLabelTitle = 0;

    delete m_pGamemodeLabelTitle;
    m_pGamemodeLabelTitle = 0;

    delete m_pMapLabelTitle;
    m_pMapLabelTitle = 0;

    delete m_pPlayersLabelTitle;
    m_pPlayersLabelTitle = 0;

    delete m_pPasswordedLabelTitle;
    m_pPasswordedLabelTitle = 0;

    delete m_pLatencyLabelTitle;
    m_pLatencyLabelTitle = 0;

    delete m_pServerPlayerList;
    m_pServerPlayerList = 0;

    delete m_pCheckboxAutojoin;
    m_pCheckboxAutojoin = 0;

    delete m_pButtonJoinGame;
    m_pButtonJoinGame = 0;

    delete m_pButtonClose;
    m_pButtonClose = 0;

    delete m_pEnterPasswordLabel;
    m_pEnterPasswordLabel = 0;

    delete m_pEnterPasswordEdit;
    m_pEnterPasswordEdit = 0;
}

bool CServerInfo::OnCloseClicked ( CGUIElement* pElement )
{
    // Hide this window
    m_pWindow->SetVisible( false );
    return true;
}

bool CServerInfo::OnJoinGameClicked ( CGUIElement* pElement )
{
    // Attempt to connect to the server
    Connect();
    return true;
}

bool CServerInfo::IsVisible( void )
{
    return m_pWindow->IsVisible ();
}

void CServerInfo::Hide( void )
{
    m_pWindow->SetVisible ( false );
}

void CServerInfo::Show ( eWindowType WindowType )
{
   Show(WindowType, g_pCore->GetConnectManager()->m_strLastHost.c_str(), g_pCore->GetConnectManager()->m_usLastPort,"" );
}

void CServerInfo::Show( eWindowType WindowType, const char* szHost, unsigned short usPort, const char* szPassword, CServerListItem* pInitialServerListItem )
{
    m_pWindow->SetZOrderingEnabled(true);
    m_pWindow->SetVisible (true);
    m_pWindow->BringToFront ();

    m_pEnterPasswordEdit->SetText ( "" );

    // Adjust our window according to what type of window we are displaying
    float fHeight;
    unsigned int LabelSizeY = 15;
    float PlayerListHeight = (INFO_WINDOW_DEFAULTHEIGHT - INFO_WINDOW_VSPACING - INFO_BUTTON_HEIGHT - INFO_WINDOW_VSPACING - LabelSizeY - INFO_WINDOW_VSPACING) - m_pServerPlayerList->GetPosition().fY;
    if ( WindowType == eWindowTypes::SERVER_INFO_QUEUE )
    {
        fHeight = INFO_WINDOW_DEFAULTHEIGHT;
        m_pCheckboxAutojoin->SetVisible( true );
        m_pEnterPasswordEdit->SetVisible( false );
        m_pEnterPasswordLabel->SetVisible( false );
        m_pWindow->SetText ( "SERVER IS FULL" );
    }
    else if ( WindowType == eWindowTypes::SERVER_INFO_PASSWORD )
    {
        fHeight = PASSWORD_WINDOW_DEFAULTHEIGHT;
        m_pCheckboxAutojoin->SetVisible( false );
        m_pEnterPasswordEdit->SetVisible( true );
        m_pEnterPasswordLabel->SetVisible( true );
        m_pWindow->SetText ( "PLEASE ENTER SERVER PASSWORD" );
        m_bWaitingToActivatePassword = true;
    }
    else
    {
        PlayerListHeight += LabelSizeY;
        fHeight = INFO_WINDOW_DEFAULTHEIGHT;
        m_pCheckboxAutojoin->SetVisible( false );
        m_pEnterPasswordEdit->SetVisible( false );
        m_pEnterPasswordLabel->SetVisible( false );
        m_pWindow->SetText ( "Information" );
    }
    float DrawPosY = fHeight - INFO_WINDOW_VSPACING - INFO_BUTTON_HEIGHT;

    m_pCurrentWindowType = WindowType;
   
    m_pServerPlayerList->SetSize ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH-INFO_WINDOW_HSPACING*4, PlayerListHeight	), false );

    m_pWindow->SetSize ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH, fHeight ), false );

    m_pButtonJoinGame->SetPosition ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH-INFO_BUTTON_WIDTH-INFO_WINDOW_HSPACING, DrawPosY ), false );        
    m_pButtonClose->SetPosition ( CVector2D ( INFO_WINDOW_DEFAULTWIDTH-(INFO_BUTTON_WIDTH*2)-(1.5f*INFO_WINDOW_HSPACING), DrawPosY ), false );

    m_pWindow->SetZOrderingEnabled(false);

    pInitialServerListItem = pInitialServerListItem ? pInitialServerListItem : CCore::GetSingleton ().GetLocalGUI()->GetMainMenu()->GetServerBrowser()->FindServer ( szHost, usPort );

    SetServerInformation( szHost, usPort, szPassword, pInitialServerListItem );
}


void CServerInfo::SetServerInformation( const char* szHost, unsigned short usPort, const char* szPassword, CServerListItem* pInitialServerListItem )
{
    // Store the parameters in our class instance for later use
    m_usPort = usPort;

    m_strPassword = szPassword;
    m_strHost = szHost;

    // Create a winsock address endpoint and parse the IP into it
    in_addr Address;
    CServerListItem::Parse(szHost, Address);

    // Set our server query's address, query port and game port
    m_Server.Address = Address;
    m_Server.usQueryPort = usPort + SERVER_LIST_QUERY_PORT_OFFSET;
    m_Server.usGamePort = usPort;

    if ( pInitialServerListItem )  // If we have a pointer to an already scanned server, we initially set text to this
        ResetServerGUI ( pInitialServerListItem );
    else // Otherwise, reset it to blank text
        Reset();

    m_pServerAddressLabel->SetText(SString("%s:%u",szHost,usPort));

    // Lets query the server now, as the previous data is out of date
    Refresh();
}

void CServerInfo::DoPulse( void )
{
    // Are we visible?
    if ( IsVisible() )
    {
        // Pulse the server query
        m_Server.Pulse ( true );

        // Do we need a refresh?
        if ( ( m_Server.bScanned && (CClientTime::GetTime () - m_ulLastUpdateTime) >=  SERVER_UPDATE_INTERVAL ) )
        {
            // Are we queing, and is auto join enabled?
            if ( m_pCurrentWindowType == eWindowTypes::SERVER_INFO_QUEUE && m_pCheckboxAutojoin->GetSelected() )
            {
                // Is a slot available?
                if ( m_Server.nPlayers < m_Server.nMaxPlayers )
                {
                    // Lets attempt to connect
                    Connect();
                    return;
                }
            }

            ResetServerGUI(&m_Server);

            m_ulLastUpdateTime = CClientTime::GetTime ();

            // Query the server
            Refresh ();
        }
        else if ( m_Server.bSkipped ) // Server query timed out
        {
            // Should we try to query again?
            if ( CClientTime::GetTime () - m_ulLastUpdateTime >=  SERVER_UPDATE_INTERVAL )
            {
                // Query the server
                Refresh ();
            }

            ResetServerGUI(&m_Server);

            // The server has timed out
            m_pLatencyLabel->SetText("Timed Out");

            m_ulLastUpdateTime = CClientTime::GetTime ();
        }

        if ( m_bWaitingToActivatePassword )
        {
            m_bWaitingToActivatePassword = false;
            m_pWindow->Activate();
            m_pEnterPasswordEdit->Activate();
        }
    }
}

void CServerInfo::Refresh( void )
{
    // Reset the query variables
    m_Server.Init();

    // Our last update time is now
    m_ulLastUpdateTime = CClientTime::GetTime ();

    // Lets query the server
    m_Server.Query();
}

void CServerInfo::Reset ( void )
{
    // Set every GUI elements text to blank
    m_pServerNameLabel->SetText("Querying...");
    m_pGamemodeLabel->SetText("");
    m_pMapLabel->SetText("");
    m_pPlayersLabel->SetText("");
    m_pPasswordedLabel->SetText("");
    m_pLatencyLabel->SetText("");
    m_pServerPlayerList->Clear();

    m_Server.Init();
}

void CServerInfo::Connect( void )
{
    // Get the nick from the cfg
    std::string strNick;
    CVARS_GET ( "nick", strNick );

    // Hide the window
    m_pWindow->SetVisible( false );

    std::string strPassword = m_strPassword;
    if ( m_pCurrentWindowType == eWindowTypes::SERVER_INFO_PASSWORD )
        strPassword = m_pEnterPasswordEdit->GetText();
    else if ( m_Server.bPassworded )  // If we're not in a passworded window, but the server is passworded
    {
        // Try to grab a saved password
        strPassword = g_pCore->GetLocalGUI()->GetMainMenu()->GetServerBrowser()->GetServerPassword( m_Server.GetEndpoint ().c_str() );

        if ( strPassword.empty() ) // No password could be found, repopup the window in password mode.
        {
            Show ( eWindowTypes::SERVER_INFO_PASSWORD, m_Server.strHost.c_str (), m_Server.usGamePort, "", &m_Server );
            return;
        }
    }

    // Let's attempt to join
    CCore::GetSingleton ().GetConnectManager ()->Connect ( m_strHost, m_usPort, strNick.c_str (), strPassword.c_str() );
}

void CServerInfo::ResetServerGUI ( CServerListItem* pServer )
{
    // Set our GUI elements to display the server information
    m_pServerNameLabel->SetText(pServer->strName.c_str());
    m_pServerAddressLabel->SetText(pServer->strEndpoint.c_str());
    m_pGamemodeLabel->SetText(pServer->strGameMode.c_str());
    m_pMapLabel->SetText(pServer->strMap.c_str());
    m_pPlayersLabel->SetText(SString("%i/%i", pServer->nPlayers, pServer->nMaxPlayers).c_str());
    
    m_pPasswordedLabel->SetText(pServer->bPassworded ? "Yes" : "No");
    m_pLatencyLabel->SetText(SString("%i", pServer->nPing));

    // Clear our player list
    m_pServerPlayerList->Clear();

    // Iterate the list of players and add their names to the player list
    for ( unsigned int i = 0; i < pServer->vecPlayers.size (); i++ ) 
    {
        std::string strPlayerName = pServer->vecPlayers[i].c_str ();

        m_pServerPlayerList->AddRow(true);
        m_pServerPlayerList->SetItemText(i, m_hPlayerName, strPlayerName.c_str(), false, false, true);
    }
}