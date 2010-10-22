/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:        core/CServerQueue.cpp
*  PURPOSE:     In-game server queue user interface
*  DEVELOPERS:  Philip Farquharson <philip@philipfarquharson.co.uk>
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#include "StdInc.h"

using namespace std;

extern CCore* g_pCore;

template<> CServerQueue * CSingleton < CServerQueue >::m_pSingleton = NULL;

CServerQueue::CServerQueue ( void )
{
    CGUI *pManager = g_pCore->GetGUI ();

    // Initialize
    m_ulLastUpdateTime = CClientTime::GetTime ();

    m_szPassword = NULL;
    m_szHost = NULL;

    m_bRequiresUpdate = false;

    // Obtain our screen resolution
    CVector2D vecResolution = CCore::GetSingleton().GetGUI()->GetResolution();

    // Create queue window
    m_pWindow = reinterpret_cast < CGUIWindow* > ( pManager->CreateWnd ( NULL, "SERVER IS FULL" ) );
    m_pWindow->SetCloseButtonEnabled( false );
    m_pWindow->SetMovable( true );
    m_pWindow->SetSizingEnabled( false );
    m_pWindow->SetPosition ( CVector2D ( vecResolution.fX / 2 - QUEUE_WINDOW_DEFAULTWIDTH / 2, vecResolution.fY / 2 - QUEUE_WINDOW_DEFAULTHEIGHT / 2  ), false );
    m_pWindow->SetSize ( CVector2D ( QUEUE_WINDOW_DEFAULTWIDTH, QUEUE_WINDOW_DEFAULTHEIGHT ) );
    m_pWindow->SetAlwaysOnTop ( true );
    m_pWindow->SetMinimumSize ( CVector2D ( QUEUE_WINDOW_DEFAULTWIDTH, QUEUE_WINDOW_DEFAULTHEIGHT ) );

    unsigned int LabelTitlePosX = 0;
    unsigned int LabelTitleSizeX = 0.3f*QUEUE_WINDOW_DEFAULTWIDTH;
    unsigned int LabelPosX = 0.325f*QUEUE_WINDOW_DEFAULTWIDTH;
    unsigned int LabelSizeX = 0.55f*QUEUE_WINDOW_DEFAULTWIDTH;
    unsigned int LabelSizeY = 15;
    unsigned int DrawPosY = 10; //Start position

    // Server Name
    m_pServerNameLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Name:" ) );
    m_pServerNameLabelTitle->SetPosition ( CVector2D (LabelTitlePosX, DrawPosY+=QUEUE_LABEL_VSPACING+LabelSizeY ), false );
    m_pServerNameLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pServerNameLabelTitle->SetFont ( "default-bold-small" );
    m_pServerNameLabelTitle->SetHorizontalAlign(CGUIHorizontalAlign::CGUI_ALIGN_RIGHT);

    m_pServerNameLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pServerNameLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pServerNameLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);


    // Server IP
    m_pServerIPLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Server IP:" ) );
    m_pServerIPLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=QUEUE_LABEL_VSPACING+LabelSizeY ), false );
    m_pServerIPLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pServerIPLabelTitle->SetFont ( "default-bold-small" );
    m_pServerIPLabelTitle->SetHorizontalAlign(CGUIHorizontalAlign::CGUI_ALIGN_RIGHT);

    m_pServerIPLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pServerIPLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pServerIPLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);


    // Gamemode
    m_pGamemodeLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Gamemode:" ) );
    m_pGamemodeLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=QUEUE_LABEL_VSPACING+LabelSizeY ), false );
    m_pGamemodeLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pGamemodeLabelTitle->SetFont ( "default-bold-small" );
    m_pGamemodeLabelTitle->SetHorizontalAlign(CGUIHorizontalAlign::CGUI_ALIGN_RIGHT);

    m_pGamemodeLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pGamemodeLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pGamemodeLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Map
    m_pMapLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Map:" ) );
    m_pMapLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=QUEUE_LABEL_VSPACING+LabelSizeY ), false );
    m_pMapLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pMapLabelTitle->SetFont ( "default-bold-small" );
    m_pMapLabelTitle->SetHorizontalAlign(CGUIHorizontalAlign::CGUI_ALIGN_RIGHT);

    m_pMapLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pMapLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pMapLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Players
    m_pPlayersLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Players:" ) );
    m_pPlayersLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=QUEUE_LABEL_VSPACING+LabelSizeY ), false );
    m_pPlayersLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pPlayersLabelTitle->SetFont ( "default-bold-small" );
    m_pPlayersLabelTitle->SetHorizontalAlign(CGUIHorizontalAlign::CGUI_ALIGN_RIGHT);

    m_pPlayersLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pPlayersLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pPlayersLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Passworded
    m_pPasswordedLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Passworded:" ) );
    m_pPasswordedLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=QUEUE_LABEL_VSPACING+LabelSizeY ), false );
    m_pPasswordedLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pPasswordedLabelTitle->SetFont ( "default-bold-small" );
    m_pPasswordedLabelTitle->SetHorizontalAlign(CGUIHorizontalAlign::CGUI_ALIGN_RIGHT);

    m_pPasswordedLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pPasswordedLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pPasswordedLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Latency
    m_pLatencyLabelTitle = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "Latency:" ) );
    m_pLatencyLabelTitle->SetPosition ( CVector2D ( LabelTitlePosX, DrawPosY+=QUEUE_LABEL_VSPACING+LabelSizeY ), false );
    m_pLatencyLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pLatencyLabelTitle->SetFont ( "default-bold-small" );
    m_pLatencyLabelTitle->SetHorizontalAlign(CGUIHorizontalAlign::CGUI_ALIGN_RIGHT);

    m_pLatencyLabel = reinterpret_cast < CGUILabel* > ( pManager->CreateLabel ( m_pWindow, "" ) );
    m_pLatencyLabel->SetPosition ( CVector2D ( LabelPosX, DrawPosY ), false );
    m_pLatencyLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Player list
    m_pServerPlayerList = reinterpret_cast < CGUIGridList* > ( pManager->CreateGridList ( m_pWindow ) );
    m_pServerPlayerList->SetPosition ( CVector2D ( QUEUE_WINDOW_HSPACING*2, DrawPosY+=(QUEUE_WINDOW_VSPACING+LabelSizeY) ), false );
    unsigned int PlayerListHeight = (QUEUE_WINDOW_DEFAULTHEIGHT - QUEUE_WINDOW_VSPACING - QUEUE_BUTTON_HEIGHT - QUEUE_WINDOW_VSPACING - LabelSizeY - QUEUE_WINDOW_VSPACING) - DrawPosY;
    m_pServerPlayerList->SetSize ( CVector2D ( QUEUE_WINDOW_DEFAULTWIDTH-QUEUE_WINDOW_HSPACING*4, PlayerListHeight      ), false );
    m_pServerPlayerList->SetIgnoreTextSpacer ( true );

    // Column for player names
    m_hPlayerName = m_pServerPlayerList->AddColumn ( "Name", 0.9f );

    // Now we draw from the bottom
    DrawPosY = QUEUE_WINDOW_DEFAULTHEIGHT - QUEUE_WINDOW_VSPACING;

    // Close button
    m_pButtonClose = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Close" ) );
    m_pButtonClose->SetPosition ( CVector2D ( QUEUE_WINDOW_DEFAULTWIDTH-(QUEUE_BUTTON_WIDTH*2)-(1.5f*QUEUE_WINDOW_HSPACING), DrawPosY-=QUEUE_BUTTON_HEIGHT ), false );
    m_pButtonClose->SetSize ( CVector2D ( QUEUE_BUTTON_WIDTH, QUEUE_BUTTON_HEIGHT ), false );
    m_pButtonClose->SetClickHandler ( GUI_CALLBACK ( &CServerQueue::OnCloseClicked, this ) );

    // Join Game button
    m_pButtonJoinGame = reinterpret_cast < CGUIButton* > ( pManager->CreateButton ( m_pWindow, "Join Game" ) );
    m_pButtonJoinGame->SetPosition ( CVector2D ( QUEUE_WINDOW_DEFAULTWIDTH-QUEUE_BUTTON_WIDTH-QUEUE_WINDOW_HSPACING, DrawPosY ), false );
    m_pButtonJoinGame->SetSize ( CVector2D ( QUEUE_BUTTON_WIDTH, QUEUE_BUTTON_HEIGHT ), false );
    m_pButtonJoinGame->SetClickHandler ( GUI_CALLBACK ( &CServerQueue::OnJoinGameClicked, this ) );

    // Autojoin checkbox
    m_pCheckboxAutojoin = reinterpret_cast < CGUICheckBox* > ( pManager->CreateCheckBox ( m_pWindow, "Join the server as soon as a player slot is available.", true ) );
    m_pCheckboxAutojoin->SetPosition ( CVector2D ( QUEUE_WINDOW_HSPACING*2, DrawPosY-=QUEUE_WINDOW_VSPACING+LabelSizeY ), false );
    m_pCheckboxAutojoin->SetSize( CVector2D ( QUEUE_WINDOW_DEFAULTWIDTH, LabelSizeY ), false );
}

CServerQueue::~CServerQueue ( void )
{
    // Delete our GUI elements
    delete m_pWindow;
    m_pWindow = 0;

    delete m_pServerNameLabel;
    m_pServerNameLabel = 0;

    delete m_pServerIPLabel;
    m_pServerIPLabel = 0;

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

    delete m_pServerIPLabelTitle;
    m_pServerIPLabelTitle = 0;

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

    if ( m_szPassword )
        delete m_szPassword;
}

bool CServerQueue::OnCloseClicked ( CGUIElement* pElement )
{
    // Hide this window
    SetVisible( false );
    return true;
}

bool CServerQueue::OnJoinGameClicked ( CGUIElement* pElement )
{
    // Attempt to connect to the server
    Connect();
    return true;
}

void CServerQueue::SetVisible( bool bVisible )
{
    // Set the visibility state
    m_pWindow->SetVisible( bVisible );

    // Are we showing when it was hidden before?
    if ( bVisible ) 
    {
        // Reset and bring us to front
        Reset();
        m_pWindow->BringToFront ();
    }
}

bool CServerQueue::IsVisible( void )
{
    return m_pWindow->IsVisible ();
}

void CServerQueue::SetServerInformation( const char* szHost, unsigned short usPort, const char* szPassword )
{
    // Store the parameters in our class instance for later use
    m_usPort = usPort;

    // cleanup previous password
    if ( m_szPassword )
        delete m_szPassword;

    m_szPassword = new char[strlen(szPassword) + 1];
    strcpy((char*)m_szPassword, szPassword);

    // cleanup previous host
    if ( m_szHost )
        delete m_szHost;

    m_szHost = new char[strlen(szHost) + 1];
    strcpy((char*)m_szHost, szHost);

    // Create a winsock address endpoint and parse the IP into it
    in_addr Address;
    CServerListItem::Parse(szHost, Address);

    // Set our server query's address, query port and game port
    m_Server.Address = Address;
    m_Server.usQueryPort = usPort + SERVER_LIST_QUERY_PORT_OFFSET;
    m_Server.usGamePort = usPort;

    m_bRequiresUpdate = true;

    // Lets query the server now, as the previous data is out of date
    Refresh();
}

void CServerQueue::DoPulse( void )
{
    // Are we visible?
    if ( IsVisible() )
    {
        // Pulse the server query
        m_Server.Pulse ( true );

        // Do we need a refresh?
        if ( ( m_Server.bScanned && (CClientTime::GetTime () - m_ulLastUpdateTime) >=  SERVER_UPDATE_INTERVAL ) || m_bRequiresUpdate )
        {
            m_bRequiresUpdate = false;

            // Is auto join enabled?
            if ( m_pCheckboxAutojoin->GetSelected() )
            {
                // Is a slot available?
                if ( m_Server.nPlayers < m_Server.nMaxPlayers )
                {
                    // Lets attempt to connect
                    Connect();
                    return;
                }
            }

            // Set our GUI elements to display the server information
            m_pServerNameLabel->SetText(m_Server.strName.c_str());
            m_pServerIPLabel->SetText(m_Server.strHost.c_str());
            m_pGamemodeLabel->SetText(m_Server.strType.c_str());
            m_pMapLabel->SetText(m_Server.strMap.c_str());
            m_pPlayersLabel->SetText(SString("%i/%i", m_Server.nPlayers, m_Server.nMaxPlayers).c_str());
            
            m_pPasswordedLabel->SetText(m_Server.bPassworded ? "Yes" : "No");
            m_pLatencyLabel->SetText(SString("%i", m_Server.nPing));

            // Clear our player list
            m_pServerPlayerList->Clear();

            // Iterate the list of players and add their names to the player list
            for ( unsigned int i = 0; i < m_Server.vecPlayers.size (); i++ ) 
            {
                std::string strPlayerName = m_Server.vecPlayers[i].c_str ();

                m_pServerPlayerList->AddRow(true);
                m_pServerPlayerList->SetItemText(i, m_hPlayerName, strPlayerName.c_str(), false, false, true);
            }


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

            m_bRequiresUpdate = false;

            // Set the text of the GUI elements to the previous data (or blank)
            m_pServerNameLabel->SetText(m_Server.strName.c_str());
            m_pServerIPLabel->SetText(m_Server.strHost.c_str());
            m_pGamemodeLabel->SetText(m_Server.strType.c_str());
            m_pMapLabel->SetText(m_Server.strMap.c_str());
            m_pPlayersLabel->SetText(SString("%i/%i", m_Server.nPlayers, m_Server.nMaxPlayers).c_str());
            m_pPasswordedLabel->SetText(m_Server.bPassworded ? "Yes" : "No");

            // The server has timed out
            m_pLatencyLabel->SetText("Timed Out");

            // Clear the player list
            m_pServerPlayerList->Clear();

            m_ulLastUpdateTime = CClientTime::GetTime ();
        }
    }
}

void CServerQueue::Refresh( void )
{
    // Reset the query variables
    m_Server.Init();

    // Our last update time is now
    m_ulLastUpdateTime = CClientTime::GetTime ();

    // Lets query the server
    m_Server.Query();
}

void CServerQueue::Reset ( void )
{
    // Set every GUI elements text to blank
    m_pServerNameLabel->SetText("");
    m_pServerIPLabel->SetText("");
    m_pGamemodeLabel->SetText("");
    m_pMapLabel->SetText("");
    m_pPlayersLabel->SetText("");
    m_pPasswordedLabel->SetText("");
    m_pLatencyLabel->SetText("");
}

void CServerQueue::Connect( void )
{
    // Get the nick from the cfg
    std::string strNick;
    CVARS_GET ( "nick", strNick );

    // Hide the window
    SetVisible( false );

    // Let's attempt to join
    CCore::GetSingleton ().GetConnectManager ()->Connect ( m_szHost, m_usPort, strNick.c_str (), m_szPassword );
}
