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

constexpr char DEFAULT_APP_ID[] = "1139666477813866546";
constexpr char DEFAULT_APP_ASSET[] = "mta_logo_round";
constexpr char DEFAULT_APP_ASSET_TEXT[] = "Multi Theft Auto";
constexpr char DEFAULT_APP_ASSET_SMALL[] = "";
constexpr char DEFAULT_APP_ASSET_SMALL_TEXT[] = "";

CDiscordRichPresence::CDiscordRichPresence() : m_uiDiscordAppStart(0), m_uiDiscordAppEnd(0)
{
    SetDefaultData();

    m_strDiscordAppState.clear();
}

CDiscordRichPresence::~CDiscordRichPresence()
{
    if (m_bDiscordRPCEnabled)
        Discord_Shutdown();
}

void CDiscordRichPresence::InitializeDiscord()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));

    // Handlers .ready .disconnected .errored maybe use in future?
    Discord_Initialize((m_strDiscordAppCurrentId.empty()) ? DEFAULT_APP_ID : m_strDiscordAppCurrentId.c_str(), &handlers, 1, nullptr);
}

void CDiscordRichPresence::ShutdownDiscord()
{
    Discord_Shutdown();
}

void CDiscordRichPresence::RestartDiscord()
{
    Discord_Shutdown();
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
    m_strDiscordAppDetails.clear();
    m_strDiscordAppCustomState.clear();
}

void CDiscordRichPresence::UpdatePresence()
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));

    discordPresence.largeImageKey = m_strDiscordAppAsset.c_str();
    discordPresence.largeImageText = m_strDiscordAppAssetText.c_str();
    discordPresence.smallImageKey = m_strDiscordAppAssetSmall.c_str();
    discordPresence.smallImageText = m_strDiscordAppAssetSmallText.c_str();

    const char* state = (!m_strDiscordAppCustomState.empty() && m_bAllowCustomDetails) ? m_strDiscordAppCustomState.c_str() : m_strDiscordAppState.c_str();
    discordPresence.state = state;

    discordPresence.details = m_strDiscordAppDetails.c_str();
    discordPresence.startTimestamp = m_uiDiscordAppStart;

    Discord_UpdatePresence(&discordPresence);
}

void CDiscordRichPresence::SetPresenceStartTimestamp(const unsigned long ulStart)
{
    m_uiDiscordAppStart = ulStart;
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
        m_strDiscordAppAsset = (szAsset && *szAsset) ? szAsset : DEFAULT_APP_ASSET;
        m_strDiscordAppAssetText = (szAssetText && *szAssetText) ? szAssetText : DEFAULT_APP_ASSET_TEXT;
    }
    else
    {
        m_strDiscordAppAssetSmall = (szAsset && *szAsset) ? szAsset : DEFAULT_APP_ASSET_SMALL;
        m_strDiscordAppAssetSmallText = (szAssetText && *szAssetText) ? szAssetText : DEFAULT_APP_ASSET_SMALL_TEXT;
    }
}

bool CDiscordRichPresence::SetPresenceState(const char* szState, bool bCustom)
{
    if (bCustom && !m_bAllowCustomDetails)
        m_strDiscordAppCustomState = szState;
    else
        m_strDiscordAppState = szState;

    return true;
}

bool CDiscordRichPresence::SetPresenceDetails(const char* szDetails, bool bCustom)
{
    if (bCustom && !m_bAllowCustomDetails)
        return false;

    m_strDiscordAppDetails = szDetails;
    return true;
}

bool CDiscordRichPresence::ResetDiscordData()
{
    SetDefaultData();

    if (m_bDiscordRPCEnabled)
    {
        RestartDiscord();
        UpdatePresence();
    }
    return true;
}

bool CDiscordRichPresence::SetApplicationID(const char* szAppID)
    {
    m_strDiscordAppCurrentId = (szAppID && *szAppID) ? szAppID : DEFAULT_APP_ID;

    if (m_bDiscordRPCEnabled)
    {
        RestartDiscord();
    }
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
    UpdatePresence();
    return true;
}

bool CDiscordRichPresence::IsDiscordRPCEnabled()
{
    return m_bDiscordRPCEnabled;
}
