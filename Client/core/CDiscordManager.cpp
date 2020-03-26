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

#ifndef DISCORD_CLIENT_ID
#define DISCORD_CLIENT_ID 468493322583801867
#endif

CDiscordManager::CDiscordManager() : m_DiscordCore(nullptr), m_Suicide(false), m_WaitingForServerName(false), m_StoredActivity{}
{
    Reconnect(true);            // Try to interact with discord on construction
    m_Thread = new CThreadHandle(CDiscordManager::DiscordThread, this);

    m_StoredActivity.GetAssets().SetLargeImage("mta_logo_round");            // Always thing
    m_StoredActivity.GetAssets().SetLargeText("Playing MTA:SA");
}

CDiscordManager::~CDiscordManager()
{
    m_Suicide = true;
    int iTries = 0;
    while (iTries++ < 400 && m_Suicide)            // Wait maximum of 2 sec on this
        Sleep(5);

    if (m_Suicide)
        m_Thread->Cancel();            // Kill it anyway

    delete m_Thread;
    SAFE_DELETE(m_DiscordCore);
}

// Establish connection with discord
void CDiscordManager::Reconnect(bool bOnInitialization)
{
    bool discordRichPresence;
    CVARS_GET("discord_rich_presence", discordRichPresence);
    if (!discordRichPresence)
        return;            // Disabled

    discord::Result res;
    {
        std::lock_guard<std::mutex> guardian(m_ThreadSafety);
        res = discord::Core::Create(DISCORD_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &m_DiscordCore);
    }

    if (!m_DiscordCore)
    {
        // Output error only when trying to connect on initialization
        if (bOnInitialization)
            WriteErrorEvent(SString("[DISCORD]: Failed to instantiate core, error code: %i", static_cast<int>(res)));
        return;
    }

    m_DiscordCore->SetLogHook(discord::LogLevel::Info, DiscordLogCallback);
    WriteDebugEvent(SString("[DISCORD]: Instantiated at %08X", m_DiscordCore));

    m_DiscordCore->ActivityManager().RegisterCommand(SString("%s /from_discord", *GetParentProcessPathFilename(GetCurrentProcessId())));
    m_DiscordCore->ActivityManager().OnActivityJoin.Connect(OnActivityJoin);

    if (!bOnInitialization)            // Player could be in a server or in menu by now
    {
        if (g_pCore->IsConnected())
            Restore();
        else
            g_pCore->ResetDiscordRichPresence();
    }
}

void CDiscordManager::DiscordLogCallback(discord::LogLevel level, const char* message)
{
    SString strMessage("[DISCORD]: %s", message);
    if (level >= discord::LogLevel::Warn)
        WriteDebugEvent(strMessage);
    else
        WriteErrorEvent(strMessage);
}

void CDiscordManager::OnActivityJoin(const char* joinSecret)
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
    if (!m_DiscordCore)
    {
        // Discord is not initialized, maybe it's not installed or not yet running
        // So every 15sec we will check if the player got discord running
        if (m_TimeForReconnection.Get() >= 15000)
        {
            Reconnect();
            m_TimeForReconnection.Reset();
        }

        return;
    }

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);

    if (m_WaitingForServerName)            // Query request sent
    {
        auto info = m_QueryReceiver.GetServerResponse();
        if (info.containingInfo)
        {
            m_StoredActivity.SetDetails(info.serverName);
            UpdateActivity([](EDiscordRes) {});
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

    discord::Result res = m_DiscordCore->RunCallbacks();
    if (res == discord::Result::NotRunning)            // Discord is now closed, needs to be reinstated in the next 15s
    {
        delete m_DiscordCore;
        m_DiscordCore = nullptr;
        m_TimeForReconnection.Reset();
        WriteDebugEvent("[DISCORD]: Lost the connection.");
    }
}

void CDiscordManager::UpdateActivity(SDiscordActivity& activity, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);

    m_StoredActivity.GetTimestamps().SetStart(activity.m_startTimestamp);
    m_StoredActivity.GetTimestamps().SetEnd(activity.m_endTimestamp);
    m_StoredActivity.SetName(activity.m_name);
    m_StoredActivity.SetState(activity.m_state);
    m_StoredActivity.SetDetails(activity.m_details);
    m_StoredActivity.SetType(static_cast<discord::ActivityType>(activity.m_activityType));

    UpdateActivity(callback);
}

void CDiscordManager::UpdateActivity(std::function<void(EDiscordRes)> callback)
{
    if (!m_DiscordCore)
        return;

    m_DiscordCore->ActivityManager().UpdateActivity(m_StoredActivity, [=](discord::Result res) { callback(static_cast<EDiscordRes>(res)); });
}

void CDiscordManager::SetType(EDiscordActivityT type, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.SetType(static_cast<discord::ActivityType>(type));
    UpdateActivity(callback);
}

void CDiscordManager::SetName(char const* name, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.SetName(name);
    UpdateActivity(callback);
}

void CDiscordManager::SetState(char const* state, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.SetState(state);
    UpdateActivity(callback);
}

void CDiscordManager::SetDetails(char const* details, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.SetDetails(details);
    UpdateActivity(callback);
}

void CDiscordManager::SetStartEndTimestamp(int64 start, int64 end, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.GetTimestamps().SetStart(start);
    m_StoredActivity.GetTimestamps().SetEnd(end);
    UpdateActivity(callback);
}

void CDiscordManager::SetJoinParameters(const char* joinSecret, const char* partyId, uint partySize, uint partyMax, std::function<void(EDiscordRes)> callback)
{
    if (!joinSecret || !partyId)
        return;

    SString      ipaddr;
    unsigned int port;
    CVARS_GET("host", ipaddr);
    CVARS_GET("port", port);
    SString strRealJoinSecret("%s:%i-%s", *ipaddr, port, joinSecret);

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.GetSecrets().SetJoin(strRealJoinSecret);
    m_StoredActivity.GetParty().SetId(partyId);
    m_StoredActivity.GetParty().GetSize().SetCurrentSize(partySize);
    m_StoredActivity.GetParty().GetSize().SetMaxSize(partyMax);
    UpdateActivity(callback);
}

void CDiscordManager::SetSpectateSecret(const char* spectateSecret, std::function<void(EDiscordRes)> callback)
{
    if (!spectateSecret)
        return;

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.GetSecrets().SetSpectate(spectateSecret);
    UpdateActivity(callback);
}

// Default rich presence settings
void CDiscordManager::RegisterPlay(bool connected)
{
    if (!connected)
    {
        m_StoredActivity.GetAssets().SetSmallText("");
        m_StoredActivity.GetAssets().SetSmallImage("");
        UpdateActivity([=](EDiscordRes res) {});
        return;
    }

    SString      ipaddr;
    unsigned int port;
    CVARS_GET("host", ipaddr);
    CVARS_GET("port", port);

    time_t currentTime;
    time(&currentTime);

    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_StoredActivity.SetDetails("Retrieving server name...");
    m_StoredActivity.GetAssets().SetSmallText(SString("Connected to %s:%i", *ipaddr, port));

    // TODO: Maybe contact with MTA:SA servers and check if this ip is a premium one
    // containing a small image to set using this function
    m_StoredActivity.GetAssets().SetSmallImage("a-server");

    m_StoredActivity.GetTimestamps().SetStart(currentTime);
    UpdateActivity([=](EDiscordRes res) {
        if (res != DiscordRes_Ok)
            WriteErrorEvent("[DISCORD]: Unable to register play rich presence.");
        else
        {
            m_QueryReceiver.RequestQuery(ipaddr, static_cast<ushort>(port + SERVER_LIST_QUERY_PORT_OFFSET));

            m_WaitingForServerName = true;
        }
    });
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
    SAFE_DELETE(m_DiscordCore);
}

void CDiscordManager::DisconnectNotification()
{
    std::lock_guard<std::mutex> guardian(m_ThreadSafety);
    m_WaitingForServerName = false;            // No longer wait
    m_QueryReceiver.InvalidateSocket();
}

SString CDiscordManager::GetJoinSecret()
{
    return CCore::GetSingleton().GetConnectManager()->GetJoinSecret();
}
