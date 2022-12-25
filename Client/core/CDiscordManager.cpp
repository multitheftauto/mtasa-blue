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

CDiscordManager::~CDiscordManager()
{
}

void CDiscordManager::Initialize()
{
}

// Establish connection with discord
void CDiscordManager::Reconnect(bool bOnInitialization)
{
}

void CDiscordManager::DiscordLogCallback(discord::LogLevel level, const char* message)
{
}

void CDiscordManager::OnActivityJoin(const char* joinSecret)
{
}

void* CDiscordManager::DiscordThread(void* arg)
{
    return nullptr;
}

// Called from separate thread so the unnecessary load won't affect the main thread,
// establishing connection with discord is sometimes time-consuming, especially when it's not running
void CDiscordManager::DoPulse()
{
}

void CDiscordManager::UpdateActivity(SDiscordActivity& activity, std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::UpdateActivity(std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::SetType(EDiscordActivityT type, std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::SetName(char const* name, std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::SetState(char const* state, std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::SetDetails(char const* details, std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::SetStartEndTimestamp(int64 start, int64 end, std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::SetJoinParameters(const char* joinSecret, const char* partyId, uint partySize, uint partyMax, std::function<void(EDiscordRes)> callback)
{
}

void CDiscordManager::SetSpectateSecret(const char* spectateSecret, std::function<void(EDiscordRes)> callback)
{
}

// Default rich presence settings
void CDiscordManager::RegisterPlay(bool connected)
{
}

void CDiscordManager::Restore()
{
}

void CDiscordManager::Disconnect()
{
}

SString CDiscordManager::GetJoinSecret()
{
    return CCore::GetSingleton().GetConnectManager()->GetJoinSecret();
}
