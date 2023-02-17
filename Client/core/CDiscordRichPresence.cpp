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

#define DEFAULT_APP_ID "727214310610436126"
#define DEFAULT_APP_ASSET "mta_logo_round"
#define DEFAULT_APP_ASSET_SMALL ""
#define DEFAULT_APP_ASSET_TEXT ""
#define DEFAULT_APP_ASSET_SMALL_TEXT ""

CDiscordRichPresence::CDiscordRichPresence()
{
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));

    m_strDiscordAppId = DEFAULT_APP_ID;
    m_strDiscordAppAsset = DEFAULT_APP_ASSET;
    m_strDiscordAppAssetSmall = DEFAULT_APP_ASSET_SMALL;
    m_strDiscordAppAssetText = DEFAULT_APP_ASSET_TEXT;
    m_strDiscordAppAssetSmallText = DEFAULT_APP_ASSET_SMALL_TEXT;

    m_strDiscordAppCurrentId = DEFAULT_APP_ID;
    m_strDiscordAppState = "";
    m_strDiscordAppDetails = "";
    m_uiDiscordAppStart = 0;
    m_uiDiscordAppEnd = 0;

    Discord_Initialize(m_strDiscordAppId.c_str(), &handlers, 1, nullptr);
}

CDiscordRichPresence::~CDiscordRichPresence()
{
    Discord_Shutdown();
}

void CDiscordRichPresence::UpdatePresence()
{
    DiscordRichPresence discordPresence;
    memset(&discordPresence, 0, sizeof(discordPresence));
    discordPresence.largeImageKey = m_strDiscordAppAsset.c_str();
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
