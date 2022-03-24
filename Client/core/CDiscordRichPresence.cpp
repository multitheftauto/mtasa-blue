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
    m_strDiscordAppStart = 0;
    m_strDiscordAppEnd = 0;

    Discord_Initialize(m_strDiscordAppId, &handlers, 1, nullptr);
}

CDiscordRichPresence::~CDiscordRichPresence()
{
    m_strDiscordAppId = DEFAULT_APP_ID;
    Discord_Shutdown();
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
}
