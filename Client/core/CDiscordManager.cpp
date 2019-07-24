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
#include <chrono>

CDiscordManager::CDiscordManager() : m_discordCore(nullptr), m_bSuicide(false), m_waitingForServerName(false), m_storedActivity{}
{
    Reconnect(true); // Try to interact with discord on construction
    m_pThread = new CThreadHandle(CDiscordManager::DiscordThread, this);

    m_storedActivity.GetAssets().SetLargeImage("multi-theft-auto"); // Always thing
    m_storedActivity.GetAssets().SetLargeText("Playing MTA:SA");
}

CDiscordManager::~CDiscordManager()
{
    m_bSuicide = true;
    int iTries = 0;
    while (iTries++ < 400 && m_bSuicide) // Wait maximum of 2 sec on this
        Sleep(5);
    if (m_bSuicide) m_pThread->Cancel(); // Kill it anyway
    delete m_pThread;
    SAFE_DELETE(m_discordCore);
}

// Establish connection with discord
void CDiscordManager::Reconnect(bool bOnInitialization)
{
    bool discordRichPresence;
    CVARS_GET("discord_rich_presence", discordRichPresence);
    if (!discordRichPresence) return; // Disabled

    m_threadSafety.lock();
    discord::Result res = discord::Core::Create(DISCORD_CLIENT_ID, DiscordCreateFlags_NoRequireDiscord, &m_discordCore);
    m_threadSafety.unlock();

    if (!m_discordCore && bOnInitialization) // Output error only when trying to connect on initialization
        WriteErrorEvent(SString("[DISCORD]: Failed to instantiate core, error code: %i", static_cast<int>(res)));
    else
    {
        m_discordCore->SetLogHook(discord::LogLevel::Info, DiscordLogCallback);
        WriteDebugEvent(SString("[DISCORD]: Instantiated at %08X", m_discordCore));

        m_discordCore->ActivityManager().RegisterCommand(SString("%s /from_discord", *GetParentProcessPathFilename(GetCurrentProcessId())));
        m_discordCore->ActivityManager().OnActivityJoin.Connect(OnActivityJoin);

        if (!bOnInitialization) // Player could be in a server or in menu by now
        {
            if (g_pCore->IsConnected()) Restore();
            else g_pCore->ResetDiscordRichPresence();
        }
    }
}

void CDiscordManager::DiscordLogCallback(discord::LogLevel level, const char* message)
{
    SString strMessage("[DISCORD]: %s", message);
    if (level >= discord::LogLevel::Warn) WriteDebugEvent(strMessage);
    else WriteErrorEvent(strMessage);
}

void CDiscordManager::OnActivityJoin(const char* joinSecret)
{
    if (!joinSecret)
        return;

    // Parse it
    SString ipport, secret;
    ushort  port;
    secret = SStringX(joinSecret).SplitRight("-", &ipport);

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
        {
            CCore::GetSingleton().GetConsole()->Printf(_("Discord Join: Connecting to %s:%u..."), *ipport, port);
        }
        else
        {
            CCore::GetSingleton().GetConsole()->Printf(_("Discord Join: could not connect to %s:%u!"), *ipport, port);
        }
    }
    else
    {
        CCore::GetSingleton().GetConsole()->Print(_("Discord Join: Failed to unload current mod"));
    }
}

void* CDiscordManager::DiscordThread(void* arg)
{
    CThreadHandle::AllowASyncCancel();
    CDiscordManager* that = reinterpret_cast<CDiscordManager*>(arg);

    while (true)
    {
        if (that->NeedsSuicide()) break;

        that->DoPulse();

        Sleep(15);
    }

    that->SetDead();

    return NULL;
}

// Called from separate thread so the unnecessary load won't affect the main thread, establishing connection with discord is sometimes time-consuming, especially when it's not running
void CDiscordManager::DoPulse()
{
    if (m_discordCore)
    {
        std::lock_guard<std::mutex> guardian(m_threadSafety);

        if (m_waitingForServerName) // Query request sent
        {
            auto info = queryReceiver.GetServerResponse();
            if (!info.containingInfo)
            {
                if (queryReceiver.GetElapsedTimeSinceLastQuery() > 2000) // Resend query request every 2s if no response came
                {
                    SString      ipaddr;
                    unsigned int port;
                    CVARS_GET("host", ipaddr);
                    CVARS_GET("port", port);
                    queryReceiver.RequestQuery(ipaddr, static_cast<ushort>(port + SERVER_LIST_QUERY_PORT_OFFSET));
                }
            }
            else
            {
                m_storedActivity.SetDetails(info.serverName);
                UpdateActivity([](EDiscordRes) {});
                m_waitingForServerName = false;
            }
        }

        discord::Result res = m_discordCore->RunCallbacks();
        if (res == discord::Result::NotRunning) // Discord is now closed, needs to be reinstated in the next 15s
        {
            delete m_discordCore;
            m_discordCore = nullptr;
            m_timeForReconnection.Reset();
            WriteDebugEvent("[DISCORD]: Lost the connection.");
        }
    }
    else
    {
        // Discord is not initialized, maybe it's not installed or not yet running
        // So every 15sec we will check if the player got discord running
        if (m_timeForReconnection.Get() >= 15000)
        {
            Reconnect();
            m_timeForReconnection.Reset();
        }
    }
}

void CDiscordManager::UpdateActivity(SDiscordActivity& activity, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);

    m_storedActivity.GetTimestamps().SetStart(activity.m_startTimestamp);
    m_storedActivity.GetTimestamps().SetEnd(activity.m_endTimestamp);
    m_storedActivity.SetName(activity.m_name);
    m_storedActivity.SetState(activity.m_state);
    m_storedActivity.SetDetails(activity.m_details);
    m_storedActivity.SetType(static_cast<discord::ActivityType>(activity.m_activityType));

    UpdateActivity(callback);
}

void CDiscordManager::UpdateActivity(std::function<void(EDiscordRes)> callback)
{
    if (!m_discordCore) return;

    m_discordCore->ActivityManager().UpdateActivity(m_storedActivity, [=](discord::Result res) {
        callback(static_cast<EDiscordRes>(res));
    });
}

void CDiscordManager::SetType(EDiscordActivityT type, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_storedActivity.SetType(static_cast<discord::ActivityType>(type));
    UpdateActivity(callback);
}

void CDiscordManager::SetName(char const* name, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_storedActivity.SetName(name);
    UpdateActivity(callback);
}

void CDiscordManager::SetState(char const* state, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_storedActivity.SetState(state);
    UpdateActivity(callback);
}

void CDiscordManager::SetDetails(char const* details, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_storedActivity.SetDetails(details);
    UpdateActivity(callback);
}

void CDiscordManager::SetStartEndTimestamp(int64 start, int64 end, std::function<void(EDiscordRes)> callback)
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_storedActivity.GetTimestamps().SetStart(start);
    m_storedActivity.GetTimestamps().SetEnd(end);
    UpdateActivity(callback);
}

void CDiscordManager::SetJoinParameters(const char* joinSecret, const char* partyId, uint partySize, uint partyMax, std::function<void(EDiscordRes)> callback)
{
    if (!joinSecret || !partyId) return;

    SString      ipaddr;
    unsigned int port;
    CVARS_GET("host", ipaddr);
    CVARS_GET("port", port);
    SString strRealJoinSecret("%s:%i-%s", *ipaddr, port, joinSecret);

    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_storedActivity.GetSecrets().SetJoin(strRealJoinSecret);
    m_storedActivity.GetParty().SetId(partyId);
    m_storedActivity.GetParty().GetSize().SetCurrentSize(partySize);
    m_storedActivity.GetParty().GetSize().SetMaxSize(partyMax);
    UpdateActivity(callback);
}

void CDiscordManager::SetSpectateSecret(const char* spectateSecret, std::function<void(EDiscordRes)> callback)
{
    if (!spectateSecret) return;

    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_storedActivity.GetSecrets().SetSpectate(spectateSecret);
    UpdateActivity(callback);
}

// Default rich presence settings
void CDiscordManager::RegisterPlay(bool connect)
{
    if (connect)
    {
        SString      ipaddr;
        unsigned int port;
        CVARS_GET("host", ipaddr);
        CVARS_GET("port", port);

        std::lock_guard<std::mutex> guardian(m_threadSafety);
        m_storedActivity.SetDetails("Retrieving server name...");
        m_storedActivity.GetAssets().SetSmallText(SString("Connected to %s:%i", *ipaddr, port));
        m_storedActivity.GetAssets().SetSmallImage("a-server"); // TODO: Maybe contact with MTA:SA servers and check if this ip is a premium one containing a small image to set using this function
        m_storedActivity.GetTimestamps().SetStart(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        UpdateActivity([=](EDiscordRes res) {
            if (res != DiscordRes_Ok) WriteErrorEvent("[DISCORD]: Unable to register play rich presence.");
            else
            {
                queryReceiver.RequestQuery(ipaddr, static_cast<ushort>(port + SERVER_LIST_QUERY_PORT_OFFSET));

                m_waitingForServerName = true;
            }
        });
    }
    else
    {
        m_storedActivity.GetAssets().SetSmallText("");
        m_storedActivity.GetAssets().SetSmallImage("");
        UpdateActivity([=](EDiscordRes res) {});
    }
}

void CDiscordManager::Restore()
{
    SString      ipaddr;
    unsigned int port;
    CVARS_GET("host", ipaddr);
    CVARS_GET("port", port);
    queryReceiver.RequestQuery(ipaddr, static_cast<ushort>(port + SERVER_LIST_QUERY_PORT_OFFSET));
    m_waitingForServerName = true;
}

void CDiscordManager::Disconnect()
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);
    SAFE_DELETE(m_discordCore);
}

void CDiscordManager::DisconnectNotification()
{
    std::lock_guard<std::mutex> guardian(m_threadSafety);
    m_waitingForServerName = false; // No longer wait
    queryReceiver.InvalidateSocket();
}
