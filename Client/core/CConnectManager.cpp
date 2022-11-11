/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CConnectManager.cpp
 *  PURPOSE:     Manager for connecting to servers
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "net/packetenums.h"
using namespace std;

static CConnectManager* g_pConnectManager = NULL;
extern CCore*           g_pCore;

CConnectManager::CConnectManager()
{
    g_pConnectManager = this;

    m_Address.s_addr = 0;
    m_usPort = 0;
    m_bReconnect = false;
    m_bIsDetectingVersion = false;
    m_bIsConnecting = false;
    m_bSave = true;
    m_tConnectStarted = 0;

    m_pOnCancelClick = new GUI_CALLBACK(&CConnectManager::Event_OnCancelClick, this);

    m_pServerItem = NULL;
    m_bNotifyServerBrowser = false;
}

CConnectManager::~CConnectManager()
{
    if (m_pOnCancelClick)
    {
        delete m_pOnCancelClick;
        m_pOnCancelClick = NULL;
    }

    SAFE_DELETE(m_pServerItem);
    g_pConnectManager = NULL;
}

bool CConnectManager::Connect(const char* szHost, unsigned short usPort, const char* szNick, const char* szPassword, bool bNotifyServerBrowser)
{
    assert(szHost);
    assert(szNick);
    assert(szPassword);

    m_bNotifyServerBrowser = bNotifyServerBrowser;

    // For detecting startup problems
    WatchDogUserDidInteractWithMenu();

    // Hide certain questions
    CCore::GetSingleton().GetLocalGUI()->GetMainMenu()->GetQuestionWindow()->OnConnect();

    // Hide the server queue
    CServerInfo::GetSingletonPtr()->Hide();

    // Save the browser state
    CServerBrowser::GetSingletonPtr()->SaveOptions();

    // Are we already connecting?
    CNet* pNet = CCore::GetSingleton().GetNetwork();
    if (m_bIsConnecting || pNet->IsConnected())
    {
        CModManager::GetSingleton().Unload();
    }

    // Is the nick valid?
    if (!CheckNickProvided((char*)szNick))
    {
        SString strBuffer = _("Connecting failed. Invalid nick provided!");
        CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC20"), strBuffer, MB_BUTTON_OK | MB_ICON_ERROR);            // Invalid nick provided
        return false;
    }

    // Save the nick too
    CVARS_SET("nick", std::string(szNick));

    // Reset fake lag
    CCore::GetSingleton().SetFakeLagCommandEnabled(false);
    pNet->SetFakeLag(0, 0, 0, 0);

    // Reset the network
    pNet->Reset();
    assert(pNet->GetServerBitStreamVersion() == 0);

    // Save input
    m_strHost = szHost;
    m_strNick = szNick;
    m_strPassword = szPassword;
    m_Address.s_addr = 0;
    m_usPort = usPort;
    m_bSave = true;

    m_strLastHost = m_strHost;
    m_usLastPort = m_usPort;
    m_strLastPassword = m_strPassword;

    // Parse host into a server item
    if (!CServerListItem::Parse(m_strHost.c_str(), m_Address))
    {
        SString strBuffer = _("Connecting failed. Invalid host provided!");
        CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC21"), strBuffer, MB_BUTTON_OK | MB_ICON_ERROR);            // Invalid host provided
        return false;
    }

    // No connect if disk space is low
    if (!CCore::GetSingleton().CheckDiskSpace())
        return false;

    // Set our packet handler
    pNet->RegisterPacketHandler(CConnectManager::StaticProcessPacket);

    // Try to start a network to connect
    SString strAddress = inet_ntoa(m_Address);
    if (m_usPort && !pNet->StartNetwork(strAddress, m_usPort, CVARS_GET_VALUE<bool>("packet_tag")))
    {
        SString strBuffer(_("Connecting to %s at port %u failed!"), m_strHost.c_str(), m_usPort);
        CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC22"), strBuffer, MB_BUTTON_OK | MB_ICON_ERROR);            // Failed to connect
        return false;
    }

    m_bIsConnecting = true;
    m_tConnectStarted = time(NULL);
    m_bHasTriedSecondConnect = false;

    // Load server password
    if (m_strPassword.empty())
        m_strPassword = CServerBrowser::GetSingletonPtr()->GetServerPassword(m_strHost + ":" + SString("%u", m_usPort));

    // Start server version detection
    SAFE_DELETE(m_pServerItem);
    m_pServerItem = new CServerListItem(m_Address, m_usPort);
    m_pServerItem->m_iTimeoutLength = 2000;
    m_bIsDetectingVersion = true;
    OpenServerFirewall(m_Address, CServerBrowser::GetSingletonPtr()->FindServerHttpPort(m_strHost, m_usPort), true);

    // Display the status box
    SString strBuffer(_("Connecting to %s:%u ..."), m_strHost.c_str(), m_usPort);
    if (m_bReconnect)
    {
        strBuffer = SString(_("Reconnecting to %s:%u ..."), m_strHost.c_str(), m_usPort);
    }
    CCore::GetSingleton().ShowMessageBox(_("CONNECTING"), strBuffer, MB_BUTTON_CANCEL | MB_ICON_INFO, m_pOnCancelClick);
    WriteDebugEvent(SString("Connecting to %s:%u ...", m_strHost.c_str(), m_usPort));

    return true;
}

bool CConnectManager::Reconnect(const char* szHost, unsigned short usPort, const char* szPassword, bool bSave)
{
    // Use previous connection datum when function arguments are not set
    unsigned int uiPort = 0;
    CVARS_GET("host", m_strHost);
    CVARS_GET("port", uiPort);
    m_usPort = uiPort;

    // If keeping the same host & port, retrieve the password as well
    if (!szHost || !szHost[0] || m_strHost == szHost)
        if (usPort == 0 || m_usPort == usPort)
            CVARS_GET("password", m_strPassword);

    // Allocate a new host and nick buffer and store the strings in them
    if (szHost && szHost[0])
    {
        m_strHost = szHost;
    }
    if (szPassword && szPassword[0])
    {
        m_strPassword = szPassword;
    }

    if (usPort)
    {
        m_usPort = usPort;
    }

    m_bSave = bSave;

    m_bReconnect = true;

    return true;
}

bool CConnectManager::Event_OnCancelClick(CGUIElement* pElement)
{
    // The user clicked cancel, so abort connecting
    Abort();
    // Remove the message box next frame
    g_pCore->RemoveMessageBox(true);

    return true;
}

bool CConnectManager::Abort()
{
    // Stop the attempt
    CNet* pNet = CCore::GetSingleton().GetNetwork();
    pNet->StopNetwork();
    pNet->Reset();

    // Reset our variables
    m_strHost = "";
    m_strNick = "";
    m_strPassword = "";

    m_Address.s_addr = 0;
    m_usPort = 0;
    m_bIsConnecting = false;
    m_bIsDetectingVersion = false;
    m_tConnectStarted = 0;
    SAFE_DELETE(m_pServerItem);

    // Success
    return true;
}

void CConnectManager::DoPulse()
{
    // Are we connecting?
    if (m_bIsConnecting)
    {
        // Are we also getting the server version?
        if (m_bIsDetectingVersion)
        {
            m_pServerItem->Pulse(true);
            // Got some sort of result?
            if (m_pServerItem->bSkipped || m_pServerItem->bScanned)
            {
                OnServerExists();

                m_bIsDetectingVersion = false;
                // Is different version?
                if (m_pServerItem->bScanned && m_pServerItem->strVersion != MTA_DM_ASE_VERSION)
                {
                    // Version mis-match. See about launching compatible .exe
                    GetVersionUpdater()->InitiateSidegradeLaunch(m_pServerItem->strVersion, m_strHost.c_str(), m_usPort, m_strNick.c_str(),
                                                                 m_strPassword.c_str());
                    Abort();
                    return;
                }
            }
        }

        int iConnectTimeDelta = time(NULL) - m_tConnectStarted;

        // Try connect again if no response after 4 seconds
        if (iConnectTimeDelta >= 4 && !m_bHasTriedSecondConnect && g_pCore->GetNetwork()->GetExtendedErrorCode() == 0)
        {
            m_bHasTriedSecondConnect = true;
            SString strAddress = inet_ntoa(m_Address);
            g_pCore->GetNetwork()->StartNetwork(strAddress, m_usPort, CVARS_GET_VALUE<bool>("packet_tag"));
        }

        // Time to timeout the connection?
        if (iConnectTimeDelta >= 8)
        {
            // Show a message that the connection timed out and abort
            g_pCore->ShowNetErrorMessageBox(_("Error") + _E("CC23"), _("Connection timed out"), "connect-timed-out", true);
            Abort();
        }
        else
        {
            // Some error?
            unsigned char ucError = CCore::GetSingleton().GetNetwork()->GetConnectionError();
            if (ucError != 0)
            {
                SString strError;
                SString strErrorCode;
                switch (ucError)
                {
                    case RID_RSA_PUBLIC_KEY_MISMATCH:
                        strError = _("Disconnected: unknown protocol error");
                        strErrorCode = _E("CC24");            // encryption key mismatch
                        break;
                    case RID_INCOMPATIBLE_PROTOCOL_VERSION:
                        strError = _("Disconnected: unknown protocol error");
                        strErrorCode = _E("CC34");            // old raknet version
                        break;
                    case RID_REMOTE_DISCONNECTION_NOTIFICATION:
                        strError = _("Disconnected: disconnected remotely");
                        strErrorCode = _E("CC25");
                        break;
                    case RID_REMOTE_CONNECTION_LOST:
                        strError = _("Disconnected: connection lost remotely");
                        strErrorCode = _E("CC26");
                        break;
                    case RID_CONNECTION_BANNED:
                        strError = _("Disconnected: you are banned from this server");
                        strErrorCode = _E("CC27");
                        break;
                    case RID_NO_FREE_INCOMING_CONNECTIONS:
                        CServerInfo::GetSingletonPtr()->Show(eWindowTypes::SERVER_INFO_QUEUE, m_strHost.c_str(), m_usPort, m_strPassword.c_str());
                        break;
                    case RID_DISCONNECTION_NOTIFICATION:
                        strError = _("Disconnected: disconnected from the server");
                        strErrorCode = _E("CC28");
                        break;
                    case RID_CONNECTION_LOST:
                        strError = _("Disconnected: connection to the server was lost");
                        strErrorCode = _E("CC29");
                        break;
                    case RID_INVALID_PASSWORD:
                        CServerInfo::GetSingletonPtr()->Show(eWindowTypes::SERVER_INFO_PASSWORD, m_strHost.c_str(), m_usPort, m_strPassword.c_str());
                        break;
                    default:
                        strError = _("Disconnected: connection was refused");
                        strErrorCode = _E("CC30");
                        break;
                }

                // Display an error, reset the error status and exit

                // Only display the error if we set one
                if (strError.length() > 0)
                {
                    CCore::GetSingleton().ShowNetErrorMessageBox(_("Error") + strErrorCode, strError);
                }
                else            // Otherwise, remove the message box and hide quick connect
                {
                    CCore::GetSingleton().RemoveMessageBox(false);
                }

                CCore::GetSingleton().GetNetwork()->SetConnectionError(0);
                Abort();
            }
        }

        // Pulse the network interface
        CCore::GetSingleton().GetNetwork()->DoPulse();
    }
    else if (m_bReconnect)
    {
        std::string strNick;
        CVARS_GET("nick", strNick);
        Connect(m_strHost.c_str(), m_usPort, strNick.c_str(), m_strPassword.c_str(), false);
        m_bReconnect = false;
    }
}

bool CConnectManager::StaticProcessPacket(unsigned char ucPacketID, NetBitStreamInterface& BitStream)
{
    // We're working on connecting?
    if (g_pConnectManager->m_bIsConnecting)
    {
        g_pConnectManager->OnServerExists();

        // The packet we're expecting?
        if (ucPacketID == PACKET_ID_MOD_NAME)
        {
            // Read packet data
            unsigned short usServerBitStreamVersion = 0x01;
            BitStream.Read(usServerBitStreamVersion);

            SString strModName;
            BitStream.ReadString(strModName);

            // Process packet data
            CCore::GetSingleton().GetNetwork()->SetServerBitStreamVersion(usServerBitStreamVersion);

            if (strModName != "")
            {
                // Populate the arguments to pass it (-c host port nick)
                SString strArguments("%s %s", g_pConnectManager->m_strNick.c_str(), g_pConnectManager->m_strPassword.c_str());

                // Hide the messagebox we're currently showing
                CCore::GetSingleton().RemoveMessageBox();

                // Save the connection details into the config
                if (g_pConnectManager->m_bSave)
                {
                    CVARS_SET("host", g_pConnectManager->m_strHost);
                    CVARS_SET("port", g_pConnectManager->m_usPort);
                    CVARS_SET("password", g_pConnectManager->m_strPassword);
                }

                SetApplicationSettingInt("last-server-ip", g_pConnectManager->m_Address.s_addr);
                SetApplicationSettingInt("last-server-port", g_pConnectManager->m_usPort);
                SetApplicationSettingInt("last-server-time", _time32(NULL));

                // Kevuwk: Forced the config to save here so that the IP/Port isn't lost on crash
                CCore::GetSingleton().SaveConfig();

                // Reset our variables
                g_pConnectManager->m_strNick = "";
                g_pConnectManager->m_strHost = "";
                g_pConnectManager->m_strPassword = "";

                g_pConnectManager->m_Address.s_addr = 0;
                g_pConnectManager->m_usPort = 0;
                g_pConnectManager->m_bIsConnecting = false;
                g_pConnectManager->m_bIsDetectingVersion = false;
                g_pConnectManager->m_tConnectStarted = 0;

                // Load the mod
                if (!CModManager::GetSingleton().Load(strModName, strArguments))
                {
                    // Failed loading the mod
                    strArguments.Format(_("No such mod installed (%s)"), strModName.c_str());
                    CCore::GetSingleton().ShowMessageBox(_("Error") + _E("CC31"), strArguments, MB_BUTTON_OK | MB_ICON_ERROR);            // Mod loading failed
                    g_pConnectManager->Abort();
                }
            }
            else
            {
                // Show failed message and abort the attempt
                CCore::GetSingleton().ShowNetErrorMessageBox(_("Error") + _E("CC32"), _("Bad server response (2)"));
                g_pConnectManager->Abort();
            }
        }
        else
        {
            // Not the player joined packet?
            if (ucPacketID != PACKET_ID_SERVER_JOIN && ucPacketID != PACKET_ID_SERVER_JOIN_DATA)
            {
                // Show failed message and abort the attempt
                CCore::GetSingleton().ShowNetErrorMessageBox(_("Error") + _E("CC33"), _("Bad server response (1)") + SString(" [%d]", ucPacketID));
                g_pConnectManager->Abort();
            }
        }

        return true;
    }

    return false;
}

bool CConnectManager::CheckNickProvided(const char* szNick)
{
    if (stricmp(szNick, "admin") == 0)
        return false;
    if (stricmp(szNick, "console") == 0)
        return false;
    if (stricmp(szNick, "server") == 0)
        return false;
    return true;
}

//
// Called at least once (maybe more) if a MTA server exists at the current address/port
//
void CConnectManager::OnServerExists()
{
    if (m_bNotifyServerBrowser)
    {
        m_bNotifyServerBrowser = false;
        CServerBrowser::GetSingletonPtr()->NotifyServerExists(m_Address, m_usPort);
    }
}

//
// Some server firewalls block UDP packets unless a TCP connection has previously been established
//
void CConnectManager::OpenServerFirewall(in_addr Address, ushort usHttpPort, bool bHighPriority)
{
    uint uiTimeOut;
    if (bHighPriority)
    {
        // Clear previously queued requests if this is high priority
        g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadMode::CONNECT_TCP_SEND)->Reset();
        g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadMode::CONNECT_TCP_SEND)->SetMaxConnections(2);
        uiTimeOut = 2000;
    }
    else
    {
        uiTimeOut = 1000;
    }

    if (usHttpPort)
    {
        // Send to server http port if known
        SHttpRequestOptions options;
        options.uiConnectionAttempts = 1;
        options.uiConnectTimeoutMs = uiTimeOut;
        SString strDummyUrl("http://%s:%d/mta_client_firewall_probe/", inet_ntoa(Address), usHttpPort);
        g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadMode::CONNECT_TCP_SEND)->QueueFile(strDummyUrl, NULL, NULL, NULL, options);
    }
    if (usHttpPort == 0 || bHighPriority)
    {
        // Send to standard http port
        SHttpRequestOptions options;
        options.uiConnectionAttempts = 1;
        options.uiConnectTimeoutMs = uiTimeOut;
        SString strDummyUrl("http://%s/mta_client_firewall_probe/", inet_ntoa(Address));
        g_pCore->GetNetwork()->GetHTTPDownloadManager(EDownloadMode::CONNECT_TCP_SEND)->QueueFile(strDummyUrl, NULL, NULL, NULL, options);
    }
}
