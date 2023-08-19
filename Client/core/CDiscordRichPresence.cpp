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

#define DEFAULT_APP_ID               "1139666477813866546"
#define DEFAULT_APP_ASSET            "mta_logo_round"
#define DEFAULT_APP_ASSET_TEXT       "Multi Theft Auto"
#define DEFAULT_APP_ASSET_SMALL      ""
#define DEFAULT_APP_ASSET_SMALL_TEXT ""

CDiscordRichPresence::CDiscordRichPresence()
{
    SetDefaultData();

    m_strDiscordAppState = "";
    m_uiDiscordAppStart = 0;            // timestamp time to elapsed time
    m_uiDiscordAppEnd = 0;              // timestamp time to remaining time
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

    Discord_Initialize((m_strDiscordAppCurrentId != m_strDiscordAppId) ? m_strDiscordAppCurrentId.c_str() : m_strDiscordAppId.c_str(), &handlers, 1, nullptr);
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
    m_strDiscordAppDetails = "";
    m_strDiscordAppCustomState = "";
}

void CDiscordRichPresence::UpdatePresence()
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.largeImageKey = m_strDiscordAppAsset.c_str();
    discordPresence.largeImageText = m_strDiscordAppAssetText.c_str();
    discordPresence.smallImageKey = m_strDiscordAppAssetSmall.c_str();
    discordPresence.smallImageText = m_strDiscordAppAssetSmallText.c_str();
    discordPresence.state = (m_strDiscordAppCustomState[0] != '\0') ? m_strDiscordAppCustomState.c_str() : m_strDiscordAppState.c_str();
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
    m_strDiscordAppAsset = (szAsset && szAsset[0] != '\0') ? szAsset : DEFAULT_APP_ID;
    m_strDiscordAppAssetText = (szAssetText && szAssetText[0] != '\0') ? szAssetText : DEFAULT_APP_ASSET_TEXT;
}

void CDiscordRichPresence::SetAssetSmallData(const char* szAsset, const char* szAssetText)
{
    m_strDiscordAppAssetSmall = (szAsset && szAsset[0] != '\0') ? szAsset : DEFAULT_APP_ASSET_SMALL;
    m_strDiscordAppAssetSmallText = (szAssetText && szAssetText[0] != '\0') ? szAssetText : DEFAULT_APP_ASSET_SMALL_TEXT;
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
    m_strDiscordAppCurrentId = (szAppID && szAppID[0] != '\0') ? szAppID : DEFAULT_APP_ID;

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
