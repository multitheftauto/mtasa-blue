/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        Client/core/CDiscordManager.cpp
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "SharedUtil.Thread.h"
#include "CDiscordManager.h"
#include <discord_register.h>
#include <discord_rpc.h>

#ifndef DISCORD_CLIENT_ID
    #define DISCORD_CLIENT_ID "468493322583801867"
#endif

CDiscordManager::~CDiscordManager()
{
    m_Suicide = true;
    int iTries = 0;
    while (iTries++ < 400 && m_Suicide)            // Wait maximum of 2 sec on this
        Sleep(5);

    if (m_Suicide)
        m_Thread->Cancel();            // Kill it anyway

    delete m_Thread;
    Discord_Shutdown();
}

void CDiscordManager::Initialize()
{
    if (m_Initialized) return;

    Reconnect(true);            // Try to interact with discord on construction
    m_Thread = new CThreadHandle(CDiscordManager::DiscordThread, this);

    m_Initialized = true;
}

// Establish connection with discord
void CDiscordManager::Reconnect(bool bOnInitialization)
{
    bool discordRichPresence;
    CVARS_GET("discord_rich_presence", discordRichPresence);
    if (!discordRichPresence)
        return;            // Disabled

    DiscordEventHandlers handlers;
    handlers.ready = HandleDiscordReady;
    handlers.disconnected = HandleDiscordDisconnected;
    handlers.errored = HandleDiscordError;
    handlers.joinGame = HandleDiscordJoin;
    handlers.spectateGame = HandleDiscordSpectate;
    handlers.joinRequest = HandleDiscordJoinRequest;

    Discord_Initialize(DISCORD_CLIENT_ID, &handlers, 0, NULL);
    Discord_Register(DISCORD_CLIENT_ID, SString("%s /from_discord", *GetParentProcessPathFilename(GetCurrentProcessId())));
    m_Connected = true;

    if (!bOnInitialization)            // Player could be in a server or in menu by now
    {
        if (g_pCore->IsConnected())
            Restore();
        else
            g_pCore->ResetDiscordRichPresence();
    }
}

void CDiscordManager::HandleDiscordError(int errcode, const char* message)
{
    WriteDebugEvent(SString("[DISCORD-ERROR #%i]: %s", errcode, message));
}

void CDiscordManager::HandleDiscordJoin(const char* joinSecret)
{
    if (!joinSecret)
        return;

    // Parse it
    SString ipport;
    SString secret = SStringX(joinSecret).SplitRight("-", &ipport);
    ushort  port;

    if (!ipport.length() || !secret.length())
        return;

    port = std::stoi(ipport.SplitRight(":", &ipport));

    // Verify ip & port (ip still could be invalid)
    if (!ipport.length() || port < 1)
        return;

    // Check if already connected to that server
    if (g_pCore->IsConnected())
    {
        SString      ipaddr;
        unsigned int port2;
        CVARS_GET("host", ipaddr);
        CVARS_GET("port", port2);

        if (ipaddr == ipport && port == port2)
        {
            CClientBase* mod = CModManager::GetSingleton().GetCurrentMod();
            if (mod)
            {
                mod->TriggerDiscordJoin(secret);

                // We don't want to proceed
                return;
            }
        }
    }

    SString nick;
    CVARS_GET("nick", nick);

    // Unload any mod before connecting to a server
    CModManager::GetSingleton().Unload();

    // Only connect if there is no mod loaded
    if (!CModManager::GetSingleton().GetCurrentMod())
    {
        // Start the connect
        if (CCore::GetSingleton().GetConnectManager()->Connect(ipport, port, nick, "", false, secret))
            CCore::GetSingleton().GetConsole()->Printf(_("Discord Join: Connecting to %s:%u..."), *ipport, port);
        else
            CCore::GetSingleton().GetConsole()->Printf(_("Discord Join: could not connect to %s:%u!"), *ipport, port);
    }
    else
        CCore::GetSingleton().GetConsole()->Print(_("Discord Join: Failed to unload current mod"));
}

void* CDiscordManager::DiscordThread(void* arg)
{
    CThreadHandle::AllowASyncCancel();
    CDiscordManager* that = reinterpret_cast<CDiscordManager*>(arg);

    while (true)
    {
        if (that->NeedsSuicide())
            break;

        that->DoPulse();

        Sleep(15);
    }

    that->SetDead();

    return nullptr;
}

// Called from separate thread so the unnecessary load won't affect the main thread,
// establishing connection with discord is sometimes time-consuming, especially when it's not running
void CDiscordManager::DoPulse()
{
    m_ThreadSafety.lock();
    if (!m_Connected)
    {
        m_ThreadSafety.unlock();

        // Discord is not initialized, maybe it's not installed or not yet running
        // So every 15sec we will check if the player got discord running
        if (m_TimeForReconnection.Get() >= 15000)
        {
            Reconnect();
            m_TimeForReconnection.Reset();
        }

        return;
    }
    m_ThreadSafety.unlock();

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);

    if (m_WaitingForServerName)            // Query request sent
    {
        auto info = m_QueryReceiver.GetServerResponse();
        if (info.containingInfo)
        {
            m_StoredActivity.m_details = info.serverName;
            UpdateActivity();
            m_WaitingForServerName = false;
        }
        else if (m_QueryReceiver.GetElapsedTimeSinceLastQuery() > 2000)            // Resend query request every 2s if no response came
        {
            SString      ipaddr;
            unsigned int port;
            CVARS_GET("host", ipaddr);
            CVARS_GET("port", port);
            m_QueryReceiver.RequestQuery(ipaddr, static_cast<ushort>(port + SERVER_LIST_QUERY_PORT_OFFSET));
        }
    }

    Discord_UpdateConnection();
    Discord_RunCallbacks();
}

void CDiscordManager::UpdateActivity(SDiscordActivity& activity)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);

    m_StoredActivity = activity;

    UpdateActivity();
}

void CDiscordManager::UpdateActivity()
{
    if (!m_Connected)
        return;

    DiscordRichPresence discordPresence{};
    discordPresence.state = m_StoredActivity.m_state;
    discordPresence.details = m_StoredActivity.m_details;
    discordPresence.startTimestamp = m_StoredActivity.m_startTimestamp;
    discordPresence.largeImageKey = "mta_logo_round";
    discordPresence.largeImageText = "Playing MTA:SA";
    discordPresence.smallImageKey = m_StoredActivity.m_smallImage;
    discordPresence.smallImageText = m_StoredActivity.m_smallText;
    discordPresence.partyId = m_StoredActivity.m_partyID;
    discordPresence.partySize = m_StoredActivity.m_partySize;
    discordPresence.partyMax = m_StoredActivity.m_partyMax;
    discordPresence.joinSecret = m_StoredActivity.m_joinSecret;
    Discord_UpdatePresence(&discordPresence);
}

void CDiscordManager::SetType(EDiscordActivityT type)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_activityType = type;
    UpdateActivity();
}

void CDiscordManager::SetName(char const* name)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_name = name;
    UpdateActivity();
}

void CDiscordManager::SetState(char const* state)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_state = state;
    UpdateActivity();
}

void CDiscordManager::SetDetails(char const* details)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_details = details;
    UpdateActivity();
}

void CDiscordManager::SetStartEndTimestamp(int64 start, int64 end)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_startTimestamp = start;
    m_StoredActivity.m_endTimestamp = end;
    UpdateActivity();
}

void CDiscordManager::SetJoinParameters(const char* joinSecret, const char* partyId, uint partySize, uint partyMax)
{
    if (!joinSecret || !partyId)
        return;

    if (!strlen(joinSecret) || !strlen(partyId))
    {
        // Delete join state
        m_StoredActivity.m_joinSecret = "";
        m_StoredActivity.m_partyID = "";
        m_StoredActivity.m_partySize = 0;
        m_StoredActivity.m_partyMax = 0;
        return;
    }

    SString      ipaddr;
    unsigned int port;
    CVARS_GET("host", ipaddr);
    CVARS_GET("port", port);
    SString strRealJoinSecret("%s:%i-%s", *ipaddr, port, joinSecret);

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_joinSecret = strRealJoinSecret;
    m_StoredActivity.m_partyID = partyId;
    m_StoredActivity.m_partySize = partySize;
    m_StoredActivity.m_partyMax = partyMax;
    UpdateActivity();
}

void CDiscordManager::SetSpectateSecret(const char* spectateSecret)
{
    if (!spectateSecret)
        return;

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_spectateSecret = spectateSecret;
    UpdateActivity();
}

// Default rich presence settings
void CDiscordManager::RegisterPlay(bool connected)
{
    if (!connected)
    {
        m_StoredActivity.m_smallText = "";
        m_StoredActivity.m_smallImage = "";
        m_StoredActivity.m_joinSecret = "";
        m_StoredActivity.m_partyID = "";
        m_StoredActivity.m_partySize = 0;
        m_StoredActivity.m_partyMax = 0;
        UpdateActivity();
        return;
    }

    SString      ipaddr;
    unsigned int port;
    CVARS_GET("host", ipaddr);
    CVARS_GET("port", port);

    time_t currentTime;
    time(&currentTime);

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.m_details = "Retrieving server name...";
    m_StoredActivity.m_smallText = SString("Connected to %s:%i", *ipaddr, port);

    // TODO: Maybe contact with MTA:SA servers and check if this ip is a premium one
    // containing a small image to set using this function
    m_StoredActivity.m_smallImage = "a-server";

    m_StoredActivity.m_startTimestamp = currentTime;
    UpdateActivity();

    m_QueryReceiver.RequestQuery(ipaddr, static_cast<ushort>(port + SERVER_LIST_QUERY_PORT_OFFSET));
    m_WaitingForServerName = true;
}

void CDiscordManager::Restore()
{
    SString      ipaddr;
    unsigned int port;
    CVARS_GET("host", ipaddr);
    CVARS_GET("port", port);
    m_QueryReceiver.RequestQuery(ipaddr, static_cast<ushort>(port + SERVER_LIST_QUERY_PORT_OFFSET));
    m_WaitingForServerName = true;
}

void CDiscordManager::Disconnect()
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    Discord_Shutdown();
    m_Connected = false;
}

SString CDiscordManager::GetJoinSecret()
{
    return CCore::GetSingleton().GetConnectManager()->GetJoinSecret();
}

void CDiscordManager::HandleDiscordReady(const DiscordUser* connectedUser)
{
    WriteDebugEvent(SString("[DISCORD]: Connected to user %s#%s - %s", connectedUser->username, connectedUser->discriminator, connectedUser->userId));
    reinterpret_cast<CDiscordManager*>(g_pCore->GetDiscordManager())->m_Connected = true;
    reinterpret_cast<CDiscordManager*>(g_pCore->GetDiscordManager())->UpdateActivity();
}

void CDiscordManager::HandleDiscordDisconnected(int errcode, const char* message)
{
    WriteDebugEvent(SString("[DISCORD]: Disconnected (Code #%i, message: %s)", errcode, message));
    reinterpret_cast<CDiscordManager*>(g_pCore->GetDiscordManager())->m_Connected = false;
}

void CDiscordManager::HandleDiscordSpectate(const char* secret)
{
    // TODO?
}

void CDiscordManager::HandleDiscordJoinRequest(const DiscordUser* request)
{
    // This needs a decision on how to make this
    Discord_Respond(request->userId, DISCORD_REPLY_YES);
}
