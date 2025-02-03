/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CDiscordRichPresence.cpp
 *  PURPOSE:     Discord rich presence implementation
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "discord_rpc.h"
#include "CDiscordRichPresence.h"

constexpr char DEFAULT_APP_ID[] = "468493322583801867";
constexpr char DEFAULT_APP_ASSET[] = "mta_logo_round";
constexpr char DEFAULT_APP_ASSET_TEXT[] = "Multi Theft Auto";
constexpr char DEFAULT_APP_ASSET_SMALL[] = "";
constexpr char DEFAULT_APP_ASSET_SMALL_TEXT[] = "";

CDiscordRichPresence::CDiscordRichPresence() : m_uiDiscordAppStart(0), m_uiDiscordAppEnd(0)
{
    SetDefaultData();

    m_strDiscordAppState.clear();
    m_strDiscordCustomResourceName.clear();
    m_bConnected = false;
}

CDiscordRichPresence::~CDiscordRichPresence()
{
    if (m_bDiscordRPCEnabled)
        ShutdownDiscord();
}

void CDiscordRichPresence::InitializeDiscord()
{
    std::lock_guard<std::mutex> lock(m_threadSafetyMutex);
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));

    handlers.ready = HandleDiscordReady;
    handlers.errored = HandleDiscordError;
    handlers.disconnected = HandleDiscordDisconnected;

    Discord_Initialize((m_strDiscordAppCurrentId.empty()) ? DEFAULT_APP_ID : m_strDiscordAppCurrentId.c_str(), &handlers, 1, nullptr);

    m_bDisallowCustomDetails = (m_strDiscordAppCurrentId == DEFAULT_APP_ID) ? true : false;
    m_bConnected = true;
}

void CDiscordRichPresence::ShutdownDiscord()
{
    Discord_ClearPresence();
    Discord_Shutdown();

    m_bConnected = false;
}

void CDiscordRichPresence::RestartDiscord()
{
    ShutdownDiscord();
    InitializeDiscord();
}

void CDiscordRichPresence::SetDefaultData()
{
    m_strDiscordAppId = DEFAULT_APP_ID;
    m_strDiscordAppAsset = DEFAULT_APP_ASSET;
    m_strDiscordAppAssetText = DEFAULT_APP_ASSET_TEXT;

    m_strDiscordAppAssetSmall = DEFAULT_APP_ASSET_SMALL;
    m_strDiscordAppAssetSmallText = DEFAULT_APP_ASSET_SMALL_TEXT;

    m_strDiscordAppCurrentId = DEFAULT_APP_ID;
    m_strDiscordCustomResourceName.clear();
    m_strDiscordAppCustomDetails.clear();
    m_strDiscordAppCustomState.clear();

    m_aButtons = {};
    m_bUpdateRichPresence = true;
    m_bDisallowCustomDetails = true;

    m_uiDiscordAppStart = 0;
    m_uiDiscordAppEnd = 0;

    m_iPartySize = 0;
    m_iPartyMax = 0;

    m_iPlayersCount = 0;
    m_iMaxPlayers = 0;
}

void CDiscordRichPresence::UpdatePresence()
{
    if (!m_bDiscordRPCEnabled)
        return;

    // run callbacks
    Discord_RunCallbacks();

    if (!m_bConnected)
        return;

    if (!m_bUpdateRichPresence)
        return;

    std::lock_guard<std::mutex> lock(m_threadSafetyMutex);
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));

    discordPresence.largeImageKey = m_strDiscordAppAsset.c_str();
    discordPresence.largeImageText = m_strDiscordAppAssetText.c_str();
    discordPresence.smallImageKey = m_strDiscordAppAssetSmall.c_str();
    discordPresence.smallImageText = m_strDiscordAppAssetSmallText.c_str();

    discordPresence.state = (!m_strDiscordAppCustomState.empty() || !m_bDisallowCustomDetails) ? m_strDiscordAppCustomState.c_str() : m_strDiscordAppState.c_str();

    discordPresence.details =
        (!m_strDiscordAppCustomDetails.empty() || !m_bDisallowCustomDetails) ? m_strDiscordAppCustomDetails.c_str() : m_strDiscordAppDetails.c_str();
    discordPresence.startTimestamp = m_uiDiscordAppStart;
    discordPresence.endTimestamp = m_uiDiscordAppEnd;
    discordPresence.instance = 0;

    DiscordButton buttons[2]{0};
    if (m_aButtons)
    {
        buttons[0].label = std::get<0>(*m_aButtons).first.c_str();
        buttons[0].url = std::get<0>(*m_aButtons).second.c_str();
        buttons[1].label = std::get<1>(*m_aButtons).first.c_str();
        buttons[1].url = std::get<1>(*m_aButtons).second.c_str();

        discordPresence.buttons = buttons;
    }

    discordPresence.partySize = (m_bDisallowCustomDetails) ? m_iPlayersCount : m_iPartySize;
    discordPresence.partyMax = (m_bDisallowCustomDetails) ? m_iMaxPlayers : m_iPartyMax;

    Discord_UpdatePresence(&discordPresence);
    m_bUpdateRichPresence = false;
}

void CDiscordRichPresence::SetPresenceStartTimestamp(const unsigned long ulStart)
{
    m_uiDiscordAppStart = ulStart;
    m_bUpdateRichPresence = true;
}

void CDiscordRichPresence::SetPresenceEndTimestamp(const unsigned long ulEnd)
{
    m_uiDiscordAppEnd = ulEnd;
    m_bUpdateRichPresence = true;
}

void CDiscordRichPresence::SetAssetLargeData(const char* szAsset, const char* szAssetText)
{
    SetAsset(szAsset, szAssetText, true);
}

void CDiscordRichPresence::SetAssetSmallData(const char* szAsset, const char* szAssetText)
{
    SetAsset(szAsset, szAssetText, false);
}

void CDiscordRichPresence::SetAsset(const char* szAsset, const char* szAssetText, bool isLarge)
{
    if (isLarge)
    {
        m_strDiscordAppAsset = (std::strlen(szAsset) > 0 && szAsset && *szAsset) ? szAsset : DEFAULT_APP_ASSET;
        m_strDiscordAppAssetText = (std::strlen(szAssetText) > 0 && szAssetText && *szAssetText) ? szAssetText : DEFAULT_APP_ASSET_TEXT;
    }
    else
    {
        m_strDiscordAppAssetSmall = (std::strlen(szAsset) > 0 && szAsset && *szAsset) ? szAsset : DEFAULT_APP_ASSET_SMALL;
        m_strDiscordAppAssetSmallText = (std::strlen(szAssetText) > 0 && szAssetText && *szAssetText) ? szAssetText : DEFAULT_APP_ASSET_SMALL_TEXT;
    }
    m_bUpdateRichPresence = true;
}

bool CDiscordRichPresence::SetPresenceState(const char* szState, bool bCustom)
{
    if (bCustom)
        m_strDiscordAppCustomState = szState;
    else
        m_strDiscordAppState = szState;

    m_bUpdateRichPresence = true;
    return true;
}

bool CDiscordRichPresence::SetPresenceButtons(unsigned short int iIndex, const char* szName, const char* szUrl)
{
    // Should it always return true?
    if (iIndex <= 2)
    {
        std::decay_t<decltype(*m_aButtons)> buttons;
        if (m_aButtons)
            buttons = *m_aButtons;

        if (iIndex == 1)
            std::get<0>(buttons) = {szName, szUrl};
        else if (iIndex == 2)
            std::get<1>(buttons) = {szName, szUrl};

        m_aButtons = buttons;
        m_bUpdateRichPresence = true;

        return true;
    }

    return false;
}

bool CDiscordRichPresence::SetPresenceDetails(const char* szDetails, bool bCustom)
{
    if (bCustom)
        m_strDiscordAppCustomDetails = szDetails;
    else
        m_strDiscordAppDetails = szDetails;

    m_bUpdateRichPresence = true;
    return true;
}

bool CDiscordRichPresence::ResetDiscordData()
{
    SetDefaultData();

    if (m_bDiscordRPCEnabled)
    {
        RestartDiscord();
        m_bUpdateRichPresence = true;
    }
    return true;
}

bool CDiscordRichPresence::SetApplicationID(const char* szResourceName, const char* szAppID)
{
    m_strDiscordAppCurrentId = (szAppID && *szAppID) ? szAppID : DEFAULT_APP_ID;

    if (m_bDiscordRPCEnabled)
    {
        RestartDiscord();
        m_bUpdateRichPresence = true;
    }

    if (*szResourceName)
        m_strDiscordCustomResourceName = szResourceName;

    return true;
}

bool CDiscordRichPresence::SetDiscordRPCEnabled(bool bEnabled)
{
    m_bDiscordRPCEnabled = bEnabled;

    if (!bEnabled)
    {
        ShutdownDiscord();
        return true;
    }

    InitializeDiscord();
    m_bUpdateRichPresence = true;
    return true;
}

void CDiscordRichPresence::SetPresencePartySize(int iSize, int iMax, bool bCustom)
{
    if (bCustom)
    {
        m_iPartySize = iSize;
        m_iPartyMax = iMax;
    }
    else
    {
        m_iPlayersCount = iSize;
        m_iMaxPlayers = iMax;
    }
}

void CDiscordRichPresence::SetDiscordUserID(const std::string& strUserID)
{
    if (CVARS_GET_VALUE<bool>("discord_rpc_share_data"))
        m_strDiscordUserID = strUserID;
}

std::string CDiscordRichPresence::GetDiscordUserID() const
{
    if (CVARS_GET_VALUE<bool>("discord_rpc_share_data"))
        return m_strDiscordUserID;

    return {};
};

#ifdef DISCORD_DISABLE_IO_THREAD
void CDiscordRichPresence::UpdatePresenceConnection()
{
    Discord_UpdateConnection();
}
#endif

void CDiscordRichPresence::HandleDiscordReady(const DiscordUser* pDiscordUser)
{
    if (const auto discord = g_pCore->GetDiscord(); discord && discord->IsDiscordRPCEnabled())
    {
        discord->SetDiscordClientConnected(true);
        discord->SetDiscordUserID(pDiscordUser->userId);
    }
}

void CDiscordRichPresence::HandleDiscordDisconnected(int iErrorCode, const char* szMessage)
{
    WriteDebugEvent(SString("[DISCORD] Disconnected %s (error #%d)", szMessage, iErrorCode));

    if (const auto discord = g_pCore->GetDiscord(); discord)
    {
        discord->SetDiscordUserID("");
        discord->SetDiscordClientConnected(false);
    }
}

void CDiscordRichPresence::HandleDiscordError(int iErrorCode, const char* szMessage)
{
    WriteDebugEvent(SString("[DISCORD] Error: %s (error #%d)", szMessage, iErrorCode));
}
