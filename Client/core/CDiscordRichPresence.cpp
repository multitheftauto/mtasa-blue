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
    m_strDiscordAppId = DEFAULT_APP_ID;
    m_strDiscordAppAsset = DEFAULT_APP_ASSET;
    m_strDiscordAppAssetText = DEFAULT_APP_ASSET_TEXT;

    m_strDiscordAppAssetSmall = DEFAULT_APP_ASSET_SMALL;
    m_strDiscordAppAssetSmallText = DEFAULT_APP_ASSET_SMALL_TEXT;

    m_strDiscordAppCurrentId = DEFAULT_APP_ID;
    m_strDiscordAppState = "";
    m_strDiscordAppDetails = "";
    m_uiDiscordAppStart = 0;            // timestamp time to elapsed time
    m_uiDiscordAppEnd = 0;              // timestamp time to remaining time
}

CDiscordRichPresence::~CDiscordRichPresence()
{
    if (m_bDiscordRPCEnabled)
        Discord_Shutdown();
}

void CDiscordRichPresence::UpdatePresence()
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.largeImageKey = m_strDiscordAppAsset.c_str();
    discordPresence.largeImageText = m_strDiscordAppAssetText.c_str();
    discordPresence.state = m_strDiscordAppState.c_str();
    discordPresence.details = m_strDiscordAppDetails.c_str();
    discordPresence.startTimestamp = m_uiDiscordAppStart;

    Discord_UpdatePresence(&discordPresence);
}

void CDiscordRichPresence::SetPresenceState(const char* szState)
{
    m_strDiscordAppState = szState;
}

bool CDiscordRichPresence::SetPresenceDetails(const char* szDetails, bool bCustom)
{
    if (bCustom && !m_bAllowCustomDetails)
        return false;

    m_strDiscordAppDetails = szDetails;
    return true;
}

void CDiscordRichPresence::SetPresenceStartTimestamp(const unsigned long ulStart)
{
    m_uiDiscordAppStart = ulStart;
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

bool CDiscordRichPresence::SetApplicationID(const char* szAppID)
{
    m_strDiscordAppCurrentId = (szAppID && szAppID[0] != '\0') ? szAppID : DEFAULT_APP_ID;

    if (m_bDiscordRPCEnabled)
    {
        ShutdownDiscord();
        InitializeDiscord();
    }
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
