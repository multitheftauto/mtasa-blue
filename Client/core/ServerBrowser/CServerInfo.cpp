/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CServerInfo.cpp
 *  PURPOSE:     In-game server queue user interface
 *
 *  Multi Theft Auto is available from https://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

using namespace std;

extern CCore* g_pCore;

template <>
CServerInfo* CSingleton<CServerInfo>::m_pSingleton = NULL;

CServerInfo::CServerInfo()
{
    CGUI* pManager = g_pCore->GetGUI();

    // Initialize
    m_ulLastUpdateTime = CClientTime::GetTime();

    // Obtain our screen resolution
    CVector2D vecResolution = CCore::GetSingleton().GetGUI()->GetResolution();

    // Create queue window
    m_pWindow = reinterpret_cast<CGUIWindow*>(pManager->CreateWnd(NULL, _("SERVER IS FULL")));
    m_pWindow->SetCloseButtonEnabled(false);
    m_pWindow->SetMovable(true);
    m_pWindow->SetSizingEnabled(false);
    m_pWindow->SetPosition(CVector2D(vecResolution.fX / 2 - INFO_WINDOW_DEFAULTWIDTH / 2, vecResolution.fY / 2 - INFO_WINDOW_DEFAULTHEIGHT / 2), false);
    m_pWindow->SetSize(CVector2D(INFO_WINDOW_DEFAULTWIDTH, INFO_WINDOW_DEFAULTHEIGHT));
    m_pWindow->BringToFront();
    m_pWindow->SetZOrderingEnabled(false);
    m_pWindow->SetAlwaysOnTop(true);
    m_pWindow->SetMinimumSize(CVector2D(INFO_WINDOW_DEFAULTWIDTH, INFO_WINDOW_DEFAULTHEIGHT));

    // Determine our label draw position for L10n
    unsigned int LabelTitleSizeX = pManager->CGUI_GetMaxTextExtent("default-bold-small", _("Name:"), _("Server Address:"), _("Gamemode:"), _("Map:"),
                                                                   _("Players:"), _("Passworded:"), _("Latency:")) +
                                   10;
    unsigned int LabelTitlePosX = 9;
    unsigned int LabelPosX = LabelTitlePosX + LabelTitleSizeX + 2;
    unsigned int LabelSizeX = INFO_WINDOW_DEFAULTWIDTH;
    unsigned int LabelSizeY = 15;
    unsigned int DrawPosY = 10;  // Start position
    // Server Name
    m_pServerNameLabelTitle = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Name:")));
    m_pServerNameLabelTitle->SetPosition(CVector2D(LabelTitlePosX, DrawPosY += INFO_LABEL_VSPACING + LabelSizeY), false);
    m_pServerNameLabelTitle->SetFont("default-bold-small");
    m_pServerNameLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pServerNameLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pServerNameLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, ""));
    m_pServerNameLabel->SetPosition(CVector2D(LabelPosX, DrawPosY), false);
    m_pServerNameLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Server IP
    m_pServerAddressLabelTitle = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Server Address:")));
    m_pServerAddressLabelTitle->SetPosition(CVector2D(LabelTitlePosX, DrawPosY += INFO_LABEL_VSPACING + LabelSizeY), false);
    m_pServerAddressLabelTitle->SetFont("default-bold-small");
    m_pServerAddressLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pServerAddressLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pServerAddressLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, ""));
    m_pServerAddressLabel->SetPosition(CVector2D(LabelPosX, DrawPosY), false);
    m_pServerAddressLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Gamemode
    m_pGamemodeLabelTitle = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Gamemode:")));
    m_pGamemodeLabelTitle->SetPosition(CVector2D(LabelTitlePosX, DrawPosY += INFO_LABEL_VSPACING + LabelSizeY), false);
    m_pGamemodeLabelTitle->SetFont("default-bold-small");
    m_pGamemodeLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pGamemodeLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pGamemodeLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, ""));
    m_pGamemodeLabel->SetPosition(CVector2D(LabelPosX, DrawPosY), false);
    m_pGamemodeLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Map
    m_pMapLabelTitle = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Map:")));
    m_pMapLabelTitle->SetPosition(CVector2D(LabelTitlePosX, DrawPosY += INFO_LABEL_VSPACING + LabelSizeY), false);
    m_pMapLabelTitle->SetFont("default-bold-small");
    m_pMapLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pMapLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pMapLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, ""));
    m_pMapLabel->SetPosition(CVector2D(LabelPosX, DrawPosY), false);
    m_pMapLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Players
    m_pPlayersLabelTitle = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Players:")));
    m_pPlayersLabelTitle->SetPosition(CVector2D(LabelTitlePosX, DrawPosY += INFO_LABEL_VSPACING + LabelSizeY), false);
    m_pPlayersLabelTitle->SetFont("default-bold-small");
    m_pPlayersLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pPlayersLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pPlayersLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, ""));
    m_pPlayersLabel->SetPosition(CVector2D(LabelPosX, DrawPosY), false);
    m_pPlayersLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Passworded
    m_pPasswordedLabelTitle = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Passworded:")));
    m_pPasswordedLabelTitle->SetPosition(CVector2D(LabelTitlePosX, DrawPosY += INFO_LABEL_VSPACING + LabelSizeY), false);
    m_pPasswordedLabelTitle->SetFont("default-bold-small");
    m_pPasswordedLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pPasswordedLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pPasswordedLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, ""));
    m_pPasswordedLabel->SetPosition(CVector2D(LabelPosX, DrawPosY), false);
    m_pPasswordedLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Latency
    m_pLatencyLabelTitle = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Latency:")));
    m_pLatencyLabelTitle->SetPosition(CVector2D(LabelTitlePosX, DrawPosY += INFO_LABEL_VSPACING + LabelSizeY), false);
    m_pLatencyLabelTitle->SetFont("default-bold-small");
    m_pLatencyLabelTitle->SetSize(CVector2D(LabelTitleSizeX, LabelSizeY), false);
    m_pLatencyLabelTitle->SetHorizontalAlign(CGUI_ALIGN_RIGHT);

    m_pLatencyLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, ""));
    m_pLatencyLabel->SetPosition(CVector2D(LabelPosX, DrawPosY), false);
    m_pLatencyLabel->SetSize(CVector2D(LabelSizeX, LabelSizeY), false);

    // Player list
    m_pServerPlayerList = reinterpret_cast<CGUIGridList*>(pManager->CreateGridList(m_pWindow));
    m_pServerPlayerList->SetPosition(CVector2D(INFO_WINDOW_HSPACING * 2, DrawPosY += (INFO_WINDOW_VSPACING + LabelSizeY)), false);
    unsigned int PlayerListHeight =
        (INFO_WINDOW_DEFAULTHEIGHT - INFO_WINDOW_VSPACING - INFO_BUTTON_HEIGHT - INFO_WINDOW_VSPACING - LabelSizeY - INFO_WINDOW_VSPACING) - DrawPosY;
    m_pServerPlayerList->SetSize(CVector2D(INFO_WINDOW_DEFAULTWIDTH - INFO_WINDOW_HSPACING * 4, PlayerListHeight), false);
    m_pServerPlayerList->SetIgnoreTextSpacer(true);

    // Column for player names
    m_hPlayerName = m_pServerPlayerList->AddColumn(_("Player list"), 0.9f);

    // Now we draw from the bottom
    DrawPosY = INFO_WINDOW_DEFAULTHEIGHT - INFO_WINDOW_VSPACING;

    // Close button
    m_pButtonClose = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, _("Close")));
    m_pButtonClose->SetPosition(CVector2D(INFO_WINDOW_DEFAULTWIDTH - (INFO_BUTTON_WIDTH * 2) - (1.5f * INFO_WINDOW_HSPACING), DrawPosY -= INFO_BUTTON_HEIGHT),
                                false);
    m_pButtonClose->SetSize(CVector2D(INFO_BUTTON_WIDTH, INFO_BUTTON_HEIGHT), false);
    m_pButtonClose->SetClickHandler(GUI_CALLBACK(&CServerInfo::OnCloseClicked, this));
    m_pButtonClose->SetZOrderingEnabled(false);

    // Join Game button
    m_pButtonJoinGame = reinterpret_cast<CGUIButton*>(pManager->CreateButton(m_pWindow, _("Join Game")));
    m_pButtonJoinGame->SetPosition(CVector2D(INFO_WINDOW_DEFAULTWIDTH - INFO_BUTTON_WIDTH - INFO_WINDOW_HSPACING, DrawPosY), false);
    m_pButtonJoinGame->SetSize(CVector2D(INFO_BUTTON_WIDTH, INFO_BUTTON_HEIGHT), false);
    m_pButtonJoinGame->SetClickHandler(GUI_CALLBACK(&CServerInfo::OnJoinGameClicked, this));
    m_pButtonJoinGame->SetZOrderingEnabled(false);

    // Password entry editbox
    m_pEnterPasswordEdit = reinterpret_cast<CGUIEdit*>(pManager->CreateEdit(m_pWindow, ""));
    m_pEnterPasswordEdit->SetPosition(CVector2D(INFO_WINDOW_HSPACING * 2, DrawPosY - INFO_WINDOW_VSPACING - INFO_BUTTON_HEIGHT), false);
    m_pEnterPasswordEdit->SetSize(CVector2D(INFO_BUTTON_WIDTH * 2, INFO_BUTTON_HEIGHT), false);
    m_pEnterPasswordEdit->SetMasked(true);
    m_pEnterPasswordEdit->SetTextAcceptedHandler(GUI_CALLBACK(&CServerInfo::OnJoinGameClicked, this));

    // Please enter password label
    m_pEnterPasswordLabel = reinterpret_cast<CGUILabel*>(pManager->CreateLabel(m_pWindow, _("Please enter the password to the server:")));
    m_pEnterPasswordLabel->SetPosition(CVector2D(0, DrawPosY - INFO_WINDOW_VSPACING - INFO_BUTTON_HEIGHT - INFO_WINDOW_VSPACING - LabelSizeY * 2), false);
    m_pEnterPasswordLabel->SetFont("default-bold-small");
    m_pEnterPasswordLabel->SetSize(CVector2D(INFO_WINDOW_DEFAULTWIDTH, LabelSizeY * 2), false);
    m_pEnterPasswordLabel->SetHorizontalAlign(CGUI_ALIGN_HORIZONTALCENTER_WORDWRAP);
    m_pEnterPasswordLabel->SetVerticalAlign(CGUI_ALIGN_VERTICALCENTER);
    m_pEnterPasswordLabel->SetTextColor(255, 0, 0);

    // Autojoin checkbox
    // TRANSLATORS:  If you need more room, there is space for one extra line using a newline (\n).
    m_pCheckboxAutojoin =
        reinterpret_cast<CGUICheckBox*>(pManager->CreateCheckBox(m_pWindow, _("Join the server as soon as a player slot is available."), true));
    m_pCheckboxAutojoin->AutoSize(m_pCheckboxAutojoin->GetText().c_str(), 20.0f, LabelSizeY);
    m_pCheckboxAutojoin->SetPosition(
        CVector2D((INFO_WINDOW_DEFAULTWIDTH - m_pCheckboxAutojoin->GetSize().fX) / 2, DrawPosY - INFO_WINDOW_VSPACING - LabelSizeY * 2),
        false);  // Horizontally center align
}

CServerInfo::~CServerInfo()
{
    // Delete our GUI elements
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

    delete m_pWindow;
    m_pWindow = 0;
}

bool CServerInfo::OnCloseClicked(CGUIElement* pElement)
{
    // Hide this window
    m_pWindow->SetVisible(false);
    return true;
}

bool CServerInfo::OnJoinGameClicked(CGUIElement* pElement)
{
    // Attempt to connect to the server
    Connect();
    return true;
}

bool CServerInfo::IsVisible()
{
    return m_pWindow->IsVisible();
}

void CServerInfo::Hide()
{
    m_pWindow->SetVisible(false);
}

void CServerInfo::Show(eWindowType WindowType)
{
    Show(WindowType, g_pCore->GetConnectManager()->m_strLastHost.c_str(), g_pCore->GetConnectManager()->m_usLastPort, "");
}

void CServerInfo::Show(eWindowType WindowType, const char* szHost, unsigned short usPort, const char* szPassword, CServerListItem* pInitialServerListItem)
{
    m_pWindow->SetZOrderingEnabled(true);
    m_pWindow->SetVisible(true);
    m_pWindow->BringToFront();

    m_pEnterPasswordEdit->SetText("");

    // Adjust our window according to what type of window we are displaying
    float PlayerListHeight;

    float DrawPosY = INFO_WINDOW_DEFAULTHEIGHT - INFO_WINDOW_VSPACING - INFO_BUTTON_HEIGHT;

    m_pButtonJoinGame->SetPosition(CVector2D(INFO_WINDOW_DEFAULTWIDTH - INFO_BUTTON_WIDTH - INFO_WINDOW_HSPACING, DrawPosY), false);
    m_pButtonClose->SetPosition(CVector2D(INFO_WINDOW_DEFAULTWIDTH - (INFO_BUTTON_WIDTH * 2) - (1.5f * INFO_WINDOW_HSPACING), DrawPosY), false);

    if (WindowType == eWindowTypes::SERVER_INFO_QUEUE)
    {
        PlayerListHeight = m_pCheckboxAutojoin->GetPosition().fY - INFO_WINDOW_VSPACING - m_pServerPlayerList->GetPosition().fY;
        m_pCheckboxAutojoin->SetVisible(true);
        m_pEnterPasswordEdit->SetVisible(false);
        m_pEnterPasswordLabel->SetVisible(false);
        m_pWindow->SetText(_("SERVER IS FULL"));
    }
    else if (WindowType == eWindowTypes::SERVER_INFO_PASSWORD)
    {
        PlayerListHeight = m_pEnterPasswordLabel->GetPosition().fY - INFO_WINDOW_VSPACING - m_pServerPlayerList->GetPosition().fY;
        m_pCheckboxAutojoin->SetVisible(false);
        m_pEnterPasswordEdit->SetVisible(true);
        m_pEnterPasswordLabel->SetVisible(true);
        m_pWindow->SetText(_("PLEASE ENTER SERVER PASSWORD"));
        m_bWaitingToActivatePassword = true;
    }
    else
    {
        PlayerListHeight = m_pButtonJoinGame->GetPosition().fY - INFO_WINDOW_VSPACING - m_pServerPlayerList->GetPosition().fY;
        m_pCheckboxAutojoin->SetVisible(false);
        m_pEnterPasswordEdit->SetVisible(false);
        m_pEnterPasswordLabel->SetVisible(false);
        m_pWindow->SetText(_("Information"));
    }

    m_pCurrentWindowType = WindowType;

    m_pServerPlayerList->SetSize(CVector2D(INFO_WINDOW_DEFAULTWIDTH - INFO_WINDOW_HSPACING * 4, PlayerListHeight), false);

    m_pWindow->SetZOrderingEnabled(false);

    pInitialServerListItem =
        pInitialServerListItem ? pInitialServerListItem : CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetServerBrowser()->FindServer(szHost, usPort);

    SetServerInformation(szHost, usPort, szPassword, pInitialServerListItem);
}

void CServerInfo::SetServerInformation(const char* szHost, unsigned short usPort, const char* szPassword, CServerListItem* pInitialServerListItem)
{
    m_usPort = usPort;
    m_strPassword = szPassword ? szPassword : "";
    m_strHost = szHost ? szHost : "";

    in_addr Address;
    memset(&Address, 0, sizeof(Address));
    if (!CServerListItem::Parse(szHost, Address))
        Address.S_un.S_addr = 0;

    m_Server.CancelPendingQuery();

    // Clear stale address data before applying the new endpoint
    m_Server.Address.S_un.S_addr = 0;
    m_Server.AddressCopy.S_un.S_addr = 0;
    m_Server.usGamePort = 0;
    m_Server.usGamePortCopy = 0;
    m_Server.strHost = "";
    m_Server.strEndpoint = "";
    m_Server.strEndpointSortKey = "";
    m_Server.bScanned = false;
    m_Server.bSkipped = false;

    // Assign the new address details
    m_Server.ChangeAddress(Address, usPort);

    if (m_Server.Address.S_un.S_addr != Address.S_un.S_addr || m_Server.usGamePort != usPort)
    {
        m_Server.Address = Address;
        m_Server.usGamePort = usPort;
    }

    m_Server.AddressCopy = m_Server.Address;
    m_Server.usGamePortCopy = m_Server.usGamePort;

    const uchar* addressBytes = reinterpret_cast<const uchar*>(&m_Server.Address.S_un.S_addr);
    SString      strAddressHost = inet_ntoa(m_Server.Address);
    if (strAddressHost.empty())
        strAddressHost = szHost;
    m_Server.strHost = strAddressHost;
    m_Server.strEndpoint = SString("%s:%u", *strAddressHost, m_Server.usGamePort);
    m_Server.strEndpointSortKey = SString("%02x%02x%02x%02x-%04x", addressBytes[0], addressBytes[1], addressBytes[2], addressBytes[3], m_Server.usGamePort);

    if (pInitialServerListItem && CServerListItem::StaticIsValid(pInitialServerListItem))
    {
        m_Server.strName = pInitialServerListItem->strName;
        m_Server.strSearchableName = pInitialServerListItem->strSearchableName;
        m_Server.strHostName = pInitialServerListItem->strHostName;
        m_Server.strGameName = pInitialServerListItem->strGameName;
        m_Server.strGameMode = pInitialServerListItem->strGameMode;
        m_Server.strMap = pInitialServerListItem->strMap;
        m_Server.strVersion = pInitialServerListItem->strVersion;
        m_Server.nPlayers = pInitialServerListItem->nPlayers;
        m_Server.nMaxPlayers = pInitialServerListItem->nMaxPlayers;
        m_Server.nPing = pInitialServerListItem->nPing;
        m_Server.bPassworded = pInitialServerListItem->bPassworded;
        m_Server.bSerials = pInitialServerListItem->bSerials;
        m_Server.isStatusVerified = pInitialServerListItem->isStatusVerified;
        m_Server.bMaybeOffline = pInitialServerListItem->bMaybeOffline;
        m_Server.bMasterServerSaysNoResponse = pInitialServerListItem->bMasterServerSaysNoResponse;
        m_Server.uiMasterServerSaysRestrictions = pInitialServerListItem->uiMasterServerSaysRestrictions;
        m_Server.vecPlayers = pInitialServerListItem->vecPlayers;
        m_Server.uiTieBreakPosition = pInitialServerListItem->uiTieBreakPosition;
        m_Server.strTieBreakSortKey = pInitialServerListItem->strTieBreakSortKey;
        m_Server.strNameSortKey = pInitialServerListItem->strNameSortKey;
        m_Server.strVersionSortKey = pInitialServerListItem->strVersionSortKey;
        m_Server.m_iBuildType = pInitialServerListItem->m_iBuildType;
        m_Server.m_iBuildNumber = pInitialServerListItem->m_iBuildNumber;
        m_Server.m_usHttpPort = pInitialServerListItem->m_usHttpPort;
        m_Server.m_ucSpecialFlags = pInitialServerListItem->m_ucSpecialFlags;
        m_Server.SetDataQuality(pInitialServerListItem->GetDataQuality());
        m_Server.bScanned = true;
        m_Server.bSkipped = false;

        if (!pInitialServerListItem->strHost.empty())
            m_Server.strHost = pInitialServerListItem->strHost;
        if (!pInitialServerListItem->strEndpoint.empty())
            m_Server.strEndpoint = pInitialServerListItem->strEndpoint;
        if (!pInitialServerListItem->strEndpointSortKey.empty())
            m_Server.strEndpointSortKey = pInitialServerListItem->strEndpointSortKey;

        ResetServerGUI(&m_Server);
    }
    else
    {
        Reset();
    }

    const SString strDisplayedEndpoint = !m_Server.strEndpoint.empty() ? m_Server.strEndpoint : SString("%s:%u", szHost, usPort);
    m_pServerAddressLabel->SetText(strDisplayedEndpoint);

    // Start a fresh query so we get the latest data for this server
    Refresh();
}

void CServerInfo::DoPulse()
{
    if (!IsVisible())
    {
        if (m_bWaitingToActivatePassword)
        {
            m_bWaitingToActivatePassword = false;
            m_pWindow->Activate();
            m_pEnterPasswordEdit->Activate();
        }
        return;
    }

    const unsigned long ulNow = CClientTime::GetTime();
    bool                bRefreshed = false;

    m_Server.Pulse(true);

    in_addr expectedAddress;
    memset(&expectedAddress, 0, sizeof(expectedAddress));
    const bool bHasExpectedAddress = !m_strHost.empty() && CServerListItem::Parse(m_strHost.c_str(), expectedAddress);
    const bool bAddressMatches = !bHasExpectedAddress || (expectedAddress.S_un.S_addr == m_Server.Address.S_un.S_addr && m_usPort == m_Server.usGamePort);

    if (m_Server.bScanned)
    {
        if (bAddressMatches)
        {
            ResetServerGUI(&m_Server);

            if (m_pCurrentWindowType == eWindowTypes::SERVER_INFO_QUEUE && m_pCheckboxAutojoin->GetSelected() && m_Server.nPlayers < m_Server.nMaxPlayers)
            {
                Connect();
                return;
            }

            m_ulLastUpdateTime = ulNow;
        }
        else
        {
            Refresh();
            bRefreshed = true;
            m_ulLastUpdateTime = ulNow;
        }
    }
    else if (m_Server.bSkipped)
    {
        ResetServerGUI(&m_Server);
        m_pLatencyLabel->SetText(_("Timed Out"));
        m_ulLastUpdateTime = ulNow;

        Refresh();
        bRefreshed = true;
    }

    if (!bRefreshed && (ulNow - m_ulLastUpdateTime) >= SERVER_UPDATE_INTERVAL)
    {
        Refresh();
        m_ulLastUpdateTime = ulNow;
    }

    if (m_bWaitingToActivatePassword)
    {
        m_bWaitingToActivatePassword = false;
        m_pWindow->Activate();
        m_pEnterPasswordEdit->Activate();
    }
}

void CServerInfo::Refresh()
{
    // Reset the query variables
    m_Server.Init();

    // Our last update time is now
    m_ulLastUpdateTime = CClientTime::GetTime();

    // Lets query the server
    m_Server.Query();
}

void CServerInfo::Reset()
{
    // Set every GUI elements text to blank
    m_pServerNameLabel->SetText(_("Querying..."));
    m_pGamemodeLabel->SetText("");
    m_pMapLabel->SetText("");
    m_pPlayersLabel->SetText("");
    m_pPasswordedLabel->SetText("");
    m_pLatencyLabel->SetText("");
    m_pServerPlayerList->Clear();

    m_Server.Init();
}

void CServerInfo::Connect()
{
    // Get the nick from the cfg
    std::string strNick;
    CVARS_GET("nick", strNick);

    // Hide the window
    m_pWindow->SetVisible(false);

    std::string strPassword = m_strPassword;
    if (m_pCurrentWindowType == eWindowTypes::SERVER_INFO_PASSWORD)
        strPassword = m_pEnterPasswordEdit->GetText();
    else if (m_Server.bPassworded)  // If we're not in a passworded window, but the server is passworded
    {
        // Try to grab a saved password
        strPassword = g_pCore->GetLocalGUI()->GetMainMenu()->GetServerBrowser()->GetServerPassword(m_Server.GetEndpoint().c_str());

        if (strPassword.empty())  // No password could be found, repopup the window in password mode.
        {
            Show(eWindowTypes::SERVER_INFO_PASSWORD, m_Server.strHost.c_str(), m_Server.usGamePort, "", &m_Server);
            return;
        }
    }

    // Let's attempt to join
    CCore::GetSingleton().GetConnectManager()->Connect(m_strHost, m_usPort, strNick.c_str(), strPassword.c_str());
}

void CServerInfo::ResetServerGUI(CServerListItem* pServer)
{
    const SString strVerified = pServer->isStatusVerified ? "" : "*";

    // Set our GUI elements to display the server information
    m_pServerNameLabel->SetText(pServer->strName.c_str());
    m_pServerAddressLabel->SetText(pServer->strEndpoint.c_str());
    m_pGamemodeLabel->SetText(pServer->strGameMode.c_str());
    m_pMapLabel->SetText(pServer->strMap.c_str());
    m_pPlayersLabel->SetText(SString("%d / %d %s", pServer->nPlayers, pServer->nMaxPlayers, *strVerified).c_str());

    m_pPasswordedLabel->SetText(pServer->bPassworded ? _("Yes") : _("No"));
    m_pLatencyLabel->SetText(SString("%i", pServer->nPing));

    // Clear our player list
    m_pServerPlayerList->Clear();

    // Iterate the list of players and add their names to the player list
    for (unsigned int i = 0; i < pServer->vecPlayers.size(); i++)
    {
        std::string strPlayerName = pServer->vecPlayers[i].c_str();

        m_pServerPlayerList->AddRow(true);
        m_pServerPlayerList->SetItemText(i, m_hPlayerName, strPlayerName.c_str(), false, false, true);
    }
}
